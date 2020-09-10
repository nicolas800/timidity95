/*
 *		guimain.cpp
 *		timidity port for win32 'windows' mode
 *		based on timidity (Unix ) from Tuukka Toivonen and
 *		provided under GNU General Public License
 *	contents : winmain entry point, widget management
 *    Nicolas Witczak mai 1996
 */


#ifdef __WIN95__

#include "win95hlp.h"
#include "win95_i.h"
#include "win95cfg.h"
#include "win95dlg.h"
#include "win95res.h"

#include "config.h"
#include "controls.h"
#include "instrum.h"
#include "playmidi.h"
#include "output.h"

///////////////////////////////////////////////////////////////////
// exported variables

HINSTANCE hInst ;
HWND hMainWnd = 0  ;		
HWND hVolumeTkBar	= 0;
HWND hBarButton  = 0 ;

int iMidiFileTotalMin = 0;
int iMidiFileTotalSec = 0;
int iMidiFileCurMin = 0 ;
int iMidiFileCurSec = 0;
int iVolume = 0;
int iDeltaVolume = 0 ; 
int iGUIWhishedState = RC_NONE ; 
char** ppszPlayList;
int cbFileInPlayList = 0 ;
int iCurPlayListIndex = 0;

BOOL bIsPlaying = FALSE ;
HANDLE hPlayThread = 0 ;

///////////////////////////////////////////////////////////////////
// local variables

/** indicate number of item in output menu */
int iDMMaxDynMenu ; 	



///////////////////////////////////////////////////////////////////
// function implementation


