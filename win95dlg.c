/*
 *		win95_dlg.cpp
 *		timidity port for win32 'windows' mode
 *		based on timidity (Unix ) from Tuukka Toivonen and
 *		provided under GNU General Public License
 *	contents : 
 *    dialog boxes , output window
 *    Nicolas Witczak mai 1996
 */

#ifdef __WIN95__

#include "win95hlp.h"
#include "win95res.h"
#include "win95dlg.h"
#include "win95_i.h"
#include "win95cfg.h"


#include "config.h"
#include "common.h"
#include "instrum.h"
#include "playmidi.h"
#include "readmidi.h"
#include "output.h"
#include "controls.h"
#include "tables.h"
#include "effects.h"


////////////////////////////////////////////////////////////////////////////////
// interesting message dialog

static BOOL InteressMessCmdProc(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id)
	{			
		case IDB_INTERESTING:
			DialogBox(hInst,MAKEINTRESOURCE(DLG_INTERSTING),hMainWnd,
			MakeProcInstance((FARPROC)(&InteressMessDlgProc),hInst));
		break	;
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg,TRUE);	
	}
	return FALSE ;
}

LRESULT CALLBACK InteressMessDlgProc(HWND  hDlg,UINT msg,WPARAM wParam,LPARAM lParam)	
{	
	switch(msg)
	{
		HANDLE_MSG(hDlg,WM_COMMAND,InteressMessCmdProc);						
		case WM_CLOSE:
			EndDialog(hDlg,TRUE);			
		default: break;
	}
	return FALSE ;
}


////////////////////////////////////////////////////////////////////////////////
// general configuration dialog

static char_buff pszConfigPathBuff ;
static char_buff pszBuffer ;

static void OnPressBrowsePath(HWND hDlg)
{
	DlgFileOpen("Timidity95 config file path",pszConfigPathBuff,szStringBuff);
	SetDlgItemText(hDlg,IDE_TIMIDCFGPATH,pszConfigPathBuff);
	return ;	
}

static void GenConfDlgOnExitOk(HWND hDlg)
{
	int iTmp ;
	GetDlgItemText(hDlg,IDE_TIMIDCFGPATH,pszConfigPathBuff,szStringBuff);
	if(strcmp(pszTimidCfgPath,pszConfigPathBuff) != 0 )
	{
		if ( read_config_file( pszConfigPathBuff ) )	 
			DisplayMessage("Unable to load instrument definition file");
		else
			strcpy(pszTimidCfgPath,pszConfigPathBuff);
	}

	GetDlgItemText(hDlg,IDE_VOLUME,pszBuffer,sizeof(pszBuffer));
	iTmp = atoi(pszBuffer);
	amplification = SatToRange(iTmp,1,MAX_AMPLIFICATION); 
	SetTrkBarVol(hVolumeTkBar,amplification);
	iVolume =  amplification ;
	iDeltaVolume =0 ;

	GetDlgItemText(hDlg,IDE_VERBOSE,pszBuffer,sizeof(pszBuffer));
	iTmp = atoi(pszBuffer);
	ctl->verbosity = SatToRange(iTmp,VERB_NORMAL,VERB_DEBUG_SILLY); 

	if(IsDlgButtonChecked(hDlg,IDC_FREEINSTR) ==	1)
		free_instruments_afterwards = 1 ;
	else
		free_instruments_afterwards = 0 ;
	if(IsDlgButtonChecked(hDlg,IDC_VIEWOUTPUTMSG) ==	1)
		bShowOutput = 1 ;
	else
		bShowOutput = 0 ;
	ShowOutput( bShowOutput );
	SaveIniFile() ;
}

static BOOL GenConfCmdProc(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id)
	{			
		case IDB_GETCFGPATH:
			OnPressBrowsePath(hDlg);
			break;
		case IDOK:
			GenConfDlgOnExitOk(hDlg);
		case IDCANCEL:
			EndDialog(hDlg,TRUE);	
	}
	return FALSE ;
}

