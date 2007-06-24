// ------------------------------------------------------
// �C�x���g�����p
// ------------------------------------------------------

currentPlayer <- null;  // ���݂̏����v���C���[

// �L�[�}�b�v
keyMap <- [];

function keyClear() {
	keyMap = [];
}

function onKeyPress(key) {
	//keyMap[key.tointeger()] <- true;
	if (currentPlayer) {
		currentPlayer.onKeyPress(key);
	}
}

function onKeyRelease(key) {
	//keyMap[key.tointeger()] <- false;
	if (currentPlayer) {
		currentPlayer.onKeyRelease(key);
	}
}

/**
 * �t�F�[�h�I��������
 */
function onMusicFadeCompleted(ch)
{
	if (currentPlayer) {
		if (ch == 0) {
			currentPlayer.onBgmFadeCompleted();
		} else {
			currentPlayer.onSEFadeComplsted(ch);
		}
	}
}

/**
 * �Đ��I��������
 */
function onMusicStop(ch)
{
	if (currentPlayer) {
		if (ch == 0) {
			currentPlayer.onBgmStop();
		} else {
			currentPlayer.onSEStop(ch);
		}
	}
}

// ------------------------------------------------------

currentTime   <- 0;     // ���ݎ���
endFlag       <- false; // �I���t���O

// �Q�[���S�̂̃X�e�[�g
STATE_TITLE  <- 0;
STATE_CONFIG <- 1;
STATE_GAME   <- 2;
state <- 0;

scenePlayer <- null;

/**
 * �Q�[����ԕύX�w��
 */
function setState(state) {
	::state = state;
	switch (state) {
	case STATE_TITLE:
		currentPlayer = null;
		break;
	case STATE_CONFIG:
		currentPlayer = null;
		break;
	case STATE_GAME:
		currentPlayer = scenePlayer;
		break;
	}
}


// �^�C�g�������p
function title(diff)
{
}

// �R���t�B�O�����p
function config(diff)
{
}

// �Q�[�����C�������p
function game(diff)
{
	local ret = currentPlayer.main(diff);
	switch (ret) {
	case currentPlayer.WAIT:
		break;
	case currentPlayer.STOP:
		setState(STATE_TITLE);
		break;
	}
}

/**
 * �Q�[���{�̃��C�����[�v
 */
function main(diff)
{
	currentTime += diff;
	switch (state) {
	case STATE_TITLE:
		title(diff);
		break;
	case STATE_CONFIG:
		config(diff);
		break;
	case STATE_GAME:
		game(diff);
		break;
	}
	return endFlag;
}

startScene<-null;

function setStartScene(startscene)
{
	::startScene = startscene;
}

/**
 * �V�X�e������������
 */
function init()
{
	scenePlayer <- ScenePlayer();
	if (startScene != null) {
		print("�V�[��:" + startScene + "����Đ��J�n���܂�");
		scenePlayer.start(startScene);
		setState(STATE_GAME);
	}
}

function start(name)
{
	setStartScene(name);
	init();
}
