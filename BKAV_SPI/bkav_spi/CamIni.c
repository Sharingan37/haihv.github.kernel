//********************************************************************************
//
//		<< LC898201 Evaluation Soft >>
//	    Program Name	: CamIni.c
//		Design			: Y.Yamada
//		History			: First edition						2011.06.06 
//********************************************************************************
//**************************
//	Include Header File		
//**************************
#define		CAMINI
#include"var.h"
//#include	"Main.h"
//#include 	"io.h"
//#include	"Cmd.h"
#include	"Cam.h"
#include	"CamIni.h"
#include	"bkavspi.h"
#include	"CamDef.h"
//#include	"CamFil.h"

const struct STMHLREG	CsMhlReg[]	= {
	{ 0x0041,	0x03 },	/*03,0041*/
	{ 0x0042,	0x03 },	/*03,0042*/
	{ 0x0021,	0x0F },	/*0F,0020*/
	{ 0x0022,	0x02 },	/*02,0021*/
	{ 0xFFFF,	0xFF }
} ;

/* 32bit */
const struct STMHLFIL	arConfigRamFilter[]	= {
	{	0x1001,		0xbf800000},
	{	0x1002,		0x3f800000},
	{	0x1003,		0x00000000},
	{	0x1004,		0x3f800000},
	{	0x1005,		0x00000000},
	{	0x1006,		0x3f6cf1a0},
	{	0x1007,		0xbf6cf1a0},
	{	0x1008,		0x3f59e340},
	{	0x1009,		0x3dc6581d},
	{	0x100A,		0x3dc6581d},
	{	0x100B,		0x3f4e69f9},
	{	0x100C,		0x3d1e57b5},
	{	0x100D,		0x3d1e57b5},
	{	0x100E,		0x3f6c3509},
	{	0x1010,		0x3cb76563},
	{	0x1011,		0x00000000},
	{	0x1012,		0x3f800000},
	{	0x1013,		0x3f800000},
	{	0x1014,		0x00000000},
	{	0x1015,		0x00000000},
	{	0x1016,		0x41c8f36f},
	{	0x1017,		0x3fcaddc8},
	{	0x1018,		0x3f800000},
	{	0x1019,		0x3f800000},
	{	0x101A,		0x3fff64c1},
	{	0x101B,		0x00000000},
	{	0x1020,		0x3c19ac6b},
	{	0x1021,		0x3c99ac6b},
	{	0x1022,		0x3fce69f9},
	{	0x1023,		0x3c19ac6b},
	{	0x1024,		0xbf266eb8},
	{	0x102A,		0x3baec0ee},
	{	0x102B,		0x3baec0ee},
	{	0x102C,		0x3f7d44fc},
	{	0x102D,		0xbf800000},
	{	0x1030,		0x3f800000},
	{	0x1031,		0x00000000},
	{	0x1032,		0x00000000},
	{	0x1033,		0x3c0d8400},
	{	0x1034,		0x3c0d8400},
	{	0x1035,		0x3f7b93e0},
	{	0x1036,		0x3b833d7e},
	{	0x1037,		0x3b833d7e},
	{	0x1038,		0x3f7df30a},
	{	0x1039,		0x3aaf7496},
	{	0x103A,		0x3aaf7496},
	{	0x103B,		0x3f7f508b},
	{	0x103C,		0x3aaf7496},
	{	0x103D,		0x3aaf7496},
	{	0x103E,		0x3f7f508b},
	{	0x1041,		0x00000000},
	{	0x1042,		0x00000000},
	{	0x1043,		0x00000000},
	{	0x1044,		0x00000000},
	{	0x1045,		0x00000000},
	{	0x1046,		0x3f800000},
	{	0x1047,		0x00000000},
	{	0x1048,		0x00000000},
	{	0x1049,		0x3f800000},
	{	0x104A,		0x00000000},
	{	0x104B,		0x00000000},
	{	0x104C,		0x3f800000},
	{	0x104D,		0x00000000},
	{	0x104E,		0x00000000},
	{	0x1050,		0x3f800000},
	{	0x1051,		0x00000000},
	{	0x1052,		0x00000000},
	{	0x1053,		0x3f800000},
	{	0x1054,		0x00000000},
	{	0x1055,		0x00000000},
	{	0x1056,		0x00000000},
	{	0x1057,		0x00000000},
	{	0x1058,		0x00000000},
	{	0x1059,		0x00000000},
	{	0x105A,		0x00000000},
	{	0x105B,		0x00000000},
	{	0x105C,		0x00000000},
	{	0x1060,		0x00000000},
	{	0x1061,		0x00000000},
	{	0x1062,		0x00000000},
	{	0x1063,		0x00000000},
	{	0x1064,		0x00000000},
	{	0x106A,		0x3b5ae0ff},
	{	0x106B,		0x3b5ae0ff},
	{	0x106C,		0x3f7e4a3e},
	{	0x106D,		0x00000000},
	{	0x107E,		0x00000000},
	{ 0xFFFF,	0xFFFFFFFF }
} ;


//********************************************************************************
// Function Name 	: IniSet
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: Initial Setting Function
// History			: First edition 						2011.06.06 
//********************************************************************************
void	IniSet( void )
{
	UcQutFlg = 0;
	UcTp1Flg = 0;
	UcTp2Flg = 0;
	//UlNerBor.SfFltVal	= 0.0F;
	//UlFarBor.SfFltVal	= 200.0F;
	// Command Execute Process Initial
	IniCmd() ;

	// Initialize LSI
	IniRst() ;
	IniOsc() ;
	IniClk() ;
	IniCha() ;
	IniEqc() ;
	IniAna() ;
	IniMon() ;
	IniPWM() ;
	
	//Stepping Driver Setting
	InsCha() ;		// Stepping Channel Setting
	InsPds() ;
/*
#if ( STM3_MODE_A == 1 ) || ( STM3_MODE_B == 1 )
	//InsPdx( PI_A_POL, PI_B_POL, PI_C_POL ) ;
#endif

#if ( STM2_MODE_A == 1 ) || ( STM2_MODE_B == 1 )
	//InsPdx( PI_A_POL, PI_B_POL ) ;
#endif
*/

	//InsBpx() ;		// Base Position
	
#if ( STM3_MODE_A == 1 ) || ( STM2_MODE_A == 1 ) || ( STM2_MODE_B == 1 )
//	IniFil() ;

	//HALL-VCM Driver Setting
//	IngCha() ;		// HALL-VCM Channel Setting
//	IngBpx() ;		// Base Position
#endif

#if ( STM3_MODE_B == 1 ) || ( STM2_MODE_B == 1 )
	IniDN() ;		// DN Initialization for Hall-VCM Channel
#endif

	//IniVsc() ;		// VSYNC Mode ON
}



//********************************************************************************
// Function Name 	: IniCmd
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: Command Execute Process Initialization
// History			: First edition 						2011.06.06 
//********************************************************************************
void	IniCmd( void )
{
	UcCmdStp	= 0 ;												// Command Sequence Step
	UcRcvPtr	= 0 ;												// Recieve Buffer Pointer Initialize
	UcSndPtr	= 0 ;												// Send Buffer Data Counter

	MemClr( UcRcvVal, sizeof( UcRcvVal ) ) ;
	MemClr( UcSndVal, sizeof( UcSndVal ) ) ;
	#if( DEBUG==1)
	printk("IniCmd():	Command Execute Process Initialization\n");
	#endif
}

