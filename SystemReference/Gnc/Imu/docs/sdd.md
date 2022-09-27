\page GncImuComponent Gnc::Imu Component
# Gnc::Imu (Passive Component)

## 1. Introduction
`Gnc::Imu` is an F' passive component that collects data from the MPU6050 6-DoF Accelerometer and Gyro. 

### 1.1. Hardware Overview

**I2C interface:**
The sensor reports data via an I2C interface.
I2C data bytes are eight bits wide. The high-order bit is a 
read/write bit indicating whether data is being read or written. 

**Power management:**
The MPU6050 has several power modes. In order for the sensor to begin collecting
data it needs to be "awakened" by the entry of 0 at the "Power Management 1" register at 0x6B. 

**Accelerometer:**
Hardware registers 0x3B through 0x40 store the most recent accelerometer measurement
as a triple of coordinates (x, y, z) in units of g (gravitational constant).
The full scale range of the digital output from the accelerometer can be set to ±2g, ±4g, ±8g, or ±16g.
Each coordinate is stored as a scaled 16-bit signed integer.
The scale factor is 32768 / _m_, where _m_ is the maximum acceleration (2, 4, 8, or 16).

**Gyroscope:**
Hardware registers 0x43 through 0x48 store the most recent gyroscope measurement
as a triple of coordinates (x, y, z) in units of deg/s (degrees per second).
The gyro sensors may be digitally programmed to ±250, ±500, ±1000, or ±2000/s.
Each coordinate is stored as a scaled 16-bit signed integer.
The scale factor is 32768 / _m_, where _m_ is the maximum output (250, 500, 100, or 2000).

**Data sheet:** For more details, see the [manufacturer's data 
sheet](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf).

### 1.2. Component Overview

`Gnc::Imu` provides a run port that periodically requests updated accelerometer and
gyroscope data from the hardware.
It provides ports for getting the latest accelerometer and gyroscope data.

`Gnc::Imu` is currently hard-coded to use the following hardware configuration:

| Hardware Output | Max Value | Scale Factor |
|-----------------|-----------|--------------|
| Accelerometer   | 2g        | 16384.0      |
| Gyroscope       | 250 deg/s | 131.072      |

## 2. Assumptions
None.

## 3. Requirements
| Requirement ID | Description                                                                                      | Verification Method |
|----------------|--------------------------------------------------------------------------------------------------|---------------------|
| GNC-IMU-001    | The `Gnc::Imu` component shall produce telemetry of accelerometer data when its run port is invoked | Unit Test           |
| GNC-IMU-002    | The `Gnc::Imu` component shall produce telemetry of gyroscope data when its run port is invoked    | Unit Test           |
| GNC-IMU-003    | The `Gnc::Imu` component shall be able to communicate with the MPU6050 over I2C                    | Inspection          |
| GNC-IMU-004    | The `Gnc::Imu` component shall produce the latest gyroscope and accelerometer data via a port call | Unit Test           |
| GNC-IMU-005    | The `Gnc::Imu` component shall support power on and power off commands                             | Unit Test           |

## 4. Design 

### 4.1. Component Diagram
The diagram below shows the `Imu` component.

![IMU Design](./img/imu.png)

### 4.2. Ports
`Imu` has the following ports: 

| Kind            | Name              | Port Type         | Usage                                              |
|-----------------|-------------------|-------------------|----------------------------------------------------|
| `output`        | `read`            | `Drv.I2c`         | Port that outputs the read data from sensor        |
| `output`        | `write`           | `Drv.I2c`         | Port that outputs written data from sensor         |
| `guarded input` | `getGyroscope`    | `Gnc.ImuDataPort` | Port that returns gyroscope data                   |
| `guarded input` | `getAcceleration` | `Gnc.ImuDataPort` | Port that returns acceleration data                |
| `guarded input` | `run`             | `Svc.Sched`       | Port that updates accelerometer and gyroscope data |

### Externally Defined Types

`Imu` uses the following externally defined types:

1. [`ImuData`](../../ImuPort/ImuPort.fpp) is an FPP struct that defines
the type of IMU data.

### Types

`Imu` defines the following type:

1. [`I2cDevAddr`](../Imu.hpp) is a C++ enumeration that defines the type
of an I2C device address.
The I2C bus uses the device address to identify the device.

### 4.3. State
`Imu` maintains the following state:
1. `m_gyro`: An instance of `Gnc::ImuData` that stores the latest gyroscope data
2. `m_accel`: An instance of `Gnc::ImuData` that stores the latest acceleration data
3. `m_i2cDevAddress`: A type `U8` that stores the address of the MPU6050 sensor
4. `m_setup`: An instance of `bool` that indicates whether the sensor has been activated

### 4.4. Runtime Configuration
At startup, the F Prime software must call the `setup` method of the
`Imu` object.
```
void setup(I2cDevAddr devAddress);
```
`devAddress` is the I2C device address.
The value should correspond to the configuration of the hardware.

### 4.5. Port Handlers

#### 4.5.1. getAcceleration
The `getAcceleration` port handler does the following: 
1. Sets the measurement status to `STALE`
2. Returns the acceleration data

#### 4.5.2. getGyroscope
The `getGyroscope` port handler does the following:
1. Sets the measurement status to `STALE`
2. Returns the gyroscope data

#### 4.5.3. Run
Ensures that the sensor has been properly setup and calls the `updateAccel` and `updateGyro` helper functions. 

### 4.6. Helper Functions

#### 4.6.1. read 
Returns the read data from the sensor.

#### 4.6.2. setupReadRegister
Returns the written data from the sensor in order for the data to be read. 

#### 4.6.3. readRegisterBlock
Reads the data from the sensors registers. Returns a status of type `Drv::I2cStatus` if the read was successful or not. 

#### 4.6.4. updateAccel
Reads the data from the accelerometer registers of the sensor. Depending on the status of the read it will either store 
the accelerometer data and emit it as telemetry while setting the measurement status as `OK`, or it will emit an event 
that a telemetry error occurred while setting the measurement status as `FAILURE`. 

#### 4.6.5. updateGyro
Reads the data from the gyroscope registers of the sensor. Depending on the status of the read it will either store the
gyroscope data and emit it as telemetry while setting the measurement status as `OK`, or it will emit an event that 
telemetry error occurred while setting the measurement status as `FAILURE`.

#### 4.6.6. powerOn
Activates the sensor, by setting the Power Management 1 register to 0. 

