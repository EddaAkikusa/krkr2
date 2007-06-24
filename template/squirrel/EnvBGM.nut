/**
 * ��BGM�I�u�W�F�N�g
 */
class EnvBgm extends EnvObject {
	
	storage = null;   // �Q�Ƃ��Ă���t�@�C����
	volume = null;    // ����
	loop = null;      // ���[�v�w��
	start = null;     // �J�n�w��
	pause = null;     // �|�[�Y�w��

	transTime = null; // �ϓ�����
	waitFade = null;  // �t�F�[�h�҂��w��
	wait = null;      // �I���҂��w��

	replay = null;  // �Đ��w��
	refade = null;  // ���ʎw��
	repause = null;

	bgmcommands = null;
	
    /**
     * �R���X�g���N�^
     */
	constructor(env) {
		::EnvObject.constructor(env, "bgm");
		bgmcommands = {
			tagname = null, 
			storage = setPlay,
			play = setPlay,
			stop = setStop,
			pause = setPause,
			["resume"] = setResume,
			fade = setFade,
			wait = setWait,
			waitfade = setWaitFade,
			loop = null,
			time = null,
			start = null,
		};
	}

	function init() {
		replay = true;
		storage = null;
		transTime = null;
	}
	
    /**
     * �Đ�����
     * @param param �Đ��Ώۃt�@�C��
     */
    function setPlay(param, elm) {
		//dm("BGM�Đ�:" + param);
		if (param != null) {
			replay = true;
			storage = param;
			if ("volume" in elm) {
				volume    = elm.volume;
			} else {
				volume = 100;
			}
			if ("loop" in elm) {
				loop      = elm.loop;
			}
			if ("start" in elm) {
				start     = elm.start;
			}
			if ("time" in elm) {
				transTime = elm.time;
			}
			player.setBGMFlag(param);
        }
    }

    /**
     * ��~����
     * @param param �t�F�[�h�A�E�g����
     */
	function setStop(param, elm) {
		//dm("BGM��~");
		replay = true;
		storage = null;
		if ("time" in elm) {
			transTime = elm.time;
		} else {
			transTime = param;
		}
    }

    /**
     * �|�[�Y����
     * @param param �t�F�[�h�A�E�g����
     */
	function setPause(param, elm) {
		if (storage != null) {
			repause = true;
			pause = true;
		}
	}

    /**
	 * �ĊJ
	 */
	function setResume(param, elm) {
		if (storage != null) {
			repause = true;
			pause = false;
		}
    }

    /**
     * ���ʃt�F�[�h
     * @param param �t�F�[�h����
     */
	function setFade(param, elm) {
		refade    = true;
		volume    = param;
		if ("time" in elm) {
			transTime = elm.time;
		} else {
			transTime = param;
		}
    }

    /**
     * �I���܂�
     * @param param �t�F�[�h����
     */
    function setWait(param, elm) {
		wait = clone elm;
    }

    /**
     * �I���܂�
     * @param param �t�F�[�h����
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
		if (cmd in bgmcommands) {
			local func = bgmcommands[cmd];
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
		//dm("BGM �p�t�@���N�V�����Ăяo��!");
		wait = null;
		waitFade = null;
		transTime = null;
		local doflag = false;
		foreach (name, value in elm) {
			if (doCommand(name, value, elm)) {
				doflag = true;
			}
		}
		if (!doflag && elm.tagname != "bgm") {
			setPlay(elm.tagname, elm);
        }
	}

	/**
	 * �R�}���h����
	 */
	function sync() {
		if (replay) {
			//dm("�Đ���ԕύX:" + storage);
			if (storage != null) {
				player.playBGM(transTime, {storage=storage, loop=loop, start=start, volume=volume});
			} else {
				player.stopBGM(transTime);
			}
			if (pause) {
				player.pauseBGM(0);
			}
			replay = false;
			refade = false;
			repause = false;
		} else if (refade) {
			player.fadeBGM(transTime, volume);
			refade = false;
		} else if (repause) {
			if (pause) {
				player.pauseBGM(transTime);
			} else {
				player.resumeBGM(transTime);
			}
			repause = false;
		}
		if (waitFade != null) {
			return player.waitBGMFade(getint(waitFade,"timeout"), getint(waitFade, "canskip", true));
		} else  if (wait != null) {
			return player.waitBGMStop(getint(wait, "timeout"), getint(wait, "canskip", true));
		}
        return 0;
    }
};
