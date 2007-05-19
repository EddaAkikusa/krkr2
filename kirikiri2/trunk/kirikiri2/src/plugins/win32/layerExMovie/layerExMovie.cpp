#pragma comment(lib, "strmiids.lib")
#include "layerExMovie.hpp"

void ParseVideoType( CMediaType &mt, const wchar_t *type )
{
	// note: audio-less mpeg stream must have an extension of
	// ".mpv" .
	if      (wcsicmp(type, L".mpg") == 0)
		mt.subtype = MEDIASUBTYPE_MPEG1System;
	else if (wcsicmp(type, L".mpeg") == 0)
		mt.subtype = MEDIASUBTYPE_MPEG1System;
	else if (wcsicmp(type, L".mpv") == 0) 
		mt.subtype = MEDIASUBTYPE_MPEG1Video;
//		mt.subtype = MEDIASUBTYPE_MPEG1System;
	else if (wcsicmp(type, L".m1v") == 0) 
		mt.subtype = MEDIASUBTYPE_MPEG1Video;
	else if (wcsicmp(type, L".dat") == 0)
		mt.subtype = MEDIASUBTYPE_MPEG1VideoCD;
	else if (wcsicmp(type, L".avi") == 0)
		mt.subtype = MEDIASUBTYPE_Avi;
	else if (wcsicmp(type, L".mov") == 0)
		mt.subtype = MEDIASUBTYPE_QTMovie;
//	else if (wcsicmp(type, L".mp4") == 0)
//		mt.subtype = MEDIASUBTYPE_QTMovie;
//	else if (wcsicmp(type, L".wmv") == 0)
//		mt.subtype = SubTypeGUID_WMV3;
	else
		TVPThrowExceptionMessage(L"Unknown video format extension."); // unknown format
}

/**
 * �R���X�g���N�^
 */
layerExMovie::layerExMovie(DispatchT obj) : _pType(obj, TJS_W("type")), layerExBase(obj)
{
	pAMStream         = NULL;
	pPrimaryVidStream = NULL;
	pDDStream         = NULL;
	pSample           = NULL; 
	pSurface          = NULL;
	loop = false;
	alpha = false;
	movieWidth = 0;
	movieHeight = 0;
	m_Proxy = NULL;
	m_Reader = NULL;
	in = NULL;
	{
		tTJSVariant var;
		if (TJS_SUCCEEDED(obj->PropGet(TJS_IGNOREPROP, L"onStartMovie", NULL, &var, obj))) onStartMovie = var;
		else onStartMovie = NULL;
		if (TJS_SUCCEEDED(obj->PropGet(TJS_IGNOREPROP, L"onStopMovie", NULL, &var, obj))) onStopMovie = var;
		else onStopMovie = NULL;
		if (TJS_SUCCEEDED(obj->PropGet(TJS_IGNOREPROP, L"onUpdateMovie", NULL, &var, obj))) onUpdateMovie = var;
		else onUpdateMovie = NULL;
	}
	playing = false;
}

/**
 * �f�X�g���N�^
 */
layerExMovie::~layerExMovie()
{
	stopMovie();
}

void
layerExMovie::clearMovie()
{ 
	if (pAMStream) {
		pAMStream->SetState(STREAMSTATE_STOP);
	}
	if (pSurface) {
		pSurface->Release();
		pSurface = NULL;
	}
	if (pSample) {
		pSample->Release();
		pSample = NULL;
	}
	if (pDDStream) {
		pDDStream->Release();
		pDDStream = NULL;
	}
	if (pPrimaryVidStream) {
		pPrimaryVidStream->Release();
		pPrimaryVidStream = NULL;
	}
	if (pAMStream) {
		pAMStream->Release();
		pAMStream = NULL;
	}
	if (m_Reader) {
		m_Reader->Release();
		m_Reader = NULL;
	}
	if( m_Proxy ) {
		delete m_Proxy;
		m_Proxy = NULL;
	}
	if (in) {
		in->Release();
		in = NULL;
	}
	//DeleteFileW(tempFile.c_str());
	//tempFile = "";
}

/**
 * ���[�r�[�t�@�C�����J���ď�������
 * @param filename �t�@�C����
 * @param alpha �A���t�@�w��i�����̃T�C�Y�Ń���������j
 */
