@if exp="typeof(global.world_object) == 'undefined'"
@iscript

/**
 * ���[���h�g��
 * ���t�b�N�ꗗ
 * �O���[�o���Ɉȉ��̃��\�b�h���������ꍇ�̓t�b�N�Ƃ��ČĂяo���܂�
 * setTimeHook(time, elm)     ���ԕύX���t�b�N
 * setStageHook(stage, elm)   �X�e�[�W�ύX���t�b�N
 */
var setTimeHook;
var setStageHook;

/**
 * ��̎���
 */
var EMPTY = %[];

/**
 * ���̓��e��\������
 * @param name ����
 * @param dict �����I�u�W�F�N�g
 */
function showKeys(name, dict) {
    name += ":";
    if (dict) {
        var array = [];
        array.assign(dict);
        for (var i=0; i<array.count; i+= 2) {
            if (i != 0) {
                name += ",";
            }
            name += array[i];
        }
        dm(name);
    }
}

/*
 * �����p�����[�^�̑O�����擾
 * @param value �p�����[�^
 */
function getTo(value) {
    var p;
    if ((p = value.indexOf(":")) > 0) {
        return value.substring(0, p);
    } else  if (p == 0) {
        return void;
    } else  {
        return value;
    }
}

/*
 * �����p�����[�^�̌㑤���擾
 * @param value �p�����[�^
 */
function getFrom(value) {
    var p;
    if ((p = value.indexOf(":")) >= 0) {
        return value.substring(p+1);
    } else {
        return void;
    }
}

var transitionName = %[
    "universal" => true,
    "crossfade" => true,
    "scroll" => true,
    "wave" => true,
    "mosaic" => true,
    "turn" => true,
    "rotatezoom" => true,
    "rotatevanish" => true,
    "rotateswap" => true,
    "ripple" => true,
    ];

var transitionParam = %[
    "time" => true,
    "rule" => true,
    "vague" => true,
    "from" => true,
    "stay" => true,
    "wavetype" => true,
    "maxh" => true,
    "maxomega" => true,
    "bgcolor1" => true,
    "bgcolor2" => true,
    "mosaic" => true,
    "maxsize" => true,
    "turn" => true,
    "bgcolor" => true,
    "factor" => true,
    "accel" => true,
    "twistaccel" => true,
    "twist" => true,
    "centerx" => true,
    "centery" => true,
    "rwidth" => true,
    "roundness" => true,
    "speed" => true,
    "maxdrift" => true,
    "msgoff" => true,
    "transwait" => true,
    ];

var actionParam = %[
    "delay" => true,
    "x" => true,
    "y" => true,
    "topTime" => true,
    "vibration" => true,
    "waitTime" => true,
    "cycle" => true,
    "distance" => true,
    "fallTime" => true,
    "angvel" => true,
    "angle" => true,
    "showTime" => true,
    "hideTime" => true,
    "intime" => true,
    "outtime" => true,
    ];

/**
 * ���摜�̊��N���X
 */
class KAGEnvImage {

    var _ret;
    property ret {
        getter() {
            return _ret;
        }
        setter(v) {
            if (v === void) {
                _ret = 0;
            } else {
                if (v < ret) {
                    _ret = v;
                }
            }
        }
    }
    
    function isSkip() {
        return env.kag.skipMode || env.kag.noeffect;
    }

    // �e�����ێ������
    var env;

    /// �\�����
    // BOTH      �o�X�g�A�b�v�{�t�F�C�X (�W���j
    // BU        �o�X�g�A�b�v
    // FACE      �t�F�C�X
    // CLEAR      �������         (�W��)
    // INVISIBLE ��\��
    var _disp;

    // �o�X�g�A�b�v�\����
    function isShowBU() {
        return _disp <= BU;
    }

    function isShowFace() {
        return _disp == BOTH || _disp == FACE;
    }

    property disp {
        getter() {
            return _disp;
        }
        setter(v) {
            if (v !== void) {
                if (v < CLEAR && (_disp >= CLEAR)) {
                    if (opacity === void) {
                        // ��\����Ԃ���\���ɐ؂�ւ��Ƃ��͕s�����x�𐧌�
                        opacity = 255;
                    }
                }
                _disp = v;
                redraw = true;
            }
        }
    }

    /// �ĕ`��w���t���O
    var redraw;
    
    // ����������
    var resetFlag;
    // �t�F�[�h�����t���O
    var fadeTime;
    // ���
    var type;
    // �����x
    var opacityFrom;
    var opacity;
    var opacityTime;
    // ��]
    var rotateFrom;
    var rotate;
    var rotateTime;
    // �Y�[������
    var zoomFrom;
    var zoom;
    var zoomTime;

    // ��]���_�w��
    var afx;
    var afy;

    // �A�N�V��������
    var actionList;
    var syncMode;
    
    // ��ʍX�V�ݒ�
    var trans;

    // �t�F�[�h�w��̃f�t�H���g�l
    property fadeValue {
        getter() {
            return env.fadeValue;
        }
    }
    
    /**
     * �R���X�g���N�^
     * @param env ��
     */
    function KAGEnvImage(env) {
        this.env = env;
        resetFlag = false;
        fadeTime = void;
        actionList = new Array();
        _disp = CLEAR;
    }

    function finalize() {
        actionList.clear();
        invalidate actionList;
    }

    /**
     * �A�N�V������ݒ�
     * @param name �A�N�V������
     * @param elm �p�����[�^
     */
    function setAction(name, elm) {
        if (env.actions !== void) {
            var info;
            if ((info = env.actions[name]) !== void) {
                var action = %[];
                (Dictionary.assign incontextof action)(info, false); 
                // ���v�f����p�����[�^���폜���R�s�[
                foreach(elm, function(name, value, elm, action) {
                    if (actionParam[name] !== void) {
                        action[name] = value;
                        //dm("�ǉ��p�����[�^:" + name + ":" + value);
                        //delete elm[name];
                    }
                }, action);
                actionList.add(action);
                return true;
            }
            if (elm !== void && name.substring(0,5) == "Layer") {
                var moduleType;
                try {
                    moduleType = global[name];
                    if (moduleType !== void && typeof moduleType == "Object" && moduleType.LayerModule != void) {
                        // �A�N�V�������W���[�����̏ꍇ
                        var action = %[];
                        // ���v�f����p�����[�^���폜���R�s�[
                        foreach(elm, function(name, value, elm, action) {
                            if (actionParam[name] !== void) {
                                action[name] = value;
                                //delete elm[name];
                            }
                        }, action);
                        action.module = name;
                        action.time = elm.time if elm.time !== void;
						if (action.time !== void) {
							action.time *= kag.drawspeed;
						}
                        actionList.add(action);
                        return true;
                    }
                } catch() {
                    // ����
                }
            }
        }
        return false;
    }

    /**
     * �g�����W�V������ݒ�
     * @param name �g�����W�V������
     */
    function setTrans(name, elm) {
        var info;
        if (env.transitions !== void && (info = env.transitions[name]) !== void) {
            // �o�^�ς݃g�����W�V����
            var tr = %[];
            // �R�s�[
            (Dictionary.assign incontextof tr)(info, false); 
            // �p�����[�^�̃R�s�[
            foreach(elm, function(name, value, elm, tr) {
                if (transitionParam[name] !== void) {
                    tr[name] = value;
                    //delete elm[name];
                }
            }, tr);
            if (!env.transMode && !isSkip()) {
                trans = tr;
                redraw = true;
            }
            return true;
        }
        
        if (elm !== void && (transitionName[name] !== void ||
                             name.substring(0,5) == "trans")) {
            // �K��̃g�����W�V����
            var tr = %[];
            // �p�����[�^�̃R�s�[
            foreach(elm, function(name, value, elm, tr) {
                if (transitionParam[name] !== void) {
                    tr[name] = value;
                    //delete elm[name];
                }
            }, tr);
            tr.method = name;
            if (!env.transMode && !isSkip()) {
                trans = tr;
                redraw = true;
            }
            return true;
        }
        return false;
    }

    /**
     * �g�����W�V������ݒ�
     * @param param �g�����W�V�����w��@������܂��͎���
     * @return �ݒ肵���ꍇ��
     */
    function setTrans2(param) {
        if (param === void) {
            return false;
        } else if (typeof param == "String") {
            setTrans(param);
            return true;
        } else if (param instanceof "Dictionary") {
            setTrans(param.method, param);
            return true;
        }
        return false;
    }
    
    /**
     * ��ԍX�V����
     */
    function updateLayer(layer) {
        if (resetFlag) {
            type    = void;
            opacity = void;
            rotate  = void;
            zoom    = void;
            afx     = void;
            afy     = void;
            actionList.clear();
            layer.reset();
            resetFlag = false;
            fadeTime = void;
        } else {
            if (redraw) {
                if (isShowBU()) {
                    if (fadeTime !== void) {
                        opacityFrom = 0;
                        opacity     = 255 if opacity === void;
                        opacityTime = fadeTime > 1 ? fadeTime : fadeValue;
                    }
                    layer.visible = true;
                } else if (disp == FACE) {
                    layer.visible = false;
                } else {
                    // ����
                    if (fadeTime !== void) {
                        opacityFrom = void;
                        opacity     = 0 if opacity === void;
                        opacityTime = fadeTime > 1 ? fadeTime : fadeValue;
                    } else {
                        layer.visible = false;
                    }
                }
            }
            fadeTime = void;

            if (afx !== void) {
                layer.afx = afx;
                afx = void;
            }
            if (afy !== void) {
                layer.afy = afy;
                afy = void;
            }
            if (opacity !== void) {
                //dm("�����x�ύX:" + opacity + ":" + opacityTime); 
                if (opacityFrom !== void) {
                    layer.opacity = opacityFrom;
					opacityFrom = void;
                }
                layer.setOpacityTime(opacity, opacityTime);
                opacity = void;
            }
            if (rotate !== void) {
                //dm("��]�ύX:" + rotate + ":" + rotateTime);
                if (rotateFrom !== void) {
                    layer.rotate = rotateFrom;
					rotateFrom = void;
                }
                layer.setRotateTime(rotate, rotateTime);
                rotate = void;
            }
            if (zoom !== void) {
                //dm("�Y�[���ύX:" + zoom + ":" + zoomTime);
                if (zoomFrom !== void) {
                    layer.zoom = zoomFrom;
					zoomFrom = void;
                }
                layer.setZoomTime(zoom, zoomTime);
                zoom = void;
            }
            if (type !== void) {
                //dm("������ʕύX");
                layer.type = type;
                type = void;
            }
            // �A�N�V��������
            for (var i=0;i<actionList.count;i++) {
                layer.beginAction(actionList[i]);
            }
            actionList.clear();

            // �X�L�b�v���͑����ŃA�N�V�������I��������
            if (isSkip()) {
                layer.stopAction();
            }
        }
        if (syncMode) {
			dm("�A�N�V�����҂�");
			if (ret == 0) {
				ret = kag._waitLayerAction(layer);
			}
            syncMode = false;
        }
    }

