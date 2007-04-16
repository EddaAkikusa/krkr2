#include <windows.h>

#include "IrrlichtDrawDevice.h"

/**
 * �R���X�g���N�^
 */
tTVPIrrlichtDrawDevice::tTVPIrrlichtDrawDevice()
{
	device = NULL;
	driver = NULL;
	smgr   = NULL;
	texture  = NULL;
	destBuffer = NULL;
}

/**
 * �f�X�g���N�^
 */
tTVPIrrlichtDrawDevice::~tTVPIrrlichtDrawDevice()
{
	detach();
}

IrrlichtDevice *
tTVPIrrlichtDrawDevice::create(HWND hwnd, video::E_DRIVER_TYPE type)
{
	SIrrlichtCreationParameters params;
	params.WindowId     = reinterpret_cast<s32>(hwnd);
	params.DriverType    = type;
	params.WindowSize    = core::dimension2d<s32>(width, height);
	params.Bits          = 32;
	params.Stencilbuffer = true;
	params.Vsync = true;
	return createDeviceEx(params);
}

/**
 * �E�C���h�E�̍Đݒ�
 */
void
tTVPIrrlichtDrawDevice::attach(HWND hwnd)
{
	// �E�C���h�E�T�C�Y�̎擾
	RECT rect;
	GetClientRect(hwnd, &rect);
	width  = rect.right - rect.left;
	height = rect.bottom - rect.top;

	device = create(hwnd, video::EDT_DIRECT3D9);
	if (device == NULL) {
		device = create(hwnd, video::EDT_DIRECT3D8);
		if (device == NULL) {
			device = create(hwnd, video::EDT_OPENGL);
		}
	}
	if (device == NULL) {
		TVPThrowExceptionMessage(L"Irrlicht �f�o�C�X�̏������Ɏ��s���܂���");
	}

	//device->setWindowCaption(title.c_str());
	driver  = device->getVideoDriver();
	smgr    = device->getSceneManager();
	texture = driver->addTexture(core::dimension2d<s32>(1024, 1024), "layer", ECF_A8R8G8B8);
	
	init();
	
	// �쓮�J�n
	start();
}

/**
 * �E�C���h�E�̉���
 */
void
tTVPIrrlichtDrawDevice::detach()
{
	stop();
	if (device) {
		device->drop();
		device = NULL;
		driver = NULL;
		smgr   = NULL;
		texture = NULL;
	}
}

/***
 * �E�C���h�E�̎w��
 */

void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::SetTargetWindow(HWND wnd)
{
	detach();
	if (wnd != NULL) {
		attach(wnd);
	}
}

/**
 * �r�b�g�}�b�v�R�s�[�����J�n
 */
void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::StartBitmapCompletion(iTVPLayerManager * manager)
{
	// bitmap�����J�n
	if (device && texture) {
		destBuffer = (unsigned char *)texture->lock();
		core::dimension2d<s32> size = texture->getSize();
		destWidth  = size.Width;
		destHeight = size.Height;
		destPitch  = texture->getPitch();
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
	// bits, bitmapinfo �ŕ\�����r�b�g�}�b�v�� cliprect �̗̈���Ax, y �ɕ`�悷��B
	if (device && texture && destBuffer) {
		int srcPitch = -bitmapinfo->bmiHeader.biWidth * 4; // XXX ���߂���
		unsigned char *srcBuffer = (unsigned char *)bits - srcPitch * (bitmapinfo->bmiHeader.biHeight - 1);
		int srcx   = cliprect.left;
		int srcy   = cliprect.top;
		int width  = cliprect.get_width();
		int height = cliprect.get_height();
		// �N���b�s���O
		if (x < 0) {
			srcx  += x;
			width += x;
			x = 0;
		}
		if (x + width > destWidth) {
			width -= ((x + width) - destWidth);
		}
		if (y < 0) {
			srcy += y;
			height += y;
			y = 0;
		}
		if (y + height > destHeight) {
			height -= ((y + height) - destHeight);
		}
		unsigned char *src  = srcBuffer  + srcy * srcPitch  + srcx * 4;
		unsigned char *dest = destBuffer +    y * destPitch +    x * 4;
		for (int i=0;i<height;i++) {
			memcpy(dest, src, width * 4);
			src  += srcPitch;
			dest += destPitch;
		}
	}
}

/**
 * �r�b�g�}�b�v�R�s�[�����I��
 */
void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::EndBitmapCompletion(iTVPLayerManager * manager)
{
	// bitmap �����I��
	if (device && texture) {
		texture->unlock();
		destBuffer = NULL;
	}
}

//---------------------------------------------------------------------------

/**
 * �e�X�g�p����������
 */
void
tTVPIrrlichtDrawDevice::init()
{
#if 0
	// �ȉ��T���v���f�[�^�̃��[�h����
	ttstr dataPath = "sydney.md2";
	dataPath = TVPNormalizeStorageName(dataPath);
	TVPGetLocalName(dataPath);
	int len = dataPath.GetNarrowStrLen() + 1;
	char *str = new char[len];
	dataPath.ToNarrowStr(str, len);
	IAnimatedMesh *mesh = smgr->getMesh(str);
	IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(mesh);
	if (node) {
		node->setMaterialFlag(EMF_LIGHTING, false);
		node->setMD2Animation ( scene::EMAT_STAND );
	}
	smgr->addCameraSceneNode(0, vector3df(0,30,-40), vector3df(0,5,0));
#else
	ttstr dataPath = "map-20kdm2.pk3";
	dataPath = TVPNormalizeStorageName(dataPath);
	TVPGetLocalName(dataPath);
	int len = dataPath.GetNarrowStrLen() + 1;
	char *str = new char[len];
	dataPath.ToNarrowStr(str, len);
	device->getFileSystem()->addZipFileArchive(str);
	
	scene::IAnimatedMesh* mesh = smgr->getMesh("20kdm2.bsp");
	scene::ISceneNode* node = 0;
		
	if (mesh)
		node = smgr->addOctTreeSceneNode(mesh->getMesh(0));
	
	if (node)
		node->setPosition(core::vector3df(-1300,-144,-1249));
	
	smgr->addCameraSceneNodeFPS();
	device->getCursorControl()->setVisible(false);
#endif
}

//---------------------------------------------------------------------------

/**
 * Ogre �Ăяo�������J�n
 */
void
tTVPIrrlichtDrawDevice::start()
{
	stop();
	TVPAddContinuousEventHook(this);
}

/**
 * Ogre �Ăяo��������~
 */
void
tTVPIrrlichtDrawDevice::stop()
{
	TVPRemoveContinuousEventHook(this);
}

/**
 * Continuous �R�[���o�b�N
 * �g���g�����ɂȂƂ��ɏ�ɌĂ΂��
 */
void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::OnContinuousCallback(tjs_uint64 tick)
{
	if (device) {
		device->getTimer()->tick();
		driver->beginScene(true, true, video::SColor(255,100,101,140));
		smgr->drawAll();
		if (texture) {
			driver->draw2DImage(texture, core::position2d<s32>(0,0),
								core::rect<s32>(0,0,width,height), 0, 
								video::SColor(255,255,255,255), true);
		}
		driver->endScene();
	}
};

