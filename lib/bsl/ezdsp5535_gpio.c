//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_gpio.c
// *                                                                          
// * Description:  GPIO implementation.
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

#include "ezdsp5535_gpio.h"
#include "csl_gpio.h"

CSL_GpioObj     GpioObj;
CSL_GpioObj     *hGpio;

/*
 *
 *  EZDSP5535_GPIO_init( )
 *
 *      Initialize GPIO Mudule.
 *
 */
Int16 EZDSP5535_GPIO_init()
{
    CSL_Status           status;
    hGpio = GPIO_open(&GpioObj, &status);
    
    /* Set Bus for GPIOs */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE6);
    
    return 0;
}

/*
 *
 *  EZDSP5535_GPIO_setDirection( number, direction )
 * 
 *      Configure the GPIO "number" to input or output.
 *
 *      Uint16 number    <- GPIO number
 *      Uint16 direction <- 0 : Set GPIO as Input
 *                          1 : Set GPIO as Output
 */
Int16 EZDSP5535_GPIO_setDirection( Uint16 number, Uint16 direction )
{
    CSL_Status           status;
    CSL_GpioPinConfig    config;
    
    /* Configure GPIO direction */
    config.pinNum    = number;
    config.direction = direction;
    config.trigger   = CSL_GPIO_TRIG_CLEAR_EDGE;
    
    /* Set configuration */
    status = GPIO_configBit(hGpio,&config);

    return status;
}

/*
 *
 *  EZDSP5535_GPIO_setOutput( number, output )
 * 
 *      Sets the GPIO "number" to the high or low state. The GPIO must 
 *      be an output.
 *
 *      Uint16 number   <- GPIO number
 *      Uint16 output    <- 0 : GPIO output is logic low 
 *                         1 : GPIO output is logic high
 */
Int16 EZDSP5535_GPIO_setOutput( Uint16 number, Uint16 output )
{
    CSL_Status           status;
    
    /* Set GPIO output */
    status = GPIO_write(hGpio, number, output);

    return status;
}

/*
 *
 *  EZDSP5535_GPIO_getInput( number )
 *
 *      Returns 0 if the GPIO "number" is in the low state and 1 if it is 
 *      in the high state.
 * 
 *      Uint16 number   <- GPIO number
 *
 *      Returns: Uint16 readVal ->  0 : Input to the GPIO is logic low
 *                                  1 : Input to the GPIO is logic high
 */
Int16 EZDSP5535_GPIO_getInput( Uint16 number )
{
    CSL_Status           status;
    Uint16          readVal = 0;
    
    /* Get GPIO input */
    status = GPIO_read(hGpio, number, &readVal);

    return readVal;
}
