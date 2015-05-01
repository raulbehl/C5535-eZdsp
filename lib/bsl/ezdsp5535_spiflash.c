//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_spiflash.c
// *                                                                          
// * Description:  SPI FLASH interface.
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
#include "ezdsp5535_spiflash.h"
#include "csl_spi.h"

static Uint16 spiflashbuf[spiflash_PAGESIZE + 8];
static Uint16 statusbuf[8];
CSL_SpiHandle   hSpi;

/*
 *  EZDSP5535_SPIFLASH_init( )
 *
 *      Enables and configures SPI for the SPIFLASH
 *      ( CS0, EBSR Mode 1, 8-bit, 100KHz clock )
 */
Int16 EZDSP5535_SPIFLASH_init( )
{
    SPI_Config      hwConfig;
    Int16           result = 0;

    hSpi->configured = FALSE;   // Set as unconfigured
    hSpi = NULL;                // Remove previous settings
    result += SPI_init();       // Enable SPI
    hSpi = SPI_open(SPI_CS_NUM_0, SPI_POLLING_MODE); // Enable CS0
    CSL_SYSCTRL_REGS->EBSR = (CSL_SYSCTRL_REGS->EBSR 
                                 & 0x0fff) | 0x1000; // EBSR Mode 1 (7 SPI pins)

    /* Configuration for SPIFLASH */
    hwConfig.wLen       = SPI_WORD_LENGTH_8;    // 8-bit
    hwConfig.spiClkDiv  = 0x00b1;               // 100KHz clock (12MHz / 120)
    hwConfig.csNum      = SPI_CS_NUM_0;         // Select CS0
    hwConfig.frLen      = 1;
    hwConfig.dataDelay  = SPI_DATA_DLY_0;
    hwConfig.clkPol     = SPI_CLKP_LOW_AT_IDLE;
    
    /* Configure SPIFLASH */
    result += SPI_config(hSpi, &hwConfig);
    
    return result;  
}

/*
 *  EZDSP5535_SPIFLASH_status( )  
 *
 *      Returns the SPIFLASH Status Register
 * 
 *      Returns: Uint16 statusbuf[0]  <- Value of SPIFLASH Status Register
 *

 *
 */
Uint16 EZDSP5535_SPIFLASH_status( )  
{
    Int16 result;
    
    /* Issue read status command */
    statusbuf[0] = spiflash_CMD_RDSR;
    
    /* Send read status command */
    CSL_SPI_REGS->SPICMD1 = 0x0000 | 1;
    result = SPI_dataTransaction(hSpi ,statusbuf, 1, SPI_WRITE );
    result = SPI_dataTransaction(hSpi ,statusbuf, 1, SPI_READ);

    return statusbuf[0];
}


/* ------------------------------------------------------------------------ *
 *  spiflash_erase( base, len )                                             *
 * ------------------------------------------------------------------------ */
void EZDSP5535_SPIFLASH_erase( Uint32 base, Uint32 length )
{
    Int32 bytes_left, bytes_to_erase, eraseaddr;
    Int16 result = 0;
    
    eraseaddr = base;
    bytes_left = length;

    while (bytes_left > 0 )
    {
        /* Limit erase unit to sector size */
	    bytes_to_erase = bytes_left;
		if (bytes_to_erase > spiflash_SECTORSIZE)
		    bytes_to_erase = spiflash_SECTORSIZE;

        /* Align to sector boundaries */
        if ((eraseaddr & spiflash_SECTORMASK) != ((eraseaddr + bytes_to_erase) & spiflash_SECTORMASK))
                bytes_to_erase -= (eraseaddr + bytes_to_erase) - ((eraseaddr + bytes_to_erase) & spiflash_SECTORMASK);

        /* Issue WPEN */
        CSL_SPI_REGS->SPICMD1 = 0x0000 | 0;
        spiflashbuf[0] = spiflash_CMD_WREN;
        result += SPI_dataTransaction(hSpi ,spiflashbuf, 1, SPI_WRITE);

        CSL_SPI_REGS->SPICMD1 = 0x0000 | 4 - 1;
        
        /* Issue erase */
        spiflashbuf[0] = spiflash_CMD_ERASESEC;
        spiflashbuf[1] = ( Uint8 )( eraseaddr >> 16 );
        spiflashbuf[2] = ( Uint8 )( eraseaddr >> 8 );
        spiflashbuf[3] = ( Uint8 )( eraseaddr );
        result += SPI_dataTransaction(hSpi ,spiflashbuf, 4, SPI_WRITE);

        /* Wait while busy */
        while( ( EZDSP5535_SPIFLASH_status( ) & 0x01 ) );

        /* Get ready for next iteration */
        bytes_left -= bytes_to_erase;
        eraseaddr += bytes_to_erase;
    }
}

