/*
 * HexToFlash.h
 *
 *  Created on: Aug 21, 2024
 *      Author: emirhan
 */

#ifndef INC_HEXTOFLASH_H_
#define INC_HEXTOFLASH_H_


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stm32f4xx_hal.h>



#define RECORD_LENGTH 2
#define ADRESS_LENGTH 4
#define RECORD_TYPE_LENGTH 2
#define CHECKSUM_LENGTH 2


#define RxBufferSize 44

extern int flashdatauzunlugu;
extern int page;
extern char page_str[10];

extern int sonpage;

extern char BinAdress[3];
extern uint16_t BinData[17];
extern uint16_t BinData2[17];
extern int kalanindex;
extern int boot;
extern int update;

extern uint32_t FlashAdress;
extern FLASH_EraseInitTypeDef EraseInitStruct;
extern uint32_t PageError;
extern uint8_t RxBuffer[RxBufferSize];
extern int satir;
extern uint16_t checksum_kontrol;
extern uint8_t checksum_kontrol2;
extern uint16_t flashayazData[16*1024];
extern uint16_t flashyazindex;
extern char CC;
extern int doru;
extern int yanlis;

extern int sector;

extern int oku;
extern int yaz;

extern uint32_t BootSettingsAdress;

// RX Data Parse Params

typedef struct HexBuff
{
    char Record_Length[3];
    char Adress[5];
    char Record_Type[3];
    char Data[33];
    char checkSum[3];

}HexBuff;

typedef struct HexInt
{
	uint64_t data;
	uint32_t adress;
	uint8_t recordType;
  uint8_t recordLength;
  uint8_t checksum;

}HexInt;


extern struct HexBuff hexbuff;
extern struct HexInt hexint;

// End RX Data Parse Params

// -----------------------------------------

// Booloader Settings Params
typedef enum {
    BL_GET_VER             =    0x51,
    BL_GET_HELP            =    0x52,
    BL_GET_CID             =    0x53,
		BL_GET_RDP_STATUS      =    0x54,
		BL_GO_TO_ADDR          =    0x55,
		BL_FLASH_ERASE         =    0x56,
		BL_MEM_WRITE           =    0x57,
		BL_ENDIS_RW_PROTECT    =    0x58,
		BL_MEM_READ            =    0x59,
		BL_READ_SECTOR_STATUS  =    0x5A,
		BL_OTP_READ            =    0x5B,
		BL_DIS_R_W_PROTECT     =    0x5C
} BootSettings;

extern uint8_t BootSetValue;


typedef struct BootSetValues
{
	uint32_t chip_id;
	uint32_t bl_version;
	uint32_t protect;
}BootSetValues; 

extern struct BootSetValues bootsetvalues;

// End Bootloader Settings Params

void flashdanoku();
void flashpageyaz(int index,int sector);
void HexToFlash();
void HexToFlashInit();
void bootGirisEkrani();

// boot settings funcs
void BootMenuSettings();
void BL_GET_HELP_FUNC();
void BL_GET_VER_FUNC();
void BL_GET_CID_FUNC();
void BL_FLASH_ERASE_FUNC();
void BL_MEM_READ_FUNC();
	
void BootSeettingsEraseAndWrite();


#endif /* INC_HEXTOFLASH_H_ */