void
layerExMovie::openMovie(const tjs_char* filename, bool alpha)
{ 
	clearMovie();

	ttstr ext = TVPExtractStorageExt(filename);
	CMediaType mt;
	mt.majortype = MEDIATYPE_Stream;
	ParseVideoType(mt, ext.c_str()); // may throw an exception

	this->alpha = alpha;
	movieWidth = 0;
	movieHeight = 0;

	// �p�X����
	//ttstr path = TVPGetPlacedPath(ttstr(filename));
	// ���[�J�����ɕϊ�
	//TVPGetLocalName(path);

	// �t�@�C�����e���|�����ɃR�s�[���đΉ�
	if ((in = TVPCreateIStream(filename, GENERIC_READ)) == NULL) {
		ttstr error = filename;
		error += ":�t�@�C�����J���܂���";
		TVPAddLog(error);
		return;
	}
	DWORD size;
	STATSTG stat;
	in->Stat(&stat, STATFLAG_NONAME);
	size = stat.cbSize.LowPart;

#if 0		
	tempFile = TVPGetTemporaryName();
	HANDLE hFile;
	if ((hFile = CreateFile(tempFile.c_str(), GENERIC_WRITE, 0, NULL,
							CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL)) == INVALID_HANDLE_VALUE) {
		TVPAddLog("�e���|�����t�@�C�����J���܂���");
		in->Release();
		tempFile = "";
		return;
	}
	// �t�@�C�����R�s�[
	BYTE buffer[1024*16];
	DWORD size;
	while (in->Read(buffer, sizeof buffer, &size) == S_OK && size > 0) {			
		WriteFile(hFile, buffer, size, &size, NULL);
	}
	CloseHandle(hFile);
#endif
	
	if (SUCCEEDED(CoCreateInstance(CLSID_AMMultiMediaStream,0,1,IID_IAMMultiMediaStream,(void**)&pAMStream))) {
		if (SUCCEEDED(pAMStream->Initialize(STREAMTYPE_READ, 0, NULL)) &&
			SUCCEEDED(pAMStream->AddMediaStream(0, &MSPID_PrimaryVideo, 0, NULL))) {
			IGraphBuilder *builder;
			if (SUCCEEDED(pAMStream->GetFilterGraph(&builder))) {
				m_Proxy = new CIStreamProxy(in, size);
				HRESULT hr;
				m_Reader = new CIStreamReader( m_Proxy, &mt, &hr );
				m_Reader->AddRef();
				builder->AddFilter(m_Reader, L"SourceFilter");
				builder->Render(m_Reader->GetPin(0));
				builder->Release();
			}

			//if (SUCCEEDED(pAMStream->OpenFile(tempFile.c_str(), AMMSF_NOCLOCK))) {
			if (SUCCEEDED(pAMStream->GetMediaStream(MSPID_PrimaryVideo, &pPrimaryVidStream))) {
				if (SUCCEEDED(pPrimaryVidStream->QueryInterface(IID_IDirectDrawMediaStream,(void**)&pDDStream))) {
					
					// �t�H�[�}�b�g�̎w�� ARGB32
					DDSURFACEDESC desc;
					ZeroMemory(&desc, sizeof DDSURFACEDESC);
					desc.dwSize = sizeof(DDSURFACEDESC);
					desc.dwFlags = DDSD_CAPS | DDSD_PIXELFORMAT;
					desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
					desc.ddpfPixelFormat.dwSize  = sizeof(DDPIXELFORMAT);
					desc.ddpfPixelFormat.dwFlags = DDPF_RGB; 
					desc.ddpfPixelFormat.dwRGBBitCount = 32;
					desc.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
					desc.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
					desc.ddpfPixelFormat.dwBBitMask = 0x000000FF;
					//						desc.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;
					HRESULT hr;
					if (SUCCEEDED(hr = pDDStream->SetFormat(&desc, NULL))) {
						if (SUCCEEDED(pDDStream->CreateSample(0,0,0,&pSample))) {
							RECT rect; 
							if (SUCCEEDED(pSample->GetSurface(&pSurface,&rect))) {
								movieWidth  = rect.right - rect.left;
								movieHeight = rect.bottom - rect.top;
								if (alpha) {
									movieWidth /= 2;
								}
								// �摜�T�C�Y�����[�r�[�̃T�C�Y�ɂ��킹��
								_pWidth.SetValue(movieWidth);
								_pHeight.SetValue(movieHeight);
								_pType.SetValue(alpha ? ltAlpha : ltOpaque);
							} else {
								// �T�[�t�F�[�X�擾���s
								pSample->Release();
								pSample = NULL;
								pDDStream->Release();
								pDDStream = NULL;
								pPrimaryVidStream->Release();
								pPrimaryVidStream = NULL;
								pAMStream->Release();
								pAMStream = NULL;
							}
						}
					} else {
						// �T���v���쐻���s
						pDDStream->Release();
						pDDStream = NULL;
						pPrimaryVidStream->Release();
						pPrimaryVidStream = NULL;
						pAMStream->Release();
						pAMStream = NULL;
					}
				} else {
					// DirectDraw �X�g���[���擾���s
					pPrimaryVidStream->Release();
					pPrimaryVidStream = NULL;
					pAMStream->Release();
					pAMStream = NULL;
				}
			} else {
				// �r�f�I�X�g���[���擾���s
				pAMStream->Release();
				pAMStream = NULL;
			}
		} else {
			pAMStream->Release();
			pAMStream = NULL;
		}
	}
}