/*
 *  EZDSP5535_SPIFLASH_read( src, dst, len )
 * 
 *      Read chunk of data from "src" that is "len" long
 *      and copy it to "dst".
 * 
 *      Uint16 src   <-  Source from SPIFLASH
 *      Uint16 dst   <-  Destination in memory
 *      Uint32 len   <-  Length of block of data
 */
Int16 EZDSP5535_SPIFLASH_read( Uint32 src, Uint32 dst, Uint32 len )
{
    Int16 i, result = 0;
    Uint16 *psrc, *pdst;

    /* Setup command */
    spiflashbuf[0] = spiflash_CMD_READ;
    spiflashbuf[1] = ( src >> 16);
    spiflashbuf[2] = ( src >> 8 );
    spiflashbuf[3] = ( src >> 0 );

    /* Execute SPIFLASH read cycle */
    CSL_SPI_REGS->SPICMD1 = 0x0000 | len + 4 - 1;
    result += SPI_dataTransaction(hSpi ,spiflashbuf, 4, SPI_WRITE);
    result += SPI_dataTransaction(hSpi ,spiflashbuf, len, SPI_READ);

    /* Copy returned data */
    pdst = ( Uint16 * )dst;
    psrc = spiflashbuf;
    for ( i = 0 ; i < len ; i++ )
        *pdst++ = *psrc++;
        
    return result;
}

/*
 *  EZDSP5535_SPIFLAH_write( src, dst, len )
 * 
 *      Write chunk of data from "src" that is "len" long
 *      to the location "dst". 
 * 
 *      Uint32 src   <-  Source from memory
 *      Uint16 dst   <-  Destination in SPIFLASH
 *      Uint32 len   <-  Length of block of data
 */
Int16 EZDSP5535_SPIFLASH_write( Uint32 src, Uint32 dst, Uint32 len )
{
    Int16 i, result = 0;
    Int32 bytes_left;
    Int32 bytes_to_program;
    Uint16 *psrc;

    /* Establish source */
    psrc = ( Uint16 * )src;
    bytes_left = len;

    /* SPIFLASH write loop */
    while ( bytes_left > 0 )
    {
        bytes_to_program = bytes_left;
        
        /* Most to program is spiflash_PAGESIZE */
        if ( bytes_to_program > spiflash_PAGESIZE )  
             bytes_to_program = spiflash_PAGESIZE;
             
        /* Make sure you don't run off the end of a block */
        if ( ( dst & spiflash_PAGEMASK ) != ( ( dst + bytes_to_program ) & spiflash_PAGEMASK ) )
            bytes_to_program -= ( dst + bytes_to_program ) - ( ( dst + bytes_to_program ) & spiflash_PAGEMASK );

        /* Issue WPEN */
        CSL_SPI_REGS->SPICMD1 = 0x0000 | 0;
        spiflashbuf[0] = spiflash_CMD_WREN;
        result += SPI_dataTransaction(hSpi ,spiflashbuf, 1, SPI_WRITE);
        while( ( EZDSP5535_SPIFLASH_status( ) & 0x01 ) );
        
        /* Create command block for program operation */
        spiflashbuf[0] = spiflash_CMD_WRITE;
        spiflashbuf[1] = ( dst >> 16 );
        spiflashbuf[2] = ( dst >> 8 );
        spiflashbuf[3] = ( dst );

        /* Execute write command */
        CSL_SPI_REGS->SPICMD1 = 0x0000 | len + 4 - 1;
        result += SPI_dataTransaction(hSpi ,spiflashbuf, 4, SPI_WRITE);
        
        /* Load source to spiflashbuf */
        for ( i = 0 ; i < bytes_to_program ; i++ )
        spiflashbuf[i] = *psrc++;
        
        /* Send data to write */
        result += SPI_dataTransaction(hSpi ,spiflashbuf, len, SPI_WRITE);

        /* Wait while busy */
        while( ( EZDSP5535_SPIFLASH_status( ) & 0x01 ) );

        /* Get ready for next iteration */
        bytes_left -= bytes_to_program;
        dst += bytes_to_program;
    }
    
    return result;
}