    var commands = %[
    tagname : null, 
    time : null,
    type : function(param, elm) {
        type =  global[param];
    } incontextof this,
    opacity : function(param, elm) {
        opacityFrom = getFrom(param);
        opacity     = getTo(param);
        opacityTime = isSkip() ? 0 : elm.time;
    } incontextof this,
    fade :  function(param, elm) {
        fadeTime = isSkip() ? void : param;
    } incontextof this,
    rotate : function(param, elm) {
        rotateFrom = getFrom(param);
        rotate     = getTo(param);
        rotateTime = isSkip() ? 0 : elm.time;
    } incontextof this,
    zoom : function(param, elm) {
        zoomFrom = getFrom(param);
        zoom     = getTo(param);
        zoomTime = isSkip() ? 0 : elm.time;
    } incontextof this,
    afx : function(param, elm) {
        afx = param;
    } incontextof this,
    afy : function(param, elm) {
        afy = param;
    } incontextof this,
    reset : function(param, elm) {
        resetFlag = true;
    } incontextof this,
    trans  : function(param, elm) {
        if (param == "void") {
            trans = void;
        } else {
            setTrans(param, elm);
        }
    } incontextof this,
    action : setAction incontextof this,

    sync : function(param) { if (param) { syncMode = true; } } incontextof this,

    show : function(param) { disp = BOTH;   } incontextof this,
    hide : function(param) { disp = CLEAR; } incontextof this,
    visible : function(param) { disp = param ? BOTH : CLEAR; }  incontextof this,
        ];

    /**
     * �R�}���h�̎��s
     * @param cmd �R�}���h
     * @param param �p�����[�^
     * @param elm ���̃R�}���h���܂ޑS�p�����[�^
     * @return ���s���s��ꂽ�ꍇ true
     */
    function doCommand(cmd, param, elm) {
        //dm("�R�}���h����:" + cmd + " �p�����[�^:" + param);
        var func;
        if ((func = commands[cmd]) !== void) {
            if (func != null) {
                func(param, elm);
            }
            return true;
        }
        // ��ōl�����ق������������H
        if (actionParam[cmd] !== void) {
            return true;
        }
        if (transitionParam[cmd] !== void) {
            return true;
        }
        return false;
    }

    /**
     * �Z�[�u����
     */
    function onStore(f) {
        f.resetFlag = resetFlag;
        f.type = type;
        f.opacityFrom = opacityFrom;
        f.opacity     = opacity;
        f.opacityTime = opacityTime;
        f.rotateFrom  = rotateFrom;
        f.rotate      = rotate;
        f.rotateTime  = rotateTime;
        f.zoomFrom    = zoomFrom;
        f.zoom        = zoom;
        f.zoomTime    = zoomTime;
        f.actionList  = new Array();
        f.actionList.assign(actionList);
        f.disp = disp;
    }

    /**
     * ���[�h����
     */
    function onRestore(f) {
        resetFlag = f.resetFlag;
        type = f.type;
        opacityFrom = f.opacityFrom;
        opacity     = f.opacity;
        opacityTime = f.opacityTime;
        rotateFrom  = f.rotateFrom;
        rotate      = f.rotate;
        rotateTime  = f.rotateTime;
        zoomFrom    = f.zoomFrom;
        zoom        = f.zoom;
        zoomTime    = f.zoomTime;
        actionList.clear();
        if (f.actionList) {
            actionList.assign(f.actionList);
        }
        disp = f.disp;
    }

    // ���̃��\�b�h����������
    // function getLayer(base);
    // function drawLayer(layer);
    // 
    function calcPosition(layer) {
    }

    // �g�����W�V�������s
    function beginTransition(trans) {
        kag.fore.base.beginTransition(trans);
        if (trans.transwait !== void) {
            ret = kag.waitTime((int)trans.time + (int)trans.transwait, kag.clickSkipEnabled);
        } else {
            ret = kag.waitTransition(EMPTY);
        }
    }

    function hideMessage() {
        if (trans !== void && trans.msgoff) {
            kag.invisibleCurrentMessageLayer();
        }
    }
    
    /**
     * �摜�̕`��
     */
    function updateImage(base) {
        kag.updateBeforeCh = 1;
        // �`��X�V���K�v�ȏꍇ
        if (redraw) {

            if (base === void && trans) {

                kag.fore.base.stopTransition();
                
                // �X�V�����𑖂点��ꍇ
                if (trans.time === void) {
                    trans.time = 1000;
                }
                trans.children = true;
    
                // �S���C�����o�b�N�A�b�v
                kag.backupLayer(%[], true);

                // �����C�����Ώ�
                var layer = getLayer(kag.back);
                if (isShowBU()) {
                    drawLayer(layer);
                    calcPosition(layer);
                }
                updateLayer(layer);
                beginTransition(trans);
                
            } else {

                // �\�ɕ`��
                var layer = getLayer(base);

                // �t�F�[�h����
                // ���ɕ\������Ă�Ƃ��̓g�����W�V�����ő�p
                if (fadeTime !== void && isShowBU() && layer.visible && layer.opacity > 0) {

                    kag.fore.base.stopTransition();
                    var trans = %[ "method" => "crossfade",
                                   "children" => true,
                                   "time" => fadeTime > 1 ? fadeTime : fadeValue];
                    fadeTime = void;

                    // �S���C���𗠂Ƀo�b�N�A�b�v
                    kag.backupLayer(%[], true);

                    // �����C�����Ώ�
                    layer = getLayer(kag.back);
                    drawLayer(layer);
                    calcPosition(layer);
                    updateLayer(layer);
                    beginTransition(trans);

                } else {
                    // �t�F�[�h����
                    //dm("�t�F�[�h�� opacity �����Ŏ���");
                    if (isShowBU()) {
                        drawLayer(layer);
                        calcPosition(layer);
                    }
                    updateLayer(layer);
                }
            }

            redraw = false;
        } else {
            var layer = getLayer(base);
            calcPosition(layer);
            updateLayer(layer);
        }
        trans = void;
    }
}

/**
 * ���C���n���N���X
 */
class KAGEnvLayer extends KAGEnvImage {

    // kag �̎Q��
    property kag {
        getter() {
            return env.kag;
        }
    }

    /// �`�撆�摜
    var imageFile;
	var xoff;
	var yoff;

    function KAGEnvLayer(env) {
        super.KAGEnvImage(env);
        redraw = false;
    }

    function onStore(f) {
        f.imageFile = imageFile;
        f.xoff = xoff;
        f.yoff = yoff;
        super.onStore(f);
    }
    
    function onRestore(f) {
        imageFile = f.imageFile;
        xoff = f.xoff;
        yoff = f.yoff;
        super.onRestore(f);
    }

    var _layerCommands = %[
    file : function(param, elm) {
        imageFile = param;
        disp = BOTH;
    } incontextof this,
    xoff : function(param, elm) {
        xoff = param;
    } incontextof this,
    yoff : function(param, elm) {
        yoff = param;
    } incontextof this
        ];

    /**
     * �R�}���h�̎��s
     * @param cmd �R�}���h
     * @param param �p�����[�^
     * @param elm ���̃R�}���h���܂ޑS�p�����[�^
     * @return ���s���s��ꂽ�ꍇ true
     */
    function doCommand(cmd, param, elm) {

        // ���ʃR�}���h
        if (super.doCommand(cmd, param, elm)) {
            return true;
        }
        // ���C�����ʃR�}���h
        var func;
        if ((func = _layerCommands[cmd]) !== void) {
            if (func != null) {
                func(param, elm);
            }
            return true;
        }
        // ���C���g���R�}���h�p
        if ((func = layerCommands[cmd]) !== void) {
            if (func != null) {
                func(param, elm);
            }
            return true;
        }

        var find = false;
        
        // �A�N�V����
        if (!find) {
            find = setAction(cmd, elm);
        }

        // �X�V
        if (!find) {
            find = setTrans(cmd, elm);
        }

        // �摜�̃��[�h
        if (!find) {
            imageFile = cmd;
            disp = BOTH;
        }
        return find;
    }

    // ���̃��\�b�h����������
    // function getLayer(base);
    // 
    function drawLayer(layer) {
        if (imageFile !== void) {
            layer.loadImages(%[ "storage" => imageFile]);
            // ���W�␳
            if (xoff !== void || yoff== void) {
                layer.left = xoff if xoff !== void;
                layer.top  = yoff if yoff !== void;
            }
        }
    }

    /**
     * KAG �^�O����
     * @param elm ����
     */
    function tagfunc(elm) {
        //dm("���C���^�O����΂ꂽ��");
        ret = void;
        foreach(elm, doCommand);
        hideMessage();
        updateImage();
        return ret;
    }
}

/**
 * �x�[�X�ɂȂ��Ă��郌�C��
 * event ���C���̎Q�Ƃɗp����
 */
class KAGEnvBaseLayer extends KAGEnvLayer {

    var name;

	var _imageFile;
	property imageFile {
		setter(v) {
            if (v !== void) {
                // �L�^
                kag.sflags["cg_" + (v.toUpperCase())] = true;

                var eventInfo;
                if (env.events !== void) {
                    eventInfo = env.events[v];
                }
                var eventTrans;
                if (eventInfo !== void) {
                    eventTrans = eventInfo.trans;
                    _imageFile = eventInfo.image !== void ? eventInfo.image : v;
                    xoff = eventInfo.xoff;
                    yoff = eventInfo.yoff;
                } else {
                    _imageFile = v;
                    xoff = 0;
                    yoff = 0;
                }
                //dm("�摜�w��:" + _imageFile);
                
                // �g�����W�V�����w��
                if (!setTrans2(eventTrans)) {
                    if (!setTrans2(env.envinfo.eventTrans)) {
                        setTrans2(env.envinfo.envTrans);
                    }
                }

            } else {
                _imageFile = void;
                xoff = 0;
                yoff = 0;
            }
        }
		getter() {
			return _imageFile;
		}
	}

    /**
     * �R���X�g���N�^
     * @param env ��
     * @param name ���̃��C����
     */
    function KAGEnvBaseLayer(env, name) {
        super.KAGEnvLayer(env);
        this.name = name;
    }

    function getLayer(base) {
        if (base === void) {
            base = env.transMode ? kag.back : kag.fore;
        }
        return base[name];
    }

    var layerCommands = %[];
}

/**
 * KAG �O�i���C���̃��x������p
 */
class KAGEnvLevelLayer {

    // ���p���C��
    var layerId;
    /// �\�����x��
    var level;
	/// �\����΃��x��
	var absolute;

    // �\���ʒu���W
    var xpos;
    var ypos;
    var xposFrom;
    var yposFrom;
    var moveTime;
    var moveAccel;
    
    /// �ʒu�ύX
    var reposition;
    var front;
    var back;

    /**
     * �R���X�g���N�^
     * @param layerId �O�i���C��ID
     */
    function KAGEnvLevelLayer(layerId) {
        this.layerId = layerId;
        xpos = 0;
        ypos = 0;
        xposFrom = void;
        yposFrom = void;
        moveTime = void;
        moveAccel = void;
        level = void;
        reposition = true;
    }

