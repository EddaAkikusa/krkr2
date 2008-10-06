#ifndef IRRLICHTDRIVER_H
#define IRRLICHTDRIVER_H

#include <windows.h>
#include "tp_stub.h"
#include <irrlicht.h>

/**
 * �h���C�o����p�N���X
 */
class IrrlichtDriver {

protected:
	irr::video::IVideoDriver *driver;

public:
	IrrlichtDriver();
	IrrlichtDriver(irr::video::IVideoDriver *driver);
	IrrlichtDriver(const IrrlichtDriver &orig);
	~IrrlichtDriver();

	// ------------------------------------------------------------
	// �h���C�o����
	// ------------------------------------------------------------
public:
	/**
	 * �摜�̃L���v�`��
	 * @param dest �i�[�惌�C��
	 */
	void captureScreenShot(iTJSDispatch2 *layer);

	// XXX 2D��{�}�`���������邩�H
	// �V�[���Ƃ͕ʂɋL�^���ĕ`�悷��悤�Ȍ`�ɂȂ�Ǝv����
};

#endif
