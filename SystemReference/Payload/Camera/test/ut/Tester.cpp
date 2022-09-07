// ======================================================================
// \title  Camera.hpp
// \author vbai
// \brief  cpp file for Camera test harness implementation class
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
    : CameraGTestBase("Tester", MAX_HISTORY_SIZE), component("Camera") {
  component.m_capture.open("/Users/vbai/fprime-stuff/fprime-system-reference/SystemReference/Payload/Camera/test/ut/TEST_1.mov");
  this->initComponents();
  this->connectPorts();
}

Tester ::~Tester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester::testCameraActionSave() {
  CameraAction cameraAction = CameraAction::SAVE;
  this->sendCmd_TakeAction(0, 0, cameraAction);
  this->component.doDispatch();
  ASSERT_EVENTS_SIZE(1);
  ASSERT_EVENTS_CameraSave_SIZE(1);
  ASSERT_TLM_SIZE(1);
  ASSERT_TLM_photosTaken_SIZE(1);
  ASSERT_CMD_RESPONSE_SIZE(1);
  ASSERT_CMD_RESPONSE(0, Camera::OPCODE_TAKEACTION, 0, Fw::CmdResponse::OK);
  this->clearHistory();
}

void Tester::testCameraActionProcess() {
  CameraAction cameraAction = CameraAction::PROCESS;
  this->sendCmd_TakeAction(0, 0, cameraAction);
  this->component.doDispatch();
  ASSERT_EVENTS_SIZE(1);
  ASSERT_EVENTS_CameraProcess_SIZE(1);
  ASSERT_TLM_SIZE(1);
  ASSERT_TLM_photosTaken_SIZE(1);
  ASSERT_CMD_RESPONSE_SIZE(1);
  ASSERT_CMD_RESPONSE(0, Camera::OPCODE_TAKEACTION, 0, Fw::CmdResponse::OK);
  this->clearHistory();
}


void Tester::testBlankFrame() {
  cv::Mat frame;
  do{
    this->component.m_capture.read(frame);
  }while(!frame.empty());

  CameraAction cameraAction = CameraAction::PROCESS;
  this->sendCmd_TakeAction(0, 0, cameraAction);
  this->component.doDispatch();
  ASSERT_EVENTS_SIZE(1);
  ASSERT_EVENTS_BlankFrame_SIZE(1);
}

void Tester::testBadBufferRawImg() {
    m_bufferSize = 0;
    cv::Mat frame;
    this->component.m_capture.read(frame);
    U32 imgSize = frame.rows * frame.cols * frame.elemSize();
    Fw::Buffer buffer(new U8[m_bufferSize], m_bufferSize);
    CameraAction cameraAction = CameraAction::PROCESS;
    this->sendCmd_TakeAction(0, 0, cameraAction);
    this->component.doDispatch();
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidBufferSizeError(0, m_bufferSize, imgSize);
    ASSERT_EVENTS_InvalidBufferSizeError_SIZE(1);
}


// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

Fw::Buffer Tester ::from_allocate_handler(const NATIVE_INT_TYPE portNum,
                                          U32 size) {
  size = m_bufferSize;
  this->pushFromPortEntry_allocate(size);
  Fw::Buffer buffer(new U8[size], size);
  return buffer;
}

void Tester ::from_deallocate_handler(const NATIVE_INT_TYPE portNum,
                                      Fw::Buffer &fwBuffer) {
  delete[] fwBuffer.getData();
  this->pushFromPortEntry_deallocate(fwBuffer);
}

void Tester ::from_process_handler(const NATIVE_INT_TYPE portNum,
                                   Payload::RawImageData &ImageData) {
  this->pushFromPortEntry_process(ImageData);
}

void Tester ::from_save_handler(const NATIVE_INT_TYPE portNum,
                                Fw::Buffer &fwBuffer) {
  this->pushFromPortEntry_save(fwBuffer);
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void Tester ::connectPorts() {

  // cmdIn
  this->connect_to_cmdIn(0, this->component.get_cmdIn_InputPort(0));

  // Log
  this->component.set_Log_OutputPort(0, this->get_from_Log(0));

  // LogText
  this->component.set_LogText_OutputPort(0, this->get_from_LogText(0));

  // Time
  this->component.set_Time_OutputPort(0, this->get_from_Time(0));

  // Tlm
  this->component.set_Tlm_OutputPort(0, this->get_from_Tlm(0));

  // allocate
  this->component.set_allocate_OutputPort(0, this->get_from_allocate(0));

  // cmdRegOut
  this->component.set_cmdRegOut_OutputPort(0, this->get_from_cmdRegOut(0));

  // cmdResponseOut
  this->component.set_cmdResponseOut_OutputPort(
      0, this->get_from_cmdResponseOut(0));

  // deallocate
  this->component.set_deallocate_OutputPort(0, this->get_from_deallocate(0));

  // process
  this->component.set_process_OutputPort(0, this->get_from_process(0));

  // save
  this->component.set_save_OutputPort(0, this->get_from_save(0));
}

void Tester ::initComponents() {
  this->init();
  this->component.init(QUEUE_DEPTH, INSTANCE);
}

} // end namespace Payload
