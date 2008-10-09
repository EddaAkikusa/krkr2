#include "IrrlichtSceneManager.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace io;
using namespace gui;

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

/**
 * �����̐ݒ�
 */
void 
IrrlichtSceneManager::setAmbientLight(irr::video::SColorf ambientColor)
{
	if (smgr) {
		return smgr->setAmbientLight(ambientColor);
	}
}

/**
 * �����̎擾
 */
irr::video::SColorf
IrrlichtSceneManager::getAmbientLight()
{
	return smgr ? smgr->getAmbientLight() : SColorf();
}

/**
 * �����̐ݒ�
 */
void
IrrlichtSceneManager::addLightSceneNode(irr::core::vector3df position, irr::video::SColorf color, float radius, int id)
{
	if (smgr) {
		smgr->addLightSceneNode(NULL, position, color, radius, id);
	}
}

/**
 * �J�����̐ݒ�
 */
void
IrrlichtSceneManager::addCameraSceneNode(vector3df position, vector3df lookat, int id)
{
	if (smgr) {
		smgr->addCameraSceneNode(NULL, position, lookat, id);
	}
}

