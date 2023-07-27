module Gnc {
    @ An actuator component that demonastrates a feedback loop.
    active component Actuator {
        
        async command ACTIVATE( on_off: Fw.On )

        @ Controller gain 
        param CONTROLLER_GAIN : U32 default 31415

        @ Receive data from the IMU 
        guarded input port imuAccelIn: Gnc.ImuData

        @ Enable or disable PWM 
        output port pwmSetEnable: Drv.EnableDisable

        @ Set PWM on time, also known as duty cycle
        output port pwmSetOnTime: Drv.OnTime

        @ Set PWM period 
        output port pwmSetPeriod: Drv.Period


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