    function finalize() {
        // �g���Ă������C��������
        var id = env.initLayerCount + layerId;
        if (id < kag.fore.layers.count) {
            kag.fore.layers[id].clearImage();
            kag.back.layers[id].clearImage();
        }
    }

    /**
     * ���C�����̎擾
     * @param base fore / back      
     * �����Ƀ��x���ɉ����� front/back �̏��������s����
     */
    function getLayer(base) {
        if (base === void) {
            base = env.transMode ? kag.back : kag.fore;
        }
        if (level === void) {
            level = env.defaultLevel;
        }
        var id = env.initLayerCount + layerId;
        kag.toLevel(id, level, base);
        if (front) {
            kag.toFront(id, base);
        } else if (back) {
            kag.toBack(id, base);
        }
        front = back = void;
		absolute = base.layers[id].absolute;
        return base.layers[id];
    }

    /**
     * ���x�����őO��
     */
    function setFront() {
        front = true;
    }

    /**
     * ���x�����Ō��
     */
    function setBack() {
        back = true;
    }

    /**
     * ���x���̎w��
     */
    function setLevel(cmd, elm) {
        level = cmd;
		absolute = void;
    } 

    function setXPos(cmd, elm) {
		//dm("X�ʒu�w��:" + cmd + ":" + elm.time);
        xposFrom = getFrom(cmd);
        xpos     = getTo(cmd);
        if (xposFrom !== void && moveTime === void) {
            moveTime  = elm.time;
            moveAccel = elm.accel;
        }
        reposition = true;
    } 

    function setYPos(cmd, elm) {
        yposFrom = getFrom(cmd);
        ypos     = getTo(cmd);
        if (yposFrom !== void && moveTime === void) {
            moveTime  = elm.time;
            moveAccel = elm.accel;
        }
        reposition = true;
    }
    
    function onStore(f) {
        f.layerId = layerId;
        f.level = level;
		f.absolute = absolute;
        f.xpos = xpos;
        f.xposFrom = xposFrom;
        f.ypos = ypos;
        f.yposFrom = yposFrom;
        f.moveTime = moveTime;
        f.moveAccel = moveAccel;
    }

    function onRestore(f) {
        layerId = f.layerId;
        level = f.level;
		absolute = f.absolute;
        xpos = f.xpos;
        xposFrom = f.xposFrom;
        ypos = f.ypos;
        yposFrom = f.yposFrom;
        moveTime = f.moveTime;
        moveAccel = f.moveAccel;
		// ���x���̕��A
		var layer = kag.fore.layers[env.initLayerCount + layerId];
		if (layer !== void) {
			if (level !== void) {
				layer.level = level;
			}
			if (absolute !== void) {
				layer.absolute = absolute;
			}
	    }
	}
}

/**
 * �P�ƃ��C��
 */
class KAGEnvSimpleLayer extends KAGEnvLevelLayer, KAGEnvLayer {

    // ���O
    var name;
    
    /**
     * �R���X�g���N�^
     * @param env
     * @param layerId
     * @param name
     * @param level
     */
    function KAGEnvSimpleLayer(env, layerId, name) {
        global.KAGEnvLayer.KAGEnvLayer(env);
        global.KAGEnvLevelLayer.KAGEnvLevelLayer(layerId);
        this.name = name;
    }

    function finalize() {
        // ���ԕύX�s��
        global.KAGEnvLevelLayer.finalize();
        global.KAGEnvLayer.finalize();
    }
    
    function calcPosition(layer) {
        if (reposition) {
            var l = kag.scWidth / 2 + (int)xpos - layer.imageWidth / 2;
            var t = kag.scHeight/ 2 + (int)ypos - layer.imageHeight / 2;
            if (moveTime !== void && moveTime > 0) {
                if (xposFrom !== void || yposFrom !== void) {
                    var fl = xposFrom !== void ? kag.scWidth  / 2 + (int)xposFrom - layer.imageWidth / 2 : l;
                    var ft = yposFrom !== void ? kag.scHeight / 2 + (int)yposFrom - layer.imageHeight / 2 : t;
                    layer.setPos(fl, ft);
                }
                layer.setMove(l, t, moveAccel, moveTime);
            } else {
                layer.setMove(l, t);
            }
            xposFrom = void;
            yposFrom = void;
	        moveTime = void;
            reposition = false;
        }
    }
    
    function onStore(f) {
        global.KAGEnvLayer.onStore(f);
        global.KAGEnvLevelLayer.onStore(f);
        f.name = name;
    }

    function onRestore(f) {
        name = name;
        global.KAGEnvLevelLayer.onRestore(f);
        global.KAGEnvLayer.onRestore(f);
    }

    var layerCommands = %[
    xpos : this.setXPos incontextof this,
    ypos : this.setYPos incontextof this,
    front : this.setFront incontextof this,
    back : this.setBack incontextof this,
    level : this.setLevel incontextof this,
    accel : null, // ����
        ];
}

class VoiceTrack extends WaveSoundBuffer {

    var owner;
    
    function VoiceTrack(owner) {
        this.owner = owner;
        super.WaveSoundBuffer(owner);
    }

    function play() {
        owner.onStartVoice();
        super.play(...);
    }

    var prevstatus = "unload"; // ���O�̃X�e�[�^�X

    function onStatusChanged() {
        super.onStatusChanged(...);
        var ps = prevstatus;
        var cs = status;
        prevstatus = cs;
        if(ps == "play" && cs == "stop") {
            owner.onStopVoice();
        }
    }
}

/**
 * �L�����N�^�v���O�C��
 */
class KAGEnvCharacter extends KAGEnvLevelLayer, KAGEnvImage {

    // kag �̎Q��
    property kag {
        getter() {
            return env.kag;
        }
    }

    // �t�F�[�h�w��̃f�t�H���g
    property fadeValue {
        getter() {
            if (init !== void && init.fadeValue !== void) {
                return init.fadeValue;
            } else {
                if (env.envinfo !== void && env.envinfo.charFadeValue !== void) {
                    return env.envinfo.charFadeValue;
                } else {
                    return env.fadeValue;
                }
            }
        }
    }

    /// ���O
    var name;
    /// ��������
    var initName;

    /// ���������
    var init;
    var poses;

    /// �|�[�Y
    var _pose;
    property pose {
        getter() {
            if (_pose !== void) {
                return _pose;
            }
            if (init.defaultPose !== void) {
                return init.defaultPose;
            }
            return "�ʏ�";
        }
        setter(v) {
            _pose = v;
        }
    };

    /// ����
    var dress;
    /// �\��
    var face;

    /// �{�C�X���
    var voice;
	var strVoice;

    // �x�[�X�摜��
    var baseImageName;
    // �x�[�X�摜
    var baseImage;
    // ��摜
    var faceImage;

    // �G���[�V����
    var newEmotionFlag;
    var emotion;
    var emotionLayer;

    /**
     * �R���X�g���N�^
     * @param env ���I�u�W�F�N�g
     * @param layerId �`��Ώۃ��C���ԍ�
     * @param name �L�����N�^��
     * @param init �L�����N�^�������f�[�^
     */
    function KAGEnvCharacter(env, layerId, name, initName, init) {

        global.KAGEnvImage.KAGEnvImage(env);
        global.KAGEnvLevelLayer.KAGEnvLevelLayer(layerId);
        
        //dm("�L�����N�^�o�^:" + name + " layerId:" + layerId); 

        this.name     = name;
        this.initName = initName;
        this.init     = init;
        if (init) {
            poses = init.poses; //showKeys("poses", poses);
        }
    }
    
    function finalize() {
        if (baseImage !== void) {
            invalidate baseImage;
        }
        if (faceImage !== void) {
            invalidate faceImage;
        }
        if (soundBuffer !== void) {
            invalidate soundBuffer;
        }
        if (emotionLayer !== void) {
            invalidate emotionLayer;
        }
        // ���ԕύX�s����
        global.KAGEnvLevelLayer.finalize();
        global.KAGEnvImage.finalize();
    }

    /**
     * �Z�[�u����
     */
    function onStore(f) {
        global.KAGEnvImage.onStore(f);
        global.KAGEnvLevelLayer.onStore(f);
        f.name = name;
        f.initName = initName;
        f.pose = pose;
        f.dress = dress;
        f.face = face;
        f.voice = voice;
		f.strVoice = strVoice;
    }

    /**
     * ���[�h����
     */
    function onRestore(f) {
        pose  = f.pose;
        dress = f.dress;
        face  = f.face;
        voice = f.voice;
		strVoice = f.strVoice;
        global.KAGEnvLevelLayer.onRestore(f);
        global.KAGEnvImage.onRestore(f);
    }

    /**
     * �|�[�Y�̐ݒ�
     */
    function setPose(poseName) {
        var info = poses[poseName];
        if (info !== void) {
            if (poseName != pose || disp == CLEAR) {
                pose = poseName;
                if (disp == CLEAR) {
                    disp = BOTH;
                }
                redraw = true;

                // �g�����W�V�����w��
                if (isShowBU()) {
                    if (!setTrans2(info.trans)) {
                        if (!setTrans2(init.poseTrans)) {
                            if (!setTrans2(init.charTrans)) {
                                if (!setTrans2(env.envinfo.poseTrans)) {
                                    setTrans2(env.envinfo.charTrans);
                                }
                            }
                        }
                    }
                }

            }
            // ��������������
            if (dress !== void && (info.dresses == void || info.dresses[dress] == void)) {
                dress = void;
            }
            // �\���������
            if (face !== void && (info.faces == void || info.faces[face] == void)) {
                face = void;
            }
        }
        // ypos �̓|�[�Y�ݒ肵���Ƃ���0�ɖ߂�
        ypos = 0;
        // �G���[�V�����N���A
        setEmotion();
    }

    /**
     * �����̐ݒ�
     */
    function setDress(dressName) {
        if (dressName != dress || disp == CLEAR) {
            dress = dressName;
            if (disp == CLEAR) {
                disp = BOTH;
            }
            redraw = true;
            // �g�����W�V�����w��
            if (isShowBU()) {
                if (!setTrans2(init.dressTrans)) {
                    if (!setTrans2(init.charTrans)) {
                        if (!setTrans2(env.envinfo.dressTrans)) {
                            setTrans2(env.envinfo.charTrans);
                        }
                    }
                }
            }
        }
    }

    /**
     * �\��̐ݒ�
      */
    function setFace(faceName) {
        if (faceName != face || disp == CLEAR) {
            face = faceName;
            if (disp == CLEAR) {
                disp = BOTH;
            }
            redraw = true;
            // �g�����W�V�����w��
            if (isShowBU()) {
                if (!setTrans2(init.faceTrans)) {
                    if (!setTrans2(init.charTrans)) {
                        if (!setTrans2(env.envinfo.faceTrans)) {
                            setTrans2(env.envinfo.charTrans);
                        }
                    }
                }
            }
        }
    }

    function setPositionTrans(info) {
        // �g�����W�V�����w��
        if (isShowBU()) {
            if (!setTrans2(info.trans)) {
                setTrans2(env.envinfo.positionTrans);
            }
        }
    }
    
