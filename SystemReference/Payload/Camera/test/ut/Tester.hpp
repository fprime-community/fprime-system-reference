// ======================================================================
// \title  Camera/test/ut/Tester.hpp
// \author pel
// \brief  hpp file for Camera test harness implementation class
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "GTestBase.hpp"
#include "SystemReference/Payload/Camera/Camera.hpp"
#include <Fw/Types/BasicTypes.hpp>
#include <opencv2/opencv.hpp>


namespace Payload {

  class Tester :
    public CameraGTestBase
  {

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

      //! Test for camera save command
      //!
      void testCameraActionSave();

      //! Test for camera process command
      //!
      void testCameraActionProcess();

      //! Test for blank frame event
      //!
      void testBlankFrame();

      //! Test for bad buffer size event for raw image
      //!
      void testBadBufferRawImg();

    private:

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_allocate
      //!
      Fw::Buffer from_allocate_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 size 
      );

      //! Handler for from_deallocate
      //!
      void from_deallocate_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer
      );

      //! Handler for from_process
      //!
      void from_process_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Payload::RawImageData &ImageData /*!< Port that carries raw image data*/
      );

      //! Handler for from_save
      //!
      void from_save_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer
      );

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
      Camera component;

      U32 m_bufferSize;

  };

} // end namespace Payload

#endif
