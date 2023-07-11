// ======================================================================
// \title  Actuator.cpp
// \author fprime
// \brief  cpp file for Actuator component implementation class
// ======================================================================


#include <SystemReference/Gnc/Actuator/Actuator.hpp>
#include <FpConfig.hpp>
#include <Fw/Logger/Logger.hpp> // for debugging
#include <stdio.h> // for debugging / for fileIO

// Driver testing ------------------

const char enablePath[] = "/sys/class/pwm/pwmchip0/pwm0/enable";
const char dutyPath[] = "/sys/class/pwm/pwmchip0/pwm0/duty_cycle";
const char periodPath[] = "/sys/class/pwm/pwmchip0/pwm0/period";
const char exportPath[] = "/sys/class/pwm/pwmchip0/export";
const char unexportPath[] = "/sys/class/pwm/pwmchip0/unexport";

/*
char SERVO_PERIOD[] = "5000000";
char SERVO_MAX[] = "2000000";
char SERVO_MIN[] = "1000000";

int enable() {
   Enable = fopen( enablePath, "r+" );
   fprintf( Enable, "%s", "1" );
   //@TODO: Add error checking and meaningful return 
   fclose( Enable );
   return 0;
}

int disable() {
   Enable = fopen( enablePath, "r+" );
   fprintf( Enable, "%s", "0" );
   //@TODO: Add error checking and meaningful return 
   fclose( Enable );
   return 0;
}

int setDutyCycle( char newCycle[9] ){ 

   DutyCycle = fopen( dutyPath, "r+" );
   fprintf( DutyCycle, "%s", newCycle );
   //@TODO: Add error checking and meaningful return 
   fclose( DutyCycle );
   return 0;
}

int setPeriod( char newPeriod[9] ){
   Period = fopen ( periodPath, "r+" );
   fprintf( Period, "%s", newPeriod );
   //@TODO: Add error checking and meaningful return 
   fclose( Period );
   return 0;
}

*/

// End Driver testing --------------


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
// Driver testing ------------------

Os::File::Status Actuator::setExport(){
    Os::File::Status status = fd.open( exportPath, Os::File::Mode::OPEN_WRITE );
    if ( status != Os::File::Status::OP_OK ){
      return status; 
    }
    this->buf[0] = '0'; 
    bufSize = 1; 
    status = fd.write( buf, bufSize ); 
    if ( status != Os::File::Status::OP_OK ){
      return status; 
    }
    fd.close(); 
    return status; 
}

Os::File::Status Actuator::setUnexport(){
    Os::File::Status status = fd.open( unexportPath, Os::File::Mode::OPEN_WRITE );
    if ( status != Os::File::Status::OP_OK ){
      return status; 
    }
    this->buf[0] = '0'; 
    bufSize = 1; 
    status = fd.write( buf, bufSize ); 
    if ( status != Os::File::Status::OP_OK ){
      return status; 
    }
    fd.close(); 
    return status; 
}


// End Driver testing --------------

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
      Os::File::Status status = this->setExport();
      printf( "export status %d\n", status ); 

      status = this->setUnexport();
      printf( "unexport status %d\n", status );

      if( accelData[1] > 1.0 ){ // Turn on LED when IMU is pointed nearly straight up
        this->gpioSet_out( 0, Fw::Logic::HIGH );
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
    // Driver testing ------------------
    


    // End driver testing --------------

    this->actuatorIsOn = on_off;
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

} // end namespace Gnc
