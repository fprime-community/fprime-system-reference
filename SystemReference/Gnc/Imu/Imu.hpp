// ======================================================================
// \title  Imu.hpp
// \author vbai
// \brief  hpp file for Imu component implementation class
// ======================================================================

#ifndef Gnc_Imu_HPP
#define Gnc_Imu_HPP

#include "SystemReference/Gnc/Imu/ImuComponentAc.hpp"

namespace Gnc {

class Imu : public ImuComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Constants and Types
    // ----------------------------------------------------------------------

    // Values for the static consts were attained from the data sheet of the
    // MPU6050

    //! The I2C device addresses
    struct I2cDevAddr {
      enum T {
        //! The I2C device address with AD0 set to zero
        AD0_0 = 0x68,
        //! The I2C device address with AD0 set to one
        AD0_1 = 0x69
      };
    };
    static const U8 POWER_MGMT_ADDR = 0x6B;
    static const U16 IMU_MAX_DATA_SIZE_BYTES = 6;
    static const U16 IMU_REG_SIZE_BYTES = 1;
    static const U8 IMU_RAW_ACCEL_ADDR = 0x3B;
    static const U8 IMU_RAW_GYRO_ADDR = 0x43;
    static const U8 GYRO_CONFIG_ADDR = 0x1B;
    static const U8 ACCEL_CONFIG_ADDR = 0x1C;
    static const U8 POWER_ON_VALUE = 0;
    static const U8 POWER_OFF_VALUE = 0x40;
    static constexpr float accelScaleFactor = 16384.0f;
    static constexpr float gyroScaleFactor = 131.072f;

    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object Imu
    //!
    Imu(const char* const compName /*!< The component name*/
    );

    //! Initialize object Imu
    //!
    void init(const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

    //! Destroy object Imu
    //!
    ~Imu();

    void setup(I2cDevAddr::T devAddress);

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for Run
    //!
    void Run_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                     NATIVE_UINT_TYPE context       /*!<The call order*/
    );

    //! Implementation for PowerSwitch command handler
    //! Command to turn on the device
    void PowerSwitch_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                const U32 cmdSeq,          /*!< The command sequence number*/
                                PowerState powerState);

    // ----------------------------------------------------------------------
    // Helper Functions
    // ----------------------------------------------------------------------

    /**
     * \brief sets up the IMU to know what register the next read should be from
     *
     * The MPU-6050 requires a write call with a register's address before a read will function correctly. This helper
     * sets up that read address by writing it to the IMU via the I2C write port.
     *
     * \param reg: IMU internal address to the first register to be read
     * \return: I2C from the write call
     */
    Drv::I2cStatus setupReadRegister(U8 reg);

    /**
     * \brief reads a block of registers from the IMU
     *
     * This function starts by writing the startRegister to the IMU by passing it to `setupReadRegister`. It then calls
     * the read port of the I2C bus to read data from the IMU. It will read `buffer.getSize()` bytes from the I2C device
     * and as such the caller must set this up.
     *
     * \param startRegister: register address to start reading from
     * \param buffer: buffer to read into. Determines size of read.
     * \return: I2C status of transactions
     */
    Drv::I2cStatus readRegisterBlock(U8 startRegister, Fw::Buffer& buffer);

    /**
     * \brief unpacks a buffer into a vector with scaled elements
     *
     * This will unpack data from buffer into a Gnc::Vector type by unpacking 3x I16 values (in big endian format) and
     * scales each by dividing by the scaleFactor provided.
     *
     * \param buffer: buffer wrapping data, must contain at least 6 byes (3x I16)
     * \param scaleFactor: scale factor to divide each element by
     * \return initalized vector
     */
    Vector deserializeVector(Fw::Buffer& buffer, F32 scaleFactor);

    //! Configure the accelerometer and gyroscope
    //!
    void config();

    //! Turn power on/off of device
    //!
    void power(PowerState powerState);

    //! Read, telemeter, and update local compy of accelerometer data
    //!
    void updateAccel();

    //! Read, telemeter, and update local compy of gyroscope data
    //!
    void updateGyro();


    // ----------------------------------------------------------------------
    // Member Variables
    // ----------------------------------------------------------------------
    I2cDevAddr::T m_i2cDevAddress; //!< Stored device address
    PowerState::t m_power; //!< Power state of device
};

}  // end namespace Gnc

#endif
