/**
 * ���I�u�W�F�N�g
 */
class Environment extends EnvBase {
	
	xmax = null;
	ymax = null;

    // �t�F�[�h�w��̃f�t�H���g�l
    // envinit.tjs �Œ�`���邩�A�V�X�e���̃f�t�H���g���g��
	function getFadeValue() {
		if ("fadeValue" in envinfo) {
			return envinfo.fadeValue;
		} else {
			return 500;
		}
    }
    
    /// ���������
	envinfo = null;

    times= null;        //< ���ԏ��
    stages= null;       //< ������
    events= null;       //< �C�x���g�G���
    positions= null;    //< �z�u���
    actions= null;      //< �A�N�V�������
    transitions= null;  //< �g�����W�V�������
    defaultTime= null;  //< �f�t�H���g�̎���
    yoffset= null;      //< �L�����N�^�z�u��yoffset �l
    defaultXpos= null;  //< �L�����N�^�z�u�̏���X�ʒu
    defaultYpos= null;  //< �L�����N�^�z�u�̏���Y�ʒu
    defaultLevel= null; //< �L�����N�^���x���̃f�t�H���g�l
	levels= null;       //< �L�����N�^���x���ʕ␳���
    faceLevelName= null;//< �t�F�C�X�E�C���h�E�p�̕\����
    
    /// �L�����N�^���������ꗗ
	characterInits = {};

	/// ���䃌�C��
	stage= null;
	/// �C�x���g���C��
	event= null;
	/// �L�����N�^���
	characters = {};
    /// ���C�����
	layers = {};

    // BGM �n
    bgm = null;

    // SE �n
    ses = null;
    
    //�@���ݎ���
	function getCurrentTime() {
		if (stage.timeName != null && stage.timeName in times) {
			return times[stage.timeName];
		}
    }

    // ���b�Z�[�W�������ΏۂɂȂ�f�t�H���g�̃I�u�W�F�N�g
    // ���O�\���̂��Ɛݒ肳���
    // ���n���߂̌�͉��������
	currentNameTarget = null;

	showFaceMode = false; // �R�}���h���s���ɕ\������s�����[�h
	bothFace = true;     // �\��\���𗧂��G�����\��

	envCommands = null;
	
    /**
     * �R���X�g���N�^
	 * @param player �Đ��v���C���[
	 * @param width ��ʉ���
	 * @param height ��ʏc��
     */
	function constructor(player, width, height) {

		this.player = player;
		::EnvBase.constructor(this, "env");
		
		xmax = width / 2;
		ymax = height / 2;
		
		characters = {};
		layers = {};
		event = EnvEventLayer(this);
		stage = EnvStageLayer(this);

		// BGM �I�u�W�F�N�g
		bgm = EnvBgm(this);
		// SE �I�u�W�F�N�g�Q
		ses = [];
		for (local i=0; i<player.senum; i++) {
			ses.append(EnvSE(this, i))
		}
		
		resetEnv();

		envCommands = {
			hidebase = hideBase,
			hideevent = hideEvent,
			hidecharacters = hideCharacters,
			hidechars = hideCharacters,
			hidelayers = hideLayers,
			hidefore = hideFore,
			hideall = hideAll,
		};
    }

	// ------------------------------------------------------

