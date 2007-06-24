//
// �ėp�A�N�V�����@�\�����p�N���X�Q
//

// �A�N�V�������Ԑ���p
// �O���[�o���Ɏw��
drawspeed <- 1;

/**
 * �P�ƃv���p�e�B�ɑ΂���A�N�V�������
 * ��΃A�N�V�����F�^�[�Q�b�g�v���p�e�B�̐�Βl���w�肷��@�@�@�@�@�P�����g���Ȃ�
 * ���΃A�N�V�����F�^�[�Q�b�g�v���p�e�B�ɑ΂��鑊�Βl���w�肷��@�@�����w��ł���
 */
class PropActionInfo {

	propName = null;      // �v���p�e�B��
    absolute = null;      // ��΃A�N�V�����n���h��
    relative = []; // ���΃A�N�V�����n���h��
	first = true;     // ������s
	startTime = null; // �J�n����
    
    /**
     * �R���X�g���N�^
     */
	constructor(propName) {
        this.propName = propName;
	}

	/**
	 * �����̐���
	 */
	function cloneObj() {
		local ret = ::PropActionInfo(propName);
		ret.absolute = absolute;
		for (local i=0;i<relative.len();i++) {
			ret.relative.append(relative[i]);
		}
		ret.first = this.first;
		ret.startTime = this.startTime;
		return ret;
	}

    /**
     * �A�N�V������ǉ�����
     * @param target �^�[�Q�b�g�I�u�W�F�N�g
     * @param handler �A�N�V�����n���h���N���X
     * @param elm �������p�����[�^
     */
    function addAction(target, handler, elm) {
		//dm("�A�N�V�����ǉ�:" + propName);
		if (typeof andler == "class") {
			if (inherited(handler, AbsoluteActionHandler)) {
				// ��΃A�N�V�����͂P�����ݒ�\
				absolute = handler(target, propName, elm);
			} else if (inherited(handler, RelativeActionHandler)) {
				// �f�t�H���g�A�N�V�����o�^
				if (absolute == null) {
					//dm("�f�t�H���g�A�N�V�����o�^");
					absolute = DefaultAction(target, propName, {});
				}
				// ���΃A�N�V�����͕����ݒ�\
				relative.append(handler(target, propName, elm));
			} else {
				dm("�n���h���ُ�:" + handler);
			}
		} else {
			dm("�n���h���ُ�:" + handler);
		}
    }

	/**
	 * �A�N�V�����̎��s���ʂ�Ԃ�
	 */
	function getResultValue() {
		if (absolute != null) {
			return absolute.getResultValue();
		} else {
			return null;
		}
	}

	
    /**
     * �A�N�V�����������s
     * @param target �^�[�Q�b�g�I�u�W�F�N�g
     * @param now ����
     * @param stopFlag �����I���w��
     * @return �I�������� true
     */
    function doAction(target, now, stopFlag) {
		if (first) {
			//dm("�A�N�V��������");
			startTime = now;
			if (absolute != null) {
				absolute.init(target, propName);
			}
			for (local i=0;i<relative.len();i++) {
				relative[i].init(target, propName);
			}
			first = false;
		}
		now -= startTime;
		//dm("�A�N�V�������:"+ now + " stop:" + stopFlag);
        local done = true;
		if (absolute != null) {
			if (!absolute.action(target, propName, now, stopFlag)) {
				done = false;
            }
        }
        if (!stopFlag) {
			for (local i=0;i<relative.len();i++) {
				if (!relative[i].action(target, propName, now)) {
					done = false;
                }
            }
        }
        return done;
    }
};

/**
 * �����v���p�e�B�ɑ΂���A�N�V����
 */
class ActionInfo {

	actionDict = {}; // �A�N�V�������
	actionList = [];

    /**
     * �R���X�g���N�^
     */
	constructor() {
    }

