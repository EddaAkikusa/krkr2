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
IrrlichtDrawDevice::IrrlichtDrawDevice(int width, int height) : IrrlichtBase(), width(width), height(height), zoomMode(true), screenWidth(-1), screenHeight(-1)
{
}

/**
 * �f�X�g���N�^
 */
IrrlichtDrawDevice::~IrrlichtDrawDevice()
{
}

// -----------------------------------------------------------------------
// Continuous
// -----------------------------------------------------------------------

/**
 * Continuous �R�[���o�b�N
 * �g���g�����ɂȂƂ��ɏ�ɌĂ΂��
 */
void TJS_INTF_METHOD
IrrlichtDrawDevice::OnContinuousCallback(tjs_uint64 tick)
{
	Window->RequestUpdate();
}

/**
 * �Y�[�����[�h�p�̕␳
 */
void
IrrlichtDrawDevice::updateZoomMode()
{
	if (device) {
		IVideoDriver *driver = device->getVideoDriver();
		if (driver) {
			tjs_int w, h;
			if (zoomMode) {
				w = width;
				h = height;
			} else {
				w = destRect.getWidth();
				h = destRect.getHeight();
			}
			if (screenWidth != w ||	screenHeight != h) {
				screenWidth = w;
				screenHeight = h;
				driver->OnResize(dimension2d<s32>(w, h));
			}
		}
	}
}

/**
 * �e�N�X�`���̊��蓖��
 * @param manager ���C���}�l�[�W��
 */
void 
IrrlichtDrawDevice::allocInfo(iTVPLayerManager * manager)
{
	freeInfo(manager);
	if (device) {
		IVideoDriver *driver = device->getVideoDriver();
		if (driver) {
			// �ݒ�
			tjs_int w, h;
			if (manager->GetPrimaryLayerSize(w, h) && w > 0 && h > 0) {
				// �e�N�X�`�����蓖��XXX �T�C�Y���肪�K�v�c
				ITexture *texture = driver->addTexture(core::dimension2d<s32>(1024, 1024), "layer", ECF_A8R8G8B8);
				if (texture == NULL) {
					TVPThrowExceptionMessage(L"�e�N�X�`���̊��蓖�ĂɎ��s���܂���");
				}
				manager->SetDrawDeviceData((void*)new LayerManagerInfo(texture, w, h));
				manager->RequestInvalidation(tTVPRect(0,0,w,h));
			}
		}
	}
}

/**
 *  �e�N�X�`���̉��
 * @param manager ���C���}�l�[�W��
 */
