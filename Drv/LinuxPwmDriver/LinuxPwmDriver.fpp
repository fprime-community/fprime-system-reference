module Drv {

    @ PWM driver for Linux
    passive component LinuxPwmDriver {
        
        sync input port enableAndExport: Drv.EnableAndExport
        
        sync input port periodAndCycle: Drv.PeriodAndOnTime

    }
}