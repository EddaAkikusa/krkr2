/**
 * squirrel �p��{���\�b�h�p
 */
#include <squirrel.h>
#include <sqstdio.h>
#include <sqstdblob.h>
#include <sqstdaux.h>

#include <string>
typedef std::basic_string<SQChar> tstring;


/**
 * ������u���֐�
 * @param str �u���Ώ�
 * @param from �u����
 * @param to �u����
 */
static SQInteger
replace(HSQUIRRELVM v)
{
	const SQChar *str; // �u���Ώ�
	const SQChar *from;  // �u����
	const SQChar *to;  // �u����
	if (SQ_SUCCEEDED(sq_getstring(v, 2, &str))) {
		if (SQ_SUCCEEDED(sq_getstring(v, 3, &from)) &&
			SQ_SUCCEEDED(sq_getstring(v, 4, &to))) {
			int nFromLen = scstrlen(from);
			const SQChar *start = str;
			const SQChar *p;
			tstring result;
			while ((p = scstrstr(start, from)) != NULL) {
				// �J�n�ʒu�܂ŃR�s�[
				while (start < p) {
					result += *start++;
				}
				// �u�����T�C�Y�����炷
				start = p + nFromLen;
				// �u������R�s�[
				result += to;
			}
			// �c��̕�������R�s�[
			result += start;
			sq_pushstring(v, result.c_str(), -1);
			return 1;
		} else {
			sq_pushstring(v, str, -1);
			return 1;
		}
	}
	return 0;
}

/**
 * �X�^�b�N���̕\��
 */
static SQInteger
printCallStack(HSQUIRRELVM v)
{
	sqstd_printcallstack(v);
	return 0;
}

/**
 * �K�[�x�[�W�R���N�g�N��
 */
static SQInteger
collectGarbage(HSQUIRRELVM v)
{
	sq_collectgarbage(v);
	return 0;
}

void
sqbasic_init(HSQUIRRELVM v)
{
	sq_pushroottable(v);

	// ���̑��̊�{���C�u�����̓o�^
	sqstd_register_iolib(v);
	sqstd_register_bloblib(v);
	
	// replace �̓o�^
	sq_pushstring(v, _SC("replace"), -1);
	sq_newclosure(v, replace, 0);
	sq_createslot(v, -3); 

	// printCallStack �̓o�^
	sq_pushstring(v, _SC("printCallStack"), -1);
	sq_newclosure(v, printCallStack, 0);
	sq_createslot(v, -3); 
	
	// collectGarbage �̓o�^
	sq_pushstring(v, _SC("collectGarbage"), -1);
	sq_newclosure(v, collectGarbage, 0);
	sq_createslot(v, -3); 
	
	sq_pop(v, 1);
}
