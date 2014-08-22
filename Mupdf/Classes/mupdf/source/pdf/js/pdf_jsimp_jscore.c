/*
 * This is a dummy JavaScript engine. It cheats by recognising the specific
 * strings in calc.pdf, and hence will work only for that file. It is for
 * testing only.
 */


#include "mupdf/fitz.h"
#include "mupdf/pdf.h"
#include "pdf-jsimp-cpp.h"
#include "JavaScriptCore.h"

struct PDFJSImp;
struct PDFJSImpType;
struct PDFJSImpGCObj;

/* Object we pass to FunctionTemplate::New, which v8 passes back to us in
 * callMethod, allowing us to call our client's, passed-in method. */
struct PDFJSImpMethod
{
	struct PDFJSImp *imp;
	pdf_jsimp_method *meth;
    
	//PDFJSImpMethod(PDFJSImp *imp, pdf_jsimp_method *meth) : imp(imp), meth(meth) {}
};

/* Object we pass to ObjectTemplate::SetAccessor, which v8 passes back to us in
 * setProp and getProp, allowing us to call our client's, passed-in set/get methods. */
struct PDFJSImpProperty
{
	struct PDFJSImp *imp;
	pdf_jsimp_getter *get;
	pdf_jsimp_setter *set;
    
//	PDFJSImpProperty(PDFJSImp *imp, pdf_jsimp_getter *get, pdf_jsimp_setter *set) : imp(imp), get(get), set(set) {}
};
/* Internal representation of the pdf_jsimp object */
struct PDFJSImp
{

	fz_context			*ctx;
	void				*jsctx;
	JSGlobalContextRef	 context;
	//vector<PDFJSImpType *> types;
	//set<PDFJSImpGCObj *> gclist;
    
//	PDFJSImp(fz_context *ctx, void *jsctx) : ctx(ctx), jsctx(jsctx)
//	{
//		context = JSGlobalContextCreate(NULL);
//	}
    
//	~PDFJSImp()
//	{
//        
//		/* Tell v8 our context will not be used again */
//        JSGlobalContextRelease(context);
//        
//		/* Unlink and destroy all the objects that v8 has yet to gc */
//        //		set<PDFJSImpGCObj *>::iterator oit;
//        //		for (oit = gclist.begin(); oit != gclist.end(); oit++)
//        //		{
//        //			delete *oit;
//        //		}
//        //
//		vector<PDFJSImpType *>::iterator it;
//		for (it = types.begin(); it < types.end(); it++)
//			delete *it;
//	}
};