    /**
     * �\���ʒu�̐ݒ�
     */
    function setPosition(cmd, elm) {

        var posName = getTo(cmd);
        var info;
        if (env.positions === void || (info = env.positions[posName]) === void) {
            return;
        }
        
        switch (info.type) {
        case global.KAGEnvironment.XPOSITION:
            moveTime = isSkip() ? 0 : elm.time;
            moveAccel = (elm.accel === void) ? 0 : +elm.accel;
            var posFrom = getFrom(cmd);
            var fromInfo;
            if (posFrom !== void && (fromInfo = env.positions[posFrom]) !== void) {
                xpos       = info.xpos;
                xposFrom   = fromInfo.xpos;
                reposition = true;
            } else {
                xpos = info.xpos;
                reposition = true;
                setPositionTrans(info);
            }
            if (disp == CLEAR) {
                disp = BOTH;
            }
            break;
        case global.KAGEnvironment.YPOSITION:
            moveTime = isSkip() ? 0 : elm.time;
            moveAccel = (elm.accel === void) ? 0 : +elm.accel;
            var posFrom = getFrom(cmd);
            var fromInfo;
            if (posFrom !== void && (fromInfo = env.positions[posFrom]) !== void) {
                ypos       = info.ypos;
                yposFrom   = fromInfo.ypos;
                reposition = true;
            } else {
                ypos = info.ypos;
                reposition = true;
                setPositionTrans(info);
            }
            if (!visible) {
                visible = true;
            }
            break;
        case global.KAGEnvironment.DISPPOSITION:
            disp = info.disp;
            redraw = true;
            setPositionTrans(info);
            break;
        case global.KAGEnvironment.LEVEL:
            //dm("���x��:" + info.level);
            level = info.level;
            redraw = true;
            reposition = true;
            if (disp > CLEAR) {
                redraw = true;
            }
            setPositionTrans(info);
            break;
        }
    }

    /**
     * �G���[�V�����̐ݒ�
     */
    function setEmotion(name, elm) {
        if (name !== void) {
            if (env.emotions !== void) {
                var info;
                if ((info = env.emotions[name]) !== void) {
                    emotion = %[];
                    (Dictionary.assign incontextof emotion)(info, false);
                    newEmotionFlag = true;
                    emotion.time = elm.time if elm.time !== void;
                    return true;
                }
            }
            return false;
        } else {
            // �G���[�V��������
            newEmotionFlag = false;
            if (emotion !== void) {
                invalidate emotion;
                emotion = void;
            }
            if (emotionLayer !== void) {
                invalidate emotionLayer;
                emotionLayer = void;
            }
        }
    }
    
    var charCommands = %[
    pose    : this.setPose incontextof this,
    dress   : this.setDress incontextof this,
    face    : this.setFace incontextof this,
    pos     : this.setPosition incontextof this,
    emotion : this.setEmotion incontextof this,
    xpos    : this.setXPos incontextof this,
    ypos    : this.setYPos incontextof this,
    front   : this.setFront incontextof this,
    back    : this.setBack incontextof this,
    level   : this.setLevel incontextof this,
    voice   : this.setVoice incontextof this,
    clearVoice : this.clearVoice incontextof this,
    playvoice : this.playVoice2 incontextof this,
    waitvoice : this.waitVoice incontextof this,
    facewin : function(param) { disp = FACE;   } incontextof this,
        ];

    /**
     * �R�}���h�̎��s
     * @param cmd �R�}���h
     * @param param �p�����[�^
     * @param elm ���̃R�}���h���܂ޑS�p�����[�^
     * @return ���s���s��ꂽ�ꍇ true
     */
    function doCommand(cmd, param, elm) {

        if (global.KAGEnvImage.doCommand(cmd, param, elm)) {
            return true;
        }

        var func;
        if ((func = charCommands[cmd]) !== void) {
            if (func != null) {
                func(param, elm);
            }
            return true;
        }
        
        var info;
        var find = false;
        if (poses !== void) {
            if (poses[cmd] !== void) {
                find = true;
                setPose(cmd);
            } else {
                var poseInfo;
                if ((poseInfo = poses[pose]) !== void) {
                    var dresses       = poseInfo.dresses;
                    var faces         = poseInfo.faces;
                    if (dresses !== void && dresses[cmd] !== void) {
                        //dm("������ݒ�");
                        find = true;
                        setDress(cmd);
                    } else if (faces !== void && faces[cmd] !== void) {
                        //dm("�\���ݒ�");
                        find = true;
                        setFace(cmd);
                    }
                } else {
                    dm("�|�[�Y��񂪂���܂���:" + pose + ":" + cmd);
                }
            }
        }

        var posName = getTo(cmd);
        if (!find && env.positions !== void && env.positions[posName] !== void) {
            setPosition(cmd, elm);
            find = true;
        }            

        // �A�N�V����
        if (!find) {
            find = setAction(cmd, elm);
        }

        // �G���[�V����
        if (!find) {
            find = setEmotion(cmd, elm);
        }
        
        // �X�V
        if (!find) {
            find = setTrans(cmd, elm);
        }
        
        return find;
    };

    /**
     * �����G�̕`��
     * @param layer �`��Ώۃ��C��
     * @param levelName ���x����
     * @param pose �|�[�Y�w��
     * @oaram face ��`��������t���O
     * @return ���������� true
     */
	function _drawLayerPose(layer, levelName, pose) {

        var imageName;
        var poseInfo;
        if (poses !== void && (poseInfo = poses[pose]) !== void) {

            // ��`��̏ꍇ�� msgImage / msgFaceImage ��D��
            var imageName     = (levelName == env.faceLevelName && poseInfo.msgImage !== void)     ? poseInfo.msgImage : poseInfo.image;
            var faceImageName = (levelName == env.faceLevelName && poseInfo.msgFaceImage !== void) ? poseInfo.msgFaceImage : poseInfo.faceImage;
            var dresses       = poseInfo.dresses;
            var faces         = poseInfo.faces;

            imageName = imageName.replace(/LEVEL/, levelName);

            // �����w�肪�����ꍇ�̓f�t�H���g���Q��
            var dressName;
            if (dress !== void) {
                dressName = dresses[dress];
            } 
            dressName = poseInfo.defaultDress if dressName === void;

            // �\��w�肪�����ꍇ�̓f�t�H���g���Q��
            var faceName;
            if (face !== void) {
                faceName = faces[face];
            }
            faceName = poseInfo.defaultFace if faceName === void;

            //dm("dress:", dressName, "face:", faceName);
            
            if (faceImageName !== void) {
                
                // �番���^�����G

                // �x�[�X����
                var imageFile = imageName.replace(/DRESS/, dressName);

                // �x�[�X�摜�̃��[�h
                if (baseImageName != imageFile) {
                    baseImageName = imageFile;
                    // �摜�x�[�X�̃L���b�V���p
                    if (baseImage === void) {
                        baseImage = new global.Layer(kag, kag.fore.base);
                        baseImage.name = "�����G�摜�L���b�V��:" + name;
                    }
					try {
                        baseImage.loadImages(imageFile);
                    } catch (e) {
                        dm("�����G�x�[�X�摜�̓ǂݍ��݂Ɏ��s���܂���:" + imageFile);
                        if (dressName !== poseInfo.defaultDress) {
                            dm("�f�t�H���g�̕��������p���܂�:" + poseInfo.defaultDress);
                            dressName = poseInfo.defaultDress;
                            imageFile = imageName.replace(/DRESS/, dressName);
                            baseImageName = imageFile;
                            try {
                                baseImage.loadImages(imageFile);
                            } catch (e) {
                                dm("�����G�x�[�X�摜�̓ǂݍ��݂Ɏ��s���܂���:" + imageFile);
                                return false;
                            }
                        } else {
                            return false;
                        }
					} 
                }
                
                // �摜�����C���Ɋ��蓖�Ă�
                layer.assignImages(baseImage);
                // ����������XXX ������ƍČ����K�v����
				layer.type    = layer._initTyoe    = baseImage.type;
                layer.opacity = layer._initOpacity = baseImage.opacity;

                // ���x���p��������
                faceImageName = faceImageName.replace(/LEVEL/, levelName);
                
                //�@�\��w��
                if (faceName !== void) {
                    imageFile = faceImageName.replace(/DRESS/, dressName);
                    imageFile = imageFile.replace(/FACE/, faceName);
                    var imageInfo;
                    if (faceImage === void) {
                        faceImage = new global.Layer(kag, kag.fore.base);
                        faceImage.name = "�����G��摜�����p:" + name;
                    }
                    try {
                        imageInfo = faceImage.loadImages(imageFile);
                    } catch (e) {
                        var succeeded = false;
                        // �����̃f�t�H���g���Ƃ����݂̓G���[�����ł͂Ȃ�
                        if (dressName != poseInfo.defaultDress) {
                            dressName = poseInfo.defaultDress;
                            imageFile = faceImageName.replace(/DRESS/, dressName);
                            imageFile = imageFile.replace(/FACE/, faceName);
                            try {
                                imageInfo = faceImage.loadImages(imageFile);
                                succeeded = true;
                            } catch (e) {
                            }
                        }
                        if (!succeeded) {
                            dm("�\��摜�̓ǂݍ��݂Ɏ��s���܂���:" + imageFile);
                            if (faceName != poseInfo.defaultFace) {
                                dm("�f�t�H���g�̕\������p���܂�:" + poseInfo.defaultFace);
                                faceName = poseInfo.defaultFace;
                                imageFile = faceImageName.replace(/DRESS/, dressName);
                                imageFile = imageFile.replace(/FACE/, faceName);
                                try {
                                    imageInfo = faceImage.loadImages(imageFile);
                                } catch (e) {
                                    dm("�\��摜�̓ǂݍ��݂Ɏ��s���܂���:" + imageFile);
                                }
                            }
                        }
                    }
                    if (imageInfo && imageInfo.offs_x !== void) {
                        layer.operateRect(imageInfo.offs_x, imageInfo.offs_y,
                                          faceImage, 0, 0, faceImage.imageWidth, faceImage.imageHeight);
                    } else {
                        layer.operateRect(0,0,faceImage,0,0,faceImage.imageWidth, faceImage.imageHeight);
                    }
                }
                return true;
                
            } else {
                // �獇���^�����G
                // �������ŏ㏑��
                if (dressName !== void) {
                    imageName = imageName.replace(/DRESS/, dressName);
                }
                // �\��ŏ㏑��
                if (faceName !== void) {
                    imageName = imageName.replace(/FACE/, faceName);
                }
                //dm("imageName", imageName);
                try {
                    if (layer instanceof "AnimationLayer") {
                        layer.loadImages(%[ "storage" => imageName]);
                    } else {
                        layer.loadImages(imageName);
                    }
                    return true;
                } catch (e) {
                    dm("ERROR:�摜�t�@�C���̓ǂݍ��݂Ɏ��s���܂���:" + imageName);
                }
            }
        } else {
            dm("�����G��񂪂���܂���:" + pose);
        }
        return false;
    }

