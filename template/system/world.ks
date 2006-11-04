@if exp="typeof(global.world_object) == 'undefined'"
@iscript

KAGLoadScript('YAML.tjs');

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
function getTo(value)
{
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

function toNumber(value)
{
    if (value === void || value === null) {
        return value;
    } else {
        return +value;
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
    "time" => true,
    "delay" => true,
    "x" => true,
    "y" => true,
    "toptime" => true,
    "vibration" => true,
    "waittime" => true,
    "cycle" => true,
    "distance" => true,
    "falltime" => true,
    "zoom" => true,
    "angvel" => true,
    "angle" => true,
    "showtime" => true,
    "hidetime" => true,
    "intime" => true,
    "outtime" => true,
    "opacity" => true,
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

    // �X�L�b�v��Ԃ�
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
    var _type;
    var doType;
    property type {
        getter() {
            return _type;
        }
        setter(v) {
            _type = v;
            doType = (v !== void);
        }
    }

    // �����x
    var opacityFrom;
    var opacityTime;
    var _opacity;
    var doOpacity;
    property opacity {
        getter() {
            return _opacity;
        }
        setter(v) {
            _opacity = v;
            doOpacity = (v !== void);
        }
    }

    // ��]
    var rotateFrom;
    var rotateTime;
    var _rotate;
    var doRotate;
    property rotate {
        getter() {
            return _rotate;
        }
        setter(v) {
            _rotate = v;
            doRotate = (v !== void);
        }
    }

    // �Y�[������
    var zoomFrom;
    var zoomTime;
    var _zoom;
    var doZoom;
    property zoom {
        getter() {
            return _zoom;
        }
        setter(v) {
            _zoom = v;
            doZoom = (v !== void);
        }
    }

    // ��]���_�w��
    var _afx;
    var _afy;
    var doAffine;
    property afx {
        getter() {
            return _afx;
        }
        setter(v) {
            _afx = v;
            doAffine = (v !== void);
        }
    }
    property afy {
        getter() {
            return _afy;
        }
        setter(v) {
            _afy = v;
            doAffine = (v !== void);
        }
    }

    // �\���ʒu���W
    var xpos;
    var ypos;
    var xposFrom;
    var yposFrom;
    var moveTime;
    var moveAccel;

    // �ʒu�ύX
    var reposition;
    
    // �A�N�V��������
    var doStopAction;
    var actionList;
    var currentActionList;
    var syncMode;

    // �摜�␮�w��
    var _grayscale;
    property grayscale {
        getter() {
            if (env.colorall) {
                return env.grayscale;
            } else {
                return _grayscale;
            }
        }
        setter(v) {
            _grayscale = v;
            if (!env.colorall) {
                redraw = true;
            }
        }
    }

    var _rgamma;
    property rgamma {
        getter() {
            if (env.colorall) {
                return env.rgamma;
            } else {
                return _rgamma;
            }
        }
        setter(v) {
            _rgamma = v;
            if (!env.colorall) {
                redraw = true;
            }
        }
    }

    var _ggamma;
    property ggamma {
        getter() {
            if (env.colorall) {
                return env.ggamma;
            } else {
                return _ggamma;
            }
        }
        setter(v) {
            _ggamma = v;
            if (!env.colorall) {
                redraw = true;
            }
        }
    }
    
    var _bgamma;
    property bgamma {
        getter() {
            if (env.colorall) {
                return env.bgamma;
            } else {
                return _bgamma;
            }
        }
        setter(v) {
            _bgamma = v;
            if (!env.colorall) {
                redraw = true;
            }
        }
    }

    function resetColor(param, elm) {
        _grayscale = void;
        _rgamma = void;
        _ggamma = void;
        _bgamma = void;
        if (!env.colorall) {
            redraw = true;
        }
    }
    
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
        actionList = new Array();
        currentActionList = new Array();
        _disp = CLEAR;
    }

    function initImage() {
        type = void;
        opacityFrom = void;
        opacityTime = void;
        opacity = void;
        rotateFrom = void;
        rotateTime = void;
        rotate = void;
        zoomFrom = void;
        zoomTime = void;
        zoom = void;
        afx = void;
        afy = void;
        xpos     = void;
        ypos     = void;
        xposFrom = void;
        yposFrom = void;
        moveTime = void;
        moveAccel = void;
        reposition = false;

        rgamma = void;
        ggamma = void;
        bgamma = void;
        grayscale = void;
        
        resetFlag = false;
        fadeTime = void;
        actionList.clear();
        currentActionList.clear();
        _disp = CLEAR;
    }

    function initLayer(layer) {
        layer.reset();
        if (!doType) {
            _type    = void;
        }
        if (!doOpacity) {
            _opacity = void;
        }
        if (!doRotate) {
            _rotate = void;
        }
        if (!doZoom) {
            _zoom = void;
        }
        if (!doAffine) {
            _afx = void;
            _afy = void;
        }
        clearAction();
    }
    
    function finalize() {
        clearAction();
        invalidate actionList;
        invalidate currentActionList;
    }

    function setXPos(cmd, elm) {
        xposFrom = getFrom(cmd);
        xpos     = getTo(cmd);
        if (moveTime === void) {
            moveTime  = elm.time;
            moveAccel = elm.accel;
        }
        //dm("X�ʒu�w��:", xpos, xposFrom, moveTime);
        reposition = true;
    } 

    function setYPos(cmd, elm) {
        yposFrom = getFrom(cmd);
        ypos     = getTo(cmd);
        if (moveTime === void) {
            moveTime  = elm.time;
            moveAccel = elm.accel;
        }
        //dm("Y�ʒu�w��:", ypos, yposFrom, moveTime);
        reposition = true;
    }

    /**
     * �A�N�V�������̒ǉ�
     */
    function addAction(action) {
        // �K��̃��W���[���͔r������
        if (action.module == "LayerFadeToModeModule") {
            opacity     = action.opacity;
            opacityTime = action.time;
        } else if (action.module == "LayerToRotateModule") {
            rotate = action.angle;
            rotateTime = action.time;
        } else if (action.module == "LayerNormalZoomModule") {
            zoom = action.zoom;
            zoomTime = action.time;
        } else if (action.module == "LayerAccelMoveModule") {
            xpos = action.x;
            ypos = action.y;
            moveTime = action.time;
            moveAccel = 1;
            reposition = true;
        } else if (action.module == "LayerNormalMoveModule") {
            xpos = action.x;
            ypos = action.y;
            moveTime = action.time;
            moveAccel = 0;
            reposition = true;
        } else if (action.module == "LayerDecelMoveModule") {
            xpos = action.x;
            ypos = action.y;
            moveTime = action.time;
            moveAccel = -1;
            reposition = true;
        } else {
            actionList.add(action);
        }
    }

    /**
     * �A�N�V�������̏���
     */
    function clearAction() {
        currentActionList.clear();
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
                if (info instanceof "Dictionary") {
                    var action = %[];
                    (Dictionary.assign incontextof action)(info, false); 
                    foreach(elm, function(name, value, elm, action) {
                        if (actionParam[name] !== void) {
                            action[name] = value;
                        }
                    }, action);
                    addAction(action);
                    return true;
                } else if (info instanceof "Array") {
                    for (var i=0;i<info.count;i++) {
                        var action = %[];
                        (Dictionary.assign incontextof action)(info[i], false); 
                        foreach(elm, function(name, value, elm, action) {
                            if (actionParam[name] !== void) {
                                action[name] = value;
                            }
                        }, action);
                        addAction(action);
                    }
                    return true;
                }
                return false;
            }
            if (elm !== void && name.substring(0,5) == "Layer") {
                var moduleType;
                try {
                    moduleType = global[name];
                    if (moduleType !== void && typeof moduleType == "Object" && moduleType.LayerModule != void) {
                        // �A�N�V�������W���[�����̏ꍇ
                        var action = %[];
                        foreach(elm, function(name, value, elm, action) {
                            if (actionParam[name] !== void) {
                                action[name] = value;
                            }
                        }, action);
                        action.module = name;
                        action.time = elm.time if elm.time !== void;
                        if (action.time !== void) {
                            action.time *= kag.drawspeed;
                        }
                        addAction(action);
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
     * �A�N�V����������
     */
    function stopAction() {
        clearAction();
        doStopAction = true;
    }
    
    /**
     * �g�����W�V�������̎擾
     */
    function getTrans(name, elm) {
        var tr = %[];
        // ���O�w��ŏ㏑��
        var info;
        if (env.transitions !== void && (info = env.transitions[name]) !== void) {
            // �R�s�[
            (Dictionary.assign incontextof tr)(info, false); 
            // �p�����[�^�̃R�s�[
            foreach(elm, function(name, value, elm, tr) {
                if (transitionParam[name] !== void) {
                    tr[name] = value;
                }
            }, tr);
        } else if (elm != null && (transitionName[name] !== void || name.substring(0,5) == "trans")) {
            // �K��̃g�����W�V����
            // �p�����[�^�̃R�s�[
            foreach(elm, function(name, value, elm, tr) {
                if (transitionParam[name] !== void) {
                    tr[name] = value;
                }
            }, tr);
            tr.method = name;
        }
        return tr;
    }
    
    /**
     * �g�����W�V������ݒ�
     * @param name �g�����W�V������
     */
    function _setTrans(name, elm) {
        var tr = getTrans(name, elm);
        if (tr.method !== void) {
            if (trans === void) {
                if (!env.transMode && !isSkip()) {
                    trans = tr;
                } else {
                    // �����ȃg�����W�V�������w��
                    trans = %[];
                }
            }
            redraw = true;
            return true;
        }
        return false;
    }

    function setTrans(name, elm) {
        if (name == "void") {
            trans = %[];
        } else {
            _setTrans(name, elm);
        }
    }
    
    /**
     * �g�����W�V������ݒ�
     * @param param �g�����W�V�����w��@������܂��͎���
     * @return �ݒ肵���ꍇ��
     */
    function setTrans2(param) {
        if (trans == void) {
            //dm("�g�����W�V�����ݒ�2:" + param);
            if (param === void) {
                return false;
            } else if (typeof param == "String") {
                _setTrans(param);
                return true;
            } else if (param instanceof "Dictionary") {
                _setTrans(param.method, param);
                return true;
            }
            return false;
        } else {
            return true;
        }
    }
    
    /**
     * ��ԍX�V����
     */
    function updateLayer(layer) {
        if (resetFlag) {
            layer.reset();
            _type    = void;
            _opacity = void;
            _rotate = void;
            _zoom = void;
            _afx = void;
            _afy = void;
            clearAction();
            _rgamma = void;
            _ggamma = void;
            _bgamma = void;
            _grayscale = void;
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

            if (doAffine) {
                layer.afx = afx;
                layer.afy = afy;
                doAffine = false;
            }
            if (doOpacity) {
                //dm("�����x�ύX:" + opacity + ":" + opacityTime); 
                if (opacityFrom !== void) {
                    layer.opacity = opacityFrom;
                    opacityFrom = void;
                }
                layer.setOpacityTime(opacity, opacityTime);
                doOpacity = false;
            }
            if (doRotate) {
                //dm("��]�ύX:" + rotate + ":" + rotateTime);
                if (rotateFrom !== void) {
                    layer.rotate = rotateFrom;
                    rotateFrom = void;
                }
                layer.setRotateTime(rotate, rotateTime);
                doRotate = false;
            }
            if (doZoom) {
                //dm("�Y�[���ύX:" + zoom + ":" + zoomTime);
                if (zoomFrom !== void) {
                    layer.zoom = zoomFrom;
                    zoomFrom = void;
                }
                layer.setZoomTime(zoom, zoomTime);
                doZoom = false;
            }
            if (doType) {
                //dm("������ʕύX");
                layer.type = type;
                doType = false;
            }

            if (doStopAction) {
                layer.stopAction();
                doStopAction = false;
            }
            // �A�N�V��������
            if (actionList.count > 0) {
                for (var i=0;i<actionList.count;i++) {
                    var action = actionList[i];
                    layer.beginAction(action);
                    if (action.time == void || action.time == 0) {
                        currentActionList.add(action);
                    }
                }
                actionList.clear();
            }
        }
        if (syncMode) {
            ret = kag._waitLayerAction(layer);
            //dm("�A�N�V�����҂�:" + ret);
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
        opacityFrom = toNumber(getFrom(param));
        opacity     = toNumber(getTo(param));
        opacityTime = isSkip() ? 0 : elm.time;
    } incontextof this,
    fade :  function(param, elm) {
        fadeTime = isSkip() || env.transMode ? void : param;
    } incontextof this,
    rotate : function(param, elm) {
        rotateFrom = toNumber(getFrom(param));
        rotate     = toNumber(getTo(param));
        rotateTime = isSkip() ? 0 : elm.time;
    } incontextof this,
    zoom : function(param, elm) {
        zoomFrom = toNumber(getFrom(param));
        zoom     = toNumber(getTo(param));
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
    grayscale : function(param, elm) {
        grayscale = param;
    } incontextof this,
    rgamma : function(param, elm) {
        rgamma = param;
    } incontextof this,
    ggamma : function(param, elm) {
        ggamma = param;
    } incontextof this,
    bgamma : function(param, elm) {
        bgamma = param;
    } incontextof this,
    resetcolor : this.resetColor incontextof this,
    trans  : setTrans incontextof this,
    action : setAction incontextof this,
    stopaction : stopAction incontextof this,

    sync : function(param) { if (param) { syncMode = true; } } incontextof this,

    show : function(param) { disp = BOTH;   } incontextof this,
    hide : function(param) { disp = CLEAR; } incontextof this,
    visible : function(param) { disp = param ? BOTH : CLEAR; }  incontextof this,
    xpos : this.setXPos incontextof this,
    ypos : this.setYPos incontextof this,
    accel : null, // ����
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
        f.type    = type;
        f.opacity = opacity;
        f.rotate  = rotate;
        f.zoom    = zoom;
        f.xpos    = xpos;
        f.ypos    = ypos;
        f.disp    = disp;
        f.actionList = [];
        (Array.assign incontextof f.actionList)(currentActionList);
        //for (var i=0;i<currentActionList.count;i++) {
        //    dm("�A�N�V�����ۑ�:" + currentActionList[i].module);
        //}

        f.grayscale = _grayscale;
        f.rgamma    = _rgamma;
        f.ggamma    = _ggamma;
        f.bgamma    = _bgamma;
    }

    /**
     * ���[�h����
     */
    function onRestore(f) {

        type = f.type;
        opacity     = f.opacity;
        opacityFrom = void;
        opacityTime = void;
        rotate      = f.rotate;
        rotateFrom  = void;
        rotateTime  = void;
        zoom        = f.zoom;
        zoomFrom    = void;
        zoomTime    = void;
        if (f.actionList !== void) {
            for (var i=0;i<f.actionList.count;i++) {
                addAction(f.actionList[i]);
                //dm("�A�N�V�������A:" + f.actionList[i].module);
            }
        }
        xpos      = f.xpos;
        xposFrom  = void;
        ypos      = f.ypos;
        yposFrom  = void;
        moveTime  = void;
        moveAccel = void;
        reposition = true;

        _grayscale = f.grayscale;
        _rgamma    = f.rgamma;
        _ggamma    = f.ggamma;
        _bgamma    = f.bgamma;
        if (_grayscale != void ||
            _rgamma    != void ||
            _ggamma    != void ||
            _bgamma    != void) {
            redraw = true;
        }
        
        disp = f.disp;
    }

    // ���̃��\�b�h����������
    // function getLayer(base);
    // function drawLayer(layer);

    /**
     * ���C���z�u����(�W��)�F���㌴�_
     * @param layer �����Ώۃ��C��
     */
    function calcPosition(layer) {
        if (reposition) {
            //dm("�ʒu�w��1",xpos,ypos);
            var l = (int)xpos;
            var t = (int)ypos;
            if (moveTime !== void && moveTime > 0) {
                if (xposFrom !== void || yposFrom !== void) {
                    var fl = xposFrom !== void ? (int)xposFrom : l;
                    var ft = yposFrom !== void ? (int)yposFrom : t;
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

    // �g�����W�V�������s
    function beginTransition(trans) {
        if (trans.time === void || isSkip()) {
            trans.time = 0;
        }
        trans.children = true;
        // ���������荞�܂���
        trans.tagname = "trans";
        kag.conductor.pendings.insert(0, %[ tagname : "syncmsg" ]);
        if (trans.transwait !== void) {
            kag.conductor.pendings.insert(0, %[ tagname : "wait", time : (int)trans.time + (int)trans.transwait, trans:true ]);
        } else {
            kag.conductor.pendings.insert(0, %[ tagname : "wt" ]);
        }
        kag.conductor.pendings.insert(0, trans);
    }

    /**
     * ���b�Z�[�W����������
     */
    function hideMessage() {
        if (trans !== void && trans.msgoff) {
            kag.conductor.pendings.insert(0, %[ tagname : "er", all:true]);
            if (kag.setCurrentMessageLayerVisible(false)) {
                ret = -2;
            }
        }
    }
    
    /**
     * �摜�̕`��
     */
    function updateImage(base) {
        kag.updateBeforeCh = 1;
        // �`��X�V���K�v�ȏꍇ
        if (redraw) {
            
            if (base === void && trans !== void && trans.method !== void) {

                kag.fore.base.stopTransition();
                
                // �S���C�����o�b�N�A�b�v
                kag.backupLayer(EMPTY, true);

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
                // ���ɕ\������Ă�Ƃ���\��݂̂̏ꍇ�̓g�����W�V�����ő�p
                if (fadeTime !== void /**&& ((isShowBU() && layer.visible && layer.opacity > 0) || _disp == FACE)*/) {

                    kag.fore.base.stopTransition();
                    var trans = %[ "method" => "crossfade",
                                   "children" => true,
                                   "time" => fadeTime > 1 ? fadeTime : fadeValue];
                    fadeTime = void;

                    // �S���C���𗠂Ƀo�b�N�A�b�v
                    kag.backupLayer(EMPTY, true);

                    // �����C�����Ώ�
                    layer = getLayer(kag.back);
                    if (isShowBU()) {
                        drawLayer(layer);
                        calcPosition(layer);
                    }
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

    function KAGEnvLayer(env) {
        super.KAGEnvImage(env);
        redraw = false;
    }

    function onStore(f) {
        super.onStore(f);
        f.imageFile = imageFile;
    }
    
    function onRestore(f) {
        imageFile = f.imageFile;
        super.onRestore(f);
    }

    function setImageFile(file, elm) {

        imageFile = file;
        disp = BOTH;

        // �w�i�w�莞�ɍ��W�w�肪�Ȃ���Ώꏊ��������������
        if (elm.xpos === void) {
            xpos = void;
        }
        if (elm.ypos === void) {
            ypos = void;
        }
        reposition = true;

        // �L�^
        kag.sflags["cg_" + (file.toUpperCase())] = true;

        return true;
    }

    var _layerCommands = %[
    file : function(param, elm) {
        setImageFile(param, elm);
    } incontextof this,
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
            find = _setTrans(cmd, elm);
        }

        // �摜�̃��[�h
        if (!find) {
            find = setImageFile(cmd, elm);
        }
        return find;
    }

    // ���̃��\�b�h����������
    // function getLayer(base);
    // 
    function drawLayer(layer) {
        initLayer(layer);
        if (imageFile !== void) {
            try {
                layer.loadImages(%[ "storage" => imageFile]);
            } catch (e) {
                dm("�摜�̓ǂݍ��݂Ɏ��s���܂���:" + imageFile);
            }
        }
        if (grayscale) {
            layer.doGrayScale();
        }
        if (rgamma != void || ggamma != void || bgamma != void) {
            layer.adjustGamma(rgamma == void ? 1.0 : rgamma, 0, 255,
                              ggamma == void ? 1.0 : ggamma, 0, 255,
                              bgamma == void ? 1.0 : bgamma, 0, 255);
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

    /**
     * ���ԗp�ŏ���������
     */
    function setRedraw() {
        // �\�����̏ꍇ�͍ĕ`�揈��
        var layer = kag.fore[name];
        if (layer.visible) {
            redraw = true;
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

    function setImageFile(file, elm) {

        var eventTrans;
        
        if (file !== void) {

            var eventInfo;
            if (env.events !== void) {
                eventInfo = env.events[file];
            }
            if (eventInfo !== void) {
                eventTrans = eventInfo.trans;
                imageFile = eventInfo.image !== void ? eventInfo.image : v;
                xpos = elm.xpos !== void ? (int)elm.xpos : eventInfo.xoff;
                ypos = elm.ypos !== void ? (int)elm.ypos : eventInfo.yoff;
                reposition = true;
            } else {
                imageFile = file;
                xpos = elm.xpos !== void ? (int)elm.xpos : void;
                ypos = elm.ypos !== void ? (int)elm.ypos : void;
                reposition = true;
            }

            disp = BOTH;
            reposition = true;
            
            // �L�^
            kag.sflags["cg_" + (file.toUpperCase())] = true;
            
            // �g�����W�V�����w��
            if (!setTrans2(eventTrans)) {
                if (!setTrans2(env.envinfo.eventTrans)) {
                    setTrans2(env.envinfo.envTrans);
                }
            }

        } else {
            imageFile = void;
            disp = CLEAR;
            xpos = elm.xpos !== void ? (int)elm.xpos : void;
            ypos = elm.ypos !== void ? (int)elm.ypos : void;
            reposition = true;
        }

        return true;
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

    /// �ʒu�ύX
    var front;
    var back;

    /**
     * ���ԗp�ŏ���������
     */
    function setRedraw() {
        // �\�����̏ꍇ�͍ĕ`�揈��
        var layer = kag.fore.layers[env.initLayerCount + layerId];
        if (layer.visible) {
            redraw = true;
        }
    }
    
    /**
     * �R���X�g���N�^
     * @param layerId �O�i���C��ID
     */
    function KAGEnvLevelLayer(layerId) {
        this.layerId = layerId;
        level = void;
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

    function onStore(f) {
        f.layerId = layerId;
        f.level = level;
        f.absolute = absolute;
    }

    function onRestore(f) {
        layerId = f.layerId;
        level = f.level;
        absolute = f.absolute;

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
    
    /**
     * ���C���z�u����(�����C���p)�F�������_
     * @param layer �����Ώۃ��C��
     */
    function calcPosition(layer) {
        if (reposition) {
            //dm("�ʒu�w��2");
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
    front : this.setFront incontextof this,
    back : this.setBack incontextof this,
    level : this.setLevel incontextof this,
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

    var facePoseMap;
    
    /// �{�C�X���
    var voice;
    var strVoice;
    var incVoice;   // true �Ȃ當����w�莞���J�E���g����
    var noincVoice; // true �Ȃ�J�E���g���Ȃ�
    
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
            
            // �\��|�[�Y�����@�\
            if (init.facePose) {
                // �\���|�[�Y�ɑ΂���}�b�v���쐬����
                facePoseMap = %[];
                foreach(poses, function(name, value, dict, facePoseMap) {
                    var faces = [];
                    faces.assign(value.faces);
                    for (var i=0; i<faces.count; i+= 2) {
                        facePoseMap[faces[i]] = name;
                    }
                }, facePoseMap);
            }

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

    // �\��`�揈�����s��
    var redrawFace;
    
    /**
     * �|�[�Y�̐ݒ�
     */
    function setPose(poseName, elm) {
        var info = poses[poseName];
        if (info !== void) {
            if (poseName != pose || disp == CLEAR) {
                pose = poseName;
                if (disp == CLEAR) {
                    disp = init.noPose ? FACE : BOTH;
                    reposition = true;
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
                
                // �\��\���擾
                if (isShowFace() && (elm.faceon || env.showFaceMode) && !elm.faceoff) {
                    redrawFace = true;
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
    function setDress(dressName, elm) {
        if (dressName != dress || disp == CLEAR) {
            dress = dressName;
            if (disp == CLEAR) {
                disp = init.noPose ? FACE : BOTH;
                reposition = true;
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
            // �\��\���擾
            if (isShowFace() && (elm.faceon || env.showFaceMode) && !elm.faceoff) {
                redrawFace = true;
            }
        }
    }
    /**
     * �\��̐ݒ�
      */
    function setFace(faceName, elm) {
        if (faceName != face || disp == CLEAR) {
            face = faceName;
            if (disp == CLEAR) {
                disp = init.noPose ? FACE : BOTH;
                reposition = true;
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

            // �\��\���擾
            if (isShowFace() && (elm.faceon || env.showFaceMode) && !elm.faceoff) {
                redrawFace = true;
            }
        }
    }

    /**
     * �ꏊ�\���p�g�����W�V�����ݒ�
     */
    function setPositionTrans(info, force=false) {
        // �g�����W�V�����w��
        if (force || isShowBU()) {
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
                disp = init.noPose ? FACE : BOTH;
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
            reposition = true;
            setPositionTrans(info, true);
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
    face    : function(cmd,elm) {
        if (facePoseMap !== void) {
            var p;
            if ((p = facePoseMap[cmd]) !== void) {
                setPose(p,elm);
                setFace(cmd,elm);
            }
        } else {
            setFace(cmd,elm);
        }
    }incontextof this,
    pos     : this.setPosition incontextof this,
    emotion : this.setEmotion incontextof this,
    xpos    : this.setXPos incontextof this,
    ypos    : this.setYPos incontextof this,
    front   : this.setFront incontextof this,
    back    : this.setBack incontextof this,
    level   : this.setLevel incontextof this,
    voice   : this.setVoice incontextof this,
    nextvoice : this.setNextVoice incontextof this,
    clearVoice : this.clearVoice incontextof this,
    playvoice : this.playVoice2 incontextof this,
    waitvoice : this.waitVoice incontextof this,
    facewin : function(param) { disp = FACE; } incontextof this,
    faceon : null,
    faceoff : null,
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

        // ��|�[�Y�}�b�v�����݂���ꍇ
        if (facePoseMap !== void) {
            var p;
            if ((p = facePoseMap[cmd]) !== void) {
                setPose(p,elm);
                setFace(cmd,elm);
                find = true;
            }
        }

        if (!find && poses !== void) {
            if (poses[cmd] !== void) {
                find = true;
                setPose(cmd,elm);
            } else {
                var poseInfo;
                if ((poseInfo = poses[pose]) !== void) {
                    var dresses       = poseInfo.dresses;
                    var faces         = poseInfo.faces;
                    if (dresses !== void && dresses[cmd] !== void) {
                        //dm("������ݒ�");
                        find = true;
                        setDress(cmd,elm);
                    } else if (faces !== void && faces[cmd] !== void) {
                        //dm("�\���ݒ�");
                        find = true;
                        setFace(cmd,elm);
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
            find = _setTrans(cmd, elm);
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
                layer.type    = layer._initType    = baseImage.type;
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
                    return false;
                }
            } else {
                return false;
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
        
        if (grayscale) {
            layer.doGrayScale();
        }
        if (rgamma != void || ggamma != void || bgamma != void) {
            layer.adjustGamma(rgamma == void ? 1.0 : rgamma, 0, 255,
                              ggamma == void ? 1.0 : ggamma, 0, 255,
                              bgamma == void ? 1.0 : bgamma, 0, 255);
        }
    }

    /**
     * ���݂̗����G�̕`��
     * @param layer �`��Ώۃ��C��
     */
    function drawLayer(layer) {
        initLayer(layer);
        var levelName;
        var levelInfo = env.levels[level];
        if (levelInfo !== void) {
            levelName = levelInfo.name;
        }
        if (levelName === void) {
            levelName = level;
        }
        return _drawLayer(layer, levelName);
    }

    /**
     * �\���`�悷��
     */
    function drawFace(layer, faceLevelName) {
        //dm("�\��`��:" + name + " face:" + face + " layer:" + layer.name + " level:" + faceLevelName);
        if (faceLevelName !== void && poses !== void) {
            _drawLayer(layer, faceLevelName);
            layer.visible = true;
        } else {
            layer.visible = false;
        }
    }

    property yoffset {
        getter() {
            return init.yoffset !== void ? +env.yoffset + +init.yoffset : +env.yoffset;
        }
    }
    
    /**
     * ���C���z�u����(�L�����N�^�p)�F�������_�{���x���␳
     * @param layer �����Ώۃ��C��
     */
    function calcPosition(layer) {
        if (reposition) {
            //dm("�ʒu�w��3");

            // �����������f�t�H���g
            if (xpos === void) {
                xpos = env.defaultXpos;
            }
            if (ypos === void) {
                ypos = env.defaultYpos;
            }
            
            // ���x���ʍ��W�␳�����B�Ƃ肠�����K���ŉ�ʒ����Ɍ������ďk���������Ă���
            var zoom;
            var levelYoffset = 0;
            var levelInfo = env.levels[level];
            if (levelInfo !== void) {
                zoom         = (int)levelInfo.zoom;
                levelYoffset = (int)levelInfo.yoffset;
            }
            if (zoom === void) {
                zoom = 100;
            }
            if (levelYoffset === void) {
                levelYoffset = 0;
            }
            
            var l = kag.scWidth  / 2 + ((int)xpos * zoom / 100) - layer.imageWidth / 2;
            var t = kag.scHeight / 2 + ((yoffset - (int)ypos) * zoom / 100) - layer.imageHeight + levelYoffset;
            if (moveTime) {
                if (xposFrom !== void || yposFrom !== void) {
                    var fl = xposFrom !== void ? kag.scWidth  / 2 + ((int)xposFrom * zoom / 100) - layer.imageWidth / 2 : l;
                    var ft = yposFrom !== void ? kag.scHeight / 2 + ((yoffset - (int)yposFrom) * zoom / 100) - layer.imageHeight + levelYoffset: t;
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
        if (redrawFace) {
            if (env.currentNameTarget != this) {
                env.drawName();
            }
            env.drawFacePage(layer.parent === kag.fore.base ? kag.fore : kag.back, this);
            env.currentNameTarget = this;

            redrawFace = false;
        }
        
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

    var reNumber = new RegExp("^[0-9][0-9]*$");

    // ���Z���Ȃ��t���O
    var noinc;
    
    /**
     * �{�C�X�t�@�C���̎w��
     */
    function setVoice(param, elm) {
        if (typeof param == "Integer") {
            if (elm.once) {
                strVoice = getVoice(param);
            } else {
                voice = param;
                strVoice = void;
            }
        } else if (typeof param == "String") {
            if (reNumber.test(param)) {
                if (elm.once) {
                    strVoice = getVoice((int)param);
                } else {
                    voice = (int)param;
                    strVoice = void;
                }
            } else {
                strVoice = param;
            }
        } else {
            voice = void;
            strVoice = void;
        }
        incVoice   = elm.incvoice;
        noincVoice = elm.noincvoice;
        //dm("�{�C�X�ݒ�:" + param + ":" + voice);
    }

    /**
     * �{�C�X�t�@�C���w��{����Đ��G���g��
     */
    function setNextVoice(param) {
        env.entryNextVoice(this);
    }
    
    /**
     * �{�C�X�t�@�C���w��̉���
     */
    function clearVoice() {
        voice = void;
        strVoice = void;
    }
    
    /**
     * �{�C�X�t�@�C�����̎擾
     * @param voice �p�����[�^�@���l�̏ꍇ�͏��������A������̏ꍇ�͂��̂܂ܕԂ�
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
        return strVoice == "ignore" ? void : getVoice(strVoice !== void ? strVoice : voice);
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
            if (strVoice === void || incVoice) {
                if (typeof voice == "Integer") {
                    if (!noincVoice) {
                        voice++;
                    }
                } else {
                    voice = void;
                }
            }
            incVoice   = void;
            noincVoice = void;
            strVoice   = void;
        }

        if (voicename !== void && kag.getVoiceOn(init.voiceName)) {
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
                    // �g���q�⊮����
                    if (!Storages.isExistentStorage(voicename)) {
                        var test;
                        if(test = voicename + ".wav", Storages.isExistentStorage(test))
                            voicename = test;
                        else if(test = voicename + ".ogg", Storages.isExistentStorage(test))
                            voicename = test;
                    }
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
        //dm("�O���{�C�X�Đ�:" + param);
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
    

    /**
     * �{�C�X�̃{�����[���̍Ē���
     */
    function resetVoiceVolume() {
        if (soundBuffer !== void) {
            soundBuffer.volume2 = kag.getVoiceVolume(init.voiceName) * 1000;
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
            kag.clearBgmLabel();
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

    /**
     * �I���܂�
     * @param param �t�F�[�h����
     */
    function wait(param, elm) {
        var e = %[];
        (Dictionary.assign incontextof e)(elm, false);
        if (e.canskip === void) {
            e.canskip = true;
        }
        ret = kag.waitBGMStop(e);
    }

    /**
     * �I���܂�
     * @param param �t�F�[�h����
     */
    function waitFade(param, elm) {
        var e = %[];
        (Dictionary.assign incontextof e)(elm, false);
        if (e.canskip === void) {
            e.canskip = true;
        }
        ret = kag.waitBGMFade(e);
    }


    var bgmcommands = %[
    tagname : null, 
    play : play incontextof this,
    stop : stop incontextof this,
    pause : pause incontextof this,
    resume : resume incontextof this,
    fade : fade incontextof this,
    wait : wait incontextof this,
    waitfade : waitFade incontextof this,
    noxchg : null,
    loop : null,
    time : null,
    start : null,
    paused : null,
    intime : null,
    outtime : null,
    overlap : null,
    canskip : null,
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

    var ret;

    /**
     * KAG �^�O����
     * @param elm ����
     */
    function tagfunc(elm) {
        //dm("BGM �p�t�@���N�V�����Ăяo��!");
        ret = 0;
        doflag = false;
        foreach(elm, doCommand);
        // �������Ȃ������ꍇ�A���A�^�O���� bgm �łȂ���΂�����Đ�����
        if (!doflag && elm.tagname != "bgm") {
            play(elm.tagname, elm);
        }
        return ret;
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

    /**
     * �I���҂�
     */
    function wait(param, elm) {
        var e = %[];
        (Dictionary.assign incontextof e)(elm, false);
        e.buf = id;
        if (e.canskip === void) {
            e.canskip = true;
        }
        ret = kag.waitSEStop(e);
    }

    /**
     * �t�F�[�h�I���҂�
     */
    function waitFade(param, elm) {
        var e = %[];
        (Dictionary.assign incontextof e)(elm, false);
        e.buf = id;
        if (e.canskip === void) {
            e.canskip = true;
        }
        ret = kag.waitSEStop(e);
    }

    var secommands = %[
    tagname : null, 
    play : play incontextof this,
    stop : stop incontextof this,
    fade : fade incontextof this,
    wait : wait incontextof this,
    waitfade : waitFade incontextof this,
    loop : null,
    time : null,
    start : null,
    canskip : null,
    buf : null
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

    var ret;
    
    /**
     * KAG �^�O����
     * @param elm ����
     */
    function tagfunc(elm) {
        // dm("SE �p�t�@���N�V�����Ăяo��!");
        ret = 0;
        doflag = false;
        foreach(elm, doCommand);
        // �������Ȃ������ꍇ�A���A�^�O���� se �łȂ���΂�����Đ�����
        if (!doflag && elm.tagname != "se") {
            play(elm.tagname, elm);
        }
        return ret;
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
    var defaultXpos;  //< �L�����N�^�z�u�̏���X�ʒu
    var defaultYpos;  //< �L�����N�^�z�u�̏���Y�ʒu
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

    // ���b�Z�[�W�������ΏۂɂȂ�f�t�H���g�̃I�u�W�F�N�g
    // ���O�\���̂��Ɛݒ肳���
    // ���n���߂̌�͉��������
    var currentNameTarget;
    // �R�}���h���s���ɕ\������s�����[�h
    var showFaceMode;
    // �\��̃t�F�[�h�w��
    var faceFadeTime;

    // �S�J���[����
    var colorall;

    // �摜�␮�w��

    property grayscale {
        getter() {
            return _grayscale;
        }
        setter(v) {
            _grayscale = v;
            if (colorall) {
                setColorAll(true);
            }
            redraw = true;
        }
    }

    property rgamma {
        getter() {
            return _rgamma;
        }
        setter(v) {
            _rgamma = v;
            if (colorall) {
                setColorAll(true);
            }
            redraw = true;
        }
    }

    property ggamma {
        getter() {
            return _ggamma;
        }
        setter(v) {
            _ggamma = v;
            if (colorall) {
                setColorAll(true);
            }
            redraw = true;
        }
    }
    
    property bgamma {
        getter() {
            return _bgamma;
        }
        setter(v) {
            _bgamma = v;
            if (colorall) {
                setColorAll(true);
            }
            redraw = true;
        }
    }

    function resetColor(param, elm) {
        _grayscale = void;
        _rgamma = void;
        _ggamma = void;
        _bgamma = void;
        if (colorall) {
            setColorAll(true);
        }
        redraw = true;
    }

    function setColorAll(param) {
        colorall = param;
        // ���ׂčĕ`�揈��
        event.setRedraw();
        foreach(characters, function(name, value, dict) {
            value.setRedraw();
        });
        foreach(layers, function(name, value, dict) {
            value.setRedraw();
        });
        redraw = true;
    }

    
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

        // ���������W�J
        envinfo = loadEnvinfo();

        if (envinfo) {
            // �f�o�b�O�\�� 
            times       = envinfo.times;       showKeys("times", times);
            stages      = envinfo.stages;      showKeys("stages", stages);
            events      = envinfo.events;      showKeys("events", events);
            positions   = envinfo.positions;   showKeys("positions", positions);
            actions     = envinfo.actions;     showKeys("actions", actions);
            emotions    = envinfo.emotions;    showKeys("emotions", emotions);
            transitions = envinfo.transitions; showKeys("transitions", transitions);
            defaultTime = envinfo.defaultTime;      dm("defaultTime:" + defaultTime);
            yoffset     = (int)envinfo.yoffset;     dm("yoffset:" + yoffset);
            defaultXpos = (int)envinfo.defaultXpos;
            defaultYpos = (int)envinfo.defaultYpos;
            defaultLevel = (int)envinfo.defaultLevel;
            levels       = envinfo.levels;
            faceLevelName = envinfo.faceLevelName;
            showFaceMode  = envinfo.showFaceMode;
            faceFadeTime  = envinfo.faceFadeTime;

        
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
        kag.tagHandlers["env"]        = this.tagfunc;
        kag.tagHandlers["allchar"]   = this.allchar;
        kag.tagHandlers["alllayer"]   = this.alllayer;

        kag.tagHandlers["begintrans"] = this.beginTrans;
        kag.tagHandlers["endtrans"]   = this.endTrans;
        kag.tagHandlers["newlay"]     = this.newLayer;
        kag.tagHandlers["newchar"]    = this.newCharacter;

        kag.tagHandlers["msgoff"]     = this.msgoff;
        kag.tagHandlers["msgon"]      = this.msgon;
        kag.tagHandlers["clear"]      = this.clear;
        
        kag.tagHandlers["dispname"]        = this.dispname;
        kag.tagHandlers["dispnameVoice"]   = this.dispnameVoice;
        kag.tagHandlers["endline"]         = this.endline;
        kag.tagHandlers["quake"]           = this.quake;
        kag.tagHandlers["afterpage"]       = this.afterpage;

        // ����n���h���o�^
        kag.unknownHandler         = this.unknown;
        kag.seStopHandler          = this.onSeStop;
        kag.stopActionHandler      = this.onStopAction;
        
        dm("������������");
    }

    /**
     * ���������̃��[�h
     */
    function loadEnvinfo() {
        try {
            var yamlFile = 'envinit.yaml';
            if (Storages.isExistentStorage(yamlFile)) {
                return YAML.parse([].load(yamlFile));
            }

            return Scripts.evalStorage("envinit.tjs");
        } catch (e) {
            throw new Exception("���������̃p�[�X�Ɏ��s���܂���(�ڍׂ̓R���\�[���Q��)");
        }
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

        f.colorall = colorall;
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

        colorall = f.colorall;
        if (colorall) {
            setColorAll(colorall);
        }

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
        if (kag.seStopHandler== this.onSeStop) {
            kag.seStopHandler          = void;
        }
        if (kag.stopActionHandler === this.onStopAction) {
            kag.stopActionHandler = void;
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

    // -----------------------------------------

    /**
     * �����̏�����
     */
    function init() {

        //dm("����������");
        time = void;
        stage = void;
        transMode = void;

        colorall = false;

        initImage();
        event.initImage();

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

        // SE ������
        for (var i=0;i<ses.count;i++) {
            ses[i].name  = void;
            ses[i].count = 0;
        }
        seCount = 0;

        // �J�����g�I�u�W�F�N�g������
        currentNameTarget = void;
        redraw = true;
        
        voiceCharacters.clear();
    }

    /**
     * �C�x���g�G�̏���
     */
    function hideEvent() {
        event.disp = CLEAR;
        redraw = true;
    }

    /**
     * �w�i�ƃC�x���g�G�̏���
     */
    function hideBase() {
        disp = CLEAR;
        event.disp = CLEAR;
        redraw = true;
    }
    
    /**
     * �S�L�����N�^����
     */
    function hideCharacters() {
        // �L�����N�^���̔j��
        foreach(characters, function(name,value,dict) {
            value.disp = global.KAGEnvImage.CLEAR;
        });
        currentNameTarget = void;
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
     * �O�i�v�f����
     */
    function hideFore() {
        hideCharacters();
        hideLayers();
    }

    /**
     * �S�v�f����
     */
    function hideAll() {
        hideBase();
        hideCharacters();
        hideLayers();
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

            // �w�i�w�莞�ɍ��W�w�肪�Ȃ���Ώꏊ��������������
            if (elm.xpos === void) {
                xpos = void;
            }
            if (elm.ypos === void) {
                ypos = void;
            }
            reposition = true;

            // �X�e�[�W�ύX���t�b�N
            if (global.setStageHook !== void) {
                global.setStageHook(stageName, elm);
            }

            // �g�����W�V�����w��
            if (!setTrans2(stages[stage].trans)) {
                if (!setTrans2(env.envinfo.stageTrans)) {
                    setTrans2(env.envinfo.envTrans);
                }
            }

        }
        // �C�x���g�G�͏���
        if (event.disp != CLEAR) {
            event.disp = CLEAR;
            redraw = true;
        }
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
                value.setRedraw();
            });

            // �g�����W�V�����w��
            if (!setTrans2(times[time].trans)) {
                if (!setTrans2(env.envinfo.timeTrans)) {
                    setTrans2(env.envinfo.envTrans);
                }
            }
        }
        // �C�x���g�G�͏���
        if (event.disp != CLEAR) {
            event.disp = CLEAR;
            redraw = true;
        }
    }

    // �g�����W�V�������[�h
    var transMode;

    /**
     * �S�̃g�����W�V�����J�n
     */
    function beginTrans(elm) {
        kag.fore.base.stopTransition();
        kag.backupLayer(EMPTY, true);
        transMode = true;
        return 0;
    }

    /**
     * �g�����W�V�����w����R�}���h����T������
     */
    function checkTrans(cmd, param, elm) {
        if (cmd == "tagname") {
            // ignore
        } else if (cmd == "trans") {
            setTrans(param, elm);
        } else if (cmd == "fade") {
            var fadeTime = +param;
            trans = %[ "method" => "crossfade",
                       "children" => true,
                       "time" => fadeTime > 1 ? fadeTime : fadeValue];
        } else {
            _setTrans(cmd, elm);
        }
    }

    /**
     * �S�̃g�����W�V�����I��
     */
    function endTrans(elm) {
        // �܂������i�g�����W�V������������ true ���Ə������o�^����Ȃ��̂�)
        transMode = false;
        ret = void;
        foreach(elm, checkTrans);
        // ���w�莞
        if (trans !== void && trans.method !== void) {
            hideMessage();
            beginTransition(trans);
        } else {
            // ����ʂ��珑���߂�
            kag.backupLayer(EMPTY, false);
        }
        trans = void;
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
        // �����f�[�^�͔p��
        if (characters[elm.name] !== void) {
            invalidate characters[elm.name];
            delete characters[elm.name];
        }
        // �V�K����
        var ch = getCharacter(elm.name, elm.initname);
        if (ch != null) {
            delete elm.tagname;
            delete elm.name;
            delete elm.initname;
            return ch.tagfunc(elm);
        }
        return 0;
    }        

    /**
     * ���b�Z�[�W���̃g�����W�V�������������݂��� ON/OFF
     */
    function msgonoff(elm, v) {
        ret = void;
        if (!v) {
            kag.conductor.pendings.insert(0, %[ tagname : "er", all:true]);
        }
        if (!transMode && !isSkip() && elm.nofade === void) {
            foreach(elm, checkTrans);
            if (trans !== void && trans.method !== void) {
                kag.updateBeforeCh = 1;
                kag.fore.base.stopTransition();
                kag.backupLayer(EMPTY, true);
                kag.setCurrentMessageLayerVisibleFast(1, v);
                beginTransition(trans);
                return ret;
            } else {
                return kag.setCurrentMessageLayerVisible(v) ? -2 : 0;
            }
        }
        kag.setCurrentMessageLayerVisibleFast(transMode ? 1: 0, v);
        return ret;
    }

    function msgon(elm) { return msgonoff(elm, true); }
    function msgoff(elm) { return msgonoff(elm, false); }

    /**
     * ��ʂ̕\���v�f�̑S����
     */
    function clear(elm) {
        ret = void;
        kag.conductor.pendings.insert(0, %[ tagname : "er", all:true]);
        hideAll();
        if (!transMode && !isSkip()) {
            foreach(elm, checkTrans);
            if (trans !== void && trans.method !== void) {
                kag.updateBeforeCh = 1;
                kag.fore.base.stopTransition();
                kag.backupLayer(EMPTY, true);
                kag.setCurrentMessageLayerVisibleFast(1, false);
                drawAll(kag.back);
                beginTransition(trans);
                return ret;
            }
        }
        kag.setCurrentMessageLayerVisibleFast(transMode ? 1: 0, false);
        drawAll();
        return ret;
    }

    var envCommands = %[
    /**
     * �S�̂̏���������
     */
    init : this.init incontextof this,
    stage : this.setStage incontextof this,
    stime : this.setTime incontextof this,
    hidebase : this.hideBase incontextof this,
    hideevent : this.hideEvent incontextof this,
    hidecharacters : this.hideCharacters incontextof this,
    hidelayers : this.hideLayers incontextof this,
    hidefore : this.hideFore incontextof this,
    hideall : this.hideAll incontextof this,
    stopallvoice : this.stopAllVoice incontextof this,
    colorall : this.setColorAll incontextof this,
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
                    
                    if (reposition) {
                        if (xpos === void) {
                            xpos = (int)stages[stage].xoff;
                        }
                        if (ypos === void) {
                            ypos = (int)stages[stage].yoff;
                        }
                    }

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

                        if (reposition) {
                            if (xpos === void) {
                                xpos = (int)stages[stage].xoff;
                            }
                            if (ypos === void) {
                                ypos = (int)stages[stage].yoff;
                            }
                        }
                        
                    } catch (e) {
                        dm("�w�i�摜�����[�h�ł��܂���" + image);
                    }
                }

            } else {
                dm("���Ԃ̃f�t�H���g�w�肪���݂��Ă��܂���");
            }


        } else {
            layer.clearImage(true);
        }

        if (grayscale) {
            layer.doGrayScale();
        }
        if (rgamma != void || ggamma != void || bgamma != void) {
            layer.adjustGamma(rgamma == void ? 1.0 : rgamma, 0, 255,
                              ggamma == void ? 1.0 : ggamma, 0, 255,
                              bgamma == void ? 1.0 : bgamma, 0, 255);
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
        initLayer(layer);

        drawStage(layer);
        
        // �q�v�f�̍ĕ`��
        foreach(characters, function(name, value, dict, base) {
            value.updateImage(base);
        }, base);
        foreach(layers, function(name, value, dict, base) {
            value.updateImage(base);
        }, base);
        event.updateImage(base);

        // ���b�Z�[�W��������
        if (currentNameTarget == void) {
            drawNamePage(base);
            if (faceLevelName !== void) {
                clearFacePage(base);
            }
        }
        
        updateLayer(layer);
        calcPosition(layer);
    }

    /**
     * ���C���z�u����(�w�i�p)�F�������_
     * @param layer �����Ώۃ��C��
     */
    function calcPosition(layer) {
        if (reposition) { 
            //dm("�ʒu�w��2");
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
    
    /**
     * �ĕ`�揈��
     */
    function updateImage() {

        kag.updateBeforeCh = 1;

        // �`��X�V���K�v�ȏꍇ
        if (redraw) {
            
            if (trans !== void && trans.method !== void) {
                kag.fore.base.stopTransition();
                
                // �X�V�����𑖂点��ꍇ
                if (trans.time === void) {
                    trans.time = 1000;
                }
                trans.children = true;

                // �S���C�����o�b�N�A�b�v
                kag.backupLayer(EMPTY, true);
                
                // �X�e�[�W�̕`��
                drawAll(kag.back);
                
                // �g�����W�V�������s
                beginTransition(trans);
                
            } else {

                var layer = getLayer();

                // �t�F�[�h����
                // ���ɑ΂��閽�߂̏ꍇ�͏�Ƀg�����W�V�������g��
                if (fadeTime !== void) {

                    kag.fore.base.stopTransition();
                    var trans = %[ "method" => "crossfade",
                                   "children" => true,
                                   "time" => fadeTime > 1 ? fadeTime : fadeValue];
                    fadeTime = void;

                    // �S���C���𗠂Ƀo�b�N�A�b�v
                    kag.backupLayer(EMPTY, true);

                    // �����C�����Ώ�
                    drawAll(kag.back);

                    // �g�����W�V�������s
                    beginTransition(trans);

                } else {
                    drawAll();
                }
            }
            redraw = false;
        } else {
            var layer = getLayer();
            updateLayer(layer);
            calcPosition(layer);
        }
        trans = void;
    }

    /**
     * KAG �^�O����
     * @param elm ����
     */
    function tagfunc(elm) {
        //dm("���^�O����΂ꂽ��");
        ret = void;
        foreach(elm, doCommand);
        hideMessage();
        updateImage();

        // �J�����g�I�u�W�F�N�g�̉��� XXX �����ł����̂��H
        return ret;
    }

    /**
     * �S�L�����ɃR�}���h���s
     * @param elm ����
     */
    function allchar(elm) {
        ret = void;
        var names = [];
        names.assign(characters);
        for (var i=0; i<names.count; i+= 2) {
            ret = names[i+1].tagfunc(elm);
        }
        return ret;
    }

    /**
     * �S���C���ɃR�}���h���s
     * @param elm ����
     */
    function alllayer(elm) {
        ret = void;
        var names = [];
        names.assign(layers);
        for (var i=0; i<names.count; i+= 2) {
            ret = names[i+1].tagfunc(elm);
        }
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

    /**
     * �S�L�����̃{�C�X���~����
     */
    function resetAllVoiceVolume() {
        foreach(characters, function(name, value, dict) {
            value.resetVoiceVolume();
        });
    }
    
    /*
     * �w�肵���L�����Ŏw�肵���{�C�X�t�@�C�����Đ�
     */
    function playVoice(name, voicename) {
        var ch = getCharacter(name);
        if (ch !== void && voicename !== void) {
            return ch.playVoice(voicename);
        }
    }

    function quake(elm) {
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
            kag.historyLayer.reline(); // XXX �v����
        }
    }

    function getMessageLayer(base) {
        if (base === void) {
            base = transMode ? kag.back : kag.fore;
        }
        return base.messages[kag.currentNum];
    }

    function getFaceLayer(base) {
        return getMessageLayer(base).faceLayer;
    }
    
    /**
     * �\��\������������
     */
    function loadFacePage(base, name) {
        var faceLayer = getFaceLayer(base);
        try {
            faceLayer.loadImages(name);
        } catch (e) {
            dm("�\��摜�̃��[�h�Ɏ��s���܂���:" + name);
        }
        faceLayer.visible = true;
    }

    /**
     * �\���������������
     */
    function clearFacePage(base) {
        if (envinfo.clearFace !== void) {
            loadFacePage(base, envinfo.clearFace);
        } else {
            var faceLayer = getFaceLayer(base);
            faceLayer.visible = false;
        }
    }
    
    /**
     * �\���������
     */
    function clearFace() {
        if (!transMode) {
            clearFacePage(kag.fore);
        }
        clearFacePage(kag.back);
    }

    // �w�肳�ꂽ�L�����N�^�̕\��\���\�Ȃ�\������
    function drawFacePage(base, ch) {
        if (faceLevelName !== void) {
            if (ch !== void) {
                var layer = getFaceLayer(base);
                if (ch.isShowFace()) {
                    ch.drawFace(layer, faceLevelName);
                } else {
                    layer.visible = false;
                }
            } else {
                clearFacePage(base);
            }
        }
    }

    /**
     * ���O�̕\���i�y�[�W�w�肠��)
     */
    function drawNamePage(base, name="") {
        var msg = getMessageLayer(base);
        msg.processName(name);
    }

    /**
     * ���O�̕\��
     */
    function drawName(name = "") {
        if (!transMode) {
            drawNamePage(kag.fore, name);
        }
        drawNamePage(kag.back, name);
    }

    var voiceCharacters = [];

    function getVoicePlayingScript(ch) {
        var voice;
        if (ch !== void && (voice = ch.getCurrentVoice()) !== void) {
            return "global.world_object.env.playVoice(\"" + ch.name + "\",\"" + voice + "\");";
        } else {
            return "";
        }
    }
    
    /**
     * ���񓯎��ɖ炷�{�C�X�̒ǉ�
     */
    function entryNextVoice(ch) {
        voiceCharacters.add(ch);
    }
    
    function getNextVoiceScript() {
        var ret = "";
        for (var i=voiceCharacters.count-1;i>=0;i--) {
            ret += getVoicePlayingScript(voiceCharacters[i]);
        }
        return ret;
    }

    /**
     * �Ȃ炷�{�C�X�����邩
     */
    function checkNextVoice() {
        for (var i=voiceCharacters.count-1;i>=0;i--) {
            var voice = voiceCharacters[i].getCurrentVoice();
            if (voice !== void) {
                return true;
            }
        }
        return false;
    }

    /**
     * �����{�C�X��炷
     */
    function playNextVoice() {
        var ret = void;
        for (var i=voiceCharacters.count-1;i>=0;i--) {
            var ch = voiceCharacters[i]; 
            var voice = ch.getCurrentVoice();
            if (voice !== void) {
                var r = ch.playVoice();
                if (ret === void || (r !== void && r > ret)) {
                    ret = r;
                }
            }
        }
        voiceCharacters.clear();
        return ret;
    }
    
    /**
     * ���O�\�������n���h��
     */
    function dispname(elm) {

        ret = void;
        
        if (kag.sflags.voicecut) {
            stopAllVoice();
        }

        // �\��\����
        
        //dm("���O�\���n���h��");
        if (elm === void || elm.name === void || elm.name == "") {
            
            // �L�����w�肪�����ꍇ
            drawName();
            
            // �\���
            if (faceLevelName !== void && currentNameTarget !== void && currentNameTarget.isShowFace() && !transMode && !isSkip() && faceFadeTime > 0) {
                // �t�F�[�X���t�F�[�h�ŏ�������ꍇ
                kag.backupLayer(EMPTY, true);
                clearFacePage(kag.back);
                beginTransition(%[ method: "crossfade", time: faceFadeTime]);
                
            } else {
                if (faceLevelName !== void) {
                    clearFace();
                }
            }

            currentNameTarget = void;
            dispnameVoice(elm);
            
        } else {

            var name = elm.name;
            var disp = elm.disp;
            var ch = getCharacter(name);

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
            elm.dispName = dispName;
            
            // ������ԂȂ��\����Ԃɂ���
            if (ch !== void && ch.disp == CLEAR && ch.poses) {
                ch.disp = FACE;
            }
            
            // �\��ύX����
            if (faceLevelName !== void && currentNameTarget != ch &&
                ((currentNameTarget !== void && currentNameTarget.isShowFace()) || (ch !== void && ch.isShowFace())) &&
                !transMode && !isSkip() && faceFadeTime > 0) {
                
                // �t�F�[�h�ŕ\������ꍇ

                // XXX ���O����
                drawName();
                
                currentNameTarget = ch;
                
                kag.backupLayer(EMPTY, true);
                if (ch !== void && ch.isShowFace()) {
                    drawFacePage(kag.back, ch);
                } else {
                    var img;
                    if (envinfo.nameFaces !== void && (img = envinfo.nameFaces[envinfo.dispNameFace ? dispName : name]) !== void) {
                        loadFacePage(kag.back, img);
                    } else {
                        clearFacePage(kag.back);
                    }
                }

                // �{�C�X�Đ������̐摗��
                var e = %[];
                (Dictionary.assign incontextof e)(elm, false);
                e.tagname = "dispnameVoice";
                kag.conductor.pendings.insert(0, e);

                beginTransition(%[ method: "crossfade", time: faceFadeTime]);

            } else {

                // �u�ԕ\������ꍇ
                // ���Ԃ񂱂��̃��W�b�N�K�v�Ȃ��H
                
                currentNameTarget = ch;
                
                if (ch !== void && ch.isShowFace()) {
                    if (!transMode) {
                        drawFacePage(kag.fore, ch);
                    }
                    drawFacePage(kag.back, ch);
                } else {
                    var img;
                    if (envinfo.nameFaces !== void && (img = envinfo.nameFaces[envinfo.dispNameFace ? dispName : name]) !== void) {
                        if (!transMode) {
                            loadFacePage(kag.fore, img);
                        }
                        loadFacePage(kag.back, img);
                    } else {
                        clearFace();
                    }
                }
                dispnameVoice(elm);
            }
        }
        return ret;
    }

    /**
     * ���O�\���{�{�C�X�Đ�����
     * �{�C�X�̏�Ԃɉ������҂����ԕ␳���s��
     */
    function dispnameVoice(elm) {

        // -------  ���O�\���i�u�ԁj

        if (elm === void || elm.name === void || elm.name == "") {

            // ���łɖ��O�\���ς�
            
        } else {
            var name     = elm.name;
            var dispName = elm.dispName;
            if (dispName === void) {
                dispName = name;
            }

            var ch = getCharacter(name);

            // �{�C�X�̓o�^
            var nextVoice;
            if (kag.historyWriteEnabled && (nextVoice = getNextVoiceScript() + getVoicePlayingScript(ch)) != "") {
                kag.historyLayer.setNewAction("global.world_object.env.stopAllVoice();" + nextVoice);
            }

            // ���O���H����
            if (typeof global.dispNameFilter !== 'undefined') {
                dispName = global.dispNameFilter(dispName);
            }
            // �q�X�g���p
            if (kag.historyWriteEnabled) {
                if (typeof kag.historyLayer.storeName !== 'undefined') {
                    kag.historyLayer.storeName(dispName);
                } else {
                    kag.historyLayer.store(dispName + " ");
                }
            }

            drawName(dispName);
        }

        // -------  �{�C�X�Đ�
        
        // �{�C�X�Đ�
        var ret;
        if (currentNameTarget !== void) {
            // �{�C�X�Đ�������ꍇ
            if (currentNameTarget.getCurrentVoice() !== void || checkNextVoice()) {
                // �ق��̃{�C�X����������
                env.stopAllVoice();
                // �����Đ��{�C�X�̍Đ�
                ret = currentNameTarget.playVoice();
                var r = playNextVoice();
                if (ret === void || (r !== void && r > ret)) {
                    ret = r;
                }
            }
        } else {
            // �{�C�X�Đ�������ꍇ
            if (checkNextVoice()) {
                // �ق��̃{�C�X����������
                env.stopAllVoice();
                // �����Đ��{�C�X�̍Đ�
                ret = playNextVoice();
            }
        }
        kag.addAutoWait(ret);
        return 0;
    }

    /**
     * �y�[�W������ɌĂяo����鏈��
     */
    function afterpage(elm) {
        if (kag.sflags.voicecutpage) {
            stopAllVoice();
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
        } else if (tagName.substring(0,2) == "ev" ||
                   (events !== void && events[tagName] !== void)) {
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
        
        // �J�����g�̃R�}���h�Ƃ��Ď��s���Ă݂�
        if (currentNameTarget !== void) {
            elm[tagName] = true;
            return currentNameTarget.tagfunc(elm);
        }
    }

    /**
     * �A�N�V������~
     */
    function onStopAction() {
        stopAction();
        event.stopAction();
        foreach(characters, function(name, value, dict) {
            value.stopAction();
        });
        foreach(layers, function(name, value, dict) {
            value.stopAction();
        });
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
