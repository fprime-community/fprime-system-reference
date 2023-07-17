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
 
  const char enablePath[] = "/sys/class/pwm/pwmchip0/pwm0/enable"; // TODO: parameterize chip & pin 
  const char onTimePath[] = "/sys/class/pwm/pwmchip0/pwm0/duty_cycle";
  const char periodPath[] = "/sys/class/pwm/pwmchip0/pwm0/period";
  const char exportPath[] = "/sys/class/pwm/pwmchip0/export";
  const char unexportPath[] = "/sys/class/pwm/pwmchip0/unexport";


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

  }

  // ----------------------------------------------------------------------
  // Driver functions called by the handlers 
  // ----------------------------------------------------------------------

                                                
  Os::File::Status LinuxPwmDriver::setOneByte( const char* filePath, U8 writeValue ){
    Os::File::Status status = fd.open( filePath, Os::File::Mode::OPEN_WRITE );
    if ( status == Os::File::Status::OP_OK ){
      NATIVE_INT_TYPE writeSize = (NATIVE_INT_TYPE) sizeof( writeValue);
      status = fd.write( &writeValue, writeSize ); 
    }
    //printf( "write status %d\n", status);
    fd.close(); 
    return status; 
}

  Os::File::Status LinuxPwmDriver::setExport(){
    return setOneByte( exportPath, '0' );
  }

  Os::File::Status LinuxPwmDriver::setUnexport(){ 
    return setOneByte( unexportPath, '0' );
  }

  Os::File::Status LinuxPwmDriver::enable(){
    return setOneByte( enablePath, '1' );
  }

  Os::File::Status LinuxPwmDriver::disable(){
    return setOneByte( enablePath, '0' );
  }

  // Refactor as setOneByte                                                             
  Os::File::Status LinuxPwmDriver::setBytes( const char * filePath, const char * inBuf, NATIVE_INT_TYPE inBufSize ){
      Os::File::Status status = fd.open( filePath, Os::File::Mode::OPEN_WRITE );
      if ( status != Os::File::Status::OP_OK ){
        fd.close();
        return status; 
      }
      status = fd.write( inBuf, inBufSize ); 
      //printf( "write status %d\n", status);
      if ( status != Os::File::Status::OP_OK ){
        fd.close();
        return status; 
      }
      fd.close(); 
      return status; 
  }


  Os::File::Status LinuxPwmDriver::setPeriod( U32 inU32 ){ // Period is measured in nanoseconds
    CHAR newPeriodBuf[11]; // TODO: make 11 a a constant 
    (void)sprintf( newPeriodBuf, "%d", inU32 ); //snprintf
    return setBytes( periodPath, newPeriodBuf, Fw::StringUtils::string_length( newPeriodBuf, 11) ); 
  }
  
  Os::File::Status LinuxPwmDriver::setOnTime( U32 inU32 ){
    CHAR newOnTimeBuf[11]; 
    sprintf( newOnTimeBuf, "%d", inU32 ); 
    return setBytes( onTimePath, newOnTimeBuf, Fw::StringUtils::string_length( newOnTimeBuf, 11) ); 
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  // TODO: add a good/bad return type
  void LinuxPwmDriver ::
    enableAndExport_handler(
        const NATIVE_INT_TYPE portNum,
        const Drv::OneByteOps &operationType
    )
  {
    switch (operationType){
      case Drv::OneByteOps::ENABLE: 
        LinuxPwmDriver::enable();
        break;
      case Drv::OneByteOps::DISABLE: 
        LinuxPwmDriver::disable(); 
        break;
      case Drv::OneByteOps::EXPORT: 
        LinuxPwmDriver::setExport(); 
        break;
      case Drv::OneByteOps::UNEXPORT: 
        LinuxPwmDriver::setUnexport(); 
        break;
      default:
        // TODO: add error log 
        FW_ASSERT(0,operationType.e);
        break; 
    }

  }

  void LinuxPwmDriver ::
      periodAndCycle_handler(
          const NATIVE_INT_TYPE portNum,
          const Drv::MultiByteOps &operationType,
          U32 operationValue
      )
    {
      switch (operationType){
        case Drv::MultiByteOps::PERIOD:
          LinuxPwmDriver::setPeriod( operationValue );
          break;
        case Drv::MultiByteOps::ON_TIME:
          LinuxPwmDriver::setOnTime( operationValue ); 
          break; 
        default:
          // TODO: add error log 
          break; 
      }
    }

} // end namespace Drv
