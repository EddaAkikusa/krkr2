/**
 * ��{�v���C���[
 * �Q�[�������̊�{�@�\��񋟂���v���C���[
 * �E�X�L�b�v����
 * �E��ʐ؂�ւ�����
 * �E���b�Z�[�W�����䏈��
 * �E�I��������
 * �E�{�C�X�Đ�
 * �ESE�Đ�
 * �EBGM�Đ�
 */
class GamePlayer extends Player {

	voiceInfos = null; // �{�C�X�Đ��p���

	/**
	 * �R���X�g���N�^
	 */
	constructor() {
		::Player.constructor();
		voiceInfos = dofile("voices.nut");
	}

	/**
	 * ��Ԃ̃N���A
	 */
	function clear() {
		::Player.clear();
		hideTransLayer();

	}

	/**
	 * �X�L�b�v����
	 */
	function isSkip() {
		//return kag.skipMode != SKIP_FORCE && (kag.skipMode || kag.noeffect);
		return false;
	}

	// ------------------------------------------------------------------
	// ��ʐ؂�ւ��֘A
	// ------------------------------------------------------------------

	// XXXX �܂����@�\����������ĂȂ����
	// �g�����W�V�����̑��
	
	/**
	 * ����ʗp����
	 */
	function backup() {
		//kag.fore.base.stopTransition();
		//kag.backupLayer(EMPTY, true);
	}

	// ----------------------------------------------------
	// ���C���ɂ���ʐ؂�ւ������p
	// ----------------------------------------------------
	
	transLayer = null;
	transShowing = false;

	/**
	 * ��ʐ؂�ւ������p���C�����B��
	 */
	function hideTransLayer() {
		if (transLayer != null) {
			transLayer.stop();
		}
		transShowing = false;
	}
	
	/**
	 * ���C����ʐ؂�ւ��̊J�n
	 */
	function beginLayerTrans(elm) {

		local time    = getint(elm, "time", 1000);
		local type    = getval(elm, "type", "wipeltor");
		local color   = getint(elm, "color",  0);
		local canskip = getval(elm, "canskip", true);

		if (transLayer == null) {
			transLayer = TransLayer();
		}
		hideTransLayer();
		
		local waitParam = {};
		if (canskip) {
			if (isSkip()) {
				time = 0;
			}
			waitParam.click <- function() { hideTransLayer(); };
		}
		transLayer.start(elm, type, time, color);
		transShowing = true;
		
		time = transLayer.time;
		
		local waitTime;
		if ("wait" in elm) {
			waitTime = time + elm.wait;
			waitParam.timeout <- function() {};
			waitWithTimeout(waitParam, waitTime);
		} else {
			waitParam.layertrans <- function() {};
			if (time > 0) {
				waitWithTimeout(waitParam, waitTime);
			}
		}
	}
	
	/**
	 * ���C����ʐ؂�ւ��̏I��
	 */
	function endLayerTrans(elm) {
		if (!transShowing) {
			return;
		}

		local time    = getint(elm, "time");
		local type    = getval(elm, "type");
		local canskip = getval(elm, "canskip", true);

		local waitParam = {};
		if (canskip) {
			if (isSkip()) {
				hideTransLayer();
				return;
			}
			waitParam.click <- function() { hideTransLayer(); };
		}
		waitParam.layertrans <- function() { hideTransLayer(); };
		transLayer.start(elm, type, time, null);
		time = transLayer.time;
		if (time > 0) {
			waitWithTimeout(waitParam, null);
		}
	}

	// ----------------------------------------------------
	// �X�N���[���ɂ���ʃt�F�[�h�����p
	// ----------------------------------------------------
	
	screenLayer = null; // �X�N���[���\���p���C��
	lastImageId = null;	// �O��w��̉摜
	
	/**
	 * �X�N���[���摜��\��
	 * @image �摜
	 * @opacity �s�����x
	 */
	function screenShow(image, opacity)	{
		if (screenLayer == null) {
			screenLayer = ScreenLayer();
			screenLayer.setPos(0,0);
			screenLayer.absolute = 600000; // XXX �������K�����H
			if (image != null) {
				lastImageId = image;
			}
			if (lastImageId == null) {
				lastImageId == "black";
			}
			screenLayer.loadImage(lastImageId);
		} else {
			if (image != null && lastImageId != image) {
				lastImageId = image;
				screenLayer.loadImage(lastImageId);
			}
		}
		screenLayer.opacity = opacity;
		screenLayer.visible = true;
	}
	
