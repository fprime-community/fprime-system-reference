# How To Integrate a New GNC Device

F´ projects will typically have a set of GNC sensors in order to control the movement of the spacecraft. These projects may
follow this guide to get a basic understanding of integrating similar sensors into F´. This guide will outline the process
integrate Gnc sensors. 

## Who Should Follow This Guide? 

Projects who wish to use the standard F driver models [here](https://github.com/nasa/fprime/tree/0ae2321bb552174ce607075b1283029d6d75a6d6/Drv)
to communicate with GNC sensors.

### Assumptions and Definitions for Guide

This guide makes the following assumptions about the user following the guide and the project setup that will deploy the
component.

1. User is familiar with standard F´ concepts
2. User understands the [application-manager-drive](https://nasa.github.io/fprime/UsersGuide/best/app-man-drv.html) pattern
3. User is familiar with the F´ development process
4. User understands how to integrate components into an F´ topology
5. User has the device's data sheet on hand

This guide also uses the following terminology:
- GNC: Guidance, navigation and controls 
- IMU: Inertial measurement unit

## Example Hardware Description 

Throughout this guide, we will be using an MPU6050 IMU sensor and its I2C interface as an example for how to integrate IMU sensors.
We are primarily interested in the accelerometer and gyroscope data that the sensor provides.

## Step 1: Define Component Requirements

The first step is to define the type of requirements that the sensor should fulfill. These requirements should 
capture: 

1. Ports and Interfaces
2. Events, telemetry, and commands
3. Component behavior
4. Assumptions about the interfaces 
5. Overall system integration 

Using the MPU6050 IMU sensor as an example, requirements can be derived by the following process. 
We begin by determining that in accordance to the Application Manager Driver Pattern which layer the component would fall under.
In the case of the MPU6050 IMU sensor it would be integrated into the system project at the manager level.
This is because the IMU component is defined to only know its current orientation and acceleration, and the driver it should
talk to in order to collect data. Besides, these aspects the IMU component does not know how the data will be 
used by the GNC application or even if the collected data will be used at all. The next step is to determine the type of 
assumptions that are being made in regard to the interface. In the case of the MPU6050 IMU sensor, information about the interface
is provided by the data sheet given by the manufacturer. Therefore, the requirements, design and implementation will be 
based on the implicit assumption that the provided information on the data sheet is accurate in order to interact with
the sensor. From these considerations the example requirements derived are shown below:

| Requirement ID  | Description                                                                                      | Verification Method |
|-----------------|--------------------------------------------------------------------------------------------------|---------------------|
| GNC-IMU-001     | The Gnc::Imu component shall produce telemetry of accelerometer data at 1Hz                      | Unit Test           |
| GNC-IMU-002     | The Gnc::Imu component shall produce telemetry of gyroscope data at 1Hz                          | Unit Test           |
| GNC-IMU-003     | The Gnc::Imu component shall be able to communicate with the MPU6050 over I2C                    | Inspection          |
| GNC-IMU-004     | The Gnc::IMu component shall produce the latest gyroscope and accelerometer data via a port call | Unit Test           |


## Step 2: Component Design 

The second step to integrating a GNC sensor is designing the component model. This section will go through each design stage
and each decision behind the design. Projects should implement design decisions based on their component requirements. 

The final FPP model for the MPU6050 IMU component can be found [here](https://github.com/fprime-community/fprime-system-reference/blob/devel/SystemReference/Gnc/Imu/Imu.fpp).

### 2.1 Base Component Design 

First, a project should choose the basic component properties. Namely, the component must be `active`, `passive` or
`queued`. Properties of the component will be dependent on the complexity of the required behavior of the hardware. 

For the MPU6050 example, since the device will only be reading and writing accelerometer and gyroscope data, which
consists of a few I2C bytes, a passive component is sufficient as the transactions will fit within the larger rate
group driving the sensor.

### 2.2 Port Design 

Ports are typically dependent on the outlined requirements, as well as the underlying driver of the sensor. 
Furthermore, projects often use rate groups to drive Gnc sensor reading and take advantage of telemetry, events,
parameters, and commands.

The example component uses ['Drv::LinuxI2cDriver'](https://github.com/nasa/fprime/tree/master/Drv/LinuxI2cDriver)
which uses the `Drv.I2c` for basic reads and writes. The component also defines a `Gnc::ImuDataPort` port for
returning telemetry to other components. A rate group port is used for the 1Hz timing, and standard F´ports
for events, channels, and telemetry ar needed. An example port chart can be seen below: 

| Kind            | Name              | Port Type         | Usage                                              |
|-----------------|-------------------|-------------------|----------------------------------------------------|
| `output`        | `read`            | `Drv.I2c`         | Port that outputs the read data from sensor        |
| `output`        | `write`           | `Drv.I2c`         | Port that outputs written data from sensor         |
| `guarded input` | `getGyroscope`    | `Gnc.ImuDataPort` | Port that returns gyroscope data                   |
| `guarded input` | `getAcceleration` | `Gnc.ImuDataPort` | Port that returns acceleration data                |
| `guarded input` | `run`             | `Svc.Sched`       | Port that updates accelerometer and gyroscope data |
**Note:** standard event, telemetry, and command ports are not listed above.

### 2.3 Event Design 

Component implementors are free to define whatever events are needed for their project. Typically, GNC components will 
emit an error event when the underlying hardware calls fail.

In this example, the IMU component will define one event:

| Event          | Severity   | Description                              |
|----------------|------------|------------------------------------------|
| TelemetryError | WARNING_HI | Error occurred when requesting telemetry |

### 2.4 Telemetry Design

Component implementors are free to define whatever telemetry/channels are needed for their project. Typically, Gnc
components emit telemetry representing the state of the given GNC sensor.

In this example, the IMU component will define two telemetry channels one for each of the sensor readings taken:

| Telemetry Channel | Type   | Description                             |
|-------------------|--------|-----------------------------------------|
| accelerometer     | Vector | X, Y, Z acceleration from accelerometer |
| gyroscope         | Vector | X, Y, Z degrees from gyroscope          |

### 2.5 Commands Design

Component implementors are free to define whatever commands are needed for their project. Some example of commands to consider 
would include enable and disable of the sensor.

In this example, no commands were defined.

## Implementation and Testing

Projects will need to implement the port handlers and implementation for their GNC components on their own.
Specific implementations will diverge based on hardware and design choices.

In order to help in this process, the example component implementation is available for 
[reference](https://github.com/fprime-community/fprime-system-reference/blob/devel/SystemReference/Gnc/Imu/Imu.cpp).


## Topology Integration

Once the design and implementation is done, the component can be added to a projects' topology.
Project may attach additional support components as needed.

## Conclusion

In this guide, we have covered the design of new Gnc components and seen how to integrate it into the
topology. At this point, projects should be able to integrate GNC components as needed.
