#include "CH58x_common.h"
#include "CH58xBLE_LIB.h"
#include "HAL.h"
#include "broadcaster.h"
#include "config.h"

__attribute__((aligned(4))) u32 MEM_BUF[BLE_MEMHEAP_SIZE / 4];

__HIGH_CODE
void Main_Circulation()
{
  while (1)
  {
    TMOS_SystemProcess();
  }
}

void DebugInit(void)
{
  GPIOB_SetBits(GPIO_Pin_19);
  GPIOB_ModeCfg(GPIO_Pin_19, GPIO_ModeIN_PU);
  GPIOB_ModeCfg(GPIO_Pin_19, GPIO_ModeOut_PP_5mA);
  UART1_DefInit();
}

int main()
{
#if (defined(DCDC_ENABLE)) && (DCDC_ENABLE == TRUE)
  PWR_DCDCCfg(ENABLE);
#endif
  SetSysClock(CLK_SOURCE_PLL_60MHz);
  DebugInit();
  printf("%s\n", VER_LIB);
  CH58X_BLEInit();
  HAL_Init();
  GAPRole_BroadcasterInit();
  Broadcaster_Init();
  Main_Circulation();
  while (1)
    ;
}
