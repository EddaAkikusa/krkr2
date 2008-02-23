#include "ncbind/ncbind.hpp"
#include <vfw.h>
#pragma comment (lib, "vfw32.lib")
#pragma comment (lib, "winmm.lib")

#include "../layerExText/layerExBase.hpp"

/**
 * ���C���摜/�����^��@�\
 */
class LayerExAVI : public layerExBase {

protected:
	// ����L�^�p�̏��
	PAVIFILE      pavi; //< AVI�t�@�C��
	PAVISTREAM    pstm; //< AVI�X�g���[��
	int aviWidth;  //< �J�n������
	int aviHeight; //< �J�n���c��
	int lastFrame; //< �ŏI�^��t���[��

protected:
	// �����L�^�p�̏��
	HWAVEIN hwi; //< �������̓n���h��
	WAVEHDR wvhdr; //< �ۑ��p�w�b�_
	IStream *wvout; //< �o�͐�
	
	/**
	 * �������̓R�[���o�b�N����
	 * @param hwi �������̓n���h��
	 * @param uMsg �R�}���h
	 * @param dwInstance �C���X�^���X���
	 * @param dwParam1 �p�����[�^1
	 * @param dwParam1 �p�����[�^2
	 */
	static void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
		LayerExAVI *owner = (LayerExAVI*)dwInstance;
		owner->waveIn(hwi, uMsg, dwParam1, dwParam2);
	}

	/**
	 * �������̓R�[���o�b�N
	 * @param hwi �������̓n���h��
	 * @param uMsg �R�}���h
	 * @param dwParam1 �p�����[�^1
	 * @param dwParam1 �p�����[�^2
	 */
	void waveIn(HWAVEIN hwi, UINT uMsg, DWORD dwParam1, DWORD dwParam2) {
		/* �C�x���g���� */
		switch(uMsg){
		case WIM_DATA:
			{
				LPWAVEHDR lpwvhdr = (LPWAVEHDR) dwParam1;
				ULONG s;
				wvout->Write(lpwvhdr->lpData, lpwvhdr->dwBufferLength, &s);
			}
			break;
		}
	}

