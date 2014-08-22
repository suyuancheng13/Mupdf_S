#include "mupdf/pdf.h"
#include<JavaScriptCore.h>


//typedef struct pdf_js_s pdf_js;
struct pdf_js_s
{
	pdf_document *doc;
	pdf_obj *form;
	pdf_js_event event;
	pdf_jsimp *imp;
	pdf_jsimp_type *doctype;
	pdf_jsimp_type *eventtype;
	pdf_jsimp_type *fieldtype;
	pdf_jsimp_type *apptype;
};

/*
 测试：
 ppdfOnUnlock 回调处理函数
 其中pdf_authenticate_password对正确密码的验证是false
 
 */

static pdf_jsimp_obj *doc_pddfUnlock(void *jsctx, void *obj, int argc, pdf_jsimp_obj *args[])
{

    int num=0,i=0;
    pdf_js *js = (pdf_js *)jsctx;
    pdf_authenticate_password(js->doc, "");

//    pdf_print_crypt(js->doc->crypt);
//    fz_write_options opts ={ 0 };
//
//        for(num = 0;num<pdf_count_pages(js->doc);num++)
//    {
//        pdf_obj *pageobject = js->doc->page_objs[num];
//        pdf_obj *content = pdf_dict_getp(pageobject,"Contents");
//        
//        //try to unlock the stream ,but failed
//        for(i=0;i<pdf_array_len(content);i++)//pdf_array_len(content)
//        {
//            pdf_obj *o = pdf_array_get(content, i);
//           fz_stream * stream = pdf_open_crypt_with_filter(js->doc->file, js->doc->crypt, "StdCF", pdf_to_num(o), pdf_to_gen(o));
//            pdf_document *tem = pdf_open_document_with_stream(js->doc->ctx, stream);
//            pdf_write_document(tem, "tem.pdf",&opts);
//      }
//    }
    return NULL;
}
/*
// the getocgs function which work for the callback
 但是对结果没什么影响
 */
static pdf_jsimp_obj *doc_getOCGs(void *jsctx, void *obj, int argc, pdf_jsimp_obj *args[])
{
	pdf_js *js = (pdf_js *)jsctx;
    pdf_jsimp_obj *obj_array[2];
    int i=0 ;
    pdf_obj  *ocg = pdf_dict_gets(pdf_dict_gets(pdf_trailer(js->doc), "Root"), "OCProperties");
	if (!ocg)
		return NULL;
	pdf_obj *array = pdf_dict_gets(ocg, "OCGs");//get the array of the ocgs
	if (!array || !pdf_is_array(array))
    
		return NULL;
	int len = pdf_array_len(array);
    for(i=0;i<len;i++)
    {
        pdf_obj *o = pdf_array_get(array,i);
       
        pdf_jsimp_obj *jsimpo = pdf_jsimp_new_obj(js->imp, js->doctype, o);
        obj_array[i] = jsimpo;
    }
   
    return pdf_jsimp_new_array_obj(js->imp, js->doctype, obj_array);//is a array
	
}

//submit call back function
static pdf_jsimp_obj *doc_submitForm(void *jsctx, void *obj, int argc, pdf_jsimp_obj *args[])
{
    pdf_js *js = (pdf_js *)jsctx;
    char *url;
    char *data;
    int get;
    int data_len;
    
    if (argc == 1)
    {
        switch (pdf_jsimp_to_type(js->imp, args[0])) {
            case JS_TYPE_STRING:
                url = pdf_jsimp_to_string(js->imp, args[0]);
                get = PDF_SUBMIT_METHOD_GET;
                break;
            case JS_TYPE_OBJECT:
                url = pdf_jsimp_to_string(js->imp, pdf_jsimp_property(js->imp, args[0], "cURL"));
                get = PDF_SUBMIT_METHOD_GET;
                data = pdf_jsimp_to_string(js->imp, pdf_jsimp_property(js->imp, args[0], "aFields"));
                data_len = sizeof(data);
                break;
            default:
                break;
        }
    }
    else if (argc == 4)
    {
        url = pdf_jsimp_to_string(js->imp, args[0]);
        get = pdf_jsimp_to_boolean(js->imp, args[2]);
        data = pdf_jsimp_to_string(js->imp, args[3]);
        data_len = sizeof(data);
    }
    
    pdf_event_issue_submit(js->doc, url, data, get, data_len);
    
    return NULL;
}




