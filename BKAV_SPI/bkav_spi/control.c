/********************************************************************************
<Copyright Notice>
Product: LC898201
Module: main.c
Version: 1.0.0.0
Author:	ThangLMb
Modified:
	<Name>
	<Date>
	<Change>
Released: 15/07/2019
Description: app control Lens for lc898201
Note: <Note>
********************************************************************************
/*-----------------------------------------------------------------------------*/
/* Header inclusions
*/
/*-----------------------------------------------------------------------------*/
#include <linux/delay.h>
#include "control.h"
#include "CamModel.h"
#include "ControlLens.h"

#define     PI_SENSOR_HIGH 0x10
#define     PI_SENSOR_LOW  0x00
int flagcheck=0;
int g_flPreIndex=0;

//unsigned long lctr1=0x00 ,lctr2=0x00, lctr3=0x00 , lctr=0x00 ,lctr5=0x00;



int GetLibLC898201Infomation(char* pVersion, char* pVersionSize, char* pInfomation, char* pInfoSize)
{
	pVersion = "None";
	pVersionSize= "2.0.0.3";
	pInfomation ="Control Lens CAMIP PP2 by ThangLMb";
	pInfoSize ="None";
	return 1;
}


/*-------------------------------------------------------------------------
* Function: unsigned int GetFocusIndex()
* Purpose: get index of Focus
* Parameter: None
* Return: index of focus from 0 to 1000
* Comment:
---------------------------------------------------------------------------*/
int GetFocusIndex(void)
{
	int getvalue;
	if(g_iSelectLens==CAMIP_S211)
	{
	unsigned long lctr1=0x00 ,lctr2=0x00, lctr3=0x00 , lctr=0x00 ,lctr5=0x00;
	unsigned char lctr4,lctr8,lctr6;
	RegReadA(B_LCTR1,&lctr4);
	RegReadA(B_LCTR2,&lctr8);
	RegReadA(B_LCTR3,&lctr6);
	lctr3 = lctr6;
	lctr2 = lctr8;
	lctr5 =lctr3 << 8;
	lctr = lctr5 |= lctr2;
	getvalue = lctr;
	}
	else getvalue=iPositionStopFocus;
	return getvalue;
}
/*-------------------------------------------------------------------------
* Function: unsigned int GetZoomIndex()
* Purpose: get index of Zoom
* Parameter: None
* Return: index of zoom from 0 to 1000
* Comment:
---------------------------------------------------------------------------*/
int GetZoomIndex(void)
{
	int getvalue;
	unsigned long lctr1=0x00 ,lctr2=0x00, lctr3=0x00 , lctr=0x00 ,lctr5=0x00;
	unsigned char lctr4,lctr8,lctr6;
	RegReadA(A_LCTR1,&lctr4);
	RegReadA(A_LCTR2,&lctr8);
	RegReadA(A_LCTR3,&lctr6);
	lctr3 = lctr6;
	lctr2 = lctr8;
	lctr5 =lctr3 << 8;
	lctr = lctr5 |= lctr2;
	 printk("thanh ghi lctr2: %d\n",lctr2);
	// printf("thanh ghi lctr3: %d\n",lctr3);
	//printf("thanh ghi zoom: %d\n",1285 - lctr);
	getvalue = 1285 - lctr ;
	return getvalue;
}
/*-------------------------------------------------------------------------
* Function: void MonitorPositionFocus()
* Purpose: Monitor position of Focus
* Parameter: None
* Return: None
* Comment:
---------------------------------------------------------------------------*/
void *MonitorPositionFocus(void)
{
	unsigned long lctr1=0x00 ,lctr2=0x00, lctr3=0x00 , lctr=0x00 ,lctr5=0x00;
	unsigned char cValueRegisterFocus;
	unsigned char lctr4,lctr8,lctr6;
	while(1)
	{
		if(g_flStatusFocus == 1)
		{
			flagcheck = 1;
			unsigned char cValueRegisterFocus;
			RegReadA( B_TRIG , &cValueRegisterFocus );
			if(cValueRegisterFocus == 0x01)// Focus is running
			{
				unsigned char cValueRegisterFocus;
				RegReadA( B_TRIG , &cValueRegisterFocus );
				g_flStatusFocus =1;
				#if ( DEBUG ==1)
					printk("\nFocus is running\n");
				#endif
				//log position
			}
			else if(cValueRegisterFocus == 0x00 && g_flStatusFocus == 1)// Focus is stopping
			{
				g_flStatusFocus = 0;
				#if ( DEBUG == 1)
				printk("Focus move finished\n");
				printk("Positon focus: %d\n", iPositionStopFocus);
				#endif
			}
		}
	msleep(1000);
	}
}