static BOOL GenConfDlgOnInit(HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	SetDlgItemText(hDlg,IDE_TIMIDCFGPATH,pszTimidCfgPath);
	SetDlgItemText(hDlg,IDE_VOLUME,itoa(amplification,pszBuffer,10));
	SetDlgItemText(hDlg,IDE_VERBOSE,itoa(ctl->verbosity,pszBuffer,10));
	if(free_instruments_afterwards)
		CheckDlgButton(hDlg,IDC_FREEINSTR,1);
	else
		CheckDlgButton(hDlg,IDC_FREEINSTR,0);
	if(bShowOutput)
		CheckDlgButton(hDlg,IDC_VIEWOUTPUTMSG,1);
	else
		CheckDlgButton(hDlg,IDC_VIEWOUTPUTMSG,0);
	return TRUE ;
}

LRESULT CALLBACK GenConfDlgProc(HWND  hDlg,UINT msg,WPARAM wParam,LPARAM lParam)	
{	
	switch(msg)
	{
		HANDLE_MSG(hDlg,WM_COMMAND,GenConfCmdProc);
		HANDLE_MSG(hDlg,WM_INITDIALOG,GenConfDlgOnInit);
		case WM_CLOSE:
			EndDialog(hDlg,TRUE);
		default: break;
	}
	return FALSE ;
}

////////////////////////////////////////////////////////////////////////////////
// output configuration dialog

static char_buff pszOutputPath;

static BOOL OutputConfDlgOnInit(HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	if( play_mode->id_character != 'd' )
	{	/* assume audio driver -> no output file */
		strcpy(pszOutputPath,play_mode->name);
		SetDlgItemText(hDlg,IDE_OUTPUTPATH,pszOutputPath);			
	}
	else
	{	
		SetWindowLong( GetDlgItem(hDlg,IDE_OUTPUTPATH) , GWL_STYLE , WS_DISABLED ) ;
		SetWindowLong( GetDlgItem(hDlg,IDB_GETOUTPUTPATH) , GWL_STYLE , WS_DISABLED ) ;
	}
	SetWindowText(hDlg , play_mode->id_name ) ;
	SetDlgItemText(hDlg,IDE_SAMPLERATE,itoa(play_mode->rate,pszBuffer,10));
	SetDlgItemText(hDlg,IDE_CBFRGMT,itoa(play_mode->extra_param[0],pszBuffer,10));
	
	if(bTrustWinAudioSpRate)
		CheckDlgButton(hDlg,IDC_TRUSTDRV,1);
	else
		CheckDlgButton(hDlg,IDC_TRUSTDRV,0);
		
	if(play_mode->encoding & PE_MONO)
		CheckRadioButton(hDlg,IDR_MONO,IDR_STEREO,IDR_MONO);
	else
		CheckRadioButton(hDlg,IDR_MONO,IDR_STEREO,IDR_STEREO);			

	if(play_mode->encoding & PE_16BIT)
		CheckRadioButton(hDlg,IDR_16BIT,IDR_8BIT,IDR_16BIT);
	else
		CheckRadioButton(hDlg,IDR_16BIT,IDR_8BIT,IDR_8BIT);	
	return TRUE ;
}

static void OutputConfDlgOnExitOk(HWND hDlg)
{
	int iTmp ;
	GetDlgItemText(hDlg,IDE_SAMPLERATE,pszBuffer,sizeof(pszBuffer));
	iTmp = atoi(pszBuffer);
	play_mode->rate = SatToRange(iTmp,MIN_OUTPUT_RATE,MAX_OUTPUT_RATE); 

	GetDlgItemText(hDlg,IDE_CBFRGMT,pszBuffer,sizeof(pszBuffer));
	iTmp = atoi(pszBuffer);
	play_mode->extra_param[0] = SatToRange(iTmp,2,32); 
	
	GetDlgItemText(hDlg,IDE_OUTPUTPATH,pszOutputPath,sizeof(pszBuffer) ) ;
	play_mode->name = pszOutputPath ;
	
	if(IsDlgButtonChecked(hDlg,IDR_MONO) == 1)
		play_mode->encoding |= PE_MONO;
	else
		play_mode->encoding &= ~PE_MONO;		
	
	if(IsDlgButtonChecked(hDlg,IDR_16BIT) == 1)
		play_mode->encoding |= PE_16BIT;
	else
		play_mode->encoding &= ~PE_16BIT;		
	
	if(IsDlgButtonChecked(hDlg,IDC_TRUSTDRV) == 1)
		bTrustWinAudioSpRate = 1;
	else
		bTrustWinAudioSpRate = 0;
	
	SaveIniFile() ;
}