    /**
     * ���݂̗����G�̕`��
     * @param layer �`��Ώۃ��C��
     * @param levelName ���x����
     */
    function _drawLayer(layer, levelName) {

        if (!_drawLayerPose(layer, levelName, pose)) {
            dm("�����G�����[�h�ł��܂���ł���:" + pose);
            if (pose !== init.defaultPose) {
                dm("�f�t�H���g�̃|�[�Y�����p���܂�:" + init.defaultPose);
                if (!_drawLayerPose(layer, levelName, init.defaultPose)) {
                    dm("�����G�����[�h�ł��܂���ł���:" + init.defaultPose);
                    return;
                }
            } else {
                return;
            }
        }

        // �T�C�Y�␳
        layer.setSizeToImageSize();
        
        // �F�␳����
        var timeInfo;
        if ((timeInfo = env.currentTime) !== void) {
            // ���C������
            if (timeInfo.charLightColor !== void) {
                layer.holdAlpha = true;
                layer.fillOperateRect(0,0,
                                      layer.width,layer.height,
                                      timeInfo.charLightColor,
                                      timeInfo.charLightType);
            }
            // ���x�␳
            if (timeInfo.charBrightness !== void) {
                layer.light(timeInfo.charBrightness,
                            timeInfo.charContrast);
            }
        }
    }

    /**
     * ���݂̗����G�̕`��
     * @param layer �`��Ώۃ��C��
     */
    function drawLayer(layer) {
        var levelName;
        var levelInfo = env.levels[level];
        if (levelInfo !== void) {
            levelName = levelInfo.name;
        }
        if (levelName === void) {
            levelName = level;
        }
        var ret = _drawLayer(layer, levelName);
        return ret;
    }
    
    /**
     * �\���`�悷��
     */
    function drawFace(layer, faceLevelName) {
        //dm("�\��`��:" + name);
        if (faceLevelName !== void && poses !== void) {
            _drawLayer(layer, faceLevelName);
            layer.visible = true;
        } else {
            env.clearFace();
        }
    }
    
    /**
     * ���C���z�u����
     * @param layer �����Ώۃ��C��
     */
    function calcPosition(layer) {
        if (reposition) {

            // ���x���ʍ��W�␳�����B�Ƃ肠�����K���ŉ�ʒ����Ɍ������ďk���������Ă���
            var zoom;
			var levelYoffset = 0;
            var levelInfo = env.levels[level];
            if (levelInfo !== void) {
                zoom         = levelInfo.zoom;
				levelYoffset = levelInfo.yoffset;
            }
            if (zoom === void) {
                zoom = 100;
            }
			if (levelYoffset === void) {
				levelYoffset = 0;
			}
            
            var l = kag.scWidth  / 2 + ((int)xpos * zoom / 100) - layer.imageWidth / 2;
            var t = kag.scHeight / 2 + ((env.yoffset - (int)ypos) * zoom / 100) - layer.imageHeight + levelYoffset;
            if (moveTime) {
                if (xposFrom !== void || yposFrom !== void) {
                    var fl = xposFrom !== void ? kag.scWidth  / 2 + ((int)xposFrom * zoom / 100) - layer.imageWidth / 2 : l;
                    var ft = yposFrom !== void ? kag.scHeight / 2 + ((env.yoffset - (int)yposFrom) * zoom / 100) - layer.imageHeight + levelYoffset: t;
                    layer.setPos(fl, ft);
                } 
                layer.setMove(l, t, moveAccel, moveTime);
            } else {
                layer.setMove(l, t);
            }
            xposFrom = void;
            yposFrom = void;
            moveTime = void;
            reposition = false;
        }
    }
    
    /**
     * ���C����ԍX�V����
     * �G���[�V����������ǉ�
     */
    function updateLayer(layer) {
        global.KAGEnvImage.updateLayer(...);

        // �\��`��
        env.drawFace(layer.parent === kag.fore ? 0 : 1, this);
        
        // �L�����N�^���\������ĂȂ��ꍇ�̓G���[�V�����͖���
        if (!isShowBU()) {
            setEmotion();
        }

        // �V�K�G���[�V��������
        if (newEmotionFlag) {
            // �G���[�V�����������s

            if (emotion) {
                var emoX;
                var emoY;
                var emoRev;

                // �\�����W
                emoX    = init.emoX   if init.emoX   !== void;
                emoY    = init.emoY   if init.emoY   !== void;
                emoRev  = init.emoRev if init.emoRev !== void;

                // �|�[�Y�ʕ␳
                var info;
                if ((info = poses[pose]) !== void) {
                    emoX   = poseInfo.emoX   if info.emoX   !== void;
                    emoY   = poseInfo.emoY   if info.emoY   !== void;
                    emoRev = poseInfo.emoRev if info.emoRev !== void;
                }
                
                // �V�K�G���[�V�������C��
                emotionLayer = new global.ActionLayer(layer.window, layer.parent);
                
                if (emoRev && emotion.imageRev !== void) {
                    emoitonLayer.loadImages(emotion.imageRev);
                } else {
                    emotionLayer.loadImages(emotion.image);
                }
                emotionLayer.setPos(layer.left + layer.width/2 + emoX,
                                    layer.top  + layer.height  - emoY);
                emotionLayer.visible = true;
                emotion.beginAction(emotion.action);
            }
            
            newEmotionFlag = false;
        }
    }
    
    /**
     * KAG �^�O����
     * @param elm ����
     */
    function tagfunc(elm) {
        //dm("�L�����N�^�^�O�̌Ăяo��:" + name);
        ret = void;
        foreach(elm, doCommand);
        hideMessage();
        updateImage();
        return ret;
    }

    /**
     * ���ԗp�ŏ���������
     */
    function setTime() {
        // �\�����̏ꍇ�͍ĕ`�揈��
        var layer = kag.fore.layers[env.initLayerCount + layerId];
        if (layer.visible) {
            redraw = true;
        }
    }
    
    var reNumber = new RegExp("^[0-9][0-9]*$");
    
    /**
     * �{�C�X�t�@�C���̎w��
     */
    function setVoice(param) {
        if (typeof param == "Integer") {
            voice = param;
			strVoice = void;
        } else if (typeof param == "String") {
            if (reNumber.test(param)) {
                voice = (int)param;
				strVoice = void;
            } else {
				strVoice = param;
            }
        } else {
            voice = void;
			strVoice = void;
        }
        //dm("�{�C�X�ݒ�:" + param + ":" + voice);
    }

    /**
     * �{�C�X�t�@�C���w��̉���
     */
    function clearVoice() {
        voice = void;
    }
    
    /**
     * ���݂̃{�C�X�t�@�C�����̎擾
     */
    function getVoice(voice) {
        if (typeof voice == "Integer") {
            if (init.voiceFile === void) {
                return void;
            }
            var voiceBase = f.voiceBase !== void ? f.voiceBase : "";
            if (f.name != sf.defaultName || f.family != sf.defaultFamily) {
                // �f�t�H���g�ȊO�̖��O�̏ꍇ
                var name = init.voiceFile.sprintf(voiceBase, voice, "N");
                if (Storages.isExistentStorage(name)) {
                    return name;
                }
            }
            // ���ʂ̖��O�̏ꍇ
            return init.voiceFile.sprintf(voiceBase, voice, "");
        } else if (typeof voice == "String") {
            return voice;
        }
        return void;
    }

    function getCurrentVoice() {
        return getVoice(strVoice !== void ? strVoice : voice);
    }
    
    var soundBuffer;
    var voiceEndTime;
    
    /**
     * �{�C�X�J�n������
     */
    function onStartVoice() {
        //dm("�{�C�X�J�n�{�����[������J�n");
        if (kag.sflags.bgmdown && !env.kag.skipMode) {
            kag.bgm.voldown = true;
        }
    }

    /**
     * �{�C�X��~������
     */
    function onStopVoice() {
        //dm("�{�C�X�J�n�{�����[�����䊮��");
        voiceEndTime = void;
        if (kag.bgm.voldown) {
            kag.bgm.voldown = false;
        }
    }

    /**
     * �{�C�X�̍Đ�
     */
    function playVoice(voicename=void) {
        voiceEndTime = void;
        var ret = void;
        if (voicename === void) {
            voicename = getCurrentVoice();
            if (strVoice !== void) {
                strVoice = void;
            } else {
                if (typeof voice == "Integer") {
                    voice++;
                } else {
                    voice = void;
                }
            }
        }

        if (voicename !== void && kag.getVoiceOn(init.voiceName)) {

            // �ق��̃{�C�X������ XXX
            env.stopAllVoice();

            if (!kag.skipMode && kag.voiceenable) {
                //dm("�Đ�����:" + voicename);
                // �Đ��������s
                if (soundBuffer == void) {
                    soundBuffer = new VoiceTrack(this);
                }
                // �{�����[���␳
                //dm("�{�C�X�{�����[��" + kag.voicevolume);
                soundBuffer.volume2 = kag.getVoiceVolume(init.voiceName) * 1000;
                try {
                    soundBuffer.open(voicename);
                    soundBuffer.play();
                    ret = soundBuffer.totalTime;
                } catch (e) {
                    dm("�{�C�X�Đ��Ɏ��s���܂��� �t�@�C����:" + voicename);
                }
            }
        }
        if (ret) {
            voiceEndTime = System.getTickCount() + ret;
        }
        return ret;
    }

    /**
     * �{�C�X�̊O������̍Đ�
     */
    function playVoice2(param) {
        //dm("�O���{�C�X�Đ�:", param);
        if (typeof param == "String") {
            if (reNumber.test(param)) {
                param = (int)param;
            }
        } else {
            param = void;
        }
        playVoice(getVoice(param));
    }
    
    /**
     * �{�C�X�̒�~
     */
    function stopVoice() {
        voiceEndTime = void;
        if (soundBuffer !== void) {
            soundBuffer.stop();
        }
    }

    /**
     * �{�C�X�҂�
     * @param true �̏ꍇ�X�L�b�v�\�i�f�t�H���g�j
     * @param false�̏ꍇ�X�L�b�v�s�\
     */
    function waitVoice(param) {
        if (voiceEndTime !== void) {
            var waitTime = voiceEndTime - System.getTickCount();
            ret = kag.waitTime(waitTime, param == true && kag.clickSkipEnabled);
        }
    }
    
};

/**
 * ��BGM�I�u�W�F�N�g
 */
class KAGEnvBgm {

    var env;
    
    /**
     * �R���X�g���N�^
     */
    function KAGEnvBgm(env) {
        this.env = env;
    }

    /**
     * �Đ�����
     * @param param �Đ��Ώۃt�@�C��
     */
    function play(param, elm) {
        if (param !== void) {
            var time = +elm.time;
            if (elm.noxchg) {
                if (time > 0)  {
                    kag.bgm.fadeIn(%[ storage:param, loop:elm.loop, time:time, start:elm.start]);
                } else {
                    kag.bgm.play(%[ storage:param, loop:elm.loop, paused:elm.paused, start:elm.start]);
                }
            } else {
                if (time > 0 || +elm.intime > 0) {
                    kag.bgm.exchange(%[ storage:param, loop:elm.loop, time:time, intime:elm.intime, outtime:elm.outtime, overlap:elm.overlap, start:elm.start]);
                } else {
                    kag.bgm.play(%[ storage:param, loop:elm.loop, paused:elm.paused, start:elm.start]);
                }
            }
            kag.clearBgmStop();
            // �Đ����ǃt���O
            kag.sflags["bgm_" + (param.toUpperCase())] = true;
        }
    }

