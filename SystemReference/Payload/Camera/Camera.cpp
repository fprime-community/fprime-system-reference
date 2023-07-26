// ======================================================================
// \title  Camera.cpp
// \author vbai
// \brief  cpp file for Camera component implementation class
// ======================================================================

#include "Fw/Types/BasicTypes.hpp"
#include <SystemReference/Payload/Camera/Camera.hpp>
#include <libcamera/libcamera/libcamera.h>
#include <errno.h>
#include <unistd.h>

namespace Payload {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  Camera ::Camera(const char *const compName)
      : CameraComponentBase(compName), m_photoCount(0){}

  void Camera ::init(const NATIVE_INT_TYPE queueDepth, const NATIVE_INT_TYPE instance) {
    CameraComponentBase::init(queueDepth, instance);
    camManager = std::make_unique<libcamera::CameraManager>();
    camManager->start();
  }

  bool Camera::open(I32 deviceIndex) {
    // check to see if any cameras were detected
    if (!camManager->cameras().empty()) {
      // get the camera from the CameraManager and acquire it
      m_capture = camManager->cameras()[deviceIndex];
      int returnCode = m_capture->acquire();

      // camera was acquired successfully
      if (returnCode == 0) {
        return true;
      }
      // camera was previously acquired, emit a CameraAlreadyOpen event
      else if (returnCode == EBUSY) {
        this->log_ACTIVITY_LO_CameraAlreadyOpen();
        return true;
      }
    }
    // no cameras were detected or some error occurred, emit a CameraOpenError event
    this->log_WARNING_HI_CameraOpenError();
    return false;
  }

  Camera ::~Camera() {}

  // void Camera ::processRequest(libcamera::Request *request);
  libcamera::Request *requestReceived;

  void Camera ::requestComplete(libcamera::Request *request) {
    if (request->status() == libcamera::Request::RequestCancelled) {
      return;
    }
    requestReceived = request;
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void Camera ::TakeAction_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq, Payload::CameraAction cameraAction) {
    RawImageData rawImageData;
    libcamera::FrameBufferAllocator *allocator = new libcamera::FrameBufferAllocator(m_capture);
    libcamera::StreamConfiguration streamConfig = m_capture->generateConfiguration( { libcamera::StreamRole::StillCapture } )->at(0);
    libcamera::Stream *stream = streamConfig.stream();
    // Set up buffer for image data
    const std::vector<std::unique_ptr<libcamera::FrameBuffer>> &buffers = allocator->buffers(stream);

    unsigned int imgSize = streamConfig.size.height * streamConfig.size.width;
    if(buffers.max_size() < imgSize) {
      this->log_WARNING_HI_InvalidBufferSizeError(buffers.max_size(), imgSize);
      allocator->free(stream);
      delete allocator;
      m_camera->release();
      return;
    }

    std::vector<std::unique_ptr<libcamera::Request>> requests;
  
    for (unsigned int i = 0; i < buffers.size(); i++) {
      std::unique_ptr<libcamera::Request> request = m_capture->createRequest();
      if (request) {
        const std::unique_ptr<libcamera::FrameBuffer> &buffer = buffers[i];
        int returnCode = request->addBuffer(stream, buffer.get());
        // success return code
        if(returnCode == 0) {
          requests.push_back(std::move(request));
        }
      }
    }
    m_capture->requestCompleted.connect(requestComplete);
    
    m_capture->start();
    for (std::unique_ptr<libcamera::Request> &request : requests) {
      camera->queueRequest(request.get());
    }
    sleep(3);
    if (requestReceived->status() == libcamera::Request::RequestComplete) {
      const libcamera::Request::BufferMap &buffers = requestReceived->buffers();
      for (auto bufferPair : buffers) {
        libcamera::FrameBuffer *buffer = bufferPair.second;
        switch (cameraAction.e) {
          case CameraAction::PROCESS:
            // need to figure out how to specify the libcamera FrameBuffer type in FPP
            rawImageData.setimgData(buffer);
            rawImageData.setheight(frame.rows);
            rawImageData.setwidth(frame.cols);
            rawImageData.setpixelFormat(frame.type());
            this->log_ACTIVITY_LO_CameraProcess();
            this->process_out(0, rawImageData);
            break;
          case CameraAction::SAVE:
            // need to figure out how to specify the libcamera FrameBuffer type in FPP
            this->save_out(0, buffer);
            this->log_ACTIVITY_LO_CameraSave();
            break;
          default:
            FW_ASSERT(0);
        }
      }
    }
    // no data, send blank frame event
    else {
      this->log_WARNING_HI_BlankFrame();
      m_camera->stop();
      allocator->free(stream);
      delete allocator;
      m_camera->release();
      return;
    }

    // need to do after the image has been processed/saved
    m_camera->stop();
    allocator->free(stream);
    delete allocator;
    m_camera->release();


    /*std::vector<uchar> buffer;
    cv::Mat frame;
    m_capture.read(frame);

    if (frame.empty()) {
      this->log_WARNING_HI_BlankFrame();
      return;
    }

    FW_ASSERT(imgSize == frame.total()*frame.elemSize(), imgSize, frame.total()*frame.elemSize());
    FW_ASSERT(frame.isContinuous());

    memcpy(imgBuffer.getData(), frame.data, imgSize);
    imgBuffer.setSize(imgSize);

    switch (cameraAction.e) {
      case CameraAction::PROCESS:
        rawImageData.setimgData(imgBuffer);
        rawImageData.setheight(frame.rows);
        rawImageData.setwidth(frame.cols);
        rawImageData.setpixelFormat(frame.type());
        this->log_ACTIVITY_LO_CameraProcess();
        this->process_out(0, rawImageData);
        break;
      case CameraAction::SAVE:
        this->save_out(0, imgBuffer);
        this->log_ACTIVITY_LO_CameraSave();
        break;
      default:
        FW_ASSERT(0);
      }
    */
    m_photoCount++;
    this->tlmWrite_photosTaken(m_photoCount);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void Camera ::ConfigImg_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq,
                                    Payload::ImgResolution resolution) {
    bool widthStatus = true;
    bool heightStatus = true;

    // configure camera
    // specify configuration type as a "StillCapture" (high resolution & high-quality still images with low frame rate)
    std::unique_ptr<libcamera::CameraConfiguration> config = m_capture->generateConfiguration( { libcamera::StreamRole::StillCapture } );
    libcamera::StreamConfiguration streamConfig = config->at(0);
    // set the image resolution (number of pixels for height and width)
    switch (resolution.e) {
      case ImgResolution::SIZE_640x480:
        streamConfig.size.height = 640;
        streamConfig.size.width = 480;
        break;
      case ImgResolution::SIZE_800x600:
        streamConfig.size.height = 800;
        streamConfig.size.width = 600;
        break;
      default:
        FW_ASSERT(0);
    }

    // check to see if the configuration was valid
    libcamera::CameraConfiguration::Status validationStatus = config->validate();
    if (validationStatus != libcamera::CameraConfiguration::Status::Invalid) {
      // apply the configuration and keep track of the return code
      int returnCode = m_capture->configure(config.get());

      // check to see if an error occurred and emit an ImgConfigSetFail event
      if (returnCode != 0) {
        this->log_WARNING_HI_ImgConfigSetFail(resolution);
        return;
      }

      this->log_ACTIVITY_HI_SetImgConfig(resolution);
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    }
    else {
      // configuration was not valid, emit an ImgConfigSetFail event
      this->log_WARNING_HI_ImgConfigSetFail(resolution);
    }
  }
} // end namespace Payload
