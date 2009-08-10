#include "sqcont.h"
#include "sqthread.h"

namespace sqobject {

ObjectInfo continuousList;

/// �n���h���o�^
static SQRESULT addContinuousHandler(HSQUIRRELVM v)
{
	ObjectInfo info(v,-1);
	if (info.isClosure()) {
		SQInteger max = continuousList.len();
		for (SQInteger i=0;i<max;i++) {
			ObjectInfo f = continuousList.get(i);
			if (f == info) {
				return SQ_OK;
			}
		}
		continuousList.append(info);
	}
	return SQ_OK;
}

/// �n���h���폜
static SQRESULT removeContinuousHandler(HSQUIRRELVM v)
{
	ObjectInfo info(v,-1);
	if (info.isClosure()) {
		SQInteger max = continuousList.len();
		SQInteger i=0;
		while (i<max) {
			if (continuousList.get(i) == info) {
				continuousList.remove(i);
				max--;
			} else {
				i++;
			}
		}
	}
	return SQ_OK;
}

static SQRESULT clearContinuousHandler(HSQUIRRELVM v)
{
	continuousList.clearData();
	return SQ_OK;
}


/// �@�\�o�^
void registerContinuous()
{
#define REGISTERMETHOD(name) \
	sq_pushstring(v, _SC(#name), -1);\
	sq_newclosure(v, name, 0);\
	sq_createslot(v, -3);

	HSQUIRRELVM v = getGlobalVM();
	sq_pushroottable(v); // root
	REGISTERMETHOD(addContinuousHandler);
	REGISTERMETHOD(removeContinuousHandler);
	REGISTERMETHOD(clearContinuousHandler);
	sq_pop(v,1);

	continuousList.initArray();
}

/// �n���h�������Ăяo���BThread::main �̌�ŌĂяo���K�v������
void mainContinuous()
{
	SQInteger max = continuousList.len();
	for (SQInteger i=0;i<max;i++) {
		continuousList.get(i).call(Thread::currentTick, Thread::diffTick);
	}
}

/// �@�\�I��
void doneContinuous()
{
	continuousList.clearData();
	continuousList.clear();
}

};

