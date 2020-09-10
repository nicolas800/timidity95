/*
 *		win95_hlp.cpp
 *		timidity port for win32 'windows' mode
 *		based on timidity (Unix ) from Tuukka Toivonen and
 *		provided under GNU General Public License
 *	contents : helper routine for graphics and window interface 
 *    Nicolas Witczak mai 1996
 */

#ifdef __WIN95__

#include <math.h>

#include "win95hlp.h"
#include "win95_i.h"
#include "win95cfg.h"

#include "config.h"
#include "controls.h"

// extension for file open dialog box
static char pszDefaultExt[]=
	"midi file\0*.mid\0wav file\0*.wav\0config file\0*.cfg\0all files\0*.*\0";

int SatToRange(int iValToSat,int iMin,int iMax)
{
	if(iValToSat < iMin)
		return iMin ;
	else if(iValToSat > iMax )
		return iMax ;
	else
		return iValToSat ;
}

int ClosestIntCelling(double dVal)
{
	int iRet = floor(dVal) ;
	if( (dVal-iRet)  > 0.5 )
		iRet++;
	return iRet;						
}
	 
BOOL ParseCmdLine(char* pszCmdLine,char*** ppszFileList,int* icbCount)
{
	char* psztemp , * pszToken;
	int Index;
	if( pszCmdLine[0] != '\0')
	{
		psztemp = (char*) calloc( strlen(pszCmdLine) + 1 , sizeof(char) );
		strcpy(psztemp,pszCmdLine);
		(*icbCount) = 1 ;		
		strtok( psztemp ," ,\t\n") ;
		while( strtok( 0 ," ,\t\n") )
			(*icbCount)++;
		strcpy(psztemp,pszCmdLine);
		*ppszFileList = (char**)calloc(*icbCount,sizeof(char*));
		
		pszToken = strtok( psztemp ," ,\t\n") ;
		for(Index = 0 ; Index < (*icbCount) ; Index++ )
		{
			(*ppszFileList)[Index] = (char*) calloc( strlen(pszToken) + 1 
				 , sizeof(char) );
			strcpy((*ppszFileList)[Index] , pszToken ) ;
			pszToken =  strtok( 0 ," ,\t\n");
		}
		return TRUE ;
	}
	return FALSE ;		
}

void DisplayMessage(char* pszToDisplay)
{
	MessageBox(hMainWnd,pszToDisplay,TIMID_APP_NAME,MB_ICONSTOP);
}

BOOL DlgFileOpen(char* pszTitle,char* pszFileName,int sizeString)
{
	OPENFILENAME ofn;
	BOOL bIsOk;
	
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize	= sizeof(OPENFILENAME);
	ofn.hwndOwner		= 0;
	ofn.hInstance		= hInst ;
	ofn.lpstrFilter	= pszDefaultExt;		
	ofn.lpstrCustomFilter  = 0;
	ofn.nMaxCustFilter	  = 1 ;
	ofn.nFilterIndex	= 1 ;
	ofn.lpstrFile		= pszFileName;
	ofn.nMaxFile		= sizeString;
	ofn.lpstrFileTitle	= 0;
	ofn.nMaxFileTitle 	= strlen(pszTitle);
	ofn.lpstrInitialDir	= 0;
	ofn.lpstrTitle		= pszTitle;
	ofn.Flags			= OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt	= 0;
	ofn.lCustData		= 0;
	ofn.lpfnHook		= 0;
	ofn.lpTemplateName= 0;
	bIsOk=GetOpenFileName(&ofn);

	return bIsOk ;
}

void DlgMultiFileOpen(char*** ppszFileList,int* icbCount)
{
#define cbszFileName 16536
	OPENFILENAME ofn;
	BOOL bIsOk;
	int iIndex;
	char* pFileName  ;
	pFileName = (char*)calloc(cbszFileName, sizeof(char)) ;
	pFileName[0] = '\0' ; 
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize	= sizeof(OPENFILENAME);
	ofn.hwndOwner		= 0;
	ofn.hInstance		= hInst ;
	ofn.lpstrFilter	= pszDefaultExt;		
	ofn.lpstrCustomFilter  = 0;
	ofn.nMaxCustFilter	  = 1 ;
	ofn.nFilterIndex	= 1 ;
	ofn.lpstrFile		= pFileName;
	ofn.nMaxFile		= cbszFileName;
	ofn.lpstrFileTitle	= 0;
	ofn.nMaxFileTitle 	= 0;
	ofn.lpstrInitialDir	= 0;
	ofn.lpstrTitle		= 0;
	ofn.Flags			= OFN_FILEMUSTEXIST  | OFN_PATHMUSTEXIST 
		| OFN_ALLOWMULTISELECT | OFN_EXPLORER;
	ofn.lpstrDefExt	= 0;
	ofn.lCustData		= 0;
	ofn.lpfnHook		= 0;
	ofn.lpTemplateName= 0;
	
	if(!GetOpenFileName(&ofn))
		return;

	// count files
	for(iIndex=1 , (*icbCount)=-1 ;(pFileName[iIndex-1] != '\0') || (pFileName[iIndex] != '\0') ;iIndex++ )
	{
		if(pFileName[iIndex] == '\0')
			(*icbCount)++;
	}
	// extract file list
	
	if( (*icbCount) > 0 )
	{ 
		(*ppszFileList) = (char**)calloc(*icbCount, sizeof(char*) );
		for(iIndex=0;iIndex<(*icbCount);iIndex++)
		{
			while( ( *(ofn.lpstrFile++) )!= '\0')
			{ (void)0 ; }	
			(*ppszFileList)[iIndex] = (char*)calloc(strlen(ofn.lpstrFile) 
				+ strlen(pFileName) + 2 , sizeof(char) ) ;
			strcpy((*ppszFileList)[iIndex],pFileName);
			strcat((*ppszFileList)[iIndex],"\\");
			strcat( (*ppszFileList)[iIndex] , ofn.lpstrFile );
		}
	}
	else
	{
		(*icbCount) = 1 ;
		(*ppszFileList) = (char**)calloc(*icbCount,sizeof(char*));
		(*ppszFileList)[0] = (char*)calloc( strlen(pFileName) + 1 , sizeof(char) );
		strcpy((*ppszFileList)[0],pFileName);
	}
	free(pFileName);
	return ;	
}

