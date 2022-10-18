// ======================================================================
// \title  Imu.hpp
// \author vbai
// \brief  cpp file for Imu test harness implementation class
// ======================================================================

#include <cstring>

#include "Tester.hpp"
#include <STest/STest/Pick/Pick.hpp>

#define INSTANCE 0
#define MAX_HISTORY_SIZE 100
#define ADDRESS_TEST Gnc::Imu::I2cDevAddr::AD0_0

namespace Gnc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

Tester ::Tester() :
  ImuGTestBase("Tester", MAX_HISTORY_SIZE), component("Imu"),
  addrBuf(0),
  accelSerBuf(this->accelBuf, sizeof this->accelBuf),
  gyroSerBuf(this->gyroBuf, sizeof this->gyroBuf)
{
    memset(this->accelBuf, 0, sizeof this->accelBuf);
    memset(this->gyroBuf, 0, sizeof this->gyroBuf);
    this->initComponents();
    this->connectPorts();
    this->component.setup(ADDRESS_TEST);
}

Tester ::~Tester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester ::testGetAccelTlm() {
    sendCmd_PowerSwitch(0, 0, PowerState::ON);
    for (U32 i = 0; i < 5; i++) {
        this->invoke_to_Run(0, 0);
        Gnc::Vector expectedVector;
        for (U32 j = 0; j < 3; ++j) {
            I16 intCoord = 0;
            const auto status = this->accelSerBuf.deserialize(intCoord);
            EXPECT_EQ(status, Fw::FW_SERIALIZE_OK);
            const F32 f32Coord = static_cast<F32>(intCoord) / Imu::accelScaleFactor;
            expectedVector[j] = f32Coord;
        }
        ASSERT_TLM_accelerometer(i, expectedVector);
    }
}

void Tester ::testGetGyroTlm() {
    sendCmd_PowerSwitch(0, 0, PowerState::ON);
    for (U32 i = 0; i < 5; i++) {
        this->invoke_to_Run(0, 0);
        Gnc::Vector expectedVector;
        for (U32 j = 0; j < 3; ++j) {
            I16 intCoord = 0;
            const auto status = this->gyroSerBuf.deserialize(intCoord);
            EXPECT_EQ(status, Fw::FW_SERIALIZE_OK);
            const F32 f32Coord =
                static_cast<F32>(intCoord) / Imu::gyroScaleFactor;
            expectedVector[j] = f32Coord;
        }
        ASSERT_TLM_gyroscope(i, expectedVector);
    }
}

void Tester::testTlmError() {
    sendCmd_PowerSwitch(0, 0, PowerState::ON);
    this->m_readStatus = Drv::I2cStatus::I2C_OTHER_ERR;
    this->m_writeStatus = Drv::I2cStatus::I2C_OTHER_ERR;
    this->invoke_to_Run(0, 0);
    ASSERT_EVENTS_TelemetryError_SIZE(2);
    ASSERT_EVENTS_TelemetryError(0, m_readStatus);
    ASSERT_EVENTS_TelemetryError(0, m_readStatus);
}

void Tester::testPowerError() {
    this->m_writeStatus = Drv::I2cStatus::I2C_WRITE_ERR;
    sendCmd_PowerSwitch(0, 0, PowerState::OFF);  // Repeated action, no change
    ASSERT_EVENTS_PowerModeError_SIZE(0);
    sendCmd_PowerSwitch(0, 0, PowerState::ON);
    ASSERT_EVENTS_PowerModeError_SIZE(1);
    ASSERT_EVENTS_SetUpConfigError_SIZE(0);
    ASSERT_EVENTS_PowerModeError(0, m_writeStatus);
}

