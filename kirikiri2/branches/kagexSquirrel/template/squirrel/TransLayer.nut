/**
 * ��ʐ؂�ւ��������C��
 */
class TransLayer {

    // XXX �{���V�X�e��������^����ׂ�
    width = 640;
    height = 448;

    info=null;       //< ��ʏ��
    time=0;          //< �؂�ւ��ɂ����鎞��
    color=0x000000;  //< �h��Ԃ��F
    paint = true;    //< �h��Ԃ�����
    
	first=true;   //< ����t���O
    now=0;        //< ���s����
    count=0;      //< ���݂̃J�E���g
    maxCount=0;   //< �ő�J�E���g

    // �h��Ԃ��̈�L�^�p
    rects = [];

    function clear() {
        for (local i=rects.len() - 1; i>=0; i--) {
            rects.remove(i);
        }
    }
    
    /**
     * �S�ʓh��Ԃ�
     */
    function fill() {
        clear();
        if (paint) {
			rects.append(FillRect(0,0,width,height,0,color));
        }
    }

    /**
     * �h��Ԃ��̈�ǉ�
     */
    function fillRect(x, y, w, h) {
        if (w > 0 && h > 0) {
			rects.append(FillRect(x, y, w, h, 0, color));
        }
    }

    // -----------------------------------------

    // ���C�v�̕����w��
    wipe_size = 16;

    /*
     * ���E���C�v������
     */
    function initWipeLR(elm) {
		wipe_size = getval(elm,"size",16);
        maxCount  = ((width+wipe_size-1)/wipe_size)+wipe_size-1;
    }

    /**
     * ������E
     */
    function wipeltor_effect(count) {
        local cnt = count;
        for (local x=0; x<width; x+=wipe_size) {
            local size = (cnt > wipe_size) ? wipe_size : (cnt < 0) ? 0 : cnt;
            if (paint) {
                fillRect(x, 0, size, height);
            } else {
                fillRect(x + size, 0, wipe_size - size, height);
            }
            cnt--;
        }
    }

    /**
     * �E���獶
     */
    function wipertol_effect(count) {
        local cnt = count;
        for (local x=width; x>0; x-=wipe_size) {
            local size = (cnt > wipe_size) ? wipe_size : (cnt < 0) ? 0 : cnt;
            if (paint) {
                fillRect(x-size, 0, size, height);
            } else {
                fillRect(x-wipe_size, 0, wipe_size - size, height);
            }
            cnt--;
        }
    }
    
    types = null;
    
    /**
     * �^�C�}�쓮��
     */
    function update(diff) {
		if (now != null) {
	        if (first) {
    	        count = -1;
        	    now = 0;
            	first = false;
	        } else {	
    	        now += diff;
	        }
	        // �`�揈��
	        local newcount;
	        if ((newcount = (now * maxCount)/time) > count) {
                clear();
                if (newcount >= maxCount) {
                    newcount = maxCount;
	            }
                info.effect(newcount);
        	    count = newcount;
	        } 
	        // �I������
    	    if (count >= maxCount) {
        	    done();
	            trigger("layertrans");
	        }
		}
    }

    /**
     * �`�揈��
     * @param elm
     * @param ���
     * @param time ����
     * @param color �F�w��
     */
    function start(elm, type, time, color) {
        if (time != null) {
            this.time = time;
        }
        if (color != null) {
			this.color = color;
			paint = true;
        } else {
            paint = false;
        }
        if (type != null && type in types) {
            info = types[type];
        }
        if (info == null) {
            info = types["wipeltor"];
        }
        info.init(elm);
		if (time == 0) {
			done();
		} else {
			first = true;
			now = 0;
		}
    }

    /**
     * �I��
     */
    function done() {
        now = null;
        fill();
    }

    /**
     * �����I��
     */
    function stop() {
        done();
        paint = false;
        fill();
    }

	constructor() {
		types = {
            // ������E�Ƀ��C�v
			wipeltor = {
				init   = initWipeLR.bindenv(this),
				effect = wipeltor_effect.bindenv(this)
                },
            wipertol = {
                init   = initWipeLR.bindenv(this),
                effect = wipertol_effect.bindenv(this)
                }
        };
    }
}
