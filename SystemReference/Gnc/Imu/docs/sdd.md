\page GncImuComponent Gnc::Imu Component
# Gnc::Imu (Passive Component)

## 1. Introduction
'Gnc::Imu' is an F' passive component that collects data from the MPU6050 6-DoF Accelerometer and Gyro. 
The sensor uses an I2C interface to collect data and at AD0 low logic has an address of 0x68. 
Since there are different power modes for the MPU6050, in order for the sensor to begin collecting
data it needs to be "awakened" by the entry of 0 at the "Power Management 1" register at 0x6B. 
I2C data bytes are defined to be 8-bits long, where the 8th bit represents the read/write bit that 
indicates whether data is being received or written. 
Registers 43-48 (in hexadecimal) are used to store the most recent gyroscope measurement. 
The output for the x, y, and z values of the gyroscope are stored as 16-bit 2's complement values.
The gyro sensors may be digitally programmed to ±250, ±500, ±1000, or ±2000 degrees per second (dps).
For the accelerometer, registers 3B-40 (in hexadecimal) store the most recent accelerometer measurements.
The output for the x, y, and z values of the accelerometer are stored as 16-bit 2's complement values.
The full scale range of the digital output from the accelerometer can be adjusted to ±2g, ±4g, ±8g, or ±16g.
More information can be found from the [data sheet](https://learn.adafruit.com/mpu6050-6-dof-accelerometer-and-gyro/downloads), 
provided by the manufacturer.

## 2. Assumptions
The design of `Imu` assumes the following:
1. Data collected by the Imu arrives through a pull interface


## 3. Requirements
| Requirement ID  | Description                                                                                      | Verification Method |
|-----------------|--------------------------------------------------------------------------------------------------|---------------------|
| GNC-IMU-001     | The Gnc::Imu component shall produce telemetry of accelerometer data at 1Hz                      | Unit Test           |
| GNC-IMU-002     | The Gnc::Imu component shall produce telemetry of gyroscope data at 1Hz                          | Unit Test           |
| GNC-IMU-003     | The Gnc::Imu component shall be able to communicate with the MPU6050 over I2C                    | Inspection          |
| GNC-IMU-004     | The Gnc::IMu component shall produce the latest gyroscope and accelerometer data via a port call | Unit Test           |

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

### 4.3. State
`Imu` maintains the following state:
1. `m_gyro`: An instance of `Gnc::ImuData` that stores the latest gyroscope data
2. `m_accel`: An instance of `Gnc::ImuData` that stores the latest acceleration data
3. `m_i2cDevAddress`: A type `U8` that stores the address of the MPU6050 sensor
4. `m_setup`: An instance of `bool` that indicates if sensor has been properly activated or not

### 4.4. Port Handlers

#### 4.4.1. getAcceleration
The `getAcceleration` port handler does the following: 
1. Sets the measurement status to `STALE`
2. Returns the acceleration data

#### 4.4.2. getGyroscope
The `getGyroscope` port handler does the following:
1. Sets the measurement status to `STALE`
2. Returns the gyroscope data

#### 4.4.3. Run
Ensures that the sensor has been properly setup and calls the `updateAccel` and `updateGyro` helper functions. 

### 4.5. Helper Functions

#### 4.5.1 read 
Returns the read data from the sensor.

#### 4.5.2 setupReadRegister
Returns the written data from the sensor in order for the data to be read. 

#### 4.5.3 readRegisterBlock
Reads the data from the sensors registers. Returns a status of type `Drv::I2cStatus` if the read was successful or not. 

#### 4.5.4 updateAccel
Reads the data from the accelerometer registers of the sensor. Depending on the status of the read it will either store 
the accelerometer data and emit it as telemetry while setting the measurement status as `OK`, or it will emit an event 
that a telemetry error occurred while setting the measurement status as `FAILURE`. 

#### 4.5.5 updateGyro
Reads the data from the gyroscope registers of the sensor. Depending on the status of the read it will either store the
gyroscope data and emit it as telemetry while setting the measurement status as `OK`, or it will emit an event that 
telemetry error occurred while setting the measurement status as `FAILURE`.

#### 4.5.6 powerOn
Activates the sensor, by setting the Power Management 1 register to 0. 

## 5. Sequence Diagram

