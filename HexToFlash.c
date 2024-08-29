/*
 * HexToFlash.c
 *
 *  Created on: Aug 21, 2024
 *      Author: emirhan
 */

#include "HexToFlash.h"


extern UART_HandleTypeDef huart3;


void HexToFlashInit()
{
    FlashAdress = 0x08008000;
    satir=0;

    checksum_kontrol=0;
    checksum_kontrol2=0;

    flashyazindex=0;

    CC = '0';
    doru=0;
    yanlis=0;
    flashdatauzunlugu=2;
		
		sector =2;
    kalanindex = 0;
		sonpage=1;
	  update =0;

    oku =0;
    yaz = 0;
}

void flashpageyaz(int index,int sector)
{
    if (HAL_FLASH_Unlock() != HAL_OK) {
        return;
    }

    FLASH_Erase_Sector(sector,FLASH_VOLTAGE_RANGE_3);


    int startIdx = 0, endIdx = 0;
    switch (index) {
        case 1:
            startIdx = 0;
            endIdx = 8192;
            break;
        case 2:
            startIdx = 8192;
            endIdx = 16384;
            break;
        case 3:
            startIdx = (sector % 2 == 0) ? 0 : 8192;
            endIdx = startIdx + ((satir+1) % 1024) * 8 - 8;
            break;
        default:
            return;
    }

    for (int i = startIdx; i < endIdx; i++) {
        //uint16_t swappedData = __REV16(flashayazData[i]); // 16-bit veriyi swap et
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, FlashAdress,flashayazData[i]);
        FlashAdress += flashdatauzunlugu;
			;
    }
    HAL_FLASH_Lock();
}

void flashdanoku()
{
	oku = 1;
    char buffer[20];
    uint16_t okunanData;
    FlashAdress = 0x08008000;

    for (int j = 0; j < satir - 1; j++) {
        for (int i = 0; i < 8; i++) {
            okunanData = *(uint16_t *)FlashAdress;
            FlashAdress += 2;  // 16-bit okuduğumuz için adresi 2 artır

            // Veriyi 4 haneli hexadecimal formatında yazdır
            snprintf(buffer, sizeof(buffer), "%02X%02X", (okunanData >> 8) & 0xFF, okunanData & 0xFF);

            HAL_UART_Transmit(&huart3, (uint8_t *)buffer, strlen(buffer), 1000);
            HAL_UART_Transmit(&huart3, (uint8_t *)"  ", 2, 1000);  // Boşluk ekle
        }
        HAL_UART_Transmit(&huart3, (uint8_t *)"\r\n", 2, 1000);
    }
    oku = 0;
}

void HexToFlash()
{
	  if(satir -1 == doru && update == 0)
	  {
		  update = 1;
		  HAL_UART_Transmit(&huart3, (uint8_t *)"\n\r================================\n\r", 36, 1000);
		  HAL_UART_Transmit(&huart3, (uint8_t *)"Hex Dosyasi Basariyla Gonderildi \n\r", 38, 1000);
		  HAL_UART_Transmit(&huart3, (uint8_t *)"================================\n\r", 34, 1000);
	  }

	if(RxBuffer[0] == ':')
			{
		yaz=1;
			}
/*
	if(strncmp((char *)RxBuffer, "<EOF>", 5))
	{
		yaz =0;
	}*/
    if (flashyazindex == 16384) {
        flashyazindex = 0;
        flashpageyaz(2,sector);
        HAL_Delay(5);
        sector++;
    } else if (flashyazindex ==8192) {
        flashpageyaz(1,sector);
        HAL_Delay(5);
        sector++;
        sonpage = 1;
    } else if (strncmp((char *)RxBuffer, "<EOF>", 5) == 0 && sonpage == 1) {
        flashyazindex = 0;
        flashpageyaz(3,sector);
        HAL_Delay(5);
			boot = 0;
			yaz = 0;
        sector++;
        sonpage = 0;
    } else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)) {
        while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0));
        flashdanoku();
    }
}


