/****************************************************************************/
/*! @file
@brief DirectShow�𗘗p�������[�r�[�̃I�[�o�[���C�Đ�

-----------------------------------------------------------------------------
	Copyright (C) 2004 T.Imoto
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2004/08/25
@note
			2004/08/25	T.Imoto		
*****************************************************************************/

#ifndef __DSOVERLAY_H__
#define __DSOVERLAY_H__

#include "dsmovie.h"

//----------------------------------------------------------------------------
//! @brief �I�[�o�[���C �r�f�I�`��N���X
//----------------------------------------------------------------------------
class tTVPDSVideoOverlay : public tTVPDSMovie
{
private:
	CComPtr<IVideoWindow>	m_VideoWindow;
	CComPtr<IBasicAudio>	m_BasicAudio;

	//----------------------------------------------------------------------------
	//! @brief	  	IVideoWindow���擾����
	//! @return		IVideoWindow�C���^�[�t�F�C�X
	//----------------------------------------------------------------------------
	IVideoWindow *VideoWindow()
	{
		assert( m_VideoWindow.p );
		return m_VideoWindow;
	}
	//----------------------------------------------------------------------------
	//! @brief	  	IBasicAudio���擾����
	//! @return		IBasicAudio�C���^�[�t�F�C�X
	//----------------------------------------------------------------------------
	IBasicAudio *Audio()
	{
		assert( m_BasicAudio.p );
		return m_BasicAudio;
	}

public:
	tTVPDSVideoOverlay();
	virtual ~tTVPDSVideoOverlay();

	virtual const wchar_t* __stdcall BuildGraph( HWND callbackwin, IStream *stream,
	const wchar_t * streamname, const wchar_t *type, unsigned __int64 size );

	virtual void __stdcall ReleaseAll();

	virtual const wchar_t* __stdcall SetWindow(HWND window);
	virtual const wchar_t* __stdcall SetMessageDrainWindow(HWND window);
	virtual const wchar_t* __stdcall SetRect(RECT *rect);
	virtual const wchar_t* __stdcall SetVisible(bool b);

	virtual const wchar_t* __stdcall Play();
};

#endif	// __DSOVERLAY_H__
