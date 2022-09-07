// ======================================================================
// \title  Imu.cpp
// \author vbai
// \brief  cpp file for Imu component implementation class
// ======================================================================

#include "Fw/Types/BasicTypes.hpp"
#include <SystemReference/Gnc/Imu/Imu.hpp>

namespace Gnc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

Imu ::Imu(const char *const compName)
    : ImuComponentBase(compName), m_power(false) {}

void Imu ::init(const NATIVE_INT_TYPE instance) {
  ImuComponentBase::init(instance);
}

void Imu::setup(U8 devAddress) { m_i2cDevAddress = devAddress; }

void Imu::power(PowerState powerState){
    Drv::I2cStatus writePowerStatus;
    U8 data[IMU_REG_SIZE_BYTES*2];
    Fw::Buffer buffer(data, sizeof(data));

    FW_ASSERT(sizeof(data) > 0);
    data[0] = POWER_MGMT_ADDR;

    switch(powerState.e){
      case PowerState::ON:
        data[1] = 0;
        m_power = true;
        break;
      case PowerState::OFF:
        // Enables sleep mode
        data[1] = 0x40;
        break;
      default:
         FW_ASSERT(0);
    }

    writePowerStatus = write_out(0, m_i2cDevAddress, buffer);
    if(writePowerStatus != Drv::I2cStatus::I2C_OK){
      this->log_WARNING_HI_PowerModeError(writePowerStatus);
    }

    if(m_power){
      config();
    }
}


void Imu::config() {
  Fw::Buffer buffer;
  Drv::I2cStatus writeStatus;
  U8 data[IMU_REG_SIZE_BYTES * 2];
  buffer.setData(data);
  buffer.setSize(sizeof(data));

  FW_ASSERT(sizeof(data) > 0);

  // Set gyro range to +-250 deg/s
  data[0] = GYRO_CONFIG_ADDR;
  data[1] = 0;

  writeStatus = write_out(0, m_i2cDevAddress, buffer);

  if (writeStatus != Drv::I2cStatus::I2C_OK) {
    this->log_WARNING_HI_SetUpConfigError(writeStatus);
  }

  // Set accel range to +- 2g
  data[0] = ACCEL_CONFIG_ADDR;
  data[1] = 0;
  writeStatus = write_out(0, m_i2cDevAddress, buffer);

  if (writeStatus != Drv::I2cStatus::I2C_OK) {
    this->log_WARNING_HI_SetUpConfigError(writeStatus);
  }
}

Imu ::~Imu() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void Imu ::Run_handler(const NATIVE_INT_TYPE portNum,
                       NATIVE_UINT_TYPE context) {
  if (m_power) {
    updateAccel();
    updateGyro();
  }
}

Gnc::ImuData Imu ::getAcceleration_handler(const NATIVE_INT_TYPE portNum) {
  ImuData accelData = m_accel;
  m_accel.setstatus(Svc::MeasurementStatus::STALE);
  return accelData;
}

Gnc::ImuData Imu ::getGyroscope_handler(const NATIVE_INT_TYPE portNum) {
  ImuData gyroData = m_gyro;
  m_gyro.setstatus(Svc::MeasurementStatus::STALE);
  return gyroData;
}

