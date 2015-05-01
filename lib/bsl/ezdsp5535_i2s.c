//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_i2s.c
// *                                                                          
// * Description:  I2S implementation.
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

#include "ezdsp5535.h"
#include "ezdsp5535_i2s.h"
#include "csl_i2s.h"

CSL_I2sHandle   hI2s;

/*
 *  EZDSP5535_I2S_init( )
 *
 *      Initialize I2S module.
 */
Int16 EZDSP5535_I2S_init( )
{
    I2S_Config      hwConfig;
    Int16           result = 0;

    /* Open the device with instance 0 */
    hI2s = I2S_open(I2S_INSTANCE2, I2S_POLLED, I2S_CHAN_STEREO);

    /* Set the value for the configure structure */
    hwConfig.dataType           = I2S_STEREO_ENABLE;
    hwConfig.loopBackMode       = I2S_LOOPBACK_DISABLE;
    hwConfig.fsPol              = I2S_FSPOL_LOW;
    hwConfig.clkPol             = I2S_RISING_EDGE;
    hwConfig.datadelay          = I2S_DATADELAY_ONEBIT;
    hwConfig.datapack           = I2S_DATAPACK_ENABLE;
    hwConfig.signext            = I2S_SIGNEXT_DISABLE;
    hwConfig.wordLen            = I2S_WORDLEN_32;
    hwConfig.i2sMode            = I2S_SLAVE;
    hwConfig.FError             = I2S_FSERROR_ENABLE;
    hwConfig.OuError            = I2S_OUERROR_ENABLE;

    /* Configure hardware registers */
    result += I2S_setup(hI2s, &hwConfig);
    result += I2S_transEnable(hI2s, TRUE);
    
    return result;
}

/*
 *  EZDSP5535_I2S_readLeft( data )
 *
 *      Reads incoming I2S left channel word and writes it
 *      to the location of "data".
 * 
 *      Int16* data   <-  Pointer to location if I2S data destination
 */
void EZDSP5535_I2S_readLeft(Int16* data)
{
    ioport  CSL_I2sRegs   *regs;
    
    regs = hI2s->hwRegs;
    while((0x08 & regs->I2SINTFL) == 0);  // Wait for receive interrupt to be pending
    *data = regs->I2SRXLT1 ;              // 16 bit left channel receive audio data
}

/*
 *  EZDSP5535_I2S_writeLeft( data )
 *
 *      Writes I2S left channel word.
 * 
 *      Int16 data   <-  I2S left data
 */
void EZDSP5535_I2S_writeLeft(Int16 data)
{
    ioport  CSL_I2sRegs   *regs;
    
    regs = hI2s->hwRegs;
    while((CSL_I2S_I2SINTFL_XMITSTFL_MASK & regs->I2SINTFL) == 0);  // Wait for transmit interrupt to be pending
    regs->I2STXLT1 = (data) ;            // 16 bit left channel transmit audio data
}

/*
 *
 *  EZDSP5535_I2S_readRight( data )
 *
 *      Reads incoming I2S right channel word and writes it
 *      to the location of "data".
 * 
 *      Int16* data   <-  Pointer to location if I2S data destination
 */
void EZDSP5535_I2S_readRight(Int16* data)
{
    ioport  CSL_I2sRegs   *regs;
    
    regs = hI2s->hwRegs;
//  while((0x08 & regs->I2SINTFL) == 0);  // Wait for receive interrupt to be pending
    *data = regs->I2SRXRT1 ;              // 16 bit left channel receive audio data
}

/*
 *  EZDSP5535_I2S_writeRight(*data)
 *
 *      Writes I2S left channel word.
 * 
 *      Int16 data   <-  I2S right data
 */
void EZDSP5535_I2S_writeRight(Int16 data)
{
    ioport  CSL_I2sRegs   *regs;
    
    regs = hI2s->hwRegs;
//  while((CSL_I2S_I2SINTFL_XMITSTFL_MASK & regs->I2SINTFL) == 0);  // Wait for transmit interrupt to be pending
    regs->I2STXRT1 = (data) ;              // 16 bit left channel transmit audio data
}


/*
 *  EZDSP5535_I2S_close()
 *
 *      Closes I2S module.
 */
Int16 EZDSP5535_I2S_close()
{
    Int16 result;
    result = I2S_reset(hI2s);
    
    return result;
}
