#ifndef SystemReferenceTopologyDefs_HPP
#define SystemReferenceTopologyDefs_HPP

#include "Drv/BlockDriver/BlockDriver.hpp"
#include "Fw/Types/MallocAllocator.hpp"
#include "SystemReference/Top/FppConstantsAc.hpp"
#include "Svc/FramingProtocol/FprimeProtocol.hpp"
#include "Fw/Logger/Logger.hpp"

namespace SystemReference {

  namespace Allocation {

    // Malloc allocator for topology construction
    extern Fw::MallocAllocator mallocator;

  }

  // State for topology construction
  struct TopologyState {
    TopologyState() :
      hostName(""),
      portNumber(0)
    {

    }
    TopologyState(
        const char *hostName,
        U32 portNumber
    ) :
      hostName(hostName),
      portNumber(portNumber)
    {

    }
    const char* hostName;
    U32 portNumber;
  };

  // Health ping entries
  namespace PingEntries {
    namespace SystemReference_blockDrv { enum { WARN = 3, FATAL = 5 }; }
    namespace SystemReference_chanTlm { enum { WARN = 3, FATAL = 5 }; }
    namespace SystemReference_cmdDisp { enum { WARN = 3, FATAL = 5 }; }
    namespace SystemReference_cmdSeq { enum { WARN = 3, FATAL = 5 }; }
    namespace SystemReference_eventLogger { enum { WARN = 3, FATAL = 5 }; }
    namespace SystemReference_fileDownlink { enum { WARN = 3, FATAL = 5 }; }
    namespace SystemReference_fileManager { enum { WARN = 3, FATAL = 5 }; }
    namespace SystemReference_fileUplink { enum { WARN = 3, FATAL = 5 }; }
    namespace SystemReference_prmDb { enum { WARN = 3, FATAL = 5 }; }
    namespace SystemReference_rateGroup1Comp { enum { WARN = 3, FATAL = 5 }; }
    namespace SystemReference_rateGroup2Comp { enum { WARN = 3, FATAL = 5 }; }
    namespace SystemReference_rateGroup3Comp { enum { WARN = 3, FATAL = 5 }; }
    namespace SystemReference_saveImageBufferLogger { enum {WARN = 3, FATAL = 5}; }
    namespace SystemReference_imageProcessor { enum {WARN = 3, FATAL = 5}; }
    namespace SystemReference_processedImageBufferLogger { enum {WARN = 3, FATAL = 5}; }
  }

}

#endif