	/**
	 * �X�N���[�������̒�~ 
	 */
	function screenStop() {
		if (screenLayer != null) {
			screenLayer.stop();
		}
	}

	/**
	 * �X�N���[�������I���҂�
	 */
	function waitScreen(canskip) {
		if (screenLayer == null || screenLayer.actionTime == null) {
			return;
		}
		if (canskip) {
			if (isSkip()) {
				screenStop();
				return;
			} 
			addWait({
				click = function() { screenStop(); },
				screen = function() {}
			}, null);
		} else {
			addWait({
				screen = function() {}
			}, null);
		}
		suspend(WAIT);
	}
	
	/**
	 * �X�N���[���摜�t�F�[�h�A�E�g
	 */
	function fadeOut(image, time) {
		screenShow(image, 0);
		screenLayer.start(1, time, false);
		waitScreen(true);
	}

	function blackOut(time) {
		fadeOut("black", time);
	}

	function whiteOut(time) {
		fadeOut("white", time);
	}

	/**
	 * �X�N���[���摜�t�F�[�h�C��
	 */
	function fadeIn(time) {
		screenShow(null, 100);
		screenLayer.start(2, time, false);
		waitScreen(true);
	}

	// ------------------------------------------------------------
	
	// ���X�^�C���A�N�V�����p�p�����[�^
	actionParam = {
		start = true,
		startx = true,
		starty = true,
		x = true,
		y = true,
		toptime = true,
		vibration = true,
		waittime = true,
		cycle = true,
		distance = true,
		falltime = true,
		zoom = true,
		angvel = true,
		angle = true,
		showtime = true,
		hidetime = true,
		intime = true,
		outtime = true,
		opacity = true,
	};

	/**
	 * �A�N�V�������̃R�s�[
	 * @param act  ���A�N�V�������
	 * @param time ���ԏ㏑���w��
	 * @param elm  �^�O�ɂ��㏑���p�p�����[�^���
	 * @return �V�X�e���o�^�p�A�N�V�������
	 */
	function copyActionInfo(act, time, elm) {
		local action = {};
		if (act != null) {
			local names = [];
			names.assign(act);
			if ("module" in act) {
				//dm("module �w��A�N�V����:" + act.module);
				local module = eval(act.module);
				if (module != null) {
					local e = clone value;
					if (time != null) {
						e.time <- time;
					}
					// �p�����[�^�̃R�s�[
					if (elm != null) {
						// �A�N�V�����p�����[�^���R�s�[
						foreach (name, value in elm) {
							if (name in actionParam) {
								e[name] <- value;
							}
						};
						if ("time" in elm) {
							e.time <- elm.time;
						}
						if ("delay" in elm) {
							e.delay <- elm.delay;
						}
					}
					// �V�X�^�C���ɒu������
					action = module(e);
				}
			} else {
				// �V�X�^�C��
				foreach (name, value in act) {
					if (typeof value == "table") {
						local e = clone value;
						if (time != null) {
							e.time <- time;
						}
						// �p�����[�^�̃R�s�[
						if (elm != null) {
							if ("time" in elm) {
								e.time <- elm.time;
							}
							if ("delay" in elm) {
								e.delay <- elm.delay;
							}
						}
						action[name] <- e;
					} else {
						// �����łȂ��ꍇ�͂��̂܂�
						action[name] = value;
					}
				}
			}
		}
		if (elm != null && "nowait" in elm) {
			action.nowait <- elm.nowait;
		}
		return action;
	}

	/**
	 * �^�O����A�N�V�������𒊏o
	 * @param name �A�N�V������
	 * @param elm  �^�O�ɂ��㏑���p�p�����[�^���
	 * @return �V�X�e���o�^�p�A�N�V�������
	 */
	function getActionInfo(name, elm) {
		if (elm != null && name.slice(0,5) == "Layer") {
			local module = eval(name);
			if (module != null) {
				local action = module(elm);
				if ("nowait" in elm) {
					action.nowait <- elm.nowait;
				}
				return action;
			}
		}
	}
	
	/**
	 * �X�V���A�N�V�����̎��s
	 */
	function beginTransActionUpdate(trans, layer) {
		if ("showaction" in trans) {
			local action = copyActionInfo(trans.showaction, getval(trans,"time"));
			addTag("action", { target=layer, action=action});
		}
		if ("hideaction" in trans) {
			local action = copyActionInfo(trans.hideaction, getval(trans, "time"));
			//addTag("action", { target=layer, target:layer.comp, action:action]);
		}
	}

