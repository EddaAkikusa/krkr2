#ifndef IRRLICHTSCENEMANAGER_H
#define IRRLICHTSCENEMANAGER_H

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
	 * �����̐ݒ�
	 */
	void setAmbientLight(irr::video::SColorf ambientColor);

	/**
	 * �����̎擾
	 */
	irr::video::SColorf getAmbientLight();
	
	/**
	 * �����̐ݒ�
	 * @param position �ʒu�w��
	 * @param color �F�w��
	 * @param radius
	 * @param id ID�w��
	 */
	void addLightSceneNode(irr::core::vector3df position, irr::video::SColorf color, float radius, int id);

	/**
	 * �J�����̐ݒ�
	 * @param position �ʒu�w��
	 * @param lookat �Q�ƈʒu
	 * @param id ID�w��
	 */
	void addCameraSceneNode(irr::core::vector3df position, irr::core::vector3df lookat, int id);
};

#endif
