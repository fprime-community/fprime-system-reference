// ======================================================================
// \title  XBee.cpp
// \author mstarch
// \brief  cpp file for XBee component implementation class
// ======================================================================

#include <SystemReference/Com/XBee/XBee.hpp>
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/BasicTypes.hpp"

namespace Com {

constexpr XBee::RadioCommand XBee::ENTER_COMMAND_MODE;
constexpr XBee::RadioCommand XBee::NODE_IDENTIFIER;
constexpr XBee::RadioCommand XBee::ENERGY_DENSITY;
constexpr XBee::RadioCommand XBee::EXIT_COMMAND_MODE;

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

XBee ::XBee(const char* const compName)
    : XBeeComponentBase(compName),
      m_circular(m_data, sizeof m_data),
      m_state(PASSTHROUGH),
      m_opCode(0),
      m_cmdSeq(0),
      m_timeoutCount(0),
      m_reinit(true) {}

XBee ::~XBee() {}

void XBee ::init(const NATIVE_INT_TYPE queueDepth, const NATIVE_INT_TYPE instance) {
    XBeeComponentBase::init(queueDepth, instance);
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void XBee ::drvConnected_handler(const NATIVE_INT_TYPE portNum) {
    report_ready();
}

void XBee ::drvDataIn_handler(const NATIVE_INT_TYPE portNum,
                              Fw::Buffer& recvBuffer,
                              const Drv::RecvStatus& recvStatus) {
    m_lock.lock();
    const ComState current_state = m_state;
    m_lock.unlock();
    // On a good read when not in transparent mode, we process data
    if (recvStatus == Drv::RecvStatus::RECV_OK && current_state != PASSTHROUGH && current_state != ERROR_TIMEOUT && current_state != QUIET_RADIO) {
        m_circular.serialize(recvBuffer.getData(), recvBuffer.getSize());
        deallocate_out(0, recvBuffer);
        state_machine();
    }
    // Otherwise pass data to the deframer
    else if (current_state == PASSTHROUGH || current_state == QUIET_RADIO) {
        comDataOut_out(0, recvBuffer, recvStatus);
    }
    else {
        deallocate_out(0, recvBuffer);
    }
}

Drv::SendStatus XBee ::comDataIn_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& sendBuffer) {
    Svc::ComSendStatus radioReady = Svc::ComSendStatus::FAIL;
    m_lock.lock();
    // Only attempt to send data when in transparent mode. Sending data in command mode is an error
    if (PASSTHROUGH == m_state) {
        Drv::SendStatus driverStatus = Drv::SendStatus::SEND_RETRY;
        for (NATIVE_UINT_TYPE i = 0; driverStatus == Drv::SendStatus::SEND_RETRY && i < retryLimit; i++) {
            driverStatus = drvDataOut_out(0, const_cast<Fw::Buffer&>(sendBuffer));
        }
        radioReady =
            (driverStatus.e == Drv::SendStatus::SEND_OK) ? Svc::ComSendStatus::READY : Svc::ComSendStatus::FAIL;
    } else {
        m_reinit = true;
    }
    m_lock.unlock();
    // Only send ready if the port is connected
    if (isConnected_comStatus_OutputPort(0)) {
        comStatus_out(0, radioReady);
    }
    return Drv::SendStatus::SEND_OK;  // Always send ok to deframer as it does not handle this anyway
}

void XBee ::run_handler(const NATIVE_INT_TYPE portNum, U32 context) {
    m_lock.lock();
    if (m_state != PASSTHROUGH) {
        m_timeoutCount = m_timeoutCount + 1;
        // Check if ready to enable command mode
        if ((m_timeoutCount > QUIET_TICKS_1HZ) && (m_state == QUIET_RADIO)) {
            initiate_command();
        }
        // Check if timeing out of command mode
        else if (m_timeoutCount > TIMEOUT_TICKS_1HZ) {
            deinitiate_command(Fw::CmdResponse::EXECUTION_ERROR);
            m_state = PASSTHROUGH;
        }
    }
    m_lock.unlock();
}


// ----------------------------------------------------------------------
// Command handler implementations
// ----------------------------------------------------------------------

void XBee ::ReportNodeIdentifier_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq) {
    m_lock.lock();
    stage_command(XBee::NODE_IDENTIFIER, opCode, cmdSeq);
    m_lock.unlock();
}

void XBee ::EnergyDensityScan_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq) {
    m_lock.lock();
    stage_command(XBee::ENERGY_DENSITY, opCode, cmdSeq);
    m_lock.unlock();
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

void XBee  ::stage_command(const RadioCommand& command, const FwOpcodeType opCode, const U32 cmdSeq) {
    if ((m_currentCommand != nullptr) || (m_state != PASSTHROUGH)) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::BUSY);
    } else {
        FW_ASSERT(m_currentCommand == nullptr);
        FW_ASSERT(m_state == PASSTHROUGH);
        m_state = QUIET_RADIO;
        m_opCode = opCode;
        m_cmdSeq = cmdSeq;
        m_currentCommand = &command;
    }
}

void XBee ::initiate_command() {
    // Attempt to enter command mode, otherwise immediately fallback to passthrough
    if (send_radio_command(XBee::ENTER_COMMAND_MODE)) {
        m_state = AWAIT_COMMAND_MODE;
    } else {
        deinitiate_command(Fw::CmdResponse::EXECUTION_ERROR);
    }
}

bool XBee ::deinitiate_command(const Fw::CmdResponse& response) {
    report_ready();
    this->cmdResponse_out(m_opCode, m_cmdSeq, response);
    m_currentCommand = nullptr;
    m_timeoutCount = 0;
    m_opCode = 0;
    m_cmdSeq = 0;
    return true;
}

