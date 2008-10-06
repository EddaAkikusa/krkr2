#include "IrrlichtSceneManager.h"
#include "ncbind/ncbind.hpp"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace io;
using namespace gui;

extern bool IsArray(tTJSVariant &var);
extern dimension2df getDimension2D(tTJSVariant &var);
extern position2df getPosition2D(tTJSVariant &var);
extern vector2df getVector2D(tTJSVariant &var);
extern vector3df getVector3D(tTJSVariant &var);
extern rect<s32> getRect(tTJSVariant &var);

IrrlichtSceneManager::IrrlichtSceneManager() : smgr(NULL)
{
}

IrrlichtSceneManager::IrrlichtSceneManager(irr::scene::ISceneManager *smgr) : smgr(smgr)
{
	if (smgr) {
		smgr->grab();
	}
}

IrrlichtSceneManager::IrrlichtSceneManager(const IrrlichtSceneManager &orig) : smgr(orig.smgr)
{
	if (smgr) {
		smgr->grab();
	}
}

IrrlichtSceneManager::~IrrlichtSceneManager()
{
	if (smgr) {
		smgr->drop();
		smgr = NULL;
	}
}

// ------------------------------------------------------------
// �V�[������
// ------------------------------------------------------------

/**
 * �V�[���f�[�^�t�@�C���̓ǂݍ���
 * @param filename �V�[���f�[�^�t�@�C��(irr�t�@�C��)
 * @return �ǂݍ��݂ɐ��������� true
 */
bool
IrrlichtSceneManager::loadScene(const char *filename)
{
	return smgr ? smgr->loadScene(filename) : false;
}

void
IrrlichtSceneManager::addCameraSceneNode(tTJSVariant position, tTJSVariant lookat, int id)
{
	if (smgr) {
		vector3df p = getVector3D(position);
		vector3df l = getVector3D(lookat);
		smgr->addCameraSceneNode(NULL, p, l, id);
	}
}

