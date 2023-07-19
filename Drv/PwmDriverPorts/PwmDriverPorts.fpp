module Drv {
    enum OneByteOps {   ENABLE,
                        DISABLE,
                        EXPORT,
                        UNEXPORT  }

    enum MultiByteOps{  PERIOD, 
                        ON_TIME,  }

    port EnableAndExport( operationType: OneByteOps )
 
    port PeriodAndOnTime( operationType: MultiByteOps,
                         operationValue: U32 @< On time in nanoseconds 
                        ) #-> Fw.Success 
}