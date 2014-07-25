#include "tjsobj.h"
extern Local<Value> toJSValue(Isolate *, const tTJSVariant &variant);
extern tTJSVariant toVariant(Isolate *, Handle<Value> value);

/**
 * Javascript�ɑ΂��ăG���[�ʒm
 */
Handle<Value>
ERROR_KRKR(Isolate *isolate, tjs_error error)
{
	switch (error) {
	case TJS_E_MEMBERNOTFOUND:
		return isolate->ThrowException(String::NewFromUtf8(isolate, "member not found"));
	case TJS_E_NOTIMPL:
		return isolate->ThrowException(String::NewFromUtf8(isolate, "not implemented"));
	case TJS_E_INVALIDPARAM:
		return isolate->ThrowException(String::NewFromUtf8(isolate, "invalid param"));
	case TJS_E_BADPARAMCOUNT:
		return isolate->ThrowException(String::NewFromUtf8(isolate, "bad param count"));
	case TJS_E_INVALIDTYPE:
		return isolate->ThrowException(String::NewFromUtf8(isolate, "invalid type"));
	case TJS_E_INVALIDOBJECT:
		return isolate->ThrowException(String::NewFromUtf8(isolate, "invalid object"));
	case TJS_E_ACCESSDENYED:
		return isolate->ThrowException(String::NewFromUtf8(isolate, "access denyed"));
	case TJS_E_NATIVECLASSCRASH:
		return isolate->ThrowException(String::NewFromUtf8(isolate, "navive class crash"));
	default:
		return isolate->ThrowException(String::NewFromUtf8(isolate, "failed"));
	}
}

Handle<Value>
ERROR_BADINSTANCE(Isolate *isolate)
{
	return isolate->ThrowException(String::NewFromUtf8(isolate, "bad instance"));
}

//----------------------------------------------------------------------------
// tTJSVariant���I�u�W�F�N�g�Ƃ��ĕێ����邽�߂̋@�\
//----------------------------------------------------------------------------

Persistent<ObjectTemplate> TJSObject::objectTemplate;

// �I�u�W�F�N�g��`������
void
TJSObject::init(Isolate *isolate)
{
	HandleScope handle_scope(isolate);
	Local<ObjectTemplate> obj = ObjectTemplate::New(isolate);
	obj->SetNamedPropertyHandler(getter, setter);
	obj->SetCallAsFunctionHandler(caller);
	objectTemplate.Reset(isolate, obj);
}

// �I�u�W�F�N�g��`���
void
TJSObject::done(Isolate *isolate)
{
	HandleScope handle_scope(isolate);
	objectTemplate.Reset();
}

// �R���X�g���N�^
TJSObject::TJSObject(Isolate *isolate, Handle<Object> obj, const tTJSVariant &variant) : TJSBase(variant)
{
	HandleScope handle_scope(isolate);
	wrap(isolate, obj);
	Persistent<Object> ref(isolate, obj);
	ref.SetWeak(this, release); 
}

// �v���p�e�B�̎擾
void
TJSObject::getter(Local<String> property, const PropertyCallbackInfo<Value>& info)
{
	HandleScope handle_scope(info.GetIsolate());
	String::Value propName(property);
	if (wcscmp(*propName, TJSINSTANCENAME) == 0) {
		return;
	}
	tTJSVariant self;
	if (getVariant(self, info.This())) {
		tjs_error error;
		tTJSVariant result;
		if (TJS_SUCCEEDED(error = self.AsObjectClosureNoAddRef().PropGet(0, *propName, NULL, &result, NULL))) {
			info.GetReturnValue().Set(toJSValue(info.GetIsolate(), result));
		} else {
			info.GetReturnValue().Set(ERROR_KRKR(info.GetIsolate(), error));
		}
		return;
	}
	info.GetReturnValue().Set(ERROR_BADINSTANCE(info.GetIsolate()));
}

// �v���p�e�B�̐ݒ�
void
TJSObject::setter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<Value>& info)
{
	HandleScope handle_scope(info.GetIsolate());
	tTJSVariant self;
	if (getVariant(self, info.This())) {
		String::Value propName(property);
		tTJSVariant param = toVariant(info.GetIsolate(), value);
		tjs_error error;
		if (TJS_SUCCEEDED(error = self.AsObjectClosureNoAddRef().PropSet(TJS_MEMBERENSURE, *propName, NULL, &param, NULL))) {
		} else {
			info.GetReturnValue().Set(ERROR_KRKR(info.GetIsolate(), error));
		}
		return;
	}
	info.GetReturnValue().Set(ERROR_BADINSTANCE(info.GetIsolate()));
}

// ���\�b�h�̌Ăяo��
void
TJSObject::caller(const FunctionCallbackInfo<Value>& args)
{
	HandleScope handle_scope(args.GetIsolate());
	tTJSVariant self;
	if (getVariant(self, args.This())) {
		Handle<Value> ret;
		
		// �����ϊ�
		tjs_int argc = args.Length();
		tTJSVariant **argv = new tTJSVariant*[argc];
		for (tjs_int i=0;i<argc;i++) {
			argv[i] = new tTJSVariant();
			*argv[i] = toVariant(args.GetIsolate(), args[i]);
		}

		if (self.AsObjectClosureNoAddRef().IsInstanceOf(0, NULL, NULL, L"Class", NULL) == TJS_S_TRUE) {
			// �N���X�I�u�W�F�N�g�Ȃ�R���X�g���N�^�Ăяo��
			iTJSDispatch2 *instance = NULL;
			tjs_error error;
			if (TJS_SUCCEEDED(error = self.AsObjectClosureNoAddRef().CreateNew(0, NULL, NULL, &instance, argc, argv, NULL))) {
				ret = toJSValue(args.GetIsolate(), tTJSVariant(instance, instance));
				instance->Release();
			} else {
				ret = ERROR_KRKR(args.GetIsolate(), error);
			}
		} else {
			// ���\�b�h�Ăяo��
			tTJSVariant result;
			tjs_error error;
			if (TJS_SUCCEEDED(error = self.AsObjectClosureNoAddRef().FuncCall(0, NULL, NULL, &result, argc, argv, NULL))) {
				ret = toJSValue(args.GetIsolate(), result);
			} else {
				ret = ERROR_KRKR(args.GetIsolate(), error);
			}
		}

		// �������
		if (argv) {
			for (int i=0;i<argc;i++) {
				delete argv[i];
			}
			delete[] argv;
		}
		
		args.GetReturnValue().Set(ret);
		return;
	}
	args.GetReturnValue().Set(ERROR_BADINSTANCE(args.GetIsolate()));
}

// tTJSVariant ���I�u�W�F�N�g��
Local<Object>
TJSObject::toJSObject(Isolate *isolate, const tTJSVariant &variant)
{
	HandleScope handle_scope(isolate);
	Handle<ObjectTemplate> templ = Local<ObjectTemplate>::New(isolate, objectTemplate);
	Local<Object> obj = templ->NewInstance();
	new TJSObject(isolate, obj, variant);
	return obj;
}
