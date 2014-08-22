
/*
 * This is a dummy JavaScript engine. It cheats by recognising the specific
 * strings in calc.pdf, and hence will work only for that file. It is for
 * testing only.
 */

extern "C" {
#include "mupdf/fitz.h"
#include "mupdf/pdf.h"
#include "pdf-jsimp-cpp.h"
#include "JavaScriptCore.h"
#include"string.h"
}

#include <vector>
#include <set>

using namespace std;

extern int who;

struct PDFJSImp;
struct PDFJSImpType;
struct PDFJSImpGCObj;
struct PDFJSImpMethod;
struct PDFJSImpProperty;

//int find(char *);
//全局标识符
enum
{
    // global property
    APP,            //0
    GLOBAL ,        //1
    EVENT,          //2
    
    // global Function
    GETOCGS =4,     //4
    GETFIELD=5,     //5
    SUBMITFORM,     //6
    PRINT,          //7
    RESETFORM,      //8
    MAILDOC,        //9
    
    //app's function
    ALERT=10,       //10
    LAUNCHURL,       //11
    
    //filed's property
    VALUE=15 ,       //15
    DISPlAY ,        //16
    //OCGs's property
    NAME = 20,      //20
    STATE,
    
    
    PPDFONUNLOCK = 30 //30
};

#define CHANGED 1
#define UNCHANGED 0
//the global struct to keep the context of every method or property
/*
 *全局保存环境结构体
 */
typedef struct
{
    PDFJSImpMethod *meth;
    PDFJSImpType *type;
    PDFJSImpProperty *property;
}Global_ctx;
Global_ctx g_ctx[100];

/* Object we pass to FunctionTemplate::New, which v8 passes back to us in
 * callMethod, allowing us to call our client's, passed-in method. */
struct PDFJSImpMethod
{
	PDFJSImp *imp;
	pdf_jsimp_method *meth;
    
	PDFJSImpMethod(PDFJSImp *imp, pdf_jsimp_method *meth) : imp(imp), meth(meth) {}
};

/* Object we pass to ObjectTemplate::SetAccessor, which v8 passes back to us in
 * setProp and getProp, allowing us to call our client's, passed-in set/get methods. */
struct PDFJSImpProperty
{
	PDFJSImp *imp;
	pdf_jsimp_getter *get;
	pdf_jsimp_setter *set;
    
	PDFJSImpProperty(PDFJSImp *imp, pdf_jsimp_getter *get, pdf_jsimp_setter *set) : imp(imp), get(get), set(set) {}
};
/* Internal representation of the pdf_jsimp object */
struct PDFJSImp
{
public:
	fz_context			*ctx;
	void				*jsctx;
	JSGlobalContextRef	 context;
	vector<PDFJSImpType *> types;
	set<PDFJSImpGCObj *> gclist;
    
	PDFJSImp(fz_context *ctx, void *jsctx) : ctx(ctx), jsctx(jsctx)
	{
		context = JSGlobalContextCreate(NULL);
	}
    
	~PDFJSImp()
	{
        
		/* Tell v8 our context will not be used again */
        JSGlobalContextRelease(context);
        
		/* Unlink and destroy all the objects that v8 has yet to gc */
        //		set<PDFJSImpGCObj *>::iterator oit;
        //		for (oit = gclist.begin(); oit != gclist.end(); oit++)
        //		{
        //			delete *oit;
        //		}
        //
		vector<PDFJSImpType *>::iterator it;
		for (it = types.begin(); it < types.end(); it++)
			delete *it;
	}
};

//这个结构体里面的东西没有用到
/* Internal representation of the pdf_jsimp_type object */
struct PDFJSImpType
{
	PDFJSImp                  *imp;
    JSClassRef                jclass;
    JSObjectRef               jobj;
    JSClassDefinition         jdef;
	pdf_jsimp_dtr             *dtr;
	vector<PDFJSImpMethod *> methods;
	vector<PDFJSImpProperty *> properties;
    
