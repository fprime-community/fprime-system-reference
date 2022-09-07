module Gnc {

    array Vector = [3] F32

    struct ImuData {
       $time: Fw.Time
       vector: Vector
       status: Svc.MeasurementStatus
    } default { status = Svc.MeasurementStatus.STALE }

    @ Port for receiving current X, Y, Z position
    port ImuDataPort() -> ImuData
}