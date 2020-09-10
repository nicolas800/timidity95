/*
 *		win95_dlg.h
 *		timidity port for win32 'windows' mode
 *		based on timidity (Unix ) from Tuukka Toivonen and
 *		win32 console port from  Davide Moretti's 
 *		provided under GNU General Public License
 *	contents : 
 *    dialog box about , interresting messages
 *    configuration dialog
 *    Nicolas Witczak mai 1996 header file
 */

#ifndef WIN95DLG_H
	#define WIN95DLG_H

#include "win95hlp.h"

////////////////////////////////////////////////////////////////////////////////
// interesting message dialog

LRESULT CALLBACK InteressMessDlgProc(HWND  hDlg,UINT  msg,WPARAM  wParam,LPARAM  lParam);


////////////////////////////////////////////////////////////////////////////////
// general configuration dialog

LRESULT CALLBACK GenConfDlgProc(HWND  hDlg,UINT msg,WPARAM wParam,LPARAM lParam);


////////////////////////////////////////////////////////////////////////////////
// output configuration dialog

LRESULT CALLBACK OutputConfDlgProc(HWND  hDlg,UINT msg,WPARAM wParam,LPARAM lParam);

////////////////////////////////////////////////////////////////////////////////
// midi configuration dialog
	
LRESULT CALLBACK MidiConfDlgProc(HWND  hDlg,UINT msg,WPARAM wParam,LPARAM lParam)	;

////////////////////////////////////////////////////////////////////////////////
// midi channels configuration dialog

LRESULT CALLBACK MidiChnlDlgProc(HWND  hDlg,UINT msg,WPARAM wParam,LPARAM lParam);

////////////////////////////////////////////////////////////////////////////////
// effect configuration dialog

LRESULT CALLBACK EffectDlgProc(HWND  hDlg,UINT msg,WPARAM wParam,LPARAM lParam);

////////////////////////////////////////////////////////////////////////////////
// message output dialog

/** class name for output window */
#define OUTPUT_CLASS "HFOutput" 

/** default size for output window */
#define OUTPUTWND_XSIZE		250
#define OUTPUTWND_YSIZE		110

/* create output window used with my win95_control_mode */
void CreateOutputWnd();

void DestroyOutputWnd();

/** bShow == TRUE ->show , FALSE -> hide */
void ShowOutput( int bShow );

void ClearOutput();

/* same beheavior that cmsg but with va_list replacing "..."  */
void vcmsg(int type, int verbosity_level, char *fmt,va_list argList);

LRESULT CALLBACK MessagesDlgProc(HWND  hDlg,UINT msg,WPARAM wParam,LPARAM lParam);


#endif //WIN95DLG_H