    /**
	 * ���̃��Z�b�g
	 */
	function resetEnv() {

		initEnv(null);

		// �L�����N�^���p��
		foreach (name,value in characterInits) {
			delete characterInits[name];
		}
		
		// ���������W�J
		envinfo = dofile("envinit.nut");
		
		if (envinfo != null) {
            // �f�o�b�O�\�� 
			times        = getval(envinfo, "times", EMPTY);
			stages       = getval(envinfo, "stages", EMPTY);
			events       = getval(envinfo, "events", EMPTY);
			positions    = getval(envinfo, "positions", EMPTY);
            actions      = getval(envinfo, "actions", EMPTY);
			transitions  = getval(envinfo, "transitions", EMPTY);
			levels       = getval(envinfo, "levels", EMPTY);
			defaultTime  = getval(envinfo, "defaultTime");
			yoffset      = getval(envinfo, "yoffset", 0);
			defaultXpos  = getval(envinfo, "defaultXpos", 0);
			defaultYpos  = getval(envinfo, "defaultYpos", 0);
			defaultLevel = getval(envinfo, "defaultLevel", 0);
			faceLevelName = getval(envinfo, "faceLevelName", "");
			showFaceMode  = getval(envinfo, "showFaceMode", false);
			bothFace      = getval(envinfo, "bothFace", true);
        
			// �L�����N�^��񏉊�������
			if ("characters" in envinfo) {
				foreach (name, init in envinfo.characters) {
					characterInits[name] <- init;
				}
			}
			
        } else {
			dm("����񂪂���܂���");
			envinfo = EMPTY;
			times   = EMPTY;
			stages  = EMPTY;
			events  = EMPTY;
			positions = EMPTY;
			actions   = EMPTY;
			transitions = EMPTY;
			levels = EMPTY;
			defaultTime = null;
			yoffset     = 0;
			defaultXpos = 0;
			defaultYpos = 0;
			defaultLevel = 0;
			faceLevelName = "";
			showFaceMode = false;
			bothFave = true;
		}
	}
	
    // -----------------------------------------

    // -----------------------------------------

	/**
	 * �����̏�����
     */
	function initEnv(elm) {

		//dm("����������");
		stage.initImage();
		event.initImage();

		// �L�����N�^���̔j��
		foreach (name,value in characters) {
			delete characters[name];
		}
        // ���ꃌ�C�����̔j��
		foreach (name,value in layers) {
			delete layers[name];
		}

        // SE ������
		for (local i=0;i<ses.len();i++) {
			ses[i].init();
			ses[i].sync();
		}
        seCount = 0;

		// BGM������
		if (elm == null || !getval(elm, "bgmcont")) {
			bgm.init();
		}
		
        // �J�����g�I�u�W�F�N�g������
		currentNameTarget = null;
    }

    /**
     * �C�x���g�G�̏���
     */
    function hideEvent(param, elm) {
		if (event.isShow()) {
			event.disp = EnvImage.CLEAR;
		}
    }

    /**
     * �C�x���g�G�̏���
     */
	function hideStage(param, elm) {
		if (stage.isShow()) {
			stage.disp = EnvImage.CLEAR;
		}
    }
	
    /**
     * �w�i�ƃC�x���g�G�̏���
     */
    function hideBase(param, elm) {
		hideEvent(param, elm);
		hideStage(param, elm);
    }
    
    /**
     * �S�L�����N�^����
     */
	function hideCharacters(param, elm) {
		// �L�����N�^���C���̏���
		foreach (name,value in characters) {
			if (value.isShow()) {
				value.disp = EnvImage.CLEAR;
			}
		}
    }

    /**
     * �S���C������
     */
    function hideLayers(param, elm) {
        // ���ꃌ�C���̏���
		foreach (name,value in layers) {
			if (value.isShow()) {
				value.disp = EnvImage.CLEAR;
			}
		}
    }

    /**
     * �O�i�v�f����
     */
    function hideFore(param, elm) {
        hideCharacters(param, elm);
        hideLayers(param, elm);
    }

    /**
     * �S�v�f����
     */
    function hideAll(param, elm) {
		hideBase(param, elm);
		hideCharacters(param, elm);
        hideLayers(param, elm);
    }

    /**
     * �R�}���h�̎��s
     * @param cmd �R�}���h
     * @param param �p�����[�^
     * @param elm ���̃R�}���h���܂ޑS�p�����[�^
     * @return ���s���s��ꂽ�ꍇ true
     */
    function doCommand(cmd, param, elm) {

		// ���ʃR�}���h
		if (::EnvBase.doCommand(cmd, param, elm)) {
			return true;
        }
		
		if (cmd in envCommands) {
			local func = envCommands[cmd];
			if (func != null) {
				func(param, elm);
			}                
			return true;
        }

		return false;
    }

	function update(isfore) {
		//dm("���X�V");
		foreach (name,value in layers) {
			value.update(isfore);
		};
		event.update(isfore);
		// XXX �L�����X�V�̓X�e�[�W�X�V�Ɋ܂܂��
		stage.update(isfore);
	}

	// ------------------------------------------------------------------
	// �ꊇ���s
	// ------------------------------------------------------------------

