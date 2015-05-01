//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_led.c
// *                                                                          
// * Description:  LED implementation.
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
#include "ezdsp5535_led.h"
#include "ezdsp5535_gpio.h"
#include "soc.h"
#include "cslr_cpu.h"

/*
 *  USBSTK5515_LED_init( )
 *
 *      Initialize LEDs
 */
Int16 EZDSP5535_LED_init( )
{
	/* Initialize GPIOs */
	EZDSP5535_GPIO_init();
	
	/* Turn XF off */
	EZDSP5535_XF_off();
	
    /* Enable LED Outputs */
    EZDSP5535_GPIO_setDirection( 14, GPIO_OUT );  // DS1
    EZDSP5535_GPIO_setDirection( 15, GPIO_OUT );  // DS2
    EZDSP5535_GPIO_setDirection( 16, GPIO_OUT );  // DS3
    EZDSP5535_GPIO_setDirection( 17, GPIO_OUT );  // DS4

    /* Turn OFF all LEDs */
    return EZDSP5535_LED_setall( 0x0F );
}

/*
 *  EZDSP5535_LED_getall( pattern )
 *
 *      Get all User LED values
 */
Int16 EZDSP5535_LED_getall( Uint16 *pattern )
{
    Uint16 retval;

	retval = EZDSP5535_GPIO_getInput(14)  |
	    (EZDSP5535_GPIO_getInput(15) << 1) |
		(EZDSP5535_GPIO_getInput(16) << 2) |
		(EZDSP5535_GPIO_getInput(17) << 3);

    *pattern = retval;

    return 0;
}

/*
 *  EZDSP5535_LED_setall( pattern )
 *
 *      Set all User LED values
 */
Int16 EZDSP5535_LED_setall( Uint16 pattern )
{
    if (pattern & 0x01)
        EZDSP5535_GPIO_setOutput( 14, 1);
    else
        EZDSP5535_GPIO_setOutput( 14, 0);

    if (pattern & 0x02)
        EZDSP5535_GPIO_setOutput( 15, 1);
    else
        EZDSP5535_GPIO_setOutput( 15, 0);

    if (pattern & 0x04)
        EZDSP5535_GPIO_setOutput( 16, 1);
    else
        EZDSP5535_GPIO_setOutput( 16, 0);

    if (pattern & 0x08)
        EZDSP5535_GPIO_setOutput( 17, 1);
    else
        EZDSP5535_GPIO_setOutput( 17, 0);

    return 0;
}

/*
 *  EZDSP5535_LED_on( number )
 *
 *      number <- LED# [0:3]
 */
Int16 EZDSP5535_LED_on( Uint16 number )
{
    Uint16 led_state;
    Uint16 led_bit_on;

    led_bit_on = 1 << ( 3 - number );

    /*
     *  Get then set LED
     */
    if ( EZDSP5535_LED_getall( &led_state ) )
        return -1;

    led_state = led_state & ( ~led_bit_on );

    if ( EZDSP5535_LED_setall( led_state ) )
        return -1;

    return 0;
}

/*
 *  EZDSP5535_LED_off( number )
 *
 *      number <- LED# [0:3]
 */
Int16 EZDSP5535_LED_off( Uint16 number )
{
    Uint16 led_state;
    Uint16 led_bit_off;

    led_bit_off = 1 << ( 3 - number );

    /*
     *  Get then set LED
     */
    if ( EZDSP5535_LED_getall( &led_state ) )
        return -1;

    led_state = led_state | led_bit_off;

    if ( EZDSP5535_LED_setall( led_state ) )
        return -1;

    return 0;
}

/*
 *  EZDSP5535_LED_toggle( number )
 *
 *      number <- LED# [0:3]
 */
Int16 EZDSP5535_LED_toggle( Uint16 number )
{
    Uint16 led_state;
    Uint16 new_led_state;
    Uint16 led_bit_toggle;


    led_bit_toggle = 1 << ( 3 - number );

    /*
     *  Get then set LED
     */
    if ( EZDSP5535_LED_getall( &led_state ) )
        return -1;

    if ( ( led_state & led_bit_toggle ) == 0 )
        new_led_state = led_state | led_bit_toggle;     /* Turn OFF */
    else
        new_led_state = led_state & ~led_bit_toggle;    /* Turn ON */

    if ( EZDSP5535_LED_setall( new_led_state ) )
        return -1;

    return 0;
}

/*
 *
 *  EZDSP5535_XF_on( )
 *
 *  Description
 *      Turns on the XF LED.
 *
 */
Int16 EZDSP5535_XF_on()
{
    CSL_CPU_REGS->ST1_55 |= CSL_CPU_ST1_55_XF_MASK;
    return 0;
}

/*
 *
 *  EZDSP5535_XF_off( )
 *
 *  Description
 *      Turns off the XF LED.
 *
 */
Int16 EZDSP5535_XF_off()
{
    CSL_CPU_REGS->ST1_55 &= ~CSL_CPU_ST1_55_XF_MASK;
    return 0;
}

/*
 *
 *  EZDSP5535_XF_get( )
 *
 *  Description
 *      Returns the state of the XF LED.
 *
 */
Int16 EZDSP5535_XF_get()
{      
    return ((CSL_CPU_REGS->ST1_55 & CSL_CPU_ST1_55_XF_MASK) >> CSL_CPU_ST1_55_XF_SHIFT);
}

/*
 *
 *  EZDSP5535_XF_toggle( )
 *
 *  Description
 *      Toggles the XF LED.
 *
 */
Int16 EZDSP5535_XF_toggle()
{      
    if((CSL_CPU_REGS->ST1_55 & CSL_CPU_ST1_55_XF_MASK) >> CSL_CPU_ST1_55_XF_SHIFT)
        EZDSP5535_XF_off();
    else
        EZDSP5535_XF_on();
    return 0;
}
