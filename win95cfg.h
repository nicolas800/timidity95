/*
 *		win95_cfg.h
 *		timidity port for win32 'windows' mode
 *		based on timidity (Unix ) from Tuukka Toivonen and
 *		provided under GNU General Public License
 *	contents : 
 *    configuration through windows 'ini' file 
 * and timidity.cgf file , midi file lauching
 *    header file 
 *    Nicolas Witczak mai 1996
 */

#ifndef WIN95CFG_H
	#define WIN95CFG_H

#include "win95hlp.h"


///////////////////////////////////////////////////////////////////
// config variables
extern int free_instruments_afterwards ;
extern char def_instr_name[256] ;
extern char_buff pszTimidCfgPath; 
extern int iCtrlPerSecond ;
extern BOOL bTrustWinAudioSpRate ;	   
extern int bShowOutput ; // show debugging output
extern int active_effect ;

///////////////////////////////////////////////////////////////////
// function definition

/** fct LoadIniFile : load ini file which contains default switches possibly  
 *	overridden by command line. returns 1 if success ,0 if fail 
 */
BOOL LoadIniFile();

/** fct SaveIniFile : save ini file
 */
BOOL SaveIniFile();

BOOL InitTimidity();

///////////////////////////////////////////////////////////////////
// imported from timidity.c
int read_config_file(char *name);
int play_midi_file(char *fn);

#endif // WIN95CFG_H