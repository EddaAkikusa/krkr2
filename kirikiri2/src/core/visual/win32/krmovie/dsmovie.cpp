//---------------------------------------------------------------------------
// ( part of KRMOVIE.DLL )
// (c)2001-2004, W.Dee <dee@kikyou.info>
//---------------------------------------------------------------------------

/*
	We must separate this module because sucking MS library has a lack of
	compiler portability.

	This requires DirectX7 or later or Windows Media Player 6.4 or later for
	playbacking MPEG streams.


	Modified by T.Imoto <http://www.kaede-software.com>
*/

#include "dsmovie.h"
#include "CIStream.h"

#ifdef _DEBUG
#include "DShowException.h"
#endif

tTVPDSMovie::tTVPDSMovie()
{
#ifdef _DEBUG
	m_dwROTReg = 0xfedcba98;
	m_RegisteredROT = false;
#endif

	CoInitialize(NULL);
	OwnerWindow = NULL;
	Visible = false;
	Rect.left = 0; Rect.top = 0; Rect.right = 320; Rect.bottom = 240;
	RefCount = 1;
	Shutdown = false;
}
tTVPDSMovie::~tTVPDSMovie()
{
	Shutdown = true;
	ReleaseAll();
	CoUninitialize();
}
//----------------------------------------------------------------------------
//! @brief	  	�Q�ƃJ�E���^�̃C���N�������g
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::AddRef()
{
	RefCount++;
}
//----------------------------------------------------------------------------
//! @brief	  	�Q�ƃJ�E���^�̃f�N�������g�B1�Ȃ�delete�B
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::Release()
{
	if(RefCount == 1)
		delete this;
	else
		RefCount--;
}
//----------------------------------------------------------------------------
//! @brief	  	�C���^�[�t�F�C�X���������
//!
//! �f�o�b�N���AROT�ɂ܂��o�^����Ă���ꍇ�́A�����œo�^����������B@n
//! �������A�{���͂��̃N���X���p�������N���X�Ŏ��O�ɓo�^�������R�[�����������悢
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::ReleaseAll()
{
#ifdef _DEBUG
	if( m_RegisteredROT )	// �o�^���܂���������Ă��Ȃ����͂����ŉ���
		RemoveFromROT( m_dwROTReg );
#endif

	if( m_MediaControl.p != NULL )
	{
		m_MediaControl->Stop();
		m_MediaControl.Release();
	}
	if( m_MediaPosition.p != NULL )
		m_MediaPosition.Release();

	if( m_MediaSeeking.p != NULL )
		m_MediaSeeking.Release();

	if( m_MediaEventEx.p != NULL )
		m_MediaEventEx.Release();

	if( m_BasicVideo.p != NULL )
		m_BasicVideo.Release();

	if( m_GraphBuilder.p != NULL )
		m_GraphBuilder.Release();

	if( m_Proxy )
	{
		delete m_Proxy;
		m_Proxy = NULL;
	}
}
//----------------------------------------------------------------------------
//! @brief	  	�r�f�I���Đ�����
//! @return		�G���[���b�Z�[�W�BNULL�̏ꍇ�G���[�Ȃ�
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::Play()
{
	HRESULT	hr;
	if( FAILED(hr = Controller()->Run()) )
	{
#ifdef _DEBUG
		OutputDebugString( DShowException(hr).what() );
#endif
		return L"Failed to call IMediaControl::Run.";
	}
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	�r�f�I���~����
//! @return		�G���[���b�Z�[�W�BNULL�̏ꍇ�G���[�Ȃ�
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::Stop()
{
	HRESULT	hr;
	if( FAILED(hr = Controller()->Stop()) )
	{
#ifdef _DEBUG
		OutputDebugString( DShowException(hr).what() );
#endif
		return L"Failed to call IMediaControl::Stop.";
	}
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	�r�f�I���ꎞ��~����
//! @return		�G���[���b�Z�[�W�BNULL�̏ꍇ�G���[�Ȃ�
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::Pause()
{
	HRESULT	hr;
	if( FAILED(hr = Controller()->Pause()) )
	{
#ifdef _DEBUG
		OutputDebugString( DShowException(hr).what() );
#endif
		return L"Failed to call IMediaControl::Pause.";
	}
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	���݂̃��[�r�[���Ԃ�ݒ肷��
//! @param 		tick : �ݒ肷�錻�݂̎���
//! @return		�G���[���b�Z�[�W�BNULL�̏ꍇ�G���[�Ȃ�
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::SetPosition( unsigned __int64 tick )
{
	HRESULT	hr;
	if(Shutdown) return NULL;
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		return L"Failed to call IMediaSeeking::GetTimeFormat.";
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		LONGLONG	requestTime = (LONGLONG)(tick * 10000);
		if( FAILED(hr = MediaSeeking()->SetPositions( &requestTime, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame, NULL, AM_SEEKING_NoPositioning )) )
		{
			return L"Failed to call IMediaSeeking::SetPositions.";
		}
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			return L"Failed to call IBasicVideo::get_AvgTimePerFrame.";
		}
		LONGLONG	requestFrame = (LONGLONG)(((tick / 1000.0) / AvgTimePerFrame) + 0.5);
		if( FAILED(hr = MediaSeeking()->SetPositions( &requestFrame, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame, NULL, AM_SEEKING_NoPositioning )) )
		{
			return L"Failed to call IMediaSeeking::SetPositions.";
		}
	}
	else
	{
		return L"Not supported time format.";
	}
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	���݂̃��[�r�[���Ԃ��擾����
//! @param 		tick : ���݂̎��Ԃ�Ԃ��ϐ�
//! @return		�G���[���b�Z�[�W�BNULL�̏ꍇ�G���[�Ȃ�
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::GetPosition( unsigned __int64 *tick )
{
	if(Shutdown) return NULL;

	HRESULT		hr;
	LONGLONG	Current;
	if( FAILED(hr = MediaSeeking()->GetCurrentPosition( &Current ) ) )
	{
		return L"Failed to call IMediaSeeking::GetCurrentPosition.";
	}
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		return L"Failed to call IMediaSeeking::GetTimeFormat.";
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		LONGLONG	curTime = (Current + 5000) / 10000; // �ꉞ�A�l�̌ܓ����Ă���
		*tick = (unsigned __int64)( curTime < 0 ? 0 : curTime);
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			return L"Failed to call IBasicVideo::get_AvgTimePerFrame.";
		}
		LONGLONG	curTime = (LONGLONG)(Current * AvgTimePerFrame * 1000.0);
		*tick = (unsigned __int64)( curTime < 0 ? 0 : curTime);
	}
	else
	{
		return L"Not supported time format.";
	}

	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	���݂̃��[�r�[�̏�Ԃ��擾����
//! @param 		status : ���݂̏�Ԃ�Ԃ��ϐ�
//! @return		Always NULL.
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::GetStatus(tTVPVideoStatus *status)
{
	if(Shutdown) return NULL;
	HRESULT hr;
	OAFilterState state;
	hr = Controller()->GetState(50, &state);
	if(hr == VFW_S_STATE_INTERMEDIATE)
		*status = vsProcessing;
	else if(state == State_Stopped)
		*status = vsStopped;
	else if(state == State_Running)
		*status = vsPlaying;
	else if(state == State_Paused)
		*status = vsPaused;
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	A sample has been delivered. Copy it to the texture.
//! @param 		evcode : �C�x���g�R�[�h
//! @param 		param1 : �p�����[�^1�B���e�̓C�x���g�R�[�h�ɂ��قȂ�B
//! @param 		param2 : �p�����[�^2�B���e�̓C�x���g�R�[�h�ɂ��قȂ�B
//! @param 		got : �擾�̐���
//! @return		Always NULL.
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::GetEvent( long *evcode, long *param1, long *param2, bool *got )
{
	if(Shutdown) return NULL;
	HRESULT hr;
	*got = false;
	hr = Event()->GetEvent(evcode, param1, param2, 0);
	if(SUCCEEDED(hr)) *got = true;
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	�C�x���g���������
//! 
//! GetEvent�ŃC�x���g�𓾂āA����������A���̃��\�b�h�ɂ���ăC�x���g��������邱��
//! @param 		evcode : �������C�x���g�R�[�h
//! @param 		param1 : �������p�����[�^1�B���e�̓C�x���g�R�[�h�ɂ��قȂ�B
//! @param 		param2 : �������p�����[�^2�B���e�̓C�x���g�R�[�h�ɂ��قȂ�B
//! @return		Always NULL.
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::FreeEventParams(long evcode, long param1, long param2)
{
	if(Shutdown) return NULL;

	Event()->FreeEventParams(evcode, param1, param2);
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	���[�r�[���ŏ��̈ʒu�܂Ŋ����߂�
//! @return		�G���[���b�Z�[�W
//! @note		IMediaPosition�͔񐄏��̂悤�����A�T���v���ł͎g�p����Ă����̂ŁA
//! 			�����܂܂ɂ��Ă����B
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::Rewind()
{
	if(Shutdown) return NULL;

	HRESULT	hr;
	if( FAILED(hr = Position()->put_CurrentPosition(0)) )
	{
#ifdef _DEBUG
		OutputDebugString( DShowException(hr).what() );
#endif
		return L"Failed to call IMediaPosition::put_CurrentPosition.";
	}
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	�w�肳�ꂽ�t���[���ֈړ�����
//! 
//! ���̃��\�b�h�ɂ���Đݒ肳�ꂽ�ʒu�́A�w�肵���t���[���Ɗ��S�Ɉ�v����킯�ł͂Ȃ��B
//! �t���[���́A�w�肵���t���[���ɍł��߂��L�[�t���[���̈ʒu�ɐݒ肳���B
//! @param		f : �ړ�����t���[��
//! @return		�G���[���b�Z�[�W
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::SetFrame( int f )
{
	if(Shutdown) return NULL;

	HRESULT	hr;
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		return L"Failed to call IMediaSeeking::GetTimeFormat.";
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			return L"Failed to call IBasicVideo::get_AvgTimePerFrame.";
		}
		LONGLONG	requestTime = (LONGLONG)(AvgTimePerFrame * 10000000.0 * f);
		if( FAILED(hr = MediaSeeking()->SetPositions( &requestTime, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame, NULL, AM_SEEKING_NoPositioning )) )
		{
			return L"Failed to call IMediaSeeking::SetPositions.";
		}
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		LONGLONG	requestFrame = f;
		if( FAILED(hr = MediaSeeking()->SetPositions( &requestFrame, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame, NULL, AM_SEEKING_NoPositioning )) )
		{
			return L"Failed to call IMediaSeeking::SetPositions.";
		}
	}
	else
	{
		return L"Not supported time format.";
	}

	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	���݂̃t���[�����擾����
//! @param		f : ���݂̃t���[��������ϐ��ւ̃|�C���^
//! @return		�G���[���b�Z�[�W
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::GetFrame( int *f )
{
	if(Shutdown) return NULL;

	HRESULT		hr;
	LONGLONG	Current;
	if( FAILED(hr = MediaSeeking()->GetCurrentPosition( &Current ) ) )
	{
		return L"Failed to call IMediaSeeking::GetCurrentPosition.";
	}
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		return L"Failed to call IMediaSeeking::GetTimeFormat.";
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			return L"Failed to call IBasicVideo::get_AvgTimePerFrame.";
		}
		double	currentTime = Current / 10000000.0;
		*f = (int)(currentTime / AvgTimePerFrame + 0.5);
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		*f = (int)Current;
	}
	else
	{
		return L"Not supported time format.";
	}

	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	FPS���擾����
//! @param		f : FPS������ϐ��ւ̃|�C���^
//! @return		�G���[���b�Z�[�W
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::GetFPS( double *f )
{
	if(Shutdown) return NULL;

	HRESULT	hr;
	REFTIME	AvgTimePerFrame;
	if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
	{
		return L"Failed to call IBasicVideo::get_AvgTimePerFrame.";
	}
	*f = 1.0 / AvgTimePerFrame;
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	�S�t���[�������擾����
//! @param		f : �S�t���[����������ϐ��ւ̃|�C���^
//! @return		�G���[���b�Z�[�W
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::GetNumberOfFrame( int *f )
{
	if(Shutdown) return NULL;

	HRESULT	hr;
	LONGLONG	totalTime;
	if( FAILED(hr = MediaSeeking()->GetDuration( &totalTime )) )
	{
		return L"Failed to call IMediaSeeking::GetDuration.";
	}
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		return L"Failed to call IMediaSeeking::GetTimeFormat.";
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			return L"Failed to call IBasicVideo::get_AvgTimePerFrame.";
		}
		double	totalSec = totalTime / 10000000.0;
		*f = (int)(totalSec / AvgTimePerFrame + 0.5);
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		*f = (int)totalTime;
	}
	else
	{
		return L"Not supported time format.";
	}

	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	���[�r�[�̒���(msec)���擾����
//! @param		f : ���[�r�[�̒���������ϐ��ւ̃|�C���^
//! @return		�G���[���b�Z�[�W
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::GetTotalTime( __int64 *t )
{
	if(Shutdown) return NULL;

	HRESULT	hr;
	LONGLONG	totalTime;
	if( FAILED(hr = MediaSeeking()->GetDuration( &totalTime )) )
	{
		return L"Failed to call IMediaSeeking::GetDuration.";
	}
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		return L"Failed to call IMediaSeeking::GetTimeFormat.";
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		*t = (__int64)(totalTime / 10000.0 );
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			return L"Failed to call IBasicVideo::get_AvgTimePerFrame.";
		}
		// �t���[������b�ցA�b����msec��
		*t = (__int64)((totalTime * AvgTimePerFrame) * 1000.0 );
	}
	else
	{
		return L"Not supported time format.";
	}

	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	�r�f�I�̉摜�T�C�Y���擾����
//! @param		width : ��
//! @param		height : ����
//! @return		Always NULL
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::GetVideoSize( long *width, long *height )
{
	if( width != NULL )
		Video()->get_SourceWidth( width );

	if( height != NULL )
		Video()->get_SourceHeight( height );

	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	buff��NULL��ݒ肷��B
//! @return		Always NULL.
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::GetFrontBuffer( BYTE **buff )
{
	*buff = NULL;
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	�������Ȃ�
//! @return		Always NULL.
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::SetVideoBuffer( BYTE *buff1, BYTE *buff2, long size )
{
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	�������Ȃ��B
//! @return		Always NULL.
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::SetWindow( HWND window )
{
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	�������Ȃ��B
//! @return		Always NULL.
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::SetMessageDrainWindow( HWND window )
{
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	�������Ȃ��B
//! @return		Always NULL.
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::SetRect( RECT *rect )
{
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	�������Ȃ��B
//! @return		Always NULL.
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::SetVisible( bool b )
{
	return NULL;
}
#ifdef _DEBUG
//----------------------------------------------------------------------------
//! @brief	  	ROT ( Running Object Table )�ɃO���t��o�^����B
//!
//! Running Object Table functions: Used to debug. By registering the graph
//! in the running object table, GraphEdit is able to connect to the running
//! graph. This code should be removed before the application is shipped in
//! order to avoid third parties from spying on your graph.
//! @param		ROTreg : �o�^I.D.�B�Ȃ񂩁A������ۂ��̂�n���΂�����łȂ��́B@n
//! 				�T���v���ł�dwROTReg = 0xfedcba98�Ƃ����̂�n���Ă���B
//! @return		����
//----------------------------------------------------------------------------
HRESULT __stdcall tTVPDSMovie::AddToROT( DWORD ROTreg )
{
	IUnknown			*pUnkGraph = m_GraphBuilder;
	IMoniker			*pmk;
	IRunningObjectTable	*pirot;

	if( FAILED(GetRunningObjectTable(0, &pirot)) )
		return E_FAIL;

	WCHAR	wsz[256];
	wsprintfW(wsz, L"FilterGraph %08x  pid %08x", (DWORD_PTR) 0, GetCurrentProcessId());
	HRESULT hr = CreateItemMoniker(L"!", wsz, &pmk);
	if( SUCCEEDED(hr) )
	{
		hr = pirot->Register( 0, pUnkGraph, pmk, &ROTreg );
		m_RegisteredROT = true;
		pmk->Release();
	}
	pirot->Release();
	return hr;
}

//----------------------------------------------------------------------------
//! @brief	  	ROT ( Running Object Table )����O���t�̓o�^����������B
//! @param		ROTreg : AddToROT�œn�����̂Ɠ�������n���B
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::RemoveFromROT( DWORD ROTreg )
{
	IRunningObjectTable *pirot;
	if (SUCCEEDED( GetRunningObjectTable(0, &pirot) ))
	{
		pirot->Revoke( ROTreg );
		m_RegisteredROT = false;
		pirot->Release();
	}
}
#endif	// _DEBUG
//----------------------------------------------------------------------------
//! @brief	  	1�t���[���̕��ϕ\�����Ԃ��擾���܂�
//! @param		pAvgTimePerFrame : 1�t���[���̕��ϕ\������
//! @return		�G���[�R�[�h
//----------------------------------------------------------------------------
HRESULT __stdcall tTVPDSMovie::GetAvgTimePerFrame( REFTIME *pAvgTimePerFrame )
{
	return Video()->get_AvgTimePerFrame( pAvgTimePerFrame );
}
//----------------------------------------------------------------------------
//! @brief	  	�g���q���烀�[�r�[�̃^�C�v�𔻕ʂ��܂�
//! @param		mt : ���f�B�A�^�C�v��Ԃ��ϐ��ւ̎Q��
//! @param		type : ���[�r�[�t�@�C���̊g���q
//! @return		�G���[�R�[�h
//----------------------------------------------------------------------------
const wchar_t* tTVPDSMovie::ParseVideoType( CMediaType &mt, const wchar_t *type )
{
	// note: audio-less mpeg stream must have an extension of
	// ".mpv" .
	if      (wcsicmp(type, L".mpg") == 0)
		mt.subtype = MEDIASUBTYPE_MPEG1System;
	else if (wcsicmp(type, L".mpeg") == 0)
		mt.subtype = MEDIASUBTYPE_MPEG1System;
	else if (wcsicmp(type, L".mpv") == 0) 
//		mt.subtype = MEDIASUBTYPE_MPEG1Video;
		mt.subtype = MEDIASUBTYPE_MPEG1System;
	else if (wcsicmp(type, L".dat") == 0)
		mt.subtype = MEDIASUBTYPE_MPEG1VideoCD;
	else if (wcsicmp(type, L".avi") == 0)
		mt.subtype = MEDIASUBTYPE_Avi;
	else if (wcsicmp(type, L".mov") == 0)
		mt.subtype = MEDIASUBTYPE_QTMovie;
	else
		return L"Unknown video format extension."; // unknown format
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	MPEG1 �p�̃O���t���蓮�ō\�z����
//! @param		pRdr : �O���t�ɎQ�����Ă��郌���_�[�t�B���^
//! @param		pSrc : �O���t�ɎQ�����Ă���\�[�X�t�B���^
//! @param		useSound : �T�E���h���g�p����邩�ǂ���
//! @return		�G���[������
//----------------------------------------------------------------------------
const wchar_t* tTVPDSMovie::BuildMPEGGraph( IBaseFilter *pRdr, IBaseFilter *pSrc )
{
	HRESULT	hr;

	// Connect to MPEG 1 splitter filter
	CComPtr<IBaseFilter>	pMPEG1Splitter;	// for MPEG 1 splitter filter
	if( FAILED(hr = pMPEG1Splitter.CoCreateInstance(CLSID_MPEG1Splitter, NULL, CLSCTX_INPROC_SERVER)) )
		return L"Failed to create MPEG 1 splitter filter object.";
	if( FAILED(hr = GraphBuilder()->AddFilter(pMPEG1Splitter, L"MPEG-I Stream Splitter")) )
		return L"Failed to call IFilterGraph::AddFilter.";
	if( FAILED(hr = ConnectFilters( pSrc, pMPEG1Splitter )) )
		return L"Failed to call ConnectFilters.";

	// Connect to MPEG 1 video codec filter
	CComPtr<IBaseFilter>	pMPEGVideoCodec;	// for MPEG 1 video codec filter
	if( FAILED(hr = pMPEGVideoCodec.CoCreateInstance(CLSID_CMpegVideoCodec, NULL, CLSCTX_INPROC_SERVER)) )
		return L"Failed to create MPEG 1 video codec filter object.";
	if( FAILED(hr = GraphBuilder()->AddFilter(pMPEGVideoCodec, L"MPEG Video Decoder")) )
		return L"Failed to call IFilterGraph::AddFilter.";
	if( FAILED(hr = ConnectFilters( pMPEG1Splitter, pMPEGVideoCodec )) )
		return L"Failed to call ConnectFilters.";

	// Connect to render filter
	if( FAILED(hr = ConnectFilters( pMPEGVideoCodec, pRdr )) )
		return L"Failed to call ConnectFilters.";

	// Connect to MPEG audio codec filter
	CComPtr<IBaseFilter>	pMPEGAudioCodec;	// for MPEG audio codec filter
	if( FAILED(hr = pMPEGAudioCodec.CoCreateInstance(CLSID_CMpegAudioCodec, NULL, CLSCTX_INPROC_SERVER)) )
		return L"Failed to create MPEG audio codec filter object.";
	if( FAILED(hr = GraphBuilder()->AddFilter(pMPEGAudioCodec, L"MPEG Audio Decoder")) )
		return L"Failed to call IFilterGraph::AddFilter.";
	if( FAILED(hr = ConnectFilters( pMPEG1Splitter, pMPEGAudioCodec )) )
	{	// not have Audio.
		if( FAILED(hr = GraphBuilder()->RemoveFilter( pMPEGAudioCodec)) )
			return L"Failed to call IFilterGraph::RemoveFilter.";
		return NULL;
	}

	// Connect to DDS render filter
	CComPtr<IBaseFilter>	pDDSRenderer;	// for sound renderer filter
	if( FAILED(hr = pDDSRenderer.CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC_SERVER)) )
		return L"Failed to create sound render filter object.";
	if( FAILED(hr = GraphBuilder()->AddFilter(pDDSRenderer, L"Sound Renderer")) )
		return L"Failed to call IFilterGraph::AddFilter.";
	if( FAILED(hr = ConnectFilters( pMPEGAudioCodec, pDDSRenderer ) ) )
	{
		if( FAILED(hr = GraphBuilder()->RemoveFilter( pMPEGAudioCodec)) )
			return L"Failed to call IFilterGraph::RemoveFilter.";
		if( FAILED(hr = GraphBuilder()->RemoveFilter( pDDSRenderer)) )
			return L"Failed to call IFilterGraph::RemoveFilter.";
	}

	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	2�̃t�B���^�[��ڑ�����
//! @param		pFilterUpstream : �A�b�v�X�g���[���t�B���^
//! @param		pFilterDownstream : �_�E���X�g���[���t�B���^
//! @return		�G���[�R�[�h
//----------------------------------------------------------------------------
HRESULT tTVPDSMovie::ConnectFilters( IBaseFilter* pFilterUpstream, IBaseFilter* pFilterDownstream )
{
	HRESULT			hr = E_FAIL;
	CComPtr<IPin>	pIPinUpstream;
	PIN_INFO		PinInfoUpstream;
	PIN_INFO		PinInfoDownstream;
#if _DEBUG
	char debug[256];
#endif

	// validate passed in filters
	ASSERT(pFilterUpstream);
	ASSERT(pFilterDownstream);

	// grab upstream filter's enumerator
	CComPtr<IEnumPins> pIEnumPinsUpstream;
	if( FAILED(hr = pFilterUpstream->EnumPins(&pIEnumPinsUpstream)) )
		throw L"Failed to call IBaseFilter::EnumPins.";

	// iterate through upstream filter's pins
	while( pIEnumPinsUpstream->Next (1, &pIPinUpstream, 0) == S_OK )
	{
		if( FAILED(hr = pIPinUpstream->QueryPinInfo(&PinInfoUpstream)) )
			throw L"Failed to call IPin::QueryPinInfo.";
#if _DEBUG
		sprintf(debug, "upstream: %ls\n", PinInfoUpstream.achName);
		OutputDebugString(debug);
#endif

		CComPtr<IPin>	 pPinDown;
		pIPinUpstream->ConnectedTo( &pPinDown );

		// bail if pins are connected
		// otherwise check direction and connect
		if( (PINDIR_OUTPUT == PinInfoUpstream.dir) && (pPinDown == NULL) )
		{
			// grab downstream filter's enumerator
			CComPtr<IEnumPins>	pIEnumPinsDownstream;
			hr = pFilterDownstream->EnumPins (&pIEnumPinsDownstream);

			// iterate through downstream filter's pins
			CComPtr<IPin>	pIPinDownstream;
			while( pIEnumPinsDownstream->Next (1, &pIPinDownstream, 0) == S_OK )
			{
				// make sure it is an input pin
				if( SUCCEEDED(hr = pIPinDownstream->QueryPinInfo(&PinInfoDownstream)) )
				{
#if _DEBUG
					sprintf(debug, "    downstream: %ls\n", PinInfoDownstream.achName);
					OutputDebugString(debug);
#endif
					CComPtr<IPin>	 pPinUp;
					pIPinDownstream->ConnectedTo( &pPinUp );
					if( (PINDIR_INPUT == PinInfoDownstream.dir) && (pPinUp == NULL) )
					{
						if( SUCCEEDED(hr = m_GraphBuilder->ConnectDirect( pIPinUpstream, pIPinDownstream, NULL)) )
						{
							PinInfoDownstream.pFilter->Release();
							PinInfoUpstream.pFilter->Release();
							return S_OK;
						}
#if _DEBUG
						else
							OutputDebugString( DShowException(hr).what() );
#endif
					}
				}

				PinInfoDownstream.pFilter->Release();
				pIPinDownstream = NULL;
			} // while next downstream filter pin

			//We are now back into the upstream pin loop
		} // if output pin

		pIPinUpstream = NULL;
		PinInfoUpstream.pFilter->Release();
	} // while next upstream filter pin

	return E_FAIL;
}
