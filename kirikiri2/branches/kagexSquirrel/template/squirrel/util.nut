//
// ���[�e�B���e�B��`
// 

/**
 * �O���[�o���֐� replace
 * ������̕����u�����s���܂�
 * @param str
 * @param from
 * @param to
 */
//function replace(str,from,to)

/**
 * �O���[�o���֐� iskanji
 * ������1�����ڂ��ǂ������肵�܂�
 * @param ch ����
 */
//function iskanji(ch)

/**
 * ���b�Z�[�W�\��
 */
function dm(msg)
{
	print(msg + "\n");
}

/**
 * �]������
 * @param eval ������
 * @return ���茋��
 */
function eval(eval) {
	local script = "return " + eval;
	return compilestring(script)();
}

/**
 * �e�[�u������ name �Ŏ������v�f�����o��
 * @param table �����Ώۂ̃e�[�u��
 * @param name  ���ڂ̖��O
 * @parem �f�t�H���g�l
 */
function getval(table, name, ...) 
{
	if (name in table) {
		return table[name];
	} else if (vargc > 0) {
		return vargv[0];
	}
}

/**
 * �e�[�u������ name �Ŏ������v�f�𐔒l�܂���null �Ƃ��ĂƂ肾��
 * @param table �����Ώۂ̃e�[�u��
 * @param name  ���ڂ̖��O
 */
function getint(table, name, ...) 
{
	if (name in table) {
		return table[name].tointeger();
	} else if (vargc > 0) {
		return vargv[0];
	}
}

/**
 * �l���琔�l���Ƃ肾���B���� null �̏ꍇ�͂��̂܂�
 * @param value �����Ώۂ̒l
 */
function toint(value, ...)
{
	if (value != null) {
		return value.tointeger();
	} else if (vargc > 0) {
		return vargv[0];
	}
}

/**
 * �e�[�u������ name �Ŏ������v�f�𐔒l�܂���null �Ƃ��ĂƂ肾��
 * @param table �����Ώۂ̃e�[�u��
 * @param name  ���ڂ̖��O
 */
function getfloat(table, name, ...) 
{
	if (name in table) {
		return table[name].tofloat();
	} else if (vargc > 0) {
		return vargv[0];
	}
}

/**
 * �l���琔�l���Ƃ肾���B���� null �̏ꍇ�͂��̂܂�
 * @param value �����Ώۂ̒l
 */
function tofloat(value, ...)
{
	if (value != null) {
		return value.tofloat();
	} else if (vargc > 0) {
		return vargv[0];
	}
}

/**
 * ��������
 */
function random(a)
{
	local d = rand().tofloat() / RAND_MAX * a;
    return  d.tointeger() ;
}

/**
 * �F�ϊ�����
 */
function colorConv(col)
{
    local a = (col >> 24) & 0xff;
    local r = (col >> 16) & 0xff;
    local g = (col >> 8)  & 0xff;
    local b = (col)       & 0xff;
	return (r * 128 / 255) << 24 | (g * 128 / 255) << 16 | (b * 128 / 255) << 8 | (a * 128 / 255);
}

/**
 * �p����Ԃ̊m�F
 * @param target �����Ώ�
 * @param source ���ɂȂ�N���X
 * @return target �� source ���p�����Ă����� true
 */
function inherited(target, source)
{
	local p = target.parent;
	while (p != null) {
		if (p == source) {
			return true;
		}
		p = p.parent;
	}
	return false
}

/**
 * �z�񂩂�f�[�^���폜����
 * @param array �z��
 * @value �폜����l
 */
function removeFromArray(array, value)
{
	local i = 0;
	while (i < array.len()) {
		if (array[i] == value) {
			array.remove(i);
		} else {
			i++;
		}
	}
}
