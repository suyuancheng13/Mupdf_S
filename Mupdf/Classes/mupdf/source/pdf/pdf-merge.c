//
//  pdf_merge.c
//  MuPDF
//
//  Created by SuYuanCheng on 13-10-10.
//  Copyright (c) 2013年 Artifex Software, Inc. All rights reserved.
//

#include <stdio.h>
#include"pdf-merge.h"
#include"pdf-password.h"
#define TEMP_FILE "tem_merge.pdf"
#define TEMP_FILE1 "tem1_merge.pdf"
#include "mupdf/fitz.h"
#include "mupdf/pdf.h" /* for mujstest */

char *tempFile=NULL;
char *tempFile1=NULL;
char *File =NULL;
#ifdef _MSC_VER
#include <winsock2.h>
#define main main_utf8
#else
#include <sys/time.h>
#endif

enum { TEXT_PLAIN = 1, TEXT_HTML = 2, TEXT_XML = 3 };

enum { OUT_PNG, OUT_PPM, OUT_PNM, OUT_PAM, OUT_PGM, OUT_PBM, OUT_SVG, OUT_PWG, OUT_PCL, OUT_PDF };

enum { CS_INVALID, CS_UNSET, CS_MONO, CS_GRAY, CS_GRAYALPHA, CS_RGB, CS_RGBA };

struct keyval
{
	pdf_obj *k;
	pdf_obj *v;
};
struct pdf_obj_s
{
	int refs;
	unsigned char kind;
	unsigned char flags;
	pdf_document *doc;
	int parent_num;
	union
	{
		int b;
		int i;
		float f;
		struct {
			unsigned short len;
			char buf[1];
		} s;
		char n[1];
		struct {
			int len;
			int cap;
			pdf_obj **items;
		} a;
		struct {
			int len;
			int cap;
			struct keyval *items;
		} d;
		struct {
			int num;
			int gen;
		} r;
	} u;
};
typedef struct
{
	char *suffix;
	int format;
} suffix_t;

static const suffix_t suffix_table[] =
{
	{ ".png", OUT_PNG },
	{ ".pgm", OUT_PGM },
	{ ".ppm", OUT_PPM },
	{ ".pnm", OUT_PNM },
	{ ".pam", OUT_PAM },
	{ ".pbm", OUT_PBM },
	{ ".svg", OUT_SVG },
	{ ".pwg", OUT_PWG },
	{ ".pcl", OUT_PCL },
	{ ".pdf", OUT_PDF },
};

typedef struct
{
	char *name;
	int colorspace;
} cs_name_t;

static const cs_name_t cs_name_table[] =
{
	{ "m", CS_MONO },
	{ "mono", CS_MONO },
	{ "g", CS_GRAY },
	{ "gray", CS_GRAY },
	{ "grey", CS_GRAY },
	{ "ga", CS_GRAYALPHA },
	{ "grayalpha", CS_GRAYALPHA },
	{ "greyalpha", CS_GRAYALPHA },
	{ "rgb", CS_RGB },
	{ "rgba", CS_RGBA },
	{ "rgbalpha", CS_RGBA }
};

typedef struct
{
	int format;
	int default_cs;
	int permitted_cs[6];
} format_cs_table_t;

static const format_cs_table_t format_cs_table[] =
{
	{ OUT_PNG, CS_RGB, { CS_GRAY, CS_GRAYALPHA, CS_RGB, CS_RGBA } },
	{ OUT_PPM, CS_RGB, { CS_GRAY, CS_RGB } },
	{ OUT_PNM, CS_GRAY, { CS_GRAY, CS_RGB } },
	{ OUT_PAM, CS_RGBA, { CS_RGBA } },
	{ OUT_PGM, CS_GRAY, { CS_GRAY, CS_RGB } },
	{ OUT_PBM, CS_MONO, { CS_MONO } },
	{ OUT_SVG, CS_RGB, { CS_RGB } },
	{ OUT_PWG, CS_RGB, { CS_MONO, CS_GRAY, CS_RGB } },
	{ OUT_PCL, CS_MONO, { CS_MONO } },
	{ OUT_PDF, CS_RGB, { CS_RGB } }
};

/*
 A useful bit of bash script to call this to generate mjs files:
 for f in tests_private/pdf/forms/v1.3/ *.pdf ; do g=${f%.*} ; echo $g ; ../mupdf.git/win32/debug/mudraw.exe -j $g.mjs $g.pdf ; done
 
 Remove the space from "/ *.pdf" before running - can't leave that
 in here, as it causes a warning about a possibly malformed comment.
 */


static char *output = NULL;

static float rotation = 0;
static pdf_document *pdfout = NULL;
static int showoutline = 0;
static int uselist = 1;
static int alphabits = 8;
static int errored = 0;
static int ignore_errors = 0;
static int output_format;
static int out_cs = CS_UNSET;
static fz_colorspace *colorspace;
static char *filename;
fz_output *out = NULL;


