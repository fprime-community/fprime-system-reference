// ======================================================================
// \title  LinuxPwmDriver.cpp
// \author fprime
// \brief  cpp file for LinuxPwmDriver component implementation class
// ======================================================================


#include <Drv/LinuxPwmDriver/LinuxPwmDriver.hpp>
#include <FpConfig.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <stdio.h>
#include <Fw/Types/Assert.hpp>

namespace Drv {
 
  const char ENABLE_PATH[] = "/sys/class/pwm/pwmchip0/pwm0/enable"; // TODO: parameterize chip & pin 
  const char ON_TIME_PATH[] = "/sys/class/pwm/pwmchip0/pwm0/duty_cycle";
  const char PERIOD_PATH[] = "/sys/class/pwm/pwmchip0/pwm0/period";
  const char EXPORT_PATH[] = "/sys/class/pwm/pwmchip0/export";
  const char UNEXPORT_PATH[] = "/sys/class/pwm/pwmchip0/unexport";
  const NATIVE_INT_TYPE MAX_PWM_WRITE_BUF_SIZE = 11; // The largest 32 bit number is only 10 character

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  LinuxPwmDriver ::
    LinuxPwmDriver(
        const char *const compName
    ) : LinuxPwmDriverComponentBase(compName)
  {

  }

  LinuxPwmDriver ::
    ~LinuxPwmDriver()
  {
    setUnexport();
  }

  Fw::Success LinuxPwmDriver ::
    open()
    {
      if(setExport() == Os::File::OP_OK){
        return Fw::Success::SUCCESS; 
      }
      return Fw::Success::FAILURE;
    }
  // ----------------------------------------------------------------------
  // Driver helper functions called by the handlers 
  // ----------------------------------------------------------------------

                                                
  Os::File::Status LinuxPwmDriver::setOneByte( const char* filePath, U8 writeValue ){
    Os::File::Status status = fd.open( filePath, Os::File::Mode::OPEN_WRITE );
    if ( status == Os::File::Status::OP_OK ){
      NATIVE_INT_TYPE writeSize = (NATIVE_INT_TYPE) sizeof( writeValue);
      status = fd.write( &writeValue, writeSize ); 
    }
    fd.close(); 
    return status; 
  }

  Os::File::Status LinuxPwmDriver::setExport(){
    return setOneByte( EXPORT_PATH, '0' );
  }

  Os::File::Status LinuxPwmDriver::setUnexport(){ 
    return setOneByte( UNEXPORT_PATH, '0' );
  }

  Os::File::Status LinuxPwmDriver::enable(){
    return setOneByte( ENABLE_PATH, '1' );
  }

  Os::File::Status LinuxPwmDriver::disable(){
    return setOneByte( ENABLE_PATH, '0' );
  }
                                                             
  Os::File::Status LinuxPwmDriver::setBytes( const char * filePath, const char * inBuf, NATIVE_INT_TYPE inBufSize ){
      Os::File::Status status = fd.open( filePath, Os::File::Mode::OPEN_WRITE );
      if ( status == Os::File::Status::OP_OK ){
        status = fd.write( inBuf, inBufSize ); 
      }
    fd.close(); 
    return status; 
  }


  Os::File::Status LinuxPwmDriver::setPeriod( U32 inU32 ){ // Period is measured in nanoseconds
    CHAR newPeriodBuf[MAX_PWM_WRITE_BUF_SIZE]; 
    (void)snprintf( newPeriodBuf, MAX_PWM_WRITE_BUF_SIZE, "%d", inU32 ); 
    return setBytes( PERIOD_PATH, newPeriodBuf, Fw::StringUtils::string_length( newPeriodBuf, 11) ); 
  }
  
  Os::File::Status LinuxPwmDriver::setOnTime( U32 inU32 ){
    CHAR newOnTimeBuf[MAX_PWM_WRITE_BUF_SIZE]; 
    (void)snprintf( newOnTimeBuf, MAX_PWM_WRITE_BUF_SIZE,"%d", inU32 ); 
    return setBytes( ON_TIME_PATH, newOnTimeBuf, Fw::StringUtils::string_length( newOnTimeBuf, 11) ); 
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

   Fw::Success LinuxPwmDriver ::
    enableDisable_handler(
        const NATIVE_INT_TYPE portNum,
        const Fw::Enabled &operationType
    )
  {
    switch(operationType){
      case Fw::Enabled::ENABLED:
        if(LinuxPwmDriver::enable() == Os::File::OP_OK){
          return Fw::Success::SUCCESS;
        }
        break;
      case Fw::Enabled::DISABLED:
        if(LinuxPwmDriver::disable() == Os::File::OP_OK){
          return Fw::Success::SUCCESS;
        }
        break;
      default:
        FW_ASSERT(0);
        return Fw::Success::FAILURE;
    }
    return Fw::Success::FAILURE; 
  }

  Fw::Success LinuxPwmDriver ::
    onTime_handler(
        const NATIVE_INT_TYPE portNum,
        U32 operationValue
    )
  {
    if(LinuxPwmDriver::setOnTime(operationValue) == Os::File::OP_OK){
      return Fw::Success::SUCCESS; 
    }
    return Fw::Success::FAILURE; 
  }

  Fw::Success LinuxPwmDriver :: 
    period_handler(
        const NATIVE_INT_TYPE portNum,
        U32 operationValue
    )
  { 
    if(LinuxPwmDriver::setPeriod(operationValue) == Os::File::OP_OK){
      return Fw::Success::SUCCESS; 
    }
    return Fw::Success::FAILURE; 
  }

} // end namespace Drv