//********************************************************************************
// Function Name 	: IniRst
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: Software Reset Function
// History			: First edition 						2012.02.08 Y.Kim
//********************************************************************************
void	IniRst( void )
{
	RegWriteA( SOFTRES1	, 0x00 ) ;		//Clock, PWM, Stepping, Servo Circuit Reset
	WitTim( 5 ) ;
	RegWriteA( SOFTRES1	, 0x0F ) ;		//Clear
	#if( DEBUG==1)
	printk("IniRst():	Software Reset Function\n");
	#endif
}



//********************************************************************************
// Function Name 	: IniOsc
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: OSC
// History			: First edition 						2012.02.08 Y.Kim
//********************************************************************************
void	IniOsc( void )
{
#ifndef USE_OSC													// Use a Crystal
	RegWriteA( TSTMOD, 0x80 ) ;									// Use External X'tal
	RegWriteA( MONSELE, 0x00 ) ;								// MONE <- fixed L
	RegWriteA( CLKO1SEL, 0x41 ) ;								// CLKO1 <- MONE
	RegWriteA( CLKO2SEL, 0x40 ) ;	//41							// CLKO2 <- MONE
	#if( DEBUG==1)
	printk("IniOsc(): 	Use External finished\n");
	#endif
#else															// Use the OSC
	RegWriteA( CLKOFF	, 0x10 ) ;								// Crystal OFF
	RegWriteA( OSCSET	, 0x48 ) ;								// Typical Frequency,  Normal Mode (48MHz)
	#if( DEBUG==1)
	printk("IniOsc(): 	Use Internal finished\n");
	#endif
#endif
	
}

//********************************************************************************
// Function Name 	: IniClk
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: Clock Division Setting Function
// History			: First edition 						2012.02.08 Y.Kim
//********************************************************************************
void	IniClk( void )
{
	RegWriteA( PWMDIV	, 0x00 ) ;		// PWM Clock	= 48MHz / 1	= 48MHz
	RegWriteA( SRVDIV	, 0x04) ;		// Servo Clock	= 48MHz / 4	= 12MHz( fs = 12MHz/512 = 23.4375kHz )   //*
	RegWriteA( STMDIV	, 0x04) ;		// PWM Clock 	= 48MHz / 4 = 12MHz               //*
	#if( DEBUG==1)
	printk("IniClk():	Clock Division Setting Function\n");
	#endif
}


//********************************************************************************
// Function Name 	: IniCha
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: Driver Mode Select Fuction
// History			: First edition 						2012.02.08 Y.Kim
//********************************************************************************
void	IniCha( void )
{
	unsigned char value=0xf0;
#if ( STM3_MODE_A == 1) || ( STM3_MODE_B == 1 )
	RegWriteA( DRVSEL, 0x00 ) ;								// STM 3ch + HALL-VCM Mode Setting
#endif

#if ( STM2_MODE_A == 1 )
	RegWriteA( DRVSEL, 0x01) ;								// STM 2ch + HALL-VCM Mode Setting
#endif

#if ( STM2_MODE_B == 1 )
	RegWriteA( DRVSEL, 0x14 ) ;								// Stepping 2ch + Hall-VCM + DN Mode
#endif

	RegWriteA( CM_ADSEL01, 0x10 ) ;
	RegWriteA( CM_ADSEL23, 0x32 ) ;
	RegWriteA( CM_ADSEL45, 0x54 ) ;
	RegWriteA( H_ADMODE, 0x03 ) ;
	#if( DEBUG==1)
	printk("IniCha():	Driver Mode Select Function\n");
	#endif
}



//********************************************************************************
// Function Name 	: IniEqc
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: Equalizer Clear
// History			: First edition 						2012.02.08 Y.Kim
//********************************************************************************
void	IniEqc( void )
{
	RegWriteA( CM_RAMINITON	, 0x03 ) ;			// Initialize RAM
	msleep(1);
	RegWriteA( AF_EQSW	, 0x00 ) ;				// AF Servo Loop OFF
	RegWriteA( GL_EQSW	, 0x00 ) ;				// GL Servo Loop OFF
	RegWriteA( AF_STPMV	, 0x00 ) ;				// AF StepMove OFF
	RegWriteA( GL_STPMV	, 0x00 ) ;				// GL StepMove OFF
	
	RamWrite32A( GL2PWMZ	, 0xBF800000 ) ;	// GL_EQ Output Setting
	RamWrite32A( AF2PWMZ	, 0x00000000 ) ;	// AF_EQ Output Setting
	RegWriteA( CM_EQON	, 0x00 ) ;				// AF&GL EQ OFF
	#if( DEBUG==1)
	printk("IniEqc():	Equalizer Clear \n");
	#endif
}



//********************************************************************************
// Function Name 	: IniAna
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: Analog Circuit
// History			: First edition 						2012.02.08 Y.Kim
//********************************************************************************
void	IniAna( void )
{
	RegWriteA( CMSDAC	, BIAS_CURRENT_GL ) ;			// BIAS Current = 1.0mA
	RegWriteA( H_DAV1	, OFFSET_GL ) ;					// OFFSET for GL	(IRIS)      0x0079  :
	RegWriteA( H_DAV4	, BIAS_GL ) ;					// BIAS for GL(IRIS)
	
	RegWriteA( OP2SET	, OPAMP_GL		) ;				// OP-AMP GAIN For GL	=x50
	
#if ( STM3_MODE_A == 1) || ( STM2_MODE_A == 1 ) || ( STM2_MODE_B == 1 )
	RegWriteA( STBB1	, 0xA7 ) ;						// Thermal Shut Down,OP-AMP,DAC,ADC ON
#endif

#if ( STM3_MODE_B == 1 )
	RegWriteA( STBB1	, 0x81 ) ;						// Thermal Shut Down,ADC ON
#endif

#ifndef USE_OSC
	RegWriteA( STBB2	, 0x02 ) ;						// Low Volt Detection, OSC Standby
#else
	RegWriteA( STBB2	, 0x03 ) ;						// Low Volt Detection, OSC ON
#endif
#if( DEBUG==1)
	printk("IniAna():	Analog Circuit \n");
#endif
}



