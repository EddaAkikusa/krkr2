/**
 * �V�[���Đ��v���C���[
 * �E�V�[���̍Đ��@�\
 * �E�V�[���̃Z�[�u�ƃ��[�h
 */
class ScenePlayer extends GamePlayer
{

	/**
	 * ���ǊǗ�
	 * ���ׂăX�L�b�v
	 */
	function isAllSkip() {
		return false;
	}

	/**
	 * ���ǊǗ�
	 * @param �V�[��
	 */
	function getReaded(scene) {
		return false;
	}

	/**
	 * ���ǊǗ�
	 * @param scene �V�[��
	 * @param �s�ԍ�
	 */
	function setReaded(scene, line) {
	}

	/**
	 * �L�����\�����x
	 */
	function getCharSpeed() {
		return 0;
	}

	env = null; // �����
	
	lines = null;    // �p�[�X����
	pendings = []; // ���荞�ݏ���

	/**
	 * �^�O���荞�ݏ���
	 */
	function addTag(name, elm) {
		local e = clone elm;
		if (name != null) {
			e.tagname = name;
		}
		pendings.append(e);
    }
	
	handlers = null;      // �����n���h���Q
	
	// ���s���
	curStorage = null;        // ���s���̃V�i���I
	cur = 0;               // ���s�|�C���g
	curLine = 0;           // �ʉߒ��̍s�ԍ�
	targetLine = null;        // ���[�h�����^�[�Q�b�g�s
	nextScene = null;         // ���̃V�[��

	/**
	 * ���s�W�����v����
	 */
	function isJump() {
		return targetLine != null && targetLine < curLine;
	}
	
	// �I�������ʉߋL�^
	branches = {};
	
	lastTagName = ""; // ���O�̃^�O��

	sceneInfos = null; // �V�[�����
	
	/**
	 * �R���X�g���N�^
	 */
	constructor() {
		::GamePlayer.constructor();
		env = Environment(this, SCWIDTH, SCHEIGHT);
		sceneInfos = dofile("scenes.nut");
		handlers = getHandlers();
	}

	/**
	 * ���N���A
	 */
	function clear() {
		// XXX �e�̃N���A���Ă�
		lastTagName = "";
		foreach (name,value in branches) {
			delete branches[name];
		}
		curStorage = null;
		cur = 0;
		curLine = 0;
		targetLine = null;
		nextScene = null;
	}

	// ------------------------------------------------------------
	
	/**
	 * �^�O�̏���
	 */
	function onTag(elm) {
		local tagname = elm.tagname;
		//dm("�V�[���^�O:" + tagname);
		if (tagname in handlers) {
			local handler = handlers[tagname];
			local ret = handler(elm);
			lastTagName = tagname;
			return ret;
		}
		// �����ȃ^�O
		local ret = env.unknown(tagname, elm);
		if (ret == null) {
			local msg = "�^�O/�}�N�� \"" + tagname + "\" �͑��݂��܂���";
			errorCmd(msg);
			return 0;
			//throw new Exception(msg);
		}
		return ret;
	}

	// --------------------------------------------------------------

	/**
	 * �V�[���t�@�C����ϊ����ăf�[�^�Ƃ��Ď擾����
	 * @param name �V�[����
	 */
	function getScene(name) {
		// �O���[�o���̃V�[���擾�֐����g��
		return ::getScene(name);
	}

	/**
	 * ���V�[�����菈��
	 * @param name ���݂̃V�[����
	 */
	function getNextScene(name) {
		if (name in sceneInfos) {
			local info = sceneInfos[name];
			if ("next" in info) {
				local next = info.next;
				for (local i=0;i<next.len();i++) {
					if (!("cond" in next) || next.cond == "" || eval(next.cond)) {
						return next.scene;
					}
				}
			}
		}
	}

	// --------------------------------------------------------------

	
	/**
	 * ���s�J�n
	 * @param storage �J�n�V�i���I
	 * @param option �p�[�X�p�I�v�V����
	 */
	function start(storage) {
		clear();
		curStorage = storage;
		lines = getScene(curStorage);
		status = RUN;
	}
	
