\page GncImuComponent Gnc::Imu Component
# Gnc::Imu (Passive Component)

## 1. Introduction
'Gnc::Imu' is an F' passive component that collects data from the MPU6050 6-DoF Accelerometer and Gyro. 
The sensor uses an I2C interface to collect data and at AD0 low logic has an address of 0x68. 
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

## 2. Requirements

| Requirement | Description                                                                             | Verification Method |
|-------------|-----------------------------------------------------------------------------------------|---------------------|
| GNC-IMU-001 | The 'Gnc::Imu' component shall produce telemetry of accelerometer data in vector form   | Unit Test           |
| GNC-IMU-002 | The 'Gnc::Imu' component shall produce telemetry of gyroscope data in vector form       | Unit Test           |
| GNC-IMU-003 | The 'Gnc::Imu' component shall be able to communicate with registers using I2C protocol | Inspection          |

## 3. Design 
The diagram below shows the `Imu` component.

<div>
<img src="img/imu.png" width=700/>
</div>

### 3.2. Ports
`Imu` has the following ports: 

| Kind            | Name              | Port Type         | Usage                                              |
|-----------------|-------------------|-------------------|----------------------------------------------------|
| `output`        | `read`            | `Drv.I2c`         | Port that outputs the read data from sensor        |
| `output`        | `write`           | `Drv.I2c`         | Port that outputs written data from sensor         |
| `guarded input` | `getGyroscope`    | `Gnc.ImuDataPort` | Port that gets gyroscope data                      |
| `guarded input` | `getAcceleration` | `Gnc.ImuDataPort` | Port that gets acceleration data                   |
| `guarded input` | `run`             | `Svc.Sched`       | Port that updates accelerometer and gyroscope data |

### 3.3. State
`Imu` maintains the following state:
1. `m_gyro`: An instance of `Gnc::ImuData` that stores the gyroscope data
2. `m_accel`: An instance of `Gnc::ImuData` that stores the acceleration data
3. `m_i2cDevAddress`: A type `U8` that stores the address of the MPU6050 device

### 3.4. Model Configuration
At component instantiation time, `Imu` has a struct defined in `ImuPort.fpp` that consists of a vector, status of the
data, and the time stamp of the data. 

### 3.5. Port Handlers

#### 3.5.1. getAcceleration
The `getAcceleration` port handler does the following: 
1. Sets the measurement status to `STALE`
2. Returns the acceleration data

#### 3.5.2. getGyroscope
The `getGyroscope` port handler does the following:
1. Sets the measurement status to `STALE`
2. Returns the gyroscope data

#### 3.5.3. Run
Calls the `updateAccel` and `updateGyro` helper functions. 

### 3.6. Helper Functions

#### 3.6.1 read 
Returns the read data from the sensor.

#### 3.6.2 write
Returns the written data from the sensor. 

#### 3.6.3 readRegisterBlock
Reads the data from the sensors registers. Returns a status of type `Drv::I2cStatus` if the read was successful or not. 

#### 3.6.4 updateAccel
Reads the data from the accelerometer registers of the sensor. Depending on the status of the read it will either store 
the accelerometer data and emit it as telemetry while setting the measurement status as `OK`, or it will emit an event 
that a telemetry error occurred while setting the measurement status as `FAILURE`. 

#### 3.6.5 updateGyro
Reads the data from the gyroscope registers of the sensor. Depending on the status of the read it will either store the
gyroscope data and emit it as telemetry while setting the measurement status as `OK`, or it will emit an event that 
telemetry error occurred while setting the measurement status as `FAILURE`.

## 4. Change Log

| Date       | Description |
|------------|---|
| 2022-07-19 | Initial Draft |