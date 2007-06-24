/**
 * �Đ������̊��N���X
 * �E�^�C���A�E�g���҂��@�\
 * �E�A�N�V�����@�\
 * �E�O���t�@���N�V�����@�\
 * ��񋟂���
 */
class Player {

	// ---------------------------------------------------
	// ���ʏ���
	// ---------------------------------------------------
	
	function dm(msg) {
		print(msg);
	}

	// ---------------------------------------------------
	// ������
	// ---------------------------------------------------
	
	STOP = 0; // ��~
	RUN  = 1; // ���쒆
	WAIT = 2; // �҂�
	status = 0; // �V�i���I���

	// �҂����
	waitUntil = {};

	// ���ԏ��
	currentTick = 0;  // ���݂̎��s����
	timeoutTick = null;  // ���ԑ҂��p
	
	// ----------------------------------------------------
	// �V�X�e���{��
	// ----------------------------------------------------
	
	/**
	 * �R���X�g���N�^
	 */
	constructor() {
	}

	/**
	 * �V�X�e�����X�L�b�v��Ԃɂ��邩
	 */
	function isSkip() {
		return false;
	}
	
	/**
	 * �҂����̔p��
	 */
	function clearWait() {
		foreach (idx,val in waitUntil) {
			delete waitUntil[idx];
		}
	}

	/**
	 * ��Ԃ̃N���A
	 */
	function clear() {
		clearWait();
		killFunc();
		status = STOP;
	}

	/**
	 * �҂�
	 * param util �V�O�i�����ƃR�[���o�b�N�֐��̎����z��
	 */
	function wait(until) {
		status = WAIT;
		foreach (name,value in until) {
			waitUntil[name] <- value;
		}
	}

	/**
	 * �^�C���A�E�g���̑҂�
	 * �^�C���A�E�g���ɂ� 'timeout' ���g���K�����
	 * @param until �҂����
	 * @param timeout �^�C���A�E�g����
	 */
	function waitWithTimeout(until, timeout) {
		wait(until);
		if (timeout == 0) timeout = 1; // timeout �� 0 �̏ꍇ�� 1 ��
		timeoutTick = currentTick + timeout;
	}

	/**
	 * �^�C���A�E�g���̍X�V
	 * @param diff �o�ߎ���
	 */
	function updateTimeout(diff) {
		// �^�C���A�E�g����
		if (timeoutTick != null) {
			timeoutTick -= diff;
			if (timeoutTick < 0) {
				trigger("timeout");
				timeoutTick = null;
			}
		}
	}
	
	/**
	 * �҂������g���K
	 *
	 * waitUntil ���ɃV�O�i���� name �����݂���΁A���s�ĊJ�A
	 * ������ waitUntil �ɓo�^���ꂽ���\�b�h(���X�^�[�g�n���h��)���Ă�
	 * �V�O�i������ _arg ���������̂� waitUntil ���ɂ���΁A
	 * ����������Ƃ��ăn���h���ɓn��
	 * waitUntil �̓N���A�����
	 * @param name �g���K��
	 * @return �g���K���������� true
	 */
	function trigger(name) {
		if (status != WAIT) return false;
		if (name in waitUntil) {
			local func = waitUntil[name];
			local argname = name + "_arg";
			if (argname in waitUntil) {
				func(waitUntil[arg]);
			} else {
				func();
			}
			clearWait();
			status = RUN;
			return true;
		} else {
			return false;
		}
	}

	/**
	 * �g���K�҂�
	 * @param name �g���K��
	 * @param waittime �^�C���A�E�g����
	 * @param canskip �X�L�b�v�\��
	 * @param onskip �X�L�b�v���Ɏ��s���鏈��
	 * @return �҂���ԂȂ� -2
	 */
	function waitTrigger(name, waittime, canskip, onskip) {
		// elm.name �Ŏ����ꂽ�g���K��҂�
		if (canskip)	{
			// �X�L�b�v�ł���ꍇ
			if (isSkip()) {
				// �X�L�b�v���쒆
				if (onskip != null) eval(onskip);
				return 0; // �����ɕԂ�
			}
			waitWithTimeout({
				click = function(arg) {	if (arg != null) eval(arg);	} 
				click_arg = onskip,
				[elm.name] = function() {},
				timeout = function() {},
			}, waittime);
		} else {
			waitWithTimeout({
				[elm.name] = function(){},
				timeout = function() {},
			});
		}
		return -2;
	}
	
	/**
	 * ���ԑ҂�
	 * @param waittime �҂�����
	 * @param canskip  �X�L�b�v�\��
	 * @return �҂���ԂȂ� -2
	 */
	function waitTime(waittime, canskip) {
		if (waittime <= 0) {
			return 0;
		}
		if (canskip) {
			if (isSkip()) {
				return 0;
			}
			waitWithTimeout({
				click = function() {},
				timeout = function() {},
			}, waittime);
		} else {
			waitWithTimeout({
				timeout = function() {},
			}, waittime);
		}
		return -2;
	}

