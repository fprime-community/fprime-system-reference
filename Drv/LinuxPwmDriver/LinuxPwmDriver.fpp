module Drv {

    @ PWM driver for Linux
    passive component LinuxPwmDriver {
        
        sync input port enableDisable: Drv.EnableDisable
        
        sync input port onTime: Drv.OnTime

        sync input port period: Drv.Period
    }
}