//********************************************************************************
// Function Name 	: IniMon
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: Monitor Setting Function
// History			: First edition 						2012.02.08 Y.Kim
//********************************************************************************
void	IniMon( void )
{
	// Delay RAM Gain
	RamWrite32A( dm1g	, 0x3F800000 ) ;	//dm1g Delay Monitor1 Gain
	RamWrite32A( dm2g	, 0x3F800000 ) ;	//dm2g Delay Monitor1 Gain
	RamWrite32A( dm3g	, 0x3F800000 ) ;	//dm3g Delay Monitor1 Gain
	RamWrite32A( dm4g	, 0x3F800000 ) ;	//dm4g Delay Monitor1 Gain
	
	// Delay RAM Monitor
	RegWriteA( H_DLYMON1	, 0x2E ) ;		//Delay Monitor 1 = Signal(12XXh)    //  0x0084
	RegWriteA( H_DLYMON2	, 0x2A ) ;		//Delay Monitor 2 = Signal(12XXh)    //  0x0085
	RegWriteA( H_DLYMON3	, 0x2E ) ;		//Delay Monitor 3 = Signal(12XXh)    //  0x0086
	RegWriteA( H_DLYMON4	, 0x2E ) ;		//Delay Monitor 4 = Signal(12XXh)    //  0x0087
	
	//RegWriteA( H_DLYMON1	, 0x2A ) ;		//Delay Monitor 1 = Signal(12XXh)    //  0x0084
	//RegWriteA( H_DLYMON2	, 0x3A ) ;		//Delay Monitor 2 = Signal(12XXh)    //  0x0085
	//RegWriteA( H_DLYMON3	, 0x2A ) ;		//Delay Monitor 3 = Signal(12XXh)    //  0x0086
	//RegWriteA( H_DLYMON4	, 0x3A ) ;		//Delay Monitor 4 = Signal(12XXh)    //  0x0087
	//RegWriteA( PWMMONA		, 0x03 ) ;		//Delay Monitor 1 = AFSELZ	Delay Monitor 2 = AFSTA
	
	// PIN Monitor
	RegWriteA( H_PINMON0	, 0x28 ) ;		//MON0=GlLevDif
	RegWriteA( H_PINMON1	, 0x29 ) ;		//MON1=GlLev
	RegWriteA( H_PINMON2	, 0x2A ) ;		//MON2=GlDif
	RegWriteA( H_PINMON3	, 0x09 ) ;		//MON3=GmStpMv
 #if ( STM2_MODE_A == 1 ) || ( STM2_MODE_B == 1 )
	// D/A Monitor
	RegWriteA( DACMONFC, 0x89 ) ;			// Monitor DAC ON
	RegWriteA( PIS2SEL, 0x01 ) ;			// PIS2  = MONA
	RegWriteA( CLKO1SEL, 0x11 ) ;			// CLK01 = MONB
	RegWriteA( CLKO2SEL, 0x21 ) ;			// CLK02 = MONC
	
	RegWriteA( MONSELA, 0x68) ; // 68-> 01	-- modifed by ThangLMb		// MONA <- DASDO             0x0270
	RegWriteA( MONSELB, 0x69 ) ; // 69-> 11 -- modifed by ThangLMb					// MONA <- DASCLK           0x0271
	RegWriteA( MONSELC, 0x6A ) ; // 6A-> 21	-- modifed by ThangLMb			// MONA <- DASYNCB	   0x0272
	//RegWriteA( MONSELA, 0x08 ) ;			// MONA <- DASDO
	//RegWriteA( MONSELB, 0x09 ) ;			// MONA <- DASCLK
	//RegWriteA( MONSELC, 0x0A ) ;			// MONA <- DASYNCB
 #endif
	
	RegWriteA( MONSEL, 0x01 ) ;			// MON  = MONA       0x0237
	RegWriteA( VSY1SEL, 0x11 ) ;			// VSYNC1  -> MONB        0x0230
	RegWriteA( VSY2SEL, 0x21 ) ;			// VSYNC2  -> MONC        0x0230
	RegWriteA( PDON, 0x00 ) ;			// VSYNC1,2  Pull Down OFF        0x0220

	//RegWriteA( MONSEL	, 0x30 ) ;			// MON  = MOND       0x0237 
	//RegWriteA( MONSELD	, 0x55 ) ;			// MOND <- A_ST0
	#if( DEBUG==1)
	printk("IniMon():	Monitor Setting Function \n");
	#endif
}

//********************************************************************************
// Function Name 	: IniPWM
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: PWM Setting Function
// History			: First edition 						2012.02.08 Y.Kim
//********************************************************************************
void	IniPWM( void )
{
	RegWriteA( DRVFC6	, 0x00 ) ;			// MODE/ENA Setting

#if ( STM3_MODE_A == 1 ) || ( STM2_MODE_A == 1 ) || ( STM2_MODE_B == 1 )
	RegWriteA( PWMFC6	, 0x5D ) ;			// GND Mode, 100KHz, Brake Mode, 12bit Resolution For IRIS
#endif

#if ( STM3_MODE_B == 1 )
	RegWriteA( PWMFC6	, 0x1D ) ;			// VREF Mode, PWMCLK/512, Brake Mode, 12bit Resolution
#endif

	RegWriteA( PWMDLY6	, 0x00 ) ;
	RegWriteA( PWMA6	, 0x80 ) ;			// PWM ON for IRIS

#if ( STM2_MODE_B == 1 )
	RegWriteA( DRVFC7	, 0x00 ) ;			// MODE = 0,ENA = 0
	RegWriteA( PWMFC7	, 0x1D ) ;			// VREF Mode, PWMCLK/512, Brake Mode, 12bit Resolution
#endif
	#if( DEBUG==1)
	printk("IniPWM():	PWM Setting Function \n");
	#endif
}