void Tester::testSetupError() {
    // Used to denote "OK" then "ERROR"; see handler
    this->m_writeStatus = Drv::I2cStatus::I2C_ADDRESS_ERR;
    sendCmd_PowerSwitch(0, 0, PowerState::ON);
    ASSERT_EVENTS_SetUpConfigError_SIZE(2);
    ASSERT_EVENTS_SetUpConfigError(0, m_writeStatus);
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

Drv::I2cStatus Tester ::from_read_handler(const NATIVE_INT_TYPE portNum, U32 addr, Fw::Buffer& serBuffer) {
    this->pushFromPortEntry_read(addr, serBuffer);
    EXPECT_EQ(addr, ADDRESS_TEST);
    if (this->m_readStatus == Drv::I2cStatus::I2C_OK) {
        // Fill buffer with random data
        U8* const data = serBuffer.getData();
        const U32 size = serBuffer.getSize();
        const U32 imu_max_data_size = Gnc::Imu::IMU_MAX_DATA_SIZE_BYTES;
        EXPECT_LE(size, imu_max_data_size);
        for (U32 i = 0; i < size; ++i) {
            const U8 byte = STest::Pick::any();
            data[i] = byte;
        }
        if (this->addrBuf == Imu::IMU_RAW_ACCEL_ADDR) {
            // Last address write was IMU_RAW_ACCEL_ADDR
            // Copy data into the accel buffer
            this->accelSerBuf.resetSer();
            const auto status = this->accelSerBuf.pushBytes(&data[0], size);
            EXPECT_EQ(status, Fw::FW_SERIALIZE_OK);
        }
        else if (this->addrBuf == Imu::IMU_RAW_GYRO_ADDR) {
            // Last address write was IMU_RAW_GYRO_ADDR
            // Copy data into the gyro buffer
            this->gyroSerBuf.resetSer();
            const auto status = this->gyroSerBuf.pushBytes(&data[0], size);
            EXPECT_EQ(status, Fw::FW_SERIALIZE_OK);
        }
    }
    return this->m_readStatus;
}

Drv::I2cStatus Tester ::from_write_handler(const NATIVE_INT_TYPE portNum, U32 addr, Fw::Buffer& serBuffer) {
    this->pushFromPortEntry_write(addr, serBuffer);
    EXPECT_EQ(addr, ADDRESS_TEST);
    const U32 size = serBuffer.getSize();
    EXPECT_GT(size, 0);
    U8* const data = (U8*)serBuffer.getData();
    Drv::I2cStatus status = Drv::I2cStatus::I2C_OK;
    if (size == 1) {
        // Address write: store the address in the address buffer
        this->addrBuf = data[0];
    }
    if (this->m_writeStatus == Drv::I2cStatus::I2C_ADDRESS_ERR) {
        // If the write status indicates an address error, then return
        // OK this time and set up the write status for a write error next time
        this->m_writeStatus = Drv::I2cStatus::I2C_WRITE_ERR;
    }
    else {
        // Otherwise return the write status
        status = this->m_writeStatus;
    }
    return status;
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void Tester ::connectPorts() {
    // Run
    this->connect_to_Run(0, this->component.get_Run_InputPort(0));

    // cmdIn
    this->connect_to_cmdIn(0, this->component.get_cmdIn_InputPort(0));

    // Log
    this->component.set_Log_OutputPort(0, this->get_from_Log(0));

    // LogText
    this->component.set_LogText_OutputPort(0, this->get_from_LogText(0));

    // Time
    this->component.set_Time_OutputPort(0, this->get_from_Time(0));

    // Tlm
    this->component.set_Tlm_OutputPort(0, this->get_from_Tlm(0));

    // cmdRegOut
    this->component.set_cmdRegOut_OutputPort(0, this->get_from_cmdRegOut(0));

    // cmdResponseOut
    this->component.set_cmdResponseOut_OutputPort(0, this->get_from_cmdResponseOut(0));

    // read
    this->component.set_read_OutputPort(0, this->get_from_read(0));

    // write
    this->component.set_write_OutputPort(0, this->get_from_write(0));
}

void Tester ::initComponents() {
    this->init();
    this->component.init(INSTANCE);
}

}  // end namespace Gnc
