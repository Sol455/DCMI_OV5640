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
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define CAMERA_FRAME_BUFFER               0xC0260000
#define LCD_FRAME_BUFFER                  0xC0130000

void initialiseCapture(void);
void BSP_CAMERA_LineEventCallback(void);
void BSP_CAMERA_FrameEventCallback(void);
void BSP_CAMERA_VsyncEventCallback(void);
void LCD_LL_ConvertLineToARGB8888(void *pSrc, void *pDst);
void LCD_DMA_Transfer_RGBTOARGB8888(void *pSrc, void *pDst);
void LCD_init(void);
void FPSCalculate(void);

#endif /* INC_CAMERA_H_ */