    /**
     * ��~����
     * @param param �t�F�[�h�A�E�g����
     */
    function stop(param, elm) {
        if (+param > 0) {
            kag.bgm.fadeOut(%[ time: +param]);
        } else {
            kag.bgm.stop();
        }
    }

    /**
     * �|�[�Y����
     * @param param �t�F�[�h�A�E�g����
     */
    function pause(param, elm) {
        if (+param > 0) {
            kag.bgm.fadePause(%[ time: +param]);
        } else {
            kag.bgm.pause();
        }
    }

    /**
     * �ĊJ
     */
    function resume(param, elm) {
        kag.bgm.resume();
    }

    /**
     * ���ʃt�F�[�h
     * @param param �t�F�[�h����
     */
    function fade(param, elm) {
        kag.bgm.fade(%[ time:elm.time, volume:param ]);
    }

    var bgmcommands = %[
    tagname : null, 
    play : play incontextof this,
    stop : stop incontextof this,
    pause : pause incontextof this,
    resume : resume incontextof this,
    fade : fade incontextof this,
    noxchg : null,
    loop : null,
    time : null,
    start : null,
    paused : null,
    intime : null,
    outtime : null,
    overlap : null,
        ];

	var doflag;
    
    /**
     * �R�}���h�̎��s
     * @param cmd �R�}���h
     * @param param �p�����[�^
     * @param elm ���̃R�}���h���܂ޑS�p�����[�^
     * @return ���s���s��ꂽ�ꍇ true
     */
    function doCommand(cmd, param, elm) {
        var func;
        if ((func = bgmcommands[cmd]) !== void) {
            if (func != null) {
                func(param, elm);
                doflag = true;
            }
            return true;
        }
        // �Đ��R�}���h�Ƃ݂Ȃ�
        play(cmd, elm);
        doflag = true;
        return true;
    }

    /**
     * KAG �^�O����
     * @param elm ����
     */
    function tagfunc(elm) {
		//dm("BGM �p�t�@���N�V�����Ăяo��!");
		doflag = false;
        foreach(elm, doCommand);
        // �������Ȃ������ꍇ�A���A�^�O���� bgm �łȂ���΂�����Đ�����
        if (!doflag && elm.tagname != "bgm") {
            play(elm.tagname, elm);
        }
        return 0;
    }
    
};

/**
 * ��BGM�I�u�W�F�N�g
 */
class KAGEnvSE {

    var env;
    var id;
    var name;  // �Q�Ƃ��Ă���t�@�C����
    var count; // �Q�Ƃ��ꂽ�J�E���g�l
    
    /**
     * �R���X�g���N�^
     */
    function KAGEnvSE(env, id) {
        this.env = env;
        this.id = id;
    }

    /**
     * �Đ�����
     * @param param �Đ��Ώۃt�@�C��
     */
    function play(param, elm) {
        if (param !== void) {
            name = param;
            var time = +elm.time;
            if (time > 0)  {
                kag.se[id].fadeIn(%[ storage:param, loop:elm.loop, time:time, start:elm.start]);
            } else {
                kag.se[id].play(%[ storage:param, loop:elm.loop, start:elm.start]);
            }
        }
    }

    /**
     * ��~����
     * @param param �t�F�[�h�A�E�g����
     */
    function stop(param, elm) {
        if (+param > 0) {
            kag.se[id].fadeOut(%[ time: +param]);
        } else {
            kag.se[id].stop();
        }
    }

    /**
     * ���ʃt�F�[�h
     * @param param �t�F�[�h����
     */
    function fade(param, elm) {
        kag.se[id].fade(%[ time:elm.time, volume:param ]);
    }

    var secommands = %[
    tagname : null, 
    play : play incontextof this,
    stop : stop incontextof this,
    fade : fade incontextof this,
    loop : null,
    time : null,
    start : null,
        ];

	var doflag;

    /**
     * �R�}���h�̎��s
     * @param cmd �R�}���h
     * @param param �p�����[�^
     * @param elm ���̃R�}���h���܂ޑS�p�����[�^
     * @return ���s���s��ꂽ�ꍇ true
     */
    function doCommand(cmd, param, elm) {
        var func;
        if ((func = secommands[cmd]) !== void) {
            if (func != null) {
                func(param, elm);
				doflag = true;
            }
            return true;
        }
        // �Đ��R�}���h�Ƃ݂Ȃ�
        play(cmd, elm);
		doflag = true;
        return true;
    }

    /**
     * KAG �^�O����
     * @param elm ����
     */
    function tagfunc(elm) {
		//dm("SE �p�t�@���N�V�����Ăяo��!");
		doflag = false;
        foreach(elm, doCommand);
        // �������Ȃ������ꍇ�A���A�^�O���� se �łȂ���΂�����Đ�����
        if (!doflag && elm.tagname != "se") {
            play(elm.tagname, elm);
        }
        return 0;
    }
};


/**
 * ���I�u�W�F�N�g
 */
class KAGEnvironment extends KAGEnvImage {

    /// KAG�{�̂̎Q��
    var kag;

    // �t�F�[�h�w��̃f�t�H���g�l
    // envinit.tjs �Œ�`���邩�A�V�X�e���̃f�t�H���g���g��
    property fadeValue {
        getter() {
            if (envinfo !== void && envinfo.fadeValue !== void) {
                return envinfo.fadeValue;
            } else {
                return 500;
            }
        }
    }
    
    /// ���������
    var envinfo;

    var times;        //< ���ԏ��
    var stages;       //< ������
	var events;       //< �C�x���g�G���
    var positions;    //< �z�u���
    var actions;      //< �A�N�V�������
    var transitions;  //< �g�����W�V�������
    var defaultTime;  //< �f�t�H���g�̎���
    var yoffset;      //< �L�����N�^�z�u��yoffset �l
    var defaultLevel; //< �L�����N�^���x���̃f�t�H���g�l
    var levels;       //< �L�����N�^���x���ʕ␳���
    var faceLevelName;//< �t�F�C�X�E�C���h�E�p�̕\����
	var emotions;
    
    /// ���������̃��C���J�E���g
    var initLayerCount;

    /// ���݂̃��C���J�E���g
    var layerCount;
    
    /// �L�����N�^���ꗗ
    var characterNames;
    /// �L�����N�^���������ꗗ
    var characterInits;
    /// �L�����N�^���
    var characters;
    /// ���C�����
    var layers;
    /// �C�x���g���C��
    var event;

    // BGM �n
    var bgm;
    var origOnBgmStop;
    function onBGMStop() {
        origOnBgmStop(...);
    }

    // SE �n
    var ses;
    
    //�@���ݎ���
    property currentTime {
        getter() {
            if (times !== void && time !== void) {
                return times[time];
            }
        }
    }

    // �����ΏۂɂȂ�f�t�H���g�̃I�u�W�F�N�g
    // ���O�\���̂��Ɛݒ肳���
    var currentObject;
    
    /**
     * �R���X�g���N�^
     * @param kag KAG�{��
     */
    function KAGEnvironment(kag) {

        super.KAGEnvImage(this);
        
        this.kag = kag;
        characters = %[];
        characterInits = %[];
        layers = %[];
        event = new KAGEnvBaseLayer(env, "event");

        // BGM �I�u�W�F�N�g
        bgm = new KAGEnvBgm(env);
        // SE �ێ��p�B�z��
        ses = [];
        for (var i=0; i<kag.numSEBuffers; i++) {
            ses[i] = new KAGEnvSE(env, i);
        }
        
        // �ŏ��̎��s���ɂ��̎��_�ő��݂��Ă��郌�C���̔ԍ��܂ł͑Ώۂ���͂����悤�ɂ���
        initLayerCount = kag.numCharacterLayers;
        
        try {
            // ���������W�J
            envinfo = Scripts.evalStorage("envinit.tjs");
            // �f�o�b�O�\��
            if (envinfo) {
                times       = envinfo.times;       showKeys("times", times);
                stages      = envinfo.stages;      showKeys("stages", stages);
                events      = envinfo.events;      showKeys("stages", events);
                positions   = envinfo.positions;   showKeys("positions", positions);
                actions     = envinfo.actions;     showKeys("actions", actions);
                emotions    = envinfo.emotions;    showKeys("actions", actions);
                transitions = envinfo.transitions; showKeys("transitions", transitions);
                defaultTime = envinfo.defaultTime; dm("defaultTime:" + defaultTime);
                yoffset     = envinfo.yoffset;     dm("yoffset:" + yoffset);
                defaultLevel = envinfo.defaultLevel; dm("defaultLevel:" + defaultLevel);
                levels       = envinfo.levels;
                faceLevelName = envinfo.faceLevelName;
           }
        } catch (e) {
            throw new Exception("���������̃p�[�X�Ɏ��s���܂���(�ڍׂ̓R���\�[���Q��)");
        }

        if (envinfo) {
        
            // �L�����N�^��񏉊�������
            if (envinfo.characters !== void) {
                
                //dm("�L�����N�^��񏉊���");
                
                var chinit = [];
                chinit.assign(envinfo.characters);
                
                //dm("�L�����N�^����" + chinit.count);
                
                for (var i=0; i<chinit.count; i+=2) {
                    var name = chinit[i];
                    var init = chinit[i+1];
                    characterInits[name] = init;
                }
                
            }
        } else {
            dm("����񂪂���܂���");
        }
        
        // KAG �Ɏ������R�}���h�Ƃ��ēo�^
        kag.tagHandlers["env"] = this.tagfunc;
        kag.tagHandlers["begintrans"] = this.beginTrans;
        kag.tagHandlers["endtrans"] = this.endTrans;
        kag.tagHandlers["newlay"]  = this.newLayer;
        kag.tagHandlers["newchar"] = this.newCharacter;

        kag.tagHandlers["dispname"] = this.dispname;
        kag.tagHandlers["endline"]  = this.endline;
		kag.tagHandlers["quake"]    = this.quake;

        kag.unknownHandler = this.unknown;
        kag.seStopHandler  = this.onSeStop;
        
        dm("������������");
    }

    /**
     * �Z�[�u����
     */
    function onStore(f) {
        super.onStore(f);
        f.time = time;
        f.stage = stage;

        var chars = %[];
        foreach(characters, function(name, value, dict) {
            var fch = %[];
            value.onStore(fch);
            this[name] = fch;
        } incontextof chars);
        f.characters = chars;
        
        var lays = %[];
        foreach(layers, function(name, value, dict) {
            var fch = %[];
            value.onStore(fch);
            this[name] = fch;
        } incontextof lays);
        f.layers = lays;

        f.event = %[];
        event.onStore(f.event);
    }