//********************************************************************************
// Function Name 	: InsCha
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: Stepping-Channel Initialization Function
// History			: First edition 						2012.02.08 Y.Kim
//********************************************************************************
void	InsCha( void )
{
	//Stepping Channel A (ZOOM)

	RegWriteA( A_DIV	, 0x0C ) ;								// DIV1 1/1, DIV2 8Bit
	RegWriteA( A_PC2	, INI_A_PC2 ) ;							// Excitaion Mode is 1024u step
	RegWriteA( A_GN		, 0xFF ) ;								// PWM Gain

	RegWriteA( A_MSPD1	, (unsigned char)(INI_A_MSPD) ) ;		// Move Speed 1500pps
	RegWriteA( A_MSPD2	, (unsigned char)(INI_A_MSPD >> 8) ) ;	// Move Speed 1500pps
	RegWriteA( A_SPDIV	, 0x00 ) ;								// SPDIV 1/1
	RegWriteA( A_STSEL	, 0x0B ) ;								// Monitor Setting = PERIOD1
	//RegReadA(A_INIT, 0x80);     
	//Stepping Channel B (AF)
	//RegReadA(B_INIT, 0x80); 
	RegWriteA( B_DIV	, 0x0C ) ;								// DIV1 1/1, DIV2 8Bit
	RegWriteA( B_PC2	, INI_B_PC2 ) ;							// Excitaion Mode is 1024u step
	RegWriteA( B_GN		, 0xFF ) ;								// PWM Gain
	//RegWriteA( B_SGAIN	, 0xFF ) ;  							// Gain Set up
	RegWriteA( B_MSPD1	, (unsigned char)(INI_B_MSPD) ) ;		// Move Speed 1500pps
	RegWriteA( B_MSPD2	, (unsigned char)(INI_B_MSPD >> 8) ) ;	// Move Speed 1500pps
	RegWriteA( B_SPDIV	, 0x00 ) ;								// SPDIV 1/1
	RegWriteA( B_STSEL	, 0x0B ) ;								// Monitor Setting = PERIOD1
#if ( STM3_MODE_A == 1 ) || ( STM3_MODE_B == 1 )
	//Stepping Channel C (DN)
	RegWriteA( C_DIV	, 0x0C ) ;								// DIV1 1/1, DIV2 8Bit
	RegWriteA( C_PC2	, INI_C_PC2 ) ;							// Excitaion Mode is 1024u step
	RegWriteA( C_GN		, 0xFF ) ;								// PWM Gain
	RegWriteA( C_MSPD1	, (unsigned char)(INI_C_MSPD) ) ;		// Move Speed 1000pps
	RegWriteA( C_MSPD2	, (unsigned char)(INI_C_MSPD >> 8) ) ;	// Move Speed 1000pps
	RegWriteA( C_SPDIV	, 0x00 ) ;								// SPDIV 1/1
	RegWriteA( C_STSEL	, 0x0B ) ;								// Monitor Setting = PERIOD1
#endif
	//Pulse Generator ON
	RegWriteA( A_PC1	, 0x82 ) ;	//	82	// Pulse Generator ON, PWM  Offstate is Brake
	RegWriteA( B_PC1	, 0x82 ) ;			// Pulse Generator ON, PWM  Offstate is Brake
/*
 Bit 1=1;
 Bit 2,3:
0: 1/64μ step position
1: 1/32μ step position
2: 1/16μ step position
3: 1-2 phase position
4: 2 phase position
_pc5: 1 phase position

*/

#if ( STM3_MODE_A == 1 ) || ( STM3_MODE_B == 1 )
	RegWriteA( C_PC1	, 0x82 ) ;			// Pulse Generator ON, PWM  Offstate is Brake
#endif
	
	RegWriteA( A_MDIS1	, 0x00 ) ;			// Move Length
	//RegWriteA( A_MDIS1	, 0x00 ) ;			// Move Length
	RegWriteA( A_LCTR1	, 0x00 ) ;			// Position Counter
	RegWriteA( A_LCTR2	, 0x00 ) ;			// Position Counter
	RegWriteA( A_LCTR3	, 0x00 ) ;			// Position Counter
	
	RegWriteA( B_MDIS1	, 0x00 ) ;			// Move Length
	//RegWriteA( B_MDIS1	, 0x00 ) ;			// Move Length
	RegWriteA( B_LCTR1	, 0x00 ) ;			// Position Counter
	RegWriteA( B_LCTR2	, 0x00 ) ;			// Position Counter
	RegWriteA( B_LCTR3	, 0x00 ) ;			// Position Counter
	
#if ( STM3_MODE_A == 1 ) || ( STM3_MODE_B == 1 )
	RegWriteA( C_MDIS1	, 0x00 ) ;			// Move Length
	//RegWriteA( C_MDIS1	, 0x00 ) ;			// Move Length
	RegWriteA( C_LCTR1	, 0x00 ) ;			// Position Counter
	RegWriteA( C_LCTR2	, 0x00 ) ;			// Position Counter
	RegWriteA( C_LCTR3	, 0x00 ) ;			// Position Counter
#endif
	#if( DEBUG==1)
	printk("InsCha():	Stepping-Channel Initialization Function\n");
	#endif
}



//********************************************************************************
// Function Name 	: InsPds
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: PI Detection Threshold Initialization Function
// History			: First edition 						2012.02.08 Y.Kim
//********************************************************************************
void	InsPds( void )
{
#if ( STM3_MODE_A == 1 ) || ( STM3_MODE_B == 1 )
	RegWriteA( PISEN	, 0x11 ) ;			// PIS1,PIS2 LED ON
#endif

#if ( STM2_MODE_A == 1 ) || ( STM2_MODE_B == 1 )
	RegWriteA( PISEN	, 0x11 ) ;			// PIS1 LED ON
#endif
	
	// Threshold settings are examples.
	RegWriteA( A_VIHL	, 0x00 ) ;			// PI Rising Detecition Level
	RegWriteA( A_VIHH	, 0x09 ) ;			// PI Rising Detecition Level
	RegWriteA( A_VILL	, 0x00 ) ;			// PI Rising Detecition Level
	RegWriteA( A_VILH	, 0x08 ) ;			// PI Rising Detecition Level
	
	RegWriteA( B_VIHL	, 0x00 ) ;			// PI Rising Detecition Level
	RegWriteA( B_VIHH	, 0x09 ) ;			// PI Rising Detecition Level
	RegWriteA( B_VILL	, 0x00 ) ;			// PI Rising Detecition Level
	RegWriteA( B_VILH	, 0x08 ) ;			// PI Rising Detecition Level
#if ( STM3_MODE_A == 1 ) || ( STM3_MODE_B == 1 )
	RegWriteA( C_VIHL	, 0x00 ) ;			// PI Rising Detecition Level
	RegWriteA( C_VIHH	, 0x09 ) ;			// PI Rising Detecition Level
	RegWriteA( C_VILL	, 0x00 ) ;			// PI Rising Detecition Level
	RegWriteA( C_VILH	, 0x08 ) ;			// PI Rising Detecition Level
#endif
	#if( DEBUG==1)
	printk("InsPds():	PI Detection Threshold Initialization Function\n");
	#endif
}