	/**
	 * ��ԕۑ�
	 */
	function save(dic) {
		dic.status <- status;
		if (status != STOP) {
			dic.storage  <- curStorage;
			dic.branches <- clone branches;
			if (curLine > 0) {
				dic.targetLine <- curLine;
			}
		}
		return dic;
	}

	/**
	 * ��ԕ��A
	 * �p�����[�^: storage
	 */
	function load(dic) {
		clear();
		if (dic.status != STOP) {
			curStorage = dic.storage;
			branches = clone dic.branches;
			lines = getScene(curStorage);
			targetLine = dic.targetLine;
			status = RUN;
		}
	}

	/**
	 * �w�胉�x���܂ňړ�
	 */
	function goToLabel(label) {
		local newcur = 0;
		local obj;
		while ((obj = lines[newcur++]) != null) {
			if (obj.tagname == "label" && obj.label == label) {
				cur = newcur;
				return;
			}
		}
		throw "���x��" + label + "���݂���܂���";
	}

	
	/**
	 * ���C�����W�b�N
	 */
	function run()	{
		local obj;
		for(;;)	{
			if (pendings.len() > 0) {
				// ��񂵂ɂ��ꂽ�^�O������ꍇ
				obj = pendings[0];
				pendings.remove(0);
			} else {
				obj = lines[cur++];
			}
			
			if(obj == null) {
				// �V�i���I�I��
				local next = nextScene != null ? nextScene : getNextScene(curStorage);
				if (next != null) {
					clear();
					lines = getScene(next);
					status = RUN;
					continue;
				} else {
					status = STOP;
					return;
				}
			} else if (obj.tagname == "line") {
				if (curLine != obj.line) {
					curLine = obj.line;
					//dm("%06d:%s %s".sprintf(curLine, curStorage, obj.lineStr));
				}
				continue;
			} else if (obj.tagname == "label") {
				continue;
			} else {
				// ���s�����肳��� cond
				if ("condex" in obj) {
					if (!eval(obj.condex)) {
						continue;
					}
					delete obj.condex;
				} else if ("if" in obj) {
					if (!eval(obj["if"])) {
						continue;
					}
					delete obj["if"];
				}
				
				// onTag ���Ă�
				local step = onTag(obj);
				if (step == null) {
					throw "onTag �� null ��Ԃ��܂��� (" + obj.tagname + ")";
				}
				step = step.toingter(); // step �𐔒l��
				//dm("�^�O�߂�l:" + step);
				
				if (step == 0) {
					// �E�F�C�g���|�����Ɏ���
					continue;
				}
				if(step < 0) {
					switch(step) {
					case -5: // ��������C�x���g������(���݂̃^�O�͌��)
						pendings.insert(0, obj);
						return;
					case -4: // ��������C�x���g������
						return;
					case -3: // ��񂵂��ău���[�N
						pendings.insert(0, obj);
						return;
					case -2: // �u���[�N
						return;
					case -1: // �V�i���I�I��
						status = STOP;
						return;
					}
				} else {
					// �����\���҂��p XXX
					waitTime(step, true);
				}
			}
		}
	}

	// -----------------------------------------------------------

	// �g�����W�V��������
	transitionName = {
		universal = true,
		crossfade = true,
		scroll = true,
		wave = true,
		mosaic = true,
		turn = true,
		rotatezoom = true,
		rotatevanish = true,
		rotateswap = true,
		ripple = true,
	};

	// �g�����W�V�����p�����[�^
	transitionParam = {
		time = true,
		rule = true,
		vague = true,
		from = true,
		stay = true,
		wavetype = true,
		maxh = true,
		maxomega = true,
		bgcolor1 = true,
		bgcolor2 = true,
		mosaic = true,
		maxsize = true,
		turn = true,
		bgcolor = true,
		factor = true,
		accel = true,
		twistaccel = true,
		twist = true,
		centerx = true,
		centery = true,
		rwidth = true,
		roundness = true,
		speed = true,
		maxdrift = true,
		msgoff = true,
		charoff = true,
		transwait = true,
		showaction = true,
		hideaction = true,
	};
	
