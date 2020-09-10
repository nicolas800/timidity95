/****************************************************************************

resource.h

*****************************************************************************/

#ifndef DS_3DLOOK 
	#define DS_3DLOOK 0x0004L 
#endif

/**************************************************************************/
/**	 main windows
 */

#define TIMIDITY_MENU							1
#define OUTPUT_MENU								2

#define IDM_FILEOPEN							101
#define IDM_QUIT								102
#define IDM_CFGMIDI								103
#define IDM_CFGGEN								104
#define IDM_CFGOUTPUT							105
#define IDM_ABOUT								107
#define IDM_VOID								108
#define TIMIDITY_ICON							1
#define IDB_BUTTON								200

/**************************************************************************/
/**	message windows 
 */
#define DLG_MESSAGES				307

#define IDB_CLEARMESSAGES						1
#define IDB_DISMISS								2
#define IDE_MESSAGES							3

/**************************************************************************/
/**	about dialog
 */
#define DLG_ABOUT					300

#define IDB_INTERESTING							101

/**************************************************************************/
/**	interresting dialog dialog
 */
#define DLG_INTERSTING				301


/**************************************************************************/
/**	general conf dialog
 */
#define DLG_GENCONF					302

#define IDE_TIMIDCFGPATH						101
#define IDB_GETCFGPATH							102
#define IDC_FREEINSTR							103
#define IDE_VOLUME								104
#define IDC_VIEWOUTPUTMSG						105
#define IDE_VERBOSE								106


/**************************************************************************/
/**	output conf dialog
 */
#define DLG_OUTPUTCONF				303

#define IDE_SAMPLERATE							101
#define IDE_CBFRGMT								102
#define IDE_SZFRGMT								103
#define IDR_MONO								105
#define IDR_STEREO								106
#define IDR_16BIT								107
#define IDR_8BIT								108
#define IDE_OUTPUTPATH							109
#define IDB_GETOUTPUTPATH						110
#define IDC_TRUSTDRV							111

/**************************************************************************/
/**	midi conf dialog
 */
#define DLG_MIDICONF				304

#define IDE_CTRLRATE							102
#define IDC_FASTDECAY							103
#define IDC_FASTPANNING							104
#define IDC_ANTIALIASING						105
#define IDE_CBVOICE								106
#define IDB_MIDICHNLS							107
#define IDB_CHANNEL_EFFECT						108 

/**************************************************************************/
/**	midi channel conf dialog
 */
#define DLG_MIDICHANNELS			305
#define IDR_DRUMCHNL1							101
#define IDR_DRUMCHNL2							102
#define IDR_DRUMCHNL3							103
#define IDR_DRUMCHNL4							104
#define IDR_DRUMCHNL5							105
#define IDR_DRUMCHNL6							106
#define IDR_DRUMCHNL7							107
#define IDR_DRUMCHNL8							108
#define IDR_DRUMCHNL9							109
#define IDR_DRUMCHNL10							110
#define IDR_DRUMCHNL11							111
#define IDR_DRUMCHNL12							112
#define IDR_DRUMCHNL13							113
#define IDR_DRUMCHNL14							114
#define IDR_DRUMCHNL15							115
#define IDR_DRUMCHNL16							116

#define IDC_QUIET1								117
#define IDC_QUIET2								118
#define IDC_QUIET3								119
#define IDC_QUIET4								120
#define IDC_QUIET5								121
#define IDC_QUIET6								122
#define IDC_QUIET7								123
#define IDC_QUIET8								124
#define IDC_QUIET9								125
#define IDC_QUIET10								126
#define IDC_QUIET11								127
#define IDC_QUIET12								128
#define IDC_QUIET13								129
#define IDC_QUIET14								130
#define IDC_QUIET15								131
#define IDC_QUIET16								132


/**************************************************************************/
/**	midi effect conf dialog
 */
#define DLG_EFFECT					306

#define IDC_CHANNEL_EFFECT						101 
#define IDS_FRAME								102
#define ID_FIRST_EFFECT							103







