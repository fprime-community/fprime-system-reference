// ======================================================================
// \title  Actuator.hpp
// \author fprime
// \brief  hpp file for Actuator component implementation class
// ======================================================================

#ifndef Actuator_HPP
#define Actuator_HPP

#include "SystemReference/Gnc/Actuator/ActuatorComponentAc.hpp"

namespace Gnc {

  class Actuator :
    public ActuatorComponentBase
  {
    static const U32 SG90_PWM_PERIOD = 5000000; 
    static const U32 SG90_MIDDLE = 1300000; 
    static const U32 SG90_MAX_ON_TIME = 2200000;
    static const U32 SG90_MIN_ON_TIME = 500000;
    static const U16 WINDOW_SIZE = 11;
    static const U32 vertCriteria = 1; // @TODO: make parameter 
    
    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object Actuator
      //!
      Actuator(
          const char *const compName /*!< The component name*/
      );

      //! Destroy object Actuator
      //!
      ~Actuator();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for imuAccelIn
      //!
      void imuAccelIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const Gnc::Vector &imuVector 
      );

      //! Handler implementation for run
      //!
      void run_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< 
      The call order
      */
      );

    private: 

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for ACTIVATE command handler
      //! 
      void ACTIVATE_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          Fw::On on_off 
      );

    
   // SG90 servo by tower pro 
    Fw::On actuatorIsOn; 
    U64 currentOnTime; 
    U64 gain; // TODO: make parameter

    U16 windowIndex; //needs state
    F32 yWindow[WINDOW_SIZE]; 

    };

} // end namespace Gnc

#endif
