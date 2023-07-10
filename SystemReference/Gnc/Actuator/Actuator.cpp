// ======================================================================
// \title  Actuator.cpp
// \author fprime
// \brief  cpp file for Actuator component implementation class
// ======================================================================


#include <SystemReference/Gnc/Actuator/Actuator.hpp>
#include <FpConfig.hpp>
#include <Fw/Logger/Logger.hpp> // for debugging
#include <stdio.h> // for debugging 

namespace Gnc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  Actuator ::
    Actuator(
        const char *const compName
    ) : ActuatorComponentBase(compName)
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
    this->accelData[0] = imuVector[0];
    this->accelData[1] = imuVector[1]; //the one we want
    this->accelData[2] = imuVector[2]; 
  }

  void Actuator ::
    run_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    if( actuatorIsOn == Fw::On::ON ){ // If Component is activated 
      if( accelData[1] > 1.0 ){ // Turn on LED when IMU is pointed nearly straight up
        this->gpioSet_out( 0, Fw::Logic::HIGH );
        printf( "Winner winner chicker dinner!\n" ); 
      }
      else{
        this->gpioSet_out( 0, Fw::Logic::LOW );
      }
    }
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
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

} // end namespace Gnc