    /**
     * ���[�h����
     */
    function onRestore(f) {
        init();
        time = f.time;
        stage = f.stage;
        if (f.characters) {
            foreach(f.characters, function(name, value, dict) {
                var ch = getCharacter(value.name, value.initName, value.layerId);
                if (ch != null) {
                    ch.onRestore(value);
                }
            } incontextof this);
        }
        if (f.layers) {
            foreach(f.layers, function(name, value, dict) {
                var l = getEnvLayer(value.name, true, value.layerId);
                if (l !== void) {
                    l.onRestore(value);
                }
            } incontextof this);
        }
        if (f.event) {
            event.onRestore(f.event);
        }
        super.onRestore(f);
		updateImage();
    }
    
    /**
     * �t�@�C�i���C�U
     */
    function finalize() {
        init();
        dm("���I��");
        if (kag.unknownHandler === this.unknown) {
            kag.tagHandlers = void;
        }
        invalidate characters;
        invalidate layers;
        invalidate event;
        super.finalize();
    }

    // -----------------------------------------

    /// ����
    var time;
    /// ����
    var stage;
    /// �C�x���g�G
    var event;

    // -----------------------------------------

    /**
     * �����̏�����
     */
    function init() {
        dm("����������");
        time = void;
        stage = void;
        disp       = CLEAR;
        event.disp = CLEAR;
        // �L�����N�^���̔j��
        foreach(characters, function(name,value,dict) {
            invalidate value;
            delete dict[name];
        });
        // ���ꃌ�C�����̔j��
        foreach(layers, function(name,value,dict) {
            invalidate value;
            delete dict[name];
        });
        layerCount = 0;
        kag.allocateCharacterLayers(initLayerCount + layerCount, false);
        transMode = void;
		trans = void;

        // SE ������
        for (var i=0;i<ses.count;i++) {
            ses[i].name  = void;
            ses[i].count = 0;
        }
        seCount = 0;

        // �J�����g�I�u�W�F�N�g������
        currentObject = void;
    }

    /**
     * �S�L�����N�^����
     */
    function hideCharacters() {
        // �L�����N�^���̔j��
        foreach(characters, function(name,value,dict) {
            value.disp = global.KAGEnvImage.CLEAR;
        });
		redraw = true;
    }

    /**
     * �S���C������
     */
    function hideLayers() {
        // ���ꃌ�C�����̔j��
        foreach(layers, function(name,value,dict) {
            value.disp = global.KAGEnvImage.CLEAR;
        });
		redraw = true;
    }

    /**
     * �S�v�f����
     */
    function hideAll() {
        hideCharacters();
        hideLayers();
		redraw = true;
    }

    /**
     * �����ݒ肷��
     * @param stageName ���䖼
     * @param elm �R�}���h�̂ق��̗v�f
     */
    function setStage(stageName, elm) {
        if (stageName != stage || disp == CLEAR) {
            stage = stageName;
            disp = BOTH;

			// �X�e�[�W�ύX���t�b�N
			if (global.setStageHook !== void) {
				global.setStageHook(stageName, elm);
			}

            // ����ύX���̓L�����̗����G����������H

            // �g�����W�V�����w��
            if (!setTrans2(stages[stage].trans)) {
                if (!setTrans2(env.envinfo.stageTrans)) {
                    setTrans2(env.envinfo.envTrans);
                }
            }
        }
        // �C�x���g�G�͏���
        event.disp = CLEAR;
    }

    /**
     * ���Ԃ�ݒ肷��
     * @param timeName ���Ԗ�
     * @param elm �R�}���h�̂ق��̗v�f
     */
    function setTime(timeName, elm) {
        if (timeName != time || disp == CLEAR) {
            time = timeName;
            disp = BOTH;

			// ���ԕύX���t�b�N
			if (global.setTimeHook !== void) {
				global.setTimeHook(timeName, elm);
			}

            // ���ԕύX�̓L�����̗����G���ĕ`��̕K�v������
            foreach(characters, function(name, value, dict) {
                value.setTime();
            });

            // �g�����W�V�����w��
            if (!setTrans2(times[time].trans)) {
                if (!setTrans2(env.envinfo.timeTrans)) {
                    setTrans2(env.envinfo.envTrans);
                }
            }
        }
        // �C�x���g�G�͏���
        event.disp = CLEAR;
    }

    // �g�����W�V�������[�h
    var transMode;

    /**
     * �S�̃g�����W�V�����J�n
     */
    function beginTrans(elm) {
        kag.fore.base.stopTransition();
        kag.backupLayer(%[], true);
        transMode = true;
        return 0;
    }

    /**
     * �S�̃g�����W�V�����I��
     */
    function endTrans(elm) {

        var name = elm.trans;
        ret = void;

        var tr = %[];
        
        // �g�����W�V�����I������
        // ���O�w��ŏ㏑��
        var info;
        if (env.transitions !== void && (info = env.transitions[name]) !== void) {
            // �R�s�[
            (Dictionary.assign incontextof tr)(info, false); 
            // �p�����[�^�̃R�s�[
            foreach(elm, function(name, value, elm, tr) {
                if (transitionParam[name] !== void) {
                    tr[name] = value;
                    //delete elm[name];
                }
            }, tr);
        } else if (transitionName[name] !== void || name.name.substring(0,5) == "trans") {
            // �K��̃g�����W�V����
            // �p�����[�^�̃R�s�[
            foreach(elm, function(name, value, elm, tr) {
                if (transitionParam[name] !== void) {
                    tr[name] = value;
                    //delete elm[name];
                }
            }, tr);
            tr.method = name;
        }
        var trans = tr;
        if (trans.method === void) {
            trans.method = "universal";
        }
        
        if (trans.time === void) {
            trans.time = 1000;
        }
        trans.children = true;

        if (trans.msgoff) {
            kag.invisibleCurrentMessageLayer();
        }
		kag.syncMessageLayer();
        
        // �X�L�b�v������
        if (isSkip()) {
            trans.time = 1;
            kag.fore.base.beginTransition(trans);
            ret = kag.waitTransition(EMPTY);
        } else {
            kag.fore.base.beginTransition(trans);
            if (trans.transwait !== void) {
                ret = kag.waitTime((int)trans.time + (int)trans.transwait, kag.clickSkipEnabled);
            } else {
                ret = kag.waitTransition(EMPTY);
            }
        }

        transMode = false;
        return ret;
    }

    /**
     * �V�K���C������
     */
    function newLayer(elm) {
        var lay = getEnvLayer(elm.name, true);
        if (lay != null) {
            delete elm.tagname;
            delete elm.name;
            return lay.tagfunc(elm);
        }
        return 0;
    }

    /**
     * �V�K�L�����N�^����
     */
    function newCharacter(elm) {
        var ch = getCharacter(elm.name, elm.initname);
        if (ch != null) {
            delete elm.tagname;
            delete elm.name;
            delete elm.initname;
            return ch.tagfunc(elm);
        }
        return 0;
    }        

    var envCommands = %[
    /**
     * �S�̂̏���������
     */
    init : this.init incontextof this,
    stage : this.setStage incontextof this,
    stime : this.setTime incontextof this,
    hidecharacters : this.hideCharacters incontextof this,
    hidelayers : this.hideLayers incontextof this,
    hideall : this.hideAll incontextof this,
    stopallvoice : this.stopAllVoice incontextof this,
        ];
    
    /**
     * �R�}���h�̎��s
     * @param cmd �R�}���h
     * @param param �p�����[�^
     * @param elm ���̃R�}���h���܂ޑS�p�����[�^
     * @return ���s���s��ꂽ�ꍇ true
     */
    function doCommand(cmd, param, elm) {

        //dm("���R�}���h���s:" + cmd);
        
        if (super.doCommand(cmd, param, elm)) {
            return true;
        }
        
        var func;
        if ((func = envCommands[cmd]) !== void) {
            //dm("�o�^�R�}���h������:" + cmd);
            if (func != null) {
                func(param, elm);
            }                
            return true;
        }

        var find = false;

        // ���Ԃƕ���
        var info;
        if (times !== void && (info = times[cmd]) !== void) {
            setTime(cmd, elm);
            find = true;
        } else if (stages !== void && (info = stages[cmd]) !== void) {
            setStage(cmd, elm);
            find = true;
        }

        // �A�N�V����
        if (!find) {
            find = setAction(cmd, elm);
        }

        // �X�V
        if (!find) {
            find = setTrans(cmd, elm);
        }

        return find;
    }

    /**
     * ����摜�̕`��
     * @param layer �`��惌�C��
     */
    function drawStage(layer) {
        var image;
        if (stage !== void) {
            image = stages[stage].image;
            // ���ԏ��ŏ㏑��
            if (time === void) {
                dm("���Ԃ̎w�肪����܂���");
                time = defaultTime;
            }
            if (time !== void) {
                image = image.replace(/TIME/, times[time].prefix);
                // �摜�̃��[�h�ƍ��W�␳����
                try {
                    layer.loadImages(%[ "storage" => image ]);
                } catch(e) {
                    // �摜�����[�h�ł��Ȃ������ꍇ�͕␳�őΉ�
                    image = stages[stage].image;
                    image = image.replace(/TIME/, times[defaultTime].prefix);

                    try {
                        layer.loadImages(%[ "storage" => image ]);
                        
                        // �F�␳����
                        var timeInfo;
                        if ((timeInfo = currentTime) !== void) {
                            // ���C������
                            if (timeInfo.lightColor !== void) {
                                layer.holdAlpha = true;
                                layer.fillOperateRect(0,0,
                                                      layer.width,layer.height,
                                                      timeInfo.lightColor,
                                                      timeInfo.lightType);
                            }
                            // ���x�␳
                            if (timeInfo.brightness !== void) {
                                layer.light(timeInfo.brightness,
                                            timeInfo.contrast);
                            }
                        }
                    } catch (e) {
                        dm("�w�i�摜�����[�h�ł��܂���" + image);
                    }
                }
                var xoff = stages[stage].xoff;
                var yoff = stages[stage].yoff;
                layer.left = (kag.scWidth  / 2) - layer.imageWidth / 2  + (xoff !== void ? xoff : 0);
                layer.top  = (kag.scHeight / 2) - layer.imageHeight / 2 + (yoff !== void ? yoff : 0);

            } else {
                dm("���Ԃ̃f�t�H���g�w�肪���݂��Ă��܂���");
            }
        } else {
            layer.clearImage(true);
        }
    }

    /**
     * ���C�����쐬����
     * @param base
     */
    function getLayer(base) {
        if (base === void) {
            base = env.transMode ? kag.back : kag.fore;
        }
        return base.stage;
    }
    
    function drawAll(base) {
        var layer = getLayer(base);

        drawStage(layer);
        
        // �q�v�f�̍ĕ`��
        foreach(characters, function(name, value, dict, base) {
            value.updateImage(base);
        }, base);
        foreach(layers, function(name, value, dict, base) {
            value.updateImage(base);
        }, base);
        event.updateImage(base);
        updateLayer(layer);
    }
    