//********************************************************************************
// Function Name 	: InsPdx
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: PI Detection
// History			: First edition 						2012.02.08 Y.Kim
//********************************************************************************
void          InsPdxA(unsigned char  UcPiaPol_t)
{
               unsigned char	UcPIInit = 0x00;
               unsigned char	UcPIATrig = 0x01;
              
	       //RegWriteA( PISEN	, 0x01 ) ;                     // PIS1 ON
	       RegWriteA( PISEN	, 0x10 ) ;                     // PIS2 ON

                //WitTim(2);  
          
                RegReadA(A_INIT, &UcPIInit);                      //  
                
               if ( (UcPIInit && 0x10) != 0x10)
               {
                  RegWriteA( A_DIV	, 0x4C ) ;     // Divider  Set up
                  RegWriteA( A_GN	, 0xFF ) ;     // PWM gain Set up
                  RegWriteA( A_MSPD1	, 0xA7 ) ;  // Move Speed Set up
                  RegWriteA( A_MSPD2	, 0x61 ) ;  // Move speed setting(60pps)
                  RegWriteA( A_INIT	, 0x80 ) ;           // <Forced move + normal move> mode 
                  RegWriteA( A_SPDIV	, 0x03 ) ;  // Divider setting
               
                  //  Excitation  ON
                  //  A ch
                   RegWriteA( A_PC1	, 0x82 ) ;  // Pulse generation control 1
                   RegWriteA( A_PC2	, 0x93 ) ;  // Pulse generation control 2 <CW>
                   RegWriteA( A_SGAIN	, 0xFF ) ;  // Gain Set up
               
                   // Move 
                   // A ch
                   RegWriteA( A_MDIS1	, 0xA0 ) ;  // Move length setting
                   RegWriteA( A_MDIS2	, 0x00 ) ;  // Move length setting (20step)
                   
                   RegWriteA( A_TRIG	, 0x01 ) ;  // Trigger Set up
               
                   while ((UcPIATrig && 0x01) == 0x01)
                   {
                      RegReadA(A_TRIG, &UcPIATrig);
                   }
               
               UcPIATrig = 0x01;
               //WitTim(10);  
               
               }// endif
                
                  RegWriteA( A_DIV	, 0x4C ) ;     // Divider  Set up
                  RegWriteA( A_GN	, 0xFF ) ;     // PWM gain Set up
                  RegWriteA( A_MSPD1	, 0xA7 ) ;  // Move Speed Set up
                  RegWriteA( A_MSPD2	, 0x61 ) ;  // Move speed setting(60pps)
                  RegWriteA( A_INIT	, 0x80 ) ;           // <Forced move + normal move> mode 
                  RegWriteA( A_SPDIV	, 0x03 ) ;  // Divider setting
                  
                  //  Excitation  ON
                  //  A ch
                   RegWriteA( A_PC1	, 0x82 ) ;  // Pulse generation control 1
                   RegWriteA( A_PC2	, 0x13 ) ;  // Pulse generation control 2 <CW>
                   RegWriteA( A_SGAIN	, 0xFF ) ;  // Gain Set up
                   
                   // Move 
                   // A ch
                   RegWriteA( A_MDIS1	, 0xA0 ) ;  // Move length setting
                   RegWriteA( A_MDIS2	, 0x00 ) ;  // Move length setting (20step)
                   
                   RegWriteA( A_TRIG	, 0x01 ) ;  // Trigger Set up
                   
                   while ((UcPIATrig && 0x01) == 0x01)
                   {
                      RegReadA(A_TRIG, &UcPIATrig);
                   }

                    RegWriteA( A_LCTR1	, 0x00 ) ;  //  LCTR = 0 
                    RegWriteA( A_LCTR2	, 0x00 ) ;  // 
                    RegWriteA( A_LCTR3	, 0x00 ) ;  //
  
                    //   Excitation  OFF
                    RegWriteA( A_PC2	, 0x03 ) ;  //
  
                   //  PI Set OFF
                   RegWriteA( PISEN	, 0x00 ) ;  //
	#if( DEBUG==1)
	printk("InsPdxA():	PI Detection chanel A\n");
	#endif
}

//=================================================================
void          InsPdxB(unsigned char  UcPibPol_t)
{
                 unsigned char	UcPIInit ;
                 unsigned char	UcPIBTrig = 0x01;

                  //-------------------------PI Set up--------------------//
               RegWriteA( PISEN	, 0x01 ) ;                     // PIS1 ON
               //RegWriteA( PISEN	, 0x10 ) ;                     // PIS2 ON
              
               //WitTim(2);  
          
               RegReadA(B_INIT, &UcPIInit);                      //  
          
               if ( (UcPIInit && 0x10) != 0x10)
               {
                  RegWriteA( B_DIV	, 0x4C ) ;     // Divider  Set up
                  RegWriteA( B_GN	, 0xFF ) ;     // PWM gain Set up
                  RegWriteA( B_MSPD1	, 0xA7 ) ;  // Move Speed Set up
                  RegWriteA( B_MSPD2	, 0x61 ) ;  // Move speed setting(60pps)
                  RegWriteA( B_INIT	, 0x80 ) ;           // <Forced move + normal move> mode 
                  RegWriteA( B_SPDIV	, 0x03 ) ;  // Divider setting
               
                  //  Excitation  ON
                  //  B ch
                   RegWriteA( B_PC1	, 0x82 ) ;  // Pulse generation control 1
                   RegWriteA( B_PC2	, 0x93 ) ;  // Pulse generation control 2 <CW>
                   RegWriteA( B_SGAIN	, 0xFF ) ;  // Gain Set up
               
                   // Move 
                   // B ch
                   RegWriteA( B_MDIS1	, 0xA0 ) ;  // Move length setting
                   RegWriteA( B_MDIS2	, 0x00 ) ;  // Move length setting (20step)
                   
                   RegWriteA( B_TRIG	, 0x01 ) ;  // Trigger Set up
               
                   while ((UcPIBTrig && 0x01) == 0x01)
                   {
                      RegReadA(B_TRIG, &UcPIBTrig);
                   }
               
                   UcPIBTrig = 0x01;
               //WitTim(10);  
               
               }// endif
               
               
                  RegWriteA( B_DIV	, 0x4C ) ;     // Divider  Set up
                  RegWriteA( B_GN	, 0xFF ) ;     // PWM gain Set up
                  RegWriteA( B_MSPD1	, 0xA7 ) ;  // Move Speed Set up
                  RegWriteA( B_MSPD2	, 0x61 ) ;  // Move speed setting(60pps)
                  RegWriteA( B_INIT	, 0x80 ) ;           // <Forced move + normal move> mode 
                  RegWriteA( B_SPDIV	, 0x03 ) ;  // Divider setting
                  
                  //  Excitation  ON
                  //  B ch
                   RegWriteA( B_PC1	, 0x82 ) ;  // Pulse generation control 1
                   RegWriteA( B_PC2	, 0x13 ) ;  // Pulse generation control 2 <CW>
                   RegWriteA( B_SGAIN	, 0xFF ) ;  // Gain Set up
                   
                   // Move 
                   // B ch
                   RegWriteA( B_MDIS1	, 0xA0 ) ;  // Move length setting
                   RegWriteA( B_MDIS2	, 0x00 ) ;  // Move length setting (20step)
                   
                   RegWriteA( B_TRIG	, 0x01 ) ;  // Trigger Set up
                   
                   while ((UcPIBTrig && 0x01) == 0x01)
                   {
                      RegReadA(B_TRIG, &UcPIBTrig);
                   }

                    RegWriteA( B_LCTR1	, 0x00 ) ;  //  LCTR = 0 
                    RegWriteA( B_LCTR2	, 0x00 ) ;  // 
                    RegWriteA( B_LCTR3	, 0x00 ) ;  //
  
                    //   Excitation  OFF
                    RegWriteA( B_PC2	, 0x03 ) ;  //
  
                   //  PI Set OFF
                   RegWriteA( PISEN	, 0x00 ) ;  //	
	#if( DEBUG==1)
	printk("InsPdxB():	PI Detection Chanel B\n");
	#endif
}



