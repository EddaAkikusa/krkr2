#include <windows.h>

#include "IrrlichtDrawDevice.h"

/**
 * �R���X�g���N�^
 */
LayerManagerInfo::LayerManagerInfo(ITexture *texture) : texture(texture)
{
	destBuffer = NULL;
};

/**
 * �f�X�g���N�^
 */
LayerManagerInfo::~LayerManagerInfo()
{
}


/**
 * �e�N�X�`�������b�N���ĕ`��̈�����擾����
 */
void
LayerManagerInfo::lock()
{
	if (texture) {
		destBuffer = (unsigned char *)texture->lock();
		core::dimension2d<s32> size = texture->getSize();
		destWidth  = size.Width;
		destHeight = size.Height;
		destPitch  = texture->getPitch();
	} else {
		destBuffer = NULL;
	}
}

/**
 * ���b�N���ꂽ�e�N�X�`���Ƀr�b�g�}�b�v�`����s��
 */
void
LayerManagerInfo::copy(tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
					   const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity)
{
	// bits, bitmapinfo �ŕ\�����r�b�g�}�b�v�� cliprect �̗̈���Ax, y �ɕ`�悷��B

	if (destBuffer) {
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
 * �e�N�X�`���̃��b�N�̉���
 */
void
LayerManagerInfo::unlock()
{
	if (texture) {
		texture->unlock();
		destBuffer = NULL;
	}
}

/**
 * �R���X�g���N�^
 */
tTVPIrrlichtDrawDevice::tTVPIrrlichtDrawDevice()
{
	device = NULL;
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
	if (device) {
		video::IVideoDriver* driver = device->getVideoDriver();

		// �e�N�X�`�����蓖��XXX �T�C�Y���肪�K�v�c
		ITexture *texture = driver->addTexture(core::dimension2d<s32>(1024, 1024), "layer", ECF_A8R8G8B8);
		if (texture == NULL) {
			TVPThrowExceptionMessage(L"�e�N�X�`���̊��蓖�ĂɎ��s���܂���");
		}

		manager->SetDrawDeviceData((void*)new LayerManagerInfo(texture));
	}
}

/**
 *  �e�N�X�`���̉��
 * @param manager ���C���}�l�[�W��
 */
void
tTVPIrrlichtDrawDevice::freeInfo(iTVPLayerManager * manager)
{
	if (device) {
		LayerManagerInfo *info = (LayerManagerInfo*)manager->GetDrawDeviceData();
		if (info != NULL) {
			video::IVideoDriver* driver = device->getVideoDriver();
			driver->removeTexture(info->texture);
			manager->SetDrawDeviceData(NULL);
			delete info;
		}
	}
}

/**
 * �f�o�C�X�̐���
 * @param hwnd �n���h��
 * @param �h���C�o�̎��
 */
IrrlichtDevice *
tTVPIrrlichtDrawDevice::create(HWND hwnd, video::E_DRIVER_TYPE type)
{
	SIrrlichtCreationParameters params;
	params.WindowId     = reinterpret_cast<s32>(hwnd);
	params.DriverType    = type;
//	params.WindowSize    = core::dimension2d<s32>(width, height);
	params.Bits          = 32;
	params.Stencilbuffer = true;
	params.Vsync = true;
	params.EventReceiver = this;
	return createDeviceEx(params);
}

/**
 * �E�C���h�E�̍Đݒ�
 * @param hwnd �n���h��
 */
void
tTVPIrrlichtDrawDevice::attach(HWND hwnd)
{
	// �E�C���h�E�T�C�Y�̎擾 XXX
	RECT rect;
	GetClientRect(hwnd, &rect);
	width  = rect.right - rect.left;
	height = rect.bottom - rect.top;

	// �f�o�C�X����
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
	
	// �}�l�[�W���ɑ΂���e�N�X�`���̊��蓖��
	for (std::vector<iTVPLayerManager *>::iterator i = Managers.begin(); i != Managers.end(); i++) {
		allocInfo(*i);
	}

	// XXX �e�X�g�p
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
	if (device) {
		stop();
		for (std::vector<iTVPLayerManager *>::iterator i = Managers.begin(); i != Managers.end(); i++) {
			freeInfo(*i);
		}
		device->drop();
		device = NULL;
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

//---------------------------------------------------------------------------

/**
 * �e�X�g�p����������
 */
void
tTVPIrrlichtDrawDevice::init()
{
	/// �V�[���}�l�[�W��
	ISceneManager* smgr = device->getSceneManager();

#if 1
	// �ȉ��T���v���f�[�^�̃��[�h����
	ttstr dataPath = "../../../media/sydney.md2";
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
	ttstr dataPath = "../../../media/map-20kdm2.pk3";
	dataPath = TVPNormalizeStorageName(dataPath);
	TVPGetLocalName(dataPath);
	int len = dataPath.GetNarrowStrLen() + 1;
	char *str = new char[len];
	dataPath.ToNarrowStr(str, len);
	device->getFileSystem()->addZipFileArchive(str);
	
	scene::IAnimatedMesh* mesh = smgr->getMesh("../../../media/20kdm2.bsp");
	scene::ISceneNode* node = 0;
		
	if (mesh)
		node = smgr->addOctTreeSceneNode(mesh->getMesh(0));
	
	if (node)
		node->setPosition(core::vector3df(-1300,-144,-1249));
	
	smgr->addCameraSceneNodeFPS();
//	device->getCursorControl()->setVisible(false);
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

		/// �h���C�o
		video::IVideoDriver* driver = device->getVideoDriver();

		// �`��J�n
		driver->beginScene(true, true, video::SColor(255,0,0,0));

		/// �V�[���}�l�[�W��
		ISceneManager* smgr = device->getSceneManager();
		smgr->drawAll();

		// �ʃ��C���}�l�[�W���̕`��
		for (std::vector<iTVPLayerManager *>::iterator i = Managers.begin(); i != Managers.end(); i++) {
			LayerManagerInfo *info = (LayerManagerInfo*)(*i)->GetDrawDeviceData();
			if (info && info->texture) {
				driver->draw2DImage(info->texture, core::position2d<s32>(0,0),
									core::rect<s32>(0,0,width,height), 0, 
									video::SColor(255,255,255,255), true);
			}
		}
		// �`�抮��
		driver->endScene();
	}
};

/**
 * �C�x���g��
 * @param event �C�x���g���
 * @return ���������� true
 */
bool
tTVPIrrlichtDrawDevice::OnEvent(SEvent event)
{
	switch (event.EventType) {
	case EET_KEY_INPUT_EVENT:
		int shift = 0;
		if (event.KeyInput.Shift) {
			shift |= 0x01;
		}
		if (event.KeyInput.Control) {
			shift |= 0x04;
		}
		if (event.KeyInput.PressedDown){
			OnKeyDown(event.KeyInput.Key, shift); 
	    } else {
			OnKeyUp(event.KeyInput.Key, shift);
        }
        return true;
	}
	return false;
}
