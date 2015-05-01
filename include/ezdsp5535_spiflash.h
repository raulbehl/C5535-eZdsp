//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_spiflash.h
// *                                                                          
// * Description:  SPI FLASH header file.
// *                                                                          
// * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
// * Copyright (C) 2010 Spectrum Digital, Incorporated
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

/* ------------------------------------------------------------------------ *
 *  SPI FLASH Definitions                                                   *
 * ------------------------------------------------------------------------ */
#define spiflash_SIZE               0x400000
#define spiflash_BASE               0x00000000
#define spiflash_PAGESIZE           256
#define spiflash_PAGEMASK           0xffffff00
#define spiflash_SECTORSIZE         4096
#define spiflash_SECTORMASK         0xfffff000

/* ------------------------------------------------------------------------ *
 *  SPI FLASH Commands                                                      *
 * ------------------------------------------------------------------------ */
#define spiflash_CMD_WREN           0x06
#define spiflash_CMD_WRDI           0x04
#define spiflash_CMD_RDSR           0x05
#define spiflash_CMD_WRSR           0x01
#define spiflash_CMD_READ           0x03
#define spiflash_CMD_WRITE          0x02
#define spiflash_CMD_ERASEBLK       0xD8
#define spiflash_CMD_ERASESEC       0x20
#define spiflash_CMD_ERASECHIP      0xC7
#define spiflash_CMD_MFGID          0x90

/* ------------------------------------------------------------------------ *
 *  Prototype                                                               *
 * ------------------------------------------------------------------------ */
Int16  EZDSP5535_SPIFLASH_init( );
Uint16 EZDSP5535_SPIFLASH_status( );
void   EZDSP5535_SPIFLASH_erase( Uint32 base, Uint32 length );
Int16  EZDSP5535_SPIFLASH_read(  Uint32 src, Uint32 dst, Uint32 len );
Int16  EZDSP5535_SPIFLASH_write( Uint32 src, Uint32 dst, Uint32 len );