	/**
	 * �\��/�����A�N�V�����̎��s
	 */
	function beginTransAction(trans, layer, hide, nosync) {
		// �A�N�V�������s
		local action = copyActionInfo(getval(trans, hide ? "hideaction" : "showaction"), getval(trans, "time"));
		action["visvalue"] <- { handler="MoveAction", value= hide ? 0 : 100, time=trans.time };
		addTag("action", { target=layer, action=action});
		if (!nosync && !getval(trans, "nosync")) {
			if ("transwait" in trans) {
				// ���ԑ҂�
				local time = trans.time.tointeger() + trans.transwait.tointeger();
				addTag("wait", {time=time, action=true});
			} else {
				// �A�N�V�����҂�
				addTag("wact", {target=layer});
			}
		}
	}

	// ------------------------------------------------------------------
	// �����C������
	// ------------------------------------------------------------------
	
	// ���ݑO�ʂɂ��郌�C���ꗗ
	// ���ׂĎ�Q�Ƃŕێ����邱��
	forelayers = [];

	/**
	 * �w�肵�����C�����w�肵�����x���Ɉړ�����
	 */
	function toLevel(layer, newlevel) {

		//dm("���x���w��:" + newlevel + ":" + layer.level);
		newlevel = newlevel.tointeger();

		if (layer.level > 0) {
			if (layer.level == newlevel) {
				//dm("���ꃌ�x���Ȃ̂ŏ������Ȃ�");
				return;
			}
			local index = layer.absolute;
			// ���Ƃ��Ƃ������x���̒���
			for (local i = 0; i<forelayers.len(); i++) {
				local target = forelayers[i];
				if (target != layer && target.level == layer.level) {
					if (target.absolute > index) {
						target.absolute -= 100;
					}
				}
			}
		}
		// �V�K���x���̈�ԏ�ɂ����Ă���
		local newindex = (newlevel + 1) * 100000;
		for (local i = 0; i<forelayers.len(); i++) {
			local target = forelayers[i];
			if (target != layer && target.level == newlevel && target.absolute > newindex) {
				newindex = target.absolute;
			}
		}
		newindex += 100;
		//dm("���x���ݒ�:" + newlevel + ":" + newindex);
		layer.absolute = newindex;
		layer.level    = newlevel;
    }
	
	/**
	 * �������x���̃��C���̒��ōőO��Ɉړ�
	 */
	function toFront(layer) {
		local level = layer.level;
		local index = layer.absolute;
		local maxindex = index;
		for (local i = 0; i<forelayers.len(); i++) {
			local target = forelayers[i];
			if (target != layer && target.level == level) {
				if (target.absolute > maxindex) {
					maxindex = target.absolute;
				}
				if (target.absolute > index) {
					target.absolute -= 100;
				}
			}
		}
		layer.absolute = maxindex;
	}

	/*
	 * ���C�����Ō��Ɉړ�
	 */
	function toBack(layer) {
		local level = layer.level;
		local index = layer.absolute;
		local minindex = index;
		for(local i = 0; i<forelayers.len(); i++) {
			local target = forelayers[i];
			if (target != layer && target.level == level) {
				if (target.absolute < minindex) {
					minindex = target.absolute;
				}
				if (target.absolute < index) {
					target.absolute += 100;
				}
			}
		}
		layer.absolute = minindex;
    }

	/**
	 * �\�ɂ��郌�C�����H
	 */
	function isForeLayer(layer) {
		///return layer.parent === kag.fore.base;
		return true;
	}

	/**
	 * �\�Ɉړ�������
	 */
	function toForeLayer(layer) {
		local absolute = layer.absolute;
		//layer.parent   = kag.fore.base; �\�Ɉړ�
		layer.absolute = absolute;
	}

	/**
	 * ���C���擾����
	 * @param fore
	 * @param origLayer �p�������C��
	 */
	function getNewLayer(owner, isfore, origLayer) {
		local newlayer = owner.createLayer(isfore);
		newlayer.level = -1;
		forelayers.add(newlayer);
		if (origLayer != null) {
			newlayer.assign(origLayer);
			//newlayer.level    = origLayer.level;
			//newlayer.absolute = origLayer.absolute;
			trashLayer(origLayer);
		}
		return newlayer;
	}
	
