#include "LayerExSWF.hpp"
#include "SWFMovie.hpp"

#include "gameswf/gameswf.h"
#include "base/utility.h"
#include "base/container.h"
#include "base/tu_file.h"
#include "base/tu_types.h"

static void
message_log(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	char msg[1024];
	_vsnprintf(msg, 1024, format, args);
	TVPAddLog(ttstr(msg));
	va_end(args);
}

/**
 * ���O�����Ăяo��
 */
static void
log_callback(bool error, const char* message)
{
	if (error) {
		TVPAddImportantLog(ttstr(message));
	} else {
		message_log(message);
	}
}

static tu_file*	file_opener(const char* url)
{
	return new tu_file(url, "rb");
}

static void	fs_callback(gameswf::movie_interface* movie, const char* command, const char* args)
{
	// FS �R�}���h�p
	// �ŏI�I�� TJS �Ăяo���ɒu������ׂ�
	message_log("fs_callback: '");
	message_log(command);
	message_log("' '");
	message_log(args);
	message_log("'\n");
}

static void	test_progress_callback(unsigned int loaded_tags, unsigned int total_tags)
{
	// �v���O���X�o�[�\���p
	// �ŏI�I�� TJS �̌ŗL���\�b�h�Ăяo��������
}

////////////////////////////////////////////////////////////////////////////////
/// ncBind �p�}�N��

#include "ncbind/ncbind.hpp"

NCB_REGISTER_CLASS(SWFMovie) {
	NCB_CONSTRUCTOR(());
	NCB_METHOD(load);
}

NCB_GET_INSTANCE_HOOK(layerExSWF)
{
	// �C���X�^���X�Q�b�^
	NCB_INSTANCE_GETTER(objthis) { // objthis �� iTJSDispatch2* �^�̈����Ƃ���
		
		ClassT* obj = GetNativeInstance(objthis);	// �l�C�e�B�u�C���X�^���X�|�C���^�擾
		if (!obj) {
			obj = new ClassT(objthis);				// �Ȃ��ꍇ�͐�������
			SetNativeInstance(objthis, obj);		// objthis �� obj ���l�C�e�B�u�C���X�^���X�Ƃ��ēo�^����
		}
		if (obj) obj->reset();						// ���\�b�h���ĂԑO�ɕK���Ă΂��
		return (_obj = obj);						//< �f�X�g���N�^�Ŏg�p�������ꍇ�̓v���C�x�[�g�ϐ��ɕۑ�
	}

	// �f�X�g���N�^�i���ۂ̃��\�b�h���Ă΂ꂽ��ɌĂ΂��j
	~NCB_GET_INSTANCE_HOOK_CLASS () {
		if (_obj) _obj->redraw();					// ���\�b�h���Ă񂾌�ɕK���Ă΂��
	}

private:
	ClassT *_obj;
}; // ���̂� class ��`�Ȃ̂� ; ��Y��Ȃ��ł�

// �t�b�N���A�^�b�`
NCB_ATTACH_CLASS_WITH_HOOK(layerExSWF, Layer) {
	NCB_METHOD(drawSWF);
}

void PreRegistCallbackTest()
{
}

void PostRegistCallbackTest()
{
	gameswf::register_file_opener_callback(file_opener);
	gameswf::register_fscommand_callback(fs_callback);
	gameswf::register_log_callback(log_callback);
}

void PreUnregistCallbackTest()
{
	gameswf::clear();
}

void PostUnregistCallbackTest()
{
}

NCB_PRE_REGIST_CALLBACK(   PreRegistCallbackTest);
NCB_POST_REGIST_CALLBACK(  PostRegistCallbackTest);
NCB_PRE_UNREGIST_CALLBACK( PreUnregistCallbackTest);
NCB_POST_UNREGIST_CALLBACK(PostUnregistCallbackTest);