    /**
     * �g�����W�V�������̎擾
     */
    function getTrans(name, elm) {
		if (name == "void" || name == "none") {
			return {};
		}
		if (name in env.transitions) {
			//dm("�o�^���݂̃g�����W�V����:" + name);
			local tr = clone env.transitions[name];
			foreach (name, value in elm) {
				if (name in transitionParam) {
					tr[name] <- value;
				}
			};
			return tr;
		} else if (elm != null &&
				   (name in transitionName || name.slice(0,5) == "trans")) {
			//dm("�K��̃V�X�e���g�����W�V����:" + name);
			local tr = {};
			foreach (name, value in elm) {
				if (name in transitionParam) {
					tr[name] <- value;
				}
			};
			tr.method <- name;
			return tr;
		}
    }

	/*
	 * �g�����W�V�������s�G���g��
	 * @param trans �g�����W�V�������
	 * KAG �Ƀg�����W�V�������s���߂���������
	 */
	function beginTransition(trans) {
		if (trans == null || !("method" in trans) || !("time" in trans) || trans.time == 0 || isSkip()) {
			addTag("forelay");
		} else {
			// ���ۂ̃g�����W�V�������s
			if (trans.method == null) {
				trans.method = "crossfade";
			}
			addTag("trans", trans);
			if ("transwait" in trans) {
				// ���ԑ҂�
				local time = trans.time.tointeger() + trans.transwait.tointeger();
				addTag("wait", {time=time, trans=true});
			} else {
				// �g�����W�V�����҂�
				addTag("wt");
			}
		}
		// ���b�Z�[�W����ԓ���
		//addTag("syncmsg");
	}
	
	// ------------------------------------------------------------------
	// ���b�Z�[�W�\���n�^�O
	// ------------------------------------------------------------------

	function tag_ch(elm) {
		//return kag.tagHandlers.ch(...);
		return 0;
	}

	function tag_r(elm) {
		//return kag.tagHandlers.r(...);
		return 0;
	}

	function tag_ruby(elm) {
		//return kag.tagHandlers.ruby(...);
		return 0;
	}

	function tag_font(elm) {
		//return kag.tagHandlers.font(...);
		return 0;
	}

	function tag_deffont(elm) {
		//return kag.tagHandlers.deffont(...);
		return 0;
	}

	function tag_resetfont(elm) {
		//return kag.tagHandlers.resetfont(...);
		return 0;
	}

	function tag_style(elm) {
		//return kag.tagHandlers.style(...);
		return 0;
	}

	function tag_defstyle(elm) {
		//return kag.tagHandlers.defstyle(...);
		return 0;
	}

	function tag_resetstyle(elm) {
		//return kag.tagHandlers.resetstyle(...);
		return 0;
	}

	function tag_l(elm) {
		//return kag.tagHandlers.l(...);
		return 0;
	}

	function tag_p(elm) {
		//return kag.tagHandlers.p(...);
		return 0;
	}

	function tag_er(elm) {
		//return kag.tagHandlers.er(...);
		return 0;
	}

	function tag_indent(elm) {
		//return kag.tagHandlers.indent(...);
		return 0;
	}
	
	function tag_endindent(elm) {
		//return kag.tagHandlers.endindent(...);
		return 0;
	}

	function tag_delay(elm) {
		//return kag.tagHandlers.delay(...);
		return 0;
	}

	// --------------------------------------------------
	// ����n�^�O
	// --------------------------------------------------
	
	function tag_cancelskip(elm) {
		//return kag.tagHandlers.cancelskip(...);
		return 0;
	}

	timeOrigin = null;
	lastWaitTime = 0;
	
	function tag_resetwait(elm) {
		timeOrigin = currentTick;
		return 0;
	}
	
	function tag_wait(elm) {
		local waittime;
		if (getval(elm, "mode") == "until")	{
			// until ���[�h
			waittime = timeOrigin + getint(elm, "time") - currentTick;
			if (waittime < 0) { lastWaitTime = 0; return 0; } // ���łɎ��Ԃ��o�߂��Ă���
			lastWaitTime = waittime;
			if (waittime < 10) return 0; // ���܂�ɑ҂����Ԃ��Z���̂ő҂��Ȃ�
		} else {
			waittime = getint(elm, "time");
		}
		return waitTime(waittime, getint(elm, "canskip", true));
						//getint(elm, "trans"),
						//getint(elm, "action"));
	}