	/**
	 * �����̐���
	 */
	function cloneObj() {
		local ret = ::ActionInfo();
		for (local i=0;i<actionList.len();i++) {
			local newact = actionList[i].cloneObj();
			ret.actionList[i] <- newact;
			ret.actionDict[newact.propName] <- newact;
		}
		return ret;
	}

	
    function getPropActionInfo(propName) {
		local ret;
		if (propName in actionDict) {
			ret = actionDict[propName];
		} else {
			ret = PropActionInfo(propName);
			actionDict[propName] <- ret;
			actionList.append(ret);
		}
		return ret;
    }

	
    /**
     * �w�肳�ꂽ�v���p�e�B�̃A�N�V������ǉ�����
     * @param target �^�[�Q�b�g
     * @param propName �v���p�e�B��
     * @param handler �A�N�V�����n���h��
     * @param elm �������p�����[�^
     */
    function addAction(target, propName, handler, elm) {
		local info = getPropActionInfo(propName);
        info.addAction(target, handler, elm);
    }

    /**
     * �w�肳�ꂽ�v���p�e�B�̃A�N�V��������������
     * @param target �^�[�Q�b�g
     * @param propName �v���p�e�B��
     */
	function delAction(target, propName) {
		if (propName in actionDict) {
			local ret = actionDict[propName];
			ret.doAction(target, 0, true);
			removeFromArray(actionList, ret);
			delete actionDict[propName];
		}
	}

	/**
	 * �A�N�V�����̌��ʂ��擾����
	 */
	function getResultValue(ret) {
		for (local i=0;i<actionList.len();i++) {
			local info = actionList[i];
			local result = info.getResultValue();
			if (result != null) {
				ret[info.propName] <- result;
			}
		}
		return ret;
	}
	
    /**
     * �A�N�V�����������s
     * @param target �^�[�Q�b�g�I�u�W�F�N�g
     * @param now ����
     * @param stopFlag �����I���w��
     * @return �I�������� true
     */
    function doAction(target, now, stopFlag) {
        local done = true;
        for (local i=0; i<actionList.len(); i++) {
            if (!actionList[i].doAction(target, now, stopFlag)) {
                done = false;
            }
        }
        return done;
    }
};

/**
 * ��A�̃A�N�V����
 */
class ActionSequense {

	actions = [];

    target = null; // �ΏۃI�u�W�F�N�g
	create = true; // �A�N�V�����ǉ��t���O
	nowait = false; // �҂��Ȃ��t���O
	
    /**
     * �R���X�g���N�^
     */
	constructor(target) {
		this.target = target;
	}

	/**
	 * �����̐���
	 */
	function cloneObj() {
		local ret = ::ActionSequense(target);
		ret.create = create;
		ret.nowait = nowait;
		for (local i=0;i<actions.len();i++) {
			ret.actions.append(actions[i].cloneObj());
		}
		return ret;
	}
	
    /**
     * �A�N�V�����V�[�P���X��i�߂�
     */
    function next() {
        create = true;
    }
    
    /**
     * �A�N�V������ǉ�����
     * @param propName �v���p�e�B��
     * @param handler �A�N�V�����n���h��
     * @param elm �������p�����[�^
     */
    function addAction(propName, handler, elm) {
		if (target.hasProp(propName)) {
			if (create) {
				actions.append(ActionInfo());
				create = false;
            }
			actions[actions.len() - 1].addAction(target, propName, handler, elm);
		} else {
			dm("�w�肳�ꂽ�v���p�e�B�͑��݂��܂���:" + target + ":" + propName);
        }
    }

    /**
     * �A�N�V��������������
     * @param propName �v���p�e�B��
     */
    function delAction(propName) {
        if (actions.len() > 0) {
            actions[actions.len() - 1].delAction(target, propName);
        }
    }
    