	// ------------------------------------------------
	// �A�N�V��������
	// ------------------------------------------------

    // �A�N�V�����ΏۑS��
	allActions = {};
	actionCount = 0;

	/**
	 * �A�N�V�������s����
	 */
	function isInAction(target) {
		return target in allActions;
	}

	/**
	 * �A�N�V���������R�s�[����
	 * @param src �R�s�[��
	 * @param dest �R�s�[��
	 * @param completed �I��������(�֐�)
	 */
	function copyAction(src, dest, completed) {
		//dm("�A�N�V�������̃R�s�[");
		stopAction(dest);
		if (src in allActions) {
			local srcAction  = allActions[src];
			local destAction = srcAction.cloneObj();
			destAction.target = dest;
			if (completed != null) {
				destAction.onActionCompleted = completed;
			}
			allActions[dest] = destAction.weakref();
			actionCount++;
		}
	}

	/**
	 * �A�N�V�������s���ʂ�Ԃ�
	 * @param target �^�[�Q�b�g�I�u�W�F�N�g
	 */
	function getActionResult(target) {
		if (target in allActions) {
			local info = allActions[target];
			return info.getResultValue();
		}
	}
	
    /**
	 * �A�N�V������o�^
	 * @param target �^�[�Q�b�g�I�u�W�F�N�g
     * @param action �A�N�V�������
	 * @param completed �I���n���h��
	 * @param nowait ������҂��Ȃ��w��
	 * @return ���̃A�N�V�����̎��s����
     */
	function beginAction(target, action, completed, nowait) {
		//dm("�A�N�V�����J�n:" + nowait + ":" + action.nowait);
		if (!(target in allActions)) {
			info = ActionSequense(target);
			allActions[target] = info.weakref();
			actionCount++;
		}
		if (completed != null) {
			// �I���n���h����o�^
			info.onActionCompleted = completed;
		}
        
        // ������̏ꍇ�͕]�����Ă��܂�
		if (typeof action == "string") {
			action = eval(action);
        }
        
		if (typeof action == "table") {
			//dm("�����A�N�V����:" + action);
			//showKeys("�A�N�V����", action);
			// �����̏ꍇ
			if ("module" in action) {
				if (typeof action.module != "string") {
					throw "�A�N�V�������W���[�����͕�����w�肵�Ă�������";
				}
				local module = eval(action.module);
				if (module != null && typeof module == "function") {
					info.addActions(module(action));
				}
			} else {
				// �����A�N�V�����̓����w��
				info.addActions(action);
			}
			if (getint(action, "next")) {
				info.next();
			}
			if (getint(action, "nowait")) {
				info.nowait = action.nowait;
			}
		} else if (typeof action == "array") {
			//dm("�V�[�P���X�A�N�V����");
			for (local i=0;i<action.len();i++) {
				info.addActions(action[i]);
				info.next();
			}
		}
		if (nowait != null) {
			info.nowait = nowait;
		}
		return info.getResultValue();
	}
    
    /**
     * �A�N�V�������~
     * @param target �^�[�Q�b�g�I�u�W�F�N�g
     */
	function stopAction(target) {
		if (target in allActions) {
			//dm("�w��A�N�V������~:" + target.name);
			actionCount--;
			info.stopAction();
			if ("onActionCompleted" in info &&
				typeof info.onActionCompleted == "function") {
				info.onActionCompleted(info.target);
			}
			delete allActions[target];
        }
    }

    /**
	 * �S�A�N�V�����̒�~
	 * @param all true �Ȃ�{���ɑS����~ false �̏ꍇ nowait �w��̂�����̂͂Ƃ߂Ȃ�
     */
	function stopAllActions(all) {
		//dm("�S�A�N�V������~:" + all);
		foreach (name, info in allActions) {
			if (all || !getint(info, "nowait")) {
				//dm("��~�ΏۃA�N�V����:" + name + ": " + info.nowait);
				actionCount--;
				info.stopAction();
				if ("onActionCompleted" in info &&
					typeof info.onActionCompleted == "function") {
					info.onActionCompleted(info.target);
				}
				delete allActions[name];
			}
		}
	}
    
    /**
     * �A�N�V������ԍX�V
     * �V�X�e���S�̂���Ăяo��
     * @param now ���ݎ���
     */
    function updateAction(now) {
		foreach (name, info in allActions) {
			if (info.doAction(now, false)) {
				//dm("�A�N�V�����I��:" + info.target);
				actionCount--;
				if ("onActionCompleted" in info &&
					typeof info.onActionCompleted == "function") {
					info.onActionCompleted(info.target);
				}
				delete allActions[name];
			}
        }
    }
    
