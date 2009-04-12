#include <windows.h>
#include <squirrel.h>
#include "tp_stub.h"

/**
 * �t�@�C���ǂݍ��ݗp�f�[�^�\��
 */
class SQFileInfo {

public:
	/// �R���X�g���N�^
	SQFileInfo(const SQChar *filename) : is(NULL), buffer(NULL), size(0), readed(0) {
		is = TVPCreateIStream(filename, TJS_BS_READ);
		if (is) {
			STATSTG stat;
			is->Stat(&stat, STATFLAG_NONAME);
			size = (ULONG)stat.cbSize.QuadPart;
			buffer = new char[size];
		}
	}

	/// �f�X�g���N�^
	~SQFileInfo() {
		if (buffer) {
			delete [] buffer;
		}
		if (is) {
			is->Release();
		}
	}

	/// @return �ǂݍ��݊��������� true
	bool check() {
		if (buffer) {
			if (readed < size) {
				ULONG len;
				if (is->Read(buffer+readed,size-readed,&len) == S_OK) {
					readed += len;
				}
			}
			return readed >= size;
		} else {
			return true;
		}
	}

	/// @return �o�b�t�@
	const char *getBuffer() {
		return (const char*)buffer;
	}

	/// @return �T�C�Y
	int getSize() {
		return (int)size;
	}

private:
	IStream *is;  ///< ���̓X�g���[��
	char *buffer; ///< ���̓f�[�^�̃o�b�t�@
	ULONG size;   ///< �ǂݍ��݃T�C�Y
	ULONG readed; ///< �ǂݍ��ݍς݃T�C�Y
};

/**
 * �t�@�C����񓯊��ɊJ��
 * @param filename �X�N���v�g�t�@�C����
 * @return �t�@�C���n���h��
 */
void *sqobjOpenFile(const SQChar *filename)
{
	return (void*) new SQFileInfo(filename);
}

/**
 * �t�@�C�����J���ꂽ���ǂ����̃`�F�b�N
 * @param handler �t�@�C���n���h��
 * @param dataPtr �f�[�^�i�[��A�h���X(�o��)
 * @param dataSize �f�[�^�T�C�Y(�o��)
 * @return ���[�h�������Ă����� true
 */
bool sqobjCheckFile(void *handler, const char **dataAddr, int *dataSize)
{
	SQFileInfo *file = (SQFileInfo*)handler;
	if (file) {
		if (file->check()) {
			*dataAddr = file->getBuffer();
			*dataSize = file->getSize();
			return true;
		} else {
			return false;
		}
	} else {
		*dataAddr = NULL;
		*dataSize = 0;
		return true;
	}
}

/**
 * �t�@�C�������
 * @param handler �t�@�C���n���h��
 */
void sqobjCloseFile(void *handler)
{
	SQFileInfo *file = (SQFileInfo*)handler;
	if (file) {
		delete file;
	}
}
