#ifndef CONTROLLENS_H__
#define CONTROLLENS_H__
#include"control.h"
#include "LC898201Typedef.h"


#define		IRIS_1_0  10
#define		IRIS_1_4  14
#define 	IRIS_2_8	28
#define		IRIS_4_0   40
#define		IRIS_5_4   54
#define		IRIS_8_0   80
#define		IRIS_11_0  11
#define		IRIS_16_0  16
#define		IRIS_22_0  22


INT GetLibLC898201Infomation(PCHAR pVersion, PCHAR pVersionSize, PCHAR pInfomation, PCHAR pInfoSize);

INT InitDriverLens(BYTE nModel);
/*-------------------------------------------------------------------------
* Function: int InitDriverLens(unsigned char choseLens)
* Purpose: Int driver Lens
* Parameter: CAMIP Model							
* Return: 1 or 0
* Comment: Return 1 if OK
            Return 0 if Error
---------------------------------------------------------------------------*/

INT MoveIris(int fSize);


INT MoveFocus(ULONG speed,ULONG iSizeStep);

/*-------------------------------------------------------------------------
* Function: int MoveFocus(unsigned short speed,unsigned short iSizeStep)
* Purpose: Move Focus step motor to control focus
* Parameter: 	unsigned short speed: 0-100
				unsigned short iSizeStep: 0- 1000
* Return:  1 or 0
* Comment: Return 1 if OK
			Return 0 if Error
---------------------------------------------------------------------------*/
INT GetFocusIndex(VOID);

INT MoveZoom(ULONG speed,ULONG iSizeStep);
/*-------------------------------------------------------------------------
* Function: int MoveZoom(unsigned short speed,unsigned short iSizeStep)
* Purpose: Move Zoom step motor to control Zoom
* Parameter: 	unsigned short speed: 0-100
				unsigned short iSizeStep: 0- 1000
* Return:  1 or 0
* Comment: Return 1 if OK
			Return 0 if Error
---------------------------------------------------------------------------*/
INT GetZoomIndex(VOID );
/*-------------------------------------------------------------------------
* Function: unsigned int GetZoomIndex()
* Purpose: get index of Zoom
* Parameter: None
* Return: index of zoom from 0 to 1000
* Comment:
---------------------------------------------------------------------------*/
INT OpenIRCut(VOID );
/*-------------------------------------------------------------------------
* Function: INT OpenIRCut(VOID )
* Purpose: open ir cut
* Parameter: 	none
* Return: 1  or 0
* Comment: 	Return 1 if OK
			Return 0 if Error
---------------------------------------------------------------------------*/
INT CloseIRCut(VOID);
/*-------------------------------------------------------------------------
* Function: INT CloseIRCut(VOID )
* Purpose: open ir cut
* Parameter: 	none
* Return: 1  or 0
* Comment: 	Return 1 if OK
			Return 0 if Error
---------------------------------------------------------------------------*/

#endif