void FreeFileList(char** ppszFileList,int icbCount)
{
	int iIndex;
	for(iIndex=0;iIndex < icbCount;iIndex++)
		free(ppszFileList[iIndex]); 
	free(ppszFileList);
}

void CenterWindow(HWND hwnd)
{
	RECT sMainScreen;
	RECT sThisWindow;
	int x,y,cx,cy;
	GetWindowRect(GetDesktopWindow(),&sMainScreen);
	GetWindowRect(hwnd,&sThisWindow);
	cx=sThisWindow.right-sThisWindow.left;
	cy=sThisWindow.bottom-sThisWindow.top;
	x=sMainScreen.right/2-cx/2;
	y=sMainScreen.bottom/2-cy/2;
	MoveWindow(hwnd,x,y,cx,cy,TRUE);
}

void SetMidiTitle()
{
	static char pszMainWndTitle[64]  ;
// create title char string		
	if( (cbFileInPlayList==0) || (ppszPlayList[iCurPlayListIndex] == '\0') )
		strcpy(pszMainWndTitle,TIMIDDEFTITLE);
	else
	{	
		char* pszTemp ;			
		pszTemp = strrchr(ppszPlayList[iCurPlayListIndex],'\\') ; 
		if( pszTemp != 0)
			strcpy(pszMainWndTitle,pszTemp+1);
		else
			strcpy(pszMainWndTitle,ppszPlayList[iCurPlayListIndex]);	
	}
// update title in caption
	SetWindowText(hMainWnd,pszMainWndTitle);		

}

void Draw3dHLine(HDC hDC,int ixStart,int ixEnd,int iyPos)
{
	HPEN hOldPen , hDarkPen , hLightPen ,hBlackPen;
	hDarkPen = CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNSHADOW));
	hLightPen = CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNHIGHLIGHT));
	hBlackPen = (HPEN)GetStockObject(BLACK_PEN);

	hOldPen = SelectObject(hDC,hDarkPen);		
	MoveToEx(hDC, ixStart , iyPos ,0);
	LineTo(hDC,ixEnd,iyPos);
	SelectObject(hDC,hLightPen);
	MoveToEx(hDC, ixStart+1 , iyPos+1 ,0);
	LineTo(hDC,ixEnd+1,iyPos+1);
	
	SelectObject(hDC,hOldPen);
	DeleteObject(hDarkPen) ;
	DeleteObject(hLightPen);
		
}

void Draw3dVLine(HDC hDC,int iyStart,int iyEnd,int ixPos)
{
	HPEN hOldPen , hDarkPen , hLightPen ,hBlackPen;
	hDarkPen = CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNSHADOW));
	hLightPen = CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNHIGHLIGHT));
	hBlackPen = (HPEN)GetStockObject(BLACK_PEN);
	hOldPen = SelectObject(hDC,hDarkPen);

	hOldPen = SelectObject(hDC,hDarkPen);		
	MoveToEx(hDC, ixPos , iyStart ,0);
	LineTo(hDC,ixPos,iyEnd);
	SelectObject(hDC,hLightPen);
	MoveToEx(hDC,ixPos +1 ,iyStart +1 ,0);
	LineTo(hDC,ixPos +1 ,iyEnd +1);
	
	SelectObject(hDC,hOldPen);
	DeleteObject(hDarkPen) ;
	DeleteObject(hLightPen);
		
}

void PumpWindowMessages()
{
	MSG msg ;
	while( PeekMessage(&msg,NULL,0,0,PM_NOREMOVE) )
	{
		if( ! GetMessage(&msg,NULL,0,0) )
			return ;
		HandleFastSearch(msg);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}	
}



// initialyse once os info structure
static OSVERSIONINFO sSysInfo ;
static BOOL bIsThreadEnable ;

void InitialyseOSVerStruct()
{
	sSysInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx( &sSysInfo );
	bIsThreadEnable = ( sSysInfo.dwPlatformId != VER_PLATFORM_WIN32s ); 
}


BOOL IsThreadEnable()
{	
	return FALSE ;
//	return bIsThreadEnable ; 
}

int fprintf( FILE *stream, const char *format ,...)
{
	va_list argStart;
	va_start(argStart, format);
	if( (stream == stderr) || (stream == stdout) )
		vcmsg(CMSG_INFO,VERB_VERBOSE,format,argStart);
	else
		vfprintf(stream,format,argStart);
	va_end(argStart);
	return 0;
}

#endif /* __WIN95__ */
