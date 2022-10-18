module Gnc {
    @ The power state enumeration
    enum PowerState {OFF, ON}

    @ 3-tuple type used for telemetry
    array Vector = [3] F32

    @ Component for receiving IMU data via poll method
    passive component Imu {
        # ----------------------------------------------------------------------
        # General ports
        # ----------------------------------------------------------------------

        @ Port to send telemetry to ground
        guarded input port Run: Svc.Sched

        @ Port that reads data from device
        output port read: Drv.I2c

        @ Port that writes data to device
        output port write: Drv.I2c

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

        @ Command to turn on the device
        guarded command PowerSwitch(
            powerState: PowerState
        ) \
        opcode 0x01

        # ----------------------------------------------------------------------
        # Events
        # ----------------------------------------------------------------------

        @ Error occurred when requesting telemetry
        event TelemetryError(
            status: Drv.I2cStatus @< the status value returned
        ) \
        severity warning high \
        format "Telemetry request failed with status {}"

        @ Configuration failed
        event SetUpConfigError(
            writeStatus: Drv.I2cStatus @< the status of writing data to device
        ) \
        severity warning high \
        format "Setup Error: Write status failed with code {}"

        @ Device was not taken out of sleep mode
        event PowerModeError(
            writeStatus: Drv.I2cStatus @< the status of writing data to device
        ) \
        severity warning high \
        format "Setup Error: Power mode failed to set up with write code {}"

        @ Report power state
        event PowerStatus(
            powerStatus: PowerState @< power state of device
        ) \
        severity activity high \
        format "The device has been turned {}"

        # ----------------------------------------------------------------------
        # Telemetry
        # ---------------------------------------------------------------------

        @ X, Y, Z acceleration from accelerometer
        telemetry accelerometer: Vector id 0 update always format "{} g"

        @ X, Y, Z degrees from gyroscope
        telemetry gyroscope: Vector id 1 update always format "{} deg/s"

    }
}
