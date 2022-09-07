// ======================================================================
// \title  XBee.hpp
// \author mstarch
// \brief  hpp file for XBee component implementation class
// ======================================================================

#ifndef XBee_HPP
#define XBee_HPP

#include "SystemReference/Com/XBee/XBeeComponentAc.hpp"
#include "Utils/Types/CircularBuffer.hpp"
#include "Os/Mutex.hpp"

namespace Com {

class XBee : public XBeeComponentBase {
  public:
    static const NATIVE_UINT_TYPE retryLimit = 10; // Arbitrary limit to the number of retries
    static const NATIVE_UINT_TYPE xbeeQuiescentTime_ms = 1000; // Milliseconds to quiesce the radio prior to sending +++
    static const U32 MAX_COMMAND_DATA = 48; // Max size of data from command mode NI command: 20 bytes, ED: 3x16: 48
    static const U32 TIMEOUT_TICKS_1HZ = 10; // Number of 1Hz ticks to timeout of command mode
    static const U32 QUIET_TICKS_1HZ = xbeeQuiescentTime_ms/1000 + 1; // Number of 1Hz ticks to quiet radio

    static_assert(QUIET_TICKS_1HZ < TIMEOUT_TICKS_1HZ, "Quiet ticks may not be less than timeout ticks");

    /**
     * \brief state machine states for talking with the radio
     *
     * Represents the state transitions in the flow to talking to the XBee radio using command mode. Notably, the radio
     * will send data only when in PASSTHROUGH state, and waits without change when in ERROR_TIMEOUT.
     */
    enum ComState {
        ERROR_TIMEOUT = -1,         //!< Error state, waiting for the 10S command mode timeout
        QUIET_RADIO = 0,            //!< State to quiet the radio so command mode will be respected
        AWAIT_COMMAND_MODE = 1,     //!< Waiting for command mode, sends command when response processed
        AWAIT_COMMAND_RESPONSE = 2, //!< Waiting for command response, sends exit when response processed
        AWAIT_PASSTHROUGH = 3,      //!< Waiting for exit response, sends "ready" for more data when response processed
        PASSTHROUGH = 4,            //!< Passing through data to remote radio
    };

    /**
     * \brief status of a processed command in radio command mode
     */
    enum ProcessResponse {
        PROCESSED_GOOD,  //!< Expected response found and processed
        PROCESSED_ERROR, //!< Unexpected response
        MORE_NEEDED,     //!< More data is needed before processing can finish
    };

    /**
     * \brief Format for configured radio commands
     */
    struct RadioCommand {
        const CHAR* command; //!< AT text of command
        U32 length;          //!< Length of text including \r and other characters
        U32 response_length; //!< Maximum length of expected response
    };

    /**
     * \brief Command text to enter command mode
     */
    static constexpr RadioCommand ENTER_COMMAND_MODE = {
        .command = "+++",
        .length = 3,
        .response_length = 2
    };

    /**
     * \brief Report the radio identifier string
     */
    static constexpr RadioCommand NODE_IDENTIFIER = {
        .command = "ATNI\r",
        .length = 5,
        .response_length = 20
    };

    /**
     * \brief Report the energy density of each channel
     */
    static constexpr RadioCommand ENERGY_DENSITY = {
        .command = "ATED\r",
        .length = 5,
        .response_length = 3 * 16
    };

    /**
     * \brief Exit command mode
     */
    static constexpr RadioCommand EXIT_COMMAND_MODE = {
        .command = "ATCN\r",
        .length = 5,
        .response_length = 2
    };

    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object XBee
    //!
    XBee(const char* const compName /*!< The component name*/
    );

    //! Destroy object XBee
    //!
    ~XBee();

    //! Initialize object XBee
    //!
    void init(const NATIVE_INT_TYPE queueDepth,  /*!< The queue depth*/
              const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for comDataIn
    //!
    Drv::SendStatus comDataIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                      Fw::Buffer& sendBuffer);