static void OutputConfOnPressPath(HWND hDlg)
{
	DlgFileOpen("Output file path",pszOutputPath,szStringBuff);
	SetDlgItemText(hDlg,IDE_OUTPUTPATH,pszOutputPath);
}

static BOOL OutputConfCmdProc(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id)
	{			
		case IDB_GETOUTPUTPATH:
			OutputConfOnPressPath(hDlg);
			break;
		case IDOK:
			OutputConfDlgOnExitOk(hDlg);
		case IDCANCEL:
			EndDialog(hDlg,TRUE);	
	}
	return FALSE ;
}

LRESULT CALLBACK OutputConfDlgProc(HWND  hDlg,UINT msg,WPARAM wParam,LPARAM lParam)	
{	
	switch(msg)
	{
		HANDLE_MSG(hDlg,WM_COMMAND,OutputConfCmdProc);
		HANDLE_MSG(hDlg,WM_INITDIALOG,OutputConfDlgOnInit);
		case WM_CLOSE:
			EndDialog(hDlg,TRUE);
		default: break ;
	}
	return FALSE ;
}


////////////////////////////////////////////////////////////////////////////////
// midi configuration dialog

static BOOL MidiConfDlgOnInit(HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	SetDlgItemText(hDlg,IDE_CTRLRATE,itoa(iCtrlPerSecond,pszBuffer,10));
	SetDlgItemText(hDlg,IDE_CBVOICE,itoa(voices,pszBuffer,10));
	if(fast_decay)
		CheckDlgButton(hDlg,IDC_FASTDECAY,1);
	else
		CheckDlgButton(hDlg,IDC_FASTDECAY,0);
	if(adjust_panning_immediately)
		CheckDlgButton(hDlg,IDC_FASTPANNING,1);
	else
		CheckDlgButton(hDlg,IDC_FASTPANNING,0);
	if(antialiasing_allowed)
		CheckDlgButton(hDlg,IDC_ANTIALIASING,1);
	else
		CheckDlgButton(hDlg,IDC_ANTIALIASING,0);
	return TRUE ;
}

static void MidiConfDlgOnMidiChnls(HWND hDlg)
{
	DialogBox(hInst,MAKEINTRESOURCE(DLG_MIDICHANNELS),hMainWnd,
		MakeProcInstance((FARPROC)(&MidiChnlDlgProc),hInst));		
}

static void MidiConfDlgOnChannelConf(HWND hDlg)
{
	DialogBox(hInst,MAKEINTRESOURCE(DLG_EFFECT),hMainWnd,
		MakeProcInstance((FARPROC)(&EffectDlgProc),hInst));		
}

static void MidiConfDlgOnExitOk(HWND hDlg)
{
	int iTmp ;
	
	GetDlgItemText(hDlg,IDE_CTRLRATE,pszBuffer,sizeof(pszBuffer));
	iTmp = atoi(pszBuffer);
	if(iTmp <= 0)
		iTmp = CONTROLS_PER_SECOND ;
	control_ratio = play_mode->rate / iTmp ;
	control_ratio = SatToRange(control_ratio, 1, MAX_CONTROL_RATIO ); 
	iCtrlPerSecond = play_mode->rate / control_ratio ;

	GetDlgItemText(hDlg,IDE_CBVOICE,pszBuffer,sizeof(pszBuffer));
	iTmp = atoi(pszBuffer);
	voices = SatToRange(iTmp,1, MAX_VOICES);

	if(IsDlgButtonChecked(hDlg,IDC_FASTDECAY) == 1)
		fast_decay = 1;
	else
		fast_decay = 0;		

	if(IsDlgButtonChecked(hDlg,IDC_FASTPANNING) == 1)
		adjust_panning_immediately = 1;
	else
		adjust_panning_immediately = 0;
	if(IsDlgButtonChecked(hDlg,IDC_ANTIALIASING) == 1)
		antialiasing_allowed = 1 ;
	else
		antialiasing_allowed = 0 ;

	SaveIniFile() ;
}

