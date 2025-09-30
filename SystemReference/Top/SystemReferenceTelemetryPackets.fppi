telemetry packets SystemReferencePackets {

    packet CDH id 1 group 1 {
        SystemReference.cmdDisp.CommandsDispatched
        SystemReference.cmdDisp.CommandsDropped
        SystemReference.rateGroup1Comp.RgMaxTime
        SystemReference.rateGroup2Comp.RgMaxTime
        SystemReference.rateGroup3Comp.RgMaxTime
        SystemReference.cmdSeq.CS_LoadCommands
        SystemReference.cmdSeq.CS_CancelCommands
        SystemReference.cmdSeq.CS_CommandsExecuted
        SystemReference.cmdSeq.CS_SequencesCompleted
        SystemReference.fileUplink.FilesReceived
        SystemReference.fileUplink.PacketsReceived
        SystemReference.comBufferManager.TotalBuffs
        SystemReference.comBufferManager.CurrBuffs
        SystemReference.comBufferManager.HiBuffs
        SystemReference.comQueue.comQueueDepth
        SystemReference.comQueue.buffQueueDepth
        SystemReference.fileDownlink.FilesSent
        SystemReference.fileDownlink.PacketsSent
        SystemReference.fileManager.CommandsExecuted
        SystemReference.tlmSend.SendLevel
    }

    packet CDHErrors id 2 group 1 {
        SystemReference.rateGroup1Comp.RgCycleSlips
        SystemReference.rateGroup2Comp.RgCycleSlips
        SystemReference.rateGroup3Comp.RgCycleSlips
        SystemReference.cmdSeq.CS_Errors
        SystemReference.fileUplink.Warnings
        SystemReference.fileDownlink.Warnings
        SystemReference.$health.PingLateWarnings
        SystemReference.fileManager.Errors
        SystemReference.comBufferManager.NoBuffs
        SystemReference.comBufferManager.EmptyBuffs
        SystemReference.fileManager.Errors
    }

    packet SystemResources1 id 5 group 2 {
        SystemReference.systemResources.MEMORY_TOTAL
        SystemReference.systemResources.MEMORY_USED
        SystemReference.systemResources.NON_VOLATILE_TOTAL
        SystemReference.systemResources.NON_VOLATILE_FREE
    }

    packet SystemResources2 id 6 group 2 {
        SystemReference.systemResources.CPU
        SystemReference.systemResources.CPU_00
        SystemReference.systemResources.CPU_01
        SystemReference.systemResources.CPU_02
        SystemReference.systemResources.CPU_03
        SystemReference.systemResources.CPU_04
        SystemReference.systemResources.CPU_05
        SystemReference.systemResources.CPU_06
        SystemReference.systemResources.CPU_07
        SystemReference.systemResources.CPU_08
        SystemReference.systemResources.CPU_09
        SystemReference.systemResources.CPU_10
        SystemReference.systemResources.CPU_11
        SystemReference.systemResources.CPU_12
        SystemReference.systemResources.CPU_13
        SystemReference.systemResources.CPU_14
        SystemReference.systemResources.CPU_15
    }

    packet Imu id 7 group 2 {
        SystemReference.imu.accelerometer
        SystemReference.imu.gyroscope
    }

    packet Camera id 8 group 2 {
        SystemReference.camera.photosTaken
        SystemReference.saveImageBufferLogger.BufferLogger_NumLoggedBuffers
    }

} omit {
  SystemReference.cmdDisp.CommandErrors
}

