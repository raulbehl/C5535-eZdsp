//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_sd_card.c
// *                                                                          
// * Description:  SD Card implementation.
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

#include "ezdsp5535.h"
#include "ezdsp5535_sdcard.h"
#include "csl_mmcsd.h"
#include "stdio.h"

/* MMCSD Data structures */
CSL_MMCControllerObj 	pMmcsdContObj;
CSL_MmcsdHandle 		mmcsdHandle;
CSL_MMCCardObj			mmcCardObj;
CSL_MMCCardIdObj 		sdCardIdObj;
CSL_MMCCardCsdObj 		sdCardCsdObj;

Uint32       cardAddr;
CSL_Status	 mmcStatus;

/*
 *  EZDSP5535_SDCARD_init( )
 *
 *      Initalize SD card interface
 */
Int16 EZDSP5535_SDCARD_init()
{
	Uint16		 actCard;
	Uint32       sectCount;
	Uint16       rca;

	sectCount = 0;
	
	/* Initialize the CSL MMCSD module */
	mmcStatus = MMC_init();
	if(mmcStatus != CSL_SOK)
	{
		printf("    MMC_init Failed\n");
		return(mmcStatus);
	}

	/* Open the MMCSD module in POLLED mode */
//	mmcsdHandle = MMC_open(&pMmcsdContObj, CSL_MMCSD1_INST, CSL_MMCSD_OPMODE_POLLED, &mmcStatus);
	mmcsdHandle = MMC_open(&pMmcsdContObj, CSL_MMCSD0_INST, CSL_MMCSD_OPMODE_POLLED, &mmcStatus);

	if(mmcStatus != CSL_SOK)
	{
		printf("    MMC_open Failed\n");
		return(mmcStatus);
	}
	else
	{
		printf("    MMC_open Successful\n");
	}

	/* Send CMD0 to the card */
	mmcStatus = MMC_sendGoIdle(mmcsdHandle);
	if(mmcStatus != CSL_SOK)
	{
		printf("    MMC_sendGoIdle Failed\n");
		return(mmcStatus);
	}

	/* Check for the card */
    mmcStatus = MMC_selectCard(mmcsdHandle, &mmcCardObj);
	if((mmcStatus == CSL_ESYS_BADHANDLE) ||
	   (mmcStatus == CSL_ESYS_INVPARAMS))
	{
		printf("    MMC_selectCard Failed\n");
		return(mmcStatus);
	}

	/* Verify whether the SD card is detected or not */
	if(mmcCardObj.cardType == CSL_SD_CARD)
	{
		printf("    SD card Detected!\n");

		/* Check if the card is high capacity card */
		if(mmcsdHandle->cardObj->sdHcDetected == TRUE)
		{
			printf("    SD card is High Capacity Card\n");
			printf("    Memory Access will use Block Addressing\n\n");

			/* For the SDHC card Block addressing will be used.
			   Sector address will be same as sector number */
			cardAddr = sectCount;
		}
		else
		{
			printf("    SD card is Standard Capacity Card\n");
			printf("    Memory Access will use Byte Addressing\n\n");

			/* For the SD card Byte addressing will be used.
			   Sector address will be product of  sector number
			   and sector size */
			cardAddr = (sectCount)*(CSL_MMCSD_BLOCK_LENGTH);
		}
	}
	else
	{
		/* Check if No card is inserted */
		if(mmcCardObj.cardType == CSL_CARD_NONE)
		{
			printf("No Card Detected!\n");
		}
		else
		{
			printf("SD card is not Detected!\n");
		}

		printf("Please Insert SD card!!\n");
		return(CSL_ESYS_FAIL);
	}

	/* Set the init clock */
    mmcStatus = MMC_sendOpCond(mmcsdHandle, 70);
	if(mmcStatus != CSL_SOK)
	{
		printf("    MMC_sendOpCond Failed\n");
		return(mmcStatus);
	}

	/* Send the card identification Data */
	mmcStatus = SD_sendAllCID(mmcsdHandle, &sdCardIdObj);
	if(mmcStatus != CSL_SOK)
	{
		printf("    SD_sendAllCID Failed\n");
		return(mmcStatus);
	}

	/* Set the Relative Card Address */
	mmcStatus = SD_sendRca(mmcsdHandle, &mmcCardObj, &rca);
	if(mmcStatus != CSL_SOK)
	{
		printf("    SD_sendRca Failed\n");
		return(mmcStatus);
	}

	/* Read the SD Card Specific Data */
	mmcStatus = SD_getCardCsd(mmcsdHandle, &sdCardCsdObj);
	if(mmcStatus != CSL_SOK)
	{
		printf("    SD_getCardCsd Failed\n");
		return(mmcStatus);
	}

	/* Set bus width - Optional */
	mmcStatus = SD_setBusWidth(mmcsdHandle, 1);
	if(mmcStatus != CSL_SOK)
	{
		printf("    SD_setBusWidth Failed\n");
		return(mmcStatus);
	}

	/* Disable SD card pull-up resistors - Optional */
	mmcStatus = SD_configurePullup(mmcsdHandle, 0);
	if(mmcStatus != CSL_SOK)
	{
		printf("    SD_configurePullup Failed\n");
		return(mmcStatus);
	}

	/* Set the card type in internal data structures */
	mmcStatus = MMC_setCardType(&mmcCardObj, mmcCardObj.cardType);
	if(mmcStatus != CSL_SOK)
	{
		printf("    MMC_setCardType Failed\n");
		return(mmcStatus);
	}

	/* Set the card pointer in internal data structures */
	mmcStatus = MMC_setCardPtr(mmcsdHandle, &mmcCardObj);
	if(mmcStatus != CSL_SOK)
	{
		printf("    MMC_setCardPtr Failed\n");
		return(mmcStatus);
	}

	/* Get the number of cards */
	mmcStatus = MMC_getNumberOfCards(mmcsdHandle, &actCard);
	if(mmcStatus != CSL_SOK)
	{
		printf("    MMC_getNumberOfCards Failed\n");
		return(mmcStatus);
	}

	/* Set clock for read-write access */
    mmcStatus = MMC_sendOpCond(mmcsdHandle, 2);
	if(mmcStatus != CSL_SOK)
	{
		printf("    MMC_sendOpCond Failed\n");
		return(mmcStatus);
	}

	/* Set Endian mode for read and write operations */
  	mmcStatus = MMC_setEndianMode(mmcsdHandle, CSL_MMCSD_ENDIAN_LITTLE,
  	                              CSL_MMCSD_ENDIAN_LITTLE);
	if(mmcStatus != CSL_SOK)
	{
		printf("    MMC_setEndianMode Failed\n");
		return(mmcStatus);
	}

	/* Set block length for the memory card
	 * For high capacity cards setting the block length will have
	 * no effect
	 */
	mmcStatus = MMC_setBlockLength(mmcsdHandle, CSL_MMCSD_BLOCK_LENGTH);
	if(mmcStatus != CSL_SOK)
	{
		printf("    MMC_setBlockLength Failed\n");
		return(mmcStatus);
	}
	
	return 0;
}

