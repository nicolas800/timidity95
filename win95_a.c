/*

    TiMidity -- Experimental MIDI to WAVE converter
    Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
    
    win95_a.c
    based on Davide Morreti implementation
	 june 1996 Nicolas Witczak
    Functions to play sound on the Win32 audio driver (Win 95 or Win NT).

*/

#ifdef __WIN95__

#include "win95hlp.h"
#include "win95cfg.h"
#include "win95_i.h"

#include "config.h"
#include "output.h"
#include "controls.h"


static int open_output(void); /* 0=success, 1=warning, -1=fatal error */
static void close_output(void);
static void output_data(int32 *buf, int32 count);
static int prepareStack();
 
static WAVEHDR* Pop();
static void	Push(WAVEHDR* pwhHeader);

static int openWinDrv();

static void CALLBACK wave95_callback (HWAVE hWave, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

static void flush_output(void);

static void purge_output(void);

/* export the playback mode */

#define dpm win95_play_mode

#define AUDIO_QUARTZ 1000000

PlayMode dpm = 
{
	DEFAULT_RATE, PE_16BIT|PE_SIGNED,
	-1,
	{16},
	"Win95 audio driver", 'd',
	NULL,
	open_output,
	close_output,
	output_data,
	flush_output,
	purge_output
};

/* additionnal data for win95 play mode */
static HWAVEOUT dev = 0 ;			/* device handle*/
static WAVEHDR** ppsWHStack = 0; /* stack of free wave headers */  
static int idxTop ;  /* points on the top (passed 
									last valid header) of the stack */ 
CRITICAL_SECTION sAccessStack ;
static DWORD dwFragDuration ;	/* duration of 1 fragment buffer in ms*/
static int cbAudioDrvBuff ; /* size of an audio fragment buffer in bit*/
static int cbdwAudioDrvBuff ; /* size of an audio fragment buffer in dwords*/
static int cbAudioDrvSample  ; /* number of samples in an audio fragment buffer */ 


static int openWinDrv()
{
	int i, j, mono, eight_bit;
	PCMWAVEFORMAT pcm;
	MMRESULT res;

	/* Check if there is at least one audio device */
	if(!waveOutGetNumDevs ())
		{
			ctl->cmsg (CMSG_ERROR, VERB_NORMAL, "No audio devices present!");
			return -1;
		}

	/* They can't mean these */
	dpm.encoding &= ~(PE_ULAW|PE_BYTESWAP);

	if(dpm.encoding & PE_16BIT)
		dpm.encoding |= PE_SIGNED;
	else
		dpm.encoding &= ~PE_SIGNED;

	mono = (dpm.encoding & PE_MONO);
	eight_bit = !(dpm.encoding & PE_16BIT);

	pcm.wf.wFormatTag = WAVE_FORMAT_PCM;
	pcm.wf.nChannels = mono ? 1 : 2;
	pcm.wf.nSamplesPerSec = i = dpm.rate;
	j = 1;
	if(!mono)
	{
		i *= 2;
		j *= 2;
	}
	if(!eight_bit)
	{
		i *= 2;
		j *= 2;
	}
	pcm.wf.nAvgBytesPerSec = i;
	pcm.wf.nBlockAlign = j;
	pcm.wBitsPerSample = eight_bit ? 8 : 16;

	res = waveOutOpen (NULL, 0, (LPWAVEFORMATEX)&pcm, 0 , 0, WAVE_FORMAT_QUERY);

	if(res)
	{
		ctl->cmsg (CMSG_ERROR, VERB_NORMAL, "Format not supported!");
		return -1;
	}

	res = waveOutOpen (&dev, 0, (LPWAVEFORMATEX)&pcm, 
		(DWORD)wave95_callback, 0, CALLBACK_FUNCTION);

	if(res)
	{
		ctl->cmsg(CMSG_ERROR, VERB_NORMAL, "Can't open audio device");
		return -1;
	}
	// treat the option trust audio driver sample rate

	if(!bTrustWinAudioSpRate)
		dpm.rate = AUDIO_QUARTZ 
			/ ClosestIntCelling( ((double)AUDIO_QUARTZ) / dpm.rate) ;
	return 0;		
}

static int prepareStack()
{
	int iDx ;
	HGLOBAL hWavHdr , hData;
	
	ppsWHStack = calloc( dpm.extra_param[0] + 1, sizeof(WAVEHDR*) );
	if( ppsWHStack == 0)
		return -1 ;
	
	cbAudioDrvBuff = AUDIO_BUFFER_SIZE ; 
	cbAudioDrvSample = AUDIO_BUFFER_SIZE ;  
	if( dpm.encoding & PE_16BIT )
		cbAudioDrvBuff *= 2; 
	if( !(dpm.encoding & PE_MONO) )
	{
		cbAudioDrvBuff *= 2;
		cbAudioDrvSample*= 2; 
	}
	cbdwAudioDrvBuff = cbAudioDrvBuff / 4 ;
	 
	for( iDx=0 ; iDx < dpm.extra_param[0] ; iDx++ )
	{
		hWavHdr = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_SHARE
			, sizeof (WAVEHDR));	
		ppsWHStack[iDx] = GlobalLock(hWavHdr);
		if( ppsWHStack[iDx] == 0 )
			return -1 ;
		hData = GlobalAlloc ( GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_SHARE
			, cbAudioDrvBuff ) ;
		if( (ppsWHStack[iDx]->lpData = GlobalLock(hData)) == 0 )
			return -1 ;
		ppsWHStack[iDx]->dwBufferLength = cbAudioDrvBuff ;				
		waveOutUnprepareHeader (dev,ppsWHStack[iDx] , sizeof (WAVEHDR));
	}
	idxTop =  dpm.extra_param[0] ;
	ppsWHStack[idxTop] = 0 ;
	return 0 ;
}