	function tag_waittrig(elm) {
		return waitTrigger(getval(elm, "name"),
						   getval(elm, "timeout"),
						   getval(elm, "canskip", true),
						   getval(elm, "onskip"));
	}
	
	function tag_beginskip(elm) {
		print("beginSkip\n");
		if (!isJump()) {
			if (prevSkipMode != null) {
				print("beginSkip �͓���q�ł��܂���");
			} else {
				prevSkipMode = skipMode;
				if (skipMode) {
				skipMode = 3;
				}
			}
		}
	}

	function tag_endskip(elm) {
		if (!isJump()) {
			if (skipMode == 3) {
				syncAll();
			}
			skipMode = prevSkipMode;
			prevSkipMode = null;
		}
		return 0;
	}
	
	// --------------------------------------------------
	// ��ʐ���n
	
	function tag_backlay(elm) {
		//return kag.tagHandlers.backlay(...);
		return 0;
	}

	function tag_forelay(elm) {
		//return kag.tagHandlers.forelay(...);
		return 0;
	}
	
	function tag_trans(elm) {
		//return kag.tagHandlers.trans(...);
		return 0;
	}

	function tag_wt(elm) {
		//return kag.tagHandlers.wt(...);
		return 0;
	}

	// --------------------------------------------------
	// �A�N�V��������n
	
	function tag_action(elm) {
		local target = getval(elm, "target");
		if (typeof target == "string") {
			target = eval(target);
		}
		if (target != null && typeof target == "instance") {
			local action;
			if ("action" in elm) {
				action = elm.action;
			} else {
				action = clone elm;
				delete action.tagname;
				delete action.layer;
				delete action.page;
				delete action.target;
			}
			beginAction(target, action, onActionCompleted, getint(elm, "nowait"));
		} else {
			error("�A�N�V�����̓I�u�W�F�N�g�Ɏw�肷��K�v������܂�");
		}
		return 0;
	}
	
	function tag_stopaction(elm) {
		if ("target" in elm) {
			local target = elm.target;
			if (typeof target == "string") {
				target = eval(target);
			}
			if (target != null && typeof target == "instance") {
				stopAction(target);
			} else {
				dm("�A�N�V�����̓I�u�W�F�N�g�Ɏw�肷��K�v������܂�");
			}
		} else {
			stopAllActions(getint(elm, "all", true));
		}
		return 0;
	}

	function tag_wact(elm) {
		if ("target" in elm) {
			local target = elm.target;
			if (typeof target == "string") {
				target = eval(target);
			}
			if (target != null && typeof target == "instance") {
				return waitAction(target, getint(elm, "canskip"));
			} else {
				dm("�A�N�V�����҂��̓I�u�W�F�N�g�Ɏw�肷��K�v������܂�");
			}
		} else {
			return waitAllAction(getint(elm, "canskip"));
		}
		return 0;
	}

	// --------------------------------------------------
	// �ϐ��n
	
	function tag_eval(elm) {
		if ("exp" in elm) {
			eval(elm.exp);
		}
		return 0;
	}

	function tag_clearsyslocal() {
		::sf = {};
		return 0;
	}

	function tag_clearlocal() {
		::f = {};
		return 0;
	}
	
	function tag_clearplaytime() {
		//return kag.tagHandlers.clearplaytime(...);
		return 0;
	}

	// --------------------------------------------------------------
	// ���b�Z�[�W������
	// --------------------------------------------------------------

	/**
	 * ���b�Z�[�W���̏�����
	 */
	function tag_msgwin(elm) {
		msgsize(getval(elm, "left", msgX), getval(elm, "top", msgY),
				getval(elm, "width", msgWidth), getval(elm, "height", msgHeight));
		return 0;
	}

	/**
	 * ���b�Z�[�W���̕\��
	 */
	function tag_msgon(elm) {
		if (!isJump()) {
			_msgon();
		}
		return 0;
	}

	/**
	 * ���b�Z�[�W���̏���
	 */
	function tag_msgoff(elm) {
		if (!isJump()) {
			_msgoff();
		}
		return 0;
	}
	
	// ------------------------------------------------------------------
	// ����R�}���h
	// ------------------------------------------------------------------