/*-------------------------------------------------------------------------
* Function: int MoveFocus(unsigned short speed,unsigned short iSizeStep)
* Purpose: Move Focus step motor to control focus
* Parameter: 	unsigned short speed: 0-100
				unsigned short iSizeStep: 0- 1000
* Return:  1 or 0
* Comment: Return 1 if OK
			Return 0 if Error
---------------------------------------------------------------------------*/
int MoveFocus(unsigned long speed, unsigned long iSizeStep)
{
	#if(PRE_POSITION == 1)
		if(g_flPreIndex == 0)
		{
			SetupMiddlePositionFocus();
			SetupMiddlePositionZoom();
			SetZoomMaxWide();
			SetFocusMaxNear();
			msleep(4);
			RegWriteA( B_LCTR1	, 0x00 );  //  LCTR = 0
			RegWriteA( B_LCTR2	, 0x00 );  //
			RegWriteA( B_LCTR3	, 0x00 );

			if(g_iSelectLens == CAMIP_S211)
			{
				RegWriteA( A_LCTR1	, 0x00 );  //  LCTR = 0
				RegWriteA( A_LCTR2	, 5 );  //
				RegWriteA( A_LCTR3	, 5 );
			}
			g_flPreIndex = 1;
		}
	#endif

		if(g_iSelectLens == CAMIP_S211)
		{
			RegWriteA( B_TRIG	, 0x00) ;
			RegWriteA( B_TRIG	, 0x00) ;
			RegWriteA( B_TRIG	, 0x00) ;

			unsigned long lctr1=0x00 ,lctr2=0x00, lctr3=0x00 , lctr=0x00 ,lctr5=0x00;
			unsigned char lctr4=0,lctr8=0,lctr6=0;

			RegReadA(B_LCTR1,&lctr4);
			RegReadA(B_LCTR2,&lctr8);
			RegReadA(B_LCTR3,&lctr6);

        	lctr3= lctr6;
			lctr2= lctr8;
			lctr5 =lctr3 << 8;
			lctr = lctr5 |= lctr2;
			iPositionStopFocus=lctr;
		}
		#if(ENABLE_VSYNC==1)
		//EnableMoveZoomFocus();
		#endif
		int checkMoveFocus;
		if((iSizeStep>=0 && iSizeStep <=g_iResolutionFocus) && ((unsigned long)(iSizeStep-iPositionStopFocus)!=0))
		{
			checkMoveFocus=1;
			if(iSizeStep <iPositionStopFocus)
			{
				g_ulsizemoveFocus= (unsigned long)((iPositionStopFocus- iSizeStep)*g_divFocusMotor*g_iDistanceFocus/g_iResolutionFocus);
				g_uldirFocus=g_bDirFocusNear;
			}
			else
			{
				g_ulsizemoveFocus= (unsigned long)((iSizeStep - iPositionStopFocus)*g_divFocusMotor*g_iDistanceFocus/g_iResolutionFocus);
				g_uldirFocus=g_bDirFocusFar;
			}
			g_ulspeedFocus=(unsigned long)((102-speed)*g_iFocusSpeedMax/2);//0x08ff
			//g_ulspeedFocus=800;
			StmDrb( g_uldirFocus, g_ulspeedFocus, g_ulsizemoveFocus);
			/*while(cValueRegisterFocus==0x01)
				{
				 RegReadA( B_TRIG , &cValueRegisterFocus );
			 	} */
			iPositionStopFocus=iSizeStep;
			g_flStatusFocus=1;

		}
		else
			checkMoveFocus =0;
		#if(ENABLE_VSYNC==1)
		//EnableMoveZoomFocus();
		#endif
		return checkMoveFocus;
}
/*-------------------------------------------------------------------------
* Function: void ControlIRIS(unsigned float fSize)
* Purpose: Control IRIS for aperture
* Parameter: 	 float fSize
* Return: None
* Comment:
---------------------------------------------------------------------------*/

