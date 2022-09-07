// ======================================================================
// \title  Imu.hpp
// \author vbai
// \brief  cpp file for Imu test harness implementation class
// ======================================================================

#include "Tester.hpp"
#include <STest/STest/Pick/Pick.hpp>

#define INSTANCE 0
#define MAX_HISTORY_SIZE 100

namespace Gnc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

Tester ::Tester()
    : ImuGTestBase("Tester", MAX_HISTORY_SIZE),
      component("Imu")
{
  this->initComponents();
  this->connectPorts();
}

Tester ::~Tester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester ::testGetAccelTlm(){
  Gnc::ImuData accelData;
  accelData = this->invoke_to_getAcceleration(0);
  ASSERT_EQ(accelData.getstatus(), Svc::MeasurementStatus::STALE);
  for(U32 i = 0; i < 5; i++) {
    this->invoke_to_Run(0, 0);
    Gnc::ImuData newData = this->invoke_to_getAcceleration(0);
    ASSERT_EQ(newData.getstatus(), Svc::MeasurementStatus::OK);
    ASSERT_EQ(component.m_accel.getstatus(), Svc::MeasurementStatus::STALE);
    ASSERT_FALSE(newData.getvector()[0] == accelData.getvector()[0] &&
                 newData.getvector()[1] == accelData.getvector()[1] &&
                 newData.getvector()[2] == accelData.getvector()[2]);
    accelData = newData;
  }
}

void Tester ::testGetGyroTlm(){
  Gnc::ImuData gyroData;
  gyroData = this->invoke_to_getGyroscope(0);
  ASSERT_EQ(gyroData.getstatus(), Svc::MeasurementStatus::STALE);
  for(U32 i = 0; i < 5; i++) {
    this->invoke_to_Run(0, 0);
    Gnc::ImuData newData = this->invoke_to_getGyroscope(0);
    ASSERT_EQ(newData.getstatus(), Svc::MeasurementStatus::OK);
    ASSERT_EQ(component.m_gyro.getstatus(), Svc::MeasurementStatus::STALE);
    ASSERT_FALSE(newData.getvector()[0] == gyroData.getvector()[0] &&
                 newData.getvector()[1] == gyroData.getvector()[1] &&
                 newData.getvector()[2] == gyroData.getvector()[2]);
    gyroData = newData;
  }
}

void Tester::testTlmError() {
  component.m_setup = true;
  this->m_readStatus = Drv::I2cStatus::I2C_OTHER_ERR;
  this->m_writeStatus = Drv::I2cStatus::I2C_OTHER_ERR;
  this->invoke_to_Run(0,0);
  ASSERT_EVENTS_SIZE(2);
  ASSERT_EVENTS_TelemetryError_SIZE(2);
  ASSERT_EQ(invoke_to_getAcceleration(0).getstatus(),Svc::MeasurementStatus::FAILURE);
  ASSERT_EVENTS_TelemetryError(0, m_readStatus);
  ASSERT_EQ(invoke_to_getGyroscope(0).getstatus(),Svc::MeasurementStatus::FAILURE);
  ASSERT_EVENTS_TelemetryError(0, m_readStatus);
}

void Tester::testSetupError() {
  component.setup(0x80);
  this->m_writeStatus = Drv::I2cStatus::I2C_WRITE_ERR;
  this->invoke_to_Run(0,0);
  ASSERT_EVENTS_PowerModeError_SIZE(1);
  ASSERT_EVENTS_PowerModeError(0, m_writeStatus);
  ASSERT_EVENTS_SetUpConfigError_SIZE(2);
  ASSERT_EVENTS_SetUpConfigError(0, m_writeStatus);
}
// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

Drv::I2cStatus Tester ::from_read_handler(const NATIVE_INT_TYPE portNum,
                                          U32 addr, Fw::Buffer &serBuffer)
{
  this->pushFromPortEntry_read(addr, serBuffer);
  if (this->m_readStatus == Drv::I2cStatus::I2C_OK) {
    // Fill buffer with random data
    U8 *const data = (U8*) serBuffer.getData();
    const U32 size = serBuffer.getSize();
    FW_ASSERT(size <= Imu::IMU_MAX_DATA_SIZE_BYTES);
    for (U32 i = 0; i < size; ++i) {
      data[i] = STest::Pick::any();
    }
  }
  return this->m_readStatus;
}

Drv::I2cStatus Tester ::from_write_handler(const NATIVE_INT_TYPE portNum,
                                           U32 addr, Fw::Buffer &serBuffer) {
  this->pushFromPortEntry_write(addr, serBuffer);
  const U32 size = serBuffer.getSize();
  FW_ASSERT(size > 0);
  U8 *const data = (U8*) serBuffer.getData();
  FW_ASSERT(data[0] == Imu::IMU_RAW_ACCEL_ADDR || Imu::IMU_RAW_GYRO_ADDR);
  return this->m_writeStatus;
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void Tester ::connectPorts() {

  // Run
  this->connect_to_Run(0, this->component.get_Run_InputPort(0));

  // getAcceleration
  this->connect_to_getAcceleration(
      0, this->component.get_getAcceleration_InputPort(0));

  // getGyroscope
  this->connect_to_getGyroscope(0,
                                this->component.get_getGyroscope_InputPort(0));

  // Log
  this->component.set_Log_OutputPort(0, this->get_from_Log(0));

  // LogText
  this->component.set_LogText_OutputPort(0, this->get_from_LogText(0));

  // Time
  this->component.set_Time_OutputPort(0, this->get_from_Time(0));

  // Tlm
  this->component.set_Tlm_OutputPort(0, this->get_from_Tlm(0));

  // read
  this->component.set_read_OutputPort(0, this->get_from_read(0));

  // write
  this->component.set_write_OutputPort(0, this->get_from_write(0));
}

void Tester ::initComponents() {
  this->init();
  this->component.init(INSTANCE);
}

} // end namespace Gnc