static int gettime(void)
{
	static struct timeval first;
	static int once = 1;
	struct timeval now;
	if (once)
	{
		gettimeofday(&first, NULL);
		once = 0;
	}
	gettimeofday(&now, NULL);
	return (now.tv_sec - first.tv_sec) * 1000 + (now.tv_usec - first.tv_usec) / 1000;
}

static int isrange(char *s)
{
	while (*s)
	{
		if ((*s < '0' || *s > '9') && *s != '-' && *s != ',')
			return 0;
		s++;
	}
	return 1;
}

static void escape_string(FILE *out, int len, const char *string)
{
	while (len-- && *string)
	{
		char c = *string++;
		switch (c)
		{
            case '\n':
                fputc('\\', out);
                fputc('n', out);
                break;
            case '\r':
                fputc('\\', out);
                fputc('r', out);
                break;
            case '\t':
                fputc('\\', out);
                fputc('t', out);
                break;
            default:
                fputc(c, out);
		}
	}
}

static void drawpage(fz_context *ctx, fz_document *doc, int pagenum)
{
	fz_page *page;
	fz_display_list *list = NULL;
	fz_device *dev = NULL;
	int start;
	fz_cookie cookie = { 0 };
	int needshot = 0;
    
	fz_var(list);
	fz_var(dev);
	fz_try(ctx)
	{
		page = fz_load_page(doc, pagenum - 1);
	}
	fz_catch(ctx)
	{
		fz_rethrow_message(ctx, "cannot load page %d in file '%s'", pagenum, filename);
	}
    
	
	if (uselist)
	{
		fz_try(ctx)
		{
			list = fz_new_display_list(ctx);
			dev = fz_new_list_device(ctx, list);
			fz_run_page(doc, page, dev, &fz_identity, &cookie);
		}
		fz_always(ctx)
		{
			fz_free_device(dev);
			dev = NULL;
		}
		fz_catch(ctx)
		{
			fz_drop_display_list(ctx, list);
			fz_free_page(doc, page);
			fz_rethrow_message(ctx, "cannot draw page %d in file '%s'", pagenum, filename);
		}
	}
    
    
	if (pdfout)
	{
		fz_matrix ctm;
		fz_rect bounds, tbounds;
		pdf_page *newpage;
        
		fz_bound_page(doc, page, &bounds);
		fz_rotate(&ctm, rotation);
		tbounds = bounds;
		fz_transform_rect(&tbounds, &ctm);
        
		newpage = pdf_create_page(pdfout, bounds, 72, 0);
        
		fz_try(ctx)
		{
			dev = pdf_page_write(pdfout, newpage);
			if (list)
				fz_run_display_list(list, dev, &ctm, &tbounds, &cookie);
			else
				fz_run_page(doc, page, dev, &ctm, &cookie);
			fz_free_device(dev);
			dev = NULL;
		}
		fz_always(ctx)
		{
			fz_free_device(dev);
			dev = NULL;
		}
		fz_catch(ctx)
		{
			fz_drop_display_list(ctx, list);
			fz_free_page(doc, page);
			fz_rethrow(ctx);
		}
		pdf_insert_page(pdfout, newpage, INT16_MAX);
		pdf_free_page(pdfout, newpage);
	}
    
	if (list)
		fz_drop_display_list(ctx, list);
    
	fz_free_page(doc, page);
	fz_flush_warnings(ctx);
    
    
	if (cookie.errors)
		errored = 1;
}

static void drawrange(fz_context *ctx, fz_document *doc, char *range)
{
	int page, spage, epage, pagecount;
	char *spec, *dash;
    
	pagecount = fz_count_pages(doc);
	spec = fz_strsep(&range, ",");
	while (spec)
	{
		dash = strchr(spec, '-');
        
		if (dash == spec)
			spage = epage = pagecount;
		else
			spage = epage = atoi(spec);
        
		if (dash)
		{
			if (strlen(dash) > 1)
				epage = atoi(dash + 1);
			else
				epage = pagecount;
		}
        
		spage = fz_clampi(spage, 1, pagecount);
		epage = fz_clampi(epage, 1, pagecount);
        
		if (spage < epage)
			for (page = spage; page <= epage; page++)
				drawpage(ctx, doc, page);
		else
			for (page = spage; page >= epage; page--)
				drawpage(ctx, doc, page);
        
		spec = fz_strsep(&range, ",");
	}
}

