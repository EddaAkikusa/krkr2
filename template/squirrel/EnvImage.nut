BOTH      <- 1;
BU        <- 2;
FACE      <- 3;
SHOW      <- 4;
CLEAR     <- 5;
INVISIBLE <- 6;

/**
 * ���p���C��
 * �E���W�z�u�����@�\������
 */
class EnvGraphicLayer extends GraphicLayer {

	owner = null;

	/**
	 * �R���X�g���N�^
	 * @param owner �I�u�W�F�N�g���������Ă�e
	 * @param isfore �\��ʂ�����ʂ�
	 */
	function EnvGraphicLayer(owner, isfore) {
		this.owner = owner;
		::GraphicLayer(owner.player, isfore);
	}

	_left = null;
	function getLeft() {
		return _left;
	}
	function setLeft(v) {
		_left = v;
		recalcPosition();
	}
	_top = null;
	function getTop() {
		return _top;
	}
	function setTop(v) {
		_top = v;
		recalcPosition();
	}

	function setPos(left, top, ...) {
		//dm("���W�ݒ�:" + left + "," + top);
		_left = left;
		_top  = top;
		recalcPosition();
	}
	
	/**
	 * �z�u�ʒu����
	 */
	function recalcPosition() {
		setRealPosition(_left, _top);
	}

	function setRealPosition(left, top) {
		_setPos(left, top);
	}
};

/**
 * ���摜�̊��N���X
 */
class EnvImage extends EnvBase {

	// �f�t�H���g���
	init = null;

	layerOn = null; //< ���C�����L���ɂȂ��Ă���
	doShow = null;  //< �\���������s
	
	/// �\����ԕύX
    // BOTH      �o�X�g�A�b�v�{�t�F�C�X (�W���j
    // BU        �o�X�g�A�b�v
    // FACE      �t�F�C�X
	// SHOW      �\����ԁi�����I���j
	// CLEAR     ������� (�W��)
    // INVISIBLE ��\��
	_disp = CLEAR;
	function getDisp() {
		return _disp;
	}
	function setDisp(v) {
		if (v != _disp) {
			if (v == SHOW) {
				if (isClear()) {
					if (init != null && "noPose" in init) {
						_disp = init.noPose ? FACE : BOTH;
					} else {
						_disp = BOTH;
					}
				}
			} else {
				_disp = v;
			}
			doShow = true;
			if (isShowBU()) {
				layerOn = true;
				redraw = true;
			}
			updateFlag = true;
		}
	}

	// �o�X�g�A�b�v�\����
	function isShowBU() {
		return _disp <= BU;
    }

	// �t�F�C�X�\����
    function isShowFace() {
		return (_disp == BOTH && env.bothFace) || _disp == FACE;
	}

	// �\������Ă��邩
	function isShow() {
        return _disp <= FACE;
	}

	// ������Ԃ�
	function isClear() {
		return _disp == CLEAR;
	}

	// �X�V�t���O
	updateFlag = false;
	// �ĕ`��w���t���O
	redraw = false;

    /**
     * ���ԗp�ŏ���������
     */
    function setRedraw() {
		if (isShowBU()) {
			redraw = true;
		}
    }
	
	// -----------------------------------------------------------------------
	// ���C������
	// -----------------------------------------------------------------------

    // ���
	_type = null;
	function getType() {
		return _type;
	}
	function setType(v) {
		_type = v;
		updateFlag = true;
    }

	// �T�C�Y�w��
	_width = null;
	function getWidth() {
		return _width;
	}
	function setWidth(v) {
		_width = v;
		updateFlag = true;
	}
	
	_height = null;
	function getHeight() {
		return _height;
	}
	function setHeight(v) {
		_height = v;
		updateFlag = true;
	}
	
	props = { zoom=100, rotate=0, opacity=255 };
	
    // �A�N�V��������
	doStopAction = false;
	actionList = [];

	imageCommands = null;
	
    /**
     * �R���X�g���N�^
     * @param env ��
     */
	constructor(env, name) {
		::EnvBase.constructor(env, name);

		imageCommands = {
			type = function(param, elm) { type =  global[param]; },
			afx = setAfx,
			afy = setAfy,
			xpos = setLeft,
			ypos = setTop,
			left = setLeft,
			top = setTop,
			opacity = setOpacity,
			rotate = setRotate,
			zoom = setZoom,
			reset = setReset,
			action = setAction,
			stopaction = stopAction,
			sync = function(param) { if (param) { syncMode = true; } },
			show = function(param) { disp = SHOW; },
			hide = function(param) { disp = CLEAR; },
			accel = null,  // �ړ��n�����̉����w��p
			nosync = null, // �V���N�����Ȃ��w��
			delay = null,  // �A�N�V���� delay �w��p
			nowait = null, // �A�N�V���� nowait �w��p
		};
	}

	function initImage() {
		setType(null);
		props = { zoom=100, rotate=0, opacity=255 };
		setDisp(CLEAR);
		actionList.resize(0);
	}

	// -------------------------------------------------------
	// ����������
	// -------------------------------------------------------