	// �p���Ώۃ��C���ꗗ
	trashlayers = [];

	/**
	 * ���C���p������
	 * @param layer
	 */
	function trashLayer(layer) {
		if (layer != null) {
			local newlayers = [];
			for (local i=0;i<forelayers.len();i++) {
				if (forelayers[i] != null && forelayers[i] != layer) {
					newlayers.append(forelayers[i]);
				}
			}
			forelayers = newlayers;
			for (local i=0;i<trashlayers.len();i++) {
				if (trashlayers[i] == layer) {
					return;
				}
			}
			trashlayers.append(layer);
		}
		return null;
	}

	/**
	 * ���C���j�����s
	 */
	function clearTrash() {
		foreach (name,value in trashlayers) {
			delete trashlayers[name];
		}
	}
	
	// -------------------------------------------------------------------
	// �{�C�X�����n
	// -------------------------------------------------------------------

	voicecut = true;     // �{�C�X���J�b�g���邩
	voicecutpage = true; // �{�C�X�J�b�g���y�[�W�����̌�ɂ��邩�ǂ���
	bgmdown = false;     // �{�C�X����BGM�������邩
	voiceenable = true;  // �{�C�X���L����

	/**
	 * �{�C�X���L�����ǂ���
	 * @param �R���t�B�O����
	 * @return �L���Ȃ� true
	 */
	function getVoiceOn(configName) {
		return true;
	}

	/**
	 * �{�C�X�̉��ʏ�Ԃ̎擾
	 * @param �R���t�B�O����
	 * @return ����
	 */
	function getVoiceVolume(configName) {
		return 100;
	}

	// ���O��`���f�t�H���g�����H
	function isDefaultName() {
		return true;
		//return kag.flags.name === void || (kag.flags.name == kag.defaultName && kag.flags.family == kag.defaultFamily);
	}

	// ----------------------------------------------------------------------------

	// ��b���{�C�X����
	talkVoiceLength = null;
	
	/**
	 * �{�C�X�Đ�
	 * @param name �L������
	 * @param voicefile �Đ��t�@�C��
	 */
	function playVoice(name, voicefile) {
		if (isSkip() || !voiceenable) {
			return;
		}

		// �{�C�X���擾
		local configName = "etc";
		if (name in voiceInfo) {
			local info = voiceInfos[name];
			if ("config" in info) {
				configName = info.config;
			}
		}
		if (!getVoiceOn(configName)) {
			return;
		}
		
		// �{�����[������
		// volume = getVoiceVolume(configName);

		// �{�C�X�Đ�����
		Talk(0, voicefile);
		talkVoiceLength = TalkLength(0);

		// ���Ԃ�Ԃ�
		return talkVoiceLength;
	}

	/**
	 * �{�C�X��~
	 * @param name �L������
	 */
	function stopVoice(name) {
		if (talkVoiceLength != null) {
			TalkStop(0);
			talkVoiceLength = null;
		}
	}

	/**
	 * �S�{�C�X��~
	 */
	function stopAllVoice() {
		// XXX
		stopVoice("");
	}
	
	/**
	 * �{�C�X�܂�
	 * @param name �L������
	 * @param canSkip �X�L�b�v�ł���
	 */
	function waitVoice(name, canSkip) {
		if (canSkip) {
			if (isSkip()) {
				stopVoice(name);
				return 0;
			}
			if (Talkingnow(0) && talkVoiceLength > 0) {
				waitWithTimeout({
					click = function(name) { stopVoice(name); },
					click_arg = name,
					timeout = function() {},
				}, talkVoiceLength);
			}
		} else {
			if (Talkingnow(0) && talkVoiceLength > 0) {
				waitWithTimeout({
					timeout = function() {},
				}, talkVoiceLength);
			}
		}
	}
	
	// �����҂����Ԃ̒ǉ�
	function addAutoWait(wait) {
		//kag.addAutoWait(wait);
	}

	// ------------------------------------------------------------
	// SE �n�@�\
	// ------------------------------------------------------------

	// �X�L�b�v���� se ���Đ����Ȃ�
	nosewhenskip = true;

	// se �ː�
	senum = 2;

	// �t���[�����ɕϊ�
	function getTime(time) {
		if (isSkip() || time == null) {
			time = 0;
		}
		return time.tointeger() / 16;
	}

