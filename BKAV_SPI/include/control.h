
#ifndef CONTROL_H__
#define CONTROL_H__
#include "LC898201Typedef.h"
/*-----------------------------------------------------------------------------*/
/* Header inclusions
*/
/*-----------------------------------------------------------------------------*/
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/types.h>
#include <linux/errno.h>
#include "gpio.h"
#include "var.h"
#include "LC898201Typedef.h"
#include "CamDef.h"
//#include"CamFil.h"
#include "bkavspi.h"
#include "Cam.h"
#include "CamIni.h"

#if(ENABLE_VSYNC==1)
//VOID EnableMoveZoomFocus();
//VOID EnableMoveIris();
#endif
VOID  *MonitorPositionFocus(void);
VOID InitGPIOtoLC898201(void);
VOID LC898201ResetIC(void);
VOID SetupMiddlePositionFocus(void);
VOID SetupMiddlePositionZoom(void);

VOID SetZoomMaxWide(void);
VOID SetFocusMaxNear(void);

VOID SelectModeLens(BYTE choseLens);

#endif
