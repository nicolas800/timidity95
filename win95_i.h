/*
 *		guimain.h
 *		timidity port for win32 'windows' mode
 *		based on timidity (Unix ) from Tuukka Toivonen and
 *		provided under GNU General Public License
 *	contents : winmain entry point, widget management 
 *    Nicolas Witczak mai 1996
 */



#ifndef WIN95I_H
	#define WIN95I_H

#include "win95hlp.h"

///////////////////////////////////////////////////////////////////
// application main windows constants

#define TIMID_WNDCLASS "timidity"
#define CONTAINER_WNDCLASS "HFContainer"
#define TIMID_APP_NAME "Timidity95"
#define TIMID_INI_FILENAME "Timidity95.ini"
#define TIMIDDEFTITLE  TIMID_APP_NAME
#define NO_FILE "no files"
#define MAINWND_XSIZE	270
#define MAINWND_YSIZE	125
#define SIZE_CHAR       25
#define SIZE_SEP        8

	///////////////////////////////////////////////////////////////
	// track bar constants 
#define IDC_VOLTRCBAR  400
#define VOLTRKBAR_WIDTH 20
#define VOLTRKBAR_HEIGHT 80
#define VOLTRKBAR_MIN	 0		 /*percent*/
#define VOLTRKBAR_MAX	MAX_AMPLIFICATION
#define VOLTRKSEL_MIN	(VOLTRKBAR_MIN)
#define VOLTRKSEL_MAX	VOLTRKBAR_MAX

#define TRKBAR_TEXT_WIDTH	25
#define TRKBAR_TEXT_Y		55

	///////////////////////////////////////////////////////////////
	// buttons bar constants 
#define NUM_BUTTONS    7
#define DXBUTTON		30
#define DYBUTTON		24
#define IDC_BUTTONBAR  500
#define IDC_LASTTRACK  501
#define IDC_FREWIND	  502
#define IDC_PLAY		  503
#define IDC_PAUSE		  504
#define IDC_STOP		  505
#define IDC_FFORWRD	  506
#define IDC_NEXTTRACK  507

	////////////////////////////////////////////////////////////////
	// dynamic menu for showing output modes
#define IDM_DYNMENU    150

	///////////////////////////////////////////////////////////////
	// time and track info symbol
#define TIME_TEXT_X 8
#define TIME_TEXT_Y 60


///////////////////////////////////////////////////////////////////
// usefull windows handles

/** instance handle for the executable */
extern HINSTANCE hInst;	

/** windows handles for main windows and sub component */
extern HWND hMainWnd , hVolumeTkBar , hBarButton  ;

///////////////////////////////////////////////////////////////////
// worker thread thread management
extern BOOL bIsPlaying  ;
extern HANDLE hPlayThread ;


///////////////////////////////////////////////////////////////////
// communication with timidity engine through ctl interface
	
	////////////////////////////////////////////////////////////////
	// midi file info
extern int iMidiFileTotalMin ;
extern int iMidiFileTotalSec ;
extern int iMidiFileCurMin ;
extern int iMidiFileCurSec ;

	////////////////////////////////////////////////////////////////
	// volume
extern int iVolume ;
/* timidity ctrl request delta vol info*/
extern int iDeltaVolume  ;
	
	////////////////////////////////////////////////////////////////
	// play state from gui
/* user request are stored here and read by ctrl in timidity play loop */
extern int iGUIWhishedState  ; 

	////////////////////////////////////////////////////////////////
	//play list
extern char** ppszPlayList;
extern int cbFileInPlayList ;
extern int iCurPlayListIndex ;

///////////////////////////////////////////////////////////////////
// function declaration

/*	WinMain entry point : replace main fonction. cmd line processing
 * is replaced with a *.ini windows file which ac enventally been edited
 * 
 */
int WINAPI WinMain( HINSTANCE hInstance,HINSTANCE hPrevInstance
	,LPSTR lpCmdLine,int nCmdShow);

/*
 * InitMainWnd function
 * descr  : launch main windows and its controls 
 */
void InitMainWnd(int nCmdShow);

/*
 *	InitOutputMenu
 *	desrc : dynamiquely append to menu output mode 
 */
void InitOutputMenu();

/*
 * InitVolTrkBar function
 * input  : hwndParent : the main window
 * output : the handle of the trackbar created
 * descr  : display a trackbar into main window 
 */
HWND InitVolTrkBar(HWND hwndParent);

/*
 * SetTrkBarVol,GetTrkBarVol functions
 * hwndTrkBar : the trackbar to query
 * input,output: iVol : the volum in percent
 * descr  : display a trackbar (volume control) into main window 
 */

void SetTrkBarVol(HWND hwndTrkBar,int iNewVol);

int GetTrkBarVol(HWND hwndTrkBar);

/*
 * InitButtons function
 * input  : hwndParent : the main window
 * output : the handle of the trackbar created
 * descr  : display bottons (play , stop , ...) 
 */
HWND InitButtons(HWND hwndParent);

/*
 * MainWinProc function
 * descr  : main window proc , response to messages 
 */
LRESULT CALLBACK MainWinProc(HWND hwnd, UINT uMess, WPARAM wParam, LPARAM lParam);

/*
 * PaintDecoration PaintText function
 * descr  : draw additionnal decoration and text
 */
void PaintDecoration(HWND hwnd, HDC hDC );
void PaintText(HWND hwnd, HDC hDC );

///////////////////////////////////////////////////////////////////
// handlers for main window

/*
 * On??? function
 * descr  : response to windows message in winproc function
 */

void OnPaintMainWnd(HWND hwnd) ;

void OnMainWndSize(HWND hwnd, UINT state, int cx, int cy);

void OnDropFilesMainWnd(HWND hwnd, HDROP hdrop);

void OnDestroy(HWND hwnd);

void OnExitApp();

/*
 * MainCmdProc function
 * descr  : called from main window proc as response to user message  
 * menu, bottuns ..
 */
void MainCmdProc(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

void OnTrackBarMove(HWND hwnd, HWND hwndCtl, UINT code, int pos);

BOOL IsForwardBtnPressed();

BOOL IsRewindBtnPressed();

/* fctHandleFastSearch:  hack for toolbar common ctrl	
 *	param msg : window message got from message queue
 *	  rq : this patch is intended for the toolbar common control 
 *  Normally it send a message only once when the button has been pressed
 *	and released which doesn't suit this particuliar case .Moreover toolbar control does not
 *	send back to us (hMainWnd) usefull message like WM_LBUTTONDOWN or WM_MOUSEMOVE
 *	Instead this function hooks mouse button while its beeing pressed upon and before
 *	trackbar control can hang WM_LBUTTONDOWN, then it regularly maintains itself active by
 *	sending WM_USER message until user depresses mouse button . Conclusion : it would have 
 *	been more simple if I had done this toolbar by hand :-( 
 */
void HandleFastSearch(MSG msg);	

void OnOpen();

void OnChangeOutputMode(int idMenuItem);

void OnChangeOutputMode();

void OnPlay();

void OnStop();

void OnPause();

void OnPrevTrack();

void OnNextTrack();

void OnForward();

void OnRewind();

void OnAboutDlg();

void OnGeneralConf();

void OnOutputConf();

void OnMidiConf();


///////////////////////////////////////////////////////////////////
// worker thread

/**	pvArg is requested by BeginThread API fct but is not used*/
void PlayList(LPVOID pvArg);	 

#endif //WIN95I_H
						  