    /**
     * �����̃A�N�V�����𓯎��o�^����
     * @param dict �A�N�V�������̓���������
     */
    function addActions(dict) {
		foreach (name, elm in dict) {
			if (elm == null) {
				delAction(name);
			} else if (typeof elm == "table") {
				if ("handler" in elm) {
					if (typeof elm.handler == "string") {
						addAction(name, eval(elm.handler), elm);
					} else if (typeof elm.handler == "class") {
						addAction(name, elm.handler, elm);
					} else {
						dm("�����ȃA�N�V�����n���h���w��:" + elm.handler);
					}
				} else {
					dm("�n���h���w�肪����܂���");
				}
			}
        }
    }

    // -----------------------------------------------


    /**
	 * �A�N�V�����̌��ʂ��擾����
     */
	function getResultValue() {
		local ret = {};
		for (local i=0;i<actions.len();i++) {
			actions[i].getResultValue(ret);
		}
		return ret;
	}
	
    /**
     * �A�N�V�������s
     * @param now ���ݎ���
     * @param stopFlag ������~�w��
     * @param �I�������� true
     */
    function doAction(now, stopFlag) {
		if (actions.len() > 0) {
			if (actions[0].doAction(target, now, stopFlag)) {
				actions.remove(0);
			}
		}
		return actions.len() <= 0;
    }

    /**
     * ������~
     */
	function stopAction() {
		while (actions.len() > 0) {
			//dm("�A�N�V������~:" + actions.len());
			actions[0].doAction(target, 0, true);
			actions.remove(0);
        }
    }
};

// ----------------------------------------------------------------
// �n���h���N���X
// ----------------------------------------------------------------

/**
 * �A�N�V�����n���h�����N���X
 */
class ActionHandler {

	time  = null; // �쓮����
	delay = null; // �J�n�x������

    /**
     * �R���X�g���N�^
     * @param target �ΏۃI�u�W�F�N�g
     * @param propName �Ώۃv���p�e�B
     * @param elm �������p�����[�^
     */
    function ActionHandler(target, propName, elm) {
		time  = getint(elm, "time");
		delay = getint(elm, "delay");
		if (time != null) {
			time *= ::drawspeed;
		}
		//dm("ActionHandler:" + propName + " time:" + time + " delay:" + delay);
    }

	/**
	 * ���s���񏈗�
	 */
	function init(target, propname) {
	}
	
    /**
     * ���s�����x�[�X
     * @param target �ΏۃI�u�W�F�N�g
     * @param propName �Ώۃv���p�e�B
     * @param now �쓮����
     * @param stopFlag ��~�t���O
     * @return �I�������� true
     */
    function action(target, propName, now, stopFlag) {
		if (!stopFlag && delay != null) {
			now -= delay;
			if (now < 0) {
				return false;
            }
		}
		return doAction(target, propName, now, stopFlag || (time != null && now > time));
    }
    
    /**
     * �A�N�V�������s
     * @param target �ΏۃI�u�W�F�N�g
     * @param propName �Ώۃv���p�e�B
     * @param now �쓮����
     * @param stopFlag ��~�t���O
     * @return �I�������� true
     */
    function doAction(target, propName, now, stopFlag) {
        // ���ꂼ��̃A�N�V�����Ŏ���
        return true;
    }

    /**
     * ���Βl�v�Z
     * @param value �l�w��
     * @param orig �I���W�i���̒l
     */
    function getRelative(value, orig) {
		if (typeof value == "string" && value.find("@") != null) {
			return eval(replace(value, "@", orig));
		} else {
			return value;
		}
    }
}

/**
 * ��Γ���A�N�V�������N���X
 * �l���ێ�����A�N�V����
 */
class AbsoluteActionHandler extends ActionHandler {
	constructor(target, propName, elm) {
		::ActionHandler.constructor(target, propName, elm);
    }
};

/**
 * ���΃A�N�V�������N���X
 */
class RelativeActionHandler extends ActionHandler {
	constructor(target, propName, elm) {
		::ActionHandler.constructor(target, propName, elm);
    }
};

