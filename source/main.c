#include "stdio.h"
#include "ezdsp5535.h"

extern void blink_led ();

void main() {   
    EZDSP5535_init( );
	blink_led( );
}