void Imu ::PowerSwitch_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq,
                                  PowerState powerState) {
  power(powerState);
  this->log_ACTIVITY_HI_PowerStatus(powerState);
  this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

// ----------------------------------------------------------------------
// Helper Functions
// ----------------------------------------------------------------------
// Reads in buffer data
Drv::I2cStatus Imu ::read(U8 dev_addr, Fw::Buffer &buffer) {
  return this->read_out(0, dev_addr, buffer);
}

// Reads the next block of address
Drv::I2cStatus Imu::readRegisterBlock(U8 registerAdd, Fw::Buffer &buffer) {
  Drv::I2cStatus status;

  status = this->setupReadRegister(m_i2cDevAddress, registerAdd);
  if (status == Drv::I2cStatus::I2C_OK) {
    status = this->read(m_i2cDevAddress, buffer);
  }
  return status;
}

void Imu::updateAccel() {
  Fw::Buffer buffer;
  U8 data[IMU_MAX_DATA_SIZE_BYTES];

  Gnc::Vector vector;

  buffer.setData(data);
  buffer.setSize(IMU_MAX_DATA_SIZE_BYTES);

  Drv::I2cStatus statusAccelerate =
      readRegisterBlock(IMU_RAW_ACCEL_ADDR, buffer);

  if (statusAccelerate == Drv::I2cStatus::I2C_OK) {
    static_assert(IMU_MAX_DATA_SIZE_BYTES >= 6,
                  "IMU data must be at least 6 bytes or more");
    m_accel.setstatus(Svc::MeasurementStatus::OK);

    // Default full scale range is set to +/- 2g
    vector[0] = static_cast<F32>(static_cast<I16>(data[0] << 8 | data[1]));
    vector[1] = static_cast<F32>(static_cast<I16>(data[2] << 8 | data[3]));
    vector[2] = static_cast<F32>(static_cast<I16>(data[4] << 8 | data[5]));

    // Convert raw data to usable units, need to divide the raw values by
    // 16384 for a range of +-2g
    vector[0] = vector[0] / accelScaleFactor;
    vector[1] = vector[1] / accelScaleFactor;
    vector[2] = vector[2] / accelScaleFactor;

    m_accel.setvector(vector);
    m_accel.settime(this->getTime());

    this->tlmWrite_accelerometer(m_accel.getvector(), m_accel.gettime());
  } else {
    this->log_WARNING_HI_TelemetryError(statusAccelerate);
    m_accel.setstatus(Svc::MeasurementStatus::FAILURE);
  }
}

void Imu::updateGyro() {
  Fw::Buffer buffer;
  U8 data[IMU_MAX_DATA_SIZE_BYTES];
  Vector vector;

  buffer.setData(data);
  buffer.setSize(IMU_MAX_DATA_SIZE_BYTES);

  Drv::I2cStatus statusGyro = readRegisterBlock(IMU_RAW_GYRO_ADDR, buffer);

  if (statusGyro == Drv::I2cStatus::I2C_OK) {
    static_assert(IMU_MAX_DATA_SIZE_BYTES >= 6,
                  "IMU data must be at least 6 bytes or more");
    m_gyro.setstatus(Svc::MeasurementStatus::OK);

    // Default full scale range is set to +/- 250 deg/sec
    vector[0] = static_cast<F32>(static_cast<I16>(data[0] << 8 | data[1]));
    vector[1] = static_cast<F32>(static_cast<I16>(data[2] << 8 | data[3]));
    vector[2] = static_cast<F32>(static_cast<I16>(data[4] << 8 | data[5]));

    // Convert raw data to usable units, need to divide the raw values by
    // 131 for a range of +-250 deg/s
    vector[0] = vector[0] / gyroScaleFactor;
    vector[1] = vector[1] / gyroScaleFactor;
    vector[2] = vector[2] / gyroScaleFactor;

    m_gyro.setvector(vector);
    m_gyro.settime(this->getTime());

    this->tlmWrite_gyroscope(m_gyro.getvector(), m_gyro.gettime());
  } else {
    this->log_WARNING_HI_TelemetryError(statusGyro);
    m_gyro.setstatus(Svc::MeasurementStatus::FAILURE);
  }
}

Drv::I2cStatus Imu ::setupReadRegister(U8 dev_addr, U8 reg) {
  Fw::Buffer buffer;
  U8 data[IMU_REG_SIZE_BYTES];
  buffer.setData(data);
  buffer.setSize(IMU_REG_SIZE_BYTES);

  FW_ASSERT(sizeof(data) > 0);
  data[0] = reg;

  return this->write_out(0, dev_addr, buffer);
}
} // end namespace Gnc
