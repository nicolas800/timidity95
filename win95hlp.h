/*
 *		win95_hlp.h
 *		timidity port for win32 'windows' mode
 *		based on timidity (Unix ) from Tuukka Toivonen and
 *		provided under GNU General Public License
 *	  contents : helper routine for graphics and window interface
 *    header file
 *    Nicolas Witczak mai 1996
 */



#ifndef WIN95HLP_H
	#define WIN95HLP_H

#define WINVER 0x0400

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <malloc.h>
#include <stdarg.h>
#include <assert.h>

#ifdef RC_NONE
	#undef RC_NONE
#endif

// defines a resonably large string buffer
#define szStringBuff 1024
typedef char char_buff[szStringBuff] ;

/** return iValToSat insuring iValToSat <- [ iMin , iMax [ */
int SatToRange(int iValToSat,int iMin,int iMax);

/** return a value iVal such as inf( | iVal - dVal | ) */
int ClosestIntCelling(double dVal);

/*parse cmd line fill play list with midi files and return 
true if files to play */
BOOL ParseCmdLine(char* pszCmdLine,char*** ppszFileList,int* icbCount);


// windowsx.h like message cracker 
#define HANDLE_SMPCMD(hookId,fn)	\
	case hookId:					\
		fn();						\
		break

#define HANDLE_USERCMD(hookId,hwndCtl,codeNotify,fn)	\
	case hookId:					\
		fn(hwndCtl,codeNotify);	\
		break

void DisplayMessage(char* pszToDisplay);

/*
 * DlgFileOpen function
 *	input : pszFileName : a buffer to be filled with
 * a file name
 * input : sizeString
 * output : return success or failure
 * descr  : return a file name 
 */
BOOL DlgFileOpen(char* pszTitle,char* pszFileName,int sizeString);

void DlgMultiFileOpen(char*** ppszFileList,int* icbCount);
void FreeFileList(char** ppszFileList,int icbCount);

/*
 * CenterWindow function
 * descr  : center a window in the middle of the screen 
 * input : hwnd : the window handle to center
 * output : none
 */

void CenterWindow(HWND hwnd);
/*
 * SetExTitle function
 * descr  : append pszNewExt file name to title 
 */

void SetMidiTitle();
/*
 * fct PumpWindowMessages : pump all window messages in message queue
 *		and return without yelding control
 */
void PumpWindowMessages();

/*
 * fct InitialyseOSVerStruct : called once at the beginning of the program
 */
void InitialyseOSVerStruct();
	
/*
 * fct : IsThreadEnable : indicate wether thread are alvaliable or not
 */
BOOL IsThreadEnable();

/*
 * Draw3dHLine , Draw3dVLine function
 * descr  : draw 3d style horlizontal and vertical 
 * lines using hDC device context and standart windows colors
 */

void Draw3dHLine(HDC hDC,int ixStart,int ixEnd,int iyPos);

void Draw3dVLine(HDC hDC,int iyStart,int iyEnd,int ixPos);

#ifndef max
	#define max(x,y)	\
		( ((x)>(y)) ? (x) : (y) ) 
#endif //max

/*
 *	 this overrides stdio 's fprintf 
 *	 windows doesn't know stdout nor stderr so we redirect 
 *  these output to our edit window
 *  otherwise we call the  'real' fprintf
 */  
int fprintf( FILE *stream, const char *format ,...);  

#endif // WIN95HLP_H