int MoveIris( int fSize)
{
#if(ENABLE_VSYNC==1)
		EnableMoveIris();
	#endif
	UnFltVal	UlStmTgt	= {0x00000000};				// Target Position
	UnFltVal	UlTgtInc	= {0x00000000};				// Target Increase
	UnFltVal	UlSmvSiz	= {0x00000000};				// StepMove Size
	if(fSize==IRIS_1_0 )
	{

		UlStmTgt.UlLngVal = 0xBF001005;
		 UlTgtInc.UlLngVal = 0x3D400000;
		 UlSmvSiz.UlLngVal = 0x3B100120;
		 VcmDrg( UlStmTgt , UlTgtInc, UlSmvSiz , 0x00 );
		//VcmDrg(0xBF001005 , 0x00000000, 0x3B100120,0x00 );//To Center: 0x3B100120
	}
	else if(fSize==IRIS_1_4 )
	{
		 UlStmTgt.UlLngVal = 0xBF001005;
		 UlTgtInc.UlLngVal = 0x3D400000;
		 UlSmvSiz.UlLngVal = 0x3B100120;
		 VcmDrg( UlStmTgt , UlTgtInc, UlSmvSiz , 0x00 );
		//VcmDrg(0xBF001005 , 0x00000000, 0x3B100120,0x00 );//To Center: 0x3B100120
	}
	else if(fSize==IRIS_2_8 )
	{
		 UlStmTgt.UlLngVal = 0xBF001000;
		 UlTgtInc.UlLngVal = 0x3D400000;
		 UlSmvSiz.UlLngVal = 0x3B100120;
		VcmDrg( UlStmTgt , UlTgtInc, UlSmvSiz , 0x00 );
	//	VcmDrg(0xBF001000 , 0x00000000, 0x3B100120,0x00 );//To Center: 0x3B100120

	}
	else if(fSize==IRIS_4_0 )
	{
		 UlStmTgt.UlLngVal = 0x00000000;
		 UlTgtInc.UlLngVal = 0x3D400000;
		 UlSmvSiz.UlLngVal = 0x3B100120;
		 VcmDrg( UlStmTgt , UlTgtInc, UlSmvSiz , 0x00 );
	//	VcmDrg(0x00000000 , 0x00000000, 0x3B100120,0x00 );//To Center: 0x3B100120
	}
	else if(fSize==IRIS_5_4 )
	{
		 UlStmTgt.UlLngVal = 0x3F000100;
		 UlTgtInc.UlLngVal = 0x3D400000;
		 UlSmvSiz.UlLngVal = 0x3B100120;
		 VcmDrg( UlStmTgt , UlTgtInc, UlSmvSiz , 0x00 );
	//	VcmDrg(0x3F000100 , 0x00000000, 0x3B100120,0x00 );//To Center: 0x3B100120

	}
	else if(fSize==IRIS_8_0 )
	{
		UlStmTgt.UlLngVal = 0x3F000100;
		 UlTgtInc.UlLngVal = 0x3D400000;
		 UlSmvSiz.UlLngVal = 0x3B100120;
		 VcmDrg( UlStmTgt , UlTgtInc, UlSmvSiz , 0x00 );
	//	VcmDrg(0x3F000100 , 0x00000000, 0x3B100120,0x00 );//To Center: 0x3B100120
	}
	else if(fSize==IRIS_11_0 )
	{
		UlStmTgt.UlLngVal = 0xBF001005;
		UlTgtInc.UlLngVal = 0x3D400000;
		 UlSmvSiz.UlLngVal = 0x3B100120;
		VcmDrg( UlStmTgt , UlTgtInc, UlSmvSiz , 0x00 );
		//VcmDrg(0x3F000100 , 0x00000000, 0x3B100120,0x00 );//To Center: 0x3B100120
	}
	else if(fSize==IRIS_16_0 )
	{
		 UlStmTgt.UlLngVal = 0x3F000100;
		 UlTgtInc.UlLngVal = 0x3D400000;
		 UlSmvSiz.UlLngVal = 0x3B100120;
		 VcmDrg( UlStmTgt , UlTgtInc, UlSmvSiz , 0x00 );
		//VcmDrg(0x3F000100 , 0x00000000, 0x3B100120,0x00 );//To Center: 0x3B100120
	}
	else if(fSize==IRIS_22_0 )
	{
		UlStmTgt.UlLngVal = 0x3F5001A0;
		UlTgtInc.UlLngVal = 0x3D400000;
		 UlSmvSiz.UlLngVal = 0x3B100120;
		 VcmDrg( UlStmTgt , UlTgtInc, UlSmvSiz , 0x00 );
		//VcmDrg(0x3F5001A0 , 0x00000000, 0x3B100120,0x00 );//To Center: 0x3B100120
	}
	#if(ENABLE_VSYNC==1)
		//EnableMoveIris();
	#endif
	return 1;
}
/*-------------------------------------------------------------------------
* Function: int MoveZoom(unsigned short speed,unsigned short iSizeStep)
* Purpose: Move zoom step motor to control Zoom
* Parameter: 	unsigned short speed: 0-100
				unsigned short iSizeStep: 0- 1000 
* Return:  1 or 0
* Comment: Return 1 if OK
			Return 0 if Error
---------------------------------------------------------------------------*/
int MoveZoom(unsigned long speed,unsigned long iSizeStep)
{

	#if(PRE_POSITION == 1)
		if(g_flPreIndex == 0)
		{
			SetupMiddlePositionFocus();
			SetupMiddlePositionZoom();
			SetZoomMaxWide();
			SetFocusMaxNear();
			msleep(4);
			RegWriteA( B_LCTR1	, 0x00 );  //  LCTR = 0 
			RegWriteA( B_LCTR2	, 0x00 );  // 
			RegWriteA( B_LCTR3	, 0x00 );  

			if(g_iSelectLens == CAMIP_S211)
			{
				RegWriteA( A_LCTR1	, 0x00 );  //  LCTR = 0 
				RegWriteA( A_LCTR2	, 5 );  // 
				RegWriteA( A_LCTR3	, 5 ); 
			}
			g_flPreIndex = 1;
		}
	#endif

	int checkMoveZoom;
		checkMoveZoom = 1;
			if((iSizeStep >= 0 && iSizeStep <= g_iResolutionZoom) && ((unsigned long)(iSizeStep-iPositionStopZoom) !=0 ))
			{
				checkMoveZoom = 1;
				if(iSizeStep < iPositionStopZoom)
				{
					g_ulsizemoveZoom = (unsigned long)((iPositionStopZoom - iSizeStep) * g_divZoomMotor * g_iDistanceZoom / g_iResolutionZoom);
					g_uldirZoom = g_bDirZoomWide;
				}
				else 
				{	
					g_ulsizemoveZoom =	(unsigned long)((iSizeStep-iPositionStopZoom) * g_divZoomMotor * g_iDistanceZoom / g_iResolutionZoom);
					g_uldirZoom = g_bDirZoomTele;
				}
			 	g_ulspeedZoom = (unsigned long)((102-speed) * g_iZoomSpeedMax / 2);//0x08ff
				//g_ulspeedZoom=800;//0x08ff
				StmDra( g_uldirZoom, g_ulspeedZoom, g_ulsizemoveZoom);
				// while(cValueRegisterZoom==0x01){		
				//  RegReadA( B_TRIG , &cValueRegisterZoom );
				//  }
				iPositionStopZoom = iSizeStep;
				//g_flStatusZoom = 1;

			}
			else
			checkMoveZoom = 0;

	#if(ENABLE_VSYNC == 1)
		//EnableMoveZoomFocus();
	#endif
	return checkMoveZoom;
}