#if ( STM3_MODE_A == 1 ) || ( STM3_MODE_B == 1 )
//=============================================================
void           InsPdxC(unsigned char UcPicPol_t)
{
               unsigned char	UcPIInit ;
               unsigned char	UcPICTrig = 0x01;
              
	      RegWriteA( PISEN	, 0x01 ) ;                     // PIS1 ON
	      //RegWriteA( PISEN	, 0x10 ) ;                     // PIS2 ON
   
                //WitTim(2);  
          
                RegReadA(C_INIT, &UcPIInit);                      //  0x018C
                
               if ( (UcPIInit && 0x10) != 0x10)
               {
                  RegWriteA( C_DIV	, 0x4C ) ;     // Divider  Set up
                  RegWriteA( C_GN	, 0xFF ) ;     // PWM gain Set up
                  RegWriteA( C_MSPD1	, 0xA7 ) ;  // Move Speed Set up
                  RegWriteA( C_MSPD2	, 0x61 ) ;  // Move speed setting(60pps)
                  RegWriteA( C_INIT	, 0x80 ) ;           // <Forced move + normal move> mode 
                  RegWriteA( C_SPDIV	, 0x03 ) ;  // Divider setting
               
                  //  Excitation  ON
                  //  B ch
                   RegWriteA( C_PC1	, 0x82 ) ;  // Pulse generation control 1
                   RegWriteA( C_PC2	, 0x93 ) ;  // Pulse generation control 2 <CW>
                   RegWriteA( C_SGAIN	, 0xFF ) ;  // Gain Set up
               
                   // Move 
                   // B ch
                   RegWriteA( C_MDIS1	, 0xA0 ) ;  // Move length setting
                   RegWriteA( C_MDIS2	, 0x00 ) ;  // Move length setting (20step)
                   
                   RegWriteA( C_TRIG	, 0x01 ) ;  // Trigger Set up
               
                   while ((UcPICTrig && 0x01) == 0x01)
                   {
                      RegReadA(C_TRIG, &UcPICTrig);
                   }
               
               UcPICTrig = 0x01;
               //WitTim(10);  
               
               }// endif
                
                  RegWriteA( C_DIV	, 0x4C ) ;     // Divider  Set up
                  RegWriteA( C_GN	, 0xFF ) ;     // PWM gain Set up
                  RegWriteA( C_MSPD1	, 0xA7 ) ;  // Move Speed Set up
                  RegWriteA( C_MSPD2	, 0x61 ) ;  // Move speed setting(60pps)
                  RegWriteA( C_INIT	, 0x80 ) ;           // <Forced move + normal move> mode 
                  RegWriteA( C_SPDIV	, 0x03 ) ;  // Divider setting
                  
                  //  Excitation  ON
                  //  C ch
                   RegWriteA( C_PC1	, 0x82 ) ;  // Pulse generation control 1
                   RegWriteA( C_PC2	, 0x13 ) ;  // Pulse generation control 2 <CW>
                   RegWriteA( C_SGAIN	, 0xFF ) ;  // Gain Set up
                   
                   // Move 
                   // C ch
                   RegWriteA( C_MDIS1	, 0xA0 ) ;  // Move length setting
                   RegWriteA( C_MDIS2	, 0x00 ) ;  // Move length setting (20step)
                   
                   RegWriteA( C_TRIG	, 0x01 ) ;  // Trigger Set up
                   
                   while ((UcPICTrig && 0x01) == 0x01)
                   {
                      RegReadA(C_TRIG, &UcPICTrig);
                   }

                    RegWriteA( C_LCTR1	, 0x00 ) ;  //  LCTR = 0 
                    RegWriteA( C_LCTR2	, 0x00 ) ;  // 
                    RegWriteA( C_LCTR3	, 0x00 ) ;  //
  
                    //   Excitation  OFF
                    RegWriteA( C_PC2	, 0x03 ) ;  //
  
                   //  PI Set OFF
                   RegWriteA( PISEN	, 0x00 ) ;  //
	#if( DEBUG==1)
	printk("InsPdxC():	PI Detection chanel C\n");
	#endif
}
#endif



#if ( STM3_MODE_A == 1 ) || ( STM3_MODE_B == 1 )
//=============================================================================
void	InsPdx( unsigned char	UcPiaPol , unsigned char UcPibPol , unsigned char UcPicPol )
{
	RegWriteA( MONSEL, 0x00 ) ;									// MON < - MONA
	RegWriteA( MONSELA , 0x57 ) ;								// MONA < -- B_ST0
	RegWriteA( VSY1SEL,   0x11 ) ;								// VSYNC1 <- MONB
	RegWriteA( MONSELB, 0x58 ) ;								// MONB <- B_ST1
	RegWriteA( B_STSEL,  0x76 ) ;								// Monitor setting:CMP,MON

	InsPdxA(UcPiaPol);
	InsPdxB(UcPibPol);
	InsPdxC(UcPicPol);
	#if( DEBUG==1)
	printk(" InsPdx():	STM3_MODE_A  or STM3_MODE_B \n");
	#endif
}
#endif

#if ( STM2_MODE_A == 1 ) || ( STM2_MODE_B == 1 )
void	InsPdx( unsigned char	UcPiaPol , unsigned char UcPibPol )
{
	RegWriteA( MONSEL, 0x00 ) ;									// MON < - MONA
	RegWriteA( MONSELA , 0x57 ) ;								// MONA < -- B_ST0
	RegWriteA( VSY1SEL,   0x11 ) ;								// VSYNC1 <- MONB
	RegWriteA( MONSELB, 0x58 ) ;								// MONB <- B_ST1
	RegWriteA( B_STSEL,  0x76 ) ;								// Monitor setting:CMP,MON       

	InsPdxA(UcPiaPol);
	InsPdxB(UcPibPol);
	#if( DEBUG==1)
	printk("InsPdx():	STM2_MODE_A  or STM2_MODE_B \n");
	#endif
}
#endif