static BOOL MidiConfCmdProc(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id)
	{			
		case IDB_MIDICHNLS:
			MidiConfDlgOnMidiChnls(hDlg);
			break;
		case IDB_CHANNEL_EFFECT:
			MidiConfDlgOnChannelConf(hDlg);
			break;		
		case IDOK:
			MidiConfDlgOnExitOk(hDlg);
		case IDCANCEL:
			EndDialog(hDlg,TRUE);	
	}
	return FALSE ;
}

LRESULT CALLBACK MidiConfDlgProc(HWND  hDlg,UINT msg,WPARAM wParam,LPARAM lParam)	
{	
	switch(msg)
	{
		HANDLE_MSG(hDlg,WM_COMMAND,MidiConfCmdProc);
		HANDLE_MSG(hDlg,WM_INITDIALOG,MidiConfDlgOnInit);	
		case WM_CLOSE:
			EndDialog(hDlg,TRUE);
		default: break ;
	}
	return FALSE ;
}

////////////////////////////////////////////////////////////////////////////////
// midi channels configuration dialog

static BOOL MidiChnlDlgOnInit(HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	int iDx ;
	for(iDx = 0 ; iDx < 15  ; iDx++ )
	{
		if( quietchannels & (1 << iDx) )
			CheckDlgButton(hDlg,IDC_QUIET1+iDx,1);
		else
			CheckDlgButton(hDlg,IDC_QUIET1+iDx,0);
	}
	for(iDx = 0 ; iDx < 15  ; iDx++ )
	{
		if( drumchannels & (1 << iDx) )
			break ;
	}
	CheckRadioButton(hDlg,IDR_DRUMCHNL1,IDR_DRUMCHNL16,IDR_DRUMCHNL1+iDx);							
	return TRUE ;
}

static void MidiChnlDlgOnExitOk(HWND hDlg)
{
	int iDx ;
	for(iDx = 0 ; iDx < 15  ; iDx++ )
	{
		if( IsDlgButtonChecked(hDlg,IDC_QUIET1 + iDx ) == 1 )
			quietchannels |= ( 1 << iDx);
		else
			quietchannels &= (~( 1 << iDx));
		if( IsDlgButtonChecked(hDlg,IDR_DRUMCHNL1 + iDx ) == 1 )
		  drumchannels = ( 1 << iDx ) ;
	}
}

static BOOL MidiChnlCmdProc(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id)
	{			
		case IDOK:
			MidiChnlDlgOnExitOk(hDlg);
		case IDCANCEL:
			EndDialog(hDlg,TRUE);	
	}
	return FALSE ;
}

LRESULT CALLBACK MidiChnlDlgProc(HWND  hDlg,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG(hDlg,WM_COMMAND,MidiChnlCmdProc);
		HANDLE_MSG(hDlg,WM_INITDIALOG,MidiChnlDlgOnInit);	
		case WM_CLOSE:
			EndDialog(hDlg,TRUE);
		default: break ;
	}
	return FALSE ;
}

////////////////////////////////////////////////////////////////////////////////
// effects configuration dialog

