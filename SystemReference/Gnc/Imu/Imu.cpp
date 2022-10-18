// ======================================================================
// \title  Imu.cpp
// \author vbai
// \brief  cpp file for Imu component implementation class
// ======================================================================

#include <SystemReference/Gnc/Imu/Imu.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace Gnc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

Imu ::Imu(const char* const compName) : ImuComponentBase(compName), m_power(PowerState::OFF) {}

void Imu ::init(const NATIVE_INT_TYPE instance) {
    ImuComponentBase::init(instance);
}

void Imu ::setup(I2cDevAddr::T devAddress) {
    m_i2cDevAddress = devAddress;
}

Imu ::~Imu() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void Imu ::Run_handler(const NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context) {
    if (m_power == PowerState::ON) {
        updateAccel();
        updateGyro();
    }
}

void Imu ::PowerSwitch_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq, PowerState powerState) {
    power(powerState);
    this->log_ACTIVITY_HI_PowerStatus(powerState);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

// ----------------------------------------------------------------------
// Helper Functions
// ----------------------------------------------------------------------

Drv::I2cStatus Imu ::setupReadRegister(U8 reg) {
    Fw::Buffer buffer(&reg, sizeof reg);
    return this->write_out(0, m_i2cDevAddress, buffer);
}

Drv::I2cStatus Imu ::readRegisterBlock(U8 startRegister, Fw::Buffer& buffer) {
    Drv::I2cStatus status;
    status = this->setupReadRegister(startRegister);
    if (status == Drv::I2cStatus::I2C_OK) {
        status = this->read_out(0, m_i2cDevAddress, buffer);
    }
    return status;
}

Gnc::Vector Imu ::deserializeVector(Fw::Buffer& buffer, F32 scaleFactor) {
    Gnc::Vector vector;
    I16 value;
    FW_ASSERT(buffer.getSize() >= 6, buffer.getSize());
    FW_ASSERT(buffer.getData() != nullptr);
    // Data is big-endian as is fprime internal storage so we can use the built-in buffer deserialization
    Fw::SerializeBufferBase& deserializeHelper = buffer.getSerializeRepr();
    deserializeHelper.setBuffLen(buffer.getSize()); // Inform the helper what size we have available
    FW_ASSERT(deserializeHelper.deserialize(value) == Fw::FW_SERIALIZE_OK);
    vector[0] = static_cast<F32>(value) / scaleFactor;

    FW_ASSERT(deserializeHelper.deserialize(value) == Fw::FW_SERIALIZE_OK);
    vector[1] = static_cast<F32>(value) / scaleFactor;

    FW_ASSERT(deserializeHelper.deserialize(value) == Fw::FW_SERIALIZE_OK);
    vector[2] = static_cast<F32>(value) / scaleFactor;
    return vector;
}

void Imu ::config() {
    U8 data[IMU_REG_SIZE_BYTES * 2];
    Fw::Buffer buffer(data, sizeof data);

    // Set gyro range to +-250 deg/s
    data[0] = GYRO_CONFIG_ADDR;
    data[1] = 0;

    Drv::I2cStatus status = write_out(0, m_i2cDevAddress, buffer);
    if (status != Drv::I2cStatus::I2C_OK) {
        this->log_WARNING_HI_SetUpConfigError(status);
    }

    // Set accel range to +- 2g
    data[0] = ACCEL_CONFIG_ADDR;
    data[1] = 0;
    status =this-> write_out(0, m_i2cDevAddress, buffer);

    if (status != Drv::I2cStatus::I2C_OK) {
        this->log_WARNING_HI_SetUpConfigError(status);
    }
}

void Imu ::power(PowerState powerState) {
    U8 data[IMU_REG_SIZE_BYTES * 2];
    Fw::Buffer buffer(data, sizeof data);

    // Check if already on/off
    if (powerState.e == m_power) {
        return;
    }

    data[0] = POWER_MGMT_ADDR;
    data[1] = (powerState.e == PowerState::ON) ? POWER_ON_VALUE : POWER_OFF_VALUE;

    Drv::I2cStatus status = this->write_out(0, m_i2cDevAddress, buffer);
    if (status != Drv::I2cStatus::I2C_OK) {
        this->log_WARNING_HI_PowerModeError(status);
    } else {
        m_power = powerState.e;
        // Must configure at power on
        if (m_power == PowerState::ON) {
            config();
        }
    }
}

void Imu ::updateAccel() {
    U8 data[IMU_MAX_DATA_SIZE_BYTES];
    Fw::Buffer buffer(data, sizeof data);

    // Read a block of registers from the IMU at the accelerometer's address
    Drv::I2cStatus status = this->readRegisterBlock(IMU_RAW_ACCEL_ADDR, buffer);

    // Check a successful read of 6 bytes before processing data
    if ((status == Drv::I2cStatus::I2C_OK) && (buffer.getSize() == 6) && (buffer.getData() != nullptr)) {
        Gnc::Vector vector = deserializeVector(buffer, accelScaleFactor);
        this->tlmWrite_accelerometer(vector);
    } else {
        this->log_WARNING_HI_TelemetryError(status);
    }
}

void Imu ::updateGyro() {
    U8 data[IMU_MAX_DATA_SIZE_BYTES];
    Fw::Buffer buffer(data, sizeof data);

    // Read a block of registers from the IMU at the gyroscope's address
    Drv::I2cStatus status = this->readRegisterBlock(IMU_RAW_GYRO_ADDR, buffer);

    // Check a successful read of 6 bytes before processing data
    if ((status == Drv::I2cStatus::I2C_OK) && (buffer.getSize() == 6) && (buffer.getData() != nullptr)) {
        Gnc::Vector vector = deserializeVector(buffer, gyroScaleFactor);
        this->tlmWrite_gyroscope(vector);
    } else {
        this->log_WARNING_HI_TelemetryError(status);
    }
}
}  // end namespace Gnc