static void drawoutline(fz_context *ctx, fz_document *doc)
{
	fz_outline *outline = fz_load_outline(doc);
	fz_output *out = NULL;
    
	fz_var(out);
	fz_try(ctx)
	{
		out = fz_new_output_with_file(ctx, stdout);
		if (showoutline > 1)
			fz_print_outline_xml(ctx, out, outline);
		else
			fz_print_outline(ctx, out, outline);
	}
	fz_always(ctx)
	{
		fz_close_output(out);
		fz_free_outline(ctx, outline);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}

static int
parse_colorspace(const char *name)
{
	int i;
    
	for (i = 0; i < nelem(cs_name_table); i++)
	{
		if (!strcmp(name, cs_name_table[i].name))
			return cs_name_table[i].colorspace;
	}
	fprintf(stderr, "Unknown colorspace \"%s\"\n", name);
	exit(1);
	return -1;
}

int pdf_merge_or_split(char *outputfile, char *filename,char *passWord ,char *page)
{
	char *password = passWord;
	fz_document *doc = NULL;
	fz_context *ctx;
    int fd = -1;
    
	fz_var(doc);
    
    output = outputfile;
    
	if (!output )
	{
		printf("nothing to do\n");
		exit(0);
	}
    
	ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
	if (!ctx)
	{
		fprintf(stderr, "cannot initialise context\n");
		exit(1);
	}
    
	fz_set_aa_level(ctx, alphabits);
    
	/* Determine output type */
	output_format = OUT_PDF;
    
	{
		int i, j;
        
		for (i = 0; i < nelem(format_cs_table); i++)
		{
			if (format_cs_table[i].format == output_format)
			{
				if (out_cs == CS_UNSET)
					out_cs = format_cs_table[i].default_cs;
				for (j = 0; j < nelem(format_cs_table[i].permitted_cs); j++)
				{
					if (format_cs_table[i].permitted_cs[j] == out_cs)
						break;
				}
				if (j == nelem(format_cs_table[i].permitted_cs))
				{
					fprintf(stderr, "Unsupported colorspace for this format\n");
					exit(1);
				}
			}
		}
	}
    
	switch (out_cs)
	{
        case CS_MONO:
        case CS_GRAY:
        case CS_GRAYALPHA:
            colorspace = fz_device_gray(ctx);
            break;
        case CS_RGB:
        case CS_RGBA:
            colorspace = fz_device_rgb(ctx);
            break;
        default:
            fprintf(stderr, "Unknown colorspace!\n");
            exit(1);
            break;
	}
    
	if (output_format == OUT_PDF)
	{
        
        
        fz_context *ctxx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
        fd = open(output, O_BINARY | O_RDONLY, 0);
        /*
         the file exist, guess the function is to merge file
         */
        if(fd!=-1)
        {
            pdfout = pdf_open_document(ctxx, output);
            /*
             load page tree
             */
            pdf_load_page(pdfout, 0);
        }
        /*
         guess the function is to split
         */
        else{
            pdfout = pdf_create_document(ctx);
        }
        
	}
    
	fz_try(ctx)
	{
        
		{
			fz_try(ctx)
			{
				fz_try(ctx)
				{
					doc = fz_open_document(ctx, filename);
				}
				fz_catch(ctx)
				{
					fz_rethrow_message(ctx, "cannot open document: %s", filename);
				}
                
				if (fz_needs_password(doc))
				{
					if (!fz_authenticate_password(doc, password))
						fz_throw(ctx, FZ_ERROR_GENERIC, "cannot authenticate password: %s", filename);
				}
                
				if (showoutline)
					drawoutline(ctx, doc);
                
				if ( output )
				{
					if (!isrange(page))
						drawrange(ctx, doc, "1-");
					if (isrange(page))
						drawrange(ctx, doc, page);
				}
				fz_close_document(doc);
				doc = NULL;
			}
			fz_catch(ctx)
			{
				if (!ignore_errors)
					fz_rethrow(ctx);
                
				fz_close_document(doc);
				doc = NULL;
				fz_warn(ctx, "ignoring error in '%s'", filename);
			}
		}
	}
	fz_catch(ctx)
	{
		fz_close_document(doc);
		fprintf(stderr, "error: cannot draw '%s'\n", filename);
		errored = 1;
	}
    
	if (pdfout)
	{
		fz_write_options opts = { 0 };
        if(fd!=-1)
            opts.do_incremental=1;
		pdf_write_document(pdfout, output, &opts);
		pdf_close_document(pdfout);
	}
    
	fz_free_context(ctx);
	return (errored != 0);
}


static
void save(pdf_document *doc,char *filename)
{
    fz_write_options ops;
    int erro = 0 ;
    ops.do_incremental = 1;
    ops.continue_on_error = 1;
    ops.do_linear = 0;
    ops.do_garbage = 1;
    ops.do_expand = 0 ;
    ops.do_ascii = 0 ;
    ops.errors = &erro;
    pdf_write_document(doc, filename, &ops);
//    pdf_close_document(doc);
}

/*
 linearize the file
 */
static
pdf_document *linearize_file(pdf_document *doc)
{
    fz_write_options ops;
    pdf_document *tdoc;
    fz_context *ctx = doc->ctx;
    File = (char *)malloc(strlen(tempFile1)*sizeof(char));
    strncpy(File, tempFile1, strlen(tempFile1));
    
    int erro = 0 ;
    ops.do_incremental = 0;
    ops.continue_on_error = 1;
    ops.do_linear = 0;
    ops.do_garbage = 1;
    ops.do_expand = 0 ;
    ops.do_ascii = 0 ;
    ops.errors = &erro;
    char *temp = rindex(File, '/');
    strncpy(temp+1, TEMP_FILE,strlen(TEMP_FILE));
    *(temp+1+strlen(TEMP_FILE))='\0';
    
    pdf_write_document(doc, File, &ops);
    // pdf_close_document(doc);
    tdoc = pdf_open_document(ctx, File);
//    unlink(File);
//     free(File);
    return tdoc;
}
/*
 linearize the file
 */
static
pdf_document *linearize(pdf_document *doc,char *oldfile)
{
    fz_write_options ops;
    pdf_document *tdoc;
    pdf_document *ttdoc;
    fz_context *ctx = doc->ctx;
    
    tempFile = (char *)malloc(strlen(oldfile)*sizeof(char));
    strncpy(tempFile, oldfile, strlen(oldfile));
    int erro = 0 ;
    ops.do_incremental = 0;
    ops.continue_on_error = 1;
    ops.do_linear = 0;
    ops.do_garbage = 0;
    ops.do_expand = 0 ;
    ops.do_ascii = 0 ;
    ops.errors = &erro;
    
    char *temp = rindex(tempFile, '/');
    strncpy(temp+1, TEMP_FILE1,strlen(TEMP_FILE1));
    *(temp+1+strlen(TEMP_FILE1))='\0';
    
    pdf_write_document(doc, oldfile, &ops);//replace the origin file
    tdoc = pdf_open_document(ctx, oldfile);
 
    free(tempFile);
    return tdoc;
}
/*
 *get the date of current day
 */
static
char * pdf_get_date()
{
    time_t date = time(NULL);
    struct tm *gdate = gmtime(&date);
    char *da= (char *)malloc(25*sizeof(char));
    char *tem = (char *)malloc(25*sizeof(char));
    sprintf(da, "%s","D:");
    sprintf(tem, "%d",gdate->tm_year+1900);
    strcat(da, tem);
    sprintf(tem, "%02d",gdate->tm_mon+1);
    strcat(da, tem);
    sprintf(tem, "%02d",gdate->tm_mday);
    strcat(da, tem);
    sprintf(tem, "%02d",gdate->tm_hour);
    strcat(da, tem);
    sprintf(tem, "%02d",gdate->tm_min);
    strcat(da, tem);
    sprintf(tem, "%02d",gdate->tm_sec);
    strcat(da, tem);
    sprintf(tem, "%s","Z00'00\0");
    strcat(da, tem);
    free(tem);
    return da;
}
static
pdf_obj *update_object(pdf_document *doc,pdf_obj*obj)
{
    int i =0;
    pdf_obj *new_obj=NULL;
    if(pdf_is_dict(obj))
    {
        int n = pdf_dict_len(obj);
        new_obj = pdf_new_dict(doc, n);
        for (i = 0; i < n; i++)
            pdf_dict_put(new_obj, pdf_dict_get_key(obj, i), pdf_dict_get_val(obj, i));
    }
    if(pdf_is_bool(obj))
    {
        new_obj= pdf_new_bool(doc, pdf_to_bool(obj));
    }
    if(pdf_is_int(obj))
    {
        new_obj= pdf_new_int(doc, pdf_to_int(obj));
    }
    if(pdf_is_real(obj))
    {
        new_obj= pdf_new_dict(doc, pdf_to_real(obj));
    }
    if(pdf_is_indirect(obj))
    {
        new_obj= pdf_new_indirect(doc, pdf_to_num(obj),0);
    }
    if(pdf_is_string(obj))
    {
        new_obj= pdf_new_string(doc, pdf_to_str_buf(obj),pdf_to_str_len(obj));
    }
    if(pdf_is_array(obj))
    {
        new_obj = pdf_new_array(doc, pdf_array_len(obj));
        int n = pdf_array_len(obj);
        for (i = 0; i < n; i++)
            pdf_array_push(new_obj, pdf_array_get(obj, i));
    }
    if(pdf_is_name(obj))
    {
        new_obj = pdf_new_name(doc, pdf_to_name(obj));
    }
    if(pdf_is_null(obj))
    {
        new_obj = pdf_new_null(doc);
    }
    return new_obj;
}
/*
 update the number of reference
 */
static
void pdf_update_reference(pdf_document *doc,pdf_obj *obj,int len)
{
    
    int j =0;
    for(j= 0 ;j< obj->u.d.len;j ++)
    {
        pdf_obj *value = obj->u.d.items[j].v;
        pdf_obj *v = NULL;
        if(pdf_is_indirect(value))//间接引用
        {
            if(!strcmp("Length", pdf_to_name(obj->u.d.items[j].k)))//处理长度是间接引用的问题，后面readstream会要用到
            {
                pdf_dict_put(obj, obj->u.d.items[j].k,pdf_new_int(doc,pdf_to_int(obj->u.d.items[j].v)));
            }
            else{
                v = pdf_new_indirect(doc, pdf_to_num(value)+len-1, 0);
                pdf_dict_put(obj, obj->u.d.items[j].k, v);
            }
        }
        else if(pdf_is_array(value))//处理数组里的间接引用
        {
            int k =0;
            for(k =0 ;k< value->u.a.len;k++)
            {
                if(pdf_is_indirect(value->u.a.items[k]))
                {
                    v = pdf_new_indirect(doc, pdf_to_num(value->u.a.items[k])+len-1, 0);
                    pdf_array_delete(value, k);
                    pdf_array_insert(value, v, k);
                }
            }
        }
        else if(pdf_is_dict(value))//处理dictionary
        {
            pdf_update_reference(doc,value,len);
        }
        if(v)
            pdf_drop_obj(v);
    }
    
    
}
/*
 *  merge_file
 *merge two file
 *parameter doc:the destination document
 *parameter oldfile :the old file name
 *parameter new_doc :the source document
 *
 */
static
pdf_document *merge_file(pdf_document *doc,pdf_document *new_doc)
{
    int l =0 ;
    if(NULL != new_doc->crypt)//因为用adobe加密的文档可能是一个非线性化的文档，为了线性化必须操作就要先移除密码
    {
        new_doc = pdf_remove_password(new_doc, tempFile1);
        l = 1;
    }
    else
        new_doc = linearize_file(new_doc);//线性化被合并的文档
    
    int i =0;
    int dct =0;
    int page_count = pdf_count_pages(new_doc);
    int count = pdf_count_objects(new_doc);
    int len  = pdf_count_objects(doc);
    
    pdf_obj *tpages =  pdf_dict_gets(pdf_dict_gets(pdf_trailer(new_doc), "Root"),"Pages");
    pdf_obj *root = pdf_dict_gets(pdf_trailer(doc), "Root");
    pdf_obj *pages = pdf_dict_gets(root,"Pages");
    pdf_obj *newObj=NULL;
    
    if(pdf_is_array(pages))
    {
        pdf_array_push_drop(pages, pdf_new_indirect(doc,pdf_to_num(tpages)+len-1, 0));
    }
    else
    {
        pdf_obj *pages_array = pdf_new_array(doc, 2);
        pdf_array_push(pages_array, pages);
        pdf_array_push_drop(pages_array,  pdf_new_indirect(doc,pdf_to_num(tpages)+len-1, 0));
        pdf_dict_puts_drop(root, "Pages", pages_array);
    }
    
    /*
     step 1: merge object 
     */
    for(i = 1;i<count;i++)//start from 1
    {
        pdf_obj *new_obj = NULL;
        fz_try(doc->ctx)
        {
            fz_try(doc->ctx)
            {
                new_obj = pdf_load_object(new_doc, i, 0);
            }
            fz_catch(doc->ctx)
            {
                continue;
            }
            
            /*
             step 1.1 update the reference that in the tdoc
             */
            if(pdf_is_dict(new_obj))
                pdf_update_reference(doc,new_obj,len);
            
            if(pdf_is_array(new_obj))
            {
                int k =0;
                for(k =0 ;k< new_obj->u.a.len;k++)
                {
                    if(pdf_is_indirect(new_obj->u.a.items[k]))
                    {
                        pdf_obj *v = pdf_new_indirect(doc, pdf_to_num(new_obj->u.a.items[k])+len-1, 0);
                        pdf_array_delete(new_obj, k);
                        pdf_array_insert_drop(new_obj, v, k);
                    }
                }
            }
              unlink(File);
            /*
             step 1.2 update objects and stream to doc
             */
            int num = pdf_create_object(doc);
            
            if(pdf_is_stream(new_doc, i, 0))
            {
                fz_stream *stream = pdf_open_stream(new_doc,i, 0);
                fz_buffer *buf = fz_read_all(stream, 1024);
                
                if(NULL != buf->data)
                {
                    if(strcmp("DCTDecode",pdf_to_name( pdf_dict_gets(new_obj,"Filter"))))
                    {
                        printf("");//这一句必须要，否则数据就没有，原因还不知道
                        compressStream(buf, doc->ctx);
                    }
                    else
                    {
                        compress_with_dct(buf, doc->ctx, pdf_to_int(pdf_dict_gets(new_obj, "Width")),pdf_to_int(pdf_dict_gets(new_obj, "Height")));
                        dct = 1;
                    }
                }
                if(NULL != doc->crypt)//如果原文件有加密，则对加入的文件也要加密（注意对象号要修改为新的对象号）
                {
                    fz_stream *stream1 =  pdf_open_crypt(doc->file, doc->crypt, i+len-1, 0);
                    fz_arc4_encrypt(&((fz_arc4c *)(stream1->state))->arc4, buf->data, buf->data,buf->len);
                    pdf_crypt_obj(doc->ctx, doc->crypt, new_obj, i+len-1, 0);
                }
                
                pdf_dict_puts_drop(new_obj,"Length", pdf_new_int(doc,buf->len));
                if(!dct)
                {
                    pdf_dict_puts_drop(new_obj,"Filter", pdf_new_name(doc,"FlateDecode"));
                }
                if(NULL != pdf_dict_gets(new_obj, "DecodeParms"))
                {
                    pdf_dict_dels(new_obj, "DecodeParms");
                }
                newObj = update_object(doc, new_obj);
                pdf_update_object(doc, num, newObj);
                pdf_update_stream(doc,num,buf);
                
                fz_drop_buffer(doc->ctx, buf);
                fz_close(stream);
            }
            else
            {
                if(NULL != doc->crypt)
                {
                    pdf_crypt_obj(doc->ctx, doc->crypt, new_obj, i+len-1, 0);
                }
                newObj = update_object(doc, new_obj);
                pdf_update_object(doc, num, newObj);
            }
            pdf_drop_obj(newObj);
        }
        fz_catch(doc->ctx)
        {
            fz_throw(doc->ctx, 1, "merge object failed! check whether the file has encrypt!");
        }
        
    }
    /*
     step 2:merge page
     */
    for(i = 0 ;i< page_count;i++)
    {
        pdf_page *page = pdf_load_page(new_doc, i);//
        pdf_insert_page(doc, page, doc->page_len);
    }
    
    //remove(TEMP_FILE);
    //pdf_close_document(new_doc);//如果close，会概率性的挂
//    unlink(File);
    free(File);
    //    if(l)
    //        remove("tem_merge_mov.pdf");
    return doc;
}

/*
 *  pdf_merge_file
 *merge  files
 *parameter filename:the file that save the merged files
 *parameter count:the file count
 *parameter ...:pdf_document *
 */
pdf_document *pdf_merge_file(char *filename,int count,...)
{
    pdf_document *tmdoc = NULL;
    pdf_document *doc = NULL;
    va_list doc_list;
    int i =0;
    tempFile1 = filename;
    va_start(doc_list, count);
    doc = va_arg(doc_list, pdf_document*);//第一个文档，作为主文档
    //if(NULL != pdf_dict_gets(pdf_trailer(doc),"Filter"))//xref table has compressed
    if(NULL != doc->crypt)//主文档加密了的移除密码
    {
        doc = pdf_remove_password(doc, filename);//如果加密文件的xref压缩了，如果不线性化追加就会失败，所以进行密码移除。
        pdf_count_pages(doc);
    }
    else//主文档线性化
    {
        doc = linearize(doc,filename);
        pdf_count_pages(doc);//加载page_tree
    }
    fz_try(doc->ctx)
    {
        for(i=0;i<count-1;i++)
        {
            
            tmdoc = va_arg(doc_list, pdf_document*);
            if(tmdoc)
            {
                doc = merge_file(doc, tmdoc);
            }
        }
    }
    fz_always(doc->ctx)
    {
        va_end(doc_list);
    }
    fz_catch(doc->ctx)
    {
        fz_throw(doc->ctx, 1, "ensure the arguments is a pointer that point to pdf_document!");
    }
    pdf_obj *info = pdf_dict_gets(pdf_trailer(doc), "Info");
    char *date = pdf_get_date();
    pdf_obj *sdate = pdf_new_string(doc, date, strlen(date));
    pdf_dict_puts(info, "CreationDate", sdate);
    pdf_dict_puts_drop(info, "ModDate", sdate);
    free(date);
    return doc;
}


pdf_document *pdf_set_author(pdf_document *doc,char *author)
{
    pdf_obj *info = pdf_dict_gets(pdf_trailer(doc), "Info");
    pdf_obj *sauthor = pdf_new_string(doc, author, strlen(author));
    pdf_dict_puts_drop(info, "Author", sauthor);
    return doc;
}
pdf_document *pdf_set_title(pdf_document *doc,char *title)
{
    pdf_obj *info = pdf_dict_gets(pdf_trailer(doc), "Info");
    pdf_obj *stitle = pdf_new_string(doc, title, strlen(title));
    pdf_dict_puts_drop(info, "Title", stitle);
    return doc;
}
int pdf_save_merged_file(pdf_document*doc,char *filename)
{
    fz_try(doc->ctx)
    {
        save(doc, filename);
    }
    fz_catch(doc->ctx)
    {
        return 1;
    }
    return 0;
}


static
void pdf_load_objects(pdf_document *doc,pdf_obj *obj)
{
    int i =0;
    if(!pdf_is_dict(obj))
        return ;
    for(i =0 ;i<obj->u.d.len;i++)
    {
        pdf_obj *value = obj->u.d.items[i].v;
        if(pdf_is_indirect(value))//处理引用
        {
            pdf_obj *re =NULL;
            int num = pdf_create_object(doc);
            pdf_obj *new = pdf_load_object(obj->doc, pdf_to_num(value), 0);
            if(strcmp("Parent",pdf_to_name( obj->u.d.items[i].k)))//has not bussiness with parent
                pdf_load_objects(doc,new);
            int dct =0;
            
            if(pdf_is_stream(obj->doc,pdf_to_num(value), 0))
            {
                fz_stream *stream = pdf_open_stream(obj->doc,pdf_to_num(value), 0);
                fz_buffer *buf = fz_read_all(stream, 1024);
                //  printf("%s\n",buf->data);
                if(NULL != buf->data)
                {
                    if(strcmp("DCTDecode",pdf_to_name( pdf_dict_gets(new,"Filter"))))
                    {
                        //      printf("");//这一句必须要，否则数据就没有，原因还不知道
                        compressStream(buf, doc->ctx);
                    }
                    else
                    {
                        compress_with_dct(buf, doc->ctx, pdf_to_int(pdf_dict_gets(new, "Width")),pdf_to_int(pdf_dict_gets(new, "Height")));
                        dct = 1;
                    }
                }                
                pdf_dict_puts_drop(new,"Length", pdf_new_int(doc,buf->len));
                if(!dct)
                {
                    pdf_dict_puts_drop(new,"Filter", pdf_new_name(doc,"FlateDecode"));
                }
                if(NULL != pdf_dict_gets(new, "DecodeParms"))
                {
                    pdf_dict_dels(new, "DecodeParms");
                }
                pdf_update_stream(doc,num,buf);
                fz_drop_buffer(doc->ctx, buf);
                fz_close(stream);
            }
            new = update_object(doc, new);
            pdf_update_object(doc, num, new);
            re = pdf_new_indirect(doc, num, 0);
            pdf_dict_put(obj,obj->u.d.items[i].k,re);
            pdf_drop_obj(re);
            
        }
        else if(pdf_is_array(value))//处理数组
        {
            int i =0 ;
            for(i =0 ;i<pdf_array_len(value)&&pdf_is_indirect(pdf_array_get(value, i));i++)
            {
                pdf_obj *re=NULL;
                int num = pdf_create_object(doc);
                pdf_obj * new = pdf_load_object(obj->doc, pdf_to_num(pdf_array_get(value, i)), 0 );
                if(strcmp("Parent",pdf_to_name( obj->u.d.items[i].k)))
                    pdf_load_objects(doc,new);
                int dct =0;
                if(pdf_is_stream(obj->doc,pdf_to_num(pdf_array_get(value, i)), 0))
                {
                    fz_stream *stream = pdf_open_stream(obj->doc,pdf_to_num(pdf_array_get(value, i)), 0);
                    fz_buffer *buf = fz_read_all(stream, 1024);
                    
                    if(NULL != buf->data)
                    {
                        if(strcmp("DCTDecode",pdf_to_name( pdf_dict_gets(new,"Filter"))))
                        {
                            printf("");//这一句必须要，否则数据就没有，原因还不知道
                            compressStream(buf, doc->ctx);
                        }
                        else
                        {
                            compress_with_dct(buf, doc->ctx, pdf_to_int(pdf_dict_gets(new, "Width")),pdf_to_int(pdf_dict_gets(new, "Height")));
                            dct = 1;
                        }
                    }
                    pdf_dict_puts_drop(new,"Length", pdf_new_int(doc,buf->len));
                    if(!dct)
                    {
                        pdf_dict_puts_drop(new,"Filter", pdf_new_name(doc,"FlateDecode"));
                    }
                    if(NULL != pdf_dict_gets(new, "DecodeParms"))
                    {
                        pdf_dict_dels(new, "DecodeParms");
                    }
                    pdf_update_stream(doc,num,buf);
                    fz_drop_buffer(doc->ctx, buf);
                    fz_close(stream);
                }
                new = update_object(doc, new);
                pdf_update_object(doc, num, new);
                re = pdf_new_indirect(doc, num, 0);
                pdf_array_delete(value, i);
                pdf_array_insert_drop(value, re, i);
            }
        }
        
        else if(pdf_is_dict(value))//处理dictionary
            pdf_load_objects(doc, value);
    }
}

static
int insert_page(pdf_obj *pages,pdf_page *new_page,int pos)
{
    if(pos<0||pos>pages->doc->page_len)
        return  1;
    pdf_obj *pageso = pdf_load_object(pages->doc, pdf_to_num(pages), 0);
    pdf_obj *kid = pdf_dict_gets(pageso, "Kids");
    int len=0,i=0;
    int count = pdf_to_int(pdf_dict_gets(pageso, "Count"));
    if(pdf_array_len(kid) == count &&pos<=count)//leaf node
    {
        pdf_array_insert(kid, pdf_new_ref(kid->doc, new_page->me), pos);
        pdf_dict_putp(new_page->me, "Parent",  pages);
        pdf_dict_putp_drop(pageso, "Count", pdf_new_int(pages->doc,++count));
        return 0;
    }
    else//page tree node
    {
        for(i=0;i<pdf_array_len(kid);i++)
        {
            pdf_obj *tree_node = pdf_load_object(kid->doc,pdf_to_num(pdf_array_get(kid, i)), 0);
            len=pdf_to_int(pdf_dict_gets(tree_node, "Count"));
            if(pos>len)
            {
                pos-=len;
            }
            else{
                if(! insert_page(pdf_array_get(kid, i), new_page, pos))
                {
                    pdf_dict_putp_drop(pageso, "Count", pdf_new_int(pages->doc,++count));
                    return 0;
                }
                
            }
        }
        return 1;
    }
}
/*
 * merge a page from new_doc  to doc
 *parameter doc:the destination document
 *parameter new_doc:the target document
 *parameter pos:the position to insert to the destination document
 *parameter page:the page that will merge to doc
 */
pdf_document *pdf_merge_page(pdf_document *doc,pdf_document *new_doc,int pos,int page,char *savefile)
{
    /*
     method 1:
     */
    int count = pdf_count_pages(doc);
    pdf_page *new_page = pdf_load_page(new_doc, page);
    if(pos<0)
        pos = 0;
    if(pos>=count)
        pos = count;
    //    if(NULL != doc->crypt)
    //    {
    //        doc = pdf_remove_password(doc, savefile);//如果加密文件的xref压缩了，如果不线性化追加就会失败，所以进行密码移除。
    //        pdf_count_pages(doc);
    //    }
    //    else//主文档线性化
    //    {
    //        doc = linearize(doc,savefile);
    //        pdf_count_pages(doc);//加载page_tree
    //    }
    /*
     step 1:load  objects that relate to the page
     */
    pdf_load_objects(doc, new_page->me);
    
    /*
     step 2:insert the page to document
     */
    pdf_obj *root = pdf_dict_gets(pdf_trailer(doc), "Root");
    pdf_obj *pages = pdf_dict_gets(root,"Pages");
    insert_page(pages, new_page, pos);
    return doc;
    
    /*
     method 2:firstly,extract page from the file ,then merge the files
     */
    //    pdf_extract_pages(new_doc, page, page, "te.pdf");
    //    pdf_document *new = pdf_open_document(doc->ctx, "te.pdf");
    //    doc = pdf_merge_file(savefile, 2,doc,new);
    //    pdf_save_merged_file(doc, savefile);
    //    remove("te.pdf");
    //    return doc;
    
}



/*
 *extract page
 */
static
void export_page_range(pdf_document *doc, int start, int end)
{
    int i;
	if (!doc || start >= doc->page_len || end < 0)
		return;
    //删掉不在start-end之间（不包括start，end）的页
    for (i = 0; i < start; i++)
    {
        //        pdf_drop_obj(doc->page_objs[i]);
        //        pdf_drop_obj(doc->page_refs[i]);
        pdf_delete_object(doc, pdf_to_int(doc->page_refs[i]));
        
    }
    for (i = end + 1; i < doc->page_len; ++i) {
        //        pdf_drop_obj(doc->page_objs[i]);
        //        pdf_drop_obj(doc->page_refs[i]);
        pdf_delete_object(doc, pdf_to_int(doc->page_refs[i]));
    }
    
    if (doc->page_len > end)
    {
        memmove(&doc->page_objs[0], &doc->page_objs[start], sizeof(pdf_page *) * (end - start + 1));
        memmove(&doc->page_refs[0], &doc->page_refs[start], sizeof(pdf_page *) * (end - start + 1));
    }
    
	doc->page_len = end - start + 1;
	doc->needs_page_tree_rebuild = 1;
}

void pdf_extract_pages(pdf_document *doc,int start_page,int end_page,char *savefile)
{
    fz_write_options ops;
    int erro = 0 ;
    int count = pdf_count_pages(doc);
    if (start_page > end_page)
    {
        start_page = start_page + end_page;
        end_page = start_page - end_page;
        start_page = start_page-end_page;
        
    }
    if (start_page < 0)
    {
        start_page = 0;
    }
    if(end_page >count)
    {
        end_page = count;
    }
    doc = linearize_file(doc);
    pdf_count_pages(doc);
    export_page_range(doc, start_page, end_page);
    
    ops.do_incremental = 0;
    ops.continue_on_error = 1;
    ops.do_linear = 0;
    ops.do_garbage = 1;
    ops.do_expand = 0 ;
    ops.do_ascii = 0 ;
    ops.errors = &erro;
    pdf_write_document(doc, savefile, &ops);
    
}

