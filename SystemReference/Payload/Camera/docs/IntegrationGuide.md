# How To Integrate a New Payload Device

F´ projects will typically have a type of payload to fulfill the objective of the spacecraft. 
These projects may follow this guide to get a basic understanding of integrating similar sensors into F´. 
This guide will outline the process of how to integrate a payload.

### Assumptions and Definitions for Guide

This guide makes the following assumptions about the user following the guide and the project setup that will deploy the
component.

1. User is familiar with standard F´ concepts
2. User understands the [application-manager-drive](https://nasa.github.io/fprime/UsersGuide/best/app-man-drv.html) pattern
3. User is familiar with the F´ development process
4. User understands how to integrate components into an F´ topology
5. User has the device's data sheet on hand 
6. User is using a third party library to interact with the sensors of the payload

## Example Hardware Description

Throughout this guide, we will be using a Raspberry Pi Camera Module 2 and one of its component, the camera component, 
as an example for how to integrate a payload system.

## Step 1: Define Payload Requirements

The first step is to define the type of requirements that the payload should fulfill. These requirements should
capture:

1. Subsystem design
2. Ports and Interfaces
3. Events, telemetry, and commands
4. Component behavior
5. Assumptions about the interfaces
6. Overall system integration

Using the Raspberry Pi camera as an example, requirements can be derived by the following procedure. 
We begin by designing the overall system of how images collected by the camera should be handled.
As the collected photos by the camera are of raw image type another component will be needed to handle the processing of the image. 
When the images are done being processed they will then get send to the buffer logger where they will eventually will 
be saved to disk.

We next define the requirements needed for the components. As the system mentioned earlier has two steps of how images
will be handled we will need two components, a camera component and an image processor component. We will only be using 
the camera component to demonstrate how requirements should be defined. In accordance to the Application Manager Driver 
Pattern the camera component would fall under application layer. This is because, the application layer fulfills the 
function of a camera, but it knows nothing of the hardware interface to operate the camera not how to translate the images
into a format that is usable.

The next step is to determine the type of assumptions that are being made in regard to the interface. In the case of the 
Raspberry Pi Camera Module 2, information about the interface is provided by the data sheet given by the manufacturer.
Therefore, the requirements, design and implementation will be based on the implicit assumption that the provided 
information on the data sheet is accurate in order to interact with the camera. From these considerations the example 
requirements derived are shown below:


| Requirement        | Description                                                                                      | Verification Method |
|--------------------|--------------------------------------------------------------------------------------------------|---------------------|
| PAYLOAD-CAMERA-001 | The `Payload::Camera` component shall capture images on command                                  | Inspection          |
| PAYLOAD-CAMERA-002 | The `Payload::Camera` component shall save raw image files to disk                               | Inspection          |
| PAYLOAD-CAMERA-003 | The `Payload::Camera` component shall be able to send raw images to another component to process | Inspection          |
| PAYLOAD-CAMERA-004 | The `Payload::Camera` component shall allow users to be able to set image resolution             | Unit-Test           |
| PAYLOAD-CAMERA-005 | The `Payload::Camera` component shall allows users to be able to set image color format          | Unit-Test           |
| PAYLOAD-CAMERA-006 | The `Payload::Camera` component shall allows users to set exposure time                          | Unit-Test           |


## Step 2: Component Design

The second step to integrating a payload is designing the needed component models. This section will go through each design stage
and each decision behind the design. Projects should implement design decisions based on their component requirements.

The final FPP models can be found here for the 
[camera](https://github.com/fprime-community/fprime-system-reference/blob/devel/SystemReference/Payload/Camera/Camera.fpp)
and [image processor](https://github.com/fprime-community/fprime-system-reference/blob/devel/SystemReference/Payload/ImageProcessor/ImageProcessor.fpp) component. 

### 2.1 Base Component Design

First, a project should choose the basic component properties. Namely, the component must be `active`, `passive` or
`queued`. Properties of the component will be dependent on the complexity of the required behavior of the hardware.

For the camera component example, since the device will be capturing images and will also be sending those image to a 
buffer logger based on a timed command the component will need to be active. 

### 2.2 Port Design

Ports are typically dependent on the outlined requirements, as well as the underlying driver or libraries needed to
interact with the sensor.

The example component does not use a driver, but it does use the buffer logger to send out image data and allocate 
memory. An example port chart can be seen below:

| Kind            | Name              | Port Type       | Usage                                                             |
|-----------------|-------------------|-----------------|-------------------------------------------------------------------|
| `output`        | `process`         | `Fw.BufferSend` | Port that outputs image data to get utilized by another component |
| `output`        | `allocate`        | `Fw.BufferGet`  | Port that allocates memory to hold image buffers                  |
| `output`        | `$save`           | `Fw.BufferSend` | Port that saves image to disk                                     |
**Note:** standard event, telemetry, and command ports are not listed above.

### 2.3 Event Design

Component implementors are free to define whatever events are needed for their project. Typically, Payload components will
emit an error event when the underlying hardware calls fail, or that a command was successfully received.

In this example, the Camera component defines multiple events:

| Event                  | Severity    | Description                              |
|------------------------|-------------|------------------------------------------|
| CameraOpenError        | WARNING_HI  | Error occurred when requesting telemetry |
| CameraSave             | ACTIVITY_LO | Take action command was set to save      | 
| CameraProcess          | ACTIVITY_LO | Take action command was set to process   | 
| ExposureTimeSet        | ACTIVITY_HI | Exposure time was set                    | 
| SetImgConfig           | ACTIVITY_HI | Image configuration was set              |
| InvalidFormatCmd       | WARNING_HI  | Invalid format was given                 |
| InvalidSizeCmd         | WARNING_HI  | Invalid size was given                   | 
| InvalidExposureTimeCmd | WARNING_HI  | Invalid exposure time was give           | 
| BlankFrame             | WARNING_HI  | Camera captured a blank image            |
| SaveError              | WARNING_HI  | Image failed to save                     |
| InvalidTakeCmd         | WARNING_HI  | Invalid take command was given           |


### 2.4 Telemetry Design

Component implementors are free to define whatever telemetry/channels are needed for their project. Typically, payload
components emit telemetry representing the state of the device.

In this example, the camera component defined two types of telemetry that keep track of the cameras action:

| Telemetry Channel | Type | Description                       |
|-------------------|------|-----------------------------------|
| photosTaken       | U32  | Total number of photos captured   |
| commandNum        | U32  | Total number of commands executed |

### 2.5 Commands Design

Component implementors are free to define whatever commands are needed for their project. Some example of commands to consider
would include the type of actions the sensor would need to take.

In this example, the camera had three actions to take and therefore three commands were defined:

| Commands     | Type    | Fields             | Description                                                       |
|--------------|---------|--------------------|-------------------------------------------------------------------|
| ExposureTime | `async` | $time              | Sets the exposure time of an image                                |
| TakeAction   | `async` | cameraAction       | Sets the action of whether the image should be processed or saved |
| ConfigImg    | `async` | resolution, format | Sets the resolution and color format of an image                  |

## Implementation and Testing

Projects will need to implement the port handlers and implementation for their payload components on their own.
Specific implementations will diverge based on hardware and design choices.

In order to help in this process, the example component implementation is available for the 
[camera](https://github.com/fprime-community/fprime-system-reference/blob/devel/SystemReference/Payload/Camera/Camera.cpp)
and [image processor](https://github.com/fprime-community/fprime-system-reference/blob/devel/SystemReference/Payload/ImageProcessor/ImageProcessor.cpp).

## Topology Integration

Once the design and implementation is done, the components can be added to a projects' topology.
Project may attach additional support components as needed.

## Conclusion

In this guide, we have covered the design of a payload components and seen how to integrate it into the
topology. At this point, projects should be able to integrate payload components as needed.