	// �g�����W�V�������[�h
	transMode = 0;

	/**
	 * ������
	 */
	function tag_init(elm) {
		transMode = 0;
		env.initEnv(elm);
		clearTrash();
		return 0;
	}
	
    /**
     * �S�̃g�����W�V�����J�n
     */
	function tag_begintrans(elm) {
		backup();
		transMode = 1;
        return 0;
    }

    /**
     * �S�̃g�����W�V�����I��
     */
	function tag_endtrans(elm) {
		// �܂������i�g�����W�V������������ true ���Ə������o�^����Ȃ��̂�)
		transMode = 0;
		local trans = null;
		foreach (cmd, param in elm) {
			if (cmd == "tagname") {
				// ignore
			} else if (cmd == "trans") {
				local tr = getTrans(param, elm);
				if (tr != null) {
					trans = tr;
				}
			} else if (cmd == "fade") {
				local time = param.tointeger();
				trans = {
					method = "crossfade",
					children = true,
					time = time > 1 ? time : getFadeValue()
					};
			} else {
				local tr = getTrans(cmd, elm);
				if (tr != null) {
					trans = tr;
				}
			}
		}
		if (trans != null && getval(trans, "msgoff")) {
			addTag("msgoff");
		}
		beginTransition(trans);
		return 0;
    }

	/**
	 * ���C���g�����W�V�����J�n
	 */
	function tag_beginlt(elm) {
		transMode = 2;
		return beginLayerTrans(elm);
	}

	/**
	 * ���C���g�����W�V�����I��
	 */
	function tag_endlt() {
		transMode = 0;
		return endLayerTrans(elm);
	}

	// ------------------------------------------------------------------
	
	// ����Đ�����{�C�X���
	// �L�[:�L�����N�^��
	// �l:�{�C�X
	nextvoices = {};
	nextvoicecount = 0;

	/**
	 * ���񓯎��ɖ炷�{�C�X�̒ǉ�
	 * @param name �L������
	 * @param voice  �{�C�X�w��
	 * @param voicen ��f�t�H���g���{�C�X�w��
	 */
	function entryNextVoice(name, voice, voicen) {
		if (name != null && voice != null) {
			nextvoices[name] = {voice=voice, voicen=voicen};
			nextvoicecount++;
		}
    }

	/**
	 * ����炷�{�C�X�̏����N���A
	 */
	function clearNextVoice() {
		nextvoices = {};
		nextvoicecount = 0;
	}

	/**
	 * �����{�C�X��炷
	 * @return �Œ��̃{�C�X�Đ�����
     */
	function playVoices(voices) {
		local ret = null;
		if (voices != null) {
			foreach (name, info in voices) {
				local r;
				// �f�t�H���g���̏ꍇ�̏���
				if (isDefaultName() && info.voicen != null) {
					r = playVoice(name, info.voicen);
				} else {
					r = playVoice(name, info.voice);
				}
				if (ret == null || r != null && r > ret) {
					ret = r;
				}
			}
		}
		return ret;
    }
	
	/**
	 * �{�C�X�Đ����s
	 */
	function playNextVoice() {
		if (nextvoicecount > 0) {
			local ret = playVoices(nextvoices);
			clearNextVoice();
			return ret;
		}
	}
	
	// ------------------------

	/**
	 * �{�C�X�ǉ�
	 */
	function tag_entryvoice(elm) {
		entryNextVoice(elm.name, elm.voice, elm.voicen);
		return 0;
	}

	/**
	 * �{�C�X�Đ�
	 */
	function tag_playvoice(elm) {
		playNextVoice();
		return 0;
	}

	/**
	 * �{�C�X�҂�
	 */
	function tag_waitvoice(elm) {
		if ("name" in elm) {
			return waitVoice(elm.name);
		}
		return 0;
	}
	
