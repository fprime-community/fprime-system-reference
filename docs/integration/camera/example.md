# Image Capture Tutorial

### Tutorial Steps:
- [Required Setup](#required-setup)
- [Capture Frame](#capture-frame)
- [Downlink Image Data](#downlink-image-data)
- [Process Image Data](#process-image-data)

### Required Setup
Before getting started, ensure the below has been done:
1. libcamera has been built for the target system, see [here](../../../README.md#setup-libcamera) for steps.
2. The System Reference with libcamera has been built for the target system, see [here](building-system-ref-with-libcamera.md) for steps.
3. Ensure that the F´ GDS is running, the System Reference has been copied over to the Rapsberry Pi, and that the software is running. See steps [here](../../../README.md#upload-to-the-raspberry-pi) for more information.

## Capture Frame
1. Open a new log file by sending the `saveImageBufferLogger.BL_OpenFile` command with a unique file name as the argument. 
> Example: `saveImageBufferLogger.BL_OpenFile, "capture_1.dat"`
2. Configure the image resolution by setting the `IMG_RESOLUTION` parameter, which can be done by issuing `camera.IMG_RESOLUTION_PRM_SET` with desired resolution argument. Note: Keep track of the resolution used, since it will be referenced when processing the image data later on.
> Example: `camera.IMG_RESOLUTION_PRM_SET, SIZE_800x600`
3. Capture frame data by sending the `camera.CaptureImage` command
4. Close the log file by sending `saveImageBufferLogger.BL_CloseFile`

## Downlink Image Data
Now that image data has been captured and saved to disk, we now want to downlink the data so that it can be processed on the ground.
1. Select the `fileDownlink.sendFile` and specify the source location of the file and the destination where the file should be stored on the ground.
> Example: `fileDownlink.SendFile, "/home/pi/capture_1.dat", "capture_1.dat"`
2. In F´ GDS, click the Downlink tab to check the downlink status. Once the file has been downlinked, take note of the destination location specified under the "Destination" column.

## Process Image Data
Before getting started, ensure that the dependencies for running the ground tools have been installed, see [here](../../../ground-tools/README.md) for more details.

From the project root, navigate to the ground tools directory: `cd ground-tools`

Process the image data as PNG by running:
```
python process_raw_image.py -i <.dat filename on ground> -r <image resolution, 640x480 or 800x600> -f PNG
```

Process the image data as JPEG by running:
```
python process_raw_image.py -i <.dat filename on ground> -r <image resolution, 640x480 or 800x600>  -f JPEG
```

Processed images will be saved in the same directory as the input file as the input file name followed by the .png or .jpg extension.






