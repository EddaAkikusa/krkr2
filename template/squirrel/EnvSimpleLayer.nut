/**
 * ����p���C��
 */
class SimpleLayerGraphicLayer extends EnvGraphicLayer {

	xbase = null;
	ybase = null;
	originMode = null;
	
	function SimpleLayerGraphicLayer(owner, isfore) {
		::EnvGraphicLayer.constructor(pwner, ifore);
		originMode = owner.originMode;
		xbase = owner.xbase;
		ybase = owner.ybase;
	}

	function recalcPosition() {
		local xdiff;
		local ydiff;
		switch (originMode) {
		case 1:
			xdiff = 0;
			ydiff = 0;
			break;
		case 2:
			xdiff = getWidth() / 2;
			ydiff = 0;
			break;
		case 3:
			xdiff = getWidth();
			ydiff = 0;
			break;
		case 4:
			xdiff = getWidth();
			ydiff = getHeight() / 2;
			break;
		case 5:
			xdiff = getWidth();
			ydiff = getHeight();
			break;
		case 6:
			xdiff = getWidth() / 2;
			ydiff = getHeight();
			break;
		case 7:
			xdiff = 0;
			ydiff = getHeight();
			break;
		case 8:
			xdiff = 0;
			ydiff = getHeight() / 2;
			break;
		default:
			xdiff = getWidth() / 2;
			ydiff = getHeight() / 2;
			break;
		}

		//dm("�ʒu�w��2:" + xbase + "," + ybase + ":" + xdiff + "," + ydiff + ":" + left + "," + top);
		setRealPosition(xbase + left - xdiff,
						ybase + top  - ydiff);
	}
};

/**
 * �P�ƃ��C��
 */
class EnvSimpleLayer extends EnvLayer {

	/**
	 * �\�������p�̎����g�����W�V�����̎w��
	 */
	function setDispAutoTrans() {
		if ("layerDispTrans" in env.envinfo) {
			setAutoTrans(env.envinfo.layerDispTrans);
		}
	}

	xbase = null;
	ybase = null;
	_viewOriginMode = null; // ��ʌ��_�ʒu�w��

	function getViewOriginMode() {
		return _viewOriginMode;
	}
	function setViewOriginMode(v) {
		_viewOriginMode = v;
		switch (v) {
		case 1:
			xbase = 0;
			ybase = 0;
			break;
		case 2:
			xbase = env.xmax;
			ybase = 0;
			break;
		case 3:
			xbase = env.xmax*2;
			ybase = 0;
			break;
		case 4:
			xbase = env.xmax*2;
			ybase = env.ymax;
			break;
		case 5:
			xbase = env.xmax*2;
			ybase = env.ymax*2;
			break;
		case 6:
			xbase = env.xmax;
			ybase = env.ymax*2;
			break;
		case 7:
			xbase = 0;
			ybase = env.ymax*2;
			break;
		case 8:
			xbase = 0;
			ybase = env.ymax;
			break;
		default:
			xbase = env.xmax;
			ybase = env.ymax;
			break;
		}
	}

	originMode = 9;     // ���C�����_
    // ����1 ����2 �E��3
    // ��  8 ����9   �E4
    // ����7 ����6 ����5

	simpleLayerCommands = null;
	
    /**
     * �R���X�g���N�^
     * @param env
     * @param name
     * @param level
     */
	function constructor(env, name) {
		::EnvLayer.EnvLayer(env, name);
		setViewOriginMode(9);
		simpleLayerCommands = {
			origin  = function(cmd,elm) { originMode = cmd.tointeger(); },
			vorigin = function(cmd,elm) { setViewOriginMode(cmd.tointeger()); },
			front   = setFront,
			back    = setBack,
			level   = setLevel,
		};
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
		if (::EnvLayer.doCommand(cmd, param, elm)) {
			return true;
        }

		if (cmd in simpleLayerCommands) {
			local func = simpleLayerCommands[cmd];
			if (func != null) {
				func(param, elm);
			}
            return true;
        }
		
		// �摜�̃��[�h
		return setImageFile(cmd, elm);
	}

	/**
	 * �V���C������
	 */
	function createLayer(isfore) {
		return SimpleLayerGraphicLayer(this, isfore);
	}

}
