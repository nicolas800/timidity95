/*
 *		win95_cfg.cpp
 *		timidity port for win32 'windows' mode
 *		based on timidity (Unix ) from Tuukka Toivonen and
 *		provided under GNU General Public License
 *	contents : 
 *    configuration through windows 'ini' file 
 * and timidity.cgf file , midi file lauching
 *    Nicolas Witczak mai 1996
 */

#ifdef __WIN95__


#include "win95hlp.h"
#include "win95cfg.h"
#include "win95_i.h"
#include "win95res.h"

#include "config.h"
#include "common.h"
#include "instrum.h"
#include "playmidi.h"
#include "readmidi.h"
#include "output.h"
#include "tables.h"
#include "controls.h"
#include "effects.h"

//////////////////////////////////////////////////////////////////
// timidity additionnal config variables

char_buff pszTimidCfgPath ; 
int iCtrlPerSecond ;
BOOL bTrustWinAudioSpRate ;
int bShowOutput ; 
int active_effect = 0 ;

//////////////////////////////////////////////////////////////////
// helper macro to manipulate ini files

static char_buff pszIntBuff ;

#define GetIniStr(key,retStr,defaultStr) \
	(GetPrivateProfileString(TIMID_APP_NAME,(key),(defaultStr),(retStr)	\
		,sizeof(retStr),TIMID_INI_FILENAME) )

#define SetIniStr(key,newStr)	\
	WritePrivateProfileString(TIMID_APP_NAME,(key),(newStr),TIMID_INI_FILENAME)

#define GetIniInt(key,defaultInt) \
	( GetIniStr((key),(pszIntBuff),itoa((defaultInt),(pszIntBuff),10)),atoi(pszIntBuff) )

#define SetIniInt(key,newInt)	\
	SetIniStr(key,itoa(newInt,pszIntBuff,10))

//////////////////////////////////////////////////////////////////
// timidity.ini sections entry

#define CFG_FILE_NAME "CfgFileName"
#define CFG_PLAY_RATE "PlayRate"
#define CFG_ANTIALIAS "AntiAliasing" 
#define CFG_STEREO    "Stereo"
#define CFG_VOICES	 "Voices"

#define GFG_AMPLIFICATION "Amplification"
#define CFG_CBFRAGMENTS "FragmentNumber"
#define CFG_SZFRAGMENTS "FragmentSize"

#define GFG_FAST_DECAY "FastDecay"
#define CFG_CTRL_PER_SEC "CtrlPsec"
#define CFG_PROMPT_PANNING "PromptPanning"
#define CFG_DEFAULT_INST	"DefaultInst"
#define CFG_FREE_INSTR "FreeInst"
#define CFG_CHANNEL_EFFECT "ChannelEffect"

#define CFG_TRUST_SMP_RATE "TrustWinAudioSmpRate"

#define CFG_VERBOSITYLEVEL "VerbosityLevel"
#define CFG_SHOWOUTPUTWND "ShowOutputWindow"

