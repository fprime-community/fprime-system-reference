module Payload {

    enum FileFormat { JPG = 0, PNG = 1 }

    @ Component for processing raw images
    active component ImageProcessor {
        # ----------------------------------------------------------------------
        # General ports
        # ----------------------------------------------------------------------

        @ Save processed photo to logger
        output port postProcess: Fw.BufferSend

        @ Image data to be processed
        async input port imageData: ImageData

        @ Port for allocating Fw::Buffer objects from a buffer manager.
        output port bufferAllocate: Fw.BufferGet

        @ Port for deallocating Fw::Buffer objects from a buffer manager.
        output port bufferDeallocate: Fw.BufferSend

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

        @ Set the format to convert to from raw image file
        async command SetFormat(
            fileFormat: FileFormat @< Type of file format to convert to
            ) \
        opcode 0x01

        # ----------------------------------------------------------------------
        # Events
        # ----------------------------------------------------------------------

        @ Event where error occurred when setting up camera
        event NoImgData \
        severity warning high \
        format "Image has not data to process" \

        @ Event file format has been set
        event SetFileFormat(
            fileFormat: FileFormat @< file format to convert to
            ) \
        severity activity high \
        format "The image will be converted to {}" \

        @ Event bad buffer size
        event BadBufferSize(
            encodeSize: U32 @< Size of allocated buffer to store output buffer from imencode
            bufferSize: U32 @< Size of output buffer from imencode
        ) \
        severity warning high \
        format "Encode buffer of size {} is smaller than buffer of size {}"

        # ----------------------------------------------------------------------
        # Telemetry
        # ----------------------------------------------------------------------

        @ Total number of files captured
        telemetry photosProcessed: U32 id 0 update on change

    }
}
