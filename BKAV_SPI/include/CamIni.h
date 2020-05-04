//********************************************************************************
//
//		<< LC898201 Evaluation Soft >>
//	    Program Name	: CamIni.c
//		Design			: Y.Sha
//		History			: First edition	       2017.10.17 
//********************************************************************************

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/delay.h>
//**************************
//	Define
//**************************
#define		USE_OSC					// In case of use the OSC, Enable it.
									// In case of use a crystal, Disable it.

#if ( STM3_MODE_A == 1 ) || ( STM3_MODE_B == 1 )
	#define	PI_A_POL	0x10		// CW Direction Move : PI Signal is Low -> High
	#define	PI_B_POL	0x10		// CW Direction Move : PI Signal is Low -> High
    #define	PI_C_POL	0x00		// CW Direction Move : PI Signal is High -> Low
#endif

#if ( STM2_MODE_A == 1 ) || ( STM2_MODE_B == 1 )
	#define	PI_A_POL	0x10		// CW Direction Move : PI Signal is Low -> High
	#define	PI_B_POL	0x00		// CW Direction Move : PI Signal is High -> Low
#endif

//Analog Setting
#define	BIAS_CURRENT_GL			0x02		// BIAS Current   0x00 : 0.5mA / 0x01 : 1.0mA / 0x02 : 1.5mA / 0x03 : 2.0mA
#define	BIAS_GL					0x80		// BIAS for 	GL(IRIS)
#define	OFFSET_GL				0x80		// OFFSET for	GL(IRIS)
#define	OPAMP_GL				0x00		// Iris VGA Gain 0x25

#if ( STM3_MODE_A == 1 ) || ( STM3_MODE_B == 1 )
	#define	INI_A_PC2	0x19			// 512u-step Mode
	#define	INI_B_PC2	0x19			// 512u-step Mode
	
	#define	INI_C_PC2	0x19			// 512u-step Mode

	#define	INI_A_MSPD	0x00BA			// 1000pps Move Speed For Initialization
	#define	INI_B_MSPD	0x00BA			// 1000pps Move Speed For Initialization
	#define	INI_C_MSPD	0x0176			//  500pps Move Speed For Initialization

	#define	INI_A_MDIS	0x0100
	#define	INI_B_MDIS	0x0100
	#define	INI_C_MDIS	0x1000
#endif

#if ( STM2_MODE_A == 1 ) || ( STM2_MODE_B == 1 )

	
	#define	INI_A_PC2	0x19  //Zoom			//set up resolution chanel A
	#define	INI_B_PC2	0x19			//set up resolution chanel B
	
	#define	INI_A_MSPD	0x00BA			// 1000pps Move Speed For Initialization
	#define	INI_B_MSPD	0x0176			//  500pps Move Speed For Initialization
	
	#define	INI_A_MDIS	0x0100
	#define	INI_B_MDIS	0x1000
#endif
/*
1:	1-2phase(saturation output)
2:	2 phase(PWM output)
3:	1-2 phase(PWM output)
4:	16μ step
5:	32μ step
6:	64μ step
7:	128μ step
8:	256μ step
9:	512μ step
10:	1024μ step
11:	2 phase(PWM full torque output
12:	1-2 phase(PWM full torque output
*/
//**************************
//	Local Function Prottype	
//**************************
void	IniCmd( void ) ;		// Command Execute Process Initial
void	IniFil( void ) ;		// Initialize Filter
void	IniRst( void ) ;		// Software Reset
void	IniOsc( void ) ;		// Oscilator Setting
void	IniClk( void ) ;		// Clock Setting
void	IniCha( void ) ;		// Driver-Channel Setting
void	IniEqc( void ) ;		// Equalizer Clear
void	IniAna( void ) ;		// Analog Circuit Setting
void	IniMon( void ) ;		// Monitor Setting
void	IniPWM( void ) ;		// PWM Setting

void	InsCha(	void ) ;		// Stepping Channel Setting
void	InsPds(	void ) ;		// PI Detection Setting

#if ( STM3_MODE_A == 1 ) || ( STM3_MODE_B == 1 )
	void	InsPdx(	unsigned char, unsigned char, unsigned char ) ;		// Excute PI Detection
	void           InsPdxC (unsigned char);
#endif

#if ( STM2_MODE_A == 1 ) || ( STM2_MODE_B == 1 )
	void	InsPdx(	unsigned char, unsigned char ) ;		// Excute PI Detection
#endif
	void           InsPdxA (unsigned char);
	void           InsPdxB (unsigned char);

void	InsBpx(	void ) ;		// Base Position

void	IngCha(	void ) ;		// HALL-VCM Channel Setting
void	IngBpx(	void ) ;		// Base Position

#if ( STM3_MODE_B == 1 ) || ( STM2_MODE_B == 1 )
void	IniDN( void ) ;			// DN Initialization for Hall-VCM Channel
#endif

void	IniVsc(	void ) ;		// VSYNC Setting
