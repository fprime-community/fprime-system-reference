// ======================================================================
// \title  ProcessImage.cpp
// \author vbai
// \brief  cpp file for ProcessImage component implementation class
// ======================================================================

#include "Fw/Types/BasicTypes.hpp"
#include <SystemReference/Payload/ImageProcessor/ImageProcessor.hpp>
#ifdef USES_OPENCV
#include <opencv2/opencv.hpp>
#endif

//static const NATIVE_INT_TYPE BUFFER_SIZE = 10*1024*1024;
namespace Payload {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

ImageProcessor ::ImageProcessor(const char *const compName)
    : ImageProcessorComponentBase(compName), m_fileFormat(&m_PNG) {}

void ImageProcessor ::init(const NATIVE_INT_TYPE queueDepth,
                           const NATIVE_INT_TYPE instance) {
  ImageProcessorComponentBase::init(queueDepth, instance);
}

ImageProcessor ::~ImageProcessor() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ---------------------------------------------------------------------+

void ImageProcessor ::imageData_handler(const NATIVE_INT_TYPE portNum,
                                        Payload::RawImageData &ImageData) {
#ifdef USES_OPENCV
  std::vector<uchar> buffer(ImageProcessor::BUFFER_SIZE);
  cv::Mat image(ImageData.getheight(), ImageData.getwidth(), ImageData.getpixelFormat(), (void *)ImageData.getimgData().getData());
  if (image.empty()) {
    this->bufferDeallocate_out(0, const_cast<Fw::Buffer &>(ImageData.getimgData()));
    this->log_WARNING_HI_NoImgData();
    return;
  }
  cv::imencode(*m_fileFormat, image, buffer);
  Fw::Buffer encodeBuffer = bufferAllocate_out(0, buffer.size());
  if(encodeBuffer.getSize() == 0){
    this->log_WARNING_HI_BadBufferSize(encodeBuffer.getSize(), buffer.size());
    this->bufferDeallocate_out(0, const_cast<Fw::Buffer &>(ImageData.getimgData()));
    return;
  }
  FW_ASSERT(encodeBuffer.getSize() >= buffer.size(), encodeBuffer.getSize(), buffer.size());
  memcpy(encodeBuffer.getData(), buffer.data(), buffer.size());
  encodeBuffer.setSize(buffer.size());
  this->postProcess_out(0, encodeBuffer);
#endif
  this->bufferDeallocate_out(0, const_cast<Fw::Buffer &>(ImageData.getimgData()));
}

// ----------------------------------------------------------------------
// Command handler implementations
// ----------------------------------------------------------------------

void ImageProcessor ::SetFormat_cmdHandler(const FwOpcodeType opCode,
                                           const U32 cmdSeq,
                                           Payload::FileFormat fileFormat) {
  switch (fileFormat.e){
    case FileFormat::JPG:
      m_fileFormat = &m_JPG;
      this->log_ACTIVITY_HI_SetFileFormat(fileFormat);
      break;
    case FileFormat::PNG:
      m_fileFormat = &m_PNG;
      this->log_ACTIVITY_HI_SetFileFormat(fileFormat);
      break;
    default:
      FW_ASSERT(0);
    }
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

} // end namespace Payload