/*
 *  EZDSP5535_SDCARD_read(Uint32 addr, Uint32 len, Uint16* pReadBuff)
 *
 *      Read from SD card
 * 
 *      addr      =>  Address to read from
 *      len       =>  Length in bytes
 *      pReadBuff =>  Pointer to destination of data read
 */
Int16 EZDSP5535_SDCARD_read(Uint32 addr, Uint32 len, Uint16* pReadBuff)
{
	cardAddr = addr;
	
	/* Read data from the SD card */
	mmcStatus = MMC_read(mmcsdHandle, cardAddr, len, pReadBuff);
	
    return mmcStatus;
}

/*
 *  EZDSP5535_SDCARD_write(Uint32 addr, Uint32 len, Uint16* pWriteBuff)
 *
 *      Write to SD card
 * 
 *      addr       =>  Address to write to
 *      len        =>  Length in bytes
 *      pWriteBuff =>  Pointer to source of data to be written
 */
Int16 EZDSP5535_SDCARD_write(Uint32 addr, Uint32 len, Uint16* pWriteBuff)
{
	cardAddr = addr;
	
    /* Write data to the SD card */
  	mmcStatus = MMC_write(mmcsdHandle, cardAddr, len, pWriteBuff);
	
    return mmcStatus;
}

/*
 *  EZDSP5535_SDCARD_close()
 *
 *      Close SD card interface
 */
Int16 EZDSP5535_SDCARD_close()
{
	/* Deselect the SD card */
	mmcStatus = MMC_deselectCard(mmcsdHandle, &mmcCardObj);
	if(mmcStatus != CSL_SOK)
	{
		printf("    MMC_deselectCard Failed\n");
		return(mmcStatus);
	}

	/* Clear the MMCSD card response registers */
	mmcStatus = MMC_clearResponse(mmcsdHandle);
	if(mmcStatus != CSL_SOK)
	{
		printf("    MMC_clearResponse Failed\n");
		return(mmcStatus);
	}

	/* Send CMD0 to the SD card */
	mmcStatus = MMC_sendCmd(mmcsdHandle, 0x00, 0x00, 0xFFFF);
	if(mmcStatus != CSL_SOK)
	{
		printf("    MMC_sendCmd Failed\n");
		return(mmcStatus);
	}

	/* Close the MMCSD module */
	mmcStatus = MMC_close(mmcsdHandle);
	if(mmcStatus != CSL_SOK)
	{
		printf("    MMC_close Failed\n");
		return(mmcStatus);
	}
	else
	{
		printf("    MMC_close Successful\n");
	}
	
	return 0;
}
