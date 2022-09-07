// ======================================================================
// \title  Camera.cpp
// \author vbai
// \brief  cpp file for Camera component implementation class
// ======================================================================

#include "Fw/Types/BasicTypes.hpp"
#include <SystemReference/Payload/Camera/Camera.hpp>

namespace Payload {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

Camera ::Camera(const char *const compName)
    : CameraComponentBase(compName), m_photoCount(0){}

void Camera ::init(const NATIVE_INT_TYPE queueDepth,
                   const NATIVE_INT_TYPE instance) {
  CameraComponentBase::init(queueDepth, instance);
}

bool Camera::open(I32 deviceIndex) {
#ifdef USES_OPENCV
  if (m_capture.isOpened()) {
    this->log_ACTIVITY_LO_CameraAlreadyOpen();
    return true;
  }
  m_capture.open(deviceIndex);
  if (!m_capture.isOpened()) {
    this->log_WARNING_HI_CameraOpenError();
    return false;
  }
#endif
  return true;
}

Camera ::~Camera() {}

// ----------------------------------------------------------------------
// Command handler implementations
// ----------------------------------------------------------------------

void Camera ::TakeAction_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq,
                                    Payload::CameraAction cameraAction) {
  RawImageData rawImageData;
  Fw::Buffer imgBuffer;
#ifdef USES_OPENCV
  std::vector<uchar> buffer;
  cv::Mat frame;
  m_capture.read(frame);

  if (frame.empty()) {
    this->log_WARNING_HI_BlankFrame();
    return;
  }

  // Set up buffer for image data
  U32 imgSize = frame.rows * frame.cols * frame.elemSize();
  imgBuffer = allocate_out(0, imgSize);
  if (imgBuffer.getSize() < imgSize){
    this->log_WARNING_HI_InvalidBufferSizeError(imgBuffer.getSize(), imgSize);
    this->deallocate_out(0, imgBuffer);
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
#endif
  m_photoCount++;
  this->tlmWrite_photosTaken(m_photoCount);
  this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void Camera ::ConfigImg_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq,
                                   Payload::ImgResolution resolution) {
  bool widthStatus = true;
  bool heightStatus = true;
#ifdef USES_OPENCV
  switch (resolution.e) {
    case ImgResolution::SIZE_640x480:
      widthStatus = m_capture.set(cv::CAP_PROP_FRAME_WIDTH, 640);
      heightStatus = m_capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
      break;
    case ImgResolution::SIZE_800x600:
      widthStatus = m_capture.set(cv::CAP_PROP_FRAME_WIDTH, 800);
      heightStatus = m_capture.set(cv::CAP_PROP_FRAME_HEIGHT, 600);
      break;
    default:
      FW_ASSERT(0);
  }

  if((widthStatus == 0) || (heightStatus == 0)){
    this->log_WARNING_HI_ImgConfigSetFail(resolution);
    return;
  }
#endif
  this->log_ACTIVITY_HI_SetImgConfig(resolution);
  this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

} // end namespace Payload
