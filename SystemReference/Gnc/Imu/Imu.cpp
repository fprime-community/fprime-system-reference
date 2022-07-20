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
    : ImuComponentBase(compName) {
  this->m_i2cDevAddress = I2C_DEV0_ADDR;

}

void Imu ::init(const NATIVE_INT_TYPE instance) {
  ImuComponentBase::init(instance);
}

Imu ::~Imu() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void Imu ::Run_handler(const NATIVE_INT_TYPE portNum,
                       NATIVE_UINT_TYPE context) {
    updateAccel();
    updateGyro();
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

// ----------------------------------------------------------------------
// Helper Functions
// ----------------------------------------------------------------------
// Reads in buffer data
Drv::I2cStatus Imu ::read(U8 dev_addr, Fw::Buffer &buffer) {
  return this->read_out(0, dev_addr, buffer);
}

// Reads the next block of address
Drv::I2cStatus Imu::readRegisterBlock(U8 registerAdd, Fw::Buffer &buffer){
  Drv::I2cStatus status;
  status = this->write(m_i2cDevAddress, registerAdd);

  if (status == Drv::I2cStatus::I2C_OK) {
    status = this->read(m_i2cDevAddress, buffer);
  }
  return status;
}

void Imu::updateAccel(){
  Fw::Buffer buffer;
  U8 data[IMU_MAX_DATA_SIZE];
  Gnc::Vector vector;

  buffer.setData(data);
  buffer.setSize(IMU_MAX_DATA_SIZE);

  Drv::I2cStatus statusAccelerate = readRegisterBlock(IMU_RAW_ACCEL_ADDR, buffer);

  if (statusAccelerate == Drv::I2cStatus::I2C_OK) {
    FW_ASSERT(IMU_MAX_DATA_SIZE >= 6);
    m_accel.setstatus(Svc::MeasurementStatus::OK);

    vector[0] = static_cast<F32>((((U16)data[0]) << 8) | ((U16)data[1]));
    vector[1] = static_cast<F32>((((U16)data[2]) << 8) | ((U16)data[3]));
    vector[2] = static_cast<F32>((((U16)data[4]) << 8) | ((U16)data[5]));
    m_accel.setvector(vector);
    m_accel.settime(this->getTime());

    this->tlmWrite_accelerometer(m_accel.getvector(), m_accel.gettime());
  } else {
    this->log_WARNING_HI_TelemetryError(statusAccelerate);
    m_accel.setstatus(Svc::MeasurementStatus::FAILURE);
  }
}

void Imu::updateGyro(){
  Fw::Buffer buffer;
  U8 data[IMU_MAX_DATA_SIZE];
  Vector vector;

  buffer.setData(data);
  buffer.setSize(IMU_MAX_DATA_SIZE);

  Drv::I2cStatus statusGyro = readRegisterBlock(IMU_RAW_GYRO_ADDR, buffer);

  if (statusGyro == Drv::I2cStatus::I2C_OK) {
    FW_ASSERT(IMU_MAX_DATA_SIZE >= 6);

    m_gyro.setstatus(Svc::MeasurementStatus::OK);
    vector[0] =  static_cast<F32>((((U16)data[0]) << 8) | ((U16)data[1]));
    vector[1] = static_cast<F32>((((U16)data[2]) << 8) | ((U16)data[3]));
    vector[2] = static_cast<F32>((((U16)data[4]) << 8) | ((U16)data[5]));
    m_gyro.setvector(vector);
    m_gyro.settime(this->getTime());

    this->tlmWrite_accelerometer(m_gyro.getvector(), m_gyro.gettime());
  } else {
    this->log_WARNING_HI_TelemetryError(statusGyro);
    m_gyro.setstatus(Svc::MeasurementStatus::FAILURE);
  }
}

Drv::I2cStatus Imu ::write(U8 dev_addr, U8 reg) {
  Fw::Buffer buffer;
  U8 data[IMU_REG_SIZE];
  buffer.setData(data);
  buffer.setSize(IMU_REG_SIZE);

  FW_ASSERT(sizeof(data) > 0);
  data[0] = reg;

  return this->write_out(0, dev_addr, buffer);
}
} // end namespace Gnc
