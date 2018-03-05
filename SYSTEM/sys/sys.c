/**********************************************************
* @ File name -> sys.c
* @ Version   -> V1.0
* @ Date      -> 12-26-2013
* @ Brief     -> 系统设置相关的函数
**********************************************************/

#include "sys.h"

/**********************************************************
* 函数功能 ---> 读取芯片闪存容量
* 入口参数 ---> *STMCapa：容量字符显示缓存
* 返回数值 ---> 容量（十进制）
* 功能说明 ---> none
**********************************************************/
void STM32_Flash_Capacity(uint8_t *STMCapa)
{
	uint16_t capa;
	
	capa = *((uint16_t*)0x1ffff7e0);	//读取闪存容量寄存器，低16位有效
	
	capa = ((capa >> 12) * 4096 + ((capa >> 8) & 0x0f) * 256 + ((capa >> 4) & 0x0f) * 16 + (capa & 0x0f));	//转换成十进制
	
	STMCapa[0] = 0x4d;	//M
	STMCapa[1] = 0x43;	//C
	STMCapa[2] = 0x55;	//U
	STMCapa[3] = 0x43;	//C
	STMCapa[4] = 0x61;	//a
	STMCapa[5] = 0x70;	//p
	STMCapa[6] = 0x3a;	//:
	
	if((capa / 1000) != 0)	STMCapa[7] = capa / 1000 + 48;	//千位不为0时显示
	else	STMCapa[7] = 0x20;
	
	STMCapa[8] = capa % 1000 / 100 + 48;	//百位
	STMCapa[9] = capa % 100 /10 + 48;		//十位
	STMCapa[10] = capa % 10 + 48;			//个位
	STMCapa[11] = 0x4b;	//K
	STMCapa[12] = 0x62;	//b
}
/**********************************************************
* 函数功能 ---> 读取CPUID
* 入口参数 ---> none
* 返回数值 ---> CPUID（十六进制）
* 功能说明 ---> none
**********************************************************/
void STM32_CPUID(uint8_t *IDbuff)
{
	uint32_t CPUID;
	CPUID = *((uint32_t*)0xe000ed00);
	sprintf((char*)IDbuff, "CPU ID:%08X", CPUID);
}
/**********************************************************
* 函数功能 ---> 设置向量表偏移地址
* 入口参数 ---> NVIC_VectTab：基址
*              Offset：偏移量	
* 返回数值 ---> 容量（十进制）
* 功能说明 ---> none
**********************************************************/	 
void MY_NVIC_SetVectorTable(uint32_t NVIC_VectTab,uint32_t Offset)	 
{ 	   	 
	SCB->VTOR = NVIC_VectTab | (Offset & (uint32_t)0x1fffff80);//设置NVIC的向量表偏移寄存器
	//用于标识向量表是在CODE区还是在RAM区
}

