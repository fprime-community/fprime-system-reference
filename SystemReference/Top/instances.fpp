module SystemReference {

  # ----------------------------------------------------------------------
  # Defaults
  # ----------------------------------------------------------------------

  module Default {

    constant queueSize = 10

    constant stackSize = 64 * 1024

  }

  # ----------------------------------------------------------------------
  # Active component instances
  # ----------------------------------------------------------------------

  instance rateGroup1Comp: Svc.ActiveRateGroup base id 0x0200 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 120 \
  {

    phase Fpp.ToCpp.Phases.configObjects """
    U32 context[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    """

    phase Fpp.ToCpp.Phases.configComponents """
    rateGroup1Comp.configure(
        ConfigObjects::SystemReference_rateGroup1Comp::context,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::SystemReference_rateGroup1Comp::context)
    );
    """

  }

  instance rateGroup2Comp: Svc.ActiveRateGroup base id 0x0300 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 119 \
  {

    phase Fpp.ToCpp.Phases.configObjects """
    U32 context[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    """

    phase Fpp.ToCpp.Phases.configComponents """
    rateGroup2Comp.configure(
        ConfigObjects::SystemReference_rateGroup2Comp::context,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::SystemReference_rateGroup2Comp::context)
    );
    """

  }

  instance rateGroup3Comp: Svc.ActiveRateGroup base id 0x0400 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 118 \
  {

    phase Fpp.ToCpp.Phases.configObjects """
    U32 context[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    """

    phase Fpp.ToCpp.Phases.configComponents """
    rateGroup3Comp.configure(
        ConfigObjects::SystemReference_rateGroup3Comp::context,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::SystemReference_rateGroup3Comp::context)
    );
    """

  }

  instance cmdDisp: Svc.CommandDispatcher base id 0x0500 \
    queue size 20 \
    stack size Default.stackSize \
    priority 101

  instance comQueue: Svc.ComQueue base id 0x0600 \
      queue size 200 \
      stack size Default.stackSize \
      priority 100 \
  {
    phase Fpp.ToCpp.Phases.configObjects """
    Svc::ComQueue::QueueConfigurationTable configurationTable;
    """

    phase Fpp.ToCpp.Phases.configComponents """
    // Channels, deep queue, low priority
    ConfigObjects::SystemReference_comQueue::configurationTable.entries[0] = {.depth = 500, .priority = 2};
    // Events , highest-priority
    ConfigObjects::SystemReference_comQueue::configurationTable.entries[1] = {.depth = 100, .priority = 0};
    // File Downlink
    ConfigObjects::SystemReference_comQueue::configurationTable.entries[2] = {.depth = 100, .priority = 1};
    // Allocation identifier is 0 as the MallocAllocator discards it
    comQueue.configure(ConfigObjects::SystemReference_comQueue::configurationTable, 0, Allocation::mallocator);
    """
  }

  instance cmdSeq: Svc.CmdSequencer base id 0x0700 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 100 \
  {
    phase Fpp.ToCpp.Phases.configConstants """
    enum {
      BUFFER_SIZE = 5*1024
    };
    """

    phase Fpp.ToCpp.Phases.configComponents """
    cmdSeq.allocateBuffer(
        0,
        Allocation::mallocator,
        ConfigConstants::SystemReference_cmdSeq::BUFFER_SIZE
    );
    """

    phase Fpp.ToCpp.Phases.tearDownComponents """
    cmdSeq.deallocateBuffer(Allocation::mallocator);
    """

  }

  instance fileDownlink: Svc.FileDownlink base id 0x0800 \
    queue size 30 \
    stack size Default.stackSize \
    priority 100 \
  {

    phase Fpp.ToCpp.Phases.configConstants """
    enum {
      TIMEOUT = 1000,
      COOLDOWN = 1000,
      CYCLE_TIME = 1000,
      FILE_QUEUE_DEPTH = 10
    };
    """

    phase Fpp.ToCpp.Phases.configComponents """
    fileDownlink.configure(
        ConfigConstants::SystemReference_fileDownlink::TIMEOUT,
        ConfigConstants::SystemReference_fileDownlink::COOLDOWN,
        ConfigConstants::SystemReference_fileDownlink::CYCLE_TIME,
        ConfigConstants::SystemReference_fileDownlink::FILE_QUEUE_DEPTH
    );
    """

  }

  instance fileManager: Svc.FileManager base id 0x0900 \
    queue size 30 \
    stack size Default.stackSize \
    priority 100

  instance fileUplink: Svc.FileUplink base id 0x0A00 \
    queue size 30 \
    stack size Default.stackSize \
    priority 100

  instance eventLogger: Svc.EventManager base id 0x0B00 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 98

  #instance tlmSend: Svc.TlmChan base id 0x0C00 \
  #  queue size Default.queueSize \
  #  stack size Default.stackSize \
  #  priority 97

  instance tlmSend: Svc.TlmPacketizer base id 0x0C00 \
      queue size Default.queueSize \
      stack size Default.stackSize \
      priority 97 \
  {
    phase Fpp.ToCpp.Phases.configComponents """
    tlmSend.setPacketList(
       SystemReference::SystemReference_SystemReferencePacketsTlmPackets::packetList,
       SystemReference::SystemReference_SystemReferencePacketsTlmPackets::omittedChannels,
       1
    );
    """  
  }

  instance prmDb: Svc.PrmDb base id 0x0D00 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 96 \
  {
    phase Fpp.ToCpp.Phases.instances """
    Svc::PrmDb prmDb(FW_OPTIONAL_NAME("prmDb"));
    """
    phase Fpp.ToCpp.Phases.configComponents """
    prmDb.configure("PrmDb.dat");
    """
    phase Fpp.ToCpp.Phases.readParameters """
    prmDb.readParamFile();
    """
  }

  instance camera: Payload.Camera base id 0x0E00 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 100 \
    {
       phase Fpp.ToCpp.Phases.configComponents"""
       if (!camera.open(0)){
           Fw::Logger::log("[ERROR] Failed to open camera device\\n");
       }
       """
    }

  # saves two images before closing
  instance saveImageBufferLogger: Svc.BufferLogger base id 0x0F00 \
    queue size 30 \
    stack size Default.stackSize \
    priority 100 \
    {
        phase Fpp.ToCpp.Phases.configConstants """
        enum {
            MAX_FILE_SIZE = 1024*1024,
            SIZE_OF_SIZE = 4,
            };
        """

        phase Fpp.ToCpp.Phases.configComponents """
        const char* const name = "/home/pi/images/saveImage";
        const char* const type = ".dat";
        saveImageBufferLogger.initLog(
            name,
            type,
            ConfigConstants::SystemReference_saveImageBufferLogger::MAX_FILE_SIZE,
            ConfigConstants::SystemReference_saveImageBufferLogger::SIZE_OF_SIZE
        );
        """

    }


# XBee Radio Integration
#  instance radio: Com.XBee base id 0x1200 \
#    queue size Default.queueSize \
#    stack size Default.stackSize \
#    priority 140

  # ----------------------------------------------------------------------
  # Queued component instances
  # ----------------------------------------------------------------------

  instance $health: Svc.Health base id 0x2000 \
    queue size 25 \
  {

    phase Fpp.ToCpp.Phases.configConstants """
    enum {
      WATCHDOG_CODE = 0x123
    };
    """

    phase Fpp.ToCpp.Phases.configComponents """
    health.setPingEntries(
        ConfigObjects::SystemReference_health::pingEntries,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::SystemReference_health::pingEntries),
        ConfigConstants::SystemReference_health::WATCHDOG_CODE
    );
    """

  }

  # ----------------------------------------------------------------------
  # Passive component instances
  # ----------------------------------------------------------------------

  # NOTE: use only one of XBee or comStub
  # TCP/IP Integration
  instance radio: Svc.ComStub base id 0x1200

#  @ Communications driver. May be swapped with other comm drivers like UART
#  @ Note: Here we have TCP reliable uplink and UDP (low latency) downlink
  instance comDriver: Drv.TcpClient base id 0x4000 \
  {
    phase Fpp.ToCpp.Phases.configConstants """
    enum {
      PRIORITY = 100,
      STACK_SIZE = Default::stackSize
    };
    """

    phase Fpp.ToCpp.Phases.startTasks """
    // Initialize socket server if and only if there is a valid specification
    if (state.hostName != nullptr && state.portNumber != 0) {
        Os::TaskString name("ReceiveTask");
        // Uplink is configured for receive so a socket task is started
        comDriver.configure(state.hostName, state.portNumber);
        comDriver.start(
            name,
            ConfigConstants::SystemReference_comDriver::PRIORITY,
            ConfigConstants::SystemReference_comDriver::STACK_SIZE
        );
    }
    """

    phase Fpp.ToCpp.Phases.stopTasks """
    comDriver.stop();
    """
    

    phase Fpp.ToCpp.Phases.freeThreads """
    (void) comDriver.join();
    """

 }

# XBee Radio Integration
# instance comDriver: Drv.LinuxUartDriver base id 0x4000 {
#    phase Fpp.ToCpp.Phases.configConstants """
#    enum {
#      PRIORITY = 100,
#      STACK_SIZE = Default::stackSize
#    };
#    """
#
#    phase Fpp.ToCpp.Phases.startTasks """
#    // Initialize socket server if and only if there is a valid specification
#    if (state.hostName != nullptr) {
#        Os::TaskString name("ReceiveTask");
#        // Uplink is configured for receive so a socket task is started
#        if (comDriver.open(state.hostName, static_cast<Drv::LinuxUartDriver::UartBaudRate>(state.portNumber), Drv::LinuxUartDriver::NO_FLOW,
#                       Drv::LinuxUartDriver::PARITY_NONE, 1024)) {
#            comDriver.start(
#                ConfigConstants::SystemReference_comDriver::PRIORITY,
#                ConfigConstants::SystemReference_comDriver::STACK_SIZE
#            );
#        } else {
#            printf("Failed to open UART port %s at speed %" PRIu32 "\n", state.hostName, state.portNumber);
#        }
#    }
#    """
#
#    phase Fpp.ToCpp.Phases.stopTasks """
#    comDriver.quitReadThread();
#    """
#
#    phase Fpp.ToCpp.Phases.freeThreads """
#    comDriver.join();
#    """
# }



  instance framer: Svc.FprimeFramer base id 0x4200

  instance fatalAdapter: Svc.AssertFatalAdapter base id 0x4300

  instance fatalHandler: Svc.FatalHandler base id 0x4400

  instance comBufferManager: Svc.BufferManager base id 0x4500 {

    phase Fpp.ToCpp.Phases.configConstants """
    enum {
      UART_BUFFER_SIZE         = 1024,
      COM_BUFFER_SIZE          = 2048,
      FILE_UPLINK_BUFFER_SIZE  = 3000,
      UART_BUFFER_COUNT        = 5,
      COM_BUFFER_COUNT         = 10,
      FILE_UPLINK_BUFFER_COUNT = 30,
      BUFFER_MANAGER_ID = 200
    };
    """

    phase Fpp.ToCpp.Phases.configComponents """
    Svc::BufferManager::BufferBins upBuffMgrBins;
    memset(&upBuffMgrBins, 0, sizeof(upBuffMgrBins));
    {
      using namespace ConfigConstants::SystemReference_comBufferManager;
      upBuffMgrBins.bins[0].bufferSize = UART_BUFFER_SIZE;
      upBuffMgrBins.bins[0].numBuffers = UART_BUFFER_COUNT;
      upBuffMgrBins.bins[1].bufferSize = COM_BUFFER_SIZE;
      upBuffMgrBins.bins[1].numBuffers = COM_BUFFER_COUNT;
      upBuffMgrBins.bins[2].bufferSize = FILE_UPLINK_BUFFER_SIZE;
      upBuffMgrBins.bins[2].numBuffers = FILE_UPLINK_BUFFER_COUNT;
      upBuffMgrBins.bins[3].bufferSize = 1024*1024*32;
      upBuffMgrBins.bins[3].numBuffers = 3;

      comBufferManager.setup(
          BUFFER_MANAGER_ID,
          0,
          Allocation::mallocator,
          upBuffMgrBins
      );
    }
    """

    phase Fpp.ToCpp.Phases.tearDownComponents """
    comBufferManager.cleanup();
    """

  }

  instance posixTime: Svc.PosixTime base id 0x4600

  instance rateGroupDriverComp: Svc.RateGroupDriver base id 0x4700 {
    phase Fpp.ToCpp.Phases.configObjects """
    Svc::RateGroupDriver::DividerSet rgDivs{{{1, 0}, {2, 0}, {4, 0}}};
    """
    
    phase Fpp.ToCpp.Phases.configComponents """
    rateGroupDriverComp.configure(
        ConfigObjects::SystemReference_rateGroupDriverComp::rgDivs
    );
    """

  }

  instance textLogger: Svc.PassiveTextLogger base id 0x4900

  instance deframer: Svc.FprimeDeframer base id 0x4A00

  instance systemResources: Svc.SystemResources base id 0x4B00
  instance imu: Gnc.Imu base id 0x4C00 {
    phase Fpp.ToCpp.Phases.configComponents """
    // The AdaFruit board uses AD0 = 0
    imu.setup(Gnc::Imu::I2cDevAddr::AD0_0);
    """
  }

  instance imuI2cBus: Drv.LinuxI2cDriver  base id 0x4D00 {
    phase Fpp.ToCpp.Phases.configComponents """
    if (!imuI2cBus.open("/dev/i2c-0")) {
         Fw::Logger::log("[ERROR] Failed to open I2C device\\n");
    }
    """
  }
  
  instance frameAccumulator: Svc.FrameAccumulator base id 0x4E00 \
  {
     phase Fpp.ToCpp.Phases.configObjects """
         Svc::FrameDetectors::FprimeFrameDetector fprimeFrameDetector;
     """
 
     phase Fpp.ToCpp.Phases.configComponents """
     {
       SystemReference::frameAccumulator.configure(ConfigObjects::SystemReference_frameAccumulator::fprimeFrameDetector, 1, Allocation::mallocator, 2048);
     }
     """
 
     phase Fpp.ToCpp.Phases.tearDownComponents """
     {
       SystemReference::frameAccumulator.cleanup();
     }
     """
 
   }

  instance fprimeRouter: Svc.FprimeRouter base id 0x4F00

  instance linuxTimer: Svc.LinuxTimer base id 0x5000
}
