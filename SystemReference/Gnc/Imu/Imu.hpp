// ======================================================================
// \title  Imu.hpp
// \author vbai
// \brief  hpp file for Imu component implementation class
// ======================================================================

#ifndef Imu_HPP
#define Imu_HPP

#include "SystemReference/Gnc/Imu/ImuComponentAc.hpp"

namespace Gnc {

class Imu : public ImuComponentBase {

public:
  // ----------------------------------------------------------------------
  // Constants and Types
  // ----------------------------------------------------------------------

  // Values for the static consts were attained from the data sheet of the
  // MPU6050
  static const U8 I2C_DEV0_ADDR = 0x68;
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
  static constexpr float gyroScaleFactor = 131.0f;

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  //! Construct object Imu
  //!
  Imu(const char *const compName /*!< The component name*/
  );

  //! Initialize object Imu
  //!
  void init(const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
  );


  //! Turn power on/off of device
  //!
  void power(PowerState powerState);

  void setup(U8 devAddress);

  void config();

  //! Destroy object Imu
  //!
  ~Imu();

  PRIVATE :

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for Run
      //!
      void
      Run_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                  NATIVE_UINT_TYPE context       /*!<The call order*/
      );

  //! Handler implementation for getAcceleration
  //!
  Gnc::ImuData
  getAcceleration_handler(const NATIVE_INT_TYPE portNum /*!< The port number*/
  );

  //! Handler implementation for getGyroscope
  //!
  Gnc::ImuData
  getGyroscope_handler(const NATIVE_INT_TYPE portNum /*!< The port number*/
  );

  //! Implementation for PowerSwitch command handler
  //! Command to turn on the device
  void
  PowerSwitch_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                         const U32 cmdSeq, /*!< The command sequence number*/
                         PowerState powerState);

  // ----------------------------------------------------------------------
  // Helper Functions
  // ----------------------------------------------------------------------

  Drv::I2cStatus read(U8 dev_addr, Fw::Buffer &buffer);
  Drv::I2cStatus setupReadRegister(U8 dev_addr, U8 reg);
  Drv::I2cStatus readRegisterBlock(U8 registerAdd, Fw::Buffer &buffer);
  void updateAccel();
  void updateGyro();

  // ----------------------------------------------------------------------
  // Member Variables
  // ----------------------------------------------------------------------

  Gnc::ImuData m_gyro;
  Gnc::ImuData m_accel;
  U8 m_i2cDevAddress;
  bool m_power;
};

} // end namespace Gnc

#endif