public:
	/**
	 * �R���X�g���N�^
	 */
	LayerExAVI(DispatchT obj) : layerExBase(obj), pavi(NULL), pstm(NULL), hwi(NULL), wvout(NULL) {
		ZeroMemory(&wvhdr, sizeof wvhdr);
	}

	/**
	 * �f�X�g���N�^
	 */
	~LayerExAVI() {
		closeAVI();
	}

	/**
	 * AVI�t�@�C�����J��
	 * @param filename �ۑ��t�@�C����
	 * @param fps �b�ԃt���[����
	 */
	void openAVI(const tjs_char *filename, int fps) {

		closeAVI();

		// �^��J�n���̃T�C�Y���L�^���Ă���
		aviWidth  = _width;
		aviHeight = _height;

		// AVI�t�@�C�����J��
		ttstr path = TVPNormalizeStorageName(ttstr(filename));
		TVPGetLocalName(path);
		if (AVIFileOpen(&pavi, path.c_str(), OF_CREATE | OF_WRITE | OF_SHARE_DENY_NONE,NULL) != 0)	{
			ttstr msg = filename;
			msg += ":can't open";
			TVPThrowExceptionMessage(msg.c_str());
		}

		// AVI�X�g���[���̐���
		AVISTREAMINFO si = {	
			streamtypeVIDEO, // Video Stream
			comptypeDIB,
			0,               // Stream Flag
			0,
			0,
			0,
			1,               // ���ԒP�� dwScale
			fps,             // �t���[�� dwRate
			0,
			0,         // �X�g���[���̒��� XXX
			0,
			0,
			(DWORD)-1,       // -1: Default�i�� [0-10000]
			0,
			// �\�������`�T�C�Y
			{ 0, 0, aviWidth, aviHeight },
			0,
			0,
			L"KIRIKIRI" };

		if (AVIFileCreateStream(pavi, &pstm, &si) != 0) {
			closeAVI();
			TVPThrowExceptionMessage(L"AVIFileCreateStream");
		}

		// �X�g���[���ɓ������ރf�[�^�t�H�[�}�b�g���w��

		BITMAPINFOHEADER bih;
		bih.biSize = sizeof(bih);
		bih.biWidth  = aviWidth;
		bih.biHeight = aviHeight;
		bih.biPlanes = 1;
		bih.biBitCount = 32;
		bih.biCompression = BI_RGB;
		bih.biSizeImage = 0;
		bih.biXPelsPerMeter = 0;
		bih.biYPelsPerMeter = 0;
		bih.biClrUsed = 0;
		bih.biClrImportant = 0;

		if (AVIStreamSetFormat(pstm, 0, &bih, sizeof(bih)) != 0 ) {
			closeAVI();
			TVPThrowExceptionMessage(L"AVIFileCreateStream");
		}

		// �擪�t���[��
		lastFrame = -1;
	}

	/**
	 * AVI�t�@�C���Ƀf�[�^���L�^
	 */
	void recordAVI(int frame) {
		if (pavi && pstm) {
			if (frame > lastFrame) {
				// �T�C�Y���ς���Ă����O
				if (aviWidth != _width ||
					aviHeight != _height) {
					TVPThrowExceptionMessage(L"layer size has changed");
				}
				// �g���g���̃o�b�t�@�� DIB �Ɠ����\���Ȃ̂ł��̏����Œʂ�
				int size = _height * -_pitch;
				const unsigned char *buffer = _buffer + (_height-1) * _pitch;
				if (AVIStreamWrite(pstm, frame, 1, (void*)buffer, size, AVIIF_KEYFRAME, NULL, NULL ) != 0) {
					TVPThrowExceptionMessage(L"AVIStreamWrite");
				}
				lastFrame = frame;
			}
		} else {
			TVPThrowExceptionMessage(L"AVI file not opened");
		}
	}

	/**
	 * AVI�t�@�C�������
	 */
	void closeAVI() {
		if (pstm) {
			AVIStreamRelease(pstm);
			pstm = NULL;
		}
		if (pavi) {
			AVIFileRelease(pavi);
			pavi = NULL;
		}
	}

	/**
	 * WAV�^������
	 * @param filename �ۑ��t�@�C����
	 * @param channel �`�����l��
	 * @param rate ���[�g
	 * @param bits �r�b�g��
	 * @param interval �擾�^�C�~���O
	 */
	void openWAV(const tjs_char *filename, int channel, int rate, int bits, int interval) {

		closeWAV();
		
		// �t�@�C�����J��
		wvout = TVPCreateIStream(filename, TJS_BS_WRITE);
		
		// �t�H�[�}�b�g���w��
		WAVEFORMATEX waveForm;
		waveForm.wFormatTag      = WAVE_FORMAT_PCM;
		waveForm.nChannels       = channel;
		waveForm.nSamplesPerSec  = rate;
		waveForm.wBitsPerSample  = bits;
		waveForm.nBlockAlign     = waveForm.nChannels * waveForm.wBitsPerSample / 8;
		waveForm.nAvgBytesPerSec = waveForm.nSamplesPerSec * waveForm.nBlockAlign;
		
		// waveIn ���J��
		if (waveInOpen(&hwi, WAVE_MAPPER, &waveForm, (DWORD)waveInProc, (DWORD)this, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
			TVPThrowExceptionMessage(L"waveInOpen");
		}
		
		/* �L���v�`���o�b�t�@�m�� */
		int length = waveForm.nAvgBytesPerSec * interval / 1000;
		wvhdr.lpData         = new char[length];
		wvhdr.dwBufferLength = length;
		wvhdr.dwFlags        = 0;
		wvhdr.reserved       = 0;

		// �o�b�t�@��ݒ�
		waveInPrepareHeader(hwi, &wvhdr, sizeof(wvhdr));
		waveInAddBuffer(hwi, &wvhdr, sizeof(wvhdr));
	}

	/**
	 * WAV�^���J�n
	 */
	void startWAV() {
		if (hwi) {
			if (waveInStart(hwi) != MMSYSERR_NOERROR) {
				TVPThrowExceptionMessage(L"waveInStart");
			}
		}
	}

	/**
	 * WAV�^����~
	 */
	void stopWAV() {
		if (hwi) {
			if(waveInStop(hwi) != MMSYSERR_NOERROR) {
				TVPThrowExceptionMessage(L"waveInStop");
			}
		}
	}

	/**
	 * WAV�^���I��
	 */
	void closeWAV() {
		if (hwi) {
			waveInStop(hwi);
			waveInUnprepareHeader(hwi, &wvhdr, sizeof(wvhdr));
			waveInReset(hwi);
			waveInClose(hwi);
			hwi = NULL;
			// �o�b�t�@�N���A
			if (wvhdr.lpData) {
				delete[] wvhdr.lpData;
			}
			ZeroMemory(&wvhdr, sizeof wvhdr);
		}
		// �t�@�C���N���[�Y
		if (wvout) {
			wvout->Release();
			wvout = NULL;
		}
	}

};

NCB_GET_INSTANCE_HOOK(LayerExAVI)
{
	// �C���X�^���X�Q�b�^
	NCB_INSTANCE_GETTER(objthis) { // objthis �� iTJSDispatch2* �^�̈����Ƃ���
		ClassT* obj = GetNativeInstance(objthis);	// �l�C�e�B�u�C���X�^���X�|�C���^�擾
		if (!obj) {
			obj = new ClassT(objthis);				// �Ȃ��ꍇ�͐�������
			SetNativeInstance(objthis, obj);		// objthis �� obj ���l�C�e�B�u�C���X�^���X�Ƃ��ēo�^����
		}
		obj->reset();
		return obj;
	}
	// �f�X�g���N�^�i���ۂ̃��\�b�h���Ă΂ꂽ��ɌĂ΂��j
	~NCB_GET_INSTANCE_HOOK_CLASS () {
	}
};

NCB_ATTACH_CLASS_WITH_HOOK(LayerExAVI, Layer) {
	NCB_METHOD(openAVI);
	NCB_METHOD(closeAVI);
	NCB_METHOD(recordAVI);
	NCB_METHOD(openWAV);
	NCB_METHOD(startWAV);
	NCB_METHOD(stopWAV);
	NCB_METHOD(closeWAV);
}

// ----------------------------------- �N���E�J������

/**
 * �o�^�����O
 */
void PreRegistCallback()
{
	AVIFileInit();
}

/**
 * �J��������
 */
void PostUnregistCallback()
{
	AVIFileExit();
}

NCB_PRE_REGIST_CALLBACK(PreRegistCallback);
NCB_POST_UNREGIST_CALLBACK(PostUnregistCallback);
