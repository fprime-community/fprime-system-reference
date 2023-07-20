module Drv {
    
    port EnableDisable( operationType: Fw.Enabled ) -> Fw.Success 
 
    port OnTime( 
                    operationValue: U32 @< On time in nanoseconds 
                ) -> Fw.Success 
    
    port Period( 
                     operationValue: U32 @< Period in nanoseconds 
                ) -> Fw.Success 
}