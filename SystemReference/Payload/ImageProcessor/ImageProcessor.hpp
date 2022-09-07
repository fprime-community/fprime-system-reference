// ======================================================================
// \title  ProcessImage.hpp
// \author vbai
// \brief  hpp file for ProcessImage component implementation class
// ======================================================================

#ifndef ProcessImage_HPP
#define ProcessImage_HPP

#include "SystemReference/Payload/ImageProcessor/ImageProcessorComponentAc.hpp"
#include <string>

namespace Payload {

class ImageProcessor : public ImageProcessorComponentBase {

public:
  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------
  static const U32 BUFFER_SIZE = 10*1024*1024;
  //! Construct object ProcessImage
  //!
  ImageProcessor(const char *const compName /*!< The component name*/
  );

  //! Initialize object ProcessImage
  //!
  void init(const NATIVE_INT_TYPE queueDepth,  /*!< The queue depth*/
            const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
  );

  //! Destroy object ProcessImage
  //!
  ~ImageProcessor();

  PRIVATE :

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation to take in image data to process
      //!
      void imageData_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Payload::RawImageData &ImageData /*!< Port that carries raw image data*/
      );


  PRIVATE :

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for SetFormat command handler
      //! Set the format to convert to from raw image file
      void
      SetFormat_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq,          /*!< The command sequence number*/
          Payload::FileFormat
              fileFormat /*!< Type of file format to convert to*/
      );

  const std::string m_PNG = ".png";
  const std::string m_JPG = ".jpg";

  // create as a pointer
  const std::string *m_fileFormat;
};

} // end namespace Payload

#endif
