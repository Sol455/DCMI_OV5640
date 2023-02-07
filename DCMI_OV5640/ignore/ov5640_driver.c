/*
 * ov5640_driver.c
 *
 *  Created on: Feb 6, 2023
 *      Author: sol
 */

#define CAMERA_FRAME_BUFFER               0xC0260000

#include "ov5640_driver.h"
#include "stdio.h"

#define QVGA_RES_X	320
#define QVGA_RES_Y	240

#define WQVGA_RES_X  480
#define WQVGA_RES_Y  272

#define VGA_RES_X  640
#define VGA_RES_Y  480

#define WVGA_RES_X  800
#define WVGA_RES_Y  480

#define ARGB8888_BYTE_PER_PIXEL  4
#define RGB565_BYTE_PER_PIXEL    2

static volatile uint32_t start_the_camera_capture = 0;
typedef enum
{
	CAMERA_OK = 0x00, CAMERA_ERROR = 0x01, CAMERA_TIMEOUT = 0x02, CAMERA_NOT_DETECTED = 0x03, CAMERA_NOT_SUPPORTED = 0x04
} Camera_StatusTypeDef;



static RCC_PeriphCLKInitTypeDef periph_clk_init_struct;
DCMI_HandleTypeDef  hDcmiHandler;
CAMERA_DrvTypeDef *camera_driv;

static uint32_t CameraCurrentResolution;

/* Camera module I2C HW address */



/* Camera module I2C HW address */
static uint32_t CameraHwAddress;
/* Image size */
uint32_t Im_size = 0;

/* Camera resolution is QWVGA (480x272) */
static uint32_t   CameraResX = QVGA_RES_X;
static uint32_t   CameraResY = QVGA_RES_Y;
static uint32_t   LcdResX    = WQVGA_RES_X; /* QWVGA landscape */
static uint32_t   LcdResY    = WQVGA_RES_Y;

static uint32_t          offset_cam = 0;
static uint32_t          offset_lcd = 0;

__IO uint16_t pBuffer[QVGA_RES_X * QVGA_RES_Y];
HAL_StatusTypeDef hal_status = HAL_OK;


//function prototypes
uint8_t CAMERA_Init(uint32_t );
void CAMERA_PwrDown(void);
void CAMERA_PwrUp(void);

//static void LTDC_Init(uint32_t , uint16_t , uint16_t , uint16_t, uint16_t);
//void LCD_GPIO_Init(LTDC_HandleTypeDef *, void *);
static void OnError_Handler(uint32_t condition);



///capture function ===================================
void Start_Video_Feed(void) {
CAMERA_PwrDown();
	hal_status = BSP_SDRAM_Init();
	//OnError_Handler(hal_status != HAL_OK);

	/*##-3- Camera Initialization and start capture ############################*/
	/* Initialize the Camera in WQVGA mode */
	  /* Init camera default resolution and format */
	//hal_status = BSP_CAMERA_Init(CAMERA_R320x240);
	CAMERA_PwrUp();
	HAL_Delay(1000);
	//hal_status = CAMERA_Init(CAMERA_R320x240);
	//hal_status = CAMERA_Init(CAMERA_R480x272);
	CAMERA_Init(CAMERA_R480x272);
	//OV5640_SetLightMode(CAMERA_I2C_ADDRESS,OV5640_LIGHT_AUTO);


	//OnError_Handler(hal_status != HAL_OK);

	HAL_Delay(1000); //Delay for the camera to output correct data
	Im_size = 0x9600; //size=320*240*2/4
	Im_size = 0xff00;  //size=480*272*2/4

	/*##-4- Camera Continuous capture start in QVGA resolution ############################*/
	/* Disable unwanted HSYNC (IT_LINE)/VSYNC interrupts */
	__HAL_DCMI_DISABLE_IT(&hDcmiHandler, DCMI_IT_LINE | DCMI_IT_VSYNC);
	//__HAL_DCMI_ENABLE_IT(&hdcmi, DCMI_IT_FRAME);

	/* uncomment the following line in case of snapshot mode */
	//HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT, (uint32_t)CAMERA_FRAME_BUFFER, Im_size);
	/* uncomment the following line in case of continuous mode */
	hal_status = HAL_DCMI_Start_DMA(&hDcmiHandler, DCMI_MODE_CONTINUOUS , (uint32_t)CAMERA_FRAME_BUFFER, Im_size);
	//OnError_Handler(hal_status != HAL_OK);

	//LTDC_Init(FRAME_BUFFER, 0, 0, 480, 272);
}


