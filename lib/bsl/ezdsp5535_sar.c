//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_sar.c
// *                                                                          
// * Description:  SAR implementation.
// *                                                                          
// * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/ 
// * Copyright (C) 2011 Spectrum Digital, Incorporated
// *                                                                          
// *                                                                          
// *  Redistribution and use in source and binary forms, with or without      
// *  modification, are permitted provided that the following conditions      
// *  are met:                                                                
// *                                                                          
// *    Redistributions of source code must retain the above copyright        
// *    notice, this list of conditions and the following disclaimer.         
// *                                                                          
// *    Redistributions in binary form must reproduce the above copyright     
// *    notice, this list of conditions and the following disclaimer in the   
// *    documentation and/or other materials provided with the                
// *    distribution.                                                         
// *                                                                          
// *    Neither the name of Texas Instruments Incorporated nor the names of   
// *    its contributors may be used to endorse or promote products derived   
// *    from this software without specific prior written permission.         
// *                                                                          
// *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS     
// *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT       
// *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR   
// *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT    
// *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,   
// *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT        
// *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   
// *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY   
// *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT     
// *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE   
// *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    
// *                                                                          
//////////////////////////////////////////////////////////////////////////////

#include "csl_sar.h"
#include "ezdsp5535.h"
#include "ezdsp5535_sar.h"

CSL_SarHandleObj SarObj;

CSL_SarHandleObj *SarHandle;

Uint16 preKey  =0;
Uint16 keyCnt1 =0;
Uint16 keyCnt2 =0;

/*
 *   Init_SAR(void)
 *       Initialize SAR ADC
 * 
 *   Returns
 *     0  :  Success
 *     1  :  Fail
 */
Uint8 EZDSP5535_SAR_init(void)
{
	CSL_Status    status;
    CSL_SarChSetup param;
    int chanNo;

    /* Set Bus for GPIOs */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE6);

    /* Initialize the SAR module */
    status = SAR_init();
    if(status != CSL_SOK)
        return 1;  // Fail
        
    /* Open SAR channel */
    status = SAR_chanOpen(&SarObj,CSL_SAR_CHAN_3);
    SarHandle = &SarObj;
    if(status != CSL_SOK)
        return 1;  // Fail

    /* Initialize channel */
    status = SAR_chanInit(SarHandle);
    if(status != CSL_SOK)
        return 1;  // Fail

    param.OpMode = CSL_SAR_POLLING;
    param.MultiCh = CSL_SAR_NO_DISCHARGE;
    param.RefVoltage = CSL_SAR_REF_VIN;
    param.SysClkDiv = 0x0b ;
    
    /* Configuration for SAR module */
    status = SAR_chanSetup(SarHandle,&param);
    if(status != CSL_SOK)
        return 1;  // Fail

    /* Set channel cycle set */
    status = SAR_chanCycSet(SarHandle,CSL_SAR_CONTINUOUS_CONVERSION);
    if(status != CSL_SOK)
        return 1;  // Fail

    /* Set ADC Measurement parameters */
    status = SAR_A2DMeasParamSet(SarHandle,CSL_KEYPAD_MEAS,&chanNo);
    if(status != CSL_SOK)
        return 1;  // Fail

    /* Start the conversion */
    status = SAR_startConversion(SarHandle);
    if(status != CSL_SOK)
        return 1;  // Fail

    return 0;
}

/*
 *   Get_Sar_Key(void)
 *     Find key pressed depending on value returned by SAR ADC
 * 
 *   Returns
 *     val  :  Value of key returned
 */
Uint16 EZDSP5535_SAR_getKey(void)
{
	CSL_Status    status;
    Uint16 val;

    /* Check whether the ADC data is available or not */
    while(CSL_SAR_DATA_AVAILABLE != SAR_getStatus(SarHandle,&status));

    status = SAR_readData(SarHandle, &val);
    if(status != CSL_SOK)
        return NoKey;

    /* Account for percentage of error */
    if((val < SW1 + 16) && (val > SW1 - 16))
        val = SW1;
    else if((val < SW2 + 16) && (val > SW2 - 16))
        val = SW2;
    else if((val < SW12 + 16) && (val > SW12 - 16))
        val = SW12;
    else if((val < NoKey + 16) && (val > NoKey - 16))
        val = NoKey;
    else
        val = NoKey;
        
    return val;
}