	/**
	 * �S�L�����ɃR�}���h���s
	 * @param elm ����
	 * poscond �p�����[�^�ŕ\���ꏊ����\
     */
    function allchar(elm) {
		ret = null;
		local posName;
		if ("poscond" in elm) {
			posName = elm.poscond;
			delete elm.poscond;
		}
		foreach (name,ch in characters) {
			if (posName != null) {
				if (ch.posName == posName && ch.isShowBU()) {
					ret = ch.tagfunc(elm);
				}
			} else {
				ret = ch.tagfunc(elm);
			}
        }
        return ret;
    }

    /**
     * �S���C���ɃR�}���h���s
     * @param elm ����
     */
    function alllayer(elm) {
		ret = null;
		foreach (name,value in layers) {
			ret = value.tagfunc(elm);
        }
        return ret;
    }

    /**
     * �SSE�ɃR�}���h���s
     * @param elm ����
     */
    function allse(elm) {
        ret = null;
		for (local i=0;i<ses.len();i++) {
			if (ses[i].name != null) {
				ret = ses[i].tagfunc(elm);
			}
		}
		return ret;
    }

    /**
     * �V�K���C������
     */
    function newLayer(elm) {
		// �����f�[�^�͔p��
		delLayer(elm);
		// �V�K����
		if ("name" in elm) {
			local lay = getEnvLayer(elm.name, true);
			if (lay != null) {
				delete elm.tagname;
				delete elm.name;
				return lay.tagfunc(elm);
			}
		} else {
			player.error("���C�������w�肳��Ă��܂���");
		}
		return 0;
    }

    /**
     * �V�K���C������
     */
    function delLayer(elm) {
		if ("name" in elm && elm.name in layers) {
			delete layers[elm.name];
		}
        return 0;
    }
    
    /**
     * �V�K�L�����N�^����
     */
    function newCharacter(elm) {
		// �����f�[�^�͔p��
		delCharacter(elm);
        // �V�K����
		if ("name" in elm && "initname" in elm) {
			local ch = getCharacter(elm.name, elm.initname);
			if (ch != null) {
				delete elm.tagname;
				delete elm.name;
				delete elm.initname;
				return ch.tagfunc(elm);
			}
		} else {
			player.error("�L�����N�^���܂��͏����������w�肳��Ă��܂���");
		}
        return 0;
    }        

	/**
     * �L�����N�^����
     */
	function delCharacter(elm) {
		if ("name" in elm && elm.name in characters) {
			delete characters[elm.name];
		}
		return 0;
	}
    
	/**
	 * ���C���ꊇ�����w��
	 */
	function onCopyLayer(toback, backlay) {
		foreach (name, value in characters) {
			value.onCopyLayer(toback, backlay);
		}
		foreach (name, value in layers) {
			value.onCopyLayer(toback, backlay);
		}
		stage.onCopyLayer(toback, backlay);
		event.onCopyLayer(toback, backlay);
	}

 	// ------------------------------------------------------------------
	// �L�����N�^�֌W����
	// ------------------------------------------------------------------

    /**
     * �w�肳�ꂽ���O�̃L�����N�^��Ԃ�
     * @param name ���O
     * @param initName ��������
     */
    function getCharacter(name, initName) {
		local ch;
		if (name in characters) {
			ch = characters[name];
		} else {
			if (initName == null) {
				initName = name;
			}
			if (initName in characterInits) {
				ch = EnvCharacter(env, name, initName, characterInits[initName]);
				characters[name] = ch;
            }
        }
        return ch;
    }

	// ------------------------------------------------------------------
	// ���C���֌W����
	// ------------------------------------------------------------------

	/**
    * �w�肳�ꂽ���O�̃��C����Ԃ�
     * @param name ���O
	 * @param create �������[�h
     */
	function getEnvLayer(name, create) {
		local lay;
		if (name in layers) {
			lay = layers[name];
		}
		if (create) {
			lay = EnvSimpleLayer(env, name);
			layers[name] = lay;
		}
        return lay;
    }

	// --------------------------------------------------------------
	// SE �����p
	// --------------------------------------------------------------

	seCount = 0;

	/**
     * SE �����p�I�u�W�F�N�g�̎擾
     * @param id SE�ԍ��w��
     * ��ԌÂ�SE���킩��悤�ɃJ�E���g���������Ă���
     */
    function getSe(id) {
        ses[id].count = seCount++;
        return ses[id];
    }