void InitMainWnd(int nCmdShow)
{
   	WNDCLASS wndclass ;
	HDC hDC;

// register main window class
	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = MainWinProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInst ;
	wndclass.hIcon         = LoadIcon(hInst,MAKEINTRESOURCE(TIMIDITY_ICON)) ;
	wndclass.hCursor       = LoadCursor(0,IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wndclass.lpszMenuName  = MAKEINTRESOURCE(TIMIDITY_MENU) ;
	wndclass.lpszClassName =  TIMID_WNDCLASS ;
	RegisterClass(&wndclass);

// create main window  and ctrls
	hMainWnd = CreateWindowEx(WS_EX_DLGMODALFRAME , TIMID_WNDCLASS ,0,
		WS_OVERLAPPED  | WS_MINIMIZEBOX | WS_SYSMENU | WS_CLIPCHILDREN ,
		CW_USEDEFAULT,0,MAINWND_XSIZE,MAINWND_YSIZE,0,0,hInst,0);
	SetMidiTitle();

	DragAcceptFiles(hMainWnd,TRUE);
	hBarButton = InitButtons( hMainWnd ) ; 
	hVolumeTkBar = InitVolTrkBar( hMainWnd ) ;
	
	hDC = GetDC( hMainWnd );		
	PaintDecoration( hMainWnd , hDC) ; 
	PaintText( hMainWnd , hDC) ;
	ReleaseDC(hMainWnd,hDC);		
	
	ShowWindow(hMainWnd,nCmdShow);	
}

void InitOutputMenu()
{
	HMENU hOutputMenu ;
	int iDx = 0 ;
	PlayMode **ppIterOutput = play_mode_list;
	
	hOutputMenu = GetSubMenu( GetMenu( hMainWnd ) , 2 ) ;		
	/* remove the unused, but needed for rc file, void menu item */
	DeleteMenu(hOutputMenu, IDM_VOID , MF_BYCOMMAND );
	while( *ppIterOutput != 0 )
	{
		if( strcmp((*ppIterOutput)->id_name , play_mode->id_name ) == 0 )  
			/* the active output here */
			AppendMenu(hOutputMenu, MF_CHECKED , IDM_DYNMENU + iDx 
				, (*ppIterOutput)->id_name ) ;
		else
			AppendMenu(hOutputMenu, MF_ENABLED , IDM_DYNMENU + iDx  
				, (*ppIterOutput)->id_name ) ;
		iDx++ ;
		ppIterOutput++ ;
	}
	iDMMaxDynMenu = IDM_DYNMENU + iDx ;
}


HWND InitVolTrkBar(HWND hwndParent)
{
	HWND hRetTrkbar ;

	hRetTrkbar = CreateWindowEx(0, TRACKBAR_CLASS ,"Vol ",
		WS_CHILD  | TBS_AUTOTICKS | TBS_ENABLESELRANGE | TBS_VERT,
		MAINWND_XSIZE - ( TRKBAR_TEXT_WIDTH + VOLTRKBAR_WIDTH) , 1           
		, VOLTRKBAR_WIDTH , VOLTRKBAR_HEIGHT ,
		hwndParent,(HMENU)IDC_VOLTRCBAR,hInst,0 ); 	
	
	ShowWindow(hRetTrkbar,SW_SHOWNORMAL);
	SendMessage(hRetTrkbar, TBM_SETRANGE, (WPARAM) TRUE,                 
    (LPARAM) MAKELONG(VOLTRKBAR_MIN,VOLTRKBAR_MAX ));   
	
	SendMessage(hRetTrkbar, TBM_SETTICFREQ , 20 , 1 );  		
	
	SetTrkBarVol(hRetTrkbar,100);
	
	return hRetTrkbar ;
}


void SetTrkBarVol(HWND hwndTrkBar,int iNewVol)
{
	iNewVol = VOLTRKSEL_MAX - iNewVol ;
	if( iNewVol < (VOLTRKSEL_MIN)) iNewVol = VOLTRKSEL_MIN;
	if( iNewVol > (VOLTRKSEL_MAX)) iNewVol = VOLTRKSEL_MAX;
	SendMessage(hwndTrkBar, TBM_SETPOS, (WPARAM) TRUE,
    (LPARAM) iNewVol);
}

int GetTrkBarVol(HWND hwndTrkBar)
{
	int retVol ;
	retVol = SendMessage(hwndTrkBar, TBM_GETPOS, (WPARAM)0,(LPARAM)0);
	retVol = VOLTRKSEL_MAX - retVol ;
	if(retVol<1)
		retVol = 1 ;
	return retVol ;
}
 
HWND InitButtons(HWND hwndParent)
{
	HWND hwndTB; 
	TBADDBITMAP tbab;
	TBBUTTON tbb[NUM_BUTTONS]; 
	int idXBBar ;

	SetWindowPos(hMainWnd,0,0,0,MAINWND_XSIZE - ( TRKBAR_TEXT_WIDTH + VOLTRKBAR_WIDTH+SIZE_SEP) + 3
		,MAINWND_YSIZE,SWP_NOMOVE|SWP_NOZORDER);
 
	hwndTB = CreateWindowEx(0, TOOLBARCLASSNAME, (LPSTR) 0, 
		WS_CHILD | CCS_ADJUSTABLE, SIZE_SEP , SIZE_SEP , 0, 0, hwndParent,
		(HMENU)IDC_BUTTONBAR ,hInst , NULL); 

	// Send the TB_BUTTONSTRUCTSIZE message, which is required for 
	// backward compatibility. 
	SendMessage(hwndTB, TB_BUTTONSTRUCTSIZE, 
		(WPARAM) sizeof(TBBUTTON), 0); 

	SendMessage(hwndTB, TB_SETBUTTONSIZE, (WPARAM) 0, 
		(LPARAM) MAKELONG(DXBUTTON, DYBUTTON)); 

	 // Add the bitmap containing button images to the toolbar. 
	 tbab.hInst = hInst; 
	 tbab.nID   = IDB_BUTTON; 
	 SendMessage(hwndTB, TB_ADDBITMAP, (WPARAM) NUM_BUTTONS, 
		  (WPARAM) &tbab); 


	// Fill the TBBUTTON array with button information, and add the 
	// buttons to the toolbar. 
	for(idXBBar=0;idXBBar < NUM_BUTTONS;idXBBar++)
	{
		tbb[idXBBar].iBitmap = idXBBar; 
		tbb[idXBBar].idCommand = IDC_LASTTRACK + idXBBar; 
		tbb[idXBBar].fsState = TBSTATE_ENABLED; 
		tbb[idXBBar].fsStyle = TBSTYLE_BUTTON; 
		tbb[idXBBar].dwData = 0; 
		tbb[idXBBar].iString = 0; 
	}
	 tbb[IDC_PAUSE-IDC_LASTTRACK].fsStyle =   TBSTYLE_CHECK ;

	SendMessage(hwndTB, TB_ADDBUTTONS, (WPARAM) NUM_BUTTONS, 
		(LPARAM) (LPTBBUTTON) &tbb);  
	ShowWindow(hwndTB, SW_SHOW); 
	SendMessage(hwndTB,TB_AUTOSIZE , 0, 0); 
	
	SetWindowPos(hMainWnd,0,0,0,MAINWND_XSIZE,MAINWND_YSIZE,SWP_NOMOVE|SWP_NOZORDER);
	return hwndTB; 		
}

/* rq: HANDLE_MSG like macros are defined in windowsx.h a standart ms include*/
LRESULT CALLBACK MainWinProc(HWND hwnd, UINT uMess, WPARAM wParam, LPARAM lParam)
{
	switch (uMess)
	{
		HANDLE_MSG(hwnd,WM_COMMAND,MainCmdProc);
		HANDLE_MSG(hwnd,WM_CLOSE,DestroyWindow);
		HANDLE_MSG(hwnd,WM_DESTROY,OnDestroy);
		HANDLE_MSG(hwnd,WM_SIZE,OnMainWndSize);
		HANDLE_MSG(hwnd,WM_PAINT,OnPaintMainWnd);		
		HANDLE_MSG(hwnd,WM_VSCROLL,OnTrackBarMove);
		HANDLE_MSG(hwnd,WM_DROPFILES,OnDropFilesMainWnd);		
		default:
			return DefWindowProc(hwnd,uMess,wParam,lParam) ;
	}
}
		 
void OnDestroy(HWND hwnd)
{	
	DestroyOutputWnd();
	if( bIsPlaying )
	{
		iGUIWhishedState = RC_QUIT ;
		WaitForSingleObject(hPlayThread,INFINITE); // wait for thread playing to terminate
	}
	PostQuitMessage(0) ;
}

void OnExitApp()
{ 

	DestroyWindow(hMainWnd); 
}	

void PaintText(HWND hwnd,HDC hDC)
{
	HFONT hDefFont, OldFont ;
	RECT rect;
	char pszTmpBuf[64]  ;

	GetClientRect(hwnd,&rect);
	hDefFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

	SetBkColor(hDC,GetSysColor(COLOR_BTNFACE));														
	SetTextAlign(hDC,TA_LEFT | TA_BASELINE);
	OldFont = SelectObject(hDC,hDefFont);

	strcpy(pszTmpBuf,"0dB");
	TextOut(hDC,MAINWND_XSIZE - TRKBAR_TEXT_WIDTH,TRKBAR_TEXT_Y
		,pszTmpBuf,3);
			
	if(cbFileInPlayList != 0)
	sprintf(pszTmpBuf,"Track %2d of %2d   Playing %2d:%02d   Total %2d:%02d  "
		,iCurPlayListIndex+1,cbFileInPlayList
		,iMidiFileCurMin,iMidiFileCurSec,
		iMidiFileTotalMin,iMidiFileTotalSec);
	else
		sprintf(pszTmpBuf,"no files in play list                                ");
	TextOut(hDC,TIME_TEXT_X,TIME_TEXT_Y,pszTmpBuf,strlen(pszTmpBuf));
	
	SelectObject(hDC,OldFont) ;
}

void PaintDecoration(HWND hwnd,HDC hDC)
{
	RECT rect;

	GetClientRect(hwnd,&rect);
	Draw3dVLine(hDC,0,MAINWND_YSIZE-SIZE_SEP
		,MAINWND_XSIZE - ( TRKBAR_TEXT_WIDTH + VOLTRKBAR_WIDTH+SIZE_SEP));				
	Draw3dHLine(hDC,0
		,MAINWND_XSIZE - ( TRKBAR_TEXT_WIDTH + VOLTRKBAR_WIDTH+SIZE_SEP)
		,TIME_TEXT_Y - SIZE_CHAR);
}

void OnPaintMainWnd(HWND hwnd)
{		
	HDC hDC ;
	PAINTSTRUCT ps ;
	hDC = BeginPaint( hwnd , &ps );
	PaintText(hwnd , hDC);
	PaintDecoration(hwnd, hDC ) ;
	EndPaint( hwnd , &ps );
	FORWARD_WM_PAINT(hwnd,DefWindowProc);
}

void OnMainWndSize(HWND hwnd, UINT state, int cx, int cy)
{
	if( state == SIZE_MINIMIZED)
		ShowOutput( FALSE );
	if( ((state == SIZE_RESTORED ) || ( state == SIZE_MAXIMIZED)) & bShowOutput)
		ShowOutput( TRUE );
	FORWARD_WM_SIZE(hwnd, state, cx, cy, DefWindowProc);
}

void OnDropFilesMainWnd(HWND hwnd, HDROP hdrop)
{
	UINT iFile;		
	int iSizeOfPath ;
	HDC hDC;

	OnStop();
	FreeFileList(ppszPlayList,cbFileInPlayList);		
	cbFileInPlayList = DragQueryFile(hdrop,0xFFFFFFFF,0,0);
		/* query number of file to drag accept*/
	ppszPlayList = calloc(cbFileInPlayList,sizeof(char*));
	
	for(iFile = 0;iFile < cbFileInPlayList;iFile++)
	{
		iSizeOfPath	= DragQueryFile(hdrop,iFile,0,0) + 1 ;
		ppszPlayList[iFile] = calloc(iSizeOfPath,1);
		DragQueryFile(hdrop,iFile,ppszPlayList[iFile],iSizeOfPath);
	}
	DragFinish(hdrop);
	iCurPlayListIndex = 0 ;		
	SetMidiTitle();
	hDC = GetDC( hMainWnd );		
	PaintText( hMainWnd , hDC );
	ReleaseDC( hMainWnd , hDC );
	OnPlay();
}

HBRUSH OnTrackBarCtlColor(HWND hwnd, HDC hdc, HWND hwndChild, int type)
{
	SetBkColor(hdc,COLOR_BTNFACE);
	return (HBRUSH)(COLOR_BTNFACE + 1);
}

void MainCmdProc(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id)
	{
		HANDLE_SMPCMD(IDM_QUIT,OnExitApp);
		HANDLE_SMPCMD(IDM_FILEOPEN,OnOpen);
		HANDLE_SMPCMD(IDC_PLAY,OnPlay);
		HANDLE_SMPCMD(IDC_STOP,OnStop);
		HANDLE_SMPCMD(IDC_PAUSE,OnPause);
		HANDLE_SMPCMD(IDC_LASTTRACK,OnPrevTrack);
		HANDLE_SMPCMD(IDC_NEXTTRACK,OnNextTrack);
		HANDLE_SMPCMD(IDM_ABOUT,OnAboutDlg);
		HANDLE_SMPCMD(IDM_CFGGEN,OnGeneralConf);
		HANDLE_SMPCMD(IDM_CFGOUTPUT,OnOutputConf);
		HANDLE_SMPCMD(IDM_CFGMIDI,OnMidiConf);	
	}
	if( ( id >= IDM_DYNMENU ) && ( id < iDMMaxDynMenu )
	   && ( codeNotify == 0) && (!bIsPlaying) )
		   OnChangeOutputMode(id);	
}