//这个结构体里面的东西没有用到
/* Internal representation of the pdf_jsimp_type object */
struct PDFJSImpType
{
	struct PDFJSImp           *imp;
    JSClassRef                jclass;
    JSObjectRef               jobj;
    JSClassDefinition         jdef;
	pdf_jsimp_dtr             *dtr;
//	vector<PDFJSImpMethod *> methods;
//	vector<PDFJSImpProperty *> properties;
//    
//	PDFJSImpType(PDFJSImp *imp, pdf_jsimp_dtr *dtr): imp(imp), dtr(dtr)
//	{
//        //jsclassdefinition 是否可以分离出来
//        jdef = {0};
//		jclass = JSClassCreate(&jdef);
//        
//        //这里少一句创建javascript对象的语句
//        //jobj = JSObjectMake(imp->context, jclass, NULL);
//    }
//    
//	~PDFJSImpType()
//	{
//		vector<PDFJSImpMethod *>::iterator mit;
//		for (mit = methods.begin(); mit < methods.end(); mit++)
//			delete *mit;
//        
//		vector<PDFJSImpProperty *>::iterator pit;
//		for (pit = properties.begin(); pit < properties.end(); pit++)
//			delete *pit;
//        
//        JSClassRelease(jclass);
//	}
};
///* Info via which we destroy the client side part of objects that
// * v8 garbage collects */
//struct PDFJSImpGCObj
//{
//    //	Persistent<Object> pobj;
//    //	PDFJSImpType *type;
//    //
//    //	PDFJSImpGCObj(Handle<Object> obj, PDFJSImpType *type): type(type)
//    //	{
//    //		pobj = Persistent<Object>::New(obj);
//    //	}
//    //
//    //	~PDFJSImpGCObj()
//    //	{
//    //		pobj.Dispose();
//    //	}
//};
//
//
/* Internal representation of the pdf_jsimp_obj object */
struct PDFJSImpObject
{
	JSValueRef   pobj;
	char  *utf8;
    

//	PDFJSImpObject(JSValueRef obj): utf8(NULL)
//	{
//		pobj = obj;
//	}
//    
//	PDFJSImpObject(const char *str): utf8(NULL)
//	{
//		//pobj = JSValueMakeString(<#JSContextRef ctx#>,  JSStringCreateWithUTF8CString(str));
//	}
//    
//	PDFJSImpObject(double num): utf8(NULL)
//	{
//		//pobj = JSValueMakeNumber(<#JSContextRef ctx#>, num);
//	}
//    
//	~PDFJSImpObject()
//	{
//		delete utf8;
//		//pobj.Dispose();
//	}
//    
//	int type()
//	{
//        //		if (jsvaluei)
//        //			return JS_TYPE_NULL;
//        //		else if (pobj->IsString() || pobj->IsStringObject())
//        //			return JS_TYPE_STRING;
//        //		else if (pobj->IsNumber() || pobj->IsNumberObject())
//        //			return JS_TYPE_NUMBER;
//        //		else if (pobj->IsArray())
//        //			return JS_TYPE_ARRAY;
//        //		else if (pobj->IsBoolean() || pobj->IsBooleanObject())
//        //			return JS_TYPE_BOOLEAN;
//        //		else
//        //			return JS_TYPE_UNKNOWN;
//	}
//    
//	char *toString()
//	{
//		delete utf8;
//		//utf8 = new String::Utf8Value(pobj);
//		return utf8 ;//**utf8;
//	}
//    
//	double toNumber()
//	{
//        //	return ;//JSValueToNumber(<#JSContextRef ctx#>, <#JSValueRef value#>, <#JSValueRef *exception#>);
//	}
//    
//	JSValueRef toValue()
//	{
//		return pobj;
//	}
};
////keep
fz_context *pdf_jsimp_ctx_cpp(pdf_jsimp *imp)
{
	return ((struct PDFJSImp *)imp)->ctx;
}
////keep
const char *pdf_new_jsimp_cpp(fz_context *ctx, void *jsctx, pdf_jsimp **imp)
{
	//Locker lock;
    struct PDFJSImp *vimp =(struct PDFJSImp *)malloc(sizeof(struct PDFJSImp));
    vimp->ctx = ctx;
    vimp->jsctx = jsctx;
    vimp->context = JSGlobalContextCreate(NULL);
	*imp = (pdf_jsimp *)vimp;
    
	return NULL;
}
////keep
//
 const char *pdf_drop_jsimp_cpp(pdf_jsimp *imp)
{
//	//Locker lock;
//	delete reinterpret_cast<PDFJSImp *>(imp);
//	return NULL;
}
//keep
const char *pdf_jsimp_new_type_cpp(pdf_jsimp *imp, pdf_jsimp_dtr *dtr, pdf_jsimp_type **type)
{
	//Locker lock;
    
	struct PDFJSImp *vImp = (struct PDFJSImp *)(imp);
	struct PDFJSImpType *vType = (struct PDFJSImpType*)malloc(sizeof(struct PDFJSImpType));
   // new PDFJSImpType(vImp, dtr);
    vType->dtr = dtr;
    vType->imp = vImp;
	//vImp->types.push_back(vType);
	*type = (pdf_jsimp_type *)(vType);
	return NULL;
}
////keep
 const char *pdf_jsimp_drop_type_cpp(pdf_jsimp *imp, pdf_jsimp_type *type)
{
	/* Types are recorded and destroyed as part of PDFJSImp */
	return NULL;
}
//adapt
//static JSValueRef callMethod(const Arguments &args)
//{
//	//HandleScope scope;
//
//	Local<External> mwrap = Local<External>::Cast(args.Data());
//	PDFJSImpMethod *m = (PDFJSImpMethod *)mwrap->Value();
//
//	Local<Object> self = args.Holder();
//	Local<External> owrap;
//	void *nself = NULL;
//	if (self->InternalFieldCount() > 0)
//	{
//		owrap = Local<External>::Cast(self->GetInternalField(0));
//		nself = owrap->Value();
//	}
//
//	int c = args.Length();
//	PDFJSImpObject **native_args = new PDFJSImpObject*[c];
//	for (int i = 0; i < c; i++)
//		native_args[i] = new PDFJSImpObject(args[i]);
//
//	PDFJSImpObject *obj = reinterpret_cast<PDFJSImpObject *>(pdf_jsimp_call_method(reinterpret_cast<pdf_jsimp *>(m->imp), m->meth, m->imp->jsctx, nself, c, reinterpret_cast<pdf_jsimp_obj **>(native_args)));
//	Handle<Value> val;
//	if (obj)
//		val = obj->toValue();
//	delete obj;
//
//	for (int i = 0; i < c; i++)
//		delete native_args[i];
//
//	delete native_args;
//
//	return scope.Close(val);
//}
//adapt
const char *pdf_jsimp_addmethod_cpp(pdf_jsimp *imp, pdf_jsimp_type *type, char *name, pdf_jsimp_method *meth)
{
	//Locker lock;
    struct PDFJSImp *vImp = (struct PDFJSImp *)(imp);
	struct PDFJSImpType *vType = (struct PDFJSImpType *)(type);
    
	struct PDFJSImpMethod *pmeth = malloc(sizeof(struct PDFJSImpMethod));
   // new PDFJSImpMethod(vType->imp, meth);
    pmeth->imp = vType->imp;
    pmeth->meth = meth;
    //	vType->templ->Set(String::New(name), FunctionTemplate::New(callMethod, External::New(pmeth)));
    
    JSObjectRef gobj = JSContextGetGlobalObject(vImp->context);
    
	//vType->methods.push_back(pmeth);
    
	return NULL;
}
////adapt
//static Handle<Value> getProp(Local<String> property, const AccessorInfo &info)
//{
//	//HandleScope scope;
//	Local<External> pwrap = Local<External>::Cast(info.Data());
//	PDFJSImpProperty *p = reinterpret_cast<PDFJSImpProperty *>(pwrap->Value());
//
//	Local<Object> self = info.Holder();
//	Local<External> owrap;
//	void *nself = NULL;
//	if (self->InternalFieldCount() > 0)
//	{
//		Local<Value> val = self->GetInternalField(0);
//		if (val->IsExternal())
//		{
//			owrap = Local<External>::Cast(val);
//			nself = owrap->Value();
//		}
//	}
//
//	PDFJSImpObject *obj = reinterpret_cast<PDFJSImpObject *>(pdf_jsimp_call_getter(reinterpret_cast<pdf_jsimp *>(p->imp), p->get, p->imp->jsctx, nself));
//	Handle<Value> val;
//	if (obj)
//		val = obj->toValue();
//	delete obj;
//	return scope.Close(val);
//}
//adapt
//static void setProp(Local<String> property, Local<Value> value, const AccessorInfo &info)
//{
//	//HandleScope scope;
//	Local<External> wrap = Local<External>::Cast(info.Data());
//	PDFJSImpProperty *p = reinterpret_cast<PDFJSImpProperty *>(wrap->Value());
//
//	Local<Object> self = info.Holder();
//	Local<External> owrap;
//	void *nself = NULL;
//	if (self->InternalFieldCount() > 0)
//	{
//		owrap = Local<External>::Cast(self->GetInternalField(0));
//		nself = owrap->Value();
//	}
//
//	PDFJSImpObject *obj = new PDFJSImpObject(value);
//    //in pdf_jsimp_cpp
//	pdf_jsimp_call_setter(reinterpret_cast<pdf_jsimp *>(p->imp), p->set, p->imp->jsctx, nself, reinterpret_cast<pdf_jsimp_obj *>(obj));
//	delete obj;
//}
////adapt