    /**
     * SE �� ID �����肷��
     * @param buf �o�b�t�@ID���w��
     */
    function getSeId(buf) {
		// ���ڃo�b�t�@���w�肳��Ă���ꍇ�͂����Ԃ�
		if (buf != null && buf.tointeger() < ses.len()) {
			return buf.tointeger();
        }
        // �g���ĂȂ����̂�������
        local max = seCount;
		local old = null;
        for (local i=0; i<ses.len(); i++) {
            if (ses[i].name == null) {
				return i;
            }
			if (ses[i].len() < max) {
				max = ses[i].len();
                old = i;
            }
        }
        // ��ԌÂ����̂�Ԃ�
		return old;
    }

    /**
     * SE �� ID �����肷��
     * @param name SE �̖��O
     */
    function getSeIdFromName(name) {
        for (local i=0; i<ses.len(); i++) {
            if (ses[i].name == name) {
                return i;
            }
		}
        // �݂���Ȃ��̂ł����Ă���ԍ���Ԃ�
        return getSeId();
    }

    /**
     * SE ��~���̏���
     * ��~����Ԃɂ���
     */
    function onSeStop(id) {
        if (id < ses.len()) {
            ses[id].name = null;
        }
    }

	// --------------------------------------------------------------
	
    /**
	 * �s���R�}���h�����n���h��
     */
    function unknown(tagName, elm) {

		//dm("���s���R�}���h����:" + tagName);

		// �X�e�[�W�p����
		if (tagName == "stage") {
			return stage.tagfunc(elm);
		}
		
		// �C�x���g�p����
		if (tagName == "event" || tagName == "ev") {
			return event.tagfunc(elm);
		} else if (tagName.slice(0,2) == "ev" || (tagName in events)) {
			elm[tagName] <- true;
			return event.tagfunc(elm);
        }

        // BGM �����p
        if (tagName == "bgm") {
            return bgm.tagfunc(elm);
        } else if (tagName.slice(0,3) == "bgm") {
            return bgm.tagfunc(elm);
        }

        // SE �����p
        if (tagName == "se") {
			if (elm.name != null) {
				return getSe(getSeIdFromName(elm.name)).tagfunc(elm);
			} else {
				return getSe(getSeId(elm.buf)).tagfunc(elm);
			}
        } else if (tagName.slice(0,2) == "se") {
			local se = getSe(getSeIdFromName(tagName));
			return se.tagfunc(elm);
        }
        
        // �L�����N�^
        if (tagName == "char") {
            local ch = getCharacter(elm.name);
            if (ch != null) {
                return ch.tagfunc(elm);
            }
		} else {
			local ch = getCharacter(tagName);
			if (ch != null) {
				return ch.tagfunc(elm);
			}
		}

        if (tagName == "layer") {
            local lay = getEnvLayer(elm.name, false);
			if (lay != null) {
                return lay.tagfunc(elm);
            }
		} else { 
			local lay = getEnvLayer(tagName, false);
			if (lay != null) {
				return lay.tagfunc(elm);
			}
		}

		// ���Ԏw�肩�w�i�w��Ȃ�X�e�[�W�p�R�}���h�Ƃ݂Ȃ�
		if (tagName in times || tagName in stages) {
			elm.tagname  <- "stage";
			elm[tagName] <- true;
			return stage.tagfunc(elm);
		}
		
		// ���̃R�}���h
		if (tagName in envCommands) {
			elm.tagname  <- "env";
			elm[tagName] <- true;
			return tagfunc(elm);
		}

		// �J�����g�̃^�[�Q�b�g������ꍇ�͂��̃R�}���h�Ƃ��Ď��s
		if (currentNameTarget != null) {
			elm[tagName] <- true;
			return currentNameTarget.tagfunc(elm);
		}

		// ����ȊO���ƃG���[�ɂȂ�
	}

    /**
	 * ������
	 */
	function syncAll() {
		dm("���S����");
		for (local i=0; i<player.senum; i++) {
			ses[i].sync();
		}
		bgm.sync();
		foreach (name,value in layers) {
			value.sync();
		};
		event.sync();
		stage.sync();
		return 0;
	}
};
