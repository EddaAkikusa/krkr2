//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <mmsystem.h>
#include "WaveReaderUnit.h"

#include "tvpsnd.h"

const static GUID __KSDATAFORMAT_SUBTYPE_IEEE_FLOAT =
		{ 0x00000003 , 0x0000, 0x0010, { 0x80,0x00,0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }};
const static GUID __KSDATAFORMAT_SUBTYPE_PCM =
		{ 0x00000001 , 0x0000, 0x0010, { 0x80,0x00,0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }};
//---------------------------------------------------------------------------
static void ConvertFloatTo16bits(short int *output, const float *input, int channels, int count)
{
	// �����_�ł� 32bit float �݂̂�ϊ�����

	// float PCM �� +1.0 �` 0 �` -1.0 �͈̔͂ɂ���
	// �͈͊O�̃f�[�^�̓N���b�v����
	int total = channels * count;
	while(total--)
	{
		float t = *(input ++) * 32768.0;
		if(t > 0)
		{
			int i = (int)(t + 0.5);
			if(i > 32767) i = 32767;
			*(output++) = (short int)i;
		}
		else
		{
			int i = (int)(t - 0.5);
			if(i < -32768) i = -32768;
			*(output++) = (short int)i;
		}
	}

}
//---------------------------------------------------------------------------
static void ConvertIntTo16bits(short int *output, const void *input, int bytespersample,
	int validbits, int channels, int count)
{
	// �{���� int16 �Ȃǂ̃T�C�Y�̌��܂����^�������ׂ�

	// �����\���� PCM �� 16bit �ɕϊ�����
	int total = channels * count;

	if(bytespersample == 1)
	{
		// �ǂݍ��ݎ��̃`�F�b�N�ł��ł� 8bit ���͂̃f�[�^�� �L���r�b�g���� 8 �r�b�g
		// �ł��邱�Ƃ��ۏ؂���Ă���̂ŒP�� 16bit �Ɋg�����邾���ɂ���
		const char *p = (const char *)input;
		while(total--) *(output++) = (short int)( ((int)*(p++)-0x80) * 0x100);
	}
	else if(bytespersample == 2)
	{
		// �L���r�b�g���ȉ��̐��l�� �}�X�N���Ď�苎��
		short int mask =  ~( (1 << (16 - validbits)) - 1);
		const short int *p = (const short int *)input;
		while(total--) *(output++) = (short int)(*(p++) & mask);
	}
	else if(bytespersample == 3)
	{
		long int mask = ~( (1 << (24 - validbits)) - 1);
		const unsigned char *p = (const unsigned char *)input;
		while(total--)
		{
			int t = p[0] + (p[1] << 8) + (p[2] << 16);
			p += 3;
			t |= -(t&0x800000); // �����g��
			t &= mask; // �}�X�N
			t >>= 8;
			*(output++) = (short int)t;
		}
	}
	else if(bytespersample == 4)
	{
		long int mask = ~( (1 << (32 - validbits)) - 1);
		const unsigned __int32 *p = (const unsigned __int32 *)input;
		while(total--)
		{
			*(output++) = (short int)((*(p++) & mask) >> 16);
		}
	}

}
//---------------------------------------------------------------------------
static void ConvertTo16bits(short int *output, const void *input, int bytespersample,
	bool isfloat, int validbits, int channels, int count)
{
	// �w�肳�ꂽ�t�H�[�}�b�g�� 16bit �t�H�[�}�b�g�ɕϊ�����
	// channels �� -6 �̏ꍇ�͓��ʂ�
	// FL FC FR BL BR LF �ƕ���ł���f�[�^ (AC3�Ɠ�������) ��
	// FL FR FC LF BL BR (WAVEFORMATEXTENSIBLE �̊��҂��鏇��) �ɕ��ёւ���
	int cns = channels;
	if(cns<0) cns = -cns;

	if(isfloat)
		ConvertFloatTo16bits(output, (const float *)input, cns, count);
	else
		ConvertIntTo16bits(output, input, bytespersample, validbits, cns, count);

	if(channels == -6)
	{
		// �`�����l���̓���ւ�
		short int *op = output;
		for(int i = 0; i < count; i++)
		{
			short int fc = op[1];
			short int bl = op[3];
			short int br = op[4];
			op[1] = op[2];
			op[2] = fc;
			op[3] = op[5];
			op[4] = bl;
			op[5] = br;
			op += 6;
		}
	}		
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
__fastcall TRIFFWaveContext::TRIFFWaveContext()
{
	FInputStream = NULL;
}
//---------------------------------------------------------------------------
__fastcall TRIFFWaveContext::~TRIFFWaveContext()
{
	if(FInputStream) delete FInputStream;
}
//---------------------------------------------------------------------------
bool __fastcall TRIFFWaveContext::Start(AnsiString filename)
{
	try
	{
		FInputStream = new TFileStream(filename, fmOpenRead | fmShareDenyWrite);
	}
	catch(...)
	{
		return false;
	}

	// �w�b�_��ǂ�
	BYTE buf[8];
	FInputStream->Position=0;
	FInputStream->ReadBuffer(buf, 8);
	if(memcmp(buf, "RIFF", 4)) return false; // RIFF ���Ȃ�
	FInputStream->ReadBuffer(buf, 8);
	if(memcmp(buf, "WAVEfmt ", 4)) return false; // WAVE �łȂ��� fmt �`�����N���������Ȃ�

	DWORD fmtlen;
	FInputStream->ReadBuffer(&fmtlen,4);

	FInputStream->ReadBuffer(&Format, sizeof(WAVEFORMATEX));
	if(Format.Format.wFormatTag != WAVE_FORMAT_EXTENSIBLE &&
		Format.Format.wFormatTag != WAVE_FORMAT_PCM &&
		Format.Format.wFormatTag != WAVE_FORMAT_IEEE_FLOAT) return false; // �Ή��ł��Ȃ��t�H�[�}�b�g

	if(Format.Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		FInputStream->ReadBuffer(
			(char*)&Format + sizeof(Format.Format),
			sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX));
	}
	else if(Format.Format.wFormatTag == WAVE_FORMAT_PCM)
	{
		Format.Samples.wValidBitsPerSample = Format.Format.wBitsPerSample;
		Format.SubFormat = __KSDATAFORMAT_SUBTYPE_PCM;
		Format.dwChannelMask = 0;
	}
	else if(Format.Format.wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
	{
		Format.Samples.wValidBitsPerSample = Format.Format.wBitsPerSample;
		Format.SubFormat = __KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
		Format.dwChannelMask = 0;
	}

	// �e��`�F�b�N
	if(Format.Format.wBitsPerSample & 0x07) return false; // 8 �̔{���ł͂Ȃ�
	if(Format.Format.wBitsPerSample > 32) return false; // 32bit ���傫���T�C�Y�͈����Ȃ�
	if(Format.Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
		Format.Samples.wValidBitsPerSample < 8) return false; // �L���r�b�g�� 8 �����͈����Ȃ�
	if(Format.Format.wBitsPerSample < Format.Samples.wValidBitsPerSample)
		return false; // �L���r�b�g�������ۂ̃r�b�g�����傫��
	if(!memcmp(&Format.SubFormat, &__KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 16))
		IsFloat = true;
	else if(!memcmp(&Format.SubFormat, &__KSDATAFORMAT_SUBTYPE_PCM, 16))
		IsFloat = false;
	else
		return false; // ���m�̃T�u�t�H�[�}�b�g

	if(IsFloat && Format.Format.wBitsPerSample != Format.Samples.wValidBitsPerSample)
		return false; // float �̗L���r�b�g�����i�[�r�b�g���Ɠ������Ȃ�
	if(IsFloat && Format.Format.wBitsPerSample != 32)
		return false; // float �̃r�b�g���� 32 �łȂ�

	// data �`�����N��T��
	FInputStream->Position= fmtlen + 0x14;

	while(1)
	{
		FInputStream->ReadBuffer(buf,4); // data ?
		FInputStream->ReadBuffer(&FDataSize,4); // �f�[�^�T�C�Y
		if(memcmp(buf,"data",4)==0) break;
		FInputStream->Position=FInputStream->Position+FDataSize;

		if(FInputStream->Position >= FInputStream->Size) return false;
	}

	FDataStart=FInputStream->Position;

	return true;
}
//---------------------------------------------------------------------------
int __fastcall TRIFFWaveContext::Read(__int16 * dest, int destgranules)
{
	int samplesize = Format.Format.wBitsPerSample / 8 * Format.Format.nChannels;
	destgranules *= samplesize; // data (in bytes) to read
	int bytestowrite;
	int remain = FDataStart + FDataSize - FInputStream->Position;
	int readsamples;

	char *buf = new char[destgranules];

	try
	{
		bytestowrite = (destgranules>remain) ? remain : destgranules;
		bytestowrite = FInputStream->Read(buf, bytestowrite);

		readsamples = bytestowrite / samplesize;

		// convert to the destination format
		ConvertTo16bits(dest, buf, Format.Format.wBitsPerSample / 8, IsFloat,
			Format.Samples.wValidBitsPerSample, Format.Format.nChannels, readsamples);
	}
	catch(...)
	{
		delete [] buf;
		throw;
	}
	delete [] buf;

	return readsamples;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class TStorageProviderImpl : public ITSSStorageProvider
{
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid,void **ppvObjOut)
	{
		if(!ppvObjOut) return E_INVALIDARG;

		*ppvObjOut=NULL;
		if(!memcmp(&iid,&IID_IUnknown,16))
			*ppvObjOut=(IUnknown*)this;
		else if(!memcmp(&iid,&IID_ITSSStorageProvider,16))
			*ppvObjOut=(ITSSStorageProvider*)this;

		if(*ppvObjOut)
		{
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	ULONG STDMETHODCALLTYPE AddRef(void) { return 1; }
	ULONG STDMETHODCALLTYPE Release(void) { return 1; }

	HRESULT _stdcall GetStreamForRead(LPWSTR url, IUnknown ** stream );
	HRESULT _stdcall GetStreamForWrite(LPWSTR url, IUnknown ** stream )
		{ return E_NOTIMPL; }
	HRESULT _stdcall GetStreamForUpdate(LPWSTR url, IUnknown ** stream )
		{ return E_NOTIMPL; }
} static StorageProvider;
//---------------------------------------------------------------------------
HRESULT _stdcall TStorageProviderImpl::GetStreamForRead(LPWSTR url, IUnknown ** stream )
{
	TStream *tvpstream;

	try
	{
		tvpstream = new TFileStream(AnsiString(url), fmOpenRead|fmShareDenyWrite);
	}
	catch(...)
	{
		return E_FAIL;
	}

	TStreamAdapter * adapter;
	adapter = new TStreamAdapter(tvpstream, soOwned);

	IUnknown *istream = (IUnknown*)(IStream*)(*adapter);
	*stream = istream;
	istream->AddRef();

	return S_OK;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TTSSWaveContext::TTSSWaveContext(AnsiString dllname)
{
	FModule = NULL;
	FHandle = NULL;
	FDecoder = NULL;

	FHandle = LoadLibrary(dllname.c_str());
	if(FHandle == NULL)
	{
		throw Exception(dllname + "��ǂݍ��ނ��Ƃ��ł��܂���");
	}

	FGetModuleInstance =(GetModuleInstanceProc)
		GetProcAddress(FHandle, "GetModuleInstance");
	FGetModuleThreadModel =(GetModuleThreadModelProc)
		GetProcAddress(FHandle, "GetModuleThreadModel");
	FShowConfigWindow =(ShowConfigWindowProc)
		GetProcAddress(FHandle, "ShowConfigWindow");
	FCanUnloadNow = (CanUnloadNowProc)
		GetProcAddress(FHandle, "CanUnloadNow");

	if(!FGetModuleInstance || FAILED(FGetModuleInstance(&FModule, &StorageProvider,
		NULL, Application->Handle)) )
	{
		FreeLibrary(FHandle);
		FHandle = NULL;
		throw Exception("�O�� DLL "+dllname+" �͎g�p�ł��܂���");
	}
}
//---------------------------------------------------------------------------
__fastcall TTSSWaveContext::~TTSSWaveContext()
{
	if(FDecoder) FDecoder->Release();
	if(FModule) FModule->Release();
	if(FHandle) FreeLibrary(FHandle);
}
//---------------------------------------------------------------------------
bool __fastcall TTSSWaveContext::Start(AnsiString filename)
{
	if(FDecoder) FDecoder->Release(), FDecoder = NULL;


	HRESULT hr;
	IUnknown * decoder;

	hr = FModule->GetMediaInstance(WideString(filename).c_bstr(), &decoder);
	if(FAILED(hr))
	{
		throw Exception(filename + " �͊J�����Ƃ��ł��܂���");
	}

	hr = decoder->QueryInterface(IID_ITSSWaveDecoder, (void**)&FDecoder);
	if(FAILED(hr))
	{
		decoder->Release();
		FDecoder = NULL;
		throw Exception(filename + " �̃��f�B�A�E�^�C�v�͈������Ƃ��ł��܂���");
	}

	decoder->Release();

	TSSWaveFormat format;
	FDecoder->GetFormat(&format);
	if(format.dwBitsPerSample != 16)
	{
		throw Exception(filename + " �� 16bit PCM �ɕϊ��ł��Ȃ����߈������Ƃ��ł��܂���");
	}
	if(format.dwChannels > 8)
	{
		throw Exception(filename + " �� 9�`���l���ȏ゠�邽�߈������Ƃ��ł��܂���");
	}
	FChannels = format.dwChannels;
	FFrequency = format.dwSamplesPerSec;
	FBitsPerSample = 16;
	FSpeakerConfig = 0; // �����_�ł͏�� 0 (�Ȃɂ��K�i�����邩��)

	FGranuleSize = FChannels * sizeof(__int16);

//	FDecoder->SetPosition(0); /// test

	return true;
}
//---------------------------------------------------------------------------
int __fastcall TTSSWaveContext::Read(__int16 * dest, int destgranules)
{
	unsigned long written;
	unsigned long status ;

	int pos = 0;
	int remain = destgranules;
	do
	{
		FDecoder->Render(dest + FChannels * pos, remain, &written, &status);
		remain -= written;
		pos += written;
	} while(remain >0 && status == 1);

	return pos;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class TWaveReaderThread : public TThread
{
	TWaveReader * FOwner;
public:
	__fastcall TWaveReaderThread(TWaveReader *owner) : TThread(true)
	{
		FOwner = owner;
		Suspended = false;
	}

	__fastcall ~TWaveReaderThread(void)
	{
		Suspended = false;
		Terminate();
		WaitFor();
	}

	void __fastcall SyncMethod(void)
	{
		if(FOwner->FOnReadProgress)
		{
			FOwner->FOnReadProgress(FOwner);
		}
	}

	void __fastcall Execute(void)
	{
		DWORD tickstart = GetTickCount();
		while(!Terminated)
		{
			if(!FOwner->ReadBlock())
			{
				Synchronize(SyncMethod);
				Terminate();
			}
			else if(GetTickCount() - tickstart >= 100)
			{
				Synchronize(SyncMethod);
				tickstart = GetTickCount();
			}
		}
	}
} ;
//---------------------------------------------------------------------------
static void FindPlugins(AnsiString path, AnsiString ext, TStringList *dest)
{
	TSearchRec sr;
	int done;
	done = FindFirst(path + "*." + ext,
		faReadOnly | faHidden | faSysFile | faArchive, sr);
	if(!done)
	{
		do
		{
			// ������ DLL �����[�h���Ă݂�
			AnsiString filename = path + sr.FindData.cFileName;
			HMODULE dll = LoadLibrary(filename.c_str());
			if(dll)
			{
				// �K�v�Ȋ֐��������Ă��邩
				if(GetProcAddress(dll, "GetModuleInstance"))
				{
					dest->Add(filename);
				}
				FreeLibrary(dll);
			}

			done = FindNext(sr);
		} while(!done);
		FindClose(sr);
	}
}
//---------------------------------------------------------------------------
__fastcall TWaveReader::TWaveReader()
{
	FTmpFileName = ExtractFileDir(ParamStr(0)) + "\\temp.dat";
	FPlugins = new TStringList();

	// �g�p�\�ȃv���O�C������������
	AnsiString exepath = ExtractFilePath(ParamStr(0));
	FindPlugins(exepath + "plugin\\", "dll", FPlugins);
	FindPlugins(exepath+  "plugin\\", "tpm", FPlugins);
	FindPlugins(exepath + "..\\plugin\\", "dll", FPlugins);
	FindPlugins(exepath + "..\\plugin\\", "tpm", FPlugins);


	// �g�p�\�Ȋg���q�Ƃ��̃t�@�C���^�C�v���擾����

	TStringList *exts = new TStringList;
	TStringList *descs = new TStringList;
	typedef HRESULT _stdcall (*GetModuleInstanceProc)(ITSSModule **out,
			ITSSStorageProvider *provider, IStream * config, HWND mainwin);

	try
	{
		exts->Add(".wav");
		descs->Add("Wave �t�@�C��");

		for(int i = 0; i < FPlugins->Count; i++)
		{
			HMODULE lib = LoadLibrary(FPlugins->Strings[i].c_str());
			if(lib)
			{
				ITSSModule * mod;
				GetModuleInstanceProc proc = (GetModuleInstanceProc)
					GetProcAddress(lib, "GetModuleInstance");
				if(proc)
				{
					HRESULT hr =
						proc(&mod, &StorageProvider, NULL,
							Application->Handle);
					if(SUCCEEDED(hr))
					{
						int idx = 0;
						for(;;)
						{
							wchar_t shortname[33];
							wchar_t ext[256];
							hr = mod->GetSupportExts(idx, shortname,
														ext, 256);
							if(hr != S_OK) break;
							if(wcslen(ext) >= 1)
							{
								exts->Add(ext);
								descs->Add(shortname);
							}
							idx++;
						}
						mod->Release();
					}
				}
				else
				{
					MessageDlg(AnsiString("���̃v���O�C���͈����܂��� : ") +
						FPlugins->Strings[i].c_str(), mtWarning, TMsgDlgButtons() << mbOK , 0);
				}
				FreeLibrary(lib);
			}
		}
	}
	catch(Exception &e)
	{
		delete exts;
		delete descs;
		throw Exception(e);
	}

	AnsiString ret;
	ret = "���ׂĂ̌`�� (";
	int i;
	for(i=0; i<exts->Count; i++)
	{
		if(i) ret+=";";
		ret+= "*" + exts->Strings[i];
	}
	ret += ")|";
	for(i=0; i<exts->Count; i++)
	{
		if(i) ret+=";";
		ret+= "*" + exts->Strings[i];
	}

	for(i=0; i<exts->Count; i++)
	{
		ret+="|";
		ret+= descs->Strings[i];
		ret+= " (*"+ exts->Strings[i] +")|";
		ret+= "*" + exts->Strings[i];
	}

	ret+="|���ׂẴt�@�C�� (*.*)|*.*";

	FFilterString = ret;
}
//---------------------------------------------------------------------------
__fastcall TWaveReader::~TWaveReader()
{
	Clear();
	DeleteFile(FTmpFileName);
	delete FPlugins;
}
//---------------------------------------------------------------------------
void __fastcall TWaveReader::Clear()
{
	if(FReaderThread) delete FReaderThread, FReaderThread = NULL;
	if(FPeaks) SysFreeMem(FPeaks), FPeaks = NULL;
	if(FData) UnmapViewOfFile(FData), FData = NULL;
	if(FMapping) CloseHandle(FMapping), FMapping = NULL;
	if(FMappingFile) CloseHandle(FMappingFile), FMappingFile = NULL;
	if(FTmpStream) delete FTmpStream, FTmpStream = NULL;
	if(FInputContext) delete FInputContext, FInputContext = NULL;
	FReadDone = false;
	FNumSamples = 0;
	FPrevRange = 0;
}
//---------------------------------------------------------------------------
bool __fastcall TWaveReader::ReadBlock()
{
	if(FReadDone) return false;
	__int16 *buf = new __int16[32768 * Format.Format.nChannels];
	int written = FInputContext->Read(buf, 32768);
	FNumSamples += written;
	FTmpStream->Write(buf, written * Format.Format.nChannels * sizeof(__int16));
	delete [] buf;
	if(written==0)
	{
		// �ǂݍ��ݏI���
		Map();
		FReadDone = true;
	}

	return written!=0;
}
//---------------------------------------------------------------------------
void __fastcall TWaveReader::Map()
{
	if(!FData)
	{
		if(FTmpStream) delete FTmpStream , FTmpStream = NULL;

		FMappingFile =
			CreateFile(FTmpFileName.c_str(),
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);

		if(!FMappingFile)
		{
			return;
		}

		FNumSamples = GetFileSize(FMappingFile, NULL) / (Format.Format.nChannels*2);

		FMapping =
			CreateFileMapping(FMappingFile,
					NULL,
					PAGE_READONLY,
					0,
					0,
					("WaveReadMapping"+AnsiString(GetTickCount())).c_str());

		if(!FMapping)
		{
			CloseHandle(FMappingFile), FMappingFile = NULL;
			return;
		}

		FData = (__int16 *)MapViewOfFile(FMapping, FILE_MAP_READ, 0, 0, 0);

		// �s�[�N��T��
		#define PEAK_RANGE 128

		if(FPeaks) SysFreeMem(FPeaks); // �s�[�N�L���b�V��

		int alloc;
		alloc = (FNumSamples / PEAK_RANGE + 2) * sizeof(__int16)*2 * Format.Format.nChannels;
		FPeaks = (__int16*)
			SysGetMem(alloc);

		alloc /=2;

		int pos;
		int idx = 0;
		for(pos=0; pos<FNumSamples + PEAK_RANGE; pos+=PEAK_RANGE)
		{
			int channel;
			for(channel = 0; channel< Format.Format.nChannels; channel++)
			{
				int high, low;
				int i;
				i = pos;
				int lim = i + PEAK_RANGE;
				if(i >= FNumSamples) i = FNumSamples-1;
				if(lim >= FNumSamples) lim = FNumSamples;
				high = low = FData[i*Format.Format.nChannels + channel];
				i++;
				while(i<lim)
				{
					int n = FData[i*Format.Format.nChannels + channel];
					if(low > n) low = n;
					if(high < n) high = n;
					i++;
				}
				FPeaks[idx++] = low;
				FPeaks[idx++] = high;
				if(idx > alloc)
				{
					return;
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveReader::LoadWave(AnsiString filename)
{
	Clear();

	FTmpStream = new TFileStream(FTmpFileName, fmCreate|fmShareDenyWrite);

	// �g�p�\�Ȃ̂�T��

	// �W�� RIFF wave reader �Ŏ���
	bool res;
	FInputContext = new TRIFFWaveContext();
	try
	{
		res = FInputContext->Start(filename);
	}
	catch(...)
	{
		res = false;
	}

	if(!res)
	{
		delete FInputContext;
		FInputContext = NULL;

		// �v���O�C������ǂݍ��݉\�ȃ��m��T��
		for(int i = 0; i < FPlugins->Count; i++)
		{
			FInputContext = new TTSSWaveContext(FPlugins->Strings[i]);

			try
			{
				res = FInputContext->Start(filename);
			}
			catch(...)
			{
				res = false;
			}
			if(!res) delete FInputContext, FInputContext = NULL;
		}
	}

	if(!FInputContext)
	{
		delete FTmpStream;
		FTmpStream = NULL;
		throw Exception(filename + " : ���̌`���̃t�@�C���͓ǂݍ��߂Ȃ����A�t�@�C�����ُ�ł�");
	}

	// Format �Ɋi�[
	// 16bit �ւ̕ϊ���inputcontext���s�����߁AFormat �ɂ� 16bit ���w�肷��
	DWORD speakerconfig = FInputContext->SpeakerConfig;
	int channels = FInputContext->Channels;
	int frequency = FInputContext->Frequency;

	Format.Format.nChannels = channels;
	Format.Format.nSamplesPerSec = frequency;
	Format.Format.wBitsPerSample = 16;
	Format.SubFormat = __KSDATAFORMAT_SUBTYPE_PCM;
	Format.Samples.wValidBitsPerSample = 16;

	if(speakerconfig == 0 && channels <= 2)
	{
		// ���ʂ� WAVE_FORMAT_PCM
		Format.Format.wFormatTag = WAVE_FORMAT_PCM;
		Format.Format.cbSize = 0;
	}
	else if(speakerconfig == 0)
	{
		// channels �� 4 ���邢�� 6 �̏ꍇ
		if(channels == 4)
		{
			// ���O�A�E�O�A����A�E��ł���Ɖ��肷��
			Format.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
			Format.dwChannelMask =
				SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_BACK_LEFT |
				SPEAKER_BACK_RIGHT;
			Format.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
		}
		else if(channels == 6)
		{
			// 5.1 �`�����l���p�f�[�^�ł���Ɖ��肷��
			Format.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
			Format.dwChannelMask = KSAUDIO_SPEAKER_5POINT1;
			Format.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
		}
		else
		{
			throw Exception(filename + " : �Ή��ł��Ȃ��`�����l�����ł�");
		}
	}
	else
	{
		// speakerconfig ������
		Format.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
		Format.dwChannelMask = speakerconfig;
		Format.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
	}

	Format.Format.nBlockAlign =
		Format.Format.wBitsPerSample * Format.Format.nChannels / 8;
	Format.Format.nAvgBytesPerSec =
		Format.Format.nSamplesPerSec * Format.Format.nBlockAlign;

	FReaderThread = new TWaveReaderThread(this);

	// ���̊֐��͓ǂݍ��݂̏I����҂����ɖ߂�܂�
}
//---------------------------------------------------------------------------
void __fastcall TWaveReader::GetPeak(int &high, int &low, int pos, int channel, int range)
{
	if(!FReadDone) return;

	if(range < PEAK_RANGE)
	{
		int i;
		i = pos;
		int lim = i + range;
		if(lim >= FNumSamples) lim = FNumSamples;
		high = low = FData[i * Format.Format.nChannels + channel];
		i++;
		while(i<lim)
		{
			int n = FData[i * Format.Format.nChannels + channel];
			if(low > n) low = n;
			if(high < n) high = n;
			i++;
		}
	}
	else
	{

		int i;
		i = pos / PEAK_RANGE;
		int lim = i + range / PEAK_RANGE;
		if(i >= FNumSamples / PEAK_RANGE) i = FNumSamples / PEAK_RANGE -1;
		if(lim >= FNumSamples / PEAK_RANGE) lim = FNumSamples / PEAK_RANGE -1;
		low = FPeaks[i*Format.Format.nChannels*2 + channel*2];
		high = FPeaks[i*Format.Format.nChannels*2 + channel*2 + 1];
		i++;
		while(i<lim)
		{
			int nlow = FPeaks[i*Format.Format.nChannels*2 + channel*2];
			int nhigh = FPeaks[i*Format.Format.nChannels*2 + channel*2 + 1];
			if(low > nlow) low = nlow;
			if(high < nhigh) high = nhigh;
			i++;
		}
	}
}
//---------------------------------------------------------------------------
int __fastcall TWaveReader::GetData(__int16 *buf, int ofs, int num)
{
	if(!FReadDone) return 0;

	if(ofs >= FNumSamples) return 0;
	if(ofs + num >= FNumSamples) num = FNumSamples - ofs;
	if(num <=0 ) return 0;

	memcpy(buf, FData + ofs * Format.Format.nChannels, num * Format.Format.nChannels * sizeof(__int16));

	return num;
}
//---------------------------------------------------------------------------
int __fastcall TWaveReader::SamplePosToTime(DWORD samplepos)
{
	if(!FReadDone) return -1;

	return (int)((__int64)samplepos*1000L / (__int64)Format.Format.nSamplesPerSec);
}
//---------------------------------------------------------------------------
AnsiString __fastcall TWaveReader::GetChannelLabel(int ch)
{
	static AnsiString chlabels[] =
	{"FL", "FR", "FC", "LF", "BL", "BR", "FLC", "FRC", "BC", "SL", "SR", "TC",
		"TFL", "TFC", "TFR", "TBL", "TBC", "TBR"};

	if(Format.Format.wFormatTag == WAVE_FORMAT_PCM)
	{
		if(Format.Format.nChannels == 1)
		{
			if(ch == 0) return "Mono";
		}
		else if(Format.Format.nChannels == 2)
		{
			if(ch == 0) return "L";
			else if(ch == 1) return "R";
		}
	}
	else if(Format.Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		DWORD mask = Format.dwChannelMask;
		DWORD m = 1;
		int index = 0;
		int nmasks = 0;
		int count = ch;
		if(!(mask & 0x80000000))
		{
			while(m)
			{
				if(mask & m)
				{
					if(count == 0)
					{
						if(index >= sizeof(chlabels) / sizeof(chlabels[0])) break;
						return chlabels[index];
					}
					count --;
					nmasks ++;
				}
				m <<= 1;
				index ++;
			}
		}
		return AnsiString(ch - count);
	}

	return "-";
}
//---------------------------------------------------------------------------
#if 0
AnsiString __fastcall GetAvailExtensions(void)
{
	// �ǂݍ��݉\�Ȍ`�����t�B���^������̌`���œ���

	TStringList *exts = new TStringList;
	TStringList *descs = new TStringList;

	exts->Add(".wav");
	descs->Add("Wave �t�@�C��");

	AnsiString exepath = ExtractFilePath(ParamStr(0));

	TSearchRec sr;
	int done;

	done = FindFirst(exepath + "plugin/*.dll",
		faReadOnly | faHidden | faSysFile | faArchive, sr);

	try
	{
		if(!done)
		{
			while(!done)
			{
				HMODULE lib =
					LoadLibrary((exepath + "plugin/" + sr.FindData.cFileName).c_str());
				if(lib)
				{
					ITSSModule * mod;
					GetModuleInstanceProc proc = (GetModuleInstanceProc)
						GetProcAddress(lib, "GetModuleInstance");
					if(proc)
					{
						HRESULT hr =
							proc(&mod, &StorageProvider, NULL,
								Application->Handle);
						if(SUCCEEDED(hr))
						{
							int idx = 0;
							for(;;)
							{
								wchar_t shortname[33];
								wchar_t ext[256];
								hr = mod->GetSupportExts(idx, shortname,
															ext, 256);
								if(hr != S_OK) break;
								if(wcslen(ext) >= 1)
								{
									exts->Add(ext);
									descs->Add(shortname);
								}
								idx++;
							}
							mod->Release();
						}
					}
					else
					{
						MessageDlg(AnsiString("���̃v���O�C���͈����܂��� : ") +
							sr.FindData.cFileName, mtWarning, TMsgDlgButtons() << mbOK , 0);
					}
					FreeLibrary(lib);
				}
				done = FindNext(sr);
			}
			FindClose(sr);
		}
	}
	catch(Exception &e)
	{
		delete exts;
		delete descs;
		throw Exception(e);
	}

	AnsiString ret;
	ret = "���ׂĂ̌`�� (";
	int i;
	for(i=0; i<exts->Count; i++)
	{
		if(i) ret+=";";
		ret+= "*" + exts->Strings[i];
	}
	ret += ")|";
	for(i=0; i<exts->Count; i++)
	{
		if(i) ret+=";";
		ret+= "*" + exts->Strings[i];
	}

	for(i=0; i<exts->Count; i++)
	{
		ret+="|";
		ret+= descs->Strings[i];
		ret+= " (*"+ exts->Strings[i] +")|";
		ret+= "*" + exts->Strings[i];
	}

	ret+="|���ׂẴt�@�C�� (*.*)|*.*";

	return ret;
}
#endif
//---------------------------------------------------------------------------
#pragma package(smart_init)