//********************************************************************************
// Function Name 	: InsBpx
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: Move to Base Position
// History			: First edition 						2012.02.08 Y.Kim
//********************************************************************************
void	InsBpx( void )
{
#if ( STM3_MODE_A == 1 ) || ( STM3_MODE_B == 1 )
	unsigned char	UcAchMdr, UcBchMdr, UcCchMdr ;
#endif

#if ( STM2_MODE_A == 1 ) || ( STM2_MODE_B == 1 )
	unsigned char	UcAchMdr, UcBchMdr ;
#endif
	
	WitTim(60);
	RegWriteA( A_LCTR1	, 0x00 ) ;
	RegWriteA( A_LCTR2	, 0x90 ) ;
	RegWriteA( A_LCTR3	, 0x00 ) ;
#if ( STM3_MODE_A == 1 ) || ( STM3_MODE_B == 1 )
	RegWriteA( B_LCTR1	, 0x00 ) ;
	RegWriteA( B_LCTR2	, 0xD8 ) ;
	RegWriteA( B_LCTR3	, 0x00 ) ;
	
	RegWriteA( C_LCTR1	, 0x00 ) ;
	RegWriteA( C_LCTR2	, 0x8C ) ;
	RegWriteA( C_LCTR3	, 0x00 ) ;
#endif

#if ( STM2_MODE_A == 1 ) || ( STM2_MODE_B == 1 )
	RegWriteA( B_LCTR1	, 0x00 ) ;
	RegWriteA( B_LCTR2	, 0xA0 ) ;
	RegWriteA( B_LCTR3	, 0x00 ) ;
#endif
	
	RegReadA( A_PC2	, &UcAchMdr ) ;
	RegReadA( B_PC2	, &UcBchMdr ) ;
#if ( STM3_MODE_A == 1 ) || ( STM3_MODE_B == 1 )
	RegReadA( C_PC2	, &UcCchMdr ) ;
#endif
	
	UcAchMdr	|= 0x80 ;
	UcBchMdr	|= 0x80 ;
#if ( STM3_MODE_A == 1 ) || ( STM3_MODE_B == 1 )
	UcCchMdr	|= 0x80 ;
#endif
	
	RegWriteA( A_PC2	, UcAchMdr ) ;
	RegWriteA( B_PC2	, UcBchMdr ) ;
#if ( STM3_MODE_A == 1 ) || ( STM3_MODE_B == 1 )
	RegWriteA( C_PC2	, UcCchMdr ) ;
#endif
	
	RegWriteA( A_MDIS1	, 0x00 ) ;
	RegWriteA( A_MDIS2	, 0x09 ) ;
#if ( STM3_MODE_A == 1 ) || ( STM3_MODE_B == 1 )
	RegWriteA( B_MDIS1	, 0x80 ) ;
	RegWriteA( B_MDIS2	, 0x0D ) ;
	
	RegWriteA( C_MDIS1	, 0xC0 ) ;
	RegWriteA( C_MDIS2	, 0x08 ) ;
#endif

#if ( STM2_MODE_A == 1 ) || ( STM2_MODE_B == 1 )
	RegWriteA( B_MDIS1	, 0x00 ) ;
	RegWriteA( B_MDIS2	, 0x0A ) ;
#endif
	
	RegWriteA( A_TRIG	, 0x01 ) ;
	RegWriteA( B_TRIG	, 0x01 ) ;
#if ( STM3_MODE_A == 1 ) || ( STM3_MODE_B == 1 )
	RegWriteA( C_TRIG	, 0x01 ) ;
#endif
	
	WitClr( A_TRIG, 0x01, 10 );					// Wait PI Detection
	WitClr( B_TRIG, 0x01, 10 );					// Wait PI Detection
	
#if ( STM3_MODE_A == 1 ) || ( STM3_MODE_B == 1 )
	WitClr( C_TRIG, 0x01, 10 );					// Wait PI Detection
#endif
	
	RegWriteA( A_MDIS1	, (unsigned char)(INI_A_MDIS) );
	RegWriteA( A_MDIS2	, (unsigned char)(INI_A_MDIS >> 8) );
	RegWriteA( B_MDIS1	, (unsigned char)(INI_B_MDIS) );
	RegWriteA( B_MDIS2	, (unsigned char)(INI_B_MDIS >> 8) );
#if ( STM3_MODE_A == 1 ) || ( STM3_MODE_B == 1 )
	RegWriteA( C_MDIS1	, (unsigned char)(INI_C_MDIS) );
	RegWriteA( C_MDIS2	, (unsigned char)(INI_C_MDIS >> 8) );
#endif
	
	RegWriteA( PISEN	, 0x00 ) ;			// PIS1 LED OFF
	#if( DEBUG==1)
	printk("InsBpx():	Move to Base Position Function \n");
	#endif
}



//********************************************************************************
// Function Name 	: IniFil
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: Register Filter Initial Parameter Setting
// History			: First edition 						2009.07.30 Y.Tashita
//********************************************************************************
void	IniFil( void )
{
	unsigned char	i ;

	// MR&HALL-VCM Register Parameter Setting
	i	= 0 ;
	printk(" Config for Register Filter: MR&HALL-VCM Register Parameter Setting ");
	// while( CsMhlReg[ i ].UsRegAdd != 0xFFFF )
	// {
	// 	#if( DEBUG ==1)
	// 		printk("Write RAM %x value %0x\n",CsMhlReg[ i ].UsRegAdd, CsMhlReg[ i ].UcRegDat);
	// 	#endif
	// 	RegWriteA( CsMhlReg[ i ].UsRegAdd, CsMhlReg[ i ].UcRegDat ) ;
	// 	i++ ;
	// }
	RegWriteA(0x0041 , 0x03 ) ;
	RegWriteA(0x0042 , 0x03 ) ;
	RegWriteA(0x0021 , 0x0F ) ;
	RegWriteA(0x0022 , 0x02 ) ;
	//MR&HALL-VCM Filter Parameter Setting
	i	= 0 ;
	printk(" Config for RAM Filter: MR&HALL-VCM Register Parameter Setting ");
	while( arConfigRamFilter[ i ].UsRamAdd != 0xFFFF )
	{
		#if( DEBUG ==1)
		printk("Write RAM %02x value %08x\n",arConfigRamFilter[ i ].UsRamAdd, arConfigRamFilter[ i ].UlRamDat);
		#endif
		RamWrite32A( arConfigRamFilter[ i ].UsRamAdd, arConfigRamFilter[ i ].UlRamDat ) ;
		i++ ;
	}
	#if( DEBUG==1)
	printk("IniFil():	Register Filter Initial Parameter Setting \n");
	#endif
}



//********************************************************************************
// Function Name 	: IngCha
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: IRIS Initialization Function
// History			: First edition 						2012.02.08 Y.Kim
//********************************************************************************
void	IngCha( void )
{
         RegWriteA( GL_DWNSMP2	, 0x05 ) ;
	//RegWriteA( GL_DWNSMP2	, 0x02 ) ;				// DS2 1/3
	RegWriteA( GL_STPMVMOD	, 0x02 );				// StepMove + Filter Mode
	RegWriteA( CM_MES3ADD	, 0x1F );			// using GL step move filter
	RamWrite32A( GLSTMSTP	, 0x3AC80190 );			// StepSize 0.001525f
	
	// Convergence judgement
	RegWriteA( GL_DWNSMP3	, 0x00) ;				//  0x0045  DS3 1/3
	RegWriteA( GL_LEVDIFSEL	, 0x03 ) ;				//  0x0046 LEV,DIFF Convergence judge	
	RegWriteA( GL_LEVDIFTMR	, 0x08 ) ;				// Convergence judgement timer

	
	RamWrite32A( glsjlev	, 0x3C16012C ) ;		// LEV Convergence judge threshold 0.00916f
	RamWrite32A( glsjdif	, 0x3AC80190 ) ;		// LEV Convergence judge threshold 0.001526f
	
	// Output Limiter
	//RegWriteA( GL_LMTMOD	, 0x03 ) ;
	RegWriteA( GL_LMTMOD	, 0x03 ) ;                     // IRIS I filter  changed to plus and minus limiter.
	RamWrite32A( gllmt		, 0x3F800000 ) ;		// GL PWM Output limiter  ( 0x7FFFh );
   // RamWrite32A( gllmt		, 0x3F1A0134 ) ;///		// GL PWM Output limiter 0.625030f ( 0x5000 );
	//RamWrite32A( gllmt		, 0x3F200140 ) ;///		// GL PWM Output limiter 0.625030f ( 0x5000 );
	
	
	RegWriteA( CM_EQON		, 0x01 ) ;				// EQ ON
	#if( DEBUG==1)
	printk("IngCha():	IRIS Initialization Function \n");
	#endif
}