void bootGirisEkrani()
{
	HAL_UART_Transmit(&huart3, (uint8_t *)"\n\r======================================\n\r", 42, 1000);
  HAL_UART_Transmit(&huart3, (uint8_t *)"            UART Bootloader        \n\r", 39, 1000);
  HAL_UART_Transmit(&huart3, (uint8_t *)"   https://github.com/Emirhan-KURU \n\r", 41, 1000);
  HAL_UART_Transmit(&huart3, (uint8_t *)"======================================\n\r", 40, 1000);

	HAL_UART_Transmit(&huart3, (uint8_t *)"_______________________________________     \n\r", 46, 1000);
	HAL_UART_Transmit(&huart3, (uint8_t *)"| [0] - BL_GET_VER            - 0x51   |    \n\r", 46, 1000);
	HAL_UART_Transmit(&huart3, (uint8_t *)"| [1] - BL_GET_HELP           - 0x52   |    \n\r", 46, 1000);
	HAL_UART_Transmit(&huart3, (uint8_t *)"| [2] - BL_GET_CID            - 0x53   |    \n\r", 46, 1000);
	HAL_UART_Transmit(&huart3, (uint8_t *)"| [3] - BL_GET_RDP_STATUS     - 0x54   |    \n\r", 46, 1000);
	HAL_UART_Transmit(&huart3, (uint8_t *)"| [4] - BL_GO_TO_ADDR         - 0x55   |    \n\r", 46, 1000);
	HAL_UART_Transmit(&huart3, (uint8_t *)"| [5] - BL_FLASH_ERASE        - 0x56   |    \n\r", 46, 1000);
	HAL_UART_Transmit(&huart3, (uint8_t *)"| [6] - BL_MEM_WRITE          - 0x57   |    \n\r", 46, 1000);
	HAL_UART_Transmit(&huart3, (uint8_t *)"| [7] - BL_ENDIS_RW_PROTECT   - 0x58   |    \n\r", 46, 1000);
	HAL_UART_Transmit(&huart3, (uint8_t *)"| [8] - BL_MEM_READ           - 0x59   |    \n\r", 46, 1000);
	HAL_UART_Transmit(&huart3, (uint8_t *)"| [9] - BL_READ_SECTOR_STATUS - 0x5A   |    \n\r", 46, 1000);
	HAL_UART_Transmit(&huart3, (uint8_t *)"| [A] - BL_OTP_READ           - 0x5B   |    \n\r", 46, 1000);
	HAL_UART_Transmit(&huart3, (uint8_t *)"| [B] - BL_DIS_R_W_PROTECT    - 0x5C   |    \n\r", 46, 1000);
	HAL_UART_Transmit(&huart3, (uint8_t *)"|______________________________________|\n\r", 45, 1000);
}

void BootMenuSettings()
{
		if(BootSetValue == BL_GET_HELP)
			BL_GET_HELP_FUNC();			
		if(BootSetValue == BL_GET_VER)
			BL_GET_VER_FUNC();				
		if(BootSetValue == BL_GET_CID)
			BL_GET_CID_FUNC();			
		if(BootSetValue == BL_FLASH_ERASE)
			BL_FLASH_ERASE_FUNC();
		if(BootSetValue == BL_MEM_READ)
		{
			BL_MEM_READ_FUNC();
		}
		
		BootSetValue = 0;
}


void BL_MEM_READ_FUNC()
{
    oku = 1;
    char buffer[20];
    uint16_t okunanData;
    uint32_t OkuAdress = 0x08000000;

    for (int j = 0; j < satir - 1; j++) {
        for (int i = 0; i < 8; i++) {
            okunanData = *(uint16_t *)OkuAdress;
            OkuAdress += 2;  // 16-bit okuduğumuz için adresi 2 artır

            // Veriyi 4 haneli hexadecimal formatında yazdır
            snprintf(buffer, sizeof(buffer), "%04X", okunanData);

            HAL_UART_Transmit(&huart3, (uint8_t *)buffer, strlen(buffer), 1000);
            HAL_UART_Transmit(&huart3, (uint8_t *)"  ", 2, 1000);  // Boşluk ekle
        }
        HAL_UART_Transmit(&huart3, (uint8_t *)"\r\n", 2, 1000);
    }
    oku = 0;
}


void BL_FLASH_ERASE_FUNC()
{
	/*	HAL_UART_Transmit(&huart3, (uint8_t *)"COMPLATED FULL ERASE CHIP!!!\n\r", 
	strlen("COMPLATED FULL ERASE CHIP!!!\n\r"), 1000);
	HAL_FLASH_Unlock();
	
	FLASH_Erase_Sector(1,FLASH_VOLTAGE_RANGE_3);
	HAL_FLASH_Lock();
	
*/
}


void BL_GET_CID_FUNC()
{
	HAL_UART_Transmit(&huart3, (uint8_t *)"Chip ID  0x413\n\r", 
											strlen("Chip ID  0x413\n\r"), 1000);	
}


void BL_GET_VER_FUNC()
{
	BootSettingsAdress =  0x080E0000;
	
	bootsetvalues.bl_version = *(uint32_t *)BootSettingsAdress;
	char buffer1[20];
	char buffer2[20];
	char buffer3[20];


	unsigned long mod1 = bootsetvalues.bl_version / 200;
	sprintf(buffer1, "%lu", mod1);

	unsigned long mod2 = (bootsetvalues.bl_version- (mod1*200)) / 20;
	sprintf(buffer2, "%lu", mod2);

	
	unsigned long mod3 =((bootsetvalues.bl_version- (mod1*200)) - (mod2*20));
	sprintf(buffer3, "%lu", mod3);
	
	char version_info[50];
	snprintf(version_info, sizeof(version_info), "version %s.%s.%s\n\r", buffer1, buffer2, buffer3);
HAL_UART_Transmit(&huart3, (uint8_t *)version_info, strlen(version_info), 1000);

}


