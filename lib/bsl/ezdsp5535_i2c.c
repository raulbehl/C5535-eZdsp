//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_i2c.c
// *                                                                          
// * Description:  I2C implementation.
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
 
#include "ezdsp5535_i2c.h"
#include "csl_i2c.h"

/*
 *  EZDSP5535_I2C_init( )
 *
 *      Enable and initalize the I2C module.
 *      The I2C clk is set to run at 100 KHz.
 */
Int16 EZDSP5535_I2C_init( )
{
    CSL_Status         status;
    CSL_I2cConfig         i2cConfig;
    
    status = I2C_init(CSL_I2C0);
    
    /* Configure I2C module for write */
    i2cConfig.icoar  = CSL_I2C_ICOAR_DEFVAL;
    i2cConfig.icimr  = CSL_I2C_ICIMR_DEFVAL;
    i2cConfig.icclkl = 20;
    i2cConfig.icclkh = 20;
    i2cConfig.icsar  = CSL_I2C_ICSAR_DEFVAL;
    i2cConfig.icmdr  = CSL_I2C_ICMDR_WRITE_DEFVAL;
    i2cConfig.icemdr = CSL_I2C_ICEMDR_DEFVAL;
    i2cConfig.icpsc  = 20;

    status |= I2C_config(&i2cConfig);

    return 0;
}

/*
 *  EZDSP5535_I2C_reset( )
 *
 *      Resets I2C module
 */
Int16 EZDSP5535_I2C_reset( )
{
    EZDSP5535_I2C_init( );
    return 0;
}

/*
 *  EZDSP5535_I2C_write( i2c_addr, data, len )
 *
 *      I2C write in Master mode: Writes to I2c device with address 
 *      "i2c_addr" from the location of "data" for length "len".
 * 
 *      Uint16 i2c_addr    <- I2C slave address
 *      Uint16* data       <- I2C data ptr
 *      Uint16 len         <- # of bytes to write
 */
Int16 EZDSP5535_I2C_write( Uint16 i2c_addr, Uint16* data, Uint16 len )
{
    Uint16 startStop            = ((CSL_I2C_START) | (CSL_I2C_STOP));
    CSL_Status         status;
    
    /* I2C Write */
    status = I2C_write(data, len, i2c_addr,
                           TRUE, startStop, CSL_I2C_MAX_TIMEOUT);

    return status;
}

/*
 *
 *  EZDSP5535_I2C_read( i2c_addr, data, len )
 *
 *      I2C read in Master mode: Reads from I2c device with address 
 *      "i2c_addr" and stores to the location of "data" for length "len".
 * 
 *      Uint16 i2c_addr    <- I2C slave address
 *      Uint16* data       <- I2C data ptr
 *      Uint16 len         <- # of bytes to write
 */
Int16 EZDSP5535_I2C_read( Uint16 i2c_addr, Uint16* data, Uint16 len )
{
    Uint16 startStop            = ((CSL_I2C_START) | (CSL_I2C_STOP));
    CSL_Status         status;
    
    /* I2C Read */
    status = I2C_read(data, len, i2c_addr,
                           TRUE, startStop, CSL_I2C_MAX_TIMEOUT, FALSE);
    return status;
}
