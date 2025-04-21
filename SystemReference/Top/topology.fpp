module SystemReference {

  # ----------------------------------------------------------------------
  # Symbolic constants for port numbers
  # ----------------------------------------------------------------------

    enum Ports_RateGroups {
      rateGroup1
      rateGroup2
      rateGroup3
    }

    enum Ports_StaticMemory {
      downlink
      uplink
    }

  topology SystemReference {

    # ----------------------------------------------------------------------
    # Instances used in the topology
    # ----------------------------------------------------------------------

    instance $health
    instance blockDrv

    instance cmdDisp
    instance cmdSeq
    instance comDriver
    instance comQueue
    instance radio
    instance framer
    instance eventLogger
    instance fatalAdapter
    instance fatalHandler
    instance fileDownlink
    instance fileManager
    instance fileUplink
    instance frameAccumulator
    instance comBufferManager
    instance posixTime
    instance prmDb
    instance rateGroup1Comp
    instance rateGroup2Comp
    instance rateGroup3Comp
    instance rateGroupDriverComp
    instance router
    instance textLogger
    instance tlmSend
    instance deframer
    instance systemResources
    instance imu
    instance imuI2cBus
    instance camera
    instance saveImageBufferLogger

    # ----------------------------------------------------------------------
    # Pattern graph specifiers
    # ----------------------------------------------------------------------

    command connections instance cmdDisp

    event connections instance eventLogger

    param connections instance prmDb

    telemetry connections instance tlmSend

    text event connections instance textLogger

    time connections instance posixTime

    health connections instance $health

    # ---------------------------------------------------------------------- 
    # Telemetry packets
    # ---------------------------------------------------------------------- 

    include "SystemReferenceTelemetryPackets.fppi"


    # ----------------------------------------------------------------------
    # Direct graph specifiers
    # ----------------------------------------------------------------------

    connections Downlink {
      eventLogger.PktSend -> comQueue.comPktQueueIn[0]
      tlmSend.PktSend -> comQueue.comPktQueueIn[1]
      fileDownlink.bufferSendOut -> comQueue.buffQueueIn[0]

      comQueue.queueSend -> framer.dataIn
      framer.dataReturn -> comQueue.bufferReturnIn
      comQueue.bufferReturnOut[0] -> fileDownlink.bufferReturn

      framer.bufferAllocate -> bufferManager.bufferGetCallee
      framer.framedDataOut -> comStub.comDataIn

      comDriver.deallocate -> comBufferManager.bufferSendIn

      radio.drvDataOut -> comDriver.$send
      comDriver.ready -> radio.drvConnected
      radio.comStatus -> comQueue.comStatusIn
    }

    connections FaultProtection {
      eventLogger.FatalAnnounce -> fatalHandler.FatalReceive
    }

    connections RateGroups {

      # Block driver
      blockDrv.CycleOut -> rateGroupDriverComp.CycleIn

      # Rate group 1
      rateGroupDriverComp.CycleOut[Ports_RateGroups.rateGroup1] -> rateGroup1Comp.CycleIn
      rateGroup1Comp.RateGroupMemberOut[0] -> tlmSend.Run
      rateGroup1Comp.RateGroupMemberOut[1] -> fileDownlink.Run
      rateGroup1Comp.RateGroupMemberOut[2] -> systemResources.run
      rateGroup1Comp.RateGroupMemberOut[3] -> imu.Run

#     XBee Radio Integration
#     rateGroup1Comp.RateGroupMemberOut[4] -> radio.run

      # Rate group 2
      rateGroupDriverComp.CycleOut[Ports_RateGroups.rateGroup2] -> rateGroup2Comp.CycleIn
      rateGroup2Comp.RateGroupMemberOut[0] -> cmdSeq.schedIn

      # Rate group 3
      rateGroupDriverComp.CycleOut[Ports_RateGroups.rateGroup3] -> rateGroup3Comp.CycleIn
      rateGroup3Comp.RateGroupMemberOut[0] -> $health.Run
      rateGroup3Comp.RateGroupMemberOut[1] -> blockDrv.Sched
      rateGroup3Comp.RateGroupMemberOut[2] -> comBufferManager.schedIn
    }

    connections Sequencer {
      cmdSeq.comCmdOut -> cmdDisp.seqCmdBuff
      cmdDisp.seqCmdStatus -> cmdSeq.cmdResponseIn
    }

    connections Uplink {
      comDriver.allocate -> comBufferManager.bufferGetCallee
      comDriver.$recv -> radio.drvDataIn

      radio.comDataOut -> frameAccumulator.dataIn
      frameAccumulator.frameOut -> deframer.framedIn
      deframer.deframedOut -> router.dataIn
      router.bufferDeallocate -> comBufferManager.bufferSendIn

      router.commandOut -> cmdDisp.seqCmdBuff
      cmdDisp.seqCmdStatus -> router.cmdResponseIn

      frameAccumulator.bufferAllocate -> comBufferManager.bufferGetCallee
      router.fileOut -> fileUplink.bufferSendIn
      frameAccumulator.bufferDeallocate -> comBufferManager.bufferSendIn
      fileUplink.bufferSendOut -> comBufferManager.bufferSendIn
    }

#    XBee Radio Integration
#    connections Radio {
#      radio.allocate -> comBufferManager.bufferGetCallee
#      radio.deallocate -> comBufferManager.bufferSendIn
#    }

    connections I2c {
        imu.read -> imuI2cBus.read
        imu.write -> imuI2cBus.write
    }

    connections Camera {
         camera.allocate -> comBufferManager.bufferGetCallee
         camera.deallocate -> comBufferManager.bufferSendIn
         camera.$save -> saveImageBufferLogger.bufferSendIn
         saveImageBufferLogger.bufferSendOut -> comBufferManager.bufferSendIn
    }

  }

}

