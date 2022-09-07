module Com {
    constant retryCount = 10
    array EnergyDensityType = [16] U8
    active component XBee {
        # ----------------------------------------------------------------------
        # Framer, deframer, and queue ports
        # ----------------------------------------------------------------------

        @ Data coming in from the framing component
        sync input port comDataIn: Drv.ByteStreamSend

        @ Status of the last radio transmission
        output port comStatus: Svc.ComStatus

        @ Com data passing back out
        output port comDataOut: Drv.ByteStreamRecv

        # ----------------------------------------------------------------------
        # Byte stream model
        # ----------------------------------------------------------------------

        @ Ready signal when driver is connected
        sync input port drvConnected: Drv.ByteStreamReady

        @ Data received from driver
        sync input port drvDataIn: Drv.ByteStreamRecv

        @ Data going to the underlying driver
        output port drvDataOut: Drv.ByteStreamSend

        # ----------------------------------------------------------------------
        # Implementation ports
        # ----------------------------------------------------------------------

        @ Allows for deallocation of XBee command communications
        output port deallocate: Fw.BufferSend

        @ Allows for allocation of buffers
        output port allocate: Fw.BufferGet

        # ----------------------------------------------------------------------
        # Special ports
        # ----------------------------------------------------------------------

        @ Port carrying 1HZ tick for timeout tracking
        sync input port run: Svc.Sched

        @ Time get port
        time get port timeCaller

        @ Command registration port
        command reg port cmdRegOut

        @ Command received port
        command recv port cmdIn

        @ Command response port
        command resp port cmdResponseOut

        @ Text event port
        text event port logTextOut

        @ Event port
        event port logOut

        @ Telemetry port
        telemetry port tlmOut

        # ----------------------------------------------------------------------
        # Commands
        # ----------------------------------------------------------------------

        @ Report the radio serial number
        async command ReportNodeIdentifier

        @ Perform energy density scan activity
        async command EnergyDensityScan

        # ----------------------------------------------------------------------
        # Events
        # ----------------------------------------------------------------------

        @ Produces a node-identifier
        event RadioNodeIdentifier(
              identifier: string size 20 @< Radio identifier
        ) \
        severity activity high \
        id 0 \
        format "Radio identification: {}"

        # ----------------------------------------------------------------------
        # Telemetry
        # ----------------------------------------------------------------------
        @ Energy density graph
        telemetry EnergyDensity: EnergyDensityType id 0 update always
    }
}