void MY_NVIC_PriorityGroup_Config(uint32_t NVIC_PriorityGroup)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup);	//设置中断分组

}
/**********************************************************
* 函数功能 ---> 设置中断分组优先级
* 入口参数 ---> NVIC_PreemptionPriority：抢先优先级
*               NVIC_Subpriority：响应优先级
*               NVIC_Channel：中断编号
*               NVIC_Group: 中断分组
* 返回数值 ---> none
* 功能说明 ---> 1、组划分:
*                  组0：0位抢占优先级，4位响应优先级
*                  组1：1位抢占优先级，3位响应优先级
*                  组2：2位抢占优先级，2位响应优先级
*                  组3：3位抢占优先级，1位响应优先级
*                  组4：4位抢占优先级，0位响应优先级
*               2、抢先优先级参数和响应优先级参数原则上是数值越小优先级越高
**********************************************************/
void MY_NVIC_Init(uint8_t NVIC_PreemptionPriority,uint8_t NVIC_Subpriority,uint8_t NVIC_Channel,uint32_t NVIC_Group)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	MY_NVIC_PriorityGroup_Config(NVIC_Group);	//设置中断分组	
	
	NVIC_InitStructure.NVIC_IRQChannel = NVIC_Channel;								//设置中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_PreemptionPriority;	//抢先优先级  	 	 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_Subpriority;				//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//使能中断

	NVIC_Init(&NVIC_InitStructure);	//初始化中断
	
}
/**********************************************************
* 函数功能 ---> THUMB指令不支持汇编内联
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> 采用如下方法实现执行汇编指令WFI
**********************************************************/
__asm void WFI_SET(void)
{
	WFI;    
}
/**********************************************************
* 函数功能 ---> 所有时钟寄存器复位
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> 不能在这里执行所有外设复位!否则至少引起串口不工作
**********************************************************/
void MY_RCC_DeInit(void)
{										  					   
	RCC->APB1RSTR = 0x00000000;	//复位结束			 
	RCC->APB2RSTR = 0x00000000; 
	  
  	RCC->AHBENR   = 0x00000014;	//睡眠模式闪存和SRAM时钟使能.其他关闭.	  
  	RCC->APB2ENR  = 0x00000000;	//外设时钟关闭.			   
  	RCC->APB1ENR  = 0x00000000;   
	RCC->CR      |= 0x00000001;	//使能内部高速时钟HSION	 															 
	RCC->CFGR    &= 0xf8ff0000;	//复位SW[1:0],HPRE[3:0],PPRE1[2:0],PPRE2[2:0],ADCPRE[1:0],MCO[2:0]					 
	RCC->CR      &= 0xfef6ffff;	//复位HSEON,CSSON,PLLON
	RCC->CR      &= 0xfffbffff;	//复位HSEBYP	   	  
	RCC->CFGR    &= 0xff80ffff;	//复位PLLSRC, PLLXTPRE, PLLMUL[3:0] and USBPRE 
	RCC->CIR      = 0x00000000;	//关闭所有中断
	
	//配置向量表				  
#ifdef  VECT_TAB_RAM
	MY_NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else   
	MY_NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif
}
/**********************************************************
* 函数功能 ---> 设置芯片进入待机模式
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> 利用WKUP引脚唤醒（PA.0）
**********************************************************/
void SYS_Standby(void)
{
	SCB->SCR |= 1 << 2;			//使能sleep deep（SYS->CTRL）
	RCC->APB1ENR |= 1 << 28;	//电源接口时钟开启
	PWR->CSR |= 1 << 8;			//设置WKUP引脚用于唤醒
	PWR->CR |= 1 << 2;			//清除WAKE-UP标志
	PWR->CR |= 1 << 1;			//PDDS置位，掉电深睡眠
	WFI_SET();					//执行WFI指令
}
/**********************************************************
* 函数功能 ---> 系统软复位
* 入口参数 ---> none
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void SYS_SoftReset(void)
{
	SCB->AIRCR = 0x05fa0000 | 0x00000004;
}
/**********************************************************
* 函数功能 ---> JTAG模式设置
* 入口参数 ---> mode：模式参数
*                    000：JTAG-DP + SW-DP（复位状态）
*                    001：JTAG-DP + SW-DP（除了JNTRST引脚，释放JRST引脚）
*                    010：JTAG-DP接口禁止，SW-DP接口允许
*                    100：JTAG-DP接口和SW-DP接口都禁止
*                    xxx：其他值，禁止
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void STM_JTAG_Set(uint32_t mode)
{
	RCC->APB2ENR |= 1 << 0;		//使能辅助时钟
	AFIO->MAPR &= 0xf8ffffff;	//清除SWJ_CFG[2:0]位，即【26:24】
	AFIO->MAPR |= (mode << 24);	//设置JTAG模式
}
/**********************************************************
* 函数功能 ---> 系统时钟初始化
* 入口参数 ---> pll：倍频数。取值范围：2 ~ 16
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/
void STM_Clock_Init(uint8_t pll)
{
	uint8_t tmp = 0;
	
	MY_RCC_DeInit();	//复位并配置向量表，并且将外部中断和外设时钟全部关闭
	
	RCC->CR |= 0x00010000;  //外部高速时钟使能HSEON
	
	while(!(RCC->CR >> 17));//等待外部时钟就绪
	
	RCC->CFGR = 0X00000400; //APB1=DIV2;APB2=DIV1;AHB=DIV1;
	pll -= 2;//抵消2个单位
	RCC->CFGR |= pll << 18;   //设置PLL值 2~16
	RCC->CFGR |= 1 << 16;	  //PLLSRC ON 
	FLASH->ACR |= 0x32;	  //FLASH 2个延时周期

	RCC->CR |= 0x01000000;  //PLLON
	
	while(!(RCC->CR >> 25));//等待PLL锁定
	
	RCC->CFGR |= 0x00000002;//PLL作为系统时钟，最后才能开启PLL，因为设定PLL时，PLL相关位必须在关闭状态下进行
	
	while(tmp != 0x02)     //等待PLL作为系统时钟设置成功
	{   
		tmp  = RCC->CFGR >> 2;
		tmp &= 0x03;
	}    
}

