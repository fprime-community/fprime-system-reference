// ======================================================================
// \title  ImageProcessor/test/ut/Tester.hpp
// \author vbai
// \brief  hpp file for ImageProcessor test harness implementation class
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "GTestBase.hpp"
#include "SystemReference/Payload/ImageProcessor/ImageProcessor.hpp"
#include "opencv2/opencv.hpp"

namespace Payload {

class Tester : public ImageProcessorGTestBase {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

public:
  //! Construct object Tester
  //!
  Tester();

  //! Destroy object Tester
  //!
  ~Tester();

public:
  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  //! Test for setting jpg file format
  //!
  void testSetFormatJpg();

  //! Test for setting png file format
  //!
  void testSetFormatPng();

  //! Test that image process behavior is correct for png files
  //!
  void testImgProcessPng();

  //! Test that image process behavior is correct for jpg files
  //!
  void testImgProcessJpg();

  //! Test for bad buffer size event
  //!
  void testBadBufferProcess();

  //! Test to check that image data was converted to a opencv matrix
  void testNoImgData();


private:
  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  //! Handler for from_bufferAllocate
  //!
  Fw::Buffer from_bufferAllocate_handler(
      const NATIVE_INT_TYPE portNum, /*!< The port number*/
      U32 size);

  //! Handler for from_bufferDeallocate
  //!
  void from_bufferDeallocate_handler(
      const NATIVE_INT_TYPE portNum, /*!< The port number*/
      Fw::Buffer &fwBuffer);

  //! Handler for from_postProcess
  //!
  void
  from_postProcess_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                           Fw::Buffer &fwBuffer);

  void cleanUp();

private:
  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  //! Connect ports
  //!
  void connectPorts();

  //! Initialize components
  //!
  void initComponents();

private:
  // ----------------------------------------------------------------------
  // Variables
  // ----------------------------------------------------------------------

  //! The component under test
  //!
  ImageProcessor component;
  U32 m_bufferSize;
};

} // end namespace Payload

#endif
