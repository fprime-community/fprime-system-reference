module Gnc {
    @ The power state enumeration
    enum PowerState {OFF, ON}

    @ Component for receiving IMU data via poll method
    passive component Imu {
        # ----------------------------------------------------------------------
        # General ports
        # ----------------------------------------------------------------------

        @ Port to send telemetry to ground
        guarded input port Run: Svc.Sched

        @ Port that attains current acceleration value
        guarded input port getAcceleration: ImuDataPort

        @ Port that attains current rotation value
        guarded input port getGyroscope: ImuDataPort

        @ Port that reads data from device
        output port read: Drv.I2c

        @ Port that writes data to device
        output port write: Drv.I2c

        # ----------------------------------------------------------------------
        # Commands
        # ----------------------------------------------------------------------

        @ Command to turn on the device
        guarded command PowerSwitch(
            powerState: PowerState
        ) \
        opcode 0x01

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
        # Events
        # ----------------------------------------------------------------------

        @ Event where error occurred when requesting telemetry
        event TelemetryError(
            status: Drv.I2cStatus @< the status value returned
        ) \
        severity warning high \
        format "Telemetry request failed with status {}" \

        @ Error where configuration failed
        event SetUpConfigError(
            writeStatus: Drv.I2cStatus @< the status of writing data to device
        ) \
        severity warning high \
        format "Setup Error: Write status failed with code {}" \

        @ Error where device was not taken out of sleep mode
        event PowerModeError(
            writeStatus: Drv.I2cStatus @< the status of writing data to device
        ) \
        severity warning high \
        format "Setup Error: Power mode failed to set up with write code {}" \

        event PowerStatus(
            powerStatus: PowerState @< power state of device
        ) \
        severity activity high \
        format "The device has been turned {}" \
        # ----------------------------------------------------------------------
        # Telemetry
        # ---------------------------------------------------------------------

        @ X, Y, Z acceleration from accelerometer
        telemetry accelerometer: Vector id 0 update always format "{} g"

        @ X, Y, Z degrees from gyroscope
        telemetry gyroscope: Vector id 1 update always format "{} deg/s"

    }
}