	// �K��̃{�����[���l�ɕϊ�
	function getVolume(volume) {
		if (volume == null) {
			volume = 100;
		}
		return 0x3fff * volume.tointeger() / 100;
	}

	function playSE(id, time, play) {
		//kag.se[id].fadeIn({ storage:play.storage, loop:play.loop, time:time, start:play.start});
		MusicStop(id, 0);
		MusicPlay(id, getSoundId(play.storage), getVolume(play.volume), getTime(time))
	}

	function fadeSE(id, time, volume) {
		MusicFade(id, getVolume(volume), getTime(time));
	}
	
	function stopSE(id, time) {
		MusicStop(id, getTime(time));
	}

	function waitSEFade(id, waittime, canskip) {
		return 0;
	}

	function waitSEStop(id, waittime, canskip) {
		if (MusicPlayingNow(id)) {
			if (canskip) {
				// �X�L�b�v�ł���ꍇ
				if (isSkip()) {
					// �X�L�b�v���̏ꍇ
					stopSE(id,0);
					return 0;
				}
				waitWithTimeout({
					click = function (id) {	stopSE(id,0);},
					["click_arg"] = id, // �n���h���ւ̈���
					["sestop"+id] = function (id) {	},
					["sestop"+id+"_arg"] = id // �n���h���ւ̈���
					}, timeout);
			} else {
				// �X�L�b�v�ł��Ȃ��ꍇ
				waitWithTimeout({
					["sestop"+id] = function (id) {},
					["sestop"+id+"_arg"] = id // �n���h���ւ̈���
					}, timeout);
			}
			return -2;
		}
		return 0;
	}
	
	// ------------------------------------------------------------
	// BGM �n�@�\
	// ------------------------------------------------------------

	// BGM �{�����[���̎�������n
	function setBGMVolDown(down) {
		if (down) {
			if (bgmdown && !isSkip()) {
				//kag.bgm.voldown = true;
			}
		} else {
			//if (kag.bgm.voldown) {
			//	kag.bgm.voldown = false;
			//}
		}
	}
	
	function playBGM(time, play) {
		//kag.bgm.fadeIn(%[ storage:play.storage, loop:play.loop, time:time, start:play.start]);
		local name = getSoundId(play.storage);
		if (!MusicPlayingNow(0) || MusicCurrent(0) != name) {
			MusicPlay(0, name, getVolume(play.volume), getTime(time));
		}
	}

	function pauseBGM(time) {
		// �|�[�Y���� XXX
	}
	
	function resumeBGM() {
		// �|�[�Y���A���� XXX
	}

	function fadeBGM(time, volume) {
		MusicFade(0, getVolume(volume), getTime(time));
	}
	
	function stopBGM(time) {
		MusicStop(0, getTime(time));
	}

	function waitBGMFade(waittime, canskip) {
		//return kag.waitBGMFade(waitFade);
		return 0;
	}

	function waitBGMStop(waittime, canskip) {
		if (MusicPlayingNow(0)) {
			if (canskip) {
				if (isSkip()) {
					stopBGM(0);
					return 0;
				}
				waitWithTimeout({
					click = function() { stopBGM(0); },
					bgmstop = function() {},
					timeout = function() { stopBGM(0); },
				}, waittime);
			} else {
				waitWithTimeout({
					bgmstop = function() {},
					timeout = function() { stopBGM();},
				}, waittime);
			}
			return -2;
		}
		return 0;
	}

	// ----------------------------------------------------
	// ���b�Z�[�W��
	// ----------------------------------------------------
	
	msg = null;
	msgWinState = false;
	msgCurrent = null;
	msgText = "";
	msgAll = "";
	msgName = null;
	msgCursor = false;
	msgX = 20;
	msgY = 24;
	msgWidth = 600;
	msgHeight = 400;

	/**
	 * ���b�Z�[�W���T�C�Y�ύX
	 */
	function msgsize(x, y, w, h) {
		msghide();
		msgX = x;
		msgY = y;
		msgWidth = w;
		msgHeight = h;
	}
	
	/**
	 * ���b�Z�[�W������
	 */
	function msgshow() {
		if (msg == null) {
			msg = MessageWindow(msgX, msgY, msgWidth, msgHeight);
		}
		if (msgWinState) {
			if (msgText != null && msgText != "") {
				msg.write(msgText);
			} else {
				msg.clear();
			}
		if (msgName != null) {
			msg.showNamePlate(msgName);
		} else {
			msg.showNamePlate(0);
		}
			msg.showCursor(msgCursor);
			msg.show(true);
		} else {
			msg.show(false);
		}
	}