	/**
	 * ���O�\��
	 */
	function tag_dispname(elm) {

		if (voicecut && !voicecutpage) {
			stopAllVoice();
		}

		local dispName = null;
		local faceName = null;
		
		//dm("���O�\���n���h��");
		if (elm == null || !("name" in elm)  || elm.name == "") {
			
			env.currentNameTarget = null;

		} else {

			local name = elm.name;
			local disp = elm.disp;

			local ch = env.getCharacter(name);
			env.currentNameTarget = ch;

			if (elm.voice != null) {
				entryNextVoice(name, elm.voice, elm.voicen);
			}

			// ���O�̌���
			if (disp != null && disp != "") {
				dispName = disp;
			} else {
				if (ch != null && "nameAlias" in ch.init) {
					dispName = eval(ch.init.nameAlias);
					if (dispName == null) {
						dispName = name;
					}
				} else {
					dispName = name;
				}
			}
			
			// ���O���H���� XXX
			//if (typeof global.dispNameFilter !== 'undefined') {
			//dispName = global.dispNameFilter(dispName);
			//}
			
			// �\���
			if (env.faceLevelName != null) {
				if (ch != null) {
					if (ch.isShowFace()) {
						faceName = ch.getPoseImage(env.faceLevelName);
					}
				} else {
					if ("nameFaces" in env.envinfo) {
						faceName = env.envinfo.nameFaces[env.envinfo.dispNameFace ? dispName : name];
					}
				}
				// �w�肪���܂�Ȃ������ꍇ
				if (faceName == null) {
					faceName = env.envinfo.clearFace;
				}
			}
		}

		// ���O�\��
		drawName(dispName, faceName, nextvoices);

		// �{�C�X�Đ�
		if (nextvoicecount > 0) {
			local ret = playNextVoice();
			if (ret > 0) {
				addAutoWait(ret);
			}
		}
		return 0;
	}

	// ---------------------------------------------
	
	/**
	 * �s�҂��I����ɌĂяo����鏈��
	 */
	function tag_afterline(elm) {
		if (voicecut && voicecutpage) {
			stopAllVoice();
		}
		//clearHistory(true);
		return 0;
	}

    /**
     * �y�[�W������ɌĂяo����鏈��
     */
	function tag_afterpage(elm) {
		if (voicecut && voicecutpage) {
			stopAllVoice();
		}
		env.currentNameTarget = null;
		//clearHistory();
		stopAllActions(false);
		return 0;
    }

	function tag_stopallvoice(elm) {
		stopAllVoice();
	}

	// ---------------------------------------------------
	// �I�����p����
	// ---------------------------------------------------

	function addSelect(elm) {
		if (!("selectInfos" in f)) {
			f.selectInfos = [];
		}
		if (getint(f, "selectDoneFlag")) {
			f.selectInfos.resize(0);
			f.selDoneTarget  <- null;
			f.selectDoneFlag <- false;
		}
		f.selectInfos.append(clone elm);
		return 0;
	}

    /**
     * �I�����̕\��
     */
	function doSelect(elm) {
		f.selectDoneFlag <- true;
		if ("target" in elm) {
			f.selDoneTarget <- elm.target;
		}
		f.selTarget <- null;
		// �I����������������
		addTag("er");
		addTag("selend");
		// f.selectInfo �̓��e�őI������\�����ďI�����܂�
		// kag.showSelect();
		// return kag.waitTrigger(%[name:"selectdone"]);
		return 0;
	}

	/**
	 * �I�����̏I��
	 */
	function endSelect(elm) {
		//�I��������
		//kag.hideSelect();
		if (f.selTarget != "") {
			goToLabel(f.selTarget);
			return 0;
		} else {
			return -1;
		}
	}

	/**
	 * �I���������̊���
	 */
	function doneSelect(elm) {
		if (f.selDoneTarget != "") {
			goToLabel(f.selDoneTarget);
			return 0;
		} else {
			return -1;
        }
	}
	
	// ------------------------------------------------------------------

