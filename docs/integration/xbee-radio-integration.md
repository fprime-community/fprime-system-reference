# System Reference XBee Com Component Integration

## Table of Contents

* [1. Initializing XBee Radios](#1-ensure-xbee-devices-are-working-together)
* [2. Topology Changes for UART Driver](#2-change-the-topology-of-the-com-components-to-include-the-linux-uart-driver)
* [3. Instance Changes for Com Component](#3-change-the-instances-of-the-com-components-to-use-xbee-radio-com)
* [4. Rerun build](#4-rerun-build)
* [5. Running F Prime with UART Devices](#5-running-f-prime-with-uart-devices)
    * [F Prime GDS](#f-prime-gds)
    * [F Prime Binary](#f-prime-binary)

## 1. Initializing XBee Radios

We recommend you to test your XBee radios before implementing them into the F Prime ecosystem

1. Download XCTU from [Digi](https://www.digi.com/products/embedded-systems/digi-xbee/digi-xbee-tools/xctu)
2. Follow the steps to download the software and install it.
3. Plug in XBee into device and do a default search for your XBee devices
4. Open serial console and troubleshoot if they are not able to communicate with eachother

[Link](https://www.digi.com/resources/documentation/digidocs/PDFs/90001458-13.pdf) to Digi documentation for XCTU.

## 2. Topology Changes for UART Driver

Find the following code snippets and uncomment them in the `SystemReference/Top/topology.fpp` file.

All segments you need to uncomment start with `# XBee Radio Integration`

```
#      rateGroup1Comp.RateGroupMemberOut[4] -> radio.run
```
```
#    connections Radio {
#      radio.allocate -> comBufferManager.bufferGetCallee
#      radio.deallocate -> comBufferManager.bufferSendIn
#    }
```


## 3. Instance Changes for Com Component

Find the following code snippets and uncomment them in `SystemReference/Top/instances.fpp` file.

All segments you need to uncomment start with `# XBee Radio Integration`

```
# instance comDriver: Drv.LinuxUartDriver base id 0x4000 {
#    phase Fpp.ToCpp.Phases.configConstants """
#    enum {
#      PRIORITY = 100,
#      STACK_SIZE = Default::stackSize
#    };
#    """
#
#    phase Fpp.ToCpp.Phases.startTasks """
#    // Initialize socket server if and only if there is a valid specification
#    if (state.hostName != nullptr) {
#        Os::TaskString name("ReceiveTask");
#        // Uplink is configured for receive so a socket task is started
#        if (comDriver.open(state.hostName, static_cast<Drv::LinuxUartDriver::UartBaudRate>(state.portNumber), Drv::LinuxUartDriver::NO_FLOW,
#                       Drv::LinuxUartDriver::PARITY_NONE, 1024)) {
#            comDriver.startReadThread(
#                ConfigConstants::comDriver::PRIORITY,
#                ConfigConstants::comDriver::STACK_SIZE
#            );
#        } else {
#            printf("Failed to open UART port %s at speed %" PRIu32 "\n", state.hostName, state.portNumber);
#        }
#    }
#    """
#
#    phase Fpp.ToCpp.Phases.stopTasks """
#    comDriver.quitReadThread();
#    """
#
#    phase Fpp.ToCpp.Phases.freeThreads """
#    comDriver.join(nullptr);
#    """
# }
```

```
#  instance radio: Com.XBee base id 0x1200 \
#    queue size Default.queueSize \
#    stack size Default.stackSize \
#    priority 140
```

Comment out the following:

All segments you need to comment out start with `# TCP/IP Integration`

```
  instance radio: Svc.ComStub base id 0x1200

#  @ Communications driver. May be swapped with other comm drivers like UART
#  @ Note: Here we have TCP reliable uplink and UDP (low latency) downlink
  instance comDriver: Drv.ByteStreamDriverModel base id 0x4000 \
    type "Drv::TcpClient" \
    at "../../Drv/TcpClient/TcpClient.hpp" \
  {
    phase Fpp.ToCpp.Phases.configConstants """
    enum {
      PRIORITY = 100,
      STACK_SIZE = Default::stackSize
    };
    """

    phase Fpp.ToCpp.Phases.startTasks """
    // Initialize socket server if and only if there is a valid specification
    if (state.hostName != nullptr && state.portNumber != 0) {
        Os::TaskString name("ReceiveTask");
        // Uplink is configured for receive so a socket task is started
        comDriver.configure(state.hostName, state.portNumber);
        comDriver.startSocketTask(
            name,
            true,
            ConfigConstants::comDriver::PRIORITY,
            ConfigConstants::comDriver::STACK_SIZE
        );
    }
    """

   phase Fpp.ToCpp.Phases.freeThreads """
   comDriver.stopSocketTask();
   (void) comDriver.joinSocketTask(nullptr);
   """

 }
 ```

## 4. Rerun build

After making these changes, you must rebuild the system reference

```bash
#In: SystemReference
fprime-util build
```

## 5. Running F Prime with UART Devices

In order to run F Prime with a UART device, you must start each part of F Prime seperately if testing on the same device

### F Prime GDS
```bash
fprime-gds -n --dictionary <path to dictionary> --comm-adapter uart --uart-device <Path to UART device> --uart-baud <Baud rate>
``` 

### F Prime Binary
```bash
./SystemReference -a <Path to UART device> -p <Baud Rate>
```