BOOL IsForwardBtnPressed()
{
	POINT sScreenPoint ;
	if( (GetAsyncKeyState(VK_LBUTTON) >> 16 )	!= 0)	// get button state
	{
		GetCursorPos(&sScreenPoint);
		ScreenToClient(hBarButton,&sScreenPoint);
		if(( sScreenPoint.x > (DXBUTTON*(IDC_FFORWRD-IDC_LASTTRACK)) )  
			&& ( sScreenPoint.x < (DXBUTTON*(1+IDC_FFORWRD-IDC_LASTTRACK)) ) 
			&& ( sScreenPoint.y >0 )  && ( sScreenPoint.y < DYBUTTON ))
				return TRUE ; // check if mouse cursor is in button 
		else
			return FALSE ;
	}
	else
		return FALSE; 	
}

BOOL IsRewindBtnPressed()
{
	POINT sScreenPoint ;
	if( (GetAsyncKeyState(VK_LBUTTON) >> 16 )	!= 0)
	{
		GetCursorPos(&sScreenPoint);
		ScreenToClient(hBarButton,&sScreenPoint);
		if(( sScreenPoint.x > (DXBUTTON*(IDC_FREWIND-IDC_LASTTRACK)) )  
			&& ( sScreenPoint.x < (DXBUTTON*(1+IDC_FREWIND-IDC_LASTTRACK)) ) 
			&& ( sScreenPoint.y >0 )  && ( sScreenPoint.y < DYBUTTON ))
				return TRUE ;
		else
			return FALSE ;
	}
	else
		return FALSE;
}

