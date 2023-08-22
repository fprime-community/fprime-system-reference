// ======================================================================
// \title  Camera.cpp
// \author vbai
// \brief  cpp file for Camera component implementation class
// ======================================================================

#include "Fw/Types/BasicTypes.hpp"
#include <SystemReference/Payload/Camera/Camera.hpp>
#include <libcamera/libcamera.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>

namespace Payload {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  Camera ::Camera(const char *const compName): CameraComponentBase(compName), m_photoCount(0){}

  void Camera ::init(const NATIVE_INT_TYPE queueDepth, const NATIVE_INT_TYPE instance) {
    CameraComponentBase::init(queueDepth, instance);
    camManager = std::make_unique<libcamera::CameraManager>();
  }

  bool Camera::open(I32 deviceIndex) {
    int returnCode = camManager->start();
    // check to see if the camera manager was started successfully
    // and if any cameras were detected
    if (returnCode == 0 && !camManager->cameras().empty()) {
      // get the camera from the CameraManager and acquire it
      m_capture = camManager->cameras()[deviceIndex];
      int returnCode = m_capture->acquire();

      // camera was acquired successfully
      if (returnCode == 0) {
        // setCameraConfiguration(ImgResolution::SIZE_640x480);
        // allocateBuffers();
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

  void Camera ::allocateBuffers() {
    printf("Allocating buffers\n");
    allocator = new libcamera::FrameBufferAllocator(m_capture);
    libcamera::StreamConfiguration streamConfig = cameraConfig->at(0);
    libcamera::Stream *stream = streamConfig.stream();
    // Set up buffer for image data
    allocator->allocate(stream);
  }

  void Camera ::createBufferMap() {
    libcamera::StreamConfiguration streamConfig = cameraConfig->at(0);
    libcamera::Stream *stream = streamConfig.stream();
    const std::vector<std::unique_ptr<libcamera::FrameBuffer>> &buffers = allocator->buffers(stream);

    for (const std::unique_ptr<libcamera::FrameBuffer> &buffer : buffers) {
      size_t buffer_size = 0;
      for (unsigned i = 0; i < buffer->planes().size(); i++) {
        const libcamera::FrameBuffer::Plane &plane = buffer->planes()[i];
        buffer_size += plane.length;
        if (i == buffer->planes().size() - 1 || plane.fd.get() != buffer->planes()[i + 1].fd.get()) {
          void *memory = mmap(NULL, buffer_size, PROT_READ | PROT_WRITE, MAP_SHARED, plane.fd.get(), 0);
          mappedBuffers[buffer.get()].push_back(libcamera::Span<uint8_t>(static_cast<uint8_t *>(memory), buffer_size));
          buffer_size = 0;
        }
      }
      frameBuffers[stream].push(buffer.get());
	  }
  }

  void Camera::configureRequests() {
    auto free_buffers(frameBuffers);
    printf("Creating requests\n");
    libcamera::Stream *stream = cameraConfig->at(0).stream();
    if (free_buffers[stream].empty()) {
      return;
    }
    else {
      // request was previously created, re-use it
      if(frameRequest) {
        frameRequest->reuse(libcamera::Request::ReuseBuffers);
      }
      else {
        // create an empty frame capture request for the application to fill with buffers
        std::unique_ptr<libcamera::Request> request = m_capture->createRequest();
        if(request) {
            frameRequest = std::move(request);
            libcamera::FrameBuffer *buffer = free_buffers[stream].front();
            free_buffers[stream].pop();
            int returnCode = frameRequest->addBuffer(stream, buffer);
            // success return code
            if(returnCode == 0) {
            }
        }
      }
    }
    m_capture->requestCompleted.connect(requestComplete);
  }

  void Camera ::cleanupRequests() {
    m_capture->requestCompleted.disconnect(requestComplete);
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void Camera ::CaptureImage_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq) {
    if(!m_capture) {
      this->log_WARNING_HI_CameraCaptureFail();
      return;
    }

    setCameraConfiguration(currentResolution);
    allocateBuffers();

    createBufferMap();
    Fw::Buffer imgBuffer;

    configureRequests();
    
    if(!cameraStarted) {
      m_capture->start();
      cameraStarted = true;
    }
    // queue request for capture
    int res = m_capture->queueRequest(frameRequest.get());
    // wait a few seconds
    sleep(3);
    // check to see if the request was completed
    if (requestReceived && requestReceived->status() == libcamera::Request::RequestComplete) {
      const libcamera::Request::BufferMap &buffers = requestReceived->buffers();
      for (auto bufferPair : buffers) {
        libcamera::FrameBuffer *buffer = bufferPair.second;
        const libcamera::FrameMetadata &metadata = buffer->metadata();
      
        imgBuffer = allocate_out(0, metadata.planes()[0].bytesused);
        // check to see if the buffer is the correct size
        // if not, emit a InvalidBufferSizeError event
        if(imgBuffer.getSize() < metadata.planes()[0].bytesused) {
          this->log_WARNING_HI_InvalidBufferSizeError(imgBuffer.getSize(), metadata.planes()[0].bytesused);
          // cleanup(allocator, stream, imgBuffer);
          return;
        }

        // copy data to framework buffer
        memcpy(imgBuffer.getData(), mappedBuffers.find(buffer)->second.back().data(), metadata.planes()[0].bytesused);
        // just for testing purposes
        std::ofstream MyFile("test_" + std::to_string(m_photoCount) + ".dat");
        for (int i = 0; i < metadata.planes()[0].bytesused; i++) {
          MyFile << mappedBuffers.find(buffer)->second.back().data()[i];
          // printf("%c", imgBuffer.getData()[i]);
        }
        MyFile.close();

        // this->save_out(0, imgBuffer);
        this->log_ACTIVITY_LO_CameraSave();
      }
    }
    // no data, send blank frame event
    else {
      // cleanup(allocator, stream, imgBuffer);
      this->log_WARNING_HI_BlankFrame();
      return;
    }

    // requestReceived = nullptr;
    printf("cleanup\n");
    // this->deallocate_out(0, imgBuffer);
    for (auto &iter : mappedBuffers) {
      for (auto &span : iter.second) {
        munmap(span.data(), span.size());
      }
    }
    mappedBuffers.clear();
    frameBuffers.clear();
    frameRequest = nullptr;
    m_capture->requestCompleted.disconnect(requestComplete);
    cleanup(cameraConfig->at(0).stream(), imgBuffer);
    m_photoCount++;
    this->tlmWrite_photosTaken(m_photoCount);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void Camera ::ConfigImg_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq,
                                    Payload::ImgResolution resolution) {

    if(setCameraConfiguration(resolution)) this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  // Other methods
  bool Camera ::setCameraConfiguration(ImgResolution resolution) {
     if(!m_capture) {
       this->log_WARNING_HI_ImgConfigSetFail(resolution);
      return false; 
    }
    if(cameraStarted) {
      m_capture->stop();
      cameraStarted = false;
    }

    // configure camera
    // specify configuration type as a "StillCapture" (high resolution & high-quality still images with low frame rate)
    cameraConfig = m_capture->generateConfiguration( { libcamera::StreamRole::StillCapture } );
    if(!cameraConfig) {
      this->log_WARNING_HI_ImgConfigSetFail(resolution);
      return false; 
    }

    libcamera::StreamConfiguration &streamConfig = cameraConfig->at(0);

    // set stream height and width
    switch (resolution.e) {
      case ImgResolution::SIZE_640x480:
        streamConfig.size.width = 640;
        streamConfig.size.height = 480;
        break;
      case ImgResolution::SIZE_800x600:
        streamConfig.size.width = 800;
        streamConfig.size.height = 600;
        break;
      default:
        FW_ASSERT(0);
    }
    
    // set the pixel format
    streamConfig.pixelFormat = libcamera::formats::RGB888;

    // check to see if the configuration was valid
    libcamera::CameraConfiguration::Status validationStatus = cameraConfig->validate();
    if (validationStatus != libcamera::CameraConfiguration::Status::Invalid) {
      printf("Validated camera configuration: ");
      printf(streamConfig.toString().c_str());
      // apply the configuration and keep track of the return code
      int returnCode = m_capture->configure(cameraConfig.get());

      // check to see if an error occurred and emit an ImgConfigSetFail event
      if (returnCode != 0) {
        this->log_WARNING_HI_ImgConfigSetFail(resolution);
        return false;
      }

      currentResolution = resolution;
      this->log_ACTIVITY_HI_SetImgConfig(resolution);
      return true;
    }
    else {
      // configuration was not valid, emit an ImgConfigSetFail event
      this->log_WARNING_HI_ImgConfigSetFail(resolution);
      return false;
    }
  }

  void Camera :: cleanup(libcamera::Stream *stream, Fw::Buffer imgBuffer) {
    if(cameraStarted) {
      m_capture->stop();
      cameraStarted = false;
    }
    for (auto &iter : mappedBuffers) {
      for (auto &span : iter.second) {
        munmap(span.data(), span.size());
      }
    }
    mappedBuffers.clear();
    frameBuffers.clear();
    frameRequest = nullptr;
    allocator->free(stream);
    // need to do after the image has been saved
    delete allocator;
    allocator = nullptr;
    printf("deallocate img buffer\n");
    this->deallocate_out(0, imgBuffer);
  }

} // end namespace Payload