static char *utf8_to_pdf(fz_context *ctx, char *utf8)
{
	char *pdf = fz_malloc(ctx, strlen(utf8)+1);
	int i = 0;
	unsigned char c;
    
	while ((c = *utf8) != 0)
	{
		if ((c & 0x80) == 0 && pdf_doc_encoding[c] == c)
		{
			pdf[i++] = c;
			utf8++ ;
		}
		else
		{
			int rune;
			int j;
            
			utf8 += fz_chartorune(&rune, utf8);
            
			for (j = 0; j < sizeof(pdf_doc_encoding) && pdf_doc_encoding[j] != rune; j++)
				;
            
			if (j < sizeof(pdf_doc_encoding))
				pdf[i++] = j;
		}
	}
    
	pdf[i] = 0;
    
	return pdf;
}
static void reset_field(pdf_js *js, pdf_jsimp_obj *item)
{
	fz_context *ctx = js->doc->ctx;
	char *name = NULL;
	char *utf8 = pdf_jsimp_to_string(js->imp, item);
    
	if (utf8)
	{
		pdf_obj *field;
        
		fz_var(name);
		fz_try(ctx)
		{
			name = utf8_to_pdf(ctx, utf8);
			field = pdf_lookup_field(js->form, name);
			if (field)
				pdf_field_reset(js->doc, field);
		}
		fz_always(ctx)
		{
			fz_free(ctx, name);
		}
		fz_catch(ctx)
		{
			fz_rethrow(ctx);
		}
	}
}



static pdf_jsimp_obj *doc_getField(void *jsctx, void *obj, int argc, pdf_jsimp_obj *args[])
{
	pdf_js *js = (pdf_js *)jsctx;
	fz_context *ctx = js->doc->ctx;
	pdf_obj *dict = NULL;
	char *utf8;
	char *name = NULL;
    
	if (argc != 1)
		return NULL;
    
	fz_var(dict);
	fz_var(name);
	fz_try(ctx)
	{
		utf8 = pdf_jsimp_to_string(js->imp, args[0]);
        
		if (utf8)
		{
			name = utf8_to_pdf(ctx, utf8);
			dict = pdf_lookup_field(js->form, name);
		}
	}
	fz_always(ctx)
	{
		fz_free(ctx, name);
	}
	fz_catch(ctx)
	{
		/* FIXME: TryLater ? */
		fz_warn(ctx, "doc_getField failed: %s", fz_caught_message(ctx));
		dict = NULL;
	}
    
	return dict ? pdf_jsimp_new_obj(js->imp, js->fieldtype, dict) : NULL;
}

static pdf_jsimp_obj *doc_resetForm(void *jsctx, void *obj, int argc, pdf_jsimp_obj *args[])
{
	pdf_js *js = (pdf_js *)jsctx;
	fz_context *ctx = js->doc->ctx;
	pdf_jsimp_obj *arr = NULL;
	pdf_jsimp_obj *elem = NULL;
    
	switch (argc)
	{
        case 0:
            break;
        case 1:
            switch (pdf_jsimp_to_type(js->imp, args[0]))
		{
            case JS_TYPE_NULL:
                break;
            case JS_TYPE_ARRAY:
                arr = args[0];
                break;
            case JS_TYPE_STRING:
                elem = args[0];
                break;
            default:
                return NULL;
		}
            break;
        default:
            return NULL;
	}
    
	fz_try(ctx)
	{
		if(arr)
		{
			/* An array of fields has been passed in. Call
			 * pdf_reset_field on each */
			int i, n = pdf_jsimp_array_len(js->imp, arr);
            
			for (i = 0; i < n; i++)
			{
				pdf_jsimp_obj *item = pdf_jsimp_array_item(js->imp, arr, i);
                
				if (item)
					reset_field(js, item);
                
			}
		}
		else if (elem)
		{
			reset_field(js, elem);
		}
		else
		{
			/* No argument or null passed in means reset all. */
			int i, n = pdf_array_len(js->form);
            
			for (i = 0; i < n; i++)
				pdf_field_reset(js->doc, pdf_array_get(js->form, i));
		}
	}
	fz_catch(ctx)
	{
		fz_warn(ctx, "doc_resetForm failed: %s", fz_caught_message(ctx));
	}
    
	return NULL;
}

