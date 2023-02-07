/*
 * camera.c
 *
 *  Created on: Jan 12, 2023
 *      Author: sol
 */

#include "camera_application.h"


DMA2D_HandleTypeDef hdma2d_eval;
//static TS_StateTypeDef  TS_State;

//uint32_t  i, previous_mode = CONTINOUS_MODE;
uint32_t  *ptrLcd;
uint8_t status = CAMERA_OK;

uint8_t *cam_fb;
uint32_t cam_fb_size;


void LCD_init(void) {

BSP_LCD_Init();
BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());

	uint32_t  *ptrLcd;


	  /* Init LCD screen buffer */
	  ptrLcd = (uint32_t*)(LCD_FRAME_BUFFER);
	  for (int i=0; i<(BSP_LCD_GetXSize()*BSP_LCD_GetYSize()); i++)
	  {
	    ptrLcd[i]=0;
	  }

	  BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, LCD_FRAME_BUFFER);
	  //BSP_LCD_LayerRgb565Init(LTDC_ACTIVE_LAYER, LCD_FRAME_BUFFER);

	  /* Set LCD Foreground Layer  */
	  BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);

//	  BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

	  /* Clear the LCD */
	  BSP_LCD_Clear(LCD_COLOR_BLACK);
//	  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
//	  BSP_LCD_FillRect(340, 80, 80, 30);
//	  BSP_LCD_FillRect(340, 150, 80, 30);
//	  BSP_LCD_DrawRect(8, 8, 322, 242);
//	  BSP_LCD_DrawRect(9, 9, 321, 241);
//	  BSP_LCD_SetFont(&Font12);
//	  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
//	  BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
//	  BSP_LCD_DisplayStringAt(345, 90, (uint8_t *)"CONTINUOUS", LEFT_MODE);
//	  BSP_LCD_DisplayStringAt(345, 160, (uint8_t *)" SNAPSHOT", LEFT_MODE);
//	  BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

	  printf("done screen\n");
}






void initialiseCapture(void) {
//status = BSP_CAMERA_Init(RESOLUTION_R320x240);
//if(status != CAMERA_OK)
//{
//  BSP_LCD_SetFont(&Font16);
//  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
//  BSP_LCD_FillRect(10, 10, 320, 240);
//  BSP_LCD_SetTextColor(LCD_COLOR_RED);
//  BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
//  BSP_LCD_DisplayStringAt(20, 100, (uint8_t *)"CAMERA sensor is unpluged", LEFT_MODE);
//  BSP_LCD_DisplayStringAt(20, 120, (uint8_t *)"Plug the OV2640 sensor   ", LEFT_MODE);
//  BSP_LCD_DisplayStringAt(20, 140, (uint8_t *)"on P1 camera connector   ", LEFT_MODE);
//  BSP_LCD_DisplayStringAt(20, 160, (uint8_t *)"And restart the program  ", LEFT_MODE);
//}
//else
//{
//	BSP_CAMERA_Init(CAMERA_R480x272);
//	BSP_CAMERA_ContinuousStart((uint8_t *)CAMERA_FRAME_BUFFER);
//	BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());

	  /* Start the Camera Capture */
	  //BSP_CAMERA_ContinuousStart((uint8_t *)CAMERA_FRAME_BUFFER);
	  //CaptureMode = CONTINOUS_MODE;

	  /* Initialize the Camera */

    cam_fb_size = 480*272*2;			// Resolution * color depth; RGB565=16bit=2byte
//
    cam_fb = (uint8_t *) malloc(cam_fb_size);

    memset(CAMERA_FRAME_BUFFER, 255, cam_fb_size);

	BSP_CAMERA_Init(CAMERA_R480x272);
	BSP_CAMERA_ContinuousStart((uint8_t *)CAMERA_FRAME_BUFFER);
//
//}
}

BSP_CAMERA_VsyncEventCallback(void) {
	//LCD_DMA_Transfer_RGBTOARGB8888((uint32_t *)(CAMERA_FRAME_BUFFER), (uint32_t *)(LCD_FRAME_BUFFER + 0x4B28));
	//frameCounter++;
}




//FPSCalculate(void)  {
////	float frameRate;
////	frameRate = (float)frameCounter / 60.0;
//	printf("\n%i FPS\n", frameCounter);
//	frameCounter = 0;
//	printf("\n%i FPS\n", frameCounter);
//}

void BSP_CAMERA_FrameEventCallback(void) {
	 printf("\nFrame Event Callback\n");
}

void BSP_CAMERA_LineEventCallback(void)
{
  static uint32_t tmp, tmp2, counter;
  if(272 > counter)
  {
    LCD_LL_ConvertLineToARGB8888((uint32_t *)(CAMERA_FRAME_BUFFER + tmp), (uint32_t *)(LCD_FRAME_BUFFER + tmp2));
    tmp  = tmp + 480*sizeof(uint16_t);
    tmp2 = tmp2 + (480) * sizeof(uint32_t);
    counter++;
  }
  else
  {
//FRAME Event
	//LCD_DMA_Transfer_RGBTOARGB8888((uint32_t *)(CAMERA_FRAME_BUFFER), (uint32_t *)(LCD_FRAME_BUFFER));
    tmp = 0;
    tmp2 = 0;
    counter = 0;

  }
}

