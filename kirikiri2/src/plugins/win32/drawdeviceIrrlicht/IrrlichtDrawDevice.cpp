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
IrrlichtDrawDevice::IrrlichtDrawDevice() : IrrlichtBase()
{
}

/**
 * �f�X�g���N�^
 */
IrrlichtDrawDevice::~IrrlichtDrawDevice()
{
}

/**
 * �e�N�X�`���̊��蓖��
 * @param manager ���C���}�l�[�W��
 */
void 
IrrlichtDrawDevice::allocInfo(iTVPLayerManager * manager)
{
	if (driver) {
	
		// �e�N�X�`�����蓖��XXX �T�C�Y���肪�K�v�c
		ITexture *texture = driver->addTexture(core::dimension2d<s32>(1024, 1024), "layer", ECF_A8R8G8B8);
		if (texture == NULL) {
			TVPThrowExceptionMessage(L"�e�N�X�`���̊��蓖�ĂɎ��s���܂���");
		}

		// �ݒ�
		tjs_int w, h;
		manager->GetPrimaryLayerSize(w, h);
		manager->SetDrawDeviceData((void*)new LayerManagerInfo(texture, w, h));
		
		// �X�V�v��
		manager->RequestInvalidation(tTVPRect(0,0,w,h));
	}
}

/**
 *  �e�N�X�`���̉��
 * @param manager ���C���}�l�[�W��
 */
void
IrrlichtDrawDevice::freeInfo(iTVPLayerManager * manager)
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
 * �E�C���h�E�̉���
 */
void
IrrlichtDrawDevice::detach()
{
	for (std::vector<iTVPLayerManager *>::iterator i = Managers.begin(); i != Managers.end(); i++) {
		freeInfo(*i);
	}
	IrrlichtBase::detach();
}

/**
 * Irrlicht �ւ̃C�x���g���M
 */
bool
IrrlichtDrawDevice::postEvent(SEvent &ev)
{
	if (device) {
		if (device->getGUIEnvironment()->postEventFromUser(ev) ||
			device->getSceneManager()->postEventFromUser(ev)) {
			return true;
		}
	}
	return false;
}

/**
 * ���C���}�l�[�W���̓o�^
 * @param manager ���C���}�l�[�W��
 */
void TJS_INTF_METHOD
IrrlichtDrawDevice::AddLayerManager(iTVPLayerManager * manager)
{
	allocInfo(manager);
	tTVPDrawDevice::AddLayerManager(manager);
}

/**
 * ���C���}�l�[�W���̍폜
 * @param manager ���C���}�l�[�W��
 */
void TJS_INTF_METHOD
IrrlichtDrawDevice::RemoveLayerManager(iTVPLayerManager * manager)
{
	freeInfo(manager);
	tTVPDrawDevice::RemoveLayerManager(manager);
}


/***
 * �E�C���h�E�̎w��
 * @param wnd �E�C���h�E�n���h��
 */
void TJS_INTF_METHOD
IrrlichtDrawDevice::SetTargetWindow(HWND wnd, bool is_main)
{
	detach();
	if (wnd != NULL) {
		attach(wnd);
		// �}�l�[�W���ɑ΂���e�N�X�`���̊��蓖��
		for (std::vector<iTVPLayerManager *>::iterator i = Managers.begin(); i != Managers.end(); i++) {
			allocInfo(*i);
		}
		// �쓮�J�n
		start();
	}
}

void
IrrlichtDrawDevice::Show()
{
}

// -------------------------------------------------------------------------------------
// ���̓C�x���g�����p
// -------------------------------------------------------------------------------------

void TJS_INTF_METHOD
IrrlichtDrawDevice::OnMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags)
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
IrrlichtDrawDevice::OnMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags)
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
IrrlichtDrawDevice::OnMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags)
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
IrrlichtDrawDevice::OnKeyDown(tjs_uint key, tjs_uint32 shift)
{
	tTVPDrawDevice::OnKeyDown(key, shift);
}

void TJS_INTF_METHOD
IrrlichtDrawDevice::OnKeyUp(tjs_uint key, tjs_uint32 shift)
{
	tTVPDrawDevice::OnKeyUp(key, shift);
}

void TJS_INTF_METHOD
IrrlichtDrawDevice::OnKeyPress(tjs_char key)
{
	tTVPDrawDevice::OnKeyPress(key);
}

void TJS_INTF_METHOD
IrrlichtDrawDevice::OnMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y)
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
IrrlichtDrawDevice::StartBitmapCompletion(iTVPLayerManager * manager)
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
IrrlichtDrawDevice::NotifyBitmapCompleted(iTVPLayerManager * manager,
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
IrrlichtDrawDevice::EndBitmapCompletion(iTVPLayerManager * manager)
{
	LayerManagerInfo *info = (LayerManagerInfo*)manager->GetDrawDeviceData();
	if (info) {
		info->unlock();
	}
}

// -------------------------------------------------------------------------------------
// ��ʍX�V����
// -------------------------------------------------------------------------------------

void
IrrlichtDrawDevice::update(tjs_uint64 tick)
{
	dimension2d<s32> screenSize = driver->getScreenSize();

	// �ʃ��C���}�l�[�W���̕`��
	for (std::vector<iTVPLayerManager *>::iterator i = Managers.begin(); i != Managers.end(); i++) {
		LayerManagerInfo *info = (LayerManagerInfo*)(*i)->GetDrawDeviceData();
		if (info && info->texture) {
			// XXX ���C����3D��Ԕz�u�H
			driver->draw2DImage(info->texture, core::position2d<s32>(0,0),
								//core::rect<s32>(0,0,screenSize.Width,screenSize.Height), 0, 
								core::rect<s32>(0,0,info->layerWidth,info->layerHeight), 0, 
								video::SColor(255,255,255,255), true);
		}
	}
}
