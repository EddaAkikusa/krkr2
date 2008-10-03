#include "ncbind/ncbind.hpp"
#include "IrrlichtDrawDevice.h"
#include "IrrlichtWindow.h"

/**
 * ���O�o�͗p
 */
void
message_log(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	char msg[1024];
	_vsnprintf_s(msg, 1024, _TRUNCATE, format, args);
	TVPAddLog(ttstr(msg));
	va_end(args);
}

/**
 * �G���[���O�o�͗p
 */
void
error_log(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	char msg[1024];
	_vsnprintf_s(msg, 1024, _TRUNCATE, format, args);
	TVPAddImportantLog(ttstr(msg));
	va_end(args);
}

// ----------------------------------- �N���X�̓o�^

using namespace irr::core;
using namespace irr::video;

NCB_REGISTER_SUBCLASS(vector3df) {
	NCB_CONSTRUCTOR(());
};

#define BASE_METHOD \
	NCB_METHOD(captureScreenShot)

NCB_REGISTER_SUBCLASS(IrrlichtDrawDevice) {
	NCB_CONSTRUCTOR((int));
	BASE_METHOD;
	NCB_PROPERTY_RO(interface, GetDevice);
};

NCB_REGISTER_SUBCLASS(IrrlichtWindow) {
	NCB_CONSTRUCTOR((int, iTJSDispatch2 *, int, int, int, int));
	BASE_METHOD;
	NCB_PROPERTY(left, getLeft, setLeft);
	NCB_PROPERTY(top, getTop, setTop);
	NCB_PROPERTY(width, getWidth, setWidth);
	NCB_PROPERTY(height, getHeight, setHeight);
	NCB_PROPERTY(visible, getVisible, setVisible);
	NCB_METHOD(setPos);
	NCB_METHOD(setSize);
};

/**
 * ���O��ԗp�_�~�[
 */
struct Irrlicht {
};

#define ENUM(n) Variant(#n, (int)n)

NCB_REGISTER_CLASS(Irrlicht) {
	//enums
	ENUM(EDT_SOFTWARE);
	ENUM(EDT_BURNINGSVIDEO);
	ENUM(EDT_DIRECT3D9);
	ENUM(EDT_OPENGL);
	//static
	//classes
	NCB_SUBCLASS(DrawDevice, IrrlichtDrawDevice);
	NCB_SUBCLASS(Window, IrrlichtWindow);
}

/**
 * �o�^�����O
 */
void PreRegistCallback()
{
	registerWindowClass();
}

/**
 * �J��������
 */
void PostUnregistCallback()
{
	unregisterWindowClass();
}

NCB_PRE_REGIST_CALLBACK(PreRegistCallback);
NCB_POST_UNREGIST_CALLBACK(PostUnregistCallback);
