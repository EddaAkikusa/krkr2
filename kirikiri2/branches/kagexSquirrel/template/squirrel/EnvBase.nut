/**
 * ���I�u�W�F�N�g�̊��N���X����2
 * �E��ʍX�V�@�\�Ή�
 */
class EnvBase extends EnvObject {

	baseCommands = null;
	
	/**
	 * �R���X�g���N�^
	 * @param env ��
	 */
	constructor(env, name) {
		::EnvObject.constructor(env, name);
		baseCommands = {
			trans = setTrans,
			tagname = null,     // �^�O���͖���
			time = null,        // �e�펞�Ԏw��p
			fade = null,
		};
	}

	// --------------------------------------------------------------------
	// �g�����W�V��������
	// --------------------------------------------------------------------
	
    // ��ʍX�V�ݒ�
	trans = null;
	
    /**
	 * �g�����W�V������ݒ�
	 * @param name �g�����W�V������
	 * @param elm �p�����[�^
	 * @return �g�����W�V�������ݒ肳�ꂽ
	 */
	function setTrans(name, elm) {
		//dm("�g�����W�V�����ݒ�:" + name);
		local tr = player.getTrans(name, elm);
		if (tr != null) {
			trans = tr;
			return true;
		}
		return false;
    }

	// �����g�����W�V����
	autoTrans = null;

	// �����g�����W�V������o�^
	function setAutoTrans(param) {
		if (param != null) {
			if (typeof param == "string") {
				local tr = player.getTrans(param);
				if (tr != null && "method" in tr) {
					autoTrans = tr;
					//dm("�����g�����W�V�����w��:" + tr.method + ":" + tr.time);
					return;
				}
			} else if (typeof param == "table") {
				autoTrans = param;
				return;
			}
		}
	}

	/**
	 * �R�}���h�̎��s
	 * @param cmd �R�}���h
	 * @param param �p�����[�^
	 * @param elm ���̃R�}���h���܂ޑS�p�����[�^
	 * @return ���s���s��ꂽ�ꍇ true
	 */
    function doCommand(cmd, param, elm) {
		
		//dm("�R�}���h����:" + cmd + " �p�����[�^:" + param);
		
		if (cmd in baseCommands) {
			local func = baseCommands[cmd];
			if (func != null) {
				func(param, elm);
			}
			return true;
        }

		// �g�����W�V�����p�����[�^��r��
		if (cmd in player.transitionParam) {
			return true;
		}

		// �g�����W�V�����w��̔���
		if (setTrans(cmd, elm)) {
			return true;
		}
		
		return false;
	}
	
	_ret = null;
	
	function getRet() {
		return _ret;
	}
	
	function setRet(v) {
		if (v == null) {
			_ret = 0;
		} else {
			if (v < _ret) {
				_ret = v;
			}
		}
	}

	/**
	 * �ʃR�}���h����
	 * @param elm ����
	 */
	function command(elm) {
		foreach (name, value in elm) {
			if (!doCommand(name, value, elm)) {
				player.errorCmd(name + ":���m�̃R�}���h:" + name);
			}
		}
	}

	/**
	 * �X�V����
	 */
	function doUpdate(elm) {
		player.backup();
		update(false);
		player.beginTransition(trans);
	}

	/**
	 * �X�V����
	 */
	function update(isfore) {
	}

	// �t�F�[�h�w��̃f�t�H���g�l
	function getFadeValue() {
		return env.fadeValue;
    }
	
	/**
	 * �^�O����
	 * @param elm �v�f
	 */
	function tagcommand(elm) {
		
		setRet(null);
		trans = null;
		autoTrans = null;
		
		// �R�}���h�����s
		command(elm);

		// �g�����W�V�����ď���
		if (trans == null) {
			if ("fade" in elm) {
				local fadeTime = elm.fade;
				trans = { time = fadeTime > 1 ? fadeTime : getFadeValue() };
			} else {
				trans = autoTrans;
			}
		}

		if (trans != null) {
			// �L�����N�^��������
			if ("charoff" in trans) {
				foreach (name, value in env.characters) {
					value.disp = CLEAR;
				};
			}
			// ���C����������
			if ("layeroff" in trans) {
				foreach (name, value in env.layers) {
					value.disp = CLEAR;
				};
			}
			// ���b�Z�[�W������
			if ("msgoff" in trans) {
				player.addTag("msgoff");
			}
		}
	}

	function sync(elm) {
		//dm("�X�V�����J�n" + name);
		if (player.transMode) {
			update(player.transMode != 1);
		} else if (trans == null) {
			update(true);
		} else {
			doUpdate(elm);
		}
		return getRet();
	}
};
