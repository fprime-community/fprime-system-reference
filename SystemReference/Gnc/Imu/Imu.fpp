
module Gnc {

    @ Component for IMU
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
        # Special ports
        # ----------------------------------------------------------------------

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

        # ----------------------------------------------------------------------
        # Telemetry
        # ----------------------------------------------------------------------

        @ X, Y, Z acceleration from accelerometer
        telemetry accelerometer: Vector id 0 update on change

        @ X, Y, Z degrees from gyroscope
        telemetry gyroscope: Vector id 1 update on change

    }
}