static void FreeStack()
{
	int iDx ;
	HGLOBAL hWavHdr , hData;
	for( iDx=0 ; iDx < dpm.extra_param[0] ; iDx++ )
	{
		if( ppsWHStack[iDx] != 0 )
		{
			waveOutUnprepareHeader (dev,ppsWHStack[iDx] , sizeof (WAVEHDR));
			hData = GlobalHandle(ppsWHStack[iDx]->lpData);
			GlobalUnlock (hData);
			GlobalFree (hData);
			hWavHdr = GlobalHandle(ppsWHStack[iDx]);
			GlobalUnlock(hWavHdr);
			GlobalFree(hWavHdr);
			ppsWHStack[iDx] = 0 ;				
		}
	}
	if( ppsWHStack != 0 )
		free( ppsWHStack ) ;
	ppsWHStack = 0 ;
}

static  WAVEHDR* Pop()
{
	WAVEHDR* pRetHeader ;

keep_trying:
	EnterCriticalSection( &sAccessStack );
	if( idxTop == 0 )				
	{
		LeaveCriticalSection( &sAccessStack );
		Sleep( dwFragDuration );
		goto keep_trying ;  /* sorry for the goto ( better idea ? ) */
	}
	pRetHeader = ppsWHStack[--idxTop];
	ppsWHStack[idxTop] = 0 ;
	LeaveCriticalSection( &sAccessStack );		
	waveOutPrepareHeader (dev, pRetHeader , sizeof (WAVEHDR));			
	
	return pRetHeader ;
}

static void	Push(WAVEHDR* pwhHeader)
{
	waveOutUnprepareHeader (dev , pwhHeader , sizeof (WAVEHDR));
	EnterCriticalSection(&sAccessStack);
	ppsWHStack[idxTop++] = pwhHeader ;
	LeaveCriticalSection( &sAccessStack );			
}

static int open_output (void)
{
	static BOOL bFirst = TRUE ;
	/** don't considere 1st time opening in main */
	if( bFirst )
	{
		bFirst = FALSE ;
		return 0 ;
	}
	dwFragDuration = (AUDIO_BUFFER_SIZE * 1000 * 2 ) / (dpm.rate*3) ;
	if( openWinDrv() == -1 )
	{
		dev = 0 ;
		return -1 ;
	}
	if( prepareStack() == -1 )
	{
		waveOutClose(dev);
		dev = 0 ;
		return -1 ;
	}
	InitializeCriticalSection(&sAccessStack);
	return 0;	
}

static void output_data (int32 *buf, int32 count)
{
	WAVEHDR* pHeader ;
	DWORD* pdwCur, *pdwLast ;		
	pHeader = Pop();
	if (dpm.encoding & PE_16BIT)
	/* Convert data to signed 16-bit PCM */
		s32tos16 (buf, cbAudioDrvSample);
	else
	/* Convert to 8-bit unsigned. */
		s32tou8 (buf, cbAudioDrvSample);
	
	pdwCur  = (DWORD*)(pHeader->lpData) ;
	pdwLast = pdwCur + cbdwAudioDrvBuff ;
	while( pdwCur < pdwLast )
	  *pdwCur++=*((DWORD*)buf)++ ;
	waveOutWrite (dev,pHeader , sizeof (WAVEHDR));
}

static void CALLBACK wave95_callback (HWAVE hWave, UINT uMsg, DWORD dwInstance, 
	DWORD dwParam1, DWORD dwParam2)
{
	if( ( uMsg == WOM_DONE ) && (bIsPlaying) )
	{	
		Push( (WAVEHDR *)dwParam1 ) ;			
	}
}

static void close_output (void)
{
	if( dev != 0 ) 
	{
		waveOutClose(dev);
		DeleteCriticalSection(&sAccessStack);
		FreeStack();
		dev = 0 ;
	}
}

static void flush_output (void)
{
	while( idxTop != dpm.extra_param[0] )
		Sleep( dwFragDuration );		
}

static void purge_output (void)
{
	waveOutReset (dev);
}

#endif /* __WIN95__ */
