module Gnc {
    @ An actuator component that demonastrates a feedback loop.
    active component Actuator {

        # One async command/port is required for active components
        # This should be overridden by the developers with a useful command/port
        
        async command ACTIVATE( on_off: Fw.On )

        ##############################################################################
        #### Uncomment the following examples to start customizing your component ####
        ##############################################################################

        # @ Example async command
        # async command COMMAND_NAME(param_name: U32)

        # @ Example telemetry counter
        # telemetry ExampleCounter: U64

        # @ Example event
        # event ExampleStateEvent(example_state: Fw.On) severity activity high id 0 format "State set to {}"

        @ Example port: receiving calls from the rate group
        sync input port run: Svc.Sched

        @ Servo Speed 
        param SERVO_SPEED: U32

        @ Receive data from the IMU 
        guarded input port imuAccelIn: Gnc.ImuData

        #TEMP
        @ Port to send commands to gpio driver
        output port gpioSet: Drv.GpioWrite

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

        @ Port for sending command registrations
        command reg port cmdRegOut

        @ Port for receiving commands
        command recv port cmdIn

        @ Port for sending command responses
        command resp port cmdResponseOut

        @ Port for sending textual representation of events
        text event port logTextOut

        @ Port for sending events to downlink
        event port logOut

        @ Port for sending telemetry channels to downlink
        telemetry port tlmOut

        @ Port to return the value of a parameter
        param get port prmGetOut

        @Port to set the value of a parameter
        param set port prmSetOut

    }
}