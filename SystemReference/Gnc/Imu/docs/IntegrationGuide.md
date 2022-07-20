# Integration Guide 

The purpose of this guide is to lay out the general process to set up and integrate sensors into a project. 
This guide will walk through each step in the process. The steps to integrate a sensor are: 

1. Define component requirements. 
2. Match driver interface of sensor to F' driver. 
3. Define data extraction and storage process 
4. Outline how data should be processed
5. Define Commands, Parameters and Events 
6. Write FPP model 
7. Implement design through code 

### Define component requirements
The first step is to define the type of requirements that the sensor should fulfill. Let us use the MPU-6050 as an example. 
To integrate a MPU-6050 into the project, we must specify the required communication interface for the sensor, the 
type of data we want from the sensor and how the project will use the data. 

## Match driver interface of sensor to F' driver 
The second step of the integration process is to match the driver's between the sensor and F'. Using the MPU-6050 as an 
example again, the MPU-6050 uses an I2C interface, therefore its corresponding matching driver from F' is the LinuxI2C 
driver that can be found in the `Drv` directory.  

## Define data extraction and storage process 
The third step is to define how data should be read and store in order to fulfill the requirements defined in step 1.
For instance, in the case of a MPU-6050, the sensor is a six degree of freedom accelerometer and gyroscope which
generates acceleration and rotational data. Furthermore, since the sensor collects data based on the x, y, and z axis,
the collected data can be stored as a vector. 

## Outline how data should be processed
The fourth step is to design how the data collected from the sensor should be processed, and whether the processed data
should be sent to the application or through telemetry channels. This is also the step where it needs to be determined 
if all the data from the sensor is needed or only a sample is required. For instance if only a sample of the data
is needed a pull design may be more efficient than a push design. 

## Define Commands, Parameters and Events 
In this step, specify the types of commands, events, and parameters that will be needed to execute the behavior that
is wanted from the project. Some event examples include: error occurrence, connection, and disconnections. Whereas for commands 
and parameters some examples include: power on, power off, and configuration for commands.  

## Write FPP model 
Create an FPP model that reflects the features outlined and defined in steps 1-4. 

## Implement design through code
Implement the design through C++ and the data sheet provided by the manufacturer. 