/*-------------------------------------------------------------------------
* Function: SetupMiddlePositonFocus()
* Purpose: Set up position focus is middle
* Parameter: None
* Return: None
* Comment: If  ( key ='S') => status= 0x10 ( RIGHT) ==> Set up (key=w) g_uldirFocus =1;
		   If   ( key ='W') => status= 0x00 ( LEFT) ==> Setup (key=w) g_uldirFocus =0;
---------------------------------------------------------------------------*/
void SetupMiddlePositionFocus(void)
{
	unsigned char cstatus;
	unsigned char index;
	RegReadA( B_INIT	, &cstatus) ;			// PI Rising Detecition Level
	cstatus &= 0xf0;
	#if( DEBUG == 1)
		printk(" Position Initiation of Focus: %02x\n", cstatus);
	#endif
	if(cstatus == PI_SENSOR_LOW)
	{
		if(g_iSelectLens == CAMIP_S213)
		{
			g_statusFocus = NEAR;
			g_ulspeedFocus = g_iFocusSpeedMax;
			g_ulsizemoveFocus=0xffff;
			g_uldirFocus = g_bDirFocusFar;
			StmDrb( g_uldirFocus, g_ulspeedFocus, g_ulsizemoveFocus);
		}
		else if(g_iSelectLens == CAMIP_S211){
			g_statusFocus = NEAR;
			g_ulspeedFocus = g_iFocusSpeedMax;
			g_ulsizemoveFocus=0xffff;
			g_uldirFocus = g_bDirFocusFar;
			StmDrb( g_uldirFocus, g_ulspeedFocus, g_ulsizemoveFocus);
		}
		else if(g_iSelectLens == CAMIP_S611){
			g_statusFocus = NEAR;
			g_ulspeedFocus = g_iFocusSpeedMax;
			g_ulsizemoveFocus = 0xffff;
			g_uldirFocus = g_bDirFocusFar;
			StmDrb( g_uldirFocus, g_ulspeedFocus, g_ulsizemoveFocus);
		}
		else if(g_iSelectLens == CAMIP_S212)
		{
			g_statusFocus = NEAR;
			g_ulspeedFocus = g_iFocusSpeedMax;
			g_ulsizemoveFocus = 0xffff;
			g_uldirFocus = g_bDirFocusFar;
			StmDrb( g_uldirFocus, g_ulspeedFocus, g_ulsizemoveFocus);
		}
		for(index =0 ; index<10; index++){
			msleep(100);
			RegReadA( B_INIT	, &cstatus) ;
			cstatus &= 0xf0;
			if(cstatus == PI_SENSOR_HIGH)
			{
				RegWriteA( B_TRIG	, 0x00) ;
				RegWriteA( B_TRIG	, 0x00) ;
				RegWriteA( B_TRIG	, 0x00) ;
				g_statusFocus=MIDDLE;
				if(g_statusFocus==MIDDLE)
				{
					#if( DEBUG==1)
						printk("Focus set up middel, Status: MIDDLE %d\n", g_statusFocus);
					#endif
					RegWriteA( B_LCTR1	, 0x00 ) ;  //  LCTR = 0
					RegWriteA( B_LCTR2	, 0x00 ) ;  //
    				RegWriteA( B_LCTR3	, 0x00) ;  //
					iPositionStopFocus=0;
					break;
				}
			}
		}
	}
	if(cstatus == PI_SENSOR_HIGH)
	{
		if(g_iSelectLens == CAMIP_S213){
			g_statusFocus = FAR;
			g_ulspeedFocus=g_iFocusSpeedMax;
			g_ulsizemoveFocus=0xffff;
			g_uldirFocus=g_bDirFocusNear;
			StmDrb( g_uldirFocus, g_ulspeedFocus, g_ulsizemoveFocus);
		}
		else if(g_iSelectLens == CAMIP_S611){
			g_statusFocus = FAR;
			g_ulspeedFocus=g_iFocusSpeedMax;
			g_ulsizemoveFocus=0xffff;
			g_uldirFocus=g_bDirFocusNear;
			StmDrb( g_uldirFocus, g_ulspeedFocus, g_ulsizemoveFocus);
		}
		else if(g_iSelectLens == CAMIP_S211){
			g_statusFocus = FAR;
			g_ulspeedFocus=g_iFocusSpeedMax;
			g_ulsizemoveFocus=0xffff;
			g_uldirFocus=g_bDirFocusNear;
			StmDrb( g_uldirFocus, g_ulspeedFocus, g_ulsizemoveFocus);
		}
		else if(g_iSelectLens == CAMIP_S212){
			g_statusFocus = FAR;
			g_ulspeedFocus=g_iFocusSpeedMax;
			g_ulsizemoveFocus=0xffff;
			g_uldirFocus=g_bDirFocusNear;
			StmDrb( g_uldirFocus, g_ulspeedFocus, g_ulsizemoveFocus);
		}
		for(index =0 ; index<10; index++){
				msleep(100);
			//printk("Focus running LEFT\n");
			RegReadA( B_INIT	, &cstatus) ;
			cstatus &= 0xf0;
			if(cstatus == PI_SENSOR_LOW)
			{
				RegWriteA( B_TRIG	, 0x00) ;
				RegWriteA( B_TRIG	, 0x00) ;
				RegWriteA( B_TRIG	, 0x00) ;
				g_statusFocus=MIDDLE ;
				if(g_statusFocus==MIDDLE)
				{
					#if( DEBUG==1)
					printk("Focus set up middel, Status: MIDDLE %d\n", g_statusFocus);
					#endif
					RegWriteA( B_LCTR1	, 0x00 ) ;  //  LCTR = 0
    				RegWriteA( B_LCTR2	, 0x00 ) ;  //
    				RegWriteA( B_LCTR3	, 0x00) ;  //
					iPositionStopFocus=0;
					break;
				}

			}
		}
	}

}
/*-------------------------------------------------------------------------
* Function: SetupMiddlePositonZoom()
* Purpose: Set up position Zoom is middle
* Parameter: None	 
* Return: None
* Comment: If  ( key ='A') => status= 0x00 ( RIGHT) ==> Set up (key=w) g_uldirZoom=1;
		   If   ( key ='D') => status= 0x10 ( LEFT) ==> Setup (key=w) g_uldirZoom =0;
---------------------------------------------------------------------------*/
void SetupMiddlePositionZoom(void)
{
	unsigned char index;
	unsigned char cstatus;
	RegReadA( A_INIT	, &cstatus) ;			// PI Rising Detecition Level
	cstatus &= 0xf0;
	#if( DEBUG==1)
		printk(" Position Initiation of Zoom: %02x\n", cstatus);
	#endif
	if(cstatus == PI_SENSOR_LOW)
	{
		if(g_iSelectLens == CAMIP_S213)
		{	
			g_statusZoom = TELE;
			g_ulspeedZoom=g_iZoomSpeedMax;
			g_ulsizemoveZoom=0xffff;
			g_uldirZoom=g_bDirZoomWide;
			StmDra( g_uldirZoom, g_ulspeedZoom, g_ulsizemoveZoom);		
		}
		else if(g_iSelectLens == CAMIP_S611){	
			g_statusZoom = WIDE;
			g_ulspeedZoom=g_iZoomSpeedMax;
			g_ulsizemoveZoom=0xffff;
			g_uldirZoom=g_bDirZoomTele;
			StmDra( g_uldirZoom, g_ulspeedZoom, g_ulsizemoveZoom);	
		}
		else if(g_iSelectLens == CAMIP_S211){	
			g_statusZoom = WIDE;
			g_ulspeedZoom=g_iZoomSpeedMax;
			g_ulsizemoveZoom=0xffff;
			g_uldirZoom=g_bDirZoomTele;
			StmDra( g_uldirZoom, g_ulspeedZoom, g_ulsizemoveZoom);
		}
		else if(g_iSelectLens == CAMIP_S212)
		{	
			g_statusZoom = WIDE;
			g_ulspeedZoom=g_iZoomSpeedMax;
			g_ulsizemoveZoom=0xffff;
			g_uldirZoom=g_bDirZoomTele;
			StmDra( g_uldirZoom, g_ulspeedZoom, g_ulsizemoveZoom);	
		}
		for(index =0 ; index<10; index++){
			msleep(100);
			RegReadA( A_INIT	, &cstatus) ;
			cstatus &= 0xf0;
			if(cstatus == PI_SENSOR_HIGH)
			{
				RegWriteA( A_TRIG	, 0x00) ;
				RegWriteA( A_TRIG	, 0x00) ;
				RegWriteA( A_TRIG	, 0x00) ;
				g_statusZoom=MIDDLE;
				if(g_statusZoom==MIDDLE)
				{
					#if(DEBUG==1)
					printk("Zoom set up middel, Status: MIDDLE %d\n", g_statusZoom);
					#endif
					RegWriteA( A_LCTR1	, 0x00 ) ;  //  LCTR = 0 
    				RegWriteA( A_LCTR2	, 0x00 ) ;  // 
    				RegWriteA( A_LCTR3	, 0x00) ;  //	
					iPositionStopZoom=0;
					break;
				}				
			}

		}
	}
	if(cstatus == PI_SENSOR_HIGH)
	{	
		if(g_iSelectLens == CAMIP_S213){	
			g_statusZoom = WIDE;
			g_ulspeedZoom=g_iZoomSpeedMax;
			g_ulsizemoveZoom=0xffff;
			g_uldirZoom=g_bDirZoomTele;
			StmDra( g_uldirZoom, g_ulspeedZoom, g_ulsizemoveZoom);	
		}
		else if(g_iSelectLens == CAMIP_S611){		
			g_statusZoom = TELE;
			g_ulspeedZoom=g_iZoomSpeedMax;
			g_ulsizemoveZoom=0xffff;
			g_uldirZoom=g_bDirZoomWide;
			StmDra( g_uldirZoom, g_ulspeedZoom, g_ulsizemoveZoom);	
		}
		else if(g_iSelectLens == CAMIP_S211){		
			g_statusZoom = TELE;
			g_ulspeedZoom=g_iZoomSpeedMax;
			g_ulsizemoveZoom=0xffff;
			g_uldirZoom=g_bDirZoomWide;
			StmDra( g_uldirZoom, g_ulspeedZoom, g_ulsizemoveZoom);	
		}
		else if(g_iSelectLens == CAMIP_S212){		
			g_statusZoom = TELE;
			g_ulspeedZoom=g_iZoomSpeedMax;
			g_ulsizemoveZoom=0xffff;
			g_uldirZoom=g_bDirZoomWide;
			StmDra( g_uldirZoom, g_ulspeedZoom, g_ulsizemoveZoom);	
		}
		for(index =0 ; index<10; index++){
			msleep(100);
			RegReadA( A_INIT	, &cstatus) ;
			cstatus &= 0xf0;
			if(cstatus == PI_SENSOR_LOW)
			{
				RegWriteA( A_TRIG	, 0x00) ;
				RegWriteA( A_TRIG	, 0x00) ;
				RegWriteA( A_TRIG	, 0x00) ;
				g_statusZoom=MIDDLE ;
				if(g_statusZoom==MIDDLE)
				{
					#if(DEBUG==1)
					printk("Zoom set up middel, Status: MIDDLE %d\n", g_statusZoom);
					#endif
					RegWriteA( A_LCTR1	, 0x00 ) ;  //  LCTR = 0 
    				RegWriteA( A_LCTR2	, 0x00 ) ;  // 
    				RegWriteA( A_LCTR3	, 0x00) ;  //	
					iPositionStopZoom=0;
					//break;
				}											
			}		
		}
	}

}
/*-------------------------------------------------------------------------
* Function: SetZoomMaxWide()
* Purpose: Set zoon at max wide position
* Parameter: None	 
* Return: None
* Comment:
---------------------------------------------------------------------------*/
void SetZoomMaxWide(void)
{
    g_ulspeedZoom = g_iZoomSpeedMax;
	g_ulsizemoveZoom = (g_iThresholdZoomWide)*g_divZoomMotor; // 454 step
	g_uldirZoom = g_bDirZoomWide;
	StmDra( g_uldirZoom, g_ulspeedZoom, g_ulsizemoveZoom); 
	iPositionStopZoom=0;
	RegWriteA( A_LCTR1	, 0x00 ) ;  //  LCTR = 0 
	RegWriteA( A_LCTR2	, 0x00 ) ;  // 
	RegWriteA( A_LCTR3	, 0x00) ;  //
	#if (DEBUG == 1)
		printk("Zoom arrived max wide positon\n");
		printk("Postion Zoom:%d\n", iPositionStopZoom);
	#endif
}
/*-------------------------------------------------------------------------
* Function: SetFocusMaxNear()
* Purpose: Set Focus at max Near position
* Parameter: None
* Return: None
* Comment:
---------------------------------------------------------------------------*/
void SetFocusMaxNear(void)
{

	g_ulspeedFocus = g_iZoomSpeedMax;
	g_ulsizemoveFocus = g_iThresholdFocusNear*g_divFocusMotor; // 454 step
	g_uldirFocus = g_bDirFocusNear;
	StmDrb( g_uldirFocus, g_ulspeedFocus, g_ulsizemoveFocus);
	iPositionStopFocus = 0;
	RegWriteA( B_LCTR1	, 0x00 ) ;  //  LCTR = 0
	RegWriteA( B_LCTR2	, 0x00 ) ;  //
	RegWriteA( B_LCTR3	, 0x00) ;  //
	#if (DEBUG == 1)
		printk("Focus arrived max Near positon\n");
		printk("Postion Focus:%d\n", iPositionStopFocus);
	#endif

}