///capture function ===================================

	uint8_t CAMERA_Init(uint32_t Resolution) /*Camera initialization*/
	{

		  DCMI_HandleTypeDef *phdcmi;
		  uint8_t status = CAMERA_ERROR;

		  /* Get the DCMI handle structure */
		  phdcmi = &hDcmiHandler;

		  /*** Configures the DCMI to interface with the camera module ***/
		  /* DCMI configuration */
		  phdcmi->Init.CaptureRate      = DCMI_CR_ALL_FRAME;
		  phdcmi->Init.HSPolarity       = DCMI_HSPOLARITY_HIGH;
		  phdcmi->Init.SynchroMode      = DCMI_SYNCHRO_HARDWARE;
		  phdcmi->Init.VSPolarity       = DCMI_VSPOLARITY_HIGH;
		  phdcmi->Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
		  phdcmi->Init.PCKPolarity      = DCMI_PCKPOLARITY_RISING;
		  phdcmi->Instance              = DCMI;

		status = CAMERA_ERROR;
		printf("\nCAMERA EST\n");
		/* Read ID of Camera module via I2C */
		if(ov5640_ReadID(CAMERA_I2C_ADDRESS) == OV5640_ID)
		{
		    BSP_CAMERA_MspInit(&hDcmiHandler, NULL);
		    HAL_DCMI_Init(phdcmi);
		    printf("\nREAD ID\n");

			camera_driv = &ov5640_drv;

			/* Initialize the camera driver structure */
			CameraHwAddress = CAMERA_I2C_ADDRESS;
			if (Resolution == CAMERA_R320x240)
			{
				camera_driv->Init(CameraHwAddress, Resolution);
				HAL_DCMI_DisableCROP(&phdcmi);
			    HAL_Delay(500);
			    printf("\nINITed\n");

			}
			if (Resolution == CAMERA_R480x272)
			{
				camera_driv->Init(CameraHwAddress, Resolution);
				HAL_DCMI_DisableCROP(&phdcmi); //phdcmi
			    HAL_Delay(500);
			    printf("\n480x272\n");
		    }

			status = CAMERA_OK; /* Return CAMERA_OK status */
		}
		else
		{
			status = CAMERA_NOT_SUPPORTED;
			printf("\nNot support\n");/* Return CAMERA_NOT_SUPPORTED status */
		}
		return status;
	}

	/**
	  * @brief  CANERA power up
	  * @retval None
	  */
	void CAMERA_PwrUp(void)
	{
	  GPIO_InitTypeDef gpio_init_structure;

	  /* Enable GPIO clock */
	  __HAL_RCC_GPIOH_CLK_ENABLE();

	  /*** Configure the GPIO ***/
	  /* Configure DCMI GPIO as alternate function */
	  gpio_init_structure.Pin       = GPIO_PIN_13;
	  gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
	  gpio_init_structure.Pull      = GPIO_NOPULL;
	  gpio_init_structure.Speed     = GPIO_SPEED_HIGH;
	  HAL_GPIO_Init(GPIOH, &gpio_init_structure);

	  /* De-assert the camera POWER_DOWN pin (active high) */
	  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_13, GPIO_PIN_RESET);

	  HAL_Delay(3);     /* POWER_DOWN de-asserted during 3ms */
	}

	/**
	  * @brief  CAMERA power down
	  * @retval None
	  */
	void CAMERA_PwrDown(void)
	{
	  GPIO_InitTypeDef gpio_init_structure;

	  /* Enable GPIO clock */
	  __HAL_RCC_GPIOH_CLK_ENABLE();

	  /*** Configure the GPIO ***/
	  /* Configure DCMI GPIO as alternate function */
	  gpio_init_structure.Pin       = GPIO_PIN_13;
	  gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
	  gpio_init_structure.Pull      = GPIO_NOPULL;
	  gpio_init_structure.Speed     = GPIO_SPEED_HIGH;
	  HAL_GPIO_Init(GPIOH, &gpio_init_structure);

	  /* Assert the camera POWER_DOWN pin (active high) */
	  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_13, GPIO_PIN_SET);
	}

	__weak void BSP_CAMERA_MspInit(DCMI_HandleTypeDef *hdcmi, void *Params)
	{
	  static DMA_HandleTypeDef hdma_handler;
	  GPIO_InitTypeDef gpio_init_structure;

	  /*** Enable peripherals and GPIO clocks ***/
	  /* Enable DCMI clock */
	  __HAL_RCC_DCMI_CLK_ENABLE();

	  /* Enable DMA2 clock */
	  __HAL_RCC_DMA2_CLK_ENABLE();

	  /* Enable GPIO clocks */
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	  __HAL_RCC_GPIOD_CLK_ENABLE();
	  __HAL_RCC_GPIOE_CLK_ENABLE();
	  __HAL_RCC_GPIOG_CLK_ENABLE();
	  __HAL_RCC_GPIOH_CLK_ENABLE();

	  /*** Configure the GPIO ***/
	  /* Configure DCMI GPIO as alternate function */
	  gpio_init_structure.Pin       = GPIO_PIN_4 | GPIO_PIN_6;
	  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
	  gpio_init_structure.Pull      = GPIO_PULLUP;
	  gpio_init_structure.Speed     = GPIO_SPEED_HIGH;
	  gpio_init_structure.Alternate = GPIO_AF13_DCMI;
	  HAL_GPIO_Init(GPIOA, &gpio_init_structure);

	  gpio_init_structure.Pin       = GPIO_PIN_3;
	  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
	  gpio_init_structure.Pull      = GPIO_PULLUP;
	  gpio_init_structure.Speed     = GPIO_SPEED_HIGH;
	  gpio_init_structure.Alternate = GPIO_AF13_DCMI;
	  HAL_GPIO_Init(GPIOD, &gpio_init_structure);

	  gpio_init_structure.Pin       = GPIO_PIN_5 | GPIO_PIN_6;
	  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
	  gpio_init_structure.Pull      = GPIO_PULLUP;
	  gpio_init_structure.Speed     = GPIO_SPEED_HIGH;
	  gpio_init_structure.Alternate = GPIO_AF13_DCMI;
	  HAL_GPIO_Init(GPIOE, &gpio_init_structure);

	  gpio_init_structure.Pin       = GPIO_PIN_9;
	  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
	  gpio_init_structure.Pull      = GPIO_PULLUP;
	  gpio_init_structure.Speed     = GPIO_SPEED_HIGH;
	  gpio_init_structure.Alternate = GPIO_AF13_DCMI;
	  HAL_GPIO_Init(GPIOG, &gpio_init_structure);

	  gpio_init_structure.Pin       = GPIO_PIN_9 | GPIO_PIN_10  | GPIO_PIN_11  |\
	                                  GPIO_PIN_12 | GPIO_PIN_14;
	  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
	  gpio_init_structure.Pull      = GPIO_PULLUP;
	  gpio_init_structure.Speed     = GPIO_SPEED_HIGH;
	  gpio_init_structure.Alternate = GPIO_AF13_DCMI;
	  HAL_GPIO_Init(GPIOH, &gpio_init_structure);

	  /*** Configure the DMA ***/
	  /* Set the parameters to be configured */
	  hdma_handler.Init.Channel             = DMA_CHANNEL_1;
	  hdma_handler.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	  hdma_handler.Init.PeriphInc           = DMA_PINC_DISABLE;
	  hdma_handler.Init.MemInc              = DMA_MINC_ENABLE;
	  hdma_handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	  hdma_handler.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
	  hdma_handler.Init.Mode                = DMA_CIRCULAR;
	  hdma_handler.Init.Priority            = DMA_PRIORITY_HIGH;
	  hdma_handler.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
	  hdma_handler.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	  hdma_handler.Init.MemBurst            = DMA_MBURST_SINGLE;
	  hdma_handler.Init.PeriphBurst         = DMA_PBURST_SINGLE;

	  hdma_handler.Instance = DMA2_Stream1;

	  /* Associate the initialized DMA handle to the DCMI handle */
	  __HAL_LINKDMA(hdcmi, DMA_Handle, hdma_handler);

	  /*** Configure the NVIC for DCMI and DMA ***/
	  /* NVIC configuration for DCMI transfer complete interrupt */
	  HAL_NVIC_SetPriority(DCMI_IRQn, 0x0F, 0);
	  HAL_NVIC_EnableIRQ(DCMI_IRQn);

	  /* NVIC configuration for DMA2D transfer complete interrupt */
	  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0x0F, 0);
	  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

	  /* Configure the DMA stream */
	  HAL_DMA_Init(hdcmi->DMA_Handle);
	}
