#ifndef IRRLICHTSCENEMANAGER_H
#define IRRLICHTSCENEMANAGER_H

#include <windows.h>
#include "tp_stub.h"
#include <irrlicht.h>

/**
 * �V�[���}�l�[�W������p�N���X
 */
class IrrlichtSceneManager {

protected:
	irr::scene::ISceneManager *smgr;
	
public:
	IrrlichtSceneManager();
	IrrlichtSceneManager(irr::scene::ISceneManager *smgr);
	IrrlichtSceneManager(const IrrlichtSceneManager &orig);
	~IrrlichtSceneManager();

	// ------------------------------------------------------------
	// �V�[������
	// ------------------------------------------------------------
public:	
	/**
	 * �V�[���f�[�^�t�@�C���̓ǂݍ���
	 * @param filename �V�[���f�[�^�t�@�C��(irr�t�@�C��)
	 * @return �ǂݍ��݂ɐ��������� true
	 */
	bool loadScene(const char *filename);

	/**
	 * �J�����̐ݒ�
	 */
	void addCameraSceneNode(tTJSVariant position, tTJSVariant lookat, int id);
};

#endif