/**
 * �l�������A�N�V����
 * ���΃A�N�V�����݂̂̏ꍇ�Ƀx�[�X�̒l���ێ����邽�߂Ɏg��
 */
class DefaultAction extends AbsoluteActionHandler {

	initValue = null; // �����l

	function constructor(target, propName, elm) {
		::AbsoluteActionHandler.constructor(target, propName, elm);
		initValue = target[propName];
    }
	/**
	 * �ړ��̌���
	 */
	function getResultValue() {
		return initValue;
	}

	function doAction(target, propName, now, stopFlag) {
		target[propName] = initValue;
		return true;
    }
}


/**
 * ��{�ړ��A�N�V�����i��΃A�N�V�����j
 * @param 
 */
class MoveAction extends AbsoluteActionHandler {

	initValue = null; // �������̒l
	start = null;  // �n�_�w�� 
	value = null;  // �I�_�w��
    accel = null;  // �������w��
	diff  = null;  // �ړ�����
    
	constructor(target, propName, elm) {
		::AbsoluteActionHandler.constructor(target, propName, elm);
		initValue = target[propName];
		start     = "start" in elm ? getRelative(getint(elm, "start"), initValue) : null;
		value     = getRelative(getint(elm, "value"), initValue);
		accel     = getint(elm.accel);
    }

	/**
	 * �ړ��̌���
	 */
	function getResultValue() {
		return value;
	}
	
	/**
	 * ���s���񏈗�
	 */
	function init(target, propName) {
		if (start == null) {
			start = target[propName];
		}
		diff  = value - start;
		//dm("MoveAction:" + propName + " start:" + start + " diff:" + diff);
	}

	
    function doAction(target, propName, now, stopFlag) {
		if (time == 0 || stopFlag) {
			target[propName] = start + diff;
			return true;
        } else {
			local n;
			if (accel == 0) {
				n = now / time;
			} else if (accel > 0) {
				local t = now / time;
				n = t*t;
            } else {
				local t = now/time;
				n = ( 1-(1-t)*(1-t) );
			}
			target[propName] = start + diff * n;
			return false;
        }
    }
}

/**
 * ���[�v�ړ��A�N�V�����i��΃A�N�V�����j
 * @param start �J�n�ʒu�i���w�莞�͌��ݒl)
 * @param min ���[�v�ŏ��l
 * @param max ���[�v�ő�l
 * @param loop ���[�v����
 */
class LoopMoveAction extends AbsoluteActionHandler {

	initValue = null; // �������̒l
	start = null;     // �n�_�w�� 
	min = null;       // ���[�v�ŏ��l
	max = null;       // ���[�v�ő�l
	loop = null;      // ���[�v����

	/**
	 * �R���X�g���N�^
	 */
	constructor(target, propName, elm) {
		::AbsoluteActionHandler.constructor(target, propName, elm);
		initValue = target[propName];
		start  = getval(elm, "start");
		min    = getint(elm, "min");
		max    = getint(elm, "max");
		loop   = getint(elm, "loop", time);
    }

	/**
	 * �ŏI�l
	 */
	function getResultValue() {
		return initValue;
	}
	
	/**
	 * ���s���񏈗�
	 */
	function init(target, propName) {
		if (start == null) {
			start = target[propName];
		} else {
			start = getRelative(start, initValue);
		}
		min = getRelative(min, initValue);
		max = getRelative(max, initValue);
		max   -= min;
		start -= min;
	}
	
	function doAction(target, propName, now, stopFlag) {
		if (stopFlag) {
			return true;
		} else {
			target[propName] = min + (start + max * (now%loop) / loop) % max;
			return false;
		}
	}
}


/**
 * �U���A�N�V�����i���΃A�N�V�����j
 * vibration : �U����
 * offset    : �␳�l
 */
class VibrateAction extends RelativeActionHandler {

	vibration = null;
	offset = null;