BOOL LoadIniFile()
{
	BOOL isOk = TRUE ;
	BOOL bFlag ;
	int idEffect ;

// display output window first if requested		
	bShowOutput = GetIniInt(CFG_SHOWOUTPUTWND,0);
	ShowOutput( bShowOutput );

	GetIniStr(CFG_FILE_NAME,pszTimidCfgPath,CONFIG_FILE);
	if ( read_config_file( pszTimidCfgPath ) )	 
	{
		DisplayMessage("Unable to load instrument definition file");
		isOk = FALSE ;
	}
// sound rendering		
	play_mode->rate = GetIniInt( CFG_PLAY_RATE , DEFAULT_RATE);
	play_mode->extra_param[0] = GetIniInt( CFG_CBFRAGMENTS , 4 );
	antialiasing_allowed = GetIniInt( CFG_ANTIALIAS , 0 );
	
	amplification = GetIniInt( GFG_AMPLIFICATION , DEFAULT_AMPLIFICATION );
	SetTrkBarVol(hVolumeTkBar,amplification);
	iVolume =  amplification ;

	bFlag = GetIniInt( CFG_STEREO , 1 ); 
	if(bFlag)  // stereo
		play_mode->encoding &= ~PE_MONO;	
	else
		play_mode->encoding |= PE_MONO;
// instruments rendering		
	fast_decay = GetIniInt( GFG_FAST_DECAY , 1 );
	iCtrlPerSecond = GetIniInt( CFG_CTRL_PER_SEC , CONTROLS_PER_SECOND );
	adjust_panning_immediately = GetIniInt(CFG_PROMPT_PANNING,adjust_panning_immediately);  
	if(iCtrlPerSecond <= 0)
		iCtrlPerSecond = CONTROLS_PER_SECOND ;
	control_ratio = play_mode->rate / iCtrlPerSecond ;
	GetIniStr(CFG_DEFAULT_INST,def_instr_name,"");
	voices = GetIniInt(CFG_VOICES,DEFAULT_VOICES);
	active_effect = GetIniInt( CFG_CHANNEL_EFFECT , 0 );

// effect 
	for( idEffect = 0 ; effect_type_list[ idEffect ] != 0 ; ++idEffect )
	{
		int idChannel ;
		if( GetIniInt(  effect_name[ idEffect ] , 0 ) != 0 )
		{
			if( effect_list[idEffect][0] == 0  )
			for( idChannel = 0 ; idChannel < 16 ; ++ idChannel )
			{
				effect_list[idEffect][idChannel] = (effect_type_list[idEffect])(idChannel) ;
			}
		}
		else
		{
			if( effect_list[idEffect][0] != 0 )
			for( idChannel = 0 ; idChannel < 16 ; ++ idChannel )
			{
				(( effect_list[idEffect][idChannel] )->m_pfnDestruct)
					( effect_list[idEffect][idChannel] );
				effect_list[idEffect][idChannel] = 0 ;
			}
		}
	}

// other
	free_instruments_afterwards = GetIniInt(CFG_FREE_INSTR,1);
	bTrustWinAudioSpRate = GetIniInt(CFG_TRUST_SMP_RATE,1);
	ctl->verbosity = GetIniInt(CFG_VERBOSITYLEVEL,VERB_NORMAL);

	return  isOk;		
}

BOOL SaveIniFile()
{
	BOOL isOk = TRUE ;
	BOOL bFlag ;
	int idEffect ;

	SetIniStr(CFG_FILE_NAME,pszTimidCfgPath);
	SetIniInt(CFG_PLAY_RATE,play_mode->rate);
	SetIniInt(CFG_CBFRAGMENTS,play_mode->extra_param[0]);
	if( play_mode->encoding & PE_MONO )
		bFlag = FALSE;
	else
		bFlag = TRUE ;
	SetIniInt(CFG_STEREO,bFlag );

	SetIniInt(CFG_ANTIALIAS,antialiasing_allowed);
	SetIniInt(GFG_AMPLIFICATION,amplification);
	
	SetIniInt(GFG_FAST_DECAY,fast_decay);
	SetIniInt(CFG_CTRL_PER_SEC,iCtrlPerSecond ) ;
	control_ratio = play_mode->rate / iCtrlPerSecond ;
	SetIniInt(CFG_PROMPT_PANNING,adjust_panning_immediately );
	SetIniStr( CFG_DEFAULT_INST , def_instr_name );
	SetIniInt(CFG_VOICES,voices);
	SetIniInt( CFG_CHANNEL_EFFECT , active_effect );

	for( idEffect = 0 ; effect_type_list[ idEffect ] != 0 ; ++idEffect )
	{
		if( effect_list[idEffect][0] == 0 )
			SetIniInt( effect_name[ idEffect ] , 0 ) ;
		else
			SetIniInt( effect_name[ idEffect ] , 1 ) ;
	}

	SetIniInt(CFG_FREE_INSTR,free_instruments_afterwards );
	SetIniInt(CFG_TRUST_SMP_RATE,bTrustWinAudioSpRate);
	SetIniInt(CFG_VERBOSITYLEVEL,ctl->verbosity);
	SetIniInt(CFG_SHOWOUTPUTWND,bShowOutput);

	return isOk;
}

BOOL InitTimidity()
{
	BOOL bOk = TRUE ;
	FILE* psTextIni;
	char_buff pszAbsPathIniFile ; 

// load config file	
	if(!LoadIniFile())
		DisplayMessage("Unable to load timidity95 configuration");

// create ini file if doesn't exist
	GetWindowsDirectory(pszAbsPathIniFile,MAX_PATH+32);
	strcat(pszAbsPathIniFile,"\\"TIMID_INI_FILENAME);
	if((psTextIni=fopen(pszAbsPathIniFile,"r"))==0)
		SaveIniFile();
	else
		fclose(psTextIni);
	effect_activate( active_effect ) ;

	return bOk;
}


#endif /* __WIN95__ */