	doReset = false;

	function setReset(param, elm) {
		setType(null);
		props = { zoom=100, rotate=0, opacity=255 };
		doReset = true;
		updateFlag = true;
	}

	// -------------------------------------------------------

	/*
	 * �����p�����[�^�̑O�����擾
	 * @param value �p�����[�^
	 */
	function getTo(value) {
		if (typeof value == "string") {
			local p;
			if ((p = value.find(":")) != null) {
				return value.slice(0, p);
			} else  if (p == 0) {
				return null;
			} else  {
				return value;
			}
		} else {
			return value == null ? value : value.tointeger();
		}
	}
	
	/*
	 * �����p�����[�^�̌㑤���擾
	 * @param value �p�����[�^
	 */
	function getFrom(value) {
		if (typeof value == "string") {
			local p;
			if ((p = value.indexOf(":")) != null) {
				return value.slice(p+1);
			} else {
				return null;
			}
		}
		return null;
	}

	/**
	 * �ꏊ�w�� ���Έʒu�w��̔���B
	 * �w��l�� "%" �ŏI����Ă�����ő�l�ւ̑��΂ł̎w��Ƃ݂Ȃ�
	 * �w��l�� "@" �ł͂��܂��Ă����猻�ݒl�ւ̑��΂̎w��Ƃ݂Ȃ�
	 * @param base  ���ݒl
	 * @param value �w��l
	 * @param valueBase �w��̍ő�l
	 */
	function calcRelative(base, value, valueBase) {
		if (value == null) {
			return value;
		} else {
			if (typeof value == "string") {
				// ���Ύw��̏ꍇ
				if (value.slice(0,1) == "@") {
					value = value.slice(1);
					// %�w��̏ꍇ
					if (valueBase != null && value.slice(-1) == "%") {
						value = valueBase * (value.slice(0,-1)).tointeger() / 100;
					}
					return base.tointeger() + value.tointeger();
				} else {
					// %�w��̏ꍇ
					if (valueBase != null && value.slice(-1) == "%") {
						value = valueBase * (value.slice(0,-1)).tointeger() / 100;
					}
				}
			}
			return value.tointeger();
		}
	}
	
	function setPropFromTo(name, max, from, to, elm) {
		if (to != null) {
			// �����l�s��̏ꍇ�͓����l�̂܂܏���
			if (!(name in props) && from == null) {
				from = to;
			}
			addAction({
				name = {
					handler = "MoveAction",
					start = calcRelative(props[name], from, max),
					value = calcRelative(props[name], to, max),
					time  = getval(elm, "time", 0),
					accel = getval(elm, "accel",0),
				}
			});
		}
	}

	function setProp(name, max, param, elm) {
		setPropFromTo(name, max, getFrom(param), getTo(param), elm);
	}

	function setAfx(param, elm) {
		setProp("afx", width, param, elm);
	} 

	function setAfy(param, elm) {
		setProp("afy", height, param, elm);
	} 
	
	function setLeft(param, elm) {
		setProp("left", env.xmax, param, elm);
	} 

	function setTop(param, elm) {
		setProp("top", env.ymax, param, elm);
    }

	function setOpacity(param, elm) {
		setProp("opacity", 255, param, elm);
	}

	function setRotate(param, elm) {
		setProp("rotate", 360, param, elm);
	}

	function setZoom(param, elm) {
		setProp("zoom", 100, param, elm);
	}

	/**
	 * ���Βl�v�Z Action.tjs ����̃R�s�[
	 * @param value �l�w��
	 * @param orig �I���W�i���̒l
	 */
	function getRelative(value, orig) {
		if (typeof value == "string" && (value.indexOf("@") != null)) {
			return player.eval(replace(value, "@", orig.tostring()));
		} else {
			return value.tointeger();
		}
	}

	/**
	 * �A�N�V�����̓K�p
	 */
	function applyMoveAction(action) {
		foreach (name, info in action) {
			if (info != null && typeof info == "table" && "handler" in info && info.handler == "MoveAction" && "value" in info) {
				//dm("�A�N�V�����K�p " + name + ":" + info.value);
				props[name] <- getRelative(info.value, name in props ? props[name] : null);
			}
		}
	}

    /**
	 * �A�N�V�������̒ǉ�
	 * @param action �A�N�V�������
     */
	function addAction(action) {
		if (typeof action == "table") {
			actionList.append(action);
			applyMoveAction(action);
		} else if (typeof action == "array" && action.len() > 0) {
			actionList.append(action);
			for (local j=0;j<action.len();j++) {
				applyMoveAction(action[j]);
			}
		}
		updateFlag = true;
	}