void LCD_DMA_Transfer_RGBTOARGB8888(void *pSrc, void *pDst) {
	  /* Enable DMA2D clock */
	  //__HAL_RCC_DMA2D_CLK_ENABLE();

	  /* Configure the DMA2D Mode, Color Mode and output offset */
	  hdma2d_eval.Init.Mode         = DMA2D_M2M_PFC;
	  hdma2d_eval.Init.ColorMode    = DMA2D_OUTPUT_ARGB8888;
	  //hdma2d_eval.Init.ColorMode    = DMA2D_OUTPUT_RGB565;
	  hdma2d_eval.Init.OutputOffset = 0;

	  /* Foreground Configuration */
	  hdma2d_eval.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
	  hdma2d_eval.LayerCfg[1].InputAlpha = 0xFF;
	  hdma2d_eval.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB565;
	  hdma2d_eval.LayerCfg[1].InputOffset = 0;

	  hdma2d_eval.Instance = DMA2D;

	  /* DMA2D Initialization */
	  if(HAL_DMA2D_Init(&hdma2d_eval) == HAL_OK)
	  {
		  //printf("\nDMA Frame transfer init sucessfully\n");
	    if(HAL_DMA2D_ConfigLayer(&hdma2d_eval, 1) == HAL_OK)
	    {
	    	//printf("\nDMA Layer config init sucessfully\n");
	      if (HAL_DMA2D_Start(&hdma2d_eval, (uint32_t)pSrc, (uint32_t)pDst, 480, 272) == HAL_OK)
	      {
	        /* Polling For DMA transfer */
	    	 //printf("\npolling for DMA transfer Frame!\n");
	        HAL_DMA2D_PollForTransfer(&hdma2d_eval, 10);
	      }
	    }
	  }
	  else
	  {
	    /* FatFs Initialization Error */
		  printf("\nFatFs Initialization Error\n");
	    while(1);
	  }

}


/**
  * @brief  Converts a line to an ARGB8888 pixel format.
  * @param  pSrc Pointer to source buffer
  * @param  pDst Output color
  * @param  xSize Buffer width
  * @param  ColorMode Input color mode
  * @retval None
  */

void LCD_LL_ConvertLineToARGB8888(void *pSrc, void *pDst)
{
  /* Enable DMA2D clock */
  //__HAL_RCC_DMA2D_CLK_ENABLE();

  /* Configure the DMA2D Mode, Color Mode and output offset */
  hdma2d_eval.Init.Mode         = DMA2D_M2M_PFC;
  hdma2d_eval.Init.ColorMode    = DMA2D_OUTPUT_ARGB8888;
  //hdma2d_eval.Init.ColorMode    = DMA2D_OUTPUT_RGB565;
  hdma2d_eval.Init.OutputOffset = 0;

  /* Foreground Configuration */
  hdma2d_eval.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hdma2d_eval.LayerCfg[1].InputAlpha = 0xFF;
  hdma2d_eval.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB565;
  hdma2d_eval.LayerCfg[1].InputOffset = 0;

  hdma2d_eval.Instance = DMA2D;

  /* DMA2D Initialization */
  if(HAL_DMA2D_Init(&hdma2d_eval) == HAL_OK)
  {
    if(HAL_DMA2D_ConfigLayer(&hdma2d_eval, 1) == HAL_OK)
    {
      if (HAL_DMA2D_Start(&hdma2d_eval, (uint32_t)pSrc, (uint32_t)pDst, 480, 1) == HAL_OK)
      {
        /* Polling For DMA transfer */

        HAL_DMA2D_PollForTransfer(&hdma2d_eval, 10);
      }
    }
  }
  else
  {
    /* FatFs Initialization Error */
	  printf("\nFatFs Initialization Error\n");
    while(1);
  }
}




//else
//BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());

  /* Start the Camera Capture */
  //BSP_CAMERA_ContinuousStart((uint8_t *)CAMERA_FRAME_BUFFER);
  //CaptureMode = CONTINOUS_MODE;

  /* Initialize the Camera */

  //cam_fb_size = 320*240*2;			// Resolution * color depth; RGB565=16bit=2byte

  //cam_fb = (uint8_t *) malloc_ext(cam_fb_size);

void BSP_CAMERA_ErrorCallback(void) {
	printf("\nDCMI ERROR\n");
}

//void DCMI_GetCaptureMode(void)
//{
//  //HAL_Delay(500);
//  uint16_t x, y;
//  /* Check in polling mode in touch screen the touch status and coordinates */
//  /* if touch occurred                                                      */
//  BSP_TS_GetState(&TS_State);
//  if(TS_State.touchDetected)
//  {
//	 //printf("\nCaptureMode\n");
//    /* Get X and Y position of the touch post calibrated */
//    x = TS_State.touchX[0];
//    y = TS_State.touchY[0];
//
//    if((x > 340) && (x < 420))
//    {
//      if((y > 80) && (y < 110))
//      {
//        CaptureMode = CONTINOUS_MODE;
//        //BSP_LCD_DisplayStringAt(20, 30, (uint8_t *)"CONT", CENTER_MODE);
//
//      }
//      if((y > 150) && (y < 180))
//      {
//        CaptureMode = SNAPSHOT_MODE;
//        //BSP_LCD_DisplayStringAt(20, 30, (uint8_t *)"SNAP", CENTER_MODE);
//        //printf("\nSnapshotMode\n");
//      }
//    }
//  }
//}