static BOOL EffectDlgOnInit(HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	int x_cur , y_cur , cx_cur , cy_cur , idEffect,delta ;
	RECT tmp ; 
	HWND  hwnd ;
	if(active_effect)
		CheckDlgButton(hDlg,IDC_CHANNEL_EFFECT,1);
	else
		CheckDlgButton(hDlg,IDC_CHANNEL_EFFECT,0);	
	GetWindowRect( GetDlgItem( hDlg , IDS_FRAME ) , &tmp ) ;
	ScreenToClient( hDlg , (POINT*)&tmp ) ;
	ScreenToClient( hDlg , ((POINT*)&tmp)+1 ) ;
	cy_cur = 13 ;
	delta = 8 ;
	x_cur = tmp.left + delta ;
	y_cur = tmp.top + delta;
	cx_cur = tmp.right - tmp.left - 2*delta ;

	for( idEffect = 0 ; effect_type_list[ idEffect ] != 0 ; ++idEffect )
	{
		y_cur += delta ;
		hwnd = CreateWindow( "Button" , 0 , 
			BS_AUTOCHECKBOX | BS_LEFTTEXT | WS_TABSTOP | WS_VISIBLE | WS_CHILD	
			, x_cur , y_cur , cx_cur , cy_cur , hDlg , 0 , hInst , 0 )  ;
		SetWindowLong( hwnd , GWL_ID , ID_FIRST_EFFECT + idEffect ) ;	
		SetWindowText( hwnd , effect_name[ idEffect ] ) ;
		if( effect_list[idEffect][0] == 0 )
			CheckDlgButton(hDlg, ID_FIRST_EFFECT + idEffect , 0 );
		else
			CheckDlgButton(hDlg, ID_FIRST_EFFECT + idEffect , 1 );
		if( active_effect == 0 )
			EnableWindow( GetDlgItem( hDlg , ID_FIRST_EFFECT + idEffect ) , FALSE ) ;
		y_cur += cy_cur ;
	}
	y_cur += delta ;
	tmp.bottom = y_cur ;	
	MoveWindow( GetDlgItem( hDlg , IDS_FRAME ) , tmp.left , tmp.top 
		, tmp.right-tmp.left , tmp.bottom - tmp.top , TRUE  ) ;
	
	GetWindowRect( hDlg , &tmp ) ;
	tmp.bottom += idEffect * ( cy_cur + delta );
	SetWindowPos( hDlg , 0 , tmp.left ,tmp.top , tmp.right-tmp.left , tmp.bottom - tmp.top , SWP_NOZORDER | SWP_NOMOVE ) ;
	ShowWindow( hDlg , SW_SHOW ) ;
	return TRUE ;
}

static void EffectDlgOnExitOk(HWND hDlg)
{
	int idChannel , idEffect ;
	if( IsDlgButtonChecked(hDlg,IDC_CHANNEL_EFFECT) == 1 )
		active_effect = 1;
	else
		active_effect = 0;
	effect_activate( active_effect ) ;
	for( idEffect = 0 ; effect_type_list[ idEffect ] != 0 ; ++idEffect )
	{
		if( IsDlgButtonChecked( hDlg, ID_FIRST_EFFECT + idEffect ) == 1 )
		{
			if( effect_list[idEffect][0] == 0 )
			for( idChannel = 0 ; idChannel < 16 ; ++ idChannel )
			{
				effect_list[idEffect][idChannel] = (effect_type_list[idEffect])(idChannel) ;
			}
		}
		else
		{
			if( effect_list[idEffect][0] != 0  )
			for( idChannel = 0 ; idChannel < 16 ; ++ idChannel )
			{
				(( effect_list[idEffect][idChannel] )->m_pfnDestruct)
					( effect_list[idEffect][idChannel] );
				effect_list[idEffect][idChannel] = 0 ;
			}
		}
	}
	SaveIniFile() ;
}

static void OnClickedEffectOnOff( HWND hwnd )
{
	int idEffect ;
	if( IsDlgButtonChecked( hwnd ,IDC_CHANNEL_EFFECT) == 1 ) 
	{
		for( idEffect = 0 ; effect_type_list[ idEffect ] != 0 ; ++idEffect )	
			EnableWindow( GetDlgItem( hwnd , ID_FIRST_EFFECT + idEffect ) , TRUE ) ;
	}
	else
	{
		for( idEffect = 0 ; effect_type_list[ idEffect ] != 0 ; ++idEffect )	
			EnableWindow( GetDlgItem( hwnd , ID_FIRST_EFFECT + idEffect ) , FALSE ) ;
	}
}
static BOOL EffectCmdProc(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id)
	{			
		case IDOK:
			EffectDlgOnExitOk(hDlg);
		case IDCANCEL:
			EndDialog(hDlg,TRUE);
			break ;
		case IDC_CHANNEL_EFFECT:
			OnClickedEffectOnOff(hDlg);
	}
	return FALSE;
}

