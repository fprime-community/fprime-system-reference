// ======================================================================
// \title  Camera.hpp
// \author vbai
// \brief  hpp file for Camera component implementation class
// ======================================================================

#ifndef Camera_HPP
#define Camera_HPP

#include "SystemReference/Payload/Camera/CameraComponentAc.hpp"
#ifdef USES_OPENCV
#include <opencv2/opencv.hpp>
#endif

namespace Payload {

  class Camera :
    public CameraComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object Camera
      //!
      Camera(
          const char *const compName /*!< The component name*/
      );

      //! Initialize object Camera
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Startup camera device
      //!
      bool open(I32 deviceIndex = 0);


      //! Destroy object Camera
      //!
      ~Camera();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for TakeAction command handler
      //! Set the action that camera should take
      void TakeAction_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          Payload::CameraAction cameraAction /*!< State where camera either saves or takes photo*/
      );

      //! Implementation for ConfigImg command handler
      //! Command to configure image
      void ConfigImg_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          Payload::ImgResolution resolution
      );

      U32 m_photoCount;
#ifdef USES_OPENCV
      cv::VideoCapture m_capture;
#endif
    };

} // end namespace Payload

#endif
