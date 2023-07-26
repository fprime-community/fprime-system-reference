# Linux PWM Driver

## Description 

The Linux Pulse Width Modulation (PWM) driver is an F' 
component that allows users to control PWM hardware 
on a Linux device. Note, this driver controls ***hardware***
PWM, if there is no PWM chip, this driver will not work. 
This driver was tested on a Raspberry Pi 3b+. No objective 
measurements (oscilliscope) were taken during testing, 
however the driver was used to successfully dim an LED
and control a SG90 servo. 

## Design  

The driver writes to files such as `/sys/class/pwm/pwmchip0/pwm0/enable`
to control the hardware. The driver allows users to
access functions to enable/disable the pwm chip,
set the period, and set the on time. Export, which
effectively creates the enable, period, on_time files,
is hidden from the user in the open function which is called 
when the PWM driver initializes. 

## Units 

Whenever setting period and or on time, the units 
are always ***nanoseconds***.


## Raspberry Pi setup 

To successfully use the Linux PWM driver on a Raspberry
Pi, users need to make two edits to the Pi's files.


1. Add the following to `/etc/udev/rules.d/99-com.rules`:

`SUBSYSTEM=="pwm*", PROGRAM="/bin/sh -c '\
        chown -R root:gpio /sys/class/pwm && chmod -R 770 /sys/class/pwm; \
        chown -R root:gpio /sys/devices/platform/soc/*.pwm/pwm/pwmchip* && chmod -R 770 /sys/devices/platform/soc/*.pwm/pwm/pwmchip* \
'"`

Unfortunately, users are not allowed to write to the 
necessary PWM by default. Adding the above rules 
will fix the permissions issues.

2. Add `dtoverlay=pwm` to `/boot/config.txt`. 

Doing this will ensure that the rules we add in step
onew are incorporate into the OS at start time.

## Future Work

1. File path should be set as a parameter, incase 
users are working with a different file system structure.

2. Device tree can support multiple pwmchips, 
currently this driver cannot.

3. Unit tests to verify code quality. 

## Resources 

1. This driver was written based off of this article: \
https://mjoldfield.com/atelier/2017/03/rpi-devicetree.html
