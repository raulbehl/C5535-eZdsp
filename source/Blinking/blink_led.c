#include "ezdsp5535.h"
#include "ezdsp5535_led.h"
#include "stdio.h"

void blink_led () {
    Int16 i;
    EZDSP5535_LED_init( );
    EZDSP5535_XF_toggle();
    
    for (i = 0; i<5; i++) {
    	
    	EZDSP5535_LED_on(0);
    	EZDSP5535_waitusec( 200000 );
    	
    	EZDSP5535_LED_on(1);
    	EZDSP5535_waitusec( 200000 );
    	
    	EZDSP5535_LED_on(2);
    	EZDSP5535_waitusec( 200000 );
    	
    	EZDSP5535_LED_on(3);
    	EZDSP5535_waitusec( 200000 );
    	
    	EZDSP5535_LED_off(0);
    	EZDSP5535_waitusec( 200000 );
    	
    	EZDSP5535_LED_off(1);
    	EZDSP5535_waitusec( 200000 );
    	
    	EZDSP5535_LED_off(2);
    	EZDSP5535_waitusec( 200000 );
    	
    	EZDSP5535_LED_off(3);
    	EZDSP5535_waitusec( 200000 );
    	
    }
    EZDSP5535_LED_setall( 0x00 );

}
