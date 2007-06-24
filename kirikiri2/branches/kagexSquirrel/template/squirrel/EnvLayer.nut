/**
 * ���C���n���N���X
 * �摜�\������������Ă���
 */
class EnvLayer extends EnvLevelLayer {

    /// �`�撆�摜
	imageFile = null;

	// �^�C���`�揈�� XXX �ꉞ�c���Ă���
	tileFile = null;

	_tilex = null;
	function getTilex() {
		return _tilex;
	}
	function setTilex(v) {
		_tilex = v;
		updateFlag = true;
	}

	_tiley = null;
	function getTiley() {
		return _tiley;
	}
	function setTiley(v) {
		_tiley = v;
		updateFlag = true;
	}
	
	envLayerCommands = null;

	function constructor(env, name) {
		::EnvLevelLayer.constructor(env, name);
		envLayerCommands = {
			file = setImageFile,
			tile = setTileFile,
			loop = null,
			alpha = null,
			width = function(param, elm) { setWidth(param); },
			height = function(param, elm) { setHeight(param); }
		};
	}
	
	function initFile(elm) {
		if ("type" in elm)    type = null;
		if ("opacity" in elm) props.opacity <- 255;
		if ("rotate" in elm)  props.rotate  <- 0;
		if ("zoom" in elm)    props.zoom    <- 100;
		if ("afx" in elm)     delete props.afx;
		if ("afy" in elm)     delete props.afy;
	}
	
	// �摜�t�@�C�����w��
	function setImageFile(file, elm) {

		initFile(elm);
		
		tileFile  = null;
		imageFile = {};
		imageFile.storage    <- file;
		if ("lightcolor" in elm)
			imageFile.lightcolor <- elm.lightcolor;
		if ("lighttype" in elm)
			imageFile.lighttype  <- elm.lighttype;
		if ("brightness" in elm)
			imageFile.brightness <- elm.brightness;
		if ("contrast" in elm)
			imageFile.contrast   <- elm.contrast;
		
		// �w�i�w�莞�ɍ��W�w�肪�Ȃ���Ώꏊ��������������
		if (!("xpos" in elm) && !("left" in elm)) {
			delete props.left;
		}
		if (!("ypos" in elm) && !("left" in elm)) {
			delete props.top;
        }

		setDisp(SHOW);
		redraw = true;

		// �L�^
		player.setCGFlag(file);
		
        return true;
    }

	// �h��Ԃ��C���[�W�t�@�C�����w��
	function setTileFile(file, elm) {

		initFile(elm);
		
		imageFile = null;
		tileFile = file;

		if ("tilex" in elm) { tilex = elm.tilex.tointeger(); }
		if ("tiley" in elm) { tiley = elm.tiley.tointeger(); }
		
		// �w�i�w�莞�ɍ��W�w�肪�Ȃ���Ώꏊ��������������
		if (!("xpos" in elm) && !("left" in elm)) {
			delete props.left;
		}
		if (!("ypos" in elm) && !("left" in elm)) {
			delete props.top;
        }

		setDisp(SHOW);
		redraw = true;

        return true;
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
		if (::EnvLevelLayer.doCommand(cmd, param, elm)) {
			return true;
        }
		// ���C�����ʃR�}���h
		if (cmd in envLayerCommands) {
			local func = envLayerCommands[cmd];
			if (func != null) {
				func(param, elm);
			}
            return true;
        }
		
		return false;
    }

	function drawLayer(layer) {
		if (isShowBU()) {
			if (tileFile != null) {
				//dm("�^�C���摜");
				try {
					layer.setTileImage(tileFile);
					// ���W�␳
					if (layer.tileLayer != null) {
						if (_width == null ) {
							_width = layer.tileLayer.width;
						}
						if (_height == null) {
							_height = layer.tileLayer.height;
						}
					}
				} catch (e) {
					local text = tileFile + ":�^�C���摜�����[�h�ł��܂���";
					player.errorImage(text);
				}
			} else if (imageFile != null) {
				// �摜�̃��[�h
				layer.clearTile();
				try {
					layer.loadImages(imageFile);
					_width  = layer.width;
					_height = layer.height;
				} catch (e) {
					local text = imageFile.storage + ":�摜�����[�h�ł��܂���";
					player.errorImage(text);
				}
			}
		}
	}

	function updateLayer(layer) {
		super.updateLayer(layer);
		if (tileFile != null) {
			layer.setTilePos(tilex, tiley);
		}
	}
}
