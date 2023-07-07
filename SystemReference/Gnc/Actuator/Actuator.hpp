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

    PRIVATE:

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


    };

} // end namespace Gnc

#endif
