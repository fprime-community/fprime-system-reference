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

  class Camera final :
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

      // allocate buffers

      void allocateBuffers();
      // create or re-use request
      void configureRequests();
      
      // update camera configuration based on specified resolution
      bool setCameraConfiguration(ImgResolution resolution);

      // parameter updates
      void parameterUpdated(FwPrmIdType id);

      // stop camera, deallocate memory, etc. 
      void cleanup();

      //! Destroy object Camera
      //!
      ~Camera();

    PRIVATE:

      void requestComplete(libcamera::Request *request);

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for CaptureImage command handler
      //! Capture image and save the raw data
      void CaptureImage_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      const ImgResolution DEFAULT_IMG_RESOLUTION = ImgResolution::SIZE_640x480;
      U32 m_photoCount;
      std::unique_ptr<libcamera::CameraManager> camManager = std::make_unique<libcamera::CameraManager>();
      std::shared_ptr<libcamera::Camera> m_capture;
      std::unique_ptr<libcamera::CameraConfiguration> cameraConfig;
      libcamera::Request *requestReceivedPtr;
      libcamera::FrameBufferAllocator *allocatorPtr = nullptr;
      std::unique_ptr<libcamera::Request> frameRequest;
	    std::map<libcamera::FrameBuffer *, std::vector<libcamera::Span<uint8_t>>> mappedBuffers;
      std::map<libcamera::Stream *, std::queue<libcamera::FrameBuffer *>> availableFrameBuffers;
      bool cameraStarted = false;
    };

} // end namespace Payload

#endif
