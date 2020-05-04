//********************************************************************************
//
//		<< LC898109 Evaluation Soft >>
//		Program Name	: CAMCMD.c
//		Design			: Y.Yamada
//		History			: First edition						2009.07.31 Y.Tashita
//********************************************************************************
//**************************
//	Include Header File		
//**************************
#define		CAMCMD
#define 	CAMIFT


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/export.h>
//#include	"Main.h"
//#include 	"timer.h"
//#include	"Cmd.h"
#include	"Cam.h"
#include	"CamDef.h"
#include    "var.h"
//#include	"io.h"
#include	"bkavspi.h"
//#include	"usb.h"

#if ( STM3_MODE_A == 1 ) || ( STM2_MODE_A == 1 ) || ( STM2_MODE_B == 1 )

#endif
//unsigned char	StmDra( unsigned char, unsigned short, unsigned short );
//unsigned char	StmDrb( unsigned char, unsigned short, unsigned short );
//unsigned char	VcmDrg( unsigned long, unsigned long, unsigned long, unsigned char );

#define	INI_A_MDIS			0x0100
#define	INI_B_MDIS			0x0100
#define	INI_C_MDIS			0x1000

// MR OFFSET ADJUSTMENT
#define	MR_OFFSET_MARGIN	0x3C000000			//0.0078125F	// 0x0100 	0x3C000000
#define	MRA_OFFSET_DIV		0x3C000000			//0.0078125F	// 0x0100	0x3C000000
#define	MRB_OFFSET_DIV		0x3C000000			//0.0078125F	// 0x0100	0x3C000000

// HALL OFFSET & BIAS ADJUSTMENT
#define	OFFSET_MARGIN		0.0078125F			//0x0100 	0x3C000000
#define	BIAS_MARGIN			0.0244150F			//0x3CC801F7    //	// 0x0320	0x3CC7FFDE	
#define	TGT_PTP				0x3F59364E			// 0.8484849(About 85%)

#define	OFFSET_DIV		0.0078125F			// 0x0100	0x3C000000
#define	BIAS_DIV		0.0243164F			// 0x031D	0x3CC73259
#define	ADJ_COUNT		100

#define CW_DIR	0
#define CCW_DIR	1


//*************************************************************************************************
//	Local Function
//*************************************************************************************************

unsigned char	StmDra( unsigned char UcSetDir, unsigned short UsSetSpd, unsigned short UsSetDis )
{
	unsigned char	UcPc2Rga;
	RegReadA( A_PC2 , &UcPc2Rga );
	if( UcSetDir == CW_DIR ){
		UcPc2Rga	&= 0x7F ; //0x19
		#if( DEBUG==1)
		printk("Direction: %02x\n", UcPc2Rga);
		#endif
	}else{
		UcPc2Rga	|= 0x80 ; //0x99
		#if( DEBUG==1)
		printk("Direction: %02x\n", UcPc2Rga);
		#endif
	}
	RegWriteA( A_PC2 , UcPc2Rga );
	RegWriteA( A_MSPD1 , (unsigned char)( UsSetSpd ) );
	RegWriteA( A_MSPD2 , (unsigned char)( UsSetSpd >> 8 ) ) ;
	if( UsSetDis != 0 ){
		RegWriteA( A_MDIS1 , (unsigned char)( UsSetDis ) ) ;
		RegWriteA( A_MDIS2 , (unsigned char)( UsSetDis >> 8 ) ) ;
	}
	RegWriteA( A_TRIG , 0x01 );			//Ch A
	RegWriteA( PISEN	, 0x11 ) ;			// PIS1 LED ON active PIS1
	#if( DEBUG==1)
	printk("Stm chanel A is running\n");
	#endif
	return 1;
}
//********************************************************************************
// Function Name 	: StmDrb
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: STM Channel-B Driver
// History			: First edition 						2012.04.05 Y.Kim
//********************************************************************************
unsigned char	StmDrb( unsigned char UcSetDir, unsigned short UsSetSpd, unsigned short UsSetDis )
{
	unsigned char	UcPc2Rgb,UcPIBTrig;
	RegReadA( B_PC2 , &UcPc2Rgb );
//	RegWriteA( B_MDIS1 , 0x00 ) ;
//	RegWriteA( B_MDIS2 , 0x00 ) ;
	if( UcSetDir == CW_DIR )
	{
		UcPc2Rgb	&= 0x7F ;

	}else{
		UcPc2Rgb	|= 0x80 ;
	}
	RegWriteA( B_PC2 , UcPc2Rgb );
	RegWriteA( B_MSPD1 , (unsigned char)( UsSetSpd ) ) ;
	RegWriteA( B_MSPD2 , (unsigned char)( UsSetSpd >> 8 ) ) ;
	if( UsSetDis != 0 )
	{
		RegWriteA( B_MDIS1 , (unsigned char)( UsSetDis ) ) ;
		RegWriteA( B_MDIS2 , (unsigned char)( UsSetDis >> 8 ) ) ;
	}
	RegWriteA( B_TRIG , 0x01 );			//Ch B
	RegWriteA( PISEN	, 0x11 ) ;			// PIS1 LED ON active PIS1
	return 1;
}

//********************************************************************************
// Function Name 	: VcmDrg
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: H-VCM Driver
// History			: First edition 						2012.04.05 Y.Kim
//********************************************************************************
unsigned char	VcmDrg( UnFltVal UlTgtPos, UnFltVal UlTgtInc, UnFltVal UlSmvSiz, unsigned char UcSmvItv )
{
	UnFltVal	UlStmZ2g = {0x00000000};
#if( DEBUG==1)
printk("VcmDrg is running\n");
#endif
	if( UlTgtInc.UlLngVal == 0x00000000 ){
		RegWriteA( GL_DWNSMP2	, UcSmvItv ) ;							// Set StepMove Interval
		RamWrite32A( GLSTMSTP	, UlSmvSiz.UlLngVal );					// Set StepMove Size

		RamWrite32A( GLSTMTGT	, UlTgtPos.UlLngVal );					// Set Target Position
		RegWriteA( GL_STPMV	, 0x01 );									// Go
	}
	else{
		RegWriteA( GL_DWNSMP2	, UcSmvItv ) ;							// Set StepMove Interval
		RamWrite32A( GLSTMSTP	, UlSmvSiz.UlLngVal );					// Set StepMove Size

		//RamWrite32A( 0x1072 , UlTgtPos.UlLngVal );
		//RamWrite32A( 0x1073 , UlTgtInc.UlLngVal );

		RamRead32A( GLSTMZ2		, &UlStmZ2g.UlLngVal ) ;				// Get Current Position

		//RamWrite32A( 0x1074 , UlStmZ2g.UlLngVal );

		//UlStmZ2g.SfFltVal	+= UlTgtInc.SfFltVal ;	// Cal Target Postion

		//RamWrite32A( 0x1075 , UlStmZ2g.UlLngVal );


		RamWrite32A( GLSTMTGT	, UlStmZ2g.UlLngVal );					// Set Target Position
		RegWriteA( GL_STPMV	, 0x01 );									// Go
	}
	return 1;
}