	function getHandlers()
	{
		return { // �����z��I�u�W�F�N�g

	//----------------------------------------- �^�O�n���h���Q(�e�L�X�g�\���n) --

			ch = tag_ch,
			r = tag_r,
			ruby = tag_ruby,
			font = tag_font,
			deffont = tag_deffont,
			resetfont = tag_resetfont,
			style = tag_style,
			defstyle = tag_defstyle,
			l = tag_l,
			p = tag_p,
			er = tag_er,
			indent = tag_indent,
			endindent = tag_endindent,
			delay = tag_delay,
			
			cancelskip = tag_cancelskip,
			resetwait = tag_resetwait,
			wait = tag_wait,
			waittrig = tag_waittrig,
			beginskip = tag_beginskip,
			endskip = tag_endskip,
			
			backlay = tag_backlay,
			forelay = tag_forelay,
			trans = tag_trans,
			wt = tag_wt,
			action = tag_action,
			stopaction = tag_stopaction,
			wact = tag_wact,
			
			eval = tag_eval,
			clearsyslocal = tag_clearsyslocal,
			clearlocal = tag_clearlocal,
			clearplaytime = tag_clearplaytime,

			//----------------------------------------- �^�O�n���h���Q(�I��������) --
			
			msgwin = tag_msgwin,
			msgoff = tag_msgoff,
			msgon  = tag_msgon,
			
			//----------------------------------------- �^�O�n���h���Q(�I��������) --
			
			seladd = addSelect,
			select = doSelect,
			seldone = doneSelect,
			selend = endSelect,

			//----------------------------------------- �^�O�n���h���Q(�V�i���I������) --
			
			["goto"] = function(elm) {
				if ("target" in elm) {
					goToLabel(elm.target);
				}
				return 0;
			},
			
			//----------------------------------------- �^�O�n���h���Q(���V�i���I�w��) --
			
			next = function(elm) {
				if ("storage" in elm) {
					nextScene = elm.storage;
				}
				return 0;
			},

			exit = function(elm) {
				cur = lines.len();
				return 0;
			},

			//----------------------------------------------- ���n�@�\ --

			env      = env.tagfunc,
			allchar  = env.allchar,
			alllayer = env.alllayer,
			allse    = env.allse,
			
			init       = tag_init,
			begintrans = tag_begintrans,
			endtrans   = tag_endtrans,
			beginlt    = tag_beginlt,
			endlt      = tag_endlt,
			
			newlay     = env.newLayer,
			newlayer   = env.newLayer,
			dellay     = env.delLayer,
			dellayer   = env.delLayer,
			newchar    = env.newCharacter,
			delchar    = env.delCharacter,
			
			entryvoice    = tag_entryvoice,
			playvoice     = tag_playvoice,
			waitvoice     = tag_waitvoice,
			dispname      = tag_dispname,
			afterpage     = tag_afterpage,
			afterline     = tag_afterline,
			stopallvoice  = tag_stopallvoice,
		};
	}

	// ----------------------------------------------
	// �V�X�e���p�t�@���N�V�����Q
	// ----------------------------------------------

	// �����Đ�
	autoMode   = false;
	
	// �X�L�b�v���[�h
	// 0: �ʏ�
	// 1: �X�L�b�v��
	// 2: �����X�L�b�v
	// 3: �Ƃ΂�������
	skipMode = 0;
	prevSkipMode = null;

	/**
	 * �X�L�b�v�����ǂ���
	 */
	function isSkip() {
		return skipMode != 0;
	}
	
	// �����҂����J�n
	function startAuto() {
		local lastLength;
		if (Talkingnow(0) && talkVoiceLength > 0) {
			lastLength  = talkVoiceLength;
		} else {
			lastLength  = 0;
		}
		lastLength += getAutoWait();
		addWait({
			click = function() { stopAllVoice(); msgWait = false; },
			timeout = function() { stopAllVoice(); msgWait = false; },
		}, lastLength);
	}

	// �����҂��̉�������
	function stopAuto() {
		if ("timeout" in waitUntil) {
			delete waitUntil.timeout;
		}
	}

	
	/**
	 * �I�[�g���[�h�̉���
	 */
	function cancelAuto() {
		if (autoMode) {
			autoMode = false;
			// ������b�҂����Ȃ炻�������
			if (msgWait) {
				stopAuto();
			}
		}
	}

	/**
	 * �������[�h�̕ύX
	 */
	function onAuto() {
		if (msgWinShow) {
			if (autoMode) {
				cancelAuto();
			} else {
				autoMode = true;
				// ������b�҂��Ȃ炻�̂܂܎����҂��ɕύX
				if (msgWait) {
					startAuto();
				}
			}
		}
	}
	
	// ---------------------------------------------------
	
