// sqthread�p�t�@�C���ǂݍ��ݏ����T���v��
#include <stdio.h>
#include <squirrel.h>
#include <sqstdio.h>

/**
 * �t�@�C���ǂݍ��ݗp�f�[�^�\��
 */
class SQFileInfo {

public:
	/// �R���X�g���N�^
	SQFileInfo(const SQChar *filename, bool binary) : file(NULL), buffer(NULL), size(0), binary(binary) {
		file = sqstd_fopen(filename, binary ? _SC("rb") : _SC("r"));
		if (file) {
			size = sqstd_fseek(file, 0, SQ_SEEK_END);
			if (size > 0) {
				buffer = sq_malloc(size);
				sqstd_fread(buffer, size, 1, file);
			}
		}
	}

	/// �f�X�g���N�^
	~SQFileInfo() {
		if (buffer) {
			sq_free(buffer,size);
		}
		if (file) {
			sqstd_fclose(file);
		}
	}

	/// @return �ǂݍ��݊��������� true
	bool check() {
		if (buffer) {
			return true;
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
	SQFILE file;  ///< ���̓X�g���[��
	void *buffer; ///< ���̓f�[�^�̃o�b�t�@
	SQInteger size;   ///< �ǂݍ��݃T�C�Y
	bool binary;
};

/**
 * �t�@�C����񓯊��ɊJ��
 * @param filename �X�N���v�g�t�@�C����
 * @param binary �o�C�i���w��ŊJ��
 * @return �t�@�C���n���h��
 */
void *sqobjOpenFile(const SQChar *filename, bool binary)
{
	return (void*) new SQFileInfo(filename, binary);
}

/**
 * �t�@�C�����J���ꂽ���ǂ����̃`�F�b�N
 * @param handler �t�@�C���n���h��
 * @param dataPtr �f�[�^�i�[��A�h���X(�o��) (�G���[����NULL)
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