bool XBee ::send_radio_command(const RadioCommand& command) {
    // Send command, wait 1 second per command mode default guidelines
    Drv::SendStatus driverStatus = Drv::SendStatus::SEND_RETRY;
    Fw::Buffer buffer = this->allocate_out(0, command.length);
    if (buffer.getSize() >= command.length) {
        ::memcpy(buffer.getData(), command.command, command.length);
        for (NATIVE_UINT_TYPE i = 0; driverStatus == Drv::SendStatus::SEND_RETRY && i < retryLimit; i++) {
            driverStatus = drvDataOut_out(0, buffer);
        }
    }
    m_timeoutCount = (driverStatus.e == Drv::SendStatus::SEND_OK) ? m_timeoutCount : 0;
    return (buffer.getSize() >= command.length) && (driverStatus.e == Drv::SendStatus::SEND_OK);
}

void XBee ::state_machine() {
    XBee::ProcessResponse response = this->process_response();
    if (response == XBee::ProcessResponse::PROCESSED_ERROR) {
        m_state = ERROR_TIMEOUT;
    } else if (response == XBee::ProcessResponse::PROCESSED_GOOD) {
        bool success = true;
        switch (m_state) {
            case AWAIT_COMMAND_MODE:
                m_lock.lock();
                FW_ASSERT(m_currentCommand != nullptr);
                success = send_radio_command(*m_currentCommand);
                m_lock.unlock();
                break;
            case AWAIT_COMMAND_RESPONSE:
                m_lock.lock();
                success = send_radio_command(EXIT_COMMAND_MODE);
                m_lock.unlock();
                break;
            case AWAIT_PASSTHROUGH:
                m_lock.lock();
                success = deinitiate_command(Fw::CmdResponse::OK);
                m_lock.unlock();
                break;
            default:
                FW_ASSERT(0);
                break;
        }
        m_lock.lock();
        m_state = success ? static_cast<ComState>(m_state + 1) : ERROR_TIMEOUT;
        FW_ASSERT(m_state >= ERROR_TIMEOUT && m_state <= PASSTHROUGH, m_state);
        m_lock.unlock();
    }
}

void XBee ::report_ready() {
    if (m_reinit) {
        Svc::ComSendStatus radioReady = Svc::ComSendStatus::READY;
        if (isConnected_comStatus_OutputPort(0)) {
            comStatus_out(0, radioReady);
        }
        m_reinit = false;
    }
}

// ----------------------------------------------------------------------
// Helper functions: XBee command response processing functions
// ----------------------------------------------------------------------

XBee::ProcessResponse XBee ::process_response() {
    XBee::ProcessResponse response = MORE_NEEDED;
    U8 last_byte = 0;
    m_circular.peek(last_byte, m_circular.get_allocated_size() - 1);
    // Looking of OK\r or ERROR\r
    if (last_byte == '\r') {
        if (m_state != AWAIT_COMMAND_RESPONSE) {
            response = process_ok_or_error();
        } else if (m_currentCommand == &XBee::NODE_IDENTIFIER) {
            response = process_node_identifier();
        } else if (m_currentCommand == &XBee::ENERGY_DENSITY) {
            response = process_energy_density();
        }
        // Clear the existing data in the circular buffer
        m_circular.rotate(m_circular.get_allocated_size());
    }
    return response;
}

XBee::ProcessResponse XBee ::process_ok_or_error() {
    CHAR ok[3];
    const U32 peek_size = FW_MIN(sizeof(ok), m_circular.get_allocated_size());
    const U32 peek_offset = FW_MAX(m_circular.get_allocated_size() - peek_size, 0);
    m_circular.peek(reinterpret_cast<U8*>(ok), peek_size, peek_offset);
    // OK was received that is a successful response
    if (ok[0] == 'O' && ok[1] == 'K' && ok[2] == '\r') {
        return XBee::ProcessResponse::PROCESSED_GOOD;
    }
    // Something else produces error
    return XBee::ProcessResponse::PROCESSED_ERROR;
}

XBee::ProcessResponse XBee ::process_node_identifier() {
    Fw::LogStringArg identifier;
    const U32 peek_size = FW_MIN(FW_MIN(identifier.getCapacity(), m_currentCommand->response_length + 1),
                                 m_circular.get_allocated_size());
    const U32 peek_offset = FW_MAX(m_circular.get_allocated_size() - peek_size, 0);
    char* peak_buffer = const_cast<char*>(identifier.toChar());

    m_circular.peek(reinterpret_cast<U8*>(peak_buffer), peek_size, peek_offset);
    peak_buffer[peek_size - 1] = '\0';
    this->log_ACTIVITY_HI_RadioNodeIdentifier(identifier);
    // Always return true
    return XBee::ProcessResponse::PROCESSED_GOOD;
}

XBee::ProcessResponse XBee ::process_energy_density() {
    XBee::ProcessResponse response = XBee::PROCESSED_GOOD;
    EnergyDensityType density;
    for (U32 i = 0; i < density.SIZE; i++) {
        U8 reading[3];
        if ((m_circular.peek(reading, sizeof(reading)) == Fw::SerializeStatus::FW_SERIALIZE_OK) &&
            (reading[sizeof(reading) - 1] == ',')) {
            density[i] = static_cast<U8>(convert_char(reading[0]) << 4 | convert_char(reading[1]));
        } else {
            response = XBee::PROCESSED_ERROR;
            break;
        }
        m_circular.rotate(sizeof(reading));
    }
    // Successful read, send telemetry
    if (response == XBee::PROCESSED_GOOD) {
        tlmWrite_EnergyDensity(density);
    }
    return response;
}

}  // end namespace Com