    //! Handler implementation for drvConnected
    //!
    void drvConnected_handler(const NATIVE_INT_TYPE portNum);

    //! Handler implementation for drvDataIn
    //!
    void drvDataIn_handler(const NATIVE_INT_TYPE portNum,
                           /*!< The port number*/ Fw::Buffer& recvBuffer,
                           const Drv::RecvStatus& recvStatus);

    //! Handler implementation for Run port
    //!
    void run_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                     U32 context);

  PRIVATE:
    // ----------------------------------------------------------------------
    // Command handler implementations
    // ----------------------------------------------------------------------

    //! Implementation for ReportXBeeSerialNumber command handler
    //!
    void ReportNodeIdentifier_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                         const U32 cmdSeq           /*!< The command sequence number*/
    );

    //! Implementation for ChannelScan command handler
    //!
    void EnergyDensityScan_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                const U32 cmdSeq           /*!< The command sequence number*/
    );

  PRIVATE:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------
    /**
     * \brief Process response entry point dispatching internal response handlers
     *
     * All responses are processed using this function. This function delegates to the specific response handlers. It
     * will process any response that currently ends with '\r', otherwise it will report MORE_NEEDED.
     *
     * \return GOOD, ERROR, or MORE_NEEDED
     */
    ProcessResponse process_response();
    /**
     * \brief Processes node-identifier text
     * \return GOOD
     */
    ProcessResponse process_node_identifier();
    /**
     * \brief Processes OK/ERROR responses
     * \return GOOD/ERROR
     */
    ProcessResponse process_ok_or_error();
    /**
     * \brief Processes energy density responses
     * \return GOOD/ERROR
     */
    ProcessResponse process_energy_density();
    /**
     * \brief Stages a command for dispatch once command-mode has been reached. Sets state to QUIET_RADIO
     * \param command: radio command to stage
     * \param opCode: opcode of the command to stage
     * \param cmdSeq: sequence number of the command being staged
     */
    void stage_command(const RadioCommand& command, const FwOpcodeType opCode, const U32 cmdSeq);
    /**
     * \brief Initiates command mode and sets state to AWAIT_COMMAND
     */
    void initiate_command();
    /**
     * \brief Deinitiates command mode, resets variables, and returns state to PASSTHROUGH
     * \param response: response to echo for commands
     * \return: true, for patallel state machine execution
     */
    bool deinitiate_command(const Fw::CmdResponse& response);
    /**
     * \brief send a command string out radio
     * \param command: command to send out radio
     * \return: true on successful transmission, false otherwise
     */
    bool send_radio_command(const RadioCommand& command);
    /**
     * \brief processes one iteration of the state machine as triggered via a response UART message
     */
    void state_machine();
    /**
     * \brief report the radio is ready for data to any attached com queue
     */
    void report_ready();
    /**
     * \brief converts ASCII hex-digits into their numerical equivalent
     * \param input: input ASCII hex character (0-F)
     * \return: numerical representation of the hex character
     */
    inline U8 convert_char(U8 input) {
        return (input >= 'A') ? (input - 'A' + 0x0Au) : (input - '0');
    }

    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    Types::CircularBuffer m_circular; //!< Used to capture data from UART port when handling cmd-response with XBee
    Os::Mutex m_lock; //!< Lock to protect state changes and UART port

    ComState m_state; //!< State of the component
    const RadioCommand* m_currentCommand; //!< Current command that is being dispatched to XBee

    FwOpcodeType m_opCode; //!< Opcode store for deferred command completion
    U32 m_cmdSeq; //!< Sequence store for deferred command completion

    U32 m_timeoutCount; //!< Counts the number of ticks while waiting for command-mode timeout
    bool m_reinit; //!< Does a new ready signal need to be sent when commanding is done
    U8 m_data[MAX_COMMAND_DATA + 1];
};
}  // end namespace Com

#endif
