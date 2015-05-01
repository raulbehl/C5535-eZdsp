//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_lcd.c
// *                                                                          
// * Description:  LCD implementation.
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
#include"ezdsp5535_i2c.h"
#include"ezdsp5535_gpio.h"

#define OSD9616_I2C_ADDR 0x3C    // OSD9616 I2C address

/*
 *  Int16 EZDSP5535_OSD9616_send( Uint16 comdat, Uint16 data )
 * 
 *      Sends 2 bytes of data to the OSD9616
 *
 *      comdat :  0x00  => Command
 *                0x40  => Data
 *      data   :  Data to be sent
 * 
 *  Returns
 *      0 => Success
 *      1 => Fail
 */
Int16 EZDSP5535_OSD9616_send( Uint16 comdat, Uint16 data )
{
    Uint16 cmd[2];
    cmd[0] = comdat & 0x00FF;     // Specifies whether data is Command or Data
    cmd[1] = data;                // Command / Data
    EZDSP5535_waitusec( 1000 );
    return EZDSP5535_I2C_write( OSD9616_I2C_ADDR, cmd, 2 );
}

/* 
 *  Int16 EZDSP5535_OSD9616_multiSend( Uint16* data, Uint16 len )
 *
 *      Sends multiple bytes of data to the OSD9616
 * 
 *      *data :  Pointer to start of I2C transfer
 *       len  :  Length of I2C transaction
 * 
 *  Returns
 *      0 => Success
 *      1 => Fail
 */
Int16 EZDSP5535_OSD9616_multiSend( Uint16* data, Uint16 len )
{
    Uint16 x;
    Uint16 cmd[10];
    for(x=0;x<len;x++)               // Command / Data
    {
    	cmd[x] = data[x];
    }
    EZDSP5535_waitusec( 1000 );
    return EZDSP5535_I2C_write( OSD9616_I2C_ADDR, cmd, len );
}

/* 
 *  Int16 EZDSP5535_OSD9616_init( )
 *
 *      Initialize the OSD9616
 */
Int16 EZDSP5535_OSD9616_init( )
{
	Uint16 cmd[10];                    // For multibyte commands

	/* Initialize GPIOs */
	EZDSP5535_GPIO_init();
	
	/* Initialize I2C */
    EZDSP5535_I2C_init( );
    
    /* Initialize LCD power */
    EZDSP5535_GPIO_setDirection( 12, 1 );  // Output
    EZDSP5535_GPIO_setOutput( 12, 1 );     // Enable 13V 
    
    /* Initialize OSD9616 display */
    EZDSP5535_OSD9616_send(0x00,0x00); // Set low column address
    EZDSP5535_OSD9616_send(0x00,0x10); // Set high column address
    EZDSP5535_OSD9616_send(0x00,0x40); // Set start line address

    cmd[0] = 0x00 & 0x00FF;  // Set contrast control register
    cmd[1] = 0x81;
    cmd[2] = 0x7f;
    EZDSP5535_OSD9616_multiSend( cmd, 3 );

    EZDSP5535_OSD9616_send(0x00,0xa1); // Set segment re-map 95 to 0
    EZDSP5535_OSD9616_send(0x00,0xa6); // Set normal display

    cmd[0] = 0x00 & 0x00FF;            // Set multiplex ratio(1 to 16)
    cmd[1] = 0xa8; 
    cmd[2] = 0x0f;
    EZDSP5535_OSD9616_multiSend( cmd, 3 );

    EZDSP5535_OSD9616_send(0x00,0xd3); // Set display offset
    EZDSP5535_OSD9616_send(0x00,0x00); // Not offset
    EZDSP5535_OSD9616_send(0x00,0xd5); // Set display clock divide ratio/oscillator frequency
    EZDSP5535_OSD9616_send(0x00,0xf0); // Set divide ratio

    cmd[0] = 0x00 & 0x00FF;  // Set pre-charge period
    cmd[1] = 0xd9;
    cmd[2] = 0x22;
    EZDSP5535_OSD9616_multiSend( cmd, 3 );

    cmd[0] = 0x00 & 0x00FF;  // Set com pins hardware configuration
    cmd[1] = 0xda;
    cmd[2] = 0x02;
    EZDSP5535_OSD9616_multiSend( cmd, 3 );

    EZDSP5535_OSD9616_send(0x00,0xdb); // Set vcomh
    EZDSP5535_OSD9616_send(0x00,0x49); // 0.83*vref
    
    cmd[0] = 0x00 & 0x00FF;  //--set DC-DC enable
    cmd[1] = 0x8d;
    cmd[2] = 0x14;
    EZDSP5535_OSD9616_multiSend( cmd, 3 );

    EZDSP5535_OSD9616_send(0x00,0xaf); // Turn on oled panel    
	
	return 0;
}


/* 

 *  Int16 EZDSP5535_OSD9616_printLetter(Uint16 l1,Uint16 l2,Uint16 l3,Uint16 l4)
 *
 *      Send 4 bytes representing a Character in a dot matrix
 * 
 *      l1  : Line 1 
 *      l2  : Line 2
 *      l3  : Line 3
 *      l4  : Line 4 
 */
Int16 EZDSP5535_OSD9616_printLetter(Uint16 l1,Uint16 l2,Uint16 l3,Uint16 l4)
{
	EZDSP5535_OSD9616_send(0x40,l1);
    EZDSP5535_OSD9616_send(0x40,l2);
    EZDSP5535_OSD9616_send(0x40,l3);
    EZDSP5535_OSD9616_send(0x40,l4);
    EZDSP5535_OSD9616_send(0x40,0x00);  // Line blank for space
    
    return 0;
}
