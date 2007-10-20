#include "IrrlichtDrawDevice.h"
#include "LayerManagerInfo.h"

extern void message_log(const char* format, ...);
extern void error_log(const char *format, ...);

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace io;
using namespace gui;

/**
 * �R���X�g���N�^
 */
tTVPIrrlichtDrawDevice::tTVPIrrlichtDrawDevice()
{
	device = NULL;
	driver = NULL;
}

/**
 * �f�X�g���N�^
 */
tTVPIrrlichtDrawDevice::~tTVPIrrlichtDrawDevice()
{
	detach();
}

/**
 * �e�N�X�`���̊��蓖��
 * @param manager ���C���}�l�[�W��
 */
void 
tTVPIrrlichtDrawDevice::allocInfo(iTVPLayerManager * manager)
{
	if (driver) {

		// �e�N�X�`�����蓖��XXX �T�C�Y���肪�K�v�c
		ITexture *texture = driver->addTexture(core::dimension2d<s32>(1024, 1024), "layer", ECF_A8R8G8B8);
		if (texture == NULL) {
			TVPThrowExceptionMessage(L"�e�N�X�`���̊��蓖�ĂɎ��s���܂���");
		}

		manager->SetDrawDeviceData((void*)new LayerManagerInfo(texture));

		// �X�V�v��
		tjs_int w, h;
		if (manager->GetPrimaryLayerSize(w, h)) {
			manager->RequestInvalidation(tTVPRect(0,0,w,h));
		}
	}
}

/**
 *  �e�N�X�`���̉��
 * @param manager ���C���}�l�[�W��
 */
void
tTVPIrrlichtDrawDevice::freeInfo(iTVPLayerManager * manager)
{
	if (driver) {
		LayerManagerInfo *info = (LayerManagerInfo*)manager->GetDrawDeviceData();
		if (info != NULL) {
			driver->removeTexture(info->texture);
			manager->SetDrawDeviceData(NULL);
			delete info;
		}
	}
}

/**
 * �E�C���h�E�̍Đݒ�
 * @param hwnd �n���h��
 */
void
tTVPIrrlichtDrawDevice::attach(HWND hwnd)
{
	// �f�o�C�X����

	SIrrlichtCreationParameters params;
	params.WindowId     = reinterpret_cast<void*>(hwnd);
	params.DriverType    = video::EDT_DIRECT3D9;
	params.Bits          = 32;
	params.Stencilbuffer = true;
	params.Vsync = true;
	params.EventReceiver = this;
	params.AntiAlias = true;

	if ((device = irr::createDeviceEx(params))) {
		TVPAddLog(L"DirectX9�ŏ�����");
	} else {
		TVPThrowExceptionMessage(L"Irrlicht �f�o�C�X�̏������Ɏ��s���܂���");
	}
	driver = device->getVideoDriver();
	
	dimension2d<s32> size = driver->getScreenSize();
	message_log("�f�o�C�X������̃X�N���[���T�C�Y:%d, %d", size.Width, size.Height);

	size = driver->getCurrentRenderTargetSize();
	message_log("�f�o�C�X�������RenderTarget��:%d, %d", size.Width, size.Height);

	// �}�l�[�W���ɑ΂���e�N�X�`���̊��蓖��
	for (std::vector<iTVPLayerManager *>::iterator i = Managers.begin(); i != Managers.end(); i++) {
		allocInfo(*i);
	}

	// �쓮�J�n
	start();
}

/**
 * �E�C���h�E�̉���
 */
void
tTVPIrrlichtDrawDevice::detach()
{
	if (device) {
		stop();
		for (std::vector<iTVPLayerManager *>::iterator i = Managers.begin(); i != Managers.end(); i++) {
			freeInfo(*i);
		}
		device->drop();
		device = NULL;
		driver = NULL;
	}
}

/**
 * ���C���}�l�[�W���̓o�^
 * @param manager ���C���}�l�[�W��
 */
void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::AddLayerManager(iTVPLayerManager * manager)
{
	allocInfo(manager);
	tTVPDrawDevice::AddLayerManager(manager);
}

/**
 * ���C���}�l�[�W���̍폜
 * @param manager ���C���}�l�[�W��
 */
void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::RemoveLayerManager(iTVPLayerManager * manager)
{
	freeInfo(manager);
	tTVPDrawDevice::RemoveLayerManager(manager);
}


/***
 * �E�C���h�E�̎w��
 * @param wnd �E�C���h�E�n���h��
 */
void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::SetTargetWindow(HWND wnd, bool is_main)
{
	detach();
	if (wnd != NULL) {
		attach(wnd);
	}
}

