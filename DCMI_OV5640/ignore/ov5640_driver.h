/*
 * ov5640_driver.h
 *
 *  Created on: Feb 6, 2023
 *      Author: sol
 */






#ifndef __OV5640_DRIVER_H
#define __OV5640_DRIVER_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ov5640.h"
#include "stm32746g_discovery.h"
#include "camera.h"
#include "stm32f7xx_hal.h"

void BSP_CAMERA_MspInit(DCMI_HandleTypeDef *hdcmi, void *Params);
uint8_t CAMERA_Init(uint32_t Resolution);
void Start_Video_Feed(void);



#endif