//********************************************************************************
// Function Name 	: IngBpx
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: IRIS Base Position Function
// History			: First edition 						2012.02.08 Y.Kim
//********************************************************************************
void	IngBpx( void )
{
	RamWrite32A( GLSTMZ2	, 0xBF800000 ) ;
	RamWrite32A( GLIZ2		, 0x3F800000 ) ;
	
	//RegWriteA( GL_EQSW	, 0x06	);
	RegWriteA( GL_EQSW	, 0x00	);
	
	WitTim(15);
	RamWrite32A( GLIZ2		, 0x3F800000 ) ;
	WitTim(15);
	//RegWriteA( GL_EQSW	, 0x07	);
	
	RamWrite32A( GLSTMSTP	, 0x3A83126F ) ;		// StepSize 0.00097f
	
	//RamWrite32A( GLSTMTGT	, 0xBF4C0000 ) ;		// BasePosition -0.79687f( 0x9A00 )
	RamWrite32A( GLSTMTGT	, 0xBF49FFD6 ) ;		// BasePosition -0.78906f( 0x9B00 )
	
	RegWriteA( GL_STPMV	, 0x01 );				// Excute StepMove
	WitClr( GL_STPMV, 0x01, 10 );
	#if( DEBUG==1)
	printk("IngBpx():	IRIS Base Position Function \n");
	#endif
}



#if ( STM3_MODE_B == 1 ) || ( STM2_MODE_B == 1 )
//********************************************************************************
// Function Name 	: IniDN
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: Hall-VCM Channel for DN Setting
// History			: First edition 						2019.07.02 Y.Tashita
//********************************************************************************
void	IniDN( void )
{
	//----------------Sinve Wave Setup--------------//
#if ( STM3_MODE_B == 1 )
	RegWriteA(CM_SINTST, 0x08);              // 0059
#endif

#if ( STM2_MODE_B == 1 )
	RegWriteA(CM_SINTST, 0x01);              // 0059
#endif
	RegWriteA(CM_SINGAIN, 0xFF);           // 005A
	RegWriteA(CM_SINFRQ0, 0x25);          // 005B   0.83Hz   2C=>1Hz
	RegWriteA(CM_SINFRQ1, 0x00);          // 005C
	RegWriteA(CM_SINPHSX, 0x20);          // 005D
	
#if ( STM3_MODE_B == 1 )
	RamWrite32A(OFF1Z, 0x3F800000);    // 1234      Offset + 1
#endif

#if ( STM2_MODE_B == 1 )
	RamWrite32A(OFF3Z, 0x3F800000);    // 1236      Offset + 1
#endif
	//-----------------------Coefficient  Set up------------------------//
	RamWrite32A(afatr0, 0x00000000);      // 104F
	RamWrite32A(afbtr0, 0x00000000);      // 105F
#if ( STM3_MODE_B == 1 )
	RamWrite32A(afmg, 0x00000000);      // 1040
	RamWrite32A(afmg2, 0x00000000);      // 1070
#endif
	
	//------------------Analog Monitor-------------------//
	// For debug( If needed )
#if ( STM3_MODE_B == 1 )
	RegWriteA(H_DLYMON1, 0x2E);          // 0084    Delay Monitor 1,  GL2PWM
#endif

#if ( STM2_MODE_B == 1 )
	RegWriteA(H_DLYMON1, 0x6E);          // 0084    Delay Monitor 1,  AF2PWM
#endif
	RegWriteA(VSY1SEL, 0x21);                // 0230, select MONC
	RegWriteA(VSY2SEL, 0x31);                // 0231, select MOND
	RegWriteA(MONSEL, 0x40);                //  0237, MONSEL, select MONE  
	RegWriteA(MONSELC, 0x08);             //  0272, MONSELC, DASDO
	RegWriteA(MONSELD, 0x09);             //  0273, MONSELD, DASCLK
	RegWriteA(MONSELE, 0x0A);             //  0274, MONSELE, DASYNCB
	RegWriteA(DACMONFC, 0x85);          //  DACMONFC, ON, PWMCLK/2, OP=01
	#if( DEBUG==1)
	printk("IniDN():	Hall-VCM Channel for DN Setting \n");
	#endif
}
#endif



//********************************************************************************
// Function Name 	: IniVsc
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: VSYNC Setting
// History			: First edition 						2012.02.08 Y.Kim
//********************************************************************************
void	IniVsc( void )
{
	RegWriteA( VSY1SEL	, 0x00 ) ;
	//RegWriteA( VSY2SEL	, 0x00 ) ;
	RegWriteA( STM1VS	, 0x00 );		// STM1 = VSYNC1
	RegWriteA( STM2VS	, 0x00 );		// STM2 = VSYNC1
	RegWriteA( STM3VS	, 0x00 );		// STM3 = VSYNC1
	RegWriteA( AFVS		, 0x00 );		// HALL-VCM(GL) = VSYNC1

	// VSYNC Function ON
	RegWriteA( A_VSMODE , 0x01 ) ;
	RegWriteA( B_VSMODE , 0x01 ) ;
#if ( STM3_MODE_A == 1 ) || ( STM3_MODE_B == 1 )
	RegWriteA( C_VSMODE , 0x01 ) ;
#endif
	RegWriteA( A_MAGWT , 0x00 ) ;
	RegWriteA( B_MAGWT , 0x00 ) ;
#if ( STM3_MODE_A == 1 ) || ( STM3_MODE_B == 1 )
	RegWriteA( C_MAGWT , 0x00 ) ;
#endif

#if ( STM2_MODE_A == 1 ) || ( STM2_MODE_B == 1 )
	RegWriteA( AF_STPMVEXEMOD	, 0x11 ) ;
#endif
	RegWriteA( GL_STPMVEXEMOD	, 0x11 ) ;
}



//********************************************************************************
// Function Name 	: WitClr
// Retun Value		: void
// Argment Value	: Register Address, Target Bit, Interval Time
// Explanation		: Waiting for operation Function
// History			: First edition 						2011.06.06 
//********************************************************************************
void	WitClr( unsigned short UsRegAdr, unsigned char UcSelBit, unsigned short UsWitTim )
{
	unsigned char	UcRegDat;
	while(1){
		RegReadA( UsRegAdr,	&UcRegDat ) ;
		if( ( UcRegDat & UcSelBit ) == 0x00 ){
			break;
		}
		WitTim( UsWitTim );
	};
}



//********************************************************************************
// Function Name 	: MemClr
// Retun Value		: void
// Argment Value	: Clear Target Pointer, Clear Byte Number
// Explanation		: Memory Clear Function
// History			: First edition 						2011.06.06 
//********************************************************************************
void	MemClr( unsigned char	*NcTgtPtr, unsigned short	UsClrSiz )
{
	unsigned short	UsClrIdx ;

	for ( UsClrIdx = 0 ; UsClrIdx < UsClrSiz ; UsClrIdx++ )
	{
		*NcTgtPtr	= 0 ;
		NcTgtPtr++ ;
	}
}



//********************************************************************************
// Function Name 	: WitTim
// Retun Value		: NON
// Argment Value	: Wait Time(ms)
// Explanation		: Timer Wait Function
// History			: First edition 						2009.07.31 Y.Tashita
//********************************************************************************
void	WitTim( unsigned short	UsWitTim )
{
	unsigned long	UlLopIdx, UlWitCyc ;

	UlWitCyc	= ( unsigned long )( ( float )UsWitTim / NOP_TIME / ( float )12 ) ;

	for( UlLopIdx = 0 ; UlLopIdx < UlWitCyc ; UlLopIdx++ )
	{
		;
	}
}