void HandleFastSearch(MSG msg)
{
	BOOL bIsPressed ;		
	if( ((msg.message == WM_LBUTTONDOWN) && (msg.wParam == MK_LBUTTON)) 
		|| ( (msg.message == WM_USER ) && (msg.hwnd == hMainWnd )))
	{
		if(IsForwardBtnPressed())
		{
			OnForward();
			PostMessage(hMainWnd,WM_USER,0,0);
		}
		else if(IsRewindBtnPressed())
		{
			OnRewind();
			PostMessage(hMainWnd,WM_USER,0,0);
		}
	}
}

void OnChangeOutputMode(int idMenuItem)
{
	HMENU hOutputMenu ;
	int iDCurMenu  ;
	
	hOutputMenu = GetSubMenu( GetMenu( hMainWnd ) , 2 ) ;		
	play_mode = play_mode_list[ idMenuItem - IDM_DYNMENU ] ;

	for(iDCurMenu = IDM_DYNMENU ; iDCurMenu < iDMMaxDynMenu ; iDCurMenu++ )
		CheckMenuItem(hOutputMenu, iDCurMenu , MF_BYCOMMAND | MF_UNCHECKED );
	CheckMenuItem(hOutputMenu, idMenuItem , MF_BYCOMMAND | MF_CHECKED );			
}

