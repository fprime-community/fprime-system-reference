// ======================================================================
// \title  Camera.hpp
// \author vbai
// \brief  hpp file for Camera component implementation class
// ======================================================================

#ifndef Camera_HPP
#define Camera_HPP

#include "SystemReference/Payload/Camera/CameraComponentAc.hpp"
#include <libcamera/libcamera.h>
#include <queue>

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

      void allocateBuffers();

      void createBufferMap();

      // void createRequests();

      // Other methods
      bool setCameraConfiguration(ImgResolution resolution);

      void configureRequests();

      void cleanupRequests();

      void cleanup(libcamera::Stream *stream, Fw::Buffer imgBuffer);

      //! Destroy object Camera
      //!
      ~Camera();

    PRIVATE:

      static void requestComplete(libcamera::Request *request);

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for CaptureImage command handler
      //! Capture image and save the raw data
      void CaptureImage_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      //! Implementation for ConfigImg command handler
      //! Command to configure image
      void ConfigImg_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          Payload::ImgResolution resolution
      );

      U32 m_photoCount;
      std::unique_ptr<libcamera::CameraManager> camManager;
      std::shared_ptr<libcamera::Camera> m_capture;
      std::unique_ptr<libcamera::CameraConfiguration> cameraConfig;
      libcamera::FrameBufferAllocator *allocator = nullptr;
      std::vector<std::unique_ptr<libcamera::Request>> requests;
      std::unique_ptr<libcamera::Request> frameRequest;
	    std::map<libcamera::FrameBuffer *, std::vector<libcamera::Span<uint8_t>>> mappedBuffers;
      std::map<libcamera::Stream *, std::queue<libcamera::FrameBuffer *>> frameBuffers;
      bool cameraStarted = false;
      ImgResolution currentResolution = ImgResolution::SIZE_640x480;
    };

} // end namespace Payload

#endif
