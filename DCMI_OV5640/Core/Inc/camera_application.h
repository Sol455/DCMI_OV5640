/*
 * camera.h
 *
 *  Created on: Jan 12, 2023
 *      Author: sol
 */

#ifndef INC_CAMERA_APPLICATION_H_
#define INC_CAMERA_APPLICATION_H_

#include "stm32f7xx_hal.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_sdram.h"
#include "stm32746g_discovery_ov5640.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
#include "stdio.h"
#include "stdlib.h"
//#include "lcd.h"
#include "string.h"


//#include "ov5460.h"
//#include "../../Drivers/BSP/Components/ov5640/ov5640.h"
//#include "../Drivers/BSP/Components/ov9665/ov9665.h"


#define CAMERA_FRAME_BUFFER               0xC0260000
//#define CAMERA_FRAME_BUFFER               0x20010000
//#define LCD_FRAME_BUFFER 0x20010000
//#define CAMERA_FRAME_BUFFER               0x60000000
#define LCD_FRAME_BUFFER                  0xC0130000

//#define CAMERA_I2C_ADDRESS (uint16_t)0x60
void I2C_CameraTest(void);
void cameraTestLoop(void);
void statusTest(void);
void initialiseCapture(void);
void DCMI_GetCaptureMode(void);
void runCamera(void);
void BSP_CAMERA_LineEventCallback(void);
void BSP_CAMERA_FrameEventCallback(void);
void LCD_LL_ConvertLineToARGB8888(void *pSrc, void *pDst);
void LCD_DMA_Transfer_RGBTOARGB8888(void *pSrc, void *pDst);
void LCD_init(void);
//void BSP_CAMERA_VsyncEventCallback(void);
void FPSCalculate(void);

#endif /* INC_CAMERA_H_ */