/*-------------------------------------------------------------------------
* Function: void SelectModeLens(unsigned char choseLens)
* Purpose: Select parameter lens
* Parameter: unsigned char choseLens
* Return: None
* Comment:
---------------------------------------------------------------------------*/
void SelectModeLens(unsigned char choseLens)
{
	switch (choseLens)
	{
		
		case CAMIP_S211:
		{

			g_iSelectLens = CAMIP_S211;

			g_divFocusMotor =16; 
			g_divZoomMotor =16;

			g_iZoomSpeedMax = 800;
			g_iFocusSpeedMax = 800;

			g_bDirFocusNear = 1;
			g_bDirFocusFar =  0;
			g_bDirZoomWide = 0;
			g_bDirZoomTele = 1;

			g_iThresholdFocusNear = 1017;
			g_iThresholdFocusFar = 645;
			g_iThresholdZoomWide = 912;
			g_iThresholdZoomTele = 373;

			g_iDistanceFocus = 1017+645;
			g_iResolutionFocus= 1017+645;
			//g_iResolutionFocus= 1000;
			g_iDistanceZoom = 912+373;
			//g_iResolutionZoom = 1000;
			g_iResolutionZoom = 912 + 373;
			
			RegWriteA( A_PC1	, 0xC0 ) ;	//	82	// Pulse Generator ON, PWM  Offstate is Brake
			RegWriteA( B_PC1	, 0xC0 ) ;			// Pulse Generator ON, PWM  Offstate is Brake
			RegWriteA( A_PC2	, 0x10 ) ; // Zoom: 0.02mm/step in 2-2 phase
			RegWriteA( B_PC2	, 0x10 ) ;	// Focus: 0.01mm/step in 2-2 phase 

			//RamWrite32A( 0x102D	, 0x3F800000);//0xBF800000
			RamWrite32A( 0x102D	, 0xBF800000);//0xBF800000
			RegWriteA(0x004C,0x01);


			#if(DEBUG==1)
				printk("---> THIS MODE IS CAMIP S211<----\n");
			#endif
			break;
		}
		case CAMIP_S212:
		{

			g_iSelectLens= CAMIP_S212;

			g_divFocusMotor =8; 
			g_divZoomMotor =16; 

			g_iZoomSpeedMax = 800;
			g_iFocusSpeedMax = 400;

			g_bDirFocusNear = 1;
			g_bDirFocusFar  =  0; 
			g_bDirZoomWide  = 1;
			g_bDirZoomTele  = 0;

			g_iThresholdFocusNear = 164; // 0xfff - position
			g_iThresholdFocusFar = 150; // step position
			g_iThresholdZoomWide = 454; // 0xfff - position
			g_iThresholdZoomTele = 176; // step position 

			g_iDistanceFocus = 164+150;
			g_iResolutionFocus= 164+150;
			g_iDistanceZoom = 454+176;
			g_iResolutionZoom = 1000;

			RegWriteA( A_PC1	, 0xD2 ) ;	//	82	// Pulse Generator ON, PWM  Offstate is Brake
			RegWriteA( B_PC1	, 0xB2 ) ;			// Pulse Generator ON, PWM  Offstate is Brake
			RegWriteA( A_PC2	, 0x1B ) ; // 0x1B ) ;		// Zoom: 0.02mm/step in 2-2 phase
			RegWriteA( B_PC2	, 0x1C) ;		// Focus: 0.01mm/step in 1-2 phase

			RamWrite32A( 0x102D	, 0xBF800000);//0xBF800000
			RegWriteA(0x004C,0x01);

			#if(DEBUG==1)
				printk("---> THIS MODE IS CAMIP S212<----\n");
			#endif
			break;
		}
		case CAMIP_S213: 
		{
			g_iSelectLens= CAMIP_S213;

			g_divFocusMotor =8; 
			g_divZoomMotor =8;

			g_iZoomSpeedMax = 400;
			g_iFocusSpeedMax = 400;

			g_bDirFocusNear = 0;
			g_bDirFocusFar  = 1;
			g_bDirZoomWide  = 1;
			g_bDirZoomTele  = 0;

			g_iThresholdFocusNear = 530;
			g_iThresholdFocusFar = 530;
			g_iThresholdZoomWide = 302-50;// fix zoom bi cuong den 302 -116 = 186
			g_iThresholdZoomTele = 281; // 186+281= 467
			
			g_iDistanceFocus = 530+530;
			g_iResolutionFocus= 530+530;
			g_iDistanceZoom = 302+281-50;
			g_iResolutionZoom = 1000;

			RegWriteA( A_PC1	, 0xB2 ) ;	//	steop 1-2 phase	// Pulse Generator ON, PWM  Offstate is Brake
			RegWriteA( B_PC1	, 0xB2 ) ;			// Pulse Generator ON, PWM  Offstate is Brake
			RegWriteA( A_PC2	, 0x1C ) ; 	// Zoom: 0.01mm/step in 1-2 phase
			RegWriteA( B_PC2	, 0x1C) ;	// Focus: 0.01mm/step in 1-2 phase

			RamWrite32A( 0x102D	, 0xBF800000);//0xBF800000
			RegWriteA(0x004C,0x01);

			#if(DEBUG==1)
				printk("---> THIS MODE IS CAMIP S213<----\n");
			#endif
			break;
		}
		case CAMIP_S611:
		{

			g_iSelectLens = CAMIP_S211;
			g_divFocusMotor =16; 
			g_divZoomMotor =16;

			g_iZoomSpeedMax = 800;
			g_iFocusSpeedMax = 800;

			g_bDirFocusNear = 1;
			g_bDirFocusFar =  0;
			g_bDirZoomWide = 0;
			g_bDirZoomTele = 1;

			g_iThresholdFocusNear = 1017;
			g_iThresholdFocusFar = 645;
			g_iThresholdZoomWide = 912;
			g_iThresholdZoomTele = 373;

			g_iDistanceFocus = 1017+645;
			g_iResolutionFocus= 1017+645;
			g_iDistanceZoom = 912+373;
			g_iResolutionZoom = 1000;

			RegWriteA( A_PC1	, 0xC0 ) ;	//	82	// Pulse Generator ON, PWM  Offstate is Brake
			RegWriteA( B_PC1	, 0xC0 ) ;			// Pulse Generator ON, PWM  Offstate is Brake
			RegWriteA( A_PC2	, 0x10 ) ; // Zoom: 0.02mm/step in 2-2 phase
			RegWriteA( B_PC2	, 0x10 ) ;	// Focus: 0.01mm/step in 2-2 phase 

			//RamWrite32A( 0x102D	, 0x3F800000);//0xBF800000
			RamWrite32A( 0x102D	, 0xBF800000);//0xBF800000
			RegWriteA(0x004C,0x01);

			#if(DEBUG==1)
				printk("---> THIS MODE IS CAMIP S611<----\n");
			#endif
			break;
		}
	default:
		break;
	}
}