	PDFJSImpType(PDFJSImp *imp, pdf_jsimp_dtr *dtr): imp(imp), dtr(dtr)
	{
        //jsclassdefinition 是否可以分离出来
       // jdef = {0};
        memset(&jdef, 0, sizeof(JSClassDefinition));
		jclass = JSClassCreate(&jdef);
        
        //这里少一句创建javascript对象的语句
        jobj = JSObjectMake(imp->context, jclass, NULL);
    }
    
	~PDFJSImpType()
	{
//		vector<PDFJSImpMethod *>::iterator mit;
//		for (mit = methods.begin(); mit < methods.end(); mit++)
//			delete *mit;
//        
//		vector<PDFJSImpProperty *>::iterator pit;
//		for (pit = properties.begin(); pit < properties.end(); pit++)
//			delete *pit;
//        
        //JSClassRelease(jclass);
	}
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
class PDFJSImpObject
{
	JSValueRef   pobj;
    JSContextRef ctx;
	char  *utf8;
    
public:
	PDFJSImpObject(JSValueRef obj, JSContextRef ctx): utf8(NULL), ctx(ctx)
	{
		pobj = obj;
        utf8 = (char *)malloc(300*sizeof(char));
	}
    
	PDFJSImpObject(const char *str, JSContextRef ctx): utf8(NULL), ctx(ctx)
	{
		pobj = JSValueMakeString(ctx,  JSStringCreateWithUTF8CString(str));
        utf8 = (char *)malloc(300*sizeof(char));
	}
    
	PDFJSImpObject(double num, JSContextRef ctx): utf8(NULL), ctx(ctx)
	{
		pobj = JSValueMakeNumber(ctx, num);
        utf8 = (char *)malloc(300*sizeof(char));
	}
    
	~PDFJSImpObject()
	{
		delete utf8;
		//pobj.Dispose();
	}
    
	int type()
	{
        if (JSValueIsNull(ctx, pobj))
            return JS_TYPE_NULL;
        else if (JSValueIsString(ctx, pobj))
            return JS_TYPE_STRING;
        else if (JSValueIsNumber(ctx, pobj))
            return JS_TYPE_NUMBER;
        else if (JSValueIsBoolean(ctx, pobj))
            return JS_TYPE_BOOLEAN;
        else if(JSValueIsObject(ctx, pobj))
            return JS_TYPE_OBJECT;
        else
            return JS_TYPE_UNKNOWN;
        
        
	}
    
	char *toString()
	{
        JSStringRef string = JSValueToStringCopy(ctx, pobj, 0);
        JSStringGetUTF8CString(string, utf8, 300);
        return utf8 ;
	}
    
	double toNumber()
	{
        return JSValueToNumber(ctx, pobj, 0);
	}
    bool toBoolean()
    {
        return JSValueToBoolean(ctx, pobj);
    }
    
