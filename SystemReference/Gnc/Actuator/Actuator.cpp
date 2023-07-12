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

Os::File::Status Actuator::setOneByte( const char* filePath, U8 writeValue ){
    Os::File::Status status = fd.open( filePath, Os::File::Mode::OPEN_WRITE );
    if ( status != Os::File::Status::OP_OK ){
      fd.close();
      return status; 
    }
    this->buf[0] = writeValue;  
    this->bufSize = 1; 
    status = fd.write( buf, bufSize ); 
    //printf( "write status %d\n", status);
    if ( status != Os::File::Status::OP_OK ){
      fd.close();
      return status; 
    }
    fd.close(); 
    return status; 
}

  Os::File::Status Actuator::setExport(){
    return Actuator::setOneByte( exportPath, '0' );
  }

  Os::File::Status Actuator::setUnexport(){ //correct 
    return setOneByte( unexportPath, '0' );
  }

  Os::File::Status Actuator::enable(){
    return setOneByte( enablePath, '1' );
  }

  Os::File::Status Actuator::disable(){
    return setOneByte( enablePath, '0' );
  }

  Os::File::Status Actuator::setBytes( const char* filePath, const char * inBuf, NATIVE_INT_TYPE inBufSize ){
      Os::File::Status status = fd.open( filePath, Os::File::Mode::OPEN_WRITE );
      if ( status != Os::File::Status::OP_OK ){
        fd.close();
        return status; 
      }
      this->bufSize = inBufSize; 
      memcpy( this->buf, inBuf, bufSize ); 
      status = fd.write( buf, bufSize ); 
      printf( "write status %d\n", status);
      if ( status != Os::File::Status::OP_OK ){
        fd.close();
        return status; 
      }
      fd.close(); 
      return status; 
  }

  Os::File::Status Actuator::setPeriod( const char * newPeriod, NATIVE_INT_TYPE inBufSize ){
    // For SG90 Servos, the optimum period is 5e+6
    // Period is measured in nanoseconds
    return setBytes( periodPath, newPeriod, inBufSize ); 
  }

  Os::File::Status Actuator::setDutyCycle( const char * newDutyCycle, NATIVE_INT_TYPE inBufSize ){
    // For SG90 Servos, duty cycle should range between 1e6 and 2e6
    return setBytes( dutyPath, newDutyCycle, inBufSize ); 
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
      
      if( accelData[1] > 1.0 ){ // Turn on LED when IMU is pointed nearly straight up
        this->gpioSet_out( 0, Fw::Logic::HIGH );
        // Driver testing ------------
        Os::File::Status status = this->setDutyCycle( "1500000", 7);
        printf( "set duty cycle statys status %d\n", status);
        this->enable();
        Os::Task::delay(400); 
        status = this->disable();
        printf( "disable status %d\n", status);
        // End driver testing --------
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
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
    // Driver testing ------------------
    Os::File::Status status = this->setExport(); // This can only be ran successfully 
                                                 // once before setUnexport is run
                                                 // it is okay for this to fail! (it created pwm0, which only needs to be done once)

    printf( "export status %d\n", status);
    status = this->setPeriod( "5000000", 7 );
    printf( "set period status %d\n", status);
    if ( actuatorIsOn == Fw::On::ON ){
      status = this->enable();
      printf( "enable status %d\n", status);
    }
    else{ // Actuator is off
      status = this->disable();
      printf( "disable status %d\n", status);
      //this->setUnexport(); // Removes pwm0 directory
    }
    


    // End driver testing --------------
  }

} // end namespace Gnc
