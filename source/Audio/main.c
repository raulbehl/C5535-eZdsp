#include "stdio.h"
#include "ezdsp5535.h"

extern Int16 aic3204_test ();

void main() {
    /* Initialize BSL */
    EZDSP5535_init( );
    aic3204_test( );
}
