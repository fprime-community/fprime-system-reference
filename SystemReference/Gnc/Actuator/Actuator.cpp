// ======================================================================
// \title  Actuator.cpp
// \author fprime
// \brief  cpp file for Actuator component implementation class
// ======================================================================


#include <SystemReference/Gnc/Actuator/Actuator.hpp>
#include <FpConfig.hpp>
#include <stdio.h> // for debugging



namespace Gnc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  Actuator ::
    Actuator(
        const char *const compName
    ) : ActuatorComponentBase(compName), newOnTime(SG90_MIDDLE), 
        currentOnTime(SG90_MIDDLE), gain(GAIN), 
        windowedY(0), ySum(0), windowIndex(0),
        actuatorIsOn(Fw::On::OFF)
  {
    
  }

  Actuator ::
    ~Actuator()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void Actuator ::
    imuAccelIn_handler(
        const NATIVE_INT_TYPE portNum,
        const Gnc::Vector &imuVector
    )
  {
    if( actuatorIsOn == Fw::On::OFF){
      return;
    }
    // TODO: migrate windowing to another component 
    yWindow[windowIndex] = imuVector[1]; 
    windowIndex++;
    //printf("array entry = %f\n", yWindow[windowIndex]); 
    if( windowIndex >= WINDOW_SIZE ){
      windowIndex = 0;
    }
    ySum = 0;
    for( int i = 0; i < WINDOW_SIZE; i++){
      ySum += yWindow[i];
    }
    windowedY = ySum / WINDOW_SIZE; 

    //printf("WINDOWED Y =%f\n", windowedY);
    // P-controller algorithm 
    error =  (desiredPos + 1) - windowedY; // 1 < error < 2 while right side up 

    if( windowedY > desiredPos ){
      this->gpioSet_out(0, Fw::Logic::HIGH);
      return; 
    }
    this->gpioSet_out(0, Fw::Logic::LOW);
    if( imuVector[0] < 0 ){
      newOnTime = currentOnTime - (gain * error);
      if( newOnTime <= SG90_MIN_ON_TIME ){
        newOnTime = SG90_MIN_ON_TIME;
      }
    }
    if( imuVector[0] > 0 ){
      newOnTime = currentOnTime + (gain * error);
      if( newOnTime >= SG90_MAX_ON_TIME ){
        newOnTime = SG90_MAX_ON_TIME;
      }
    }
    this->pwmSetOnTime_out(0, newOnTime);
    currentOnTime = newOnTime;
    //printf( "NEW ON TIME = %d\n", currentOnTime );
    Os::Task::delay(50);
  }

  void Actuator ::
    run_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void Actuator ::
    ACTIVATE_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        Fw::On on_off
    )
  {
    this->actuatorIsOn = on_off;
    if(on_off == Fw::On::ON){           // TODO: doesn't come on until off command is sent and on command is sent again
      this->pwmSetPeriod_out(0,SG90_PWM_PERIOD);
      Os::Task::delay(400);
      this->pwmSetOnTime_out(0,SG90_MIDDLE);
      currentOnTime = SG90_MIDDLE; 
      Os::Task::delay(400);
      this-pwmSetEnable_out(0,Fw::Enabled::ENABLED);
    }
    else{
      this-pwmSetEnable_out(0,Fw::Enabled::DISABLED);
    }
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

} // end namespace Gnc
