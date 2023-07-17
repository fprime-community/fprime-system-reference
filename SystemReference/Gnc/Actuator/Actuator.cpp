// ======================================================================
// \title  Actuator.cpp
// \author fprime
// \brief  cpp file for Actuator component implementation class
// ======================================================================


#include <SystemReference/Gnc/Actuator/Actuator.hpp>
#include <FpConfig.hpp> 

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
        // ENABLE
        this->pwmExportAndEnable_out(0, Drv::OneByteOps::ENABLE);
        this->pwmPeriodandOnTime_out(0,Drv::MultiByteOps::ON_TIME, 1500000);
        //this->Drv::LinuxPwmDriver::setPeriod( 1500000 ); DO I DO THIS OR ABOVE????
        Os::Task::delay(400); 
        this->pwmExportAndEnable_out(0, Drv::OneByteOps::DISABLE);
      }
      else{
        this->gpioSet_out( 0, Fw::Logic::LOW );
        this->pwmExportAndEnable_out(0, Drv::OneByteOps::DISABLE); 
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
    if ( actuatorIsOn == Fw::On::ON ){
      // EXPORT
      this->pwmExportAndEnable_out(0, Drv::OneByteOps::EXPORT);
      // SET PERIOD
      this->pwmPeriodandOnTime_out(0, Drv::MultiByteOps::PERIOD, 5000000);                                                   
    }
    else{ // Actuator is off
      this->pwmExportAndEnable_out(0, Drv::OneByteOps::DISABLE);
    }
  
  } // END ACTIVATE_CMDHANDLER 

} // end namespace Gnc
