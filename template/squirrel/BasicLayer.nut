// �萔��`
AFFINEOFFSET_CENTER <- -100000;
AFFINEOFFSET_TOP    <- -100001;
AFFINEOFFSET_BOTTOM <- -100002;
AFFINEOFFSET_LEFT   <- -100001;
AFFINEOFFSET_RIGHT  <- -100002;

/**
 * ��{�������C��
 * �E�摜�̕\��
 * �E���W�ʒu����
 * �E�\�����x���w��
 * �E�s�����x�w��
 * �E��]/�g��k��
 */
class BasicLayer extends Object {

	// �����Ώە������C��
	_layer = null;

    /**
	 * �R���X�g���N�^
     */
	constructor() {
		::Object.constructor();
		_layer = Layer();
    }

	/**
	 * �����̎擾
	 */
	function getWidth() {
		return _layer.getWidth();
	}

	/**
	 * �c���̎擾
	 */
	function getHeight() {
        return _layer.getHeight();
    }

	/**
	 * �摜�̃��[�h
	 * @param image �摜
	 */
	function loadImage(image) {
		local id = getSceneDataId(image);
		if (id != null) {
			_layer.loadImage();
		} else {
			throw "�摜�̓ǂݍ��݂Ɏ��s���܂���:" + image;
		}
    }

	/**
	 * �摜�̃��[�h
	 * @param 
	 */
	function loadImages(elm) {
		local image = getval(elm, "storage");
		if (image != null) {
			loadImage(image);
		}
		// �p�����[�^�ɉ���������
	}
	
    // -----------------------------------------
    // �\���֘A
    // -----------------------------------------
    
	_visible = false;
	function setVisible(visible) {
		_visible = visible;
        _layer.show(show);
    }
    function getVisible() {
		return _visible;
    }

	_absolute = 0;
	function setAbsolute(abs) {
		print("���C�����x���w��:" + level + "\n");
		_absolute = abs;
		//_layer.setLevel(2048-level);
	}
	function getAbsolute() {
		return _absolute;
	}

    _opacity = 100;
    function setOpacity(opacity) {
		//print("���C�������x�w��:" + opacity + "\n");
        _opacity = opacity;
		_layer.setOpacity(_opacity);
    }
    function getOpacity() {
        return _opacity;
    }
    
    // -----------------------------------------
    // �A�t�B���ό`�֘A
	// -----------------------------------------

	_afn_offx = AFFINEOFFSET_CENTER;
	_afn_offy = AFFINEOFFSET_CENTER;
	
	function calcAffineOffset() {
		local x, y;
		switch (_afn_offx) {
		case AFFINEOFFSET_CENTER: x = width/2;   break;
		case AFFINEOFFSET_LEFT:   x = 0;         break;
		case AFFINEOFFSET_RIGHT:  x = width;     break;
		default:                  x = _afn_offx; break;
		}
		switch (_afn_offy) {
		case AFFINEOFFSET_CENTER: y = height/2;  break;
		case AFFINEOFFSET_TOP:    y = 0;         break;
		case AFFINEOFFSET_BOTTOM: y = height;    break;
		default:                  y = _afn_offy; break;
		}
		_layer.setAffineOrigin(ax.tointeger(), ay.tointeger());
	}

	function setAffineOffset(afx, afy) {
		if (afx != null) {
			if (typeof afx == "string") {			
				if (afx == "center" || afx == "void" || afx == "null") {
					_afn_offx = AFFINEOFFSET_CENTER;
				} else if (afx == "left" || afx == "top") {
					_afn_offx = AFFINEOFFSET_LEFT;
				} else if (afx == "right" || afx == "bottom") {
					_afn_offx = AFFINEOFFSET_RIGHT;
				} else {
					_afn_offx = afx.tointeger();
				}
			} else {
				_afn_offx = afx.integer();
			}
		}
		if (afy != null) {
			if (typeof afy == "string") {
				if (afy == "center" || afy == "void" || afy=="null") {
					_afn_offy = AFFINEOFFSET_CENTER;
				} else if (afy == "left" || afy == "top") {
					_afn_offy = AFFINEOFFSET_LEFT;
				} else if (afy == "right" || afy == "bottom") {
					_afn_offy = AFFINEOFFSET_RIGHT;
				} else {
					_afn_offy = afy.tointeger();
				}
			} else {
				_afn_offy = afy.tointeger();
			}
		}
		calcAffine();
	}
	
	function setAfx(v) {
		setAffineOffset(v, null);
	}

	function getAfx() {
		return _afn_offx;
    }

	function setAfy(v) {
		setAffineOffset(null, v);
	}

	function getAfy() {
		return _afn_offy;
    }

    _rotate = 0;
    function setRotate(rotate) {
        _rotate = rotate;
        _layer.setRotate(rotate.tointeger());
    }
    function getRotate() {
        return _rotate;
    }

    _zoom = 100;
    function setZoom(zoom) {
        _zoom = zoom;
        _layer.setZoom(zoom.tointeger());
    }
    function getZoom() {
        return _zoom;
    }

    // -----------------------------------------
    // �ʒu�w��֌W
    // -----------------------------------------

	_left = 0;
    _top  = 0;

    function _setPos(x, y) {
		_layer.setPos(x.tointeger(), y.tointeger());
    }
    
    /**
     * �ʒu�w��
     */
	function setPos(left, top, ...) {
		_left = left;
		_top  = top;
		_setPos(_left, _top);
	}

    function getLeft() {
        return _left;
    }

	function setLeft(v) {
		_left = v;
		_setPos(_left, _top);
	}
	
    function getTop() {
		return _top;
    }

	function setTop(v) {
		_top = v;
		_setPos(_left, _top);
	}
    
    /**
     * ��Ԃ̕���
     */
    function assign(orig) {
		//print("��Ԃ̕���:" + orig + "\n");
		_layer.copyImage(orig._layer);
		setVisible(orig.visible);
		setAbsolute(orig.absolute);
		setAffineOrigin(orig.afx, orig.afy);
        setRotate(orig.rotate);
		setZoom(orig.zoom);
		setPos(orig.left, orig.top);
    }

    /**
     * ��Ԃ̃��Z�b�g
     */
    function reset() {
        setOpacity(100);
		setAfx("left");
		setAfy("top");
        setRotate(0);
        setZoom(100);
        setActionOffset(0,0);
    }
};