void
IrrlichtDrawDevice::freeInfo(iTVPLayerManager * manager)
{
	if (device) {
		IVideoDriver *driver = device->getVideoDriver();
		if (driver) {
			LayerManagerInfo *info = (LayerManagerInfo*)manager->GetDrawDeviceData();
			if (info != NULL) {
				driver->removeTexture(info->texture);
				manager->SetDrawDeviceData(NULL);
				delete info;
			}
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
	screenWidth = -1;
	screenHeight = -1;
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

void
IrrlichtDrawDevice::TransformTo(tjs_int &x, tjs_int &y)
{
	// x , y �� DestRect �� 0, 0 �����_�Ƃ������W�Ƃ��ēn����Ă��Ă���
	x = screenWidth  ? (x * width  / screenWidth) : 0;
	y = screenHeight ? (y * height / screenHeight) : 0;
}

void
IrrlichtDrawDevice::TransformFrom(tjs_int &x, tjs_int &y)
{
	// x , y �� DestRect �� 0, 0 �����_�Ƃ������W�Ƃ��ēn����Ă��Ă���
	x = width ? (x * screenWidth / width) : 0;
	y = height ? (y * screenHeight / height) : 0;
}

/**
 * �ŗL�X�V����
 */
void
IrrlichtDrawDevice::update(irr::video::IVideoDriver *driver)
{
	// �ʃ��C���}�l�[�W���̕`��
	for (std::vector<iTVPLayerManager *>::iterator i = Managers.begin(); i != Managers.end(); i++) {
		LayerManagerInfo *info = (LayerManagerInfo*)(*i)->GetDrawDeviceData();
		if (info && info->texture) {
			tjs_int w = width  ? (info->layerWidth  * screenWidth / width) : 0;
			tjs_int h = height ? (info->layerHeight * screenHeight / height) : 0;
			driver->draw2DImage(info->texture,
								core::rect<s32>(0,0,w,h),
								core::rect<s32>(0,0,info->layerWidth,info->layerHeight),
								NULL, NULL, true);
		}
	}
}


/**
 * ���C���}�l�[�W���̓o�^
 * @param manager ���C���}�l�[�W��
 */
void TJS_INTF_METHOD
IrrlichtDrawDevice::AddLayerManager(iTVPLayerManager * manager)
{
	tTVPDrawDevice::AddLayerManager(manager);
	allocInfo(manager);
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
		Window->NotifySrcResize(); // ������ĂԂ��Ƃ� GetSrcSize(), SetDestRectangle() �̌ĂѕԂ�������
		// �}�l�[�W���ɑ΂���e�N�X�`���̊��蓖��
		for (std::vector<iTVPLayerManager *>::iterator i = Managers.begin(); i != Managers.end(); i++) {
			allocInfo(*i);
		}
	}
}

void TJS_INTF_METHOD
IrrlichtDrawDevice::SetDestRectangle(const tTVPRect &dest)
{
	destRect.setLeft(dest.left);
	destRect.setTop(dest.top);
	destRect.setWidth(dest.get_width());
	destRect.setHeight(dest.get_height());
	updateZoomMode();
}

void TJS_INTF_METHOD
IrrlichtDrawDevice::GetSrcSize(tjs_int &w, tjs_int &h)
{
	w = width;
	h = height;
}

void TJS_INTF_METHOD
IrrlichtDrawDevice::NotifyLayerResize(iTVPLayerManager * manager)
{
	allocInfo(manager);
}

// -------------------------------------------------------------------------------------
// ���̓C�x���g�����p
// -------------------------------------------------------------------------------------

void TJS_INTF_METHOD
IrrlichtDrawDevice::OnMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags)
{
	if (device) {
		tjs_int dx = x;
		tjs_int dy = y;
		TransformTo(dx, dy);
		SEvent ev;
		ev.EventType = EET_MOUSE_INPUT_EVENT;
		ev.MouseInput.X = dx;
		ev.MouseInput.Y = dy;
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
	tTVPDrawDevice::OnMouseDown(x, y, mb, flags);
}

void TJS_INTF_METHOD
IrrlichtDrawDevice::OnMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags)
{
	if (device) {
		tjs_int dx = x;
		tjs_int dy = y;
		TransformTo(dx, dy);
		SEvent ev;
		ev.EventType = EET_MOUSE_INPUT_EVENT;
		ev.MouseInput.X = dx;
		ev.MouseInput.Y = dy;
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
	tTVPDrawDevice::OnMouseUp(x, y, mb, flags);
}

void TJS_INTF_METHOD
IrrlichtDrawDevice::OnMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags)
{
	if (device) {
		tjs_int dx = x;
		tjs_int dy = y;
		TransformTo(dx, dy);
		SEvent ev;
		ev.EventType = EET_MOUSE_INPUT_EVENT;
		ev.MouseInput.X = dx;
		ev.MouseInput.Y = dy;
		ev.MouseInput.Wheel = 0;
		ev.MouseInput.Event = EMIE_MOUSE_MOVED;
		if (postEvent(ev)) {
			return;
		}
	}
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
// �ĕ`�揈���p
// -------------------------------------------------------------------------------------

void
IrrlichtDrawDevice::Show()
{
	show(&destRect);
}

// -------------------------------------------------------------------------------------
// LayerManager����̉摜�����킽��
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

