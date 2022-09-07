module Payload {

    enum ImgResolution { SIZE_640x480 = 0 , SIZE_800x600 = 1 }
    enum CameraAction { SAVE = 0, PROCESS = 1 }

    @ Component to capture raw images
    active component Camera {
        # ----------------------------------------------------------------------
        # General ports
        # ----------------------------------------------------------------------

        @ Sends photo to another component to get processed
        output port process: ImageData

        @ Allocates memory to hold photo buffers
        output port allocate: Fw.BufferGet

        @ Deallocates memory that holds photo buffers
        output port deallocate: Fw.BufferSend

        @ Save photo to disk, send image to buffer logger
        output port $save: Fw.BufferSend

        # ----------------------------------------------------------------------
        # Special ports
        # ----------------------------------------------------------------------

        @ Command receive
        command recv port cmdIn

        @ Command registration
        command reg port cmdRegOut

        @ Command response
        command resp port cmdResponseOut

        @ Port for emitting events
        event port Log

        @ Port for emitting text events
        text event port LogText

        @ Port for getting the time
        time get port Time

        @ Telemetry port
        telemetry port Tlm

        # ----------------------------------------------------------------------
        # Commands
        # ----------------------------------------------------------------------

        @ Set the action that camera should take
        async command TakeAction(
            cameraAction: CameraAction @< State where camera either saves or takes photo
            ) \
        opcode 0x01

        @ Command to configure image
        async command ConfigImg(
            resolution: ImgResolution
            ) \
        opcode 0x03

        # ----------------------------------------------------------------------
        # Events
        # ----------------------------------------------------------------------

        @ Event where error occurred when setting up camera
        event CameraOpenError \
        severity warning high \
        format "Camera failed to open" \

        @ Event where camera is already open
        event CameraAlreadyOpen \
        severity activity low \
        format "The Camera is already open" \

        event CameraSave \
        severity activity low \
        format "Image was saved"

        event CameraProcess \
        severity activity low \
        format "Image will be processed" \

        @ Event image configuration has been set
        event SetImgConfig(
            resolution: ImgResolution @< Image size,
            ) \
        severity activity high \
        format "The image has resolution {}" \

        @ Failed to set size and color format
        event ImgConfigSetFail(
            resolution: ImgResolution @< Image size
            ) \
        severity warning high \
        format "Image resolution of {} failed to set" \

        @ Blank frame Error
        event BlankFrame \
        severity warning high \
        format "Error: Blank frame was grabbed" \

        @ Invalid buffer size error
        event InvalidBufferSizeError(
            imgBufferSize: U32 @< size of imgBuffer to hold image data
            imgSize: U32 @< size of image
        ) \
        severity warning high \
        format "imgBuffer of size {} is less than imgSize of size {}"

        # ----------------------------------------------------------------------
        # Telemetry
        # ----------------------------------------------------------------------

        @ Total number of files captured
        telemetry photosTaken: U32 id 0 update on change

    }
}