void BootSeettingsEraseAndWrite()
{
	if(1 == boot)
	{
	BootSettingsAdress =  0x080E0000;
	bootsetvalues.bl_version = *(uint32_t *)BootSettingsAdress;
	bootsetvalues.chip_id = *(uint32_t *)(BootSettingsAdress + 4);
	bootsetvalues.protect = *(uint32_t *)(BootSettingsAdress + 8);
	
	
if(bootsetvalues.bl_version == 0xFFFFFFFF)
	{
		bootsetvalues.bl_version =0;
	}
	bootsetvalues.bl_version++;
	HAL_FLASH_Unlock();
	FLASH_Erase_Sector(11,FLASH_VOLTAGE_RANGE_3);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, BootSettingsAdress,bootsetvalues.bl_version);
	HAL_FLASH_Lock();	
	boot =0;
}
	
	
}

void BL_GET_HELP_FUNC()
{
		HAL_UART_Transmit(&huart3, (uint8_t *)" HOST SEND              NOTES\n\r", 
											strlen(" HOST SEND              NOTES\n\r"), 1000);

		HAL_UART_Transmit(&huart3, (uint8_t *)" ----------------------------------------------------\n\r", 
											strlen(" ----------------------------------------------------\n\r"), 1000);

		HAL_UART_Transmit(&huart3, (uint8_t *)" BL_GET_VER            : Read the bootloader version "
																				"from MCU \n\r", 
											strlen(" BL_GET_VER            : Read the bootloader version "
																				"from MCU \n\r"), 1000);

		HAL_UART_Transmit(&huart3, (uint8_t *)" BL_GET_CID            : Read the MCU chip "
																				"identification number\n\r", 
											strlen(" BL_GET_CID            : Read the MCU chip "
																				"identification number\n\r"), 1000);

		HAL_UART_Transmit(&huart3, (uint8_t *)" BL_GET_RDP_STATUS     : Read the FLASH Read "
																				"Protection level\n\r", 
											strlen(" BL_GET_RDP_STATUS     : Read the FLASH Read "
																				"Protection level\n\r"), 1000);

		HAL_UART_Transmit(&huart3, (uint8_t *)" BL_GO_TO_ADDR         : Jump bootloader to "
																				"specified address\n\r", 
											strlen(" BL_GO_TO_ADDR         : Jump bootloader to "
																				"specified address\n\r"), 1000);

		HAL_UART_Transmit(&huart3, (uint8_t *)" BL_FLASH_ERASE        : Mass erase or sector erase "
																				"of the user flash\n\r", 
											strlen(" BL_FLASH_ERASE        : Mass erase or sector erase "
																				"of the user flash\n\r"), 1000);

		HAL_UART_Transmit(&huart3, (uint8_t *)" BL_MEM_WRITE          : Write data into different "
																				"memories of the MCU\n\r", 
											strlen(" BL_MEM_WRITE          : Write data into different "
																				"memories of the MCU\n\r"), 1000);

		HAL_UART_Transmit(&huart3, (uint8_t *)" BL_ENDIS_RW_PROTECT   : Enable/disable read/write "
																				"protect on different sectors of the user flash.\n\r", 
											strlen(" BL_ENDIS_RW_PROTECT   : Enable/disable read/write "
																				"protect on different sectors of the user flash.\n\r"), 1000);

		HAL_UART_Transmit(&huart3, (uint8_t *)" BL_MEM_READ           : Read data from different "
																				"memories of the microcontroller\n\r", 
											strlen(" BL_MEM_READ           : Read data from different "
																				"memories of the microcontroller\n\r"), 1000);

		HAL_UART_Transmit(&huart3, (uint8_t *)" BL_READ_SECTOR_STATUS : Read all of the sector "
																				"protection statuses\n\r", 
											strlen(" BL_READ_SECTOR_STATUS : Read all of the sector "
																				"protection statuses\n\r"), 1000);

		HAL_UART_Transmit(&huart3, (uint8_t *)" BL_OTP_READ           : Read the OTP contents\n\r", 
											strlen(" BL_OTP_READ           : Read the OTP contents\n\r"), 1000);
											
		HAL_UART_Transmit(&huart3, (uint8_t *)"Cikis ici : q\n\r", 
											strlen("Cikis ici : q\n\r"), 1000);
}