	constructor(target, propName, elm) {
		::RelativeActionHandler.constructor(target, propName, elm);
		local initValue = target[propName];
		vibration  = getRelative("value" in elm ? getval(elm, "value") : getval(elm, "vibration"), initValue);
		offset     = getRelative(getval(elm, "offset"), init.value);
    }
};

/**
 * �����_���U���A�N�V�����i���΃A�N�V�����j
 * vibration : �U����
 * offset    : �␳�l
 * waittime �U���Ԃ̑҂�����
 */
class RandomAction extends VibrateAction {

    waitTime = null;
    randomTime = null;
	randomValue = null;
    
	constructor(target, propName, elm) {
		::VibrateAction.constructor(target, propName, elm);
		waitTime    = getint(elm, "waittime");
		randomTime  = 0;
		randomValue = 0;
    }

    function doAction(target, propName, now, stopFlag) {
        if (stopFlag) {
            return true; 
        } else {
            if (now >= randomTime) {
				randomValue = (random(vibration * 2) - vibration) + offset;
				randomTime += waitTime;
			}
			target[propName] += randomValue;
			return false;
        }
    }
};

/**
 * ��`�g
 * vibration : �U����
 * offset    : �␳�l
 * ontime  : ON�̎���   �l:vibration
 * offtime : OFF�̎���  �l:-vibration
 */
class SquareAction extends VibrateAction {

    ontime = null;
    offtime = null;
	
	constructor(target, propName, elm) {
		::VibrateAction.constructor(target, propName, elm);
		ontime  = getint(elm, "ontime");
		offtime = getint(elm, "offtime");
    };

    function doAction(target, propName, now, stopFlag) {
        if (stopFlag) {
            return true;
        } else {
			target[propName] += ((now % (ontime + offtime) < ontime) ? vibration : -vibration) + offset;
			return false;
        }
    }
};

/**
 * �O�p�g
 * vibration : �U����
 * offset    : �␳�l
 * 0�`vibration ���J��Ԃ�
 * ontime  : �㏸����
 * offtime : ���H����
 */
class TriangleAction extends VibrateAction {

    ontime = null;
	offtime = null;

	constructor(target, propName, elm) {
		::VibrateAction.constructor(target, propName, elm);
		ontime  = getint(elm, "ontime");
		offtime = getint(elm, "offtime");
    };

    function doAction(target, propName, now, stopFlag) {
        if (stopFlag) {
            return true;
        } else {
            now = now % (ontime + offtime);
            local v;
            if (now <= ontime) {
                v = vibration * now / ontime;
            } else {
                if (offtime > 0) {
                    v = vibration * (offtime - (now - ontime)) / offtime;
                } else {
                    v = 0;
                }
            }
			target[propName] += v + offset;
			return false;
        }
    }
};

/**
 * �O�p�֐��U���A�N�V�����i���΃A�N�V�����j
 * vibration : �U����
 * offset    : �␳�l
 * cycle     : ����(ms)
 * angvel    : �p���x(�x/sec) ��cycle�w�莞�͖���
 */
class TrigonoAction extends VibrateAction {
    
    cycle = null;
	angvel = null;
    
    constructor(target, propName, elm) {
		::VibrateAction.constructor(target, propName, elm);
		cycle  = getint(elm, "cycle");
		angvel = getint(elm, "angvel");
    }

    function calcAngle(now) {
		return (cycle != null) ? (PI * 2 * now / cycle) : (PI * 2 * now * angvel / 360000.0);
    }
};

/**
 * sin�U���A�N�V�����i���΃A�N�V�����j
 * vibration : �U����
 * offset    : �␳�l
 * cycle     : ����(ms)
 * angvel    : �p���x(�x/sec) ��cycle�w�莞�͖���
 */
class SinAction extends TrigonoAction {

	constructor(target, propName, elm) {
		::TrigonoAction.constructor(target, propName, elm);
	}
    function doAction(target, propName, now, stopFlag) {
        if (stopFlag) {
            return true;
        } else {
			target[propName] += vibration * sin(calcAngle(now)) + offset;
			return false;
        }
    }
}