/**
 * ���[�r�[�̊J�n
 */
void
layerExMovie::startMovie(bool loop)
{
	if (pSample) {
		// �Đ��J�n
		this->loop = loop;
		pAMStream->SetState(STREAMSTATE_RUN);
		pSample->Update(SSUPDATE_ASYNC, NULL, NULL, 0);
		start();
		if (onStartMovie != NULL) {
			onStartMovie->FuncCall(0, NULL, NULL, NULL, 0, NULL, _obj);
		}
	}
}

/**
 * ���[�r�[�̒�~
 */
void
layerExMovie::stopMovie()
{
	bool p = playing;
	stop();
	clearMovie();
	if (p) {
		if (onStopMovie != NULL) {
			onStopMovie->FuncCall(0, NULL, NULL, NULL, 0, NULL, _obj);
		}
	}
}

bool
layerExMovie::isPlayingMovie()
{
	return playing;
}

void
layerExMovie::start()
{
	stop();
	TVPAddContinuousEventHook(this);
	playing = true;
}

/**
 * Irrlicht �Ăяo��������~
 */
void
layerExMovie::stop()
{
	TVPRemoveContinuousEventHook(this);
	playing = false;
}

void TJS_INTF_METHOD
layerExMovie::OnContinuousCallback(tjs_uint64 tick)
{
	if (pSample) {
		// �X�V
		HRESULT hr = pSample->CompletionStatus(0,0);
		if (hr == MS_S_PENDING) {
//			TVPAddLog("�X�V�҂�");
		} else if (hr == S_OK) {
			// �X�V����
			// �T�[�t�F�[�X���烌�C���ɉ�ʃR�s�[
			reset();
			if (_buffer != NULL) {
				DDSURFACEDESC  ddsd; 
				ddsd.dwSize=sizeof(DDSURFACEDESC); 
				if (SUCCEEDED(pSurface->Lock( NULL,&ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT , NULL))) {
					if (alpha) {
						int w = movieWidth*4;
						for (int y=0; y<ddsd.dwHeight && y<_height; y++) {
							BYTE *dst  = _buffer+(y*_pitch);
							BYTE *src1 = (BYTE*)ddsd.lpSurface+y*ddsd.lPitch;
							BYTE *src2 = src1 + w;
							for (int x=0; x<_width;x++) {
								*dst++ = *src1++;
								*dst++ = *src1++;
								*dst++ = *src1++;
								*dst++ = *src2; src2+=4; src1++;
							}
						}
					} else {
						int w = _width < movieWidth ? _width * 4 : movieWidth * 4;
						for (int y=0; y<_height; y++) {
							memcpy(_buffer+(y*_pitch), (BYTE*)ddsd.lpSurface+y*ddsd.lPitch, w);
						}
					}
					pSurface->Unlock(NULL); 
				}
				//redraw();
				if (onUpdateMovie != NULL) {
					onUpdateMovie->FuncCall(0, NULL, NULL, NULL, 0, NULL, _obj);
				}
			}
			pSample->Update(SSUPDATE_ASYNC, NULL, NULL, 0);
		} else if (hr == MS_S_ENDOFSTREAM) {
			// �X�V�I��
			if (loop) {
				pAMStream->SetState(STREAMSTATE_STOP);
				pAMStream->SetState(STREAMSTATE_RUN);
				pSample->Update(SSUPDATE_ASYNC, NULL, NULL, 0);
			} else {
				stopMovie();
			}
		} else {
			stopMovie();
		}
	} else {
		stop();
	}
}
