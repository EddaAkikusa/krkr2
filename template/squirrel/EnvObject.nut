/**
 * ���I�u�W�F�N�g�̊��N���X
 */
class EnvObject {

	// ���s��
	env = null;

	// ���s�v���C���[
	player = null;

	// ���O
	name = null;

	constructor(env, name) {
		this.env    = env;
		this.player = env.player;
		this.name   = name;
	}

	// ���b�Z�[�W�\���p
	function dm(msg) {
		return player.dm(msg);
	}

	/**
	 * �R�}���h�̏���
	 */
	function tagfunc(elm) {
		//dm("�R�}���h����:" + name);
		tagcommand(elm);
		if (!player.isJump()) {
			return sync(elm);
		}
		return 0;
	}

	/**
	 * �R�}���h���s
	 */
	function tagcommand(elm) {
	}
	
	/**
	 * ��ԓ���
	 */
	function sync(elm) {
		return 0;
	}
}
