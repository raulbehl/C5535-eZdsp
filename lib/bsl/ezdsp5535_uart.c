//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_uart.c
// *                                                                          
// * Description:  UART interface.
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
#include "ezdsp5535_uart.h"
#include "csl_uart.h"
#include "csl_general.h"

CSL_UartObj uartObj;
CSL_UartHandle    hUart;

/*
 *  UART_open( )
 *    Open UART handle and configure it for 115200 baud
 * 
 */
Int16 EZDSP5535_UART_open( )
{
    CSL_Status        status;
    CSL_UartConfig 		    Config;

    /*  
     * Configuring for baud rate of 115200
     * Divisor = UART input clock frequency / (Desired baud rate*16)
     * = 100MHz / 115200 / 16
     * = 54 = 0x36
     */
    Config.DLL = 0x36;  // Set baud rate
    Config.DLH = 0x00;
        
    Config.FCR = 0x0000;            // Clear UART TX & RX FIFOs
    Config.LCR = 0x0003;            // 8-bit words,
                                    // 1 STOP bit generated,
                                    // No Parity, No Stick paritiy,
                                    // No Break control
    Config.MCR = 0x0000;            // RTS & CTS disabled,
                                    // Loopback mode disabled,
                                    // Autoflow disabled

    status = UART_init(&uartObj, CSL_UART_INST_0, UART_INTERRUPT);
    hUart = (CSL_UartHandle)&uartObj;
    status |= UART_reset(hUart);
    status |= UART_config(hUart,&Config);
    status |= UART_resetOff(hUart);        

    CSL_SYSCTRL_REGS->EBSR &= ~0xF000;   //
    CSL_SYSCTRL_REGS->EBSR |=  0x1000;   // Set parallel port to mode 1 (SPI, GPIO, UART, and I2S2)
    
	return status;
}

/* 
 *  UART_putChar( uart_handle, data )
 *    Send 1 byte of serial data
 * 
 */
Int16 EVM5515_UART_putChar( char data )
{
	UART_fputc(hUart, data, 0);

    return 0;
}

/*
 *  UART_getChar( uart_handle, data )
 *    Recv 1 byte of serial data
 *
 */
Int16 EVM5515_UART_getChar( char* data )
{
	UART_fgetc(hUart, data, 0);

    return 0;
}

