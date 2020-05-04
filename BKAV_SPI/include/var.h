#ifndef VAR_H_
#define VAR_H_
#include "LC898201Typedef.h"

#define    PRE_POSITION 1
#define	   DEBUG		1 // bo log .
#define    ENABLE_VSYNC 0
#define    FOCUSTRUE    1
#define    FOCUSFALSE   0

#define    ZOOMTRUE     1
#define    ZOOMFALSE    0


#define    MIDDLE	0
#define    FAR		1
#define    NEAR		-1
#define	   WIDE 	-1
#define    TELE		1


#define SPEED_DEFAULT_S611 100
#define SPEED_DEFAULT_S211 100
#define SPEED_DEFAULT_S212 100
#define SPEED_DEFAULT_S213 100

// Dung de cau hinh doc Pi sensor. Khi nao co IC logic PI_SENSOR_HIGH 0x00, Khi khong co IC logic PI_SENSOR_HIGH 0x10

/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/

static INT     g_iResolutionFocus,g_iResolutionZoom;
static INT      g_iDistanceFocus, g_iDistanceZoom;

static BYTE   g_divFocusMotor, g_divZoomMotor;
static BYTE  g_iSelectLens;
static BYTE   g_bDirFocusNear ; 
static BYTE   g_bDirFocusFar ;  
static  BYTE   g_bDirZoomWide ;
static BYTE  g_bDirZoomTele ; 

static INT g_iThresholdFocusNear;
static INT g_iThresholdFocusFar;
static INT g_iThresholdZoomWide;
static INT g_iThresholdZoomTele;

static UINT g_iZoomSpeedMax, g_iFocusSpeedMax;

static ULONG g_ulspeedZoom=0x00ff,g_ulsizemoveZoom=0, g_uldirZoom=1;
static ULONG g_ulspeedFocus=0x00ff,g_ulsizemoveFocus=0, g_uldirFocus=1;

static CHAR g_flMiddle, g_statusFocus,g_statusZoom;

static BYTE g_flStatusFocus=0,g_flStatusZoom=0;
static BYTE g_flThresholdFocus=0;
static BYTE g_flThresholdZoom=0;
static LONG  iPositionStopFocus=0;
static LONG  iPositionStopZoom=0;
#endif