void OnOpen()
{	
	HDC hDC;

	if(!bIsPlaying)
	{
		if(cbFileInPlayList != 0 )
		{
			FreeFileList(ppszPlayList,cbFileInPlayList);
			ppszPlayList = 0 ;
			cbFileInPlayList = 0;
		}
		DlgMultiFileOpen(&ppszPlayList,&cbFileInPlayList);	
	}
	iCurPlayListIndex = 0 ;		
	SetMidiTitle();
	hDC = GetDC( hMainWnd );		
	PaintText( hMainWnd , hDC );
	ReleaseDC( hMainWnd , hDC );
} 

void OnTrackBarMove(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	int iNewVol ;
	if( (hwndCtl == hVolumeTkBar) && 
	( (code == TB_THUMBPOSITION) || (code == TB_PAGEUP ) 
		|| (code == TB_PAGEDOWN )) )
	{
		iNewVol = GetTrkBarVol(hVolumeTkBar);					
		iDeltaVolume = iNewVol - iVolume ;
		if(iDeltaVolume != 0)
			iGUIWhishedState = RC_CHANGE_VOLUME ;	
	}
}

void OnPlay()
{		
	if(!bIsPlaying)
	{		
		/* thread for playing midi file is lauched here , there is no more than one
		tread active at a time + another one for graphic interface.*/
		hPlayThread = (HANDLE)_beginthread(&PlayList,0,0);
		//SetThreadPriority(hPlayThread,THREAD_PRIORITY_ABOVE_NORMAL); //uncomment this if you like
		/*rq : ms doc advise to use _beginthreadex instead of CreateThread */
	}
}