/**
 * cos�U���A�N�V�����i���΃A�N�V�����j
 * vibration : �U����
 * offset    : �␳�l
 * cycle     : ����(ms)
 * angvel    : �p���x(�x/sec) ��cycle�w�莞�͖���
 */
class CosAction extends TrigonoAction {

	constructor(target, propName, elm) {
		::TrigonoAction.constructor(target, propName, elm);
    }
    function doAction(target, propName, now, stopFlag) {
        if (stopFlag) {
            return true;
        } else {
			target[propName] += vibration * cos(calcAngle(now)) + offset;
            return false;
        } 
    }
}

/**
 * �����A�N�V����
 * distance ������
 * time ��������
 */
class FallAction extends RelativeActionHandler {

    distance = null;
    fallTime = null;
    
	constructor(target, propName, elm) {
		::RelativeActionHandler.consturctor(target, propName, elm);
		distance = getint(elm, "distance");
    }

    function doAction(target, propName, now, stopFlag) {
        if (stopFlag) {
            return true;
        } else {
			//dm("fall:" + now);
			target[propName] += distance * (now/time - 1.0);
            return false;
        }
    }
};

// ----------------------------------------------------------------
// ���Ō݊��p�@�\
// ----------------------------------------------------------------

function LayerNormalZoomModule(elm) {
	return {
		zoom = {
			handler = "MoveAction",
			start = getval(elm, "start"),
			value = getval(elm, "zoom"),
			time  = getval(elm, "time"),
			delay = getval(elm, "delay"),
			accel = getval(elm, "accel"),
		}
	};
}

function LayerToRotateModule(elm) {
	local rotate = getval(elm, "angle" in elm ? "angle" : "rotate");
	return {
		rotate = {
			handler = "MoveAction",
			start   = getval(elm, "start"),
			value   = rotate,
			time    = getval(elm, "time"),
			delay   = getval(elm, "delay"),
			accel   = getval(elm, "accel"),
		}
	};
}

function LayerFadeToModeModule(elm) {
	return {
		opacity= {
			handler = "MoveAction",
			start   = getval(elm, "start"),
			value   = getval(elm, "opacity"),
			time    = getval(elm, "time"),
			delay   = getval(elm, "delay"),
			accel   = getval(elm, "accel"),
		}
	};
}

function LayerMoveModule(elm, accel) {
	return {
		left = {
			handler = "MoveAction",
			start   = getval(elm, "startx"),
			value   = getval(elm, "x"),
			time    = getval(elm, "time"),
			delay   = getval(elm, "delay"),
			accel   = accel,
		},
		top = {
			handler = "MoveAction",
			start   = getval(elm, "starty"),
			value   = getval(elm, "y"),
			time    = getval(elm, "time"),
			delay   = getval(elm, "delay"),
			accel   = accel,
		}
	};
}

function LayerNormalMoveModule(elm) {
	return LayerMoveModule(elm, 0);
}

function LayerAccelMoveModule(elm) {
	return LayerMoveModule(elm, 1);
}

function LayerDecelMoveModule(elm) {
	return LayerMoveModule(elm, -1);
}

function LayerVibrateActionModule(elm) {
	return {
		left = {
			handler   = "RandomAction",
			vibration = getint(elm, "vibration")/2,
			waittime  = getval(elm, "waittime"),
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		},
		top   = {
			handler   = "RandomAction",
			vibration = getint(elm, "vibration")/2,
			waittime  = getval(elm, "waittime"),
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		}
	};
}

function LayerJumpActionModule(elm) {
	return {
		left =  null,
		top =  {
			handler   = "SinAction",
			vibration = getval(elm, "vibration"),
			cycle     = getval(elm, "cycle"),
			angvel    = getval(elm, "angvel"),
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		}
	};
}

