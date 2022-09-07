// ======================================================================
// \title  ImageProcessor.hpp
// \author vbai
// \brief  cpp file for ImageProcessor test harness implementation class
// ======================================================================

#include "Tester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10
#define QUEUE_DEPTH 10

namespace Payload {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

Tester ::Tester()
    : ImageProcessorGTestBase("Tester", MAX_HISTORY_SIZE),
      component("ImageProcessor") {
  this->initComponents();
  this->connectPorts();
}

Tester ::~Tester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester::testSetFormatJpg() {
  FileFormat fileFormat = FileFormat::JPG;
  this->sendCmd_SetFormat(0, 0, fileFormat);
  this->component.doDispatch();

  ASSERT_EVENTS_SIZE(1);
  ASSERT_EVENTS_SetFileFormat_SIZE(1);
  ASSERT_EVENTS_SetFileFormat(0, fileFormat);

  ASSERT_CMD_RESPONSE_SIZE(1);
  ASSERT_CMD_RESPONSE(0, ImageProcessor::OPCODE_SETFORMAT, 0, Fw::CmdResponse::OK);
}

void Tester::testSetFormatPng() {
  FileFormat fileFormat = FileFormat::PNG;
  this->sendCmd_SetFormat(0, 0, fileFormat);
  this->component.doDispatch();

  ASSERT_EVENTS_SIZE(1);
  ASSERT_EVENTS_SetFileFormat_SIZE(1);
  ASSERT_EVENTS_SetFileFormat(0, fileFormat);

  ASSERT_CMD_RESPONSE_SIZE(1);
  ASSERT_CMD_RESPONSE(0, ImageProcessor::OPCODE_SETFORMAT, 0, Fw::CmdResponse::OK);
}

void Tester::testImgProcessPng() {
  // Buffer of image that has 3 color channels and resolution of 640x480
  m_bufferSize = 640*480*3;
  const char *pngHeader = "PNG";
  Fw::Buffer encodeBuffer(new U8[m_bufferSize], m_bufferSize);
  this->component.m_fileFormat = &this->component.m_PNG;
  RawImageData rawImageData(1, 1, 1, encodeBuffer);
  this->invoke_to_imageData(0, rawImageData);
  this->component.doDispatch();

  ASSERT_from_bufferDeallocate_SIZE(1);
  ASSERT_from_postProcess_SIZE(1);

  Fw::Buffer encodeBufferOut = this->fromPortHistory_postProcess->at(0).fwBuffer;
  // Check that size of encode buffer is greater than the size of its header
  ASSERT_GT(encodeBuffer.getSize(), 8);
  // Check that file was encoded to proper format
  for(int index = 0; index < 3; index++){
    ASSERT_EQ(pngHeader[index], encodeBufferOut.getData()[index+1]);
  }
  cleanUp();
}

void Tester::testImgProcessJpg() {
  // Buffer of image that has 3 color channels and resolution of 640x480
  m_bufferSize = 640*480*3;
  const unsigned char jpgHeader[3] = {0xFF, 0xD8, 0xFF};
  Fw::Buffer encodeBuffer(new U8[m_bufferSize], m_bufferSize);
  this->component.m_fileFormat = &this->component.m_JPG;
  RawImageData rawImageData(1, 1, 1, encodeBuffer);
  this->invoke_to_imageData(0, rawImageData);
  this->component.doDispatch();

  ASSERT_from_bufferDeallocate_SIZE(1);
  ASSERT_from_postProcess_SIZE(1);

  Fw::Buffer encodeBufferOut = this->fromPortHistory_postProcess->at(0).fwBuffer;

  // Check that size of encode buffer is greater than the size of its header
  ASSERT_GT(encodeBuffer.getSize(), 3);
  // Check that file was encoded to proper format
  for(int index = 0; index < 3; index++){
    ASSERT_EQ(jpgHeader[index], encodeBufferOut.getData()[index]);
  }
  cleanUp();
}

void Tester::testNoImgData() {
  m_bufferSize = 0;
  Fw::Buffer buffer(new U8[m_bufferSize], m_bufferSize);
  RawImageData rawImageData(0, 0, 0, buffer);
  this->invoke_to_imageData(0, rawImageData);
  this->component.doDispatch();
  ASSERT_EVENTS_SIZE(1);
  ASSERT_EVENTS_NoImgData_SIZE(1);
  cleanUp();
}

void Tester::testBadBufferProcess() {
  m_bufferSize = 0;
  Fw::Buffer buffer(new U8[m_bufferSize], m_bufferSize);
  RawImageData rawImageData(1, 1, 1, buffer);
  this->component.m_fileFormat = &this->component.m_PNG;
  this->invoke_to_imageData(0, rawImageData);
  this->component.doDispatch();
  ASSERT_EVENTS_SIZE(1);
  ASSERT_EVENTS_BadBufferSize_SIZE(1);
  cleanUp();
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

Fw::Buffer Tester ::from_bufferAllocate_handler(const NATIVE_INT_TYPE portNum,
                                                U32 size) {
  size = m_bufferSize;
  this->pushFromPortEntry_bufferAllocate(size);
  Fw::Buffer buffer(new U8[size], size);
  return buffer;
}

void Tester ::from_bufferDeallocate_handler(const NATIVE_INT_TYPE portNum,
                                            Fw::Buffer &fwBuffer) {
  delete[] fwBuffer.getData();
  this->pushFromPortEntry_bufferDeallocate(fwBuffer);
}

void Tester ::from_postProcess_handler(const NATIVE_INT_TYPE portNum,
                                       Fw::Buffer &fwBuffer) {
  this->pushFromPortEntry_postProcess(fwBuffer);
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void Tester ::connectPorts() {

  // cmdIn
  this->connect_to_cmdIn(0, this->component.get_cmdIn_InputPort(0));

  // imageData
  this->connect_to_imageData(0, this->component.get_imageData_InputPort(0));

  // Log
  this->component.set_Log_OutputPort(0, this->get_from_Log(0));

  // LogText
  this->component.set_LogText_OutputPort(0, this->get_from_LogText(0));

  // Time
  this->component.set_Time_OutputPort(0, this->get_from_Time(0));

  // Tlm
  this->component.set_Tlm_OutputPort(0, this->get_from_Tlm(0));

  // bufferAllocate
  this->component.set_bufferAllocate_OutputPort(
      0, this->get_from_bufferAllocate(0));

  // bufferDeallocate
  this->component.set_bufferDeallocate_OutputPort(
      0, this->get_from_bufferDeallocate(0));

  // cmdRegOut
  this->component.set_cmdRegOut_OutputPort(0, this->get_from_cmdRegOut(0));

  // cmdResponseOut
  this->component.set_cmdResponseOut_OutputPort(
      0, this->get_from_cmdResponseOut(0));

  // postProcess
  this->component.set_postProcess_OutputPort(0, this->get_from_postProcess(0));
}

void Tester ::initComponents() {
  this->init();
  this->component.init(QUEUE_DEPTH, INSTANCE);
}

void Tester::cleanUp() {
  for (int i = 0; i < this->fromPortHistory_postProcess->size(); i++) {
    delete[] this->fromPortHistory_postProcess->at(i).fwBuffer.getData();
  }
  this->fromPortHistory_postProcess->clear();
}

} // end namespace Payload
