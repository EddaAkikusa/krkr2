/**
 * ���I�u�W�F�N�g
 * getter/setter ���\�b�h�ɂ��v���p�e�B�@�\������
 */
class Object {

	/**
	 * �R���X�g���N�^
	 */
	constructor() {
	}
	
	/**
	 * �v���p�e�B�̑��݊m�F
	 * @param name �v���p�e�B��
	 * getter �֐�������ꍇ�͂�����Q�Ƃ���B�Ȃ��ꍇ�͒��ڎQ�Ƃ���
	 */
	function hasProp(name) {
		local name2 = "set" + name.slice(0,1).toupper() + name.slice(1);
		if (name2 in this && typeof this[name2] == "function") {
			return true;
		}
		return (name in this);
	}

	/**
	 * �v���p�e�B�̎擾
	 * @param name �v���p�e�B��
	 * getter �֐���T���Ă�����Ăяo��
	 */
	function _get(name) {
		local name2 = "get" + name.slice(0,1).toupper() + name.slice(1);
		if (name2 in this) {
			return this[name2]()
		}
		throw "no such property " + name;
	}

	/**
	 * �v���p�e�B�̐ݒ�
	 * @param name �v���p�e�B��
	 * @param value �l
	 * setter �֐���T���Ă�����Ăяo��
	 */
	function _set(name, value) {
		local name2 = "set" + name.slice(0,1).toupper() + name.slice(1);
		if (name2 in this) {
			this[name2](value);
			return;
		}
		throw "no such property " + name;
	}
};

class Test extends Object {
	constructor() {
		::Object.constructor();
	}
	_width = null;
	function getWidth() {
		return _width;
	}
	function setWidth(v) {
		_width = v;
	}
};