	/**
	 * ���b�Z�[�W������
	 */
	function msghide() {
		msg = null;
	}
	
	/**
	 * ���b�Z�[�W���\��
	 */
	function _msgon() {
		msgWinState = true;
		msgshow();
	}
	
	/**
	 * ���b�Z�[�W������
	 */
	function _msgoff() {
		print("msgoff\n");
		msgWinState = false;
		msgCurrent = null;
		msgText = "";
		msgAll = "";
		msgName = null;
		msgCursor = false;
		if (msg != null) {
			msg.clear();
			msg.showCursor(false);
			msg.showNamePlate(0);
			msg.show(msgWinState);
		}
	}
	
	/**
	 * ���b�Z�[�W�\��
	 */
	function msgwrite(text) {
		_msgon();
		if (text != null) {
			msgText = msgAll + text;
			msg.write(msgText);
		} else {
			msg.clear();
		}
	}

	function msgdone() {
		msgAll = msgText;
	}

	/**
	 * ���b�Z�[�W�����O�w��
	 */
	function msgname(name) {
		_msgon();
		msgName = name;
		if (msgName != null) {
			msg.showNamePlate(msgName);
		} else {
			msg.showNamePlate(0);
		}
	}

	// ��b�҂���
	msgWait = false;
	
	/**
	 * ���b�Z�[�W����
	 */
	function msgclear() {
		msgAll = "";
		msgText = "";
		msgCursor = false;
		if (msg != null) {
			msg.clear();
			msg.showCursor(false);
		}
		msgWait = false;
	}
	
	/**
	 * ���b�Z�[�W���҂��J�[�\���\��
	 */
	function msgwait() {
		msgCursor = true;
		if (msg != null) {
			msg.showCursor(true);
		}
		msgWait = true;
	}

	// ------------------------------------------------------
	// �G���[�����n
	// ------------------------------------------------------

	function errorImage(msg) {
		print(msg + "\n");
	}

	function errorCmd(msg) {
		print(msg + "\n");
	}
	
	function error(msg) {
		print(msg + "\n");
	}

	function errorVoice(msg) {
		print(msg + "\n");
	}

	// ------------------------------------------------------
	// �t���O�����n
	// ------------------------------------------------------

	function setBGMFlag(name) {
		//kag.sflags["bgm_" + (name.toUpperCase())] = true;
	}

	function setCGFlag(name) {
		//kag.sflags["cg_" + (name.toUpperCase())] = true;
	}

	// --------------------------------------------------------------
	// ���b�Z�[�W������
	// --------------------------------------------------------------

	/**
	 * ���O�̕\��
	 * @param name �\����
	 * @param faceName �\�
	 * @param voices �Đ��{�C�X�f�[�^
     */
	function drawName(name, faceName, voices) {
		// �����ɖ��O���w��
		// ���O�̕\��
		msgname(name);
		// �\��\��
	}

	// ------------------------------------------------------
	// ���C�������p
	// ------------------------------------------------------
	
	function update(diff) {
		::Player.update(diff);
		
		// ��ʐ؂�ւ��p���C��
		if (transLayer != null) {
			transLayer.update(diff);
		}
		// ��ʃ}�X�N�؂�ւ��p���C��
		if (screenLayer != null) {
			screenLayer.update(diff);
		}

		// �{�C�X�p�X�V
		if (talkVoiceLength > 0) {
			talkVoiceLength -= diff;
		}
	}

	// ------------------------------------------------------
	// �O���C�x���g
	// ------------------------------------------------------

	/**
	 * BGM�̃t�F�[�h���I������
	 */
	function onBgmFadeCompleted() {
		trigger("bgmfade");
	}

	function onBgmStop() {
		trigger("bgmstop");
	}

	function onSEFadeCompleted(id) {
		trigger("sefade" + id);
	}

	function onSEStop(id) {
		trigger("sestop" + id);
	}
	
	/**
	 * �g�����W�V�����I��������
	 */
	function onExchangeForeBack() {
		clearTrash();
	}
	
	/**
	 * �\���R�s�[�����s���ꂽ�ꍇ�̏���
	 */
	function onCopyLayer(toback, backlay) {
	}

};