void
tTVPIrrlichtDrawDevice::Show()
{
}

bool
tTVPIrrlichtDrawDevice::postEvent(SEvent &ev)
{
	if (device) {
		if (device->getGUIEnvironment()->postEventFromUser(ev) ||
			device->getSceneManager()->postEventFromUser(ev)) {
			return true;
		}
	}
	return false;
}

// -------------------------------------------------------------------------------------
// ���̓C�x���g�����p
// -------------------------------------------------------------------------------------

void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::OnMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags)
{
	if (driver) {
		SEvent ev;
		ev.EventType = EET_MOUSE_INPUT_EVENT;
		ev.MouseInput.X = x;
		ev.MouseInput.Y = y;
		ev.MouseInput.Wheel = 0;
		switch ((mb & 0xff)) {
		case mbLeft:
			ev.MouseInput.Event = EMIE_LMOUSE_PRESSED_DOWN;
			break;
		case mbMiddle:
			ev.MouseInput.Event = EMIE_MMOUSE_PRESSED_DOWN;
			break;
		case mbRight:
			ev.MouseInput.Event = EMIE_RMOUSE_PRESSED_DOWN;
			break;
		}
		if (postEvent(ev)) {
			return;
		}
	}
	// XXX ��ʃT�C�Y�ɉ������␳���K�v
	tTVPDrawDevice::OnMouseDown(x, y, mb, flags);
}

void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::OnMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags)
{
	if (driver) {
		SEvent ev;
		ev.EventType = EET_MOUSE_INPUT_EVENT;
		ev.MouseInput.X = x;
		ev.MouseInput.Y = y;
		ev.MouseInput.Wheel = 0;
		switch ((mb & 0xff)) {
		case mbLeft:
			ev.MouseInput.Event = EMIE_LMOUSE_LEFT_UP;
			break;
		case mbMiddle:
			ev.MouseInput.Event = EMIE_MMOUSE_LEFT_UP;
			break;
		case mbRight:
			ev.MouseInput.Event = EMIE_RMOUSE_LEFT_UP;
			break;
		}
		if (postEvent(ev)) {
			return;
		}
	}
	// XXX ��ʃT�C�Y�ɉ������␳���K�v
	tTVPDrawDevice::OnMouseUp(x, y, mb, flags);
}

void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::OnMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags)
{
	if (driver) {
		SEvent ev;
		ev.EventType = EET_MOUSE_INPUT_EVENT;
		ev.MouseInput.X = x;
		ev.MouseInput.Y = y;
		ev.MouseInput.Wheel = 0;
		ev.MouseInput.Event = EMIE_MOUSE_MOVED;
		if (postEvent(ev)) {
			return;
		}
	}
	// XXX ��ʃT�C�Y�ɉ������␳���K�v
	tTVPDrawDevice::OnMouseMove(x, y, flags);
}

void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::OnKeyDown(tjs_uint key, tjs_uint32 shift)
{
	tTVPDrawDevice::OnKeyDown(key, shift);
}

void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::OnKeyUp(tjs_uint key, tjs_uint32 shift)
{
	tTVPDrawDevice::OnKeyUp(key, shift);
}

void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::OnKeyPress(tjs_char key)
{
	tTVPDrawDevice::OnKeyPress(key);
}

void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::OnMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y)
{
	tTVPDrawDevice::OnMouseWheel(shift, delta, x, y);
}

// -------------------------------------------------------------------------------------
// �`�揈���p
// -------------------------------------------------------------------------------------

/**
 * �r�b�g�}�b�v�R�s�[�����J�n
 */
void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::StartBitmapCompletion(iTVPLayerManager * manager)
{
	LayerManagerInfo *info = (LayerManagerInfo*)manager->GetDrawDeviceData();
	if (info) {
		info->lock();
	}
}

/**
 * �r�b�g�}�b�v�R�s�[����
 */
void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::NotifyBitmapCompleted(iTVPLayerManager * manager,
	tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
	const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity)
{
	LayerManagerInfo *info = (LayerManagerInfo*)manager->GetDrawDeviceData();
	if (info) {
		info->copy(x, y, bits, bitmapinfo, cliprect, type, opacity);
	}
}

/**
 * �r�b�g�}�b�v�R�s�[�����I��
 */
void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::EndBitmapCompletion(iTVPLayerManager * manager)
{
	LayerManagerInfo *info = (LayerManagerInfo*)manager->GetDrawDeviceData();
	if (info) {
		info->unlock();
	}
}
