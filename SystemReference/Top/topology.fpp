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
    instance fprimeRouter
    instance textLogger
    instance tlmSend
    instance deframer
    instance systemResources
    instance imu
    instance imuI2cBus
    instance camera
    instance saveImageBufferLogger
    instance linuxTimer

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
      # Inputs to ComQueue (events, telemetry, file)
      eventLogger.PktSend         -> comQueue.comPacketQueueIn[0]
      tlmSend.PktSend             -> comQueue.comPacketQueueIn[1]
      fileDownlink.bufferSendOut  -> comQueue.bufferQueueIn[0]
      comQueue.bufferReturnOut[0] -> fileDownlink.bufferReturn
      # ComQueue <-> Framer
      comQueue.dataOut   -> framer.dataIn
      framer.dataReturnOut -> comQueue.dataReturnIn
      framer.comStatusOut  -> comQueue.comStatusIn
      # Buffer Management for Framer
      framer.bufferAllocate   -> comBufferManager.bufferGetCallee
      framer.bufferDeallocate -> comBufferManager.bufferSendIn
      # Framer <-> ComStub
      framer.dataOut        -> radio.dataIn
      radio.dataReturnOut -> framer.dataReturnIn
      radio.comStatusOut  -> framer.comStatusIn
      # ComStub <-> ComDriver
      radio.drvSendOut      -> comDriver.$send
      comDriver.ready         -> radio.drvConnected
    }

    connections FaultProtection {
      eventLogger.FatalAnnounce -> fatalHandler.FatalReceive
    }

    connections RateGroups {

      # Linux Timer to drive rate groups
      linuxTimer.CycleOut -> rateGroupDriverComp.CycleIn

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
      rateGroup3Comp.RateGroupMemberOut[1] -> comBufferManager.schedIn
    }

    connections Sequencer {
      cmdSeq.comCmdOut -> cmdDisp.seqCmdBuff
      cmdDisp.seqCmdStatus -> cmdSeq.cmdResponseIn
    }

    connections Uplink {
      # ComDriver buffer allocations
      comDriver.allocate      -> comBufferManager.bufferGetCallee
      comDriver.deallocate    -> comBufferManager.bufferSendIn
      # ComDriver <-> ComStub
      comDriver.$recv             -> radio.drvReceiveIn
      radio.drvReceiveReturnOut -> comDriver.recvReturnIn
      # ComStub <-> FrameAccumulator
      radio.dataOut                -> frameAccumulator.dataIn
      frameAccumulator.dataReturnOut -> radio.dataReturnIn
      # FrameAccumulator buffer allocations
      frameAccumulator.bufferDeallocate -> comBufferManager.bufferSendIn
      frameAccumulator.bufferAllocate   -> comBufferManager.bufferGetCallee
      # FrameAccumulator <-> Deframer
      frameAccumulator.dataOut  -> deframer.dataIn
      deframer.dataReturnOut    -> frameAccumulator.dataReturnIn
      # Deframer <-> Router
      deframer.dataOut           -> fprimeRouter.dataIn
      fprimeRouter.dataReturnOut -> deframer.dataReturnIn
      # Router buffer allocations
      fprimeRouter.bufferAllocate   -> comBufferManager.bufferGetCallee
      fprimeRouter.bufferDeallocate -> comBufferManager.bufferSendIn
      # Router <-> CmdDispatcher/FileUplink
      fprimeRouter.commandOut  -> cmdDisp.seqCmdBuff
      cmdDisp.seqCmdStatus     -> fprimeRouter.cmdResponseIn
      fprimeRouter.fileOut     -> fileUplink.bufferSendIn
      fileUplink.bufferSendOut -> fprimeRouter.fileBufferReturnIn
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

