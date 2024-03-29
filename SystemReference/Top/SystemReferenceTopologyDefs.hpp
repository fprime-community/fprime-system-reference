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
    namespace blockDrv { enum { WARN = 3, FATAL = 5 }; }
    namespace chanTlm { enum { WARN = 3, FATAL = 5 }; }
    namespace cmdDisp { enum { WARN = 3, FATAL = 5 }; }
    namespace cmdSeq { enum { WARN = 3, FATAL = 5 }; }
    namespace eventLogger { enum { WARN = 3, FATAL = 5 }; }
    namespace fileDownlink { enum { WARN = 3, FATAL = 5 }; }
    namespace fileManager { enum { WARN = 3, FATAL = 5 }; }
    namespace fileUplink { enum { WARN = 3, FATAL = 5 }; }
    namespace prmDb { enum { WARN = 3, FATAL = 5 }; }
    namespace rateGroup1Comp { enum { WARN = 3, FATAL = 5 }; }
    namespace rateGroup2Comp { enum { WARN = 3, FATAL = 5 }; }
    namespace rateGroup3Comp { enum { WARN = 3, FATAL = 5 }; }
    namespace saveImageBufferLogger { enum {WARN = 3, FATAL = 5}; }
    namespace imageProcessor { enum {WARN = 3, FATAL = 5}; }
    namespace processedImageBufferLogger { enum {WARN = 3, FATAL = 5}; }
  }

}

#endif
