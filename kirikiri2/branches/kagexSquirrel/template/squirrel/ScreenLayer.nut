/**
 * ��ʃt�F�[�h�����p�̃��C��
 */
class ScreenLayer extends BasicLayer
{
    // �I���������[�h
	mode = 0;
	// ���Ԏw��
    time = 0;
    // swap �t���O
    swap = false;

    // ���s����
    actionTime = null;
    
    constructor() {
		::BasicLayer.constructor(true);
    }

    /**
     * ���C���̍����ւ�����
     */
    function swapLayers() {
		if (swap) {
			foreach (idx,val in objects) {
				val.backDone();
            }
			swap = false;
		}
    }
    
    /**
	 * �t�F�[�h�������J�n����
     */
	function start(mode, time, swap) {
        this.mode = mode;
        this.time = time;
        this.swap = swap;
        actionTime = 0;
        if (mode == 2) {
            // �t�F�[�h�C���̏ꍇ�͂܂��X���b�v
            swapLayers();
        }
    }

    /**
     * �����n���h��
     */
    function handler(now, stopFlag) {
        //print("screen:" + now + "\n");
        local done = false;
        if (isSkip() || stopFlag) {
            now = time;
        }
        if (now >= time) {
            done = true;
        }
        switch (mode) {
        case 1:
            // fadeout
			opacity = 100 * now / time;
            break;
        case 2:
            // fadein
			opacity = 100 - 100 * now / time;
            break;
        case 3:
            // fadeout/in
            {
                local time2 = time / 2;
                if (now < time2) {
                    // fadeout
					opacity = 100 * now / time2;
                } else {
                    // fadein
                    swapLayers();
                    local n = now - time2;
					opacity = 100 - 100 * n / time2;
                }
            }
            break;
        }
        if (done) {
            swapLayers();
            actionTime = null;
            if (mode ==2 || mode == 3) {
                screenLayer.visible = false;
            }
            trigger("screen");
        }
    }

    function update(diff) {
        if (actionTime != null) {
            actionTime += diff;
            handler(actionTime, false);
        }
    }

    function stop() {
        handler(actionTime, true);
        actionTime = null;
    }
};