    /**
     * �A�N�V������ݒ�
     * @param name �A�N�V������
     * @param elm �p�����[�^
     */
	function setAction(name, elm) {
		// �A�N�V������񂩂�T��
		if (name in env.actions) {
			local info = env.actions[name];
			if (typeof info == "table") {
				local time = "time" in info ? info.time:null;
				local action = player.copyActionInfo(info, time, elm);
				addAction(action);
				return true;
			} else if (typeof info == "array") {
				//dm("�z��A�N�V����:" + name);
				local arrayAction = [];
				for (local i=0;i<info.len();i++) {
					local i = info[i];
					local time = "time" in i ? i.time:null;
					if (typeof i == "string") {
						// ������̏ꍇ�ċA�I�ɎQ��
						i = i in env.actions ? i = env.actions[i] : null;
					}
					if (i != null && typeof i == "table") {
						local action = player.copyActionInfo(i, time, elm);
						arrayAction.append(action);
					}
				}
				addAction(arrayAction);
				return true;
			}
			return false;
		}
		local action = getActionInfo(name, elm);
		if (action != null) {
			addAction(action);
			return true;
		}
		return false;
	}

    /**
	 * �S�A�N�V����������
     */
	function stopAction() {
		doStopAction = true;
		updateFlag = true;
	}

	// --------------------------------------------------------------

	dispMode = false;
	syncMode = false;
	prevShow = false;

	/**
	 * ��ԍX�V����
     */
	function updateLayer(layer) {

		//dm("updateLayer:" + name);
		if (doReset) {
			//dm("���Z�b�g����");
			layer.reset();
		}
		if (doStopAction) {
			//dm("�A�N�V������~");
			layer.stopAction();
		}
		if (doShow) {
			//dm("�\���ύX!");
			layer.visible = isShowBU();
		}
		// ���C���p�����[�^���f
		if (_type != null && layer.type != type) { layer.type = type; }
		if (_width != null && _width != layer.width || _height != null && _height != layer.height()) {
			layer.setSize(_width, _height);
		}
		// �A�N�V��������
		for (local i=0;i<actionList.len();i++) {
			layer.beginAction(actionList[i]);
		}
		// �A�N�V�����I���܂�
		if (syncMode) {
			ret = player.waitAction(layer, true);
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

		// ���ʃR�}���h
		if (::EnvBase.doCommand(cmd, param, elm)) {
			return true;
        }
		
		// Image�n���ʃR�}���h
		if (cmd in imageCommands) {
			local func = imageCommands[cmd];
			if (func != null) {
				func(param, elm);
            }
			return true;
		}

		// �A�N�V����
		if (setAction(cmd, elm)) {
			return true;
		}

		// �A�N�V�����p�����[�^�̔r��
		if (cmd in player.actionParam) {
			return true;
		}

		return false;
	}
	
    // ���̃��\�b�h����������
    // function getLayer(isfore);
	// function drawLayer(layer)

	/**
	 * �V���C������
	 */
	function createLayer(isfore) {
		return EnvGraphicLayer(this, isfore);
	}

	// -------------------------------------------------------------------------------

	/**
	 * �P���X�V
	 */
	function update(isfore) {
		//dm("�X�V����:" + name + ":" + layerOn + ":" + updateFlag + ":" + redraw);
		if (layerOn && (updateFlag || redraw)) {
			local layer = getLayer(isfore);
			if (redraw) {
				//dm("�ĕ`��:" + name);
				layer.reset();
				if (type != null) {
					layer.type = type;
				}
				drawLayer(layer);
				foreach (name, value in props) {
					layer[name] = value;
				}
				layer.recalcPosition();
				redraw = false;
			}
			updateLayer(layer);
			updateFlag = false;
			return layer;
		}
		updateFlag = false;
	}

	/**
	 * �\���X�V�O�̃t���O������
	 */
	function tagcommand(elm) {
		syncMode = false;
		layerOn = prevShow = isShowBU();
		dispMode = 0;
		doReset = false;
		doStopAction = false;
		actionList.clear();
		super.tagcommand(elm);
	}

	/**
	 * �\�������p�̎����g�����W�V�����̎w��
	 */
	function setDispAutoTrans() {
	}
	
	function command(elm) {
		::EnvBase.command(elm);
		// 0:��\�� 1:�\�� 2:���� 3:�X�V
		dispMode = (prevShow ? 2 : 0) + (isShowBU() ? 1 : 0);
		// �\���E�����w��ɑΉ�
		if (dispMode == 1 || dispMode == 2) {
			setDispAutoTrans();
		}
	}
	
	/**
	 * �\���X�V�������ꉻ
	 */
	function doUpdate(elm) {
		if ("method" in trans || dispMode == 3) {
			//dm("�摜�X�V:" + trans.method + ":" + trans.showaction + ":" + trans.hideaction);
			player.backup();
			local layer = update(false);
			player.beginTransition(trans);
			if (layer != null) {
				player.beginTransActionUpdate(trans, layer);
			}
		} else if (dispMode == 2) {
			//dm("�摜����");
			doShow = false;
			local layer = update(true);
			if (layer != null) {
				player.beginTransAction(trans, layer, true, elm != null && getval(elm,"nosync"));
			}
		} else if (dispMode == 1) {
			//dm("�摜�\��");
			doShow = false;
			local layer = update(true);
			if (layer != null) {
				player.beginTransAction(trans, layer, false, elm != null && getval(elm,"nosync"));
			}
		} else {
			update(true);
		}
	}
}