/*-------------------------------------------------------------------------
* Function: VOID InitGPIOtoLC898201()
* Purpose:
* Paramenter:
* Return: None
* Comment:
---------------------------------------------------------------------------*/
void InitGPIOtoLC898201(void)
{
	// gpio for pin reset
	GPIOExport( RESET_PIN, "reset");
	GPIODirection(RESET_PIN, GPIO_OUT );
	// gpio for pin IR Cut control
	GPIOExport( IRCUT_PIN,"IR_cut_control");
	GPIODirection(IRCUT_PIN, GPIO_OUT );
	// gpio for pin IR Cut Enable
	GPIOExport( ENABLE_IRCUT_PIN,"IR_cut_enable");
	GPIODirection(ENABLE_IRCUT_PIN, GPIO_OUT );
	#if(ENABLE_VSYNC==1)
	// vsync1 for Zoom, Focus
	GPIOExport( 94, "Zoom");
	GPIODirection(94, GPIO_OUT );
	// vsync2 for Iris
	GPIOExport( 95,"Vsync2_Iris");
	GPIODirection(95, GPIO_OUT );
	#endif
}
/*-------------------------------------------------------------------------
* Function: void ControlGpioQCS(int pin , int dir, int value)
* Purpose: Set up gpio QCS
* Parameter: None
* Return: None
* Comment:
---------------------------------------------------------------------------*/
void LC898201ResetIC(void)
{
	GPIOWrite( RESET_PIN,GPIO_LOW);
	msleep(500000);
	GPIOWrite( RESET_PIN,GPIO_HIGH);

}
/*-------------------------------------------------------------------------
* Function: INT OpenIRCut(VOID )
* Purpose: open ir cut
* Parameter: 	none
* Return: 1  or 0
* Comment: 	Return 1 if OK
			Return 0 if Error
---------------------------------------------------------------------------*/
int OpenIRCut(void )
{
	GPIOWrite(ENABLE_IRCUT_PIN, GPIO_HIGH);	//enable ir cut
	GPIOWrite(IRCUT_PIN, GPIO_HIGH);
	msleep(500);
	GPIOWrite(ENABLE_IRCUT_PIN, GPIO_LOW);	//enable ir cut
	return 1;
}
/*-------------------------------------------------------------------------
* Function: INT CloseIRCut(VOID )
* Purpose: open ir cut
* Paramenter: 	none
* Return: 1  or 0
* Comment: 	Return 1 if OK
			Return 0 if Error
---------------------------------------------------------------------------*/
int CloseIRCut(void)
{
	GPIOWrite(ENABLE_IRCUT_PIN, GPIO_HIGH);	//enable ir cut
	GPIOWrite(IRCUT_PIN, GPIO_LOW);
	msleep(500);
	GPIOWrite(ENABLE_IRCUT_PIN, GPIO_LOW);	//enable ir cut
	return 1;
}
/*-------------------------------------------------------------------------
* Function: int InitDriverLens(unsigned char choseLens)
* Purpose: Int driver Lens
* Parameter: unsigned char choseLens:								
* Return: None
* Comment:
---------------------------------------------------------------------------*/