	/**
	 * �A�N�V�����I��
	 * @param self �A�N�V�����̎��s�Ώ�
     */
	function onActionCompleted(self) {
		// �ʃA�N�V������~
		trigger("action_" + self);
		if (actionCount == 0) {
			// �S�A�N�V������~
			trigger("action");
		}
    }

	/**
	 * �A�N�V�����҂�
	 * @param target �A�N�V�����Ώ�
	 * @param canskip �X�L�b�v�ł��邩�ǂ���
	 */
	function waitAction(target, canskip) {
		if (!isInAction(target)) return 0; // �A�N�V������҂ĂȂ�
		local name = "action_" + target.tostring();
		if (canskip) {
			// �X�L�b�v�ł���ꍇ
			if (isSkip()) {
				// �X�L�b�v���쒆
				stopAction(target);
				return 0; // �����ړ����~�����Ă����ɕԂ�
			}
			wait({
				click = function (target) {	stopAction(target); },
				click_arg = target, // �n���h���ւ̈���
				[name] = function() {} 
			});
		} else {
			wait({[name] = function() {}});
		}
		return -2;
	}

	/**
	 * �S�A�N�V������҂�
	 * @param canskip �X�L�b�v�ł��邩�ǂ���
	 */
	function waitAllAction(canskip)	{
		// �A�N�V������҂�
		if (actionCount == 0) return 0; // �A�N�V������҂ĂȂ�
		if (calskip) {
			// �X�L�b�v�ł���ꍇ
			if (isSkip()) {
				// �X�La�b�v���쒆
				stopAllActions(false);
				return 0; // �����ړ����~�����Ă����ɕԂ�
			}
			wait({
				click = function() { stopAllActions(false); },
				action = function() {}
			});
		} else {
			wait({action = function(){}});
		}
		return -2;
	}

	// ------------------------------------------------
	// �@�\�t�@���N�V����
	// ------------------------------------------------

	// �@�\�t�@���N�V����
	funcs = [];

	// �t�@���N�V�����n�̋����I��
	function stopFunc()	{
		for (local i=funcs.len() - 1; i>=0; i--) {
			funcs[i].done();
		}
	}
	
	// �t�@���N�V�����n�̃N���b�N����
	function clickFunc() {
		for (local i=funcs.len() - 1; i>=0; i--) {
			funcs[i].click();
		}
	}
	
	// �t�@���N�V�����n�̋����j��
	function killFunc() {
		if (funcs.len() > 0) {
			for (local i=funcs.len() - 1; i>=0; i--) {
				local func = funcs[i];
				funcs.remove(i);
			}
			trigger("func");
		}
	}

	/**
	 * �@�\�I���҂�
	 */
	function waitFunc(func, waittime, canskip) {
		if (canskip) {
			//print("Func�҂��X�L�b�v�\");
			if (isSkip()) {
				stopFunc();
				return 0;
			}
			funcs.append(func);
			waitWithTimeout({
				click = function() { stopFunc(); },
				func = function() {},
				timeout = function() { stopFunc(); },
			}, waittime);
		} else {
			//print("Func�҂��X�L�b�v�s��");
			waitWithTimeout(func);
			addWait({
				func = function() {},
				timeout = function() { stopFunc();},
			}, waittime);
		}
	}

	/**
	 * ����@�\��ԍX�V
	 */
	function updateFunc(diff) {
		if (funcs.len() > 0) {
			for (local i=funcs.len() - 1; i>=0; i--) {
				local func = funcs[i];
				if (!func.update(diff)) {
					funcs.remove(i);
				}
			}
			if (funcs.len() == 0) {
				trigger("func");
			}
		}
	}

	// ------------------------------------------------------
	// ���C�����W�b�N
	// ------------------------------------------------------

	/**
	 * �X�V����
	 */
	function update(diff) {
		currentTick += diff;
		updateFunc(diff);
		updateTimeout(diff);
		updateAction(currentTick);
	}

	/**
	 * ���s����
	 */
	function run() {
	}

	/**
	 * ���C��
	 */
	function main(diff)	{
		update(diff);
		if (status == RUN) {
			run();
		}
		return status;
	}

	// ------------------------------------------------------
	// �O���C�x���g
	// ------------------------------------------------------
	
	function onKeyPress(key) {
		print("�L�[�v���X:" + key + "\n");
	}

	function onKeyRelease(key) {
		print("�L�[�����[�X:" + key + "\n");
	}

	/**
	 * BGM�̃t�F�[�h���I������
	 */
	function onBgmFadeCompleted() {
	}

	/**
	 * BGM ����~����
	 */
	function onBgmStop() {
	}

	/**
	 * SE �̃t�F�[�h����~����
	 */
	function onSEFadeCompleted(id) {
	}

	/**
	 * SE ����~����
	 */
	function onSEStop(id) {
	}

	/**
	 * �S���Z�b�g���̏���
	 */
	function onResetAll() {
	}
}
