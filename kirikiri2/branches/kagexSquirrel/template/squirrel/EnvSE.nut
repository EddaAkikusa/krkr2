/**
 * ��SE�I�u�W�F�N�g
 */
class EnvSE extends EnvObject {

	id = null;
	count = 0; // �Q�Ƃ��ꂽ�J�E���g�l

	storage = null;   // �Q�Ƃ��Ă���t�@�C����
	volume = null;    // ����
	loop = null;      // ���[�v�w��
	start = null;     // �J�n�w��

	transTime = null; // �ϓ�����
	waitFade = null;  // �t�F�[�h�҂��w��
	wait = null;      // �I���҂��w��
	
	replay = null;  // �Đ��w��
	refade = null;  // ���ʎw��

    secommands = null;
	
    /**
     * �R���X�g���N�^
     */
    function constructor(env, id) {
		::EnvObject.constructor(env, null);
		this.id = id;
		secommands = {
			tagname = null, 
			storage = setPlay,
			play = setPlay,
			stop = setStop,
			fade = setFade,
			volume = setFade,
			wait = setWait,
			waitfade = setWaitFade,
			loop = null,
			time = null,
			start = null,
			canskip = null,
			buf = null,
		};
	}

	function init() {
		name = null;
		count = 0;
		replay = true;
		storage = null;
		transTime = null;
	}
	
    /**
     * �Đ�����
     * @param param �Đ��Ώۃt�@�C��
     */
	function setPlay(param, elm) {
		if (param != null) {
			replay  = true;
			if (getval(elm, loop) || !(player.isSkip() && player.nosewhenskip)) {
				storage   = param;
				volume    = getval(elm, "volume", 100);
				loop      = getval(elm, "loop", false);
				start     = getval(elm, "start");
				transTime = getval(elm, "time");
			} else {
				storage = null;
			}
		}
	}

    /**
     * ��~����
     * @param param �t�F�[�h�A�E�g����
     */
	function setStop(param, elm) {
		replay    = true;
		storage   = null;
		transTime = getval(elm, "time", param);
    }

    /**
     * ���ʃt�F�[�h
     * @param param �t�F�[�h����
     */
	function setFade(param, elm) {
		refade    = true;
		volume    = param;
		transTime = getval(elm, "time", param);
    }

    /**
     * �I���҂�
     */
    function setWait(param, elm) {
		wait = clone elm;
    }

    /**
     * �t�F�[�h�I���҂�
     */
	function setWaitFade(param, elm) {
		waitFade = clone elm;
    }

    /**
     * �R�}���h�̎��s
     * @param cmd �R�}���h
     * @param param �p�����[�^
     * @param elm ���̃R�}���h���܂ޑS�p�����[�^
     * @return ���s���s��ꂽ�ꍇ true
     */
	function doCommand(cmd, param, elm) {
		if (cmd in secommands) {
			local func = secommands[cmd];
			if (func != null) {
				func(param, elm);
				return true;
            }
			return false;
        }
        // �Đ��R�}���h�Ƃ݂Ȃ�
		setPlay(cmd, elm);
        return true;
    }

    /**
     * �^�O����
     * @param elm ����
     */
	function tagcommand(elm) {
		// dm("SE �p�t�@���N�V�����Ăяo��!");
		wait = null;
		waitFade = null;
		transTime = null;
		local doflag = false;
		foreach (name, value in elm) {
			if (doCommand(name, value, elm)) {
				doflag = true;
			}
		}
		if (!doflag && elm.tagname != "se") {
			setPlay(elm.tagname, elm);
        }
    }

	function sync() {
		if (replay) {
			if (storage != null) {
				player.playSE(id, transTime, {storage=storage, loop=loop, start=start, volume=volume});
			} else {
				player.stopSE(id, transTime);
			}
			replay = false;
			refade = false;
		} else if (refade) {
			player.fadeSE(id, transTime, volume);
			refade = false;
		}
		if (waitFade != null) {
			return player.waitSEFade(id, getint(waitFade, "timeout"), getint(waitFade, "canskip", true));
		} else if (wait != null) {
			return player.waitSEStop(id, getint(wait, "timeout"), getint(wiatFade, "canskip", true));
		}
		return 0;
	}

};