static JSValueRef app_setTimeOut(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    return NULL;
}

//static JSValueRef app_viewerVersion(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
//{
//    JSValueRef xa = JSValueMakeString(ctx, JSStringCreateWithUTF8CString("9"));
//    return xa;
//}


static JSValueRef app_alert(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    char res[100];
    JSValueRef obj;
    JSStringRef jstr;
    
    //    obj = JSObjectGetPrototype(ctx, function);
    //    jstr = JSValueToStringCopy(ctx, obj, 0);
    //    JSStringGetUTF8CString(jstr, res, 100);
    //    printf("%s\n",res);
    //
    //    obj = JSObjectGetPrototype(ctx, thisObject);
    //    jstr = JSValueToStringCopy(ctx, obj, 0);
    //    JSStringGetUTF8CString(jstr, res, 100);
    //    printf("%s\n",res);
    
    JSStringRef str =  JSValueToStringCopy(ctx, arguments[0], 0);
    JSStringGetUTF8CString(str, res, 100);
    printf("argument is %s\n",res);
    return NULL;
}

static bool app_setViewerVersion (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception)
{
    return JSValueMakeNumber(ctx, 9);
    // return NULL;
}

static JSValueRef app_getViewerVersion(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception)
{
    //    char buffer[100];
    //    JSStringGetUTF8CString(propertyName, buffer, 100);
    //    printf("property name is %s,",buffer);
    return JSValueMakeNumber(ctx, 9) ;
    //    return NULL;
}

