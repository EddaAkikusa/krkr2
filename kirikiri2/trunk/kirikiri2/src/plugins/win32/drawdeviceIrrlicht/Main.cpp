#pragma comment(lib, "Irrlicht.lib")
#include "ncbind/ncbind.hpp"
#include "IrrlichtDrawDevice.h"

/**
 * Irrlicht �x�[�X DrawDevice �̃N���X
 */
class IrrlichtDrawDevice {
public:
	// �f�o�C�X���
	iTVPDrawDevice *device;
public:
	/**
	 * �R���X�g���N�^
	 */
	IrrlichtDrawDevice() {
		device = new tTVPIrrlichtDrawDevice();
	}

	~IrrlichtDrawDevice() {
		if (device) {
			device->Destruct();
			device = NULL;
		}
	}

	/**
	 * @return �f�o�C�X���
	 */
	tjs_int64 GetDevice() {
		return reinterpret_cast<tjs_int64>(device);
	}

	// ---------------------------------------------
	// �ȉ� Irrlicht �𐧌䂷�邽�߂̋@�\�������ǉ��\��
	// ---------------------------------------------
	
};


NCB_REGISTER_CLASS(IrrlichtDrawDevice) {
	NCB_CONSTRUCTOR(());
	NCB_PROPERTY_RO(interface, GetDevice);
}
