/*
 * camera.c
 *
 *  Created on: Jan 12, 2023
 *      Author: sol
 */

#include "camera_application.h"


DMA2D_HandleTypeDef hdma2d_eval;
uint32_t  *ptrLcd;
uint8_t status = CAMERA_OK;

uint8_t *cam_fb;
uint32_t cam_fb_size;


void LCD_init(void) {

	BSP_LCD_Init();
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
		  printf("done screen\n");
}


void initialiseCapture(void) {
    //cam_fb_size = 480*272*2;			// Resolution * color depth; RGB565=16bit=2byte
    //cam_fb = (uint8_t *) malloc(cam_fb_size);

    memset(CAMERA_FRAME_BUFFER, 255, cam_fb_size);

	BSP_CAMERA_Init(CAMERA_R480x272);
	BSP_CAMERA_ContinuousStart((uint8_t *)CAMERA_FRAME_BUFFER);
}

BSP_CAMERA_VsyncEventCallback(void) {
	LCD_DMA_Transfer_RGBTOARGB8888((uint32_t *)(CAMERA_FRAME_BUFFER), (uint32_t *)(LCD_FRAME_BUFFER));
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
	//LCD_DMA_Transfer_RGBTOARGB8888((uint32_t *)(CAMERA_FRAME_BUFFER), (uint32_t *)(LCD_FRAME_BUFFER));
	//printf("\nFrame Event Callback\n");
}

//void BSP_CAMERA_LineEventCallback(void)
//{
//  static uint32_t tmp, tmp2, counter;
//  if(272 > counter)
//  {
//    LCD_LL_ConvertLineToARGB8888((uint32_t *)(CAMERA_FRAME_BUFFER + tmp), (uint32_t *)(LCD_FRAME_BUFFER + tmp2));
//    tmp  = tmp + 480*sizeof(uint16_t);
//    tmp2 = tmp2 + (480) * sizeof(uint32_t);
//    counter++;
//  }
//  else
//  {
//	//FRAME Event//
//	//LCD_DMA_Transfer_RGBTOARGB8888((uint32_t *)(CAMERA_FRAME_BUFFER), (uint32_t *)(LCD_FRAME_BUFFER));
//    tmp = 0;
//    tmp2 = 0;
//    counter = 0;
//
//  }
//}

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

	    if(HAL_DMA2D_ConfigLayer(&hdma2d_eval, 1) == HAL_OK)
	    {
	      if (HAL_DMA2D_Start(&hdma2d_eval, (uint32_t)pSrc, (uint32_t)pDst, 480, 272) == HAL_OK)
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


void LCD_LL_ConvertLineToARGB8888(void *pSrc, void *pDst)
{
  /* Enable DMA2D clock */
  __HAL_RCC_DMA2D_CLK_ENABLE();

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


void BSP_CAMERA_ErrorCallback(void) {
	printf("\nDCMI ERROR\n");
}