    /**
     * �ĕ`�揈��
     */
    function updateImage() {

        kag.updateBeforeCh = 1;

        // �`��X�V���K�v�ȏꍇ
        if (redraw) {
            if (trans) {
                kag.fore.base.stopTransition();
                
                // �X�V�����𑖂点��ꍇ
                if (trans.time === void) {
                    trans.time = 1000;
                }
                trans.children = true;

                // �S���C�����o�b�N�A�b�v
                kag.backupLayer(%[], true);
                
                // �X�e�[�W�̕`��
                drawAll(kag.back);
                
                // �g�����W�V�������s
                kag.fore.base.beginTransition(trans);
                if (ret == 0) {
                    ret = kag.waitTransition(EMPTY);
                } else {
                    kag.waitTransition(EMPTY);
                }
                
            } else {

                var layer = getLayer();

                // �t�F�[�h����
                // ���ɕ\������Ă�Ƃ��̓g�����W�V�����ő�p
                if (fadeTime !== void && isShowBU() && layer.visible) {

                    kag.fore.base.stopTransition();
                    var trans = %[ "method" => "crossfade",
                                   "children" => true,
                                   "time" => fadeTime > 1 ? fadeTime : fadeValue];
                    fadeTime = void;

                    // �S���C���𗠂Ƀo�b�N�A�b�v
                    kag.backupLayer(%[], true);

                    // �����C�����Ώ�
                    drawAll(kag.back);
                    
                    // �g�����W�V�������s
                    kag.fore.base.beginTransition(trans);
                    if (ret == 0) {
                        ret = kag.waitTransition(EMPTY);
                    } else {
                        kag.waitTransition(EMPTY);
                    }

                } else {
                    drawAll();
                }
            }
            redraw = false;
        } else {
            updateLayer(getLayer());
        }
        trans = void;
    }

    /**
     * KAG �^�O����
     * @param elm ����
     */
    function tagfunc(elm) {
        //dm("���^�O����΂ꂽ��");
        var ret;
        foreach(elm, doCommand);
        hideMessage();
        updateImage();
        return ret;
    }

    /**
     * �V�K�Ƀ��C��ID ���擾����
     * @param layerId �w�肳�ꂽ���C��ID
     */
    function getLayerId(layerId) {
        if (layerId !== void) {
            if (layerId >= layerCount) {
                layerCount = layerId + 1;
            }
        } else {
            layerId = layerCount++;
        }
        kag.allocateCharacterLayers(env.initLayerCount + layerCount, false);
        return layerId;
    }
    
    /**
     * �w�肳�ꂽ���O�̃L�����N�^��Ԃ�
     * @param name ���O
     * @param initName ��������
     * @param layerId ���C��ID
     */
    function getCharacter(name, initName, layerId) {
        var ch = characters[name];
        if (ch === void) {
            if (initName === void) {
                initName = name;
            }
            var init;
            if ((init = characterInits[initName]) !== void) {
                layerId = getLayerId(layerId);
                ch = new KAGEnvCharacter(this, layerId, name, initName, init);
                characters[name] = ch;
            }
        }
        return ch;
    }

    /**
     * �w�肳�ꂽ���O�̃��C����Ԃ�
     * @param name ���O
     * @param create �������[�h
     */
    function getEnvLayer(name, create=false, layerId) {
        var lay = layers[name];
        if (lay === void && create) {
            layerId = getLayerId(layerId);
            lay = new KAGEnvSimpleLayer(this, layerId, name);
            layers[name] = lay;
        }
        return lay;
    }

    /**
     * �S�L�����̃{�C�X���~����
     */
    function stopAllVoice() {
        foreach(characters, function(name, value, dict) {
            value.stopVoice();
        });
    }

    /*
     * �w�肵���L�����Ŏw�肵���{�C�X�t�@�C�����Đ�
     */
    function playVoice(name, voicename) {
        stopAllVoice();
        var ch = getCharacter(name);
        if (ch !== void && voicename !== void) {
            return ch.playVoice(voicename);
        }
    }


    function quake(elm)	{
        // �h����̂��Ƃ�
        if (!isSkip()) {
            kag.doQuake(elm);
        }
        return 0;
	}
    
    /**
     * �s�I�������n���h��
     */
    function endline(elm) {
        if (kag.historyWriteEnabled) {
            kag.historyLayer.clearAction();
            kag.historyLayer.reline();
        }
    }

    /**
     * �\���������
     */
    function clearFace() {
        if (envinfo.clearFace !== void) {
            kag.current.faceLayer.loadImages(envinfo.clearFace);
            kag.current.faceLayer.visible = true;
        } else {
            kag.current.clearFace();
        }
    }

    // �w�肳�ꂽ�L�����N�^�̕\��\���\�Ȃ�\������
    function drawFace(page, ch) {
        if (faceLevelName !== void && currentObject === ch) {
            if (ch !== void && ch.isShowFace()) {
                ch.drawFace(page == 0
                            ? kag.fore.messages[kag.currentNum].faceLayer
                            : kag.back.messages[kag.currentNum].faceLayer,
                            faceLevelName);
            } else {
                clearFace();
            }
        }
    }
    
    /**
     * ���O�\�������n���h��
     */
    function dispname(elm) {

        if (kag.sflags.voicecut) {
            stopAllVoice();
        }

        //dm("���O�\���n���h��");
        if (elm === void || elm.name === void || elm.name == "") {
            // ���O�\�����������W�b�N
            // ���O����
            kag.current.clearName();
            if (faceLevelName !== void) {
                clearFace();
            }
            // �ǉ�wait�͂Ȃ�
            kag.addAutoWait();

            // �J�����g�I�u�W�F�N�g�̉���
            currentObject = void;
            
        } else {

            var name = elm.name;
            var disp = elm.disp;

            var ch = getCharacter(name);
            var voice;

            if (kag.historyWriteEnabled && ch !== void && (voice = ch.getCurrentVoice()) !== void) {
                kag.historyLayer.setNewAction("global.world_object.env.playVoice(\"" + name + "\",\"" + voice + "\")");
            }
            
            // ���O�\������
            var dispName;
            if (disp !== void && disp != "") {
                dispName = disp;
            } else {
                if (ch !== void && ch.init.nameAlias !== void) {
                    dispName = Scripts.eval(ch.init.nameAlias);
					if (dispName === void) {
						dispName = name;
					}
                } else {
                    dispName = name;
                }
            }

// XXX ���O�␳�B�O�����ɂ��邱��!
//            if (dispName.length <= 1) {
//                dispName = "�@" + dispName + "�@";
//            }
//            var dispName = "�y" + dispName + "�z";


            kag.current.processName(dispName);
            if (kag.historyWriteEnabled) {
				if (typeof kag.historyLayer.storeName !== 'undefined') {
                    kag.historyLayer.storeName(dispName);
				} else {
					kag.historyLayer.store(dispName + " ");
                }
			}

            // ������ԂȂ��\����Ԃɂ���
            if (ch !== void && ch.disp == CLEAR) {
                ch.disp = FACE;
            }
            
            // �\��ύX����
            //if (currentObject !== ch) {
                currentObject = ch;
                drawFace(kag.currentPage, ch);
			//}
            
            // �{�C�X�Đ�
            if (ch !== void) {
                // �{�C�X�Đ�����
                kag.addAutoWait(ch.playVoice());
            } else {
                kag.addAutoWait();
            }

        }
        return 0;
    }

    var seCount = 0;
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
     * SE ��~���̏���
     * ��~����Ԃɂ���
     */
    function onSeStop(id) {
        if (id < ses.count) {
            ses[id].name = void;
        }
    }
    
    /**
     * SE �� ID �����肷��
     * @param buf �o�b�t�@ID���w��
     */
    function getSeId(buf) {
        // ���ڃo�b�t�@���w�肳��Ă���ꍇ�͂����Ԃ�
        if (buf !== void && +buf < ses.count) {
            return +buf;
        }
        // �g���ĂȂ����̂�������
        var max = seCount;
        var old = void;
        for (var i=0; i<ses.count; i++) {
            if (ses[i].name == void) {
                return i;
            }
            if (ses[i].count < max) {
                max = ses[i].count;
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
        for (var i=0; i<ses.count; i++) {
            if (ses[i].name == name) {
                return i;
            }
        }
        // �݂���Ȃ��̂ł����Ă���ԍ���Ԃ�
        return getSeId();
    }
    
    /**
     * KAG �s�������n���h��
     */
    function unknown(tagName, elm) {

        //dm("�s���R�}���h�n���h��:" + tagName);

        // ���̃R�}���h���H
        if (doCommand(tagName, true, elm)) {
            // �c������R�}���h�Ƃ��Ď��s����
            ret = void;
            elm.tagname = "env";
            foreach(elm, doCommand);
            showKeys("trans", trans);
            hideMessage();
            updateImage();
            return ret;
        }

        // �C�x���g�p����
        if (tagName == "event" || tagName == "ev") {
            return event.tagfunc(elm);
        } else if (tagName.substring(0,2) == "ev") {
            elm[tagName] = true;
            return event.tagfunc(elm);
        }

        // BGM �����p
        if (tagName == "bgm") {
            return bgm.tagfunc(elm);
        } else if (tagName.substring(0,3) == "bgm") {
            return bgm.tagfunc(elm);
        }

        // SE �����p
        if (tagName == "se") {
            return getSe(getSeId(elm.buf)).tagfunc(elm);
        } else if (tagName.substring(0,2) == "se") {
            var se = getSe(getSeIdFromName(tagName));
            return se.tagfunc(elm);
        }
        
        // �Y���L�����N�^�����݂��邩�H
        var ch = getCharacter(tagName);
        if (ch !== void) {
            return ch.tagfunc(elm);
        }

        // �Y�����C�������݂��邩�H
        var lay = getEnvLayer(tagName);
        if (lay !== void) {
            return lay.tagfunc(elm);
        }
        
        // �J�����g�I�u�W�F�N�g�̃R�}���h�Ƃ��Ď��s���Ă݂�
        if (currentObject !== void) {
            elm[tagName] = true;
            return currentObject.tagfunc(elm);
        }
    }
};

KAGEnvironment.XPOSITION    = 1;
KAGEnvironment.LEVEL        = 2;
KAGEnvironment.DISPPOSITION = 3;
KAGEnvironment.YPOSITION    = 4;

KAGEnvImage.BOTH      = 1;
KAGEnvImage.BU        = 2;
KAGEnvImage.CLEAR     = 3;
KAGEnvImage.FACE      = 4;
KAGEnvImage.INVISIBLE = 5;

/**
 * KAG �p���[���h�g���v���O�C��
 */
class KAGWorldPlugin extends KAGPlugin
{
	var env;
    function KAGWorldPlugin(kag) {
		super.KAGPlugin();
        env = new KAGEnvironment(kag);
    }

    function finalize() {
        if (env) {
            invalidate env;
        }
    }

	function onStore(f, elm)
	{
		var dic = f.env = %[];
		env.onStore(dic);
	}

	function onRestore(f, clear, elm)
	{
		if (f.env !== void) {
			env.onRestore(f.env);
		}
	}
};

kag.addPlugin(global.world_object = new KAGWorldPlugin(kag));

dm("���[���h���ݒ芮��");

@endscript
@endif

@return
