// ======================================================================
// \title  Actuator.cpp
// \author fprime
// \brief  cpp file for Actuator component implementation class
// ======================================================================

#include <stdio.h>  // for debugging
#include <FpConfig.hpp>
#include <SystemReference/Gnc/Actuator/Actuator.hpp>

namespace Gnc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

Actuator ::Actuator(const char* const compName)
    : ActuatorComponentBase(compName),
      currentOnTime(SG90_MIDDLE),
      gain(100000),
      windowIndex(0),
      actuatorIsOn(Fw::On::OFF) {}

Actuator ::~Actuator() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void Actuator ::imuAccelIn_handler(const NATIVE_INT_TYPE portNum, const Gnc::Vector& imuVector) {
    // TODO: migrate windowing to another component
    yWindow[windowIndex] = imuVector[1];
    windowIndex = (windowIndex + 1) % WINDOW_SIZE; 
   
    if (actuatorIsOn == Fw::On::OFF) {
      return;
    }
    F32 ySum; 
    for (int i = 0; i < WINDOW_SIZE; i++) {
        ySum += yWindow[i];
    }

    F32 windowedY = ySum / WINDOW_SIZE;

    //  P-controller algorithm
    F32 error = vertCriteria - windowedY;  // 0 < error < 1 while right side up
    if (windowedY > vertCriteria) {
        // TODO: create is balanced telem or event 
        return;
    }
    U32 newOnTime = 0; 
    U32 correction = 0; 
    U32 minStepSize = 31415;
    if (imuVector[0] < 0) {
        correction = gain * error;
        newOnTime = (correction > minStepSize) ? (currentOnTime - correction) : (currentOnTime - minStepSize); 
        
    }
    if (imuVector[0] > 0) {
        correction = gain * error;
        newOnTime = (correction > minStepSize) ? (currentOnTime + correction) : (currentOnTime + minStepSize); 
    }
    if (newOnTime >= SG90_MAX_ON_TIME) {
            newOnTime = SG90_MAX_ON_TIME;
        }
    if (newOnTime <= SG90_MIN_ON_TIME) {
            newOnTime = SG90_MIN_ON_TIME;
        } 
    currentOnTime = newOnTime;
    this->pwmSetOnTime_out(0, newOnTime);
}

void Actuator ::run_handler(const NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context) {}

// ----------------------------------------------------------------------
// Command handler implementations
// ----------------------------------------------------------------------

void Actuator ::ACTIVATE_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq, Fw::On on_off) {
    this->actuatorIsOn = on_off;
    if (on_off == Fw::On::ON) {  // TODO: doesn't come on until off command is sent and on command is sent again
        this->pwmSetPeriod_out(0, SG90_PWM_PERIOD);
        currentOnTime = SG90_MIDDLE;
        this->pwmSetOnTime_out(0, SG90_MIDDLE);
        this->pwmSetEnable_out(0, Fw::Enabled::ENABLED);
        for(int i = 0; i < WINDOW_SIZE; i++){
            yWindow[i] = 0; 
        }
    } else {
        this->pwmSetEnable_out(0, Fw::Enabled::DISABLED);
    }
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

}  // end namespace Gnc