function LayerJumpOnceActionModule(elm) {
	return {
		left =  null,
		top =  {
			handler   = "SinAction",
			vibration = -getint(elm, "vibration"),
			cycle     = getval(elm, "cycle"),
			angvel    = getval(elm, "angvel"),
			time      = getint(elm, "cycle")/2,
			delay     = getval(elm, "delay"),
		}
	};
}

function LayerWaveActionModule(elm) {
	return {
		top =  null,
		left =  {
			handler = "SinAction",
			vibration = getval(elm, "vibration"),
			cycle     = getval(elm, "cycle"),
			angvel    = getval(elm, "angvel"),
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		}
	};
}

function LayerWaveOnceActionModule(elm) {
	return {
		top =  null,
		left =  {
			handler = "SinAction",
			vibration = -getint(elm, "vibration"),
			cycle     = getval(elm, "cycle"),
			angvel    = getval(elm, "angvel"),
			time      = getval(elm, "cycle")/2,
			delay     = getval(elm, "delay"),
		},
	};
}

function LayerFallActionModule(elm) {
	local time  = "falltime" in elm ?
		getval(elm, "falltime") : getval(elm, "time");
	return {
		left =  null,
		top =  {
			handler  = "FallAction",
			distance = getval(elm, "distance"),
			delay    = getval(elm, "delay"),
			time     = time,
		}
	};
}

function LayerVRotateZoomModule(elm) {
	return {
		zoomy =  null,
		zoomx =  {
			handler   = "SinAction",
			vibration = "@",
			offset    = "-@",
			cycle     = getval(elm, "time"),
			angvel    = getval(elm, "angvel"),
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		},
	};
}

function LayerHRotateZoomModule(elm) {
	return {
		zoomx =  null,
		zoomy =  {
			handler   = "SinAction",
			vibration = "@",
			offset    = "-@",
			cycle     = getval(elm, "time"),
			angvel    = getval(elm, "angvel"),
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		},
	};
}

function LayerHeartBeatZoomModule(elm) {
	return {
		zoom =  {
			handler = "SinAction",
			vibration = getint(elm, "zoom")-100,
			cycle     = getint(elm, "time")*2,
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		}
	};
}

function LayerVibrateZoomModule(elm) {
	return {
		zoomx =  {
			handler = "RandomAction",
			vibration = getint(elm, "vibration")/2,
			waittime  = getval(elm, "waittime"),
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		},
		zoomy =  {
			handler = "RandomAction",
			vibration = getint(elm, "vibration")/2,
			waittime  = getval(elm, "waittime"),
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		}
	};
}

function LayerNormalRotateModule(elm) {
	local cycle = "angvel" in elm ? 360/getval(elm, "angvel")*1000
		: getval(elm, "cycle");
	return {
		rotate =  {
			handler = "TriangleAction",
			vibration = 360,
			ontime = cycle,
			offtime = 0,
			time  = getval(elm, "time"),
			delay = getval(elm, "delay"),
		}
	};
}

function LayerVibrateRotateModule(elm) {
 	return {
		rotate =  {
			handler   = "RandomAction",
			vibration = getint(elm, "vibration")/2,
			waittime  = getval(elm, "waittime"),
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		}
	};
}

function LayerFalldownRotateModule(elm) {
 	return {
		rotate =  {
			handler = "MoveAction",
			value   = "@+" + getval(elm, "angle"),
			accel   = 1,
			time    = getval(elm, "time"),
			delay   = getval(elm, "delay"),
		}
	};
}

function LayerBlinkModeModule(elm) {
 	return {
		visible =  {
			handler   = "SquareAction",
			vibration = 1,
			ontime    = getval(elm, "showtime"),
			offtime   = getval(elm, "hidetime"),
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		}
	};
}

function LayerFadeModeModule(elm) {
 	return {
		opacity =  {
			handler   = "TriangleAction",
			vibration = "@",
			offset    = "-@",
			ontime    = getval(elm, "intime"),
			offtime   = getval(elm, "outtime"),
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		},
	};
}