	JSValueRef toValue()
	{
        
		return pobj;
	}
    JSValueRef toArray()
    {
        return JSValueToObject(ctx, pobj, 0);
    }
};

//the memeory release
 void release()
{
    
    for(int i = 0;i<100;i++)
    {
        if(NULL != g_ctx[i].property)
            delete g_ctx[i].property;
           
        if(NULL != g_ctx[i].meth)
            delete g_ctx[i].meth;
        if(NULL != g_ctx[i].type)
            delete g_ctx[i].type;
        g_ctx[i].property = NULL;
        g_ctx[i].meth = NULL;
        g_ctx[i].type = NULL;
    }
    who = UNCHANGED;
    
}

extern "C" int find(char *name)
{
    if(!strcmp(name,"getField"))
        return GETFIELD;
    if(!strcmp(name, "submitForm"))
        return SUBMITFORM;
    if(!strcmp(name, "app"))
        return APP;
    if(!strcmp(name, "global"))
        return GLOBAL;
    if(!strcmp(name, "alert"))
        return ALERT;
    if(!strcmp(name, "print"))
        return PRINT;
    if(!strcmp(name, "value"))
        return VALUE;
    if(!strcmp(name,"event"))
        return EVENT;
    if(!strcmp(name, "resetForm"))
        return RESETFORM;
    if(!strcmp(name, "launchURL"))
        return LAUNCHURL;
    if(!strcmp(name, "display"))
        return DISPlAY;
    if(!strcmp(name,"getOCGs"))
        return GETOCGS;
    if(!strcmp(name, "name"))
        return NAME;
    if(!strcmp(name, "state"))
        return STATE;
    if(!strcmp(name, "ppdfOnUnlock"))
        return PPDFONUNLOCK;
    return -1;
}

////keep
extern "C" fz_context *pdf_jsimp_ctx_cpp(pdf_jsimp *imp)
{
	return reinterpret_cast<PDFJSImp *>(imp)->ctx;
}
////keep
extern "C" const char *pdf_new_jsimp_cpp(fz_context *ctx, void *jsctx, pdf_jsimp **imp)
{
	//Locker lock;
	*imp = reinterpret_cast<pdf_jsimp *>(new PDFJSImp(ctx, jsctx));
    
	return NULL;
}
////keep
//
extern "C" const char *pdf_drop_jsimp_cpp(pdf_jsimp *imp)
{
	//Locker lock;
	delete reinterpret_cast<PDFJSImp *>(imp);
	return NULL;
}
//keep
extern "C" const char *pdf_jsimp_new_type_cpp(pdf_jsimp *imp, pdf_jsimp_dtr *dtr, pdf_jsimp_type **type)
{
	//Locker lock;
    
	PDFJSImp *vImp = reinterpret_cast<PDFJSImp *>(imp);
	PDFJSImpType *vType = new PDFJSImpType(vImp, dtr);
	vImp->types.push_back(vType);
	*type = reinterpret_cast<pdf_jsimp_type *>(vType);
	return NULL;
}
////keep
extern "C" const char *pdf_jsimp_drop_type_cpp(pdf_jsimp *imp, pdf_jsimp_type *type)
{
	/* Types are recorded and destroyed as part of PDFJSImp */
	return NULL;
}

/*
 *the calllback functions handle
 */
static JSValueRef callMethod(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
//    char res[100];
//    if(arguments[0]!=NULL)
//    {
//        JSStringRef str =  JSValueToStringCopy(ctx, arguments[0], 0);
//        JSStringGetUTF8CString(str, res, 100);
//        printf("getfield argument is %s\n",res);
//    }
    
    
    const char *declare = "function ";
    const char *end = "(";
    char function_name[20];
    PDFJSImpObject *fobj = new PDFJSImpObject(function, ctx);
    char * functionn = fobj->toString();
    /*第二个参数：出现funtion的初始位置 + “function ”的长度，即函数名出现的开始位置
     *第三个参数：首次出现“（”前面的字符数量 - “function ”的长度，即函数名的长度
     */
    strncpy(function_name, strstr(functionn, declare) + strlen(declare), strcspn(functionn, end) - strlen(declare));
    function_name[strcspn(functionn, end) - strlen(declare)] = '\0';

    PDFJSImpMethod *m = g_ctx[find(function_name)].meth;
    
    PDFJSImpObject **native_args = new PDFJSImpObject*[argumentCount];
    for (int i = 0; i < argumentCount; i++)
        native_args[i] = new PDFJSImpObject(arguments[i],m->imp->context);
    
    PDFJSImpObject *obj = reinterpret_cast<PDFJSImpObject *>(pdf_jsimp_call_method(reinterpret_cast<pdf_jsimp *>(m->imp), m->meth, m->imp->jsctx, NULL, argumentCount, reinterpret_cast<pdf_jsimp_obj **>(native_args)));
    if(NULL!=obj)
        return obj->toArray();// the value is a array
    return JSValueMakeNull(ctx);
}
/*
 *the function global parameter
 *保存回调函数，用于加入方法
 */

//JSObjectCallAsFunctionCallback callAsfunction[30]={0,0,0,0,getOCGs,get_field,submitForm,print,0,0,app_alert,};

//get the id of the property or method present by the name
//adapt   pdf_jsimp_method  is the function pointer
/*
 *加函数到js中去，加全局函数没问题，但是加某一个属性的方法会有问题
 *这里会出现把某属性的函数变成全局的冗余，对于这样的情况我们主要是保存全局的环境，
 */
extern "C" const char *pdf_jsimp_addmethod_cpp(pdf_jsimp *imp, pdf_jsimp_type *type, char *name, pdf_jsimp_method *meth)
{
    //if the open file has changed ,to clear the globabl enviorment
	if(UNCHANGED != who)
    {
        release();
    }
    PDFJSImp *vImp = reinterpret_cast<PDFJSImp *>(imp);
	PDFJSImpType *vType = reinterpret_cast<PDFJSImpType *>(type);
	PDFJSImpMethod *pmeth = new PDFJSImpMethod(vType->imp, meth);
    
    // JSObjectRef gobj = vType->jobj;
    JSObjectRef mobj = JSContextGetGlobalObject(vType->imp->context);
    
    int index =find(name);
    if(-1 != index && NULL == g_ctx[index].meth)
    {
        //keep the global parameter ,the function of some property just keep the enviorment
        g_ctx[index].meth = pmeth;
        g_ctx[index].type = vType;
       
        //only the global function to add
        if((index>=4 && index<=9)||index >=30)
        {
            JSClassRef mclass ;
            JSClassDefinition mClassDefinition = kJSClassDefinitionEmpty;
            mclass = JSClassCreate( &mClassDefinition );
            JSObjectRef b= JSObjectMakeFunctionWithCallback(vType->imp->context,JSStringCreateWithUTF8CString(name),callMethod);
            JSObjectSetProperty(vType->imp->context, mobj, JSStringCreateWithUTF8CString(name), b, kJSPropertyAttributeNone, 0);
            //no use
        //vType->methods.push_back(pmeth);
        }
    }
    
	return NULL;
}
////adapt

static JSValueRef app_setTimeOut(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    return JSValueMakeNull(ctx);
}
static bool app_setViewerVersion (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception)
{
    return JSValueMakeNumber(ctx, 9);
}

static JSValueRef app_getViewerVersion(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception)
{
    return JSValueMakeNumber(ctx, 9) ;
}

static bool app_setViewerType (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception)
{
    
    return NULL;
}

static JSValueRef app_getViewerType(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception)
{
    return JSValueMakeString(ctx,  JSStringCreateWithUTF8CString("Reader"));
}
static JSValueRef app_getPlatform(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception)
{
    return JSValueMakeString(ctx,  JSStringCreateWithUTF8CString("MAC"));
}
static bool app_setPlatform (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception)
{
    
    return true;
}
static JSValueRef app_getPlugins(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception)
{
    return JSValueMakeNull(ctx);
}
static bool app_setPlugins (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception)
{
    
    return true;
}

static bool global_setboom_guid (JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception)
{
    return true;
}
static JSValueRef global_getboom_guid(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception)
{

    return JSValueMakeString(ctx, JSStringCreateWithUTF8CString("$$0_g_0000000"));
}
static JSValueRef global_getA(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception)
{
    return JSValueMakeString(ctx, JSStringCreateWithUTF8CString("$$0_g_0000000"));
}

static JSStaticValue globalvalues[]=
{
    {"boom_guid",global_getboom_guid,global_setboom_guid,kJSPropertyAttributeNone},
    {"$$0_g_403356", global_getA, app_setViewerVersion, kJSPropertyAttributeNone},
    {"$$0_g_1050616", global_getA, app_setViewerVersion, kJSPropertyAttributeNone},
    {"$$0_g_0000", global_getA, app_setViewerVersion, kJSPropertyAttributeNone},
    {0,0,0,0}
};

static JSStaticValue appValues[] =
{
   // {"plugIns",app_getPlugins,app_setPlugins,kJSPropertyAttributeNone},
    {"platform",app_getPlatform,app_setPlatform,kJSPropertyAttributeNone},
    {"viewerVersion", app_getViewerVersion, app_setViewerVersion, kJSPropertyAttributeNone},
    {"viewerType",app_getViewerType,app_setViewerType,kJSPropertyAttributeNone},
    {0,0,0,0}
};
static JSStaticFunction appFunctions[] =
{
    {"alert", callMethod, kJSPropertyAttributeNone},
    {"setTimeOut", app_setTimeOut, kJSPropertyAttributeNone},
    {0,0,0}
};

/*
 *加入属性到js 中去，还有问题。用于测试
 * all property has the same static function and static value
 * adapt done
 */
extern "C" const char *pdf_jsimp_addproperty_cpp(pdf_jsimp *imp, pdf_jsimp_type *type, char *name, pdf_jsimp_getter *get, pdf_jsimp_setter *set)
{
    if(0 != who)
    {
        release();
    }
    PDFJSImp *vImp = reinterpret_cast<PDFJSImp *>(imp);
	PDFJSImpType *vType = reinterpret_cast<PDFJSImpType *>(type);
	PDFJSImpProperty *prop = new PDFJSImpProperty(vType->imp, get, set);
    
    // JSObjectRef type_obj = vType->jobj;
    JSObjectRef type_obj =  JSContextGetGlobalObject(vType->imp->context);
    int index =find(name);
    if(-1!=index && NULL == g_ctx[index].property)
    {
        
        g_ctx[index].property = prop;
        g_ctx[index].type = vType;
        
        if(index<=2)
        {
            JSClassRef   pclass ;
            JSClassDefinition definition = kJSClassDefinitionEmpty;
            switch(index)
            {
                case APP:
                    definition.staticFunctions = appFunctions;
                    definition.staticValues = appValues;
                    break;
                case GLOBAL:
                    definition.staticValues = globalvalues ;
                    break;
                case EVENT :
                    
                    break;
                default:
                    return NULL;//not the global property just keep the enviorment
            }
            
            pclass = JSClassCreate(&definition);
            JSValueRef value = JSObjectMake(vType->imp->context, pclass, NULL);
            JSObjectSetProperty(vType->imp->context, type_obj, JSStringCreateWithUTF8CString(name),value, kJSPropertyAttributeNone, 0);
            //vType->properties.push_back(prop);
        }
    }
    return NULL;
}


/*
 *加field的.value 属性，在js 中会用到du.value回调对应
 */
static JSValueRef get_value(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception)
{
    //FIXMIE:we still don't know which type to get
    char name[20];
    JSStringGetUTF8CString(propertyName, name, 20);
    int index = find(name);
    if(-1!=index)
    {
        
        PDFJSImpProperty *p = g_ctx[index].property;
        PDFJSImpObject *obj = reinterpret_cast<PDFJSImpObject *>(pdf_jsimp_call_getter(reinterpret_cast<pdf_jsimp *>(p->imp), p->get, p->imp->jsctx,object));
        return obj->toValue();
    }
    return (JSValueMakeNull(ctx));
}
static bool set_value(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception)
{
    char name[20];
    JSStringGetUTF8CString(propertyName, name, 20);
    int index = find(name);
    PDFJSImpObject *val = new PDFJSImpObject(value,ctx);
    if(-1!=index)
    {
       PDFJSImpProperty *p = g_ctx[index].property;
       pdf_jsimp_call_setter(reinterpret_cast<pdf_jsimp *>(p->imp), p->set, p->imp->jsctx,object,reinterpret_cast<pdf_jsimp_obj *>(val));
        return true;
    }
    return false;
}
static JSStaticValue jsobjValue[] =
{

    {"state",get_value,set_value,kJSClassAttributeNone},
    {"name",get_value,set_value,kJSClassAttributeNone},
    {"display",get_value,set_value,kJSPropertyAttributeNone},
    {"value", get_value, set_value, kJSPropertyAttributeNone},
    {0,0,0,0}
};


//adapt
/*
 * according the type to new a object .e.g. js->doctype,js->eventtype,js->apptype,js->fieldtype
 * there the all object has the same static value,it for test
 *
 */
extern "C" const char *pdf_jsimp_new_obj_cpp(pdf_jsimp *imp, pdf_jsimp_type *type, void *natobj, pdf_jsimp_obj **robj)
{
	//Locker lock;
	PDFJSImpType *vType = reinterpret_cast<PDFJSImpType *>(type);
    PDFJSImp *vimp = reinterpret_cast<PDFJSImp *>(imp);
    
    JSClassRef jsclass;
    JSClassDefinition jsobjdefinition = kJSClassDefinitionEmpty;
    
    jsobjdefinition.staticValues = jsobjValue;
    jsclass = JSClassCreate(&jsobjdefinition);
    JSValueRef obj = JSObjectMake(vType->imp->context, jsclass, natobj);// the data natobj is set to the jsobject's private data
  	*robj = reinterpret_cast<pdf_jsimp_obj *>(new PDFJSImpObject(obj,vType->imp->context));
  
	return NULL;
}


//just for the ocgs function
extern "C" const char *pdf_jsimp_new_array_obj_cpp(pdf_jsimp *imp, pdf_jsimp_type *type,  pdf_jsimp_obj *natobj[2], pdf_jsimp_obj **robj)
{
    PDFJSImpType *vType = reinterpret_cast<PDFJSImpType *>(type);
    PDFJSImp *vimp = reinterpret_cast<PDFJSImp *>(imp);
    JSValueRef arguments[4] ;
    int i=0,j=0;
    JSClassRef jsclass;
    JSClassDefinition jsobjdefinition = kJSClassDefinitionEmpty;
    jsclass = JSClassCreate(&jsobjdefinition);
    
    
    while (j<2) {
        arguments[i++]= (reinterpret_cast<PDFJSImpObject *>(natobj[j]))->toValue();
        j++;
        
    }
    JSValueRef array =  JSObjectMakeArray(vType->imp->context, i, arguments, 0);
    
    *robj = reinterpret_cast<pdf_jsimp_obj *>(new PDFJSImpObject(array,vType->imp->context));
    return NULL;
}

//adapt set the js->document as the global type
extern "C" const char *pdf_jsimp_set_global_type_cpp(pdf_jsimp *imp, pdf_jsimp_type *type)
{
	PDFJSImp	 *vImp  = reinterpret_cast<PDFJSImp *>(imp);
	PDFJSImpType *vType = reinterpret_cast<PDFJSImpType *>(type);
    vImp->context = JSGlobalContextCreate(vType->jclass);
    
	return NULL;
}

//adapt done
extern "C" const char *pdf_jsimp_drop_obj_cpp(pdf_jsimp *imp, pdf_jsimp_obj *obj)
{
	delete reinterpret_cast<PDFJSImpObject *>(obj);
	return NULL;
}
//adapt done
extern "C" const char *pdf_jsimp_to_type_cpp(pdf_jsimp *imp, pdf_jsimp_obj *obj, int *type)
{
    
	*type = reinterpret_cast<PDFJSImpObject *>(obj)->type();
	return NULL;
}
////adapt done
extern "C" const char *pdf_jsimp_from_string_cpp(pdf_jsimp *imp, char *str, pdf_jsimp_obj **obj)
{
    PDFJSImp *Imp = reinterpret_cast<PDFJSImp *>(imp);
    *obj = reinterpret_cast<pdf_jsimp_obj *>(new PDFJSImpObject(str,Imp->context));
	return NULL;
	
}
extern "C" const char *pdf_jsimp_from_array_cpp(pdf_jsimp *imp, void **ar, pdf_jsimp_obj **obj)
{
    
	
}
//adapt done
extern "C" const char *pdf_jsimp_to_string_cpp(pdf_jsimp *imp, pdf_jsimp_obj *obj, char **str)
{
	*str = reinterpret_cast<PDFJSImpObject *>(obj)->toString();
	return NULL;
}
//adapt done
extern "C" const char *pdf_jsimp_from_number_cpp(pdf_jsimp *imp, double num, pdf_jsimp_obj **obj)
{
    PDFJSImp *vimp = reinterpret_cast<PDFJSImp *>(imp);
	*obj = reinterpret_cast<pdf_jsimp_obj *>(new PDFJSImpObject(num,vimp->context));
	return NULL;
}
//adapt done
extern "C" const char *pdf_jsimp_to_number_cpp(pdf_jsimp *imp, pdf_jsimp_obj *obj, double *num)
{
	*num = reinterpret_cast<PDFJSImpObject *>(obj)->toNumber();
	return NULL;
}
extern "C" const char *pdf_jsimp_to_boolean_cpp(pdf_jsimp *imp, pdf_jsimp_obj *obj, int *boo)
{
	*boo = reinterpret_cast<PDFJSImpObject *>(obj)->toBoolean();
	return NULL;
}
////adapt
extern "C" const char *pdf_jsimp_array_len_cpp(pdf_jsimp *imp, pdf_jsimp_obj *obj, int *len)
{
	return NULL;
}
////adapt
extern "C" const char *pdf_jsimp_array_item_cpp(pdf_jsimp *imp, pdf_jsimp_obj *obj, int i, pdf_jsimp_obj **item)
{
    return NULL;
}
//adapt
extern "C" const char *pdf_jsimp_property_cpp(pdf_jsimp *imp, pdf_jsimp_obj *obj, char *prop, pdf_jsimp_obj **pobj)
{
    
    PDFJSImp *vImp = reinterpret_cast<PDFJSImp *>(imp);
    PDFJSImpObject *vObj = reinterpret_cast<PDFJSImpObject *>(obj);
    
    JSObjectRef jsobj = JSValueToObject(vImp->context, vObj->toValue(), 0);
    JSStringRef vProp = JSStringCreateWithUTF8CString(prop);
    *pobj = reinterpret_cast<pdf_jsimp_obj *>(new PDFJSImpObject(JSObjectGetProperty(vImp->context, jsobj, vProp, NULL),vImp->context));
    
	return NULL;
}

//adapt done
extern "C" const char *pdf_jsimp_execute_cpp(pdf_jsimp *imp, char *code)
{
	PDFJSImp *vImp = reinterpret_cast<PDFJSImp *>(imp);
    
    
    JSGlobalContextRef global_context = vImp->context;
    JSStringRef scriptJS = JSStringCreateWithUTF8CString(code);
    JSValueRef exception = NULL;
    
    //execte the js function
    JSValueRef result = JSEvaluateScript(global_context, scriptJS, NULL, NULL, 0, &exception);
    
    char res[300];
    
    if (!result) {
        if (exception) {
            JSStringRef exceptionArg = JSValueToStringCopy(global_context, exception, NULL);
            char exceptionRes[100];
            JSStringGetUTF8CString(exceptionArg,exceptionRes,300);
            JSStringRelease(exceptionArg);
            printf("[JSC] JavaScript exception: %s\n", exceptionRes);
            return NULL;
        }
        printf("[JSC] No result returned\n");
    } else {
        JSStringRef jstrArg = JSValueToStringCopy(global_context, result, NULL);
        JSStringGetUTF8CString(jstrArg, res, 300);
        JSStringRelease(jstrArg);
    }
    JSStringRelease(scriptJS);
    
    return NULL;
    
}
//adapt
extern "C" const char *pdf_jsimp_execute_count_cpp(pdf_jsimp *imp, char *code, int count)
{
	
	PDFJSImp *vImp = reinterpret_cast<PDFJSImp *>(imp);
    
    JSGlobalContextRef global_context = vImp->context;
    JSStringRef scriptJS = JSStringCreateWithUTF8CString(code);
    JSValueRef exception = NULL;
    
    //execte the js function
    JSValueRef result = JSEvaluateScript(global_context, scriptJS, NULL, NULL, 0, &exception);
    char res [100];
    if (!result) {
        if (exception) {
            JSStringRef exceptionArg = JSValueToStringCopy(global_context, exception, NULL);
            char exceptionRes[100];
            JSStringGetUTF8CString(exceptionArg,exceptionRes,100);
            JSStringRelease(exceptionArg);
            printf("[JSC] JavaScript exception: %s\n", exceptionRes);
        }
        printf("[JSC] No result returned\n");
    } else {
        JSStringRef jstrArg = JSValueToStringCopy(global_context, result, NULL);
        JSStringGetUTF8CString(jstrArg,res,100);
        JSStringRelease(jstrArg);
    }
    JSStringRelease(scriptJS);
    
    return NULL;
}
