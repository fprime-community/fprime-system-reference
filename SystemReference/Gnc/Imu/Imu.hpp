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

  static const U8 I2C_DEV0_ADDR = 0x68;
  static const U16 IMU_MAX_DATA_SIZE = 6;
  static const U16 IMU_REG_SIZE = 1;
  static const U8 IMU_RAW_ACCEL_ADDR = 0x3B;
  static const U8 IMU_RAW_GYRO_ADDR = 0x48;

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

  // ----------------------------------------------------------------------
  // Helper Functions
  // ----------------------------------------------------------------------

  Drv::I2cStatus read(U8 dev_addr, Fw::Buffer &buffer);
  Drv::I2cStatus write(U8 dev_addr, U8 reg);
  Drv::I2cStatus readRegisterBlock(U8 registerAdd, Fw::Buffer &buffer);
  void updateAccel();
  void updateGyro();


  // ----------------------------------------------------------------------
  // Member variables
  // ----------------------------------------------------------------------

  Gnc::ImuData m_gyro;
  Gnc::ImuData m_accel;
  U8 m_i2cDevAddress;
};

} // end namespace Gnc

#endif
