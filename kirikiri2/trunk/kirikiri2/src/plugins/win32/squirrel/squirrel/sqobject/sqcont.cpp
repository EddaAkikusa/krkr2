#include "sqcont.h"
#include "sqthread.h"

namespace sqobject {

static std::vector<ObjectInfo> continuousList; ///< �X���b�h�ꗗ

/// �n���h���o�^
static SQRESULT addContinuousHandler(HSQUIRRELVM v)
{
	ObjectInfo info(v,-1);
	if (info.isClosure()) {
		std::vector<ObjectInfo>::iterator i = continuousList.begin();
		while (i != continuousList.end()) {
			if (*i == info) {
				return SQ_OK;
			}
			i++;
		}
		continuousList.push_back(info);
	}
	return SQ_OK;
}

/// �n���h���폜
static SQRESULT removeContinuousHandler(HSQUIRRELVM v)
{
	ObjectInfo info(v,-1);
	if (info.isClosure()) {
		std::vector<ObjectInfo>::iterator i = continuousList.begin();
		while (i != continuousList.end()) {
			if (*i == info) {
				i = continuousList.erase(i);
			} else {
				i++;
			}
		}
	}
	return SQ_OK;
}

static SQRESULT clearContinuousHandler(HSQUIRRELVM v)
{
	continuousList.clear();
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
}

/// �n���h�������Ăяo���BThread::main �̌�ŌĂяo���K�v������
void mainContinuous()
{
	std::vector<ObjectInfo>::iterator i = continuousList.begin();
	while (i != continuousList.end()) {
		i->call(Thread::currentTick, Thread::diffTick);
		i++;
	}
}

/// �@�\�I��
void doneContinuous()
{
	continuousList.clear();
}

};