LRESULT CALLBACK EffectDlgProc(HWND  hDlg,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG(hDlg,WM_COMMAND,EffectCmdProc);
		HANDLE_MSG(hDlg,WM_INITDIALOG,EffectDlgOnInit);	
		case WM_CLOSE:
			EndDialog(hDlg,TRUE);
		default: break ;
	}
	return FALSE ;
}

////////////////////////////////////////////////////////////////////////////////
// message output dialog


/** dialog windows containing edit output window */
static HWND hdlgOutput = 0 ;

/** the actual edit output window */
static HWND hwndOutput = 0 ;

static char_buff pszCWn95CMSG ;

extern ControlMode win95_control_mode ;

void vcmsg(int type, int verbosity_level, char *fmt,va_list argList)
{
	int strLength ;
	if( ((type==CMSG_TEXT) || (type==CMSG_INFO) || (type==CMSG_WARNING)) &&
		( win95_control_mode.verbosity < verbosity_level ) )
	return ;
	pszCWn95CMSG[0] = '\0' ;
	vsprintf( pszCWn95CMSG , fmt, argList) ;
	strLength = strlen(pszCWn95CMSG);
	pszCWn95CMSG[strLength] = 13 ;
	pszCWn95CMSG[strLength+1] = 10 ;
	pszCWn95CMSG[strLength+2] = '\0' ;
	if( IsWindow(hwndOutput) )
		Edit_ReplaceSel(hwndOutput,pszCWn95CMSG);
}

void CreateOutputWnd()
{		
	hdlgOutput = CreateDialog( hInst , MAKEINTRESOURCE(DLG_MESSAGES) , 
		0 , MakeProcInstance((FARPROC)(&MessagesDlgProc),hInst) );

	if( hdlgOutput != 0 )
		hwndOutput = GetDlgItem( hdlgOutput , IDE_MESSAGES ) ;
	ShowOutput( bShowOutput );		
}

void DestroyOutputWnd()
{
	DestroyWindow(hdlgOutput);
	hdlgOutput = 0 ;
	hwndOutput = 0 ;
}

void ShowOutput( int bShow )
{
	if( bShow )
		ShowWindow(hdlgOutput,SW_SHOW);	
	else
		ShowWindow(hdlgOutput,SW_HIDE);
}

void ClearOutput()
{
	char pszVoid[]="";
	Edit_SetText(hwndOutput,pszVoid);
}

static BOOL MessagesDlgOnInit(HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	return TRUE ;
}

static BOOL MessagesCmdProc(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id)
	{			
		case IDB_CLEARMESSAGES:
			ClearOutput();
			break ;
		case IDB_DISMISS:
			ShowOutput( FALSE );
	}
	return FALSE;
}

void MessagesDlg_OnSize(HWND hwnd, UINT state, int cx, int cy) 
{
	RECT  rect ;
	GetWindowRect( hwndOutput , &rect ) ;
	ScreenToClient( hwnd , (POINT*)(&rect) ) ;
	ScreenToClient( hwnd , ( (POINT*)(&rect) ) + 1 ) ;
	SetWindowPos( hwndOutput , 0 ,0 ,0 , cx - rect.left , cy - rect.top , SWP_NOZORDER | SWP_NOMOVE ) ;
}

LRESULT CALLBACK MessagesDlgProc(HWND  hDlg,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG(hDlg,WM_COMMAND,MessagesCmdProc);
		HANDLE_MSG(hDlg,WM_INITDIALOG,MessagesDlgOnInit);	
		HANDLE_MSG(hDlg,WM_SIZE,MessagesDlg_OnSize);		
		case WM_CLOSE:
			ShowOutput( FALSE );
			break;
		default: break ;
	}
	return FALSE ;
}

#endif /* __WIN95__ */