static JSStaticFunction jsBaseTvFunctions[] =
{
    //    {"viewerVersion", app_viewerVersion, kJSPropertyAttributeNone},
    {"alert", app_alert, kJSPropertyAttributeNone},
    {"setTimeOut", app_setTimeOut, kJSPropertyAttributeNone},
    {0,0,0}
};

static JSStaticValue jsBaseTvValue[] =
{
    {"viewerVersion", app_getViewerVersion, app_setViewerVersion, kJSPropertyAttributeNone},
    {0,0,0,0}
};

static JSClassDefinition jsBaseTvDefinition =
{
    0,                     //version
    kJSClassAttributeNone, //attributes
    "_app",                //className
    0,                     //parentClass
    jsBaseTvValue,         //staticValues
    jsBaseTvFunctions,     //staticFunctions
    0,                     //Initialize
    0,                     //Finalize
    0,                     //has Property
    0,                     //get Property
    0,                     //set Property
    0,                     //delete Property
    0,                     //getPropertyNames
    0,                     //callAsFunction
    0,                     //hasInstance
    0,                     //callAsConstructor
    0                      //convertToType
};
//extern "C" newClass()
//extern "C" method(char *name,)
//{
//    JSStaticFunction fun;
//    fun.name = name;
//
//    fun.JSObjectSetProperty = kJSClassAttributeNone;
//}
//extern "C" newValue()
const char *pdf_jsimp_addproperty_cpp(pdf_jsimp *imp, pdf_jsimp_type *type, char *name, pdf_jsimp_getter *get, pdf_jsimp_setter *set)
{
	//Locker lock;
    struct  PDFJSImp *vImp = (struct  PDFJSImp *)(imp);
	struct PDFJSImpType *vType = (struct PDFJSImpType *)(type);
	//HandleScope scope;
    
	struct PDFJSImpProperty *prop = malloc(sizeof(struct PDFJSImpProperty));
    prop->imp = vType->imp ;
    prop->get = get;
    prop->set = set;
   // new PDFJSImpProperty(vType->imp, get, set);
    //	vType->templ->SetAccessor(String::New(name), getProp, setProp, External::New(prop));
    
    JSStringRef vName = JSStringCreateWithUTF8CString(name);
    JSObjectRef gobj = JSContextGetGlobalObject(vImp->context);
    JSClassRef jclass = JSClassCreate(&jsBaseTvDefinition);//app class
    JSValueRef x = JSObjectMake(vImp->context, jclass, NULL);
    
    JSObjectSetProperty(vImp->context, gobj, vName, x, kJSPropertyAttributeNone, 0);
    
	//vType->properties.push_back(prop);
    
    JSStringRelease(vName);
    JSClassRelease(jclass);
	return NULL;
}
//adapt
 const char *pdf_jsimp_set_global_type_cpp(pdf_jsimp *imp, pdf_jsimp_type *type)
{
	//Locker lock;
	struct PDFJSImp	 *vImp  = (struct PDFJSImp	 *)(imp);
	struct PDFJSImpType *vType = (struct PDFJSImpType *)(type);
	//HandleScope scope;
    
    //	vImp->context = Persistent<Context>::New(Context::New(NULL, vType->templ));
    vImp->context = JSGlobalContextCreate(vType->jclass);
    
	return NULL;
}
////adapt
//static void gcCallback(Persistent<Value> val, void *parm)
//{
//	PDFJSImpGCObj *gco = reinterpret_cast<PDFJSImpGCObj *>(parm);
//	PDFJSImpType *vType = gco->type;
//	HandleScope scope;
//	Persistent<Object> obj = Persistent<Object>::Cast(val);
//
//	Local<External> owrap = Local<External>::Cast(obj->GetInternalField(0));
//	vType->dtr(vType->imp->jsctx, owrap->Value());
//	vType->imp->gclist.erase(gco);
//	delete gco; /* Disposes of the persistent handle */
//}
////adapt
 const char *pdf_jsimp_new_obj_cpp(pdf_jsimp *imp, pdf_jsimp_type *type, void *natobj, pdf_jsimp_obj **robj)
{
	//Locker lock;
	struct PDFJSImpType *vType = (struct PDFJSImpType *)(type);
    struct PDFJSImp *vimp = (struct PDFJSImp *)(imp);
	//HandleScope scope;
	//Local<Object> obj = vType->templ->NewInstance();
	//obj->SetInternalField(0, External::New(natobj));
    JSValueRef obj = JSObjectMake(vimp->context, vType->jclass, natobj);
    
	/* Arrange for destructor to be called on the client-side object
	 * when the v8 object is garbage collected */
    //	if (vType->dtr)
    //	{
    //		/* Wrap obj in a PDFJSImpGCObj, which takes a persistent handle to
    //		 * obj, and stores its type with it. The persistent handle tells v8
    //		 * it cannot just destroy obj leaving the client-side object hanging */
    //		PDFJSImpGCObj *gco = new PDFJSImpGCObj(pobj, vType);
    //		/* Keep the wrapped object in a list, so that we can take back control
    //		 * of destroying client-side objects when shutting down this context */
    //		vType->imp->gclist.insert(gco);
    //		/* Tell v8 that it can destroy the persistent handle to obj when it has
    //		 * no further need for it, but it must inform us via gcCallback */
    //		gco->pobj.MakeWeak(gco, gcCallback);
    //	}
    
    struct PDFJSImpObject * o = malloc(sizeof(struct PDFJSImpObject));
    o->pobj = obj;
	*robj = (pdf_jsimp_obj *)o;
	return NULL;
}
//adapt done
 const char *pdf_jsimp_drop_obj_cpp(pdf_jsimp *imp, pdf_jsimp_obj *obj)
{
	//Locker lock;
	//delete ((struct PDFJSImpObject *)(obj));
	return NULL;
}
//adapt done
const char *pdf_jsimp_to_type_cpp(pdf_jsimp *imp, pdf_jsimp_obj *obj, int *type)
{
	//Locker lock;
    struct PDFJSImpObject *o = ((struct PDFJSImpObject *)obj);
    // to type hasn't done
	//*type = o->pobj;
	return NULL;
}
////adapt done
 const char *pdf_jsimp_from_string_cpp(pdf_jsimp *imp, char *str, pdf_jsimp_obj **obj)
{
//	//Locker lock;
//	*obj = reinterpret_cast<pdf_jsimp_obj *>(new PDFJSImpObject(str));
//	return NULL;
}
//adapt done
const char *pdf_jsimp_to_string_cpp(pdf_jsimp *imp, pdf_jsimp_obj *obj, char **str)
{
	//Locker lock;
//	*str = r(struct PDFJSImpObject *)(obj)->toString();
	return NULL;
}
//adapt done
const char *pdf_jsimp_from_number_cpp(pdf_jsimp *imp, double num, pdf_jsimp_obj **obj)
{
	//Locker lock;
    struct PDFJSImpObject * o = (struct PDFJSImpObject *)malloc(sizeof(struct PDFJSImpObject));
//hasn't done
	//*obj = (pdf_jsimp_obj *)(new PDFJSImpObject(num));
	return NULL;
}
//adapt done
 const char *pdf_jsimp_to_number_cpp(pdf_jsimp *imp, pdf_jsimp_obj *obj, double *num)
{
	//Locker lock;
	//*num = reinterpret_cast<PDFJSImpObject *>(obj)->toNumber();
	return NULL;
}
////adapt
 const char *pdf_jsimp_array_len_cpp(pdf_jsimp *imp, pdf_jsimp_obj *obj, int *len)
{
//	//Locker lock;
//	Local<Object> jsobj = reinterpret_cast<PDFJSImpObject *>(obj)->toValue()->ToObject();
//	Local<Array> arr = Local<Array>::Cast(jsobj);
//	*len = arr->Length();
//	return NULL;
}
////adapt
 const char *pdf_jsimp_array_item_cpp(pdf_jsimp *imp, pdf_jsimp_obj *obj, int i, pdf_jsimp_obj **item)
{
//	//Locker lock;
//	Local<Object> jsobj = reinterpret_cast<PDFJSImpObject *>(obj)->toValue()->ToObject();
//	*item = reinterpret_cast<pdf_jsimp_obj *>(new PDFJSImpObject(jsobj->Get(Number::New(i))));
//	return NULL;
}
//adapt
const char *pdf_jsimp_property_cpp(pdf_jsimp *imp, pdf_jsimp_obj *obj, char *prop, pdf_jsimp_obj **pobj)
{
	//Locker lock;
    //	JSValueRef jsobj = reinterpret_cast<PDFJSImpObject *>(obj)->toValue()->ToObject();
    //	*pobj = reinterpret_cast<pdf_jsimp_obj *>(new PDFJSImpObject(jsobj->Get(String::New(prop))));
    
    struct PDFJSImp *vImp = (struct PDFJSImp *)(imp);
    struct PDFJSImpObject *vObj = (struct PDFJSImpObject *)(obj);
    
    //hasn't done
//    //JSObjectRef jsobj = JSValueToObject(vImp->context, vObj->toValue(), 0);
//    JSStringRef vProp = JSStringCreateWithUTF8CString(prop);
//    *pobj = reinterpret_cast<pdf_jsimp_obj *>(new PDFJSImpObject(JSObjectGetProperty(vImp->context, jsobj, vProp, NULL)));
    
	return NULL;
}

