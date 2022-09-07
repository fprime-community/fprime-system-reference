\page PayloadCameraComponent Payload::Camera Component
# Payload::Camera (Active Component)

## 1. Introduction
'Payload::Camera' is an F' active component that captures images using the Raspberry Pi Camera Module 2. 
The component uses the library opencv to interact with the camera in order to capture images, set image resolution, and
color format.
More information can be found from the [data sheet](https://www.raspberrypi.com/documentation/accessories/camera.html#introducing-the-raspberry-pi-cameras),
provided by the manufacturer.

## 2. Assumptions
1. 

## 3. Requirements
| Requirement        | Description                                                                                                            | Verification Method |
|--------------------|------------------------------------------------------------------------------------------------------------------------|---------------------|
| PAYLOAD-CAMERA-001 | The `Payload::Camera` component shall capture images on command                                                        | Inspection          |
| PAYLOAD-CAMERA-002 | The `Payload::Camera` component shall send raw images to a buffer logger when given a save command                     | Unit-Test           |
| PAYLOAD-CAMERA-003 | The `Payload::Camera` component shall send raw images to another component for processing when given a process command | Unit-Test           |
| PAYLOAD-CAMERA-004 | The `Payload::Camera` component shall allow users to be able to set image resolution                                   | Unit-Test           |
| PAYLOAD-CAMERA-005 | The `Payload::Camera` component shall allows users to be able to set image color format                                | Unit-Test           |
| PAYLOAD-CAMERA-006 | The `Payload::Camera` component shall allows users to set exposure time                                                | Unit-Test           |

## 4. Design
The diagram below shows the `Camera` component.


### 4.1. Component Design

![camera design](img/camera.png)

### 4.2. Ports
`Camera` has the following ports:

| Kind            | Name             | Port Type       | Usage                                                             |
|-----------------|------------------|-----------------|-------------------------------------------------------------------|
| `output`        | `process`        | `Fw.BufferSend` | Port that outputs image data to get utilized by another component |
| `output`        | `allocate`       | `Fw.BufferGet`  | Port that allocates memory to hold image buffers                  |
| `output`        | `save`           | `Fw.BufferSend` | Port that saves image to disk                                     |
**Note:** standard event, telemetry, and command ports are not listed above.


### 4.3. State
`ImageProcessor` maintains the following state:
1. m_cmdCount: `U32` type that keeps track of the number of commands executed 
2. m_photoCount: `U32` type that keeps track of the number of photos captured 
3. m_validCommand: An instance of `bool` that indicates weather a valid command was received or not 
4. m_capture: An instance of `cv::VideoCapture` that stores the captured image frame
5. m_imageFrame: An instance of `cv::Mat` that stores the image matrix

### 4.4. Port Handlers

#### 4.4.1 TakeAction
The `takeAction` port handler does the following:
1. Reads the image frame
2. Determines whether the image gets saved to disk or sent to the buffer logger 

#### 4.4.2 ExposureTime
The `ExposureTime` port handler does the following:
1. Set the exposure time of the camera

#### 4.4.3 ConfigImg
The `ConfigImg` port handler does the following:
1. Sets the resolution of the image
2. Sets the color format of the image

### 4.5. Helper Functions

#### 4.5.1 open
Activates the camera and ensures that it is open.

## 4. Sequence Diagram
