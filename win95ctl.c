/*
 *		win95_ctl.cpp
 *		timidity port for win32 'windows' mode
 *		based on timidity (Unix ) from Tuukka Toivonen and
 *		provided under GNU General Public License
 *	contents :
 *		ControlMode redefinition for win32 GUI interface 
 *   here we declare an instance of win95_control_mode
 *    Nicolas Witczak mai 1996
 */

#ifdef __WIN95__

#include "win95hlp.h"
#include "win95dlg.h"
#include "win95_i.h"
#include "win95cfg.h"

#include "config.h"
#include "common.h"
#include "output.h"
#include "controls.h"


/////////////////////////////////////////////////////////////////
// export the interface functions 

#define ctl win95_control_mode

ControlMode ctl ;

static FILE *infp=stdin, *outfp=stdout; 

#define GUIWin95ctlName "hf interface"

static int ctl_open(int using_stdin, int using_stdout);
static void ctl_pass_playing_list(int number_of_files, char *list_of_files[]);
static void ctl_close(void);
static int ctl_read(int32 *valp);
static int cmsg(int type, int verbosity_level, char *fmt, ...);

static void ctl_refresh(void);
static void ctl_reset(void);
static void ctl_file_name(char *name);
static void ctl_total_time(int tt);
static void ctl_current_time(int ct);

static void ctl_note(int v);
static void ctl_master_volume(int mv);
static void ctl_program(int ch, int val);
static void ctl_volume(int channel, int val);
static void ctl_expression(int channel, int val);
static void ctl_panning(int channel, int val);
static void ctl_sustain(int channel, int val);
static void ctl_pitch_bend(int channel, int val);

ControlMode ctl= 
{
	GUIWin95ctlName ,'w', 1 , 0 , 0 ,
	ctl_open, ctl_pass_playing_list , ctl_close, ctl_read, cmsg,
	ctl_refresh, ctl_reset, ctl_file_name, ctl_total_time, ctl_current_time, 
	ctl_note, ctl_master_volume, ctl_program, ctl_volume, ctl_expression, ctl_panning, ctl_sustain, ctl_pitch_bend
};

/////////////////////////////////////////////////////////////////
// win 95 control interface

static int ctl_open(int using_stdin, int using_stdout)
{
	InitialyseOSVerStruct();	

// init external variables

	hInst = GetModuleHandle(0) ;

// loads common control's DLL	for trackbar volume and button toolbar	
	InitCommonControls();
	InitMainWnd( SW_SHOW );
	CreateOutputWnd();
	if( hMainWnd == 0 )
		return 1 ;

// append various output mode to output menu	
	InitOutputMenu();
	ctl.opened=1;
	return 0;
}

static void ctl_pass_playing_list(int number_of_files, char *list_of_files[])
{
	MSG msg ;
	int iFile ;
	FreeFileList(ppszPlayList,cbFileInPlayList);			
	cbFileInPlayList = number_of_files ;
	ppszPlayList = calloc(cbFileInPlayList,sizeof(char*));
	for(iFile = 0;iFile < cbFileInPlayList;iFile++)
	{
		ppszPlayList[iFile] = malloc( strlen(list_of_files[ iFile ]) + 1 );
		strcpy( ppszPlayList[iFile] , list_of_files[ iFile ] ) ;
	}
	OnPlay();
	while( GetMessage(&msg,NULL,0,0) )
	{
		HandleFastSearch(msg);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

static void ctl_close(void)
{ 
	ctl.opened = 0;	
}

static int ctl_read(int32 *valp)
{
	int iRetState ;
	iRetState = iGUIWhishedState ;
	iGUIWhishedState = RC_NONE ;
	if( (iRetState == RC_FORWARD) || (iRetState == RC_BACK) )
		*valp=play_mode->rate ;
	else if( iRetState == RC_CHANGE_VOLUME )
	{
		iVolume += iDeltaVolume ;
		*valp = iDeltaVolume ;
		iDeltaVolume = 0 ; 
	}	
	return iRetState ;
}

static int cmsg(int type, int verbosity_level, char *fmt, ...)
{  
	va_list ap;
	va_start(ap, fmt);
	vcmsg(type,verbosity_level,fmt,ap);
	va_end(ap);
	return 0;
}


static void ctl_refresh(void) 
{ 
	//PumpWindowMessages() ;
}

static void ctl_total_time(int tt)
{
	iMidiFileTotalSec=tt/play_mode->rate;
	iMidiFileTotalMin=iMidiFileTotalSec/60;
	iMidiFileTotalSec-=iMidiFileTotalMin*60;
}

static void ctl_master_volume(int mv) {}

static void ctl_file_name(char *name)
{
	SetMidiTitle();
}

static void ctl_current_time(int ct)
{
	int secs;
	HDC hDC ;
	secs=ct/play_mode->rate;  
	if ( iMidiFileCurSec != secs )
	{
		iMidiFileCurSec = secs ;
		iMidiFileCurMin=iMidiFileCurSec/60;
		iMidiFileCurSec-=iMidiFileCurMin*60;
		hDC = GetDC( hMainWnd );		
		PaintText(hMainWnd,hDC);
		ReleaseDC(hMainWnd,hDC);			
	}
}

static void ctl_note(int v) {}

static void ctl_program(int ch, int val) {}

static void ctl_volume(int channel, int val) {}

static void ctl_expression(int channel, int val) {}

static void ctl_panning(int channel, int val) {}

static void ctl_sustain(int channel, int val) {}

static void ctl_pitch_bend(int channel, int val) {}

static void ctl_reset(void) {}


#endif /* __WIN95__ */