//adapt done
 const char *pdf_jsimp_execute_cpp(pdf_jsimp *imp, char *code)
{
	struct PDFJSImp *vImp = (struct PDFJSImp *)(imp);
    
    
    //可否用局部的context来代替？
    JSGlobalContextRef global_context = vImp->context;
    JSStringRef scriptJS = JSStringCreateWithUTF8CString(code);
    JSValueRef exception = NULL;
    
    //execte the js function
    JSValueRef result = JSEvaluateScript(global_context, scriptJS, NULL, NULL, 0, &exception);
    
    char res[100];
    
    if (!result) {
        if (exception) {
            JSStringRef exceptionArg = JSValueToStringCopy(global_context, exception, NULL);
            char exceptionRes[100];
            JSStringGetUTF8CString(exceptionArg,exceptionRes,100);
            JSStringRelease(exceptionArg);
            printf("[JSC] JavaScript exception: %s\n", exceptionRes);
            return exceptionRes;
        }
        printf("[JSC] No result returned\n");
    } else {
        JSStringRef jstrArg = JSValueToStringCopy(global_context, result, NULL);
        JSStringGetUTF8CString(jstrArg, res, 100);
        JSStringRelease(jstrArg);
    }
    JSStringRelease(scriptJS);
    
    return NULL;
    
}
//adapt
 const char *pdf_jsimp_execute_count_cpp(pdf_jsimp *imp, char *code, int count)
{
//	//Locker lock;
//	PDFJSImp *vImp = reinterpret_cast<PDFJSImp *>(imp);
//	//HandleScope scope;
//	//Context::Scope context_scope(vImp->context);
//    //Handle<Script> script = Script::Compile(String::New(code, count));
//    //    if (script.IsEmpty())
//    //		return "compile failed in pdf_jsimp_execute_count";
//    //	script->Run();
//
//    JSGlobalContextRef global_context = vImp->context;
//    JSStringRef scriptJS = JSStringCreateWithUTF8CString(str);
//    JSValueRef exception = NULL;
//
//    //execte the js function
//    JSValueRef result = JSEvaluateScript(global_context, scriptJS, NULL, NULL, 0, &exception);
//    char res [100];
//    if (!result) {
//        if (exception) {
//            JSStringRef exceptionArg = JSValueToStringCopy(jsglctx, exception, NULL);
//            char exceptionRes[100];
//            JSStringGetUTF8CString(exceptionArg,exceptionRs,100);
//            JSStringRelease(exceptionArg);
//            printf("[JSC] JavaScript exception: %s\n", exceptionRes);
//        }
//        printf("[JSC] No result returned\n");
//    } else {
//        JSStringRef jstrArg = JSValueToStringCopy(jsglctx, result, NULL);
//        res = JSStringGetUTF8CString(jstrArg,exceptionRs,100);
//        JSStringRelease(jstrArg);
//    }
//    JSStringRelease(scriptJS);
//	return res;
}