static pdf_jsimp_obj *doc_print(void *jsctx, void *obj, int argc, pdf_jsimp_obj *args[])
{
	pdf_js *js = (pdf_js *)jsctx;
    
	pdf_event_issue_print(js->doc);
    
	return NULL;
}

//static pdf_jsimp_obj *doc_mailDoc(void *jsctx, void *obj, int argc, pdf_jsimp_obj *args[])
//{
//	pdf_js *js = (pdf_js *)jsctx;
//	fz_context *ctx = js->doc->ctx;
//	pdf_jsimp_obj *bUI_obj = NULL;
//	pdf_jsimp_obj *cTo_obj = NULL;
//	pdf_jsimp_obj *cCc_obj = NULL;
//	pdf_jsimp_obj *cBcc_obj = NULL;
//	pdf_jsimp_obj *cSubject_obj = NULL;
//	pdf_jsimp_obj *cMessage_obj = NULL;
//	pdf_mail_doc_event event;
//	int arg_is_obj = 0;
//
//	if (argc < 1 || argc > 6)
//		return NULL;
//
//	event.ask_user = 1;
//	event.to = "";
//	event.cc = "";
//	event.bcc = "";
//	event.subject = "";
//	event.message = "";
//
//	fz_var(bUI_obj);
//	fz_var(cTo_obj);
//	fz_var(cCc_obj);
//	fz_var(cBcc_obj);
//	fz_var(cSubject_obj);
//	fz_var(cMessage_obj);
//	fz_try(ctx)
//	{
//		arg_is_obj = (argc == 1 && pdf_jsimp_to_type(js->imp, args[0]) != JS_TYPE_BOOLEAN);
//		if (arg_is_obj)
//		{
//			bUI_obj = pdf_jsimp_property(js->imp, args[0], "bUI");
//			cTo_obj = pdf_jsimp_property(js->imp, args[0], "cTo");
//			cCc_obj = pdf_jsimp_property(js->imp, args[0], "cCc");
//			cBcc_obj = pdf_jsimp_property(js->imp, args[0], "cBcc");
//			cSubject_obj = pdf_jsimp_property(js->imp, args[0], "cSubject");
//			cMessage_obj = pdf_jsimp_property(js->imp, args[0], "cMessage");
//		}
//		else
//		{
//			switch (argc)
//			{
//			case 6:
//				cMessage_obj = args[5];
//			case 5:
//				cSubject_obj = args[4];
//			case 4:
//				cBcc_obj = args[3];
//			case 3:
//				cCc_obj = args[2];
//			case 2:
//				cTo_obj = args[1];
//			case 1:
//				bUI_obj = args[0];
//			}
//		}
//
//		if (bUI_obj)
//			event.ask_user = (int)pdf_jsimp_to_number(js->imp, bUI_obj);
//
//		if (cTo_obj)
//			event.to = pdf_jsimp_to_string(js->imp, cTo_obj);
//
//		if (cCc_obj)
//			event.cc = pdf_jsimp_to_string(js->imp, cCc_obj);
//
//		if (cBcc_obj)
//			event.bcc = pdf_jsimp_to_string(js->imp, cBcc_obj);
//
//		if (cSubject_obj)
//			event.subject = pdf_jsimp_to_string(js->imp, cSubject_obj);
//
//		if (cMessage_obj)
//			event.message = pdf_jsimp_to_string(js->imp, cMessage_obj);
//
//		pdf_event_issue_mail_doc(js->doc, &event);
//	}
//	fz_always(ctx)
//	{
//		if (arg_is_obj)
//		{
//			pdf_jsimp_drop_obj(js->imp, bUI_obj);
//			pdf_jsimp_drop_obj(js->imp, cTo_obj);
//			pdf_jsimp_drop_obj(js->imp, cCc_obj);
//			pdf_jsimp_drop_obj(js->imp, cBcc_obj);
//			pdf_jsimp_drop_obj(js->imp, cSubject_obj);
//			pdf_jsimp_drop_obj(js->imp, cMessage_obj);
//		}
//	}
//	fz_catch(ctx)
//	{
//		fz_rethrow(ctx);
//	}
//
//	return NULL;
//}