void OnStop()
{
	if(bIsPlaying)
	{
		iGUIWhishedState = RC_QUIT ;
	}
}

void OnPause()
{
	static BOOL bPauseState = FALSE	;
	// iGUIWhishedState = RC_TOGGLE_PAUSE : doesn't seem to work
	if(bIsPlaying)
	{
		if(bPauseState)
		{
			ResumeThread(hPlayThread);				
			bPauseState = FALSE	;
		}
		else
		{
			SuspendThread(hPlayThread);		
			bPauseState = TRUE ;
		}
	} 
}

void OnPrevTrack()
{
	HDC hDC;

	if(bIsPlaying)
	{
		iGUIWhishedState = RC_REALLY_PREVIOUS ;
	}
	else
	{
		if(iCurPlayListIndex>0)
			iCurPlayListIndex--	 ;
		SetMidiTitle();
	}
	hDC = GetDC( hMainWnd );		
	PaintText( hMainWnd , hDC );
	ReleaseDC( hMainWnd , hDC );
}

void OnNextTrack()
{
	HDC hDC;
	if(bIsPlaying)
	{
		iGUIWhishedState = RC_NEXT ;
	}
	else
	{
		if(iCurPlayListIndex < (cbFileInPlayList-1))
			iCurPlayListIndex++	 ;
		SetMidiTitle();
	}
	hDC = GetDC( hMainWnd );		
	PaintText( hMainWnd , hDC );
	ReleaseDC( hMainWnd , hDC );
}

void OnForward()
{
	if(bIsPlaying)
		iGUIWhishedState =  RC_FORWARD ;
}

void OnRewind()
{
	if(bIsPlaying)	
		iGUIWhishedState = RC_BACK ; 	
}

void OnAboutDlg()
{ 
	DialogBox(hInst,MAKEINTRESOURCE(DLG_ABOUT),hMainWnd,
		MakeProcInstance((FARPROC)(&InteressMessDlgProc),hInst));
}

void OnGeneralConf()
{
	if(!bIsPlaying)
		DialogBox(hInst,MAKEINTRESOURCE(DLG_GENCONF),hMainWnd,
			MakeProcInstance((FARPROC)(&GenConfDlgProc),hInst));	
}

void OnOutputConf()
{
	if(!bIsPlaying)
		DialogBox(hInst,MAKEINTRESOURCE(DLG_OUTPUTCONF),hMainWnd,
			MakeProcInstance((FARPROC)(&OutputConfDlgProc),hInst));
}

void OnMidiConf()
{
	if(!bIsPlaying)
		DialogBox(hInst,MAKEINTRESOURCE(DLG_MIDICONF),hMainWnd,
			MakeProcInstance((FARPROC)(&MidiConfDlgProc),hInst));
}


void PlayList(LPVOID pvArg) // this is a thread
{
	int iCtrlState = RC_NONE ;
	
	if(!bIsPlaying) // play the files 
	{
		bIsPlaying = TRUE ;
		while( (iCurPlayListIndex < cbFileInPlayList) 
			&& (iCurPlayListIndex >= 0) )
		{
			if(play_mode->open_output()<0)
			{
				DisplayMessage("Unable to play midi file");	
				iCurPlayListIndex = cbFileInPlayList;
			}
			else
				iCtrlState = play_midi_file(ppszPlayList[iCurPlayListIndex]) ;
			play_mode->close_output();	
			switch(iCtrlState)
			{
				case RC_NONE :
				case RC_NEXT :
				case RC_TUNE_END:
				  iCurPlayListIndex++; 
				  break;
				case RC_PREVIOUS:
				case RC_REALLY_PREVIOUS:
					iCurPlayListIndex--;
					break;
				case RC_QUIT:
				case RC_ERROR :
					bIsPlaying = FALSE ;	
					_endthread();
				default:
					break;
			}				
		}
	}
	bIsPlaying = FALSE ;		
	iCurPlayListIndex = SatToRange(iCurPlayListIndex,0,cbFileInPlayList-1);
	_endthread();
}

#endif /* __WIN95__ */