int InitDriverLens(unsigned char nModel)
{
	unsigned char value_read=0;
    RegWriteA(SPIMD3,MODE3);
    RegReadA(SPIMD3, &value_read);
    printk("SPIMD3 = %d", value_read);
   // InitGPIOtoLC898201();
	IniSet();
	SelectModeLens(nModel);
	//VcmDrg(IRISCLOSE , 0x00000000, 0x3B100120,0x00 );//To Center: 0x3B100120
	msleep(20);
#if(PRE_POSITION == 0)
		if(g_flPreIndex == 0)
		{
			SetupMiddlePositionFocus();
			SetupMiddlePositionZoom();
			SetZoomMaxWide();
			SetFocusMaxNear();
			msleep(40);
			RegWriteA( B_LCTR1	, 0x00 );  //  LCTR = 0
			RegWriteA( B_LCTR2	, 0x00 );  //
			RegWriteA( B_LCTR3	, 0x00 );

			if(g_iSelectLens == CAMIP_S211)
			{
				RegWriteA( A_LCTR1	, 0x00 );  //  LCTR = 0
				RegWriteA( A_LCTR2	, 5 );  //
				RegWriteA( A_LCTR3	, 5 );
			}
			g_flPreIndex = 1;
		}
	#endif
	if(g_iSelectLens==CAMIP_S211)
	{
		msleep(2000);
		RegWriteA( B_LCTR1 , 0x00 ); // LCTR = 0
		RegWriteA( B_LCTR2 , 0x00 ); //
		RegWriteA( B_LCTR3 , 0x00 );
		msleep(2000);
		RegWriteA( A_LCTR1 , 0x00 ); // LCTR = 0
		RegWriteA( A_LCTR2 , 5 ); //
		RegWriteA( A_LCTR3 , 5 );
	}
	#if( DEBUG==1)
		printk("Init Finished\n");
	#endif
	return 0;
}