static pdf_jsimp_obj *app_alert(void *jsctx, void *obj, int argc, pdf_jsimp_obj *args[])
{
	pdf_js *js = (pdf_js *)jsctx;
	fz_context *ctx = js->doc->ctx;
	pdf_jsimp_obj *cMsg_obj = NULL;
	pdf_jsimp_obj *nIcon_obj = NULL;
	pdf_jsimp_obj *nType_obj = NULL;
	pdf_jsimp_obj *cTitle_obj = NULL;
	pdf_jsimp_obj *nButton_obj = NULL;
	pdf_alert_event event;
	int arg_is_obj = 0;
    
	if (argc < 1 || argc > 6)
		return NULL;
    
	event.message = "";
	event.icon_type = PDF_ALERT_ICON_ERROR;
	event.button_group_type = PDF_ALERT_BUTTON_GROUP_OK;
	event.title = "MuPDF";
	event.check_box_message = NULL;
	event.button_pressed = 0;
    
	fz_var(cMsg_obj);
	fz_var(nIcon_obj);
	fz_var(nType_obj);
	fz_var(cTitle_obj);
	fz_try(ctx)
	{
		arg_is_obj = (argc == 1 && pdf_jsimp_to_type(js->imp, args[0]) != JS_TYPE_STRING);
		if (arg_is_obj)
		{
			cMsg_obj = pdf_jsimp_property(js->imp, args[0], "cMsg");
			nIcon_obj = pdf_jsimp_property(js->imp, args[0], "nIcon");
			nType_obj = pdf_jsimp_property(js->imp, args[0], "nType");
			cTitle_obj = pdf_jsimp_property(js->imp, args[0], "cTitle");
		}
		else
		{
			switch (argc)
			{
                case 6:
                case 5:
                case 4:
                    cTitle_obj = args[3];
                case 3:
                    nType_obj = args[2];
                case 2:
                    nIcon_obj = args[1];
                case 1:
                    cMsg_obj = args[0];
			}
		}
        
		if (cMsg_obj)
			event.message = pdf_jsimp_to_string(js->imp, cMsg_obj);
        
		if (nIcon_obj)
			event.icon_type = (int)pdf_jsimp_to_number(js->imp, nIcon_obj);
        
		if (nType_obj)
			event.button_group_type = (int)pdf_jsimp_to_number(js->imp, nType_obj);
        
		if (cTitle_obj)
			event.title = pdf_jsimp_to_string(js->imp, cTitle_obj);
        
		pdf_event_issue_alert(js->doc, &event);
		nButton_obj = pdf_jsimp_from_number(js->imp, (double)event.button_pressed);
	}
	fz_always(ctx)
	{
		if (arg_is_obj)
		{
			pdf_jsimp_drop_obj(js->imp, cMsg_obj);
			pdf_jsimp_drop_obj(js->imp, nIcon_obj);
			pdf_jsimp_drop_obj(js->imp, nType_obj);
			pdf_jsimp_drop_obj(js->imp, cTitle_obj);
		}
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
    
	return nButton_obj;
}

static pdf_jsimp_obj *app_execDialog(void *jsctx, void *obj, int argc, pdf_jsimp_obj *args[])
{
	pdf_js *js = (pdf_js *)jsctx;
    
	pdf_event_issue_exec_dialog(js->doc);
    
	return NULL;
}

static pdf_jsimp_obj *app_execMenuItem(void *jsctx, void *obj, int argc, pdf_jsimp_obj *args[])
{
	pdf_js *js = (pdf_js *)jsctx;
    
	if (argc == 1)
		pdf_event_issue_exec_menu_item(js->doc, pdf_jsimp_to_string(js->imp, args[0]));
    
	return NULL;
}

static pdf_jsimp_obj *app_launchURL(void *jsctx, void *obj, int argc, pdf_jsimp_obj *args[])
{
	pdf_js *js = (pdf_js *)jsctx;
	char *cUrl;
	int bNewFrame = 0;
    
	switch (argc)
	{
        default:
            return NULL;
        case 2:
            bNewFrame = (int)pdf_jsimp_to_number(js->imp, args[1]);
        case 1:
            cUrl = pdf_jsimp_to_string(js->imp, args[0]);
	}
    
	pdf_event_issue_launch_url(js->doc, cUrl, bNewFrame);
    
	return NULL;
}
/*
get the value of a property
 */
static pdf_jsimp_obj *doc_getEvent(void *jsctx, void *obj)
{
	pdf_js *js = (pdf_js *)jsctx;
    
	return pdf_jsimp_new_obj(js->imp, js->eventtype, &js->event);
}
static pdf_jsimp_obj *field_getDisplay(void *jsctx, void *obj)
{
	pdf_js *js = (pdf_js *)jsctx;
	pdf_obj *field = (pdf_obj *)obj;
    
	return field ? pdf_jsimp_from_number(js->imp, (double)pdf_field_display(js->doc, field)) : NULL;
}
static pdf_jsimp_obj *field_getValue(void *jsctx, void *obj)
{
	pdf_js *js = (pdf_js *)jsctx;
    
	pdf_obj *field = (pdf_obj *)(JSObjectGetPrivate((JSObjectRef)obj));
	char *fval;
    
	if (!field)
		return NULL;
    
	fval = pdf_field_value(js->doc, field);
	return pdf_jsimp_from_string(js->imp, fval?fval:"");
}

//get the ocg's name
static pdf_jsimp_obj *ocg_getName(void *jsctx, void *obj)
{
	pdf_js *js = (pdf_js *)jsctx;
    
	pdf_obj *oobj = (pdf_obj *)(JSObjectGetPrivate((JSObjectRef)obj));
    pdf_obj *oc = pdf_load_object(js->doc, pdf_to_num(oobj), pdf_to_num(oobj));
	char *val;
    
	if (!oobj)
		return NULL;
  
	val = pdf_to_str_buf( pdf_dict_getp(oc, "Name"));
	return pdf_jsimp_from_string(js->imp, val?val:"");
}
//get the ocg's state
static pdf_jsimp_obj *ocg_getState(void *jsctx, void *obj)
{
	pdf_js *js = (pdf_js *)jsctx;
    
	pdf_obj *oobj = (pdf_obj *)(JSObjectGetPrivate((JSObjectRef)obj));
	char *val;
    
	if (!oobj)
		return NULL;

	val = pdf_to_name( pdf_dict_getp(oobj, "Usage/View/ViewState"));
	return pdf_jsimp_from_string(js->imp, val?val:"");
  
}

//no use
static pdf_jsimp_obj *doc_getGlobal(void *jsctx, void *obj)
{
	pdf_js *js = (pdf_js *)jsctx;
    
	return pdf_jsimp_new_obj(js->imp, js->apptype, NULL);
}

static pdf_jsimp_obj *doc_getApp(void *jsctx, void *obj)
{
	pdf_js *js = (pdf_js *)jsctx;
    
	return pdf_jsimp_new_obj(js->imp, js->apptype, NULL);
}

/*
 set the value of property
 */
static void doc_setEvent(void *jsctx, void *obj, pdf_jsimp_obj *val)
{
	pdf_js *js = (pdf_js *)jsctx;
	fz_warn(js->doc->ctx, "Unexpected call to doc_setEvent");
}

static void field_setDisplay(void *jsctx, void *obj, pdf_jsimp_obj *val)
{
	pdf_js *js = (pdf_js *)jsctx;
	pdf_obj *field = (pdf_obj *)(JSObjectGetPrivate(obj));
	if (field)
		pdf_field_set_display(js->doc, field, (int)pdf_jsimp_to_number(js->imp, val));
}
static void field_setValue(void *jsctx, void *obj, pdf_jsimp_obj *val)
{
	pdf_js *js = (pdf_js *)jsctx;
	pdf_obj *field = (pdf_obj *)(JSObjectGetPrivate((JSObjectRef)obj));
    
	if (field)
		(void)pdf_field_set_value(js->doc, field, pdf_jsimp_to_string(js->imp, val));
}

/*
 set the ocg property's state value
*/
static void ocg_setState(void *jsctx, void *obj, pdf_jsimp_obj *val)
{
	pdf_js *js = (pdf_js *)jsctx;
	pdf_obj *o = (pdf_obj *)(JSObjectGetPrivate((JSObjectRef)obj));
    bool bo;
    pdf_obj *oo = pdf_load_object(js->doc, pdf_to_num(o), pdf_to_gen(o));
  //  pdf_obj * p1 = pdf_dict_getp(oo, "Usage/View/ViewState");
    pdf_obj * pdf_state;
    int i =0;
    char *sstate =NULL ;
	if (o)
    {
        bo = pdf_jsimp_to_boolean(js->imp,val);
        if(bo)
        {
            sstate = "ON";
            for(i =0 ;i<js->doc->ocg->len;i++)
         
                if(pdf_to_num(o) == js->doc->ocg[i].ocgs->num && pdf_to_gen(o) ==js->doc->ocg[i].ocgs->gen)
                {
                    js->doc->ocg[i].ocgs->state = 1;
                    break ;
                }

        }
        else
        {
            sstate = "OFF";
//          for(i =0 ;i<js->doc->ocg->len;i++)
//              //  printf("%d,%d",js->doc->ocg[i].ocgs->num,js->doc->ocg[i].ocgs->gen);
//                if(pdf_to_num(o) == js->doc->ocg[i].ocgs->num &&pdf_to_gen(o) ==js->doc->ocg[i].ocgs->gen)
//                {
//                    printf("%d,%d",js->doc->ocg[i].ocgs->num,js->doc->ocg[i].ocgs->gen);
//                    js->doc->ocg[i].ocgs->state = 0;
//                    break ;
//                }
        }
    
    }
   pdf_state =  pdf_new_name(js->doc, sstate);
    pdf_dict_putp(oo, "Usage/View/ViewState", pdf_state);

    
}
//no use
static void ocg_setName(void *jsctx, void *obj, pdf_jsimp_obj *val)
{
	pdf_js *js = (pdf_js *)jsctx;
	fz_warn(js->doc->ctx, "Unexpected call to doc_setApp");
}
//no use
static void doc_setGlobal(void *jsctx, void *obj, pdf_jsimp_obj *val)
{
	pdf_js *js = (pdf_js *)jsctx;
	fz_warn(js->doc->ctx, "Unexpected call to doc_setApp");
}

static void doc_setApp(void *jsctx, void *obj, pdf_jsimp_obj *val)
{
	pdf_js *js = (pdf_js *)jsctx;
	fz_warn(js->doc->ctx, "Unexpected call to doc_setApp");
}
static pdf_jsimp_obj *doc_mailDoc(void *jsctx, void *obj, int argc, pdf_jsimp_obj *args[])
{
	pdf_js *js = (pdf_js *)jsctx;
	fz_context *ctx = js->doc->ctx;
	pdf_jsimp_obj *bUI_obj = NULL;
	pdf_jsimp_obj *cTo_obj = NULL;
	pdf_jsimp_obj *cCc_obj = NULL;
	pdf_jsimp_obj *cBcc_obj = NULL;
	pdf_jsimp_obj *cSubject_obj = NULL;
	pdf_jsimp_obj *cMessage_obj = NULL;
	pdf_mail_doc_event event;
	int arg_is_obj = 0;
    
	if (argc < 1 || argc > 6)
		return NULL;
    
	event.ask_user = 1;
	event.to = "";
	event.cc = "";
	event.bcc = "";
	event.subject = "";
	event.message = "";
    
	fz_var(bUI_obj);
	fz_var(cTo_obj);
	fz_var(cCc_obj);
	fz_var(cBcc_obj);
	fz_var(cSubject_obj);
	fz_var(cMessage_obj);
	fz_try(ctx)
	{
		arg_is_obj = (argc == 1 && pdf_jsimp_to_type(js->imp, args[0]) != JS_TYPE_BOOLEAN);
		if (arg_is_obj)
		{
			bUI_obj = pdf_jsimp_property(js->imp, args[0], "bUI");
			cTo_obj = pdf_jsimp_property(js->imp, args[0], "cTo");
			cCc_obj = pdf_jsimp_property(js->imp, args[0], "cCc");
			cBcc_obj = pdf_jsimp_property(js->imp, args[0], "cBcc");
			cSubject_obj = pdf_jsimp_property(js->imp, args[0], "cSubject");
			cMessage_obj = pdf_jsimp_property(js->imp, args[0], "cMessage");
		}
		else
		{
			switch (argc)
			{
                case 6:
                    cMessage_obj = args[5];
                case 5:
                    cSubject_obj = args[4];
                case 4:
                    cBcc_obj = args[3];
                case 3:
                    cCc_obj = args[2];
                case 2:
                    cTo_obj = args[1];
                case 1:
                    bUI_obj = args[0];
			}
		}
        
		if (bUI_obj)
			event.ask_user = (int)pdf_jsimp_to_number(js->imp, bUI_obj);
        
		if (cTo_obj)
			event.to = pdf_jsimp_to_string(js->imp, cTo_obj);
        
		if (cCc_obj)
			event.cc = pdf_jsimp_to_string(js->imp, cCc_obj);
        
		if (cBcc_obj)
			event.bcc = pdf_jsimp_to_string(js->imp, cBcc_obj);
        
		if (cSubject_obj)
			event.subject = pdf_jsimp_to_string(js->imp, cSubject_obj);
        
		if (cMessage_obj)
			event.message = pdf_jsimp_to_string(js->imp, cMessage_obj);
        
		pdf_event_issue_mail_doc(js->doc, &event);
	}
	fz_always(ctx)
	{
		if (arg_is_obj)
		{
			pdf_jsimp_drop_obj(js->imp, bUI_obj);
			pdf_jsimp_drop_obj(js->imp, cTo_obj);
			pdf_jsimp_drop_obj(js->imp, cCc_obj);
			pdf_jsimp_drop_obj(js->imp, cBcc_obj);
			pdf_jsimp_drop_obj(js->imp, cSubject_obj);
			pdf_jsimp_drop_obj(js->imp, cMessage_obj);
		}
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
    
	return NULL;
}


static void declare_dom(pdf_js *js)
{
	pdf_jsimp *imp = js->imp;
    
	/* Create the document type */
	js->doctype = pdf_jsimp_new_type(imp, NULL);
    
    //add the unlock function ,it doesnt with the unlock
    //pdf_jsimp_addmethod(imp, js->doctype, "ppdfOnUnlock", doc_pddfUnlock);
    pdf_jsimp_addmethod(imp, js->doctype, "getOCGs", doc_getOCGs);
    
    pdf_jsimp_addmethod(imp, js->doctype, "getField", doc_getField);
	pdf_jsimp_addmethod(imp, js->doctype, "resetForm", doc_resetForm);
	pdf_jsimp_addmethod(imp, js->doctype, "print", doc_print);
	pdf_jsimp_addmethod(imp, js->doctype, "mailDoc", doc_mailDoc);
	pdf_jsimp_addproperty(imp, js->doctype, "event", doc_getEvent, doc_setEvent);
	pdf_jsimp_addproperty(imp, js->doctype, "app", doc_getApp, doc_setApp);//doc_getApp return the apptype pdf_jsimp_object
    pdf_jsimp_addproperty(imp, js->doctype, "global", doc_getGlobal, doc_setGlobal);
    pdf_jsimp_addmethod(imp, js->doctype, "submitForm", doc_submitForm);
    pdf_jsimp_addmethod(imp, js->doctype, "getOCGs", doc_getOCGs);
    pdf_jsimp_addproperty(imp, js->doctype, "name", ocg_getName, ocg_setName);
    pdf_jsimp_addproperty(imp, js->doctype, "state", ocg_getState, ocg_setState);
    /* Create the event type */
	js->eventtype = pdf_jsimp_new_type(imp, NULL);//add the property to the event type object
    //	pdf_jsimp_addproperty(imp, js->eventtype, "target", event_getTarget, event_setTarget);
    //	pdf_jsimp_addproperty(imp, js->eventtype, "value", event_getValue, event_setValue);
    //	pdf_jsimp_addproperty(imp, js->eventtype, "willCommit", event_getWillCommit, event_setWillCommit);
    //	pdf_jsimp_addproperty(imp, js->eventtype, "rc", event_getRC, event_setRC);
    
	/* Create the field type */
	js->fieldtype = pdf_jsimp_new_type(imp, NULL);//add the property to the filed type object
    pdf_jsimp_addproperty(imp, js->fieldtype, "value", field_getValue, field_setValue);
    //	pdf_jsimp_addproperty(imp, js->fieldtype, "borderStyle", field_getBorderStyle, field_setBorderStyle);
    //	pdf_jsimp_addproperty(imp, js->fieldtype, "textColor", field_getTextColor, field_setTextColor);
    //	pdf_jsimp_addproperty(imp, js->fieldtype, "fillColor", field_getFillColor, field_setFillColor);
    	pdf_jsimp_addproperty(imp, js->fieldtype, "display", field_getDisplay, field_setDisplay);
    //	pdf_jsimp_addproperty(imp, js->fieldtype, "name", field_getName, field_setName);
    //	pdf_jsimp_addmethod(imp, js->fieldtype, "buttonSetCaption", field_buttonSetCaption);
    
	/* Create the app type */
	js->apptype = pdf_jsimp_new_type(imp, NULL);//add the method to app variable
	pdf_jsimp_addmethod(imp, js->apptype, "alert", app_alert);
	pdf_jsimp_addmethod(imp, js->apptype, "execDialog", app_execDialog);
	pdf_jsimp_addmethod(imp, js->apptype, "execMenuItem", app_execMenuItem);
	pdf_jsimp_addmethod(imp, js->apptype, "launchURL", app_launchURL);
    
   // pdf_jsimp_addproperty(imp, js->doctype, "boom_gui", golbal_getboom_guid, <#pdf_jsimp_setter *set#>)
	/* Create the document object and tell the engine to use */
	//pdf_jsimp_set_global_type(js->imp, js->doctype);
}

static void preload_helpers(pdf_js *js)
{
	/* When testing on the cluster, redefine the Date object
	 * to use a fixed date */
#ifdef CLUSTER
	pdf_jsimp_execute(js->imp,
                      "var MuPDFOldDate = Date\n"
                      "Date = function() { return new MuPDFOldDate(1979,5,15); }\n"
                      );
#endif
    
	pdf_jsimp_execute(js->imp,
#include "gen_js_util.h"
                      );
}

pdf_js *pdf_new_js(pdf_document *doc)
{
	fz_context *ctx = doc->ctx;
	pdf_js *js = NULL;
    
	fz_var(js);
	fz_try(ctx)
	{
		pdf_obj *root, *acroform;
        
		js = fz_malloc_struct(ctx, pdf_js);
		js->doc = doc;
        
		/* Find the form array */
		root = pdf_dict_gets(pdf_trailer(doc), "Root");
		acroform = pdf_dict_gets(root, "AcroForm");
		js->form = pdf_dict_gets(acroform, "Fields");
        
		/* Initialise the javascript engine, passing the main context
		 * for use in memory allocation and exception handling. Also
		 * pass our js context, for it to pass back to us. */
		js->imp = pdf_new_jsimp(ctx, js);
		declare_dom(js);
        
		preload_helpers(js);
	}
	fz_catch(ctx)
	{
		pdf_drop_js(js);
		js = NULL;
	}
    
	return js;
}

void pdf_js_load_document_level(pdf_js *js)
{
    pdf_document *doc = js->doc;
	fz_context *ctx = doc->ctx;
	pdf_obj *javascript = NULL;
    char *codebuf = NULL;
   // char codebuf[100000];
    
	fz_var(javascript);
	fz_var(codebuf);
	fz_try(ctx)
	{
		int len, i;
        
		javascript = pdf_load_name_tree(doc, "JavaScript");
		len = pdf_dict_len(javascript);
        
		for (i = 0; i < len; i++)
        {
            pdf_obj *fragment = pdf_dict_get_val(javascript, i);
            pdf_obj *code = pdf_dict_gets(fragment, "JS");
//
//           FILE *f=fopen("/Volumes/Mac_HD_2/Work/project/mupdf_study/platform/ios/test.js","r");
//           fread(codebuf, sizeof(char), 100000, f);
            
			fz_var(codebuf);
			fz_try(ctx)
			{
				codebuf = pdf_to_utf8(doc, code);
//                FILE *f=fopen("/Volumes/Mac_HD_2/Work/project/mupdf_study/platform/ios/test2.js","w");
//                           fwrite(codebuf, sizeof(char), 100000, f);
              //  printf("%s\n",codebuf);
				pdf_jsimp_execute(js->imp, codebuf);
			}
			fz_always(ctx)
			{
				fz_free(ctx, codebuf);
				codebuf = NULL;
			}
			fz_catch(ctx)
			{
				/* FIXME: TryLater ? */
				fz_warn(ctx, "Warning: %s", fz_caught_message(ctx));
			}
		}
	}
	fz_always(ctx)
	{
		pdf_drop_obj(javascript);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
    
}

void pdf_drop_js(pdf_js *js)
{
}

void pdf_js_setup_event(pdf_js *js, pdf_js_event *e)
{
}

pdf_js_event *pdf_js_get_event(pdf_js *js)
{
	return NULL;
}

void pdf_js_execute(pdf_js *js, char *code)
{
    if (js)
	{
		fz_context *ctx = js->doc->ctx;
		fz_try(ctx)
		{
			pdf_jsimp_execute(js->imp, code);
		}
		fz_catch(ctx)
		{
		}
//        if(!strcmp(code, "cy();"))
//            pdf_jsimp_execute(js->imp, "a('0xFFCD11A0E6F5CAE0', false);");
	}
    
}

void pdf_js_execute_count(pdf_js *js, char *code, int count)
{
}

int pdf_js_supported(void)
{
	return 0;
}