	/**
	 * �X�L�b�v���[�h�̉���
	 */
	function cancelSkip() {
		if (skipMode == 1 || skipMode == 2) {
			//print("�N���b�N�ŃX�L�b�v���f\n");
			skipMode = 0;
		} else if (prevSkipMode != null) {
			skipMode = 3;
		}
	}

	/**
	 * �X�L�b�v�J�n
	 */
	function onSkip() {
		if (msgWinShow) {
			if (skipMode == 1) {
				//print("�X�L�b�v����\n");
				skipMode = 0;
			} else if (skipMode == 0) {
				if (prevSkipMode != null) {
					if (prevSkipMode == 0) {
						if (isAllSkip() || getReaded(scene) >= currentLine) {
							prevSkipMode = 1;
							//stopVoice();
						}
					}
					skipMode = 3;
				} else {
					onClick();
					if (isAllSkip() || getReaded(scene) >= currentLine) {
						//print("�X�L�b�v�J�n\n");
						skipMode = 1;
						//stopVoice();
					} else {
						//print("���ǂŃX�L�b�v�J�n�ł���\n");
					}
				}
			}
		}
	}

	/**
	 * �����X�L�b�vON
	 */
	function onSkipOn() {
		if (msgWinShow) {
			if (skipMode < 2) {
				//print("�����X�L�b�vON\n");
				cancelAuto();
				onClick();
				skipMode = 2;
				//stopVoice();
			}
		}
	}

	/**
	 * �����X�L�b�vOFF
	 */
	function onSkipOff(param) {
		if (skipMode == 2) {
			//print("�����X�L�b�vOFF\n");
			skipMode = 0;
		}
	}
	
	// ---------------------------------------------------

	msgWinShow = true;
	
	/**
	 * ���b�Z�[�W���������[�h�̉���
	 */
	function cancelMsg() {
		if (!msgWinShow) {
			msgshow();
			msgWinShow = true;
			return true;
		}
		return false;
	}
	
	/**
	 * ���b�Z�[�W���� ON/OFF ����
	 */
	function onMeswin(param) {
		cancelAuto();
		cancelSkip();
		// �\�����Ă�ꍇ�̂ݗL��
		if (msgWinState) {
			if (msgWinShow) {
				// ������������ ON �w�߂�����܂Œ�~����
				msghide();
				msgWinShow = false;
			} else {
				// ����\�����đ҂��̉���
				msgshow();
				msgWinShow = true;
			}
		}
	}

	// ---------------------------------------------------
	
	/**
	 * �N���b�N���ꂽ
	 */
	function onClick() {
		// ��Ԃɉ������������s�� 
		if (cancelMsg()) { return; }
		cancelAuto();
		cancelSkip();
		//if (cancelTalk()) { return; }
		clickFunc();
		trigger("click");
	}
	
	/**
	 * �L�����Z���N���b�N���ꂽ
	 */
	function onCancel() {
		// ��Ԃɉ������������s�� 
		if (cancelMsg()) { return; }
		cancelAuto();
		cancelSkip();
		trigger("cancel");
	}

	// ------------------------------------------------------
	
	/**
	 * �N���b�N�҂�
	 */
	function waitClick() {
		print("�N���b�N�҂�\n");
		if (!isJump()) {
			if (isSkip()) {
				return;
			}
			addWait({
				click = function() {},
			}, null);
			suspend(WAIT);
		}
	}
	
	// ------------------------------------------------------
	// �O���C�x���g
	// ------------------------------------------------------

	function onKeyPress(key) {
		if (key == KEY_ENTER) {
			onClick();
		} else if (key == KEY_ESCAPE) {
			onCancel();
		}
	}
	
	/**
	 * SE �p���̃N���A
	 */
	function onSEStop(id) {
		::GamePlayer.onSEStop(id);
		env.onSeStop(id);
	}

	/**
	 * �\���R�s�[�����s���ꂽ�ꍇ�̏���
	 */
	function onCopyLayer(toback, backlay) {
		env.onCopyLayer(toback, backlay);
		if (backlay && !toback) {
			clearTrash();
		}
	}

	/**
	 * �S���Z�b�g���̏���
	 */
	function onResetAll() {
		env.resetEnv();
		env.syncAll();
	}
};
