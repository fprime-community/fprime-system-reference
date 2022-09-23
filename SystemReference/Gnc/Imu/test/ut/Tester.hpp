// ======================================================================
// \title  Imu/test/ut/Tester.hpp
// \author vbai
// \brief  hpp file for Imu test harness implementation class
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "Fw/Types/SerialBuffer.hpp"
#include "GTestBase.hpp"
#include "STest/Pick/Pick.hpp"
#include "STest/Random/Random.hpp"
#include "SystemReference/Gnc/Imu/Imu.hpp"

namespace Gnc {

class Tester : public ImuGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants and Types
    // ----------------------------------------------------------------------

    //! The size of the read buffer
    static constexpr U16 READ_BUF_SIZE_BYTES = Imu::IMU_MAX_DATA_SIZE_BYTES;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object Tester
    //!
    Tester();

    //! Destroy object Tester
    //!
    ~Tester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Test to get gyroscope telemetry
    //!
    void testGetGyroTlm();

    //! Test to get accelerometer telemetry
    //!
    void testGetAccelTlm();

    //! Test to check event of bad telemetry request
    //!
    void testTlmError();

    //! Test to check power on error
    //!
    void testPowerError();

    //! Test to check setup error
    //!
    void testSetupError();

  private:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Handler for from_read
    //!
    Drv::I2cStatus from_read_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                     U32 addr,                      /*!< I2C slave device address*/
                                     Fw::Buffer& serBuffer          /*!< Buffer with data to read/write to/from*/
    );

    //! Handler for from_write
    //!
    Drv::I2cStatus from_write_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                      U32 addr,                      /*!< I2C slave device address*/
                                      Fw::Buffer& serBuffer          /*!< Buffer with data to read/write to/from*/
    );

  private:
    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    //! Connect ports
    //!
    void connectPorts();

    //! Initialize components
    //!
    void initComponents();

  private:
    // ----------------------------------------------------------------------
    // Variables
    // ----------------------------------------------------------------------

    //! The component under test
    //!
    Imu component;

    //! The read status from the driver
    Drv::I2cStatus m_readStatus;

    //! The write status from the driver
    Drv::I2cStatus m_writeStatus;

    //! Buffer for storing last address written
    U8 addrBuf;

    //! Buffer for storing accel data read by the component
    U8 accelBuf[READ_BUF_SIZE_BYTES];

    //! Serial buffer wrapping accelBuf
    Fw::SerialBuffer accelSerBuf;

    //! Buffer for storing gyro data read by the component
    U8 gyroBuf[READ_BUF_SIZE_BYTES];

    //! Serial buffer wrapping gyroBuf
    Fw::SerialBuffer gyroSerBuf;
};

}  // end namespace Gnc

#endif
