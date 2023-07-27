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

  instance blockDrv: Drv.BlockDriver base id 0x0100 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 140

  instance rateGroup1Comp: Svc.ActiveRateGroup base id 0x0200 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 120 \
  {

    phase Fpp.ToCpp.Phases.configObjects """
    NATIVE_INT_TYPE context[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    """

    phase Fpp.ToCpp.Phases.configComponents """
    rateGroup1Comp.configure(
        ConfigObjects::rateGroup1Comp::context,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::rateGroup1Comp::context)
    );
    """

  }

  instance rateGroup2Comp: Svc.ActiveRateGroup base id 0x0300 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 119 \
  {

    phase Fpp.ToCpp.Phases.configObjects """
    NATIVE_INT_TYPE context[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    """

    phase Fpp.ToCpp.Phases.configComponents """
    rateGroup2Comp.configure(
        ConfigObjects::rateGroup2Comp::context,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::rateGroup2Comp::context)
    );
    """

  }

  instance rateGroup3Comp: Svc.ActiveRateGroup base id 0x0400 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 118 \
  {

    phase Fpp.ToCpp.Phases.configObjects """
    NATIVE_INT_TYPE context[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    """

    phase Fpp.ToCpp.Phases.configComponents """
    rateGroup3Comp.configure(
        ConfigObjects::rateGroup3Comp::context,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::rateGroup3Comp::context)
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
    ConfigObjects::comQueue::configurationTable.entries[0] = {.depth = 500, .priority = 2};
    // Events , highest-priority
    ConfigObjects::comQueue::configurationTable.entries[1] = {.depth = 100, .priority = 0};
    // File Downlink
    ConfigObjects::comQueue::configurationTable.entries[2] = {.depth = 100, .priority = 1};
    // Allocation identifier is 0 as the MallocAllocator discards it
    comQueue.configure(ConfigObjects::comQueue::configurationTable, 0, Allocation::mallocator);
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
        ConfigConstants::cmdSeq::BUFFER_SIZE
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
        ConfigConstants::fileDownlink::TIMEOUT,
        ConfigConstants::fileDownlink::COOLDOWN,
        ConfigConstants::fileDownlink::CYCLE_TIME,
        ConfigConstants::fileDownlink::FILE_QUEUE_DEPTH
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

  instance eventLogger: Svc.ActiveLogger base id 0x0B00 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 98

  instance chanTlm: Svc.TlmChan base id 0x0C00 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 97

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
           Fw::Logger::logMsg("[ERROR] Failed to open camera device\\n");
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
        const char* const name = "/home/pi/images/save/saveImage";
        const char* const type = ".data";
        saveImageBufferLogger.initLog(
            name,
            type,
            ConfigConstants::saveImageBufferLogger::MAX_FILE_SIZE,
            ConfigConstants::saveImageBufferLogger::SIZE_OF_SIZE
        );
        """

    }


   instance imageProcessor: Payload.ImageProcessor base id 0x1000 \
    queue size 30 \
    stack size Default.stackSize \
    priority 100 

   instance processedImageBufferLogger: Svc.BufferLogger base id 0x1100 \
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
        const char* const filename = "/home/pi/images/process/processedImage";
        const char* const filetype = ".data";
        processedImageBufferLogger.initLog(
            filename,
            filetype,
            ConfigConstants::processedImageBufferLogger::MAX_FILE_SIZE,
            ConfigConstants::processedImageBufferLogger::SIZE_OF_SIZE
        );
        """

    }

    
    instance actuator: Gnc.Actuator base id 0x1300 \
      queue size Default.queueSize\
      stack size Default.stackSize \
      priority 100
    

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
        ConfigObjects::health::pingEntries,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::health::pingEntries),
        ConfigConstants::health::WATCHDOG_CODE
    );
    """

  }

  # ----------------------------------------------------------------------
  # Passive component instances
  # ----------------------------------------------------------------------

  # NOTE: use only one of XBee or comStub
  instance radio: Svc.ComStub base id 0x1200

#  @ Communications driver. May be swapped with other comm drivers like UART
#  @ Note: Here we have TCP reliable uplink and UDP (low latency) downlink
  instance comDriver: Drv.ByteStreamDriverModel base id 0x4000 \
    type "Drv::TcpClient" \
    at "../../Drv/TcpClient/TcpClient.hpp" \
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
        comDriver.startSocketTask(
            name,
            true,
            ConfigConstants::comDriver::PRIORITY,
            ConfigConstants::comDriver::STACK_SIZE
        );
    }
    """

   phase Fpp.ToCpp.Phases.freeThreads """
   comDriver.stopSocketTask();
   (void) comDriver.joinSocketTask(nullptr);
   """

 }

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
#            comDriver.startReadThread(
#                ConfigConstants::comDriver::PRIORITY,
#                ConfigConstants::comDriver::STACK_SIZE
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
#    comDriver.join(nullptr);
#    """
# }



  instance framer: Svc.Framer base id 0x4200 {
    phase Fpp.ToCpp.Phases.configObjects """
    Svc::FprimeFraming framing;
    """

    phase Fpp.ToCpp.Phases.configComponents """
    framer.setup(ConfigObjects::framer::framing);
    """

  }

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
      using namespace ConfigConstants::comBufferManager;
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

  instance linuxTime: Svc.Time base id 0x4600 \
    type "Svc::LinuxTime" \
    at "../../Svc/LinuxTime/LinuxTime.hpp"

  instance rateGroupDriverComp: Svc.RateGroupDriver base id 0x4700 {
    phase Fpp.ToCpp.Phases.configObjects """
    NATIVE_INT_TYPE rgDivs[Svc::RateGroupDriver::DIVIDER_SIZE] = { 1, 2, 4 };
    """
    
    phase Fpp.ToCpp.Phases.configComponents """
    rateGroupDriverComp.configure(
        ConfigObjects::rateGroupDriverComp::rgDivs,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::rateGroupDriverComp::rgDivs)
    );
    """

  }

  instance textLogger: Svc.PassiveTextLogger base id 0x4900

  instance deframer: Svc.Deframer base id 0x4A00 {
    phase Fpp.ToCpp.Phases.configObjects """
    Svc::FprimeDeframing deframing;
    """

    phase Fpp.ToCpp.Phases.configComponents """
    deframer.setup(ConfigObjects::deframer::deframing);
    """

  }

  instance systemResources: Svc.SystemResources base id 0x4B00
  instance imu: Gnc.Imu base id 0x4C00 {
    phase Fpp.ToCpp.Phases.configComponents """
    // The AdaFruit board uses AD0 = 0
    imu.setup(Gnc::Imu::I2cDevAddr::AD0_0);
    """
  }

  instance imuI2cBus: Drv.LinuxI2cDriver  base id 0x4D00 {
    phase Fpp.ToCpp.Phases.configComponents """
    if (!imuI2cBus.open("/dev/i2c-1")) {
         Fw::Logger::logMsg("[ERROR] Failed to open I2C device\\n");
    }
    """
  }
 
  instance pwmDriver: Drv.LinuxPwmDriver base id 0x4F00 {
    phase Fpp.ToCpp.Phases.configComponents """
        pwmDriver.open();
    """
  }
  
}
