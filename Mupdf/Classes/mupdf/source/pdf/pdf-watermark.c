//
//  pdf-watermark.c
//  MuPDF
//
//  Created by kdanmobile on 13-8-5.
//  Copyright (c) 2013年 Artifex Software, Inc. All rights reserved.
//
/*
 暂时添加watermark到annot.h的头文件
 */

#include "mupdf/pdf.h"
#include <stdarg.h>
#include "time.h"
#include "watermark.h"

/* 默认文字水印的docset字符串 */
#define DEFAUL_TEXT_DOCSET "<?xml version = \"1.0\" encoding = \"UTF-8\" ?>\
<WatermarkSettings version = \"8.0\"><Font type=\"Type0\" size=\"24.0\" name=\"Hei\"/>\
中文chinese\
<Scale value=\"1.0\"/><Rotation value=\"45.0\"/><Opacity value=\"0.5\"/><Location ontop=\"1.0\"/>\
<Color r=\"0.0\" b=\"0.0\" g=\"0.0\"/>\
<Alignment vertalign=\"1.0\" horizalign=\"1.0\" vertvalue= \"0.0\" horizvalue=\"0.0\" unit=\"0.0\"/>\
<Appearance onscreen=\"1.0\" onprint=\"1.0\" fixedprint=\"0.0\"/>\
<PageRange start=\"-1\" end=\"-1\" even=\"1\" odd=\"1\"/>\
</WatermarkSettings>"

#define DEFAUL_IMG_DOCSET "<?xml version = \"1.0\" encoding = \"UTF-8\" ?>\
<WatermarkSettings version = \"8.0\"><SourceFile type=\"\" name=\"CurrentImage\"/>\
<Scale value=\"0.5\"/><Rotation value=\"0\"/><Opacity value=\"0.5\"/><Location ontop=\"0\"/>\
<Color r=\"1\" b=\"0\" g=\"1\"/>\
<Alignment vertalign=\"1\" horizalign=\"1\" vertvalue= \"0.0\" horizvalue=\"0.0\" unit=\"3\"/>\
<Appearance onscreen=\"1\" onprint=\"1\" fixedprint=\"0\"/><PageRange start=\"-1\" end=\"-1\" even=\"1\" odd=\"1\"/>\
</WatermarkSettings>"

enum
{
    PDF_WATERMARK_TYPE_UNKWON = -1,
    PDF_WATERMARK_TYPE_TEXT,
    PDF_WATERMARK_TYPE_IMG
};
/* pdf_watermark_s: 存储水印的结构体
 * 成员：
 *   doc        PDF文档
 *   fz_font    字体
 *   ExtGState  gs 控制内容的显示
 *   XObject    xobject 要显示的内容
 *   content    内容 控制旋转等，执行gs和fm
 *   rect       水印的位置 */
struct pdf_watermark_s
{
    int type;
    pdf_document *doc;
    
    char text[1024];
    char font_name[32];
    int font_size;
    
    char color_space[8];
    float color[8];
    
    float rotation;
    float opacity;
    fz_rect bbox;
    
    char *LastModified;
    
    pdf_obj *ExtGState;
    pdf_obj *XObject;
    pdf_obj *content;
    pdf_obj *docset;
    
    pdf_watermark *next;
};

/* 存放水印内容流的结构体
 * pageobj   content的页的对象
 * content   content对象
 * gs_name
 * gs        gs
 * xobj_name
 * Do        Do
 * mat       转换矩阵 */
typedef struct watermark_attribute_s watermark_attribute;
struct watermark_attribute_s
{
    pdf_obj *pageobj;
    pdf_obj *content;
    
    char gs_name[64];
    char gs[3];
    char xobj_name[64];
    char Do[3];
    
    fz_matrix mat;
};

/* 水印坐标变换
 * 分别为：平移，旋转，缩放*/
enum
{
    PDF_WATERMARK_TRANSLATE,
    PDF_WATERMARK_ROTATE,
    PDF_WATERMARK_SCALE
};

/* next_stream_skip_string:获取跳过stream后面跳过string以及空格和\n的字符串*/
static char*
next_stream_skip_string(char *stream, char *string)
{
    int i = strlen(string);
    char c;
    
    if (!stream)
        return NULL;
    
    do
    {
        c = *(stream + i++);
    } while (c==' ' || c=='\n');
    
    return stream + i - 1;
}

static char*
stream_after_name(char *name)
{
    char *after;
    
    after = strstr(name, "EMC");
    if (after)
        after = next_stream_skip_string(after, "EMC");
    else
    {
        after = strstr(name, "Q");
        if (after)
            after = next_stream_skip_string(after, "Q");
        else
            after = NULL;
    }
    
    return after;
}

static char*
stream_before_name(char *name)
{
    char *x = name;
    char *before = NULL;
    
    do
    {
        x = strstr(x, "/Artifact");
        if (x)
        {
            before = x;
            x = next_stream_skip_string(x, "/Artifact");
        }
        
    }
    while (x);
    
    return before;
}

/* pdf_watermark_init:
 * 功能：用最后修改时间初始化水印
 * 参数：
 *   watermark      要初始化的水印
 *   lastModified   最后修改时间  */
static void
pdf_watermark_init(pdf_document *doc, pdf_watermark *watermark, char *lastModified)
{
    watermark->doc = doc;
    watermark->LastModified = lastModified;
    watermark->XObject = pdf_new_array(doc, 0);
    watermark->ExtGState = pdf_new_array(doc, 0);
    watermark->content = pdf_new_array(doc, 0);
    watermark->next = NULL;
}

/* pdf_drop_watermark：释放水印
 */
static void
pdf_drop_watermark(pdf_document *doc, pdf_watermark *watermark)
{
    pdf_drop_obj(watermark->ExtGState);
    pdf_drop_obj(watermark->XObject);
    pdf_drop_obj(watermark->content);
    
    //free(watermark);
    fz_free(doc->ctx, watermark);
}

/* watermark_array_push:
 * 功能：将元素加入到数组中,如果已经存在在数组当中，则不添加
 * 参数：
 *   array  数组
 *   item   元素，必须为字典类型 */
static void
watermark_array_push_drop(pdf_obj *array, pdf_obj *item)
{
    int len = pdf_array_len(array);
    int i;
    
    for (i=0; i<len; i++)
    {
        if(!pdf_objcmp(pdf_dict_get_val(item, 0), pdf_dict_get_val( pdf_array_get(array, i), 0)))
            break;
    }
    if (i == len)
        pdf_array_push_drop(array, item);
}

/* pdf_load_watermark_extgstate:
 * 功能：载入文档水印的ExtGState,如果成功,返回添加了extgstate的水印的指针；否则返回空指针
 * 参数：
 *   watermarks     所有水印
 *   pageobj        页的对象  */
static pdf_watermark *
pdf_load_watermark_extgstate(pdf_watermark *watermarks, watermark_attribute *wa)
{
    pdf_watermark *itr = watermarks->next;
    
    if (wa->xobj_name == NULL || wa->gs_name == NULL)
        return NULL;
    
    while (itr)
    {
        int len = pdf_array_len(itr->XObject);
        for (int i=0; i<len; i++)
        {
            pdf_obj *xobj = pdf_array_get(itr->XObject, i);
            char *xobj_name = pdf_to_name(pdf_dict_get_key(xobj, 0));
            
            /* 比较wa->xobj_name 和 watermark->XObject中的xobj_name */
            if (!strcmp(xobj_name, wa->xobj_name))
            {
                /* 如果相等，wa->gs_name 就是水印内容对应的ExtGState,
                 * 将其加入到对应水印的ExtGState成员中 */
                pdf_obj *ExtGState = pdf_dict_getp(wa->pageobj, "Resources/ExtGState");
                pdf_obj *gsobj = pdf_dict_gets(ExtGState, wa->gs_name);
                
                pdf_obj *ExtGState_item = pdf_new_dict(itr->doc, 1);
                pdf_dict_puts(ExtGState_item, wa->gs_name, gsobj);
                
                watermark_array_push_drop(itr->ExtGState, ExtGState_item);
                
                return itr;
            }
        }
        
        itr = itr->next;
    }
    return NULL;
}

/* pdf_load_watermark_content:
 * 功能：载入水印的content，如果成功，返回添加content的水印的指针，否则返回空 */
static pdf_watermark *
pdf_load_watermark_content(pdf_watermark *watermark, watermark_attribute *wa)
{
    if (!watermark)
        return NULL;
    
    int len = pdf_array_len(watermark->content);
    int i;
    
    for (i=0; i<len; i++)
    {
        if(!pdf_objcmp(pdf_array_get(watermark->content, i), wa->content))
            break;
    }
    if (i == len)
        pdf_array_push(watermark->content, wa->content);
    
    return watermark;
}

/* watermark_parse_content_stream:
 * 功能：解析流，读取内容到gs_fm中
 * 参数：
 *   stream     要解析的流
 *   watermark  水印 */
static void
watermark_parse_content_stream(pdf_watermark *watermark, fz_stream *stream, watermark_attribute *wa)
{
    char name[256];
    float num[128];
    int num_i = 0;//记录num数组的下角标
    fz_context *ctx = watermark->doc->ctx;
    
    pdf_lexbuf *buf = fz_malloc(ctx, sizeof(*buf));
    pdf_lexbuf_init(ctx, buf, PDF_LEXBUF_SMALL);
    pdf_token tok = PDF_TOK_ERROR;
    
    fz_try(ctx)
    {
        /* 解析流，找出流当中的 gs_name, gs, xobj_name, Do */
        do
        {
            /* mupdf词法解析函数 */
            tok = pdf_lex(stream, buf);
            
            switch (tok)
            {
                case PDF_TOK_KEYWORD:
                    if (!strcmp(buf->scratch, "q"))
                    {
                    }
                    else if (!strcmp(buf->scratch, "gs"))
                    {
                        strcpy(wa->gs, buf->scratch);
                        strcpy(wa->gs_name, name);
                    }
                    else if (!strcmp(buf->scratch, "Do"))
                    {
                        strcpy(wa->Do, buf->scratch);
                        strcpy(wa->xobj_name, name);
                    }
                    else if (!strcmp(buf->scratch, "Q"))
                    {
                        /* 如果成功载入extgstate，则将这段content加入到水印的content成员中 */
                        pdf_watermark *itr = pdf_load_watermark_extgstate(watermark, wa);
                        if (itr != NULL)
                            pdf_load_watermark_content(itr, wa);
                    }
                    else if (!strcmp(buf->scratch, "cm"))
                    {
                        wa->mat.a = num[0];
                        wa->mat.b = num[1];
                        wa->mat.c = num[2];
                        wa->mat.d = num[3];
                        wa->mat.e = num[4];
                        wa->mat.f = num[5];
                        num_i = 0;
                    }
                    else
                    {
                        num_i = 0;
                    }
                    break;
                    
                case PDF_TOK_NAME:
                    strcpy(name, buf->scratch);
                    break;
                    
                case PDF_TOK_REAL:
                    if (num_i < 128)
                        num[num_i++] = buf->f;
                    break;
                    
                case PDF_TOK_INT:
                    if (num_i < 128)
                        num[num_i++] = buf->i;
                    break;
                    
                default:
                    num_i = 0;
                    break;
            }
        }
        while (tok != PDF_TOK_EOF);
    }
    fz_catch(ctx)
    {
        
    }
    
    pdf_lexbuf_fin(buf);
	fz_free(ctx, buf);
}

/* pdf_parse_watermark_stream
 * 功能：解析页面的内容流，找出水印的部分 */
static void
parse_watermark_content_stream(pdf_document *doc, pdf_obj *content, pdf_obj *pageobj, pdf_watermark *watermarks)
{
    fz_context *ctx = doc->ctx;
    
    fz_stream *content_stream = pdf_open_contents_stream(doc, content);
    
    /* 创建一个水印内容流属性的结构体 */
    watermark_attribute *wa = fz_malloc_struct(ctx, watermark_attribute);
    wa->pageobj = pageobj;
    wa->content = content;
    
    fz_try(ctx)
    {
        /* 解析流的时候会给结构体赋值 */
        watermark_parse_content_stream(watermarks, content_stream, wa);
    }
    fz_always(ctx)
    {
        fz_close(content_stream);
        fz_free(ctx, wa);
    }
    fz_catch(ctx)
    {
        fz_rethrow(ctx);
    }
}

static void
pdf_load_watermark_bbox(pdf_document *doc, pdf_watermark *watermarks)
{
    fz_context *ctx = doc->ctx;
    pdf_watermark *itr = watermarks->next;
    
    while (itr)
    {
        /* 取第一个xobj和content */
        pdf_obj *w_xobj = itr->XObject;
        pdf_obj *w_cont = itr->content;
        pdf_obj *xobj = pdf_dict_get_val(pdf_array_get(w_xobj, 0), 0);
        pdf_obj *cont = pdf_array_get(w_cont, 0);
        
        /* 获取转换矩阵 */
        watermark_attribute *wa = fz_malloc_struct(ctx, watermark_attribute);
        fz_stream *content_stream = pdf_open_contents_stream(doc, cont);
        
        fz_try(ctx)
        {
            /* 解析流的时候会给结构体赋值 */
            watermark_parse_content_stream(itr, content_stream, wa);
        }
        fz_always(ctx)
        {
            fz_close(content_stream);
        }
        fz_catch(ctx)
        {
            fz_rethrow(ctx);
        }
        
        /* 根据转换矩阵和bbox计算真正的bbox */
        fz_rect bbox;
        pdf_obj *bbox_array = pdf_dict_gets(xobj, "BBox");
        pdf_to_rect(ctx, bbox_array, &bbox);
        
        bbox.x0 = wa->mat.a * bbox.x0 + wa->mat.c * bbox.y0 + wa->mat.e;
        bbox.x1 = wa->mat.a * bbox.x1 + wa->mat.c * bbox.y1 + wa->mat.e;
        bbox.y0 = wa->mat.b * bbox.x0 + wa->mat.d * bbox.y0 + wa->mat.f;
        bbox.y1 = wa->mat.b * bbox.x1 + wa->mat.d * bbox.y1 + wa->mat.f;
        itr->bbox = bbox;
        
        /* 根据矩阵求旋转角度 */
        double angel =  atan(wa->mat.b/wa->mat.a)*180/M_PI;
        itr->rotation = angel;
        
        /* 获取横向和竖向的对齐方式 */
        pdf_obj *docset = pdf_dict_getp(xobj, "PieceInfo/ADBE_CompoundType/DocSettings");
        fz_buffer *buff = pdf_load_stream(doc, pdf_to_num(docset), pdf_to_gen(docset));
        char *stream = (char *)buff->data;
        stream[buff->len] = '\0';
        
        strstr(stream, "");
        
        itr = itr->next;
    }
    
}
/* pdf_load_watermarks：
 * 功能：载入文档中的水印 */
pdf_watermark *
pdf_load_watermarks(pdf_document *doc)
{
    pdf_watermark *watermark, *head, *end, *itr;
    char *LastModified;
    int page_count;
    int isSame = 0;
    
    fz_context *ctx = doc->ctx;
    
    /* 创建一个空的watermark头 */
    head = fz_malloc_struct(ctx, pdf_watermark);
    pdf_watermark_init(doc, head, "");
    itr = end = head;
    
    /* FIXME: try later? find a better algorithm */
    /* 读取每一页 */
    page_count = pdf_count_pages(doc);
    for (int i=0; i<page_count; i++)
    {
        pdf_obj *pageobj = doc->page_objs[i];
        
        pdf_obj *xdict = pdf_dict_getp(pageobj, "Resources/XObject");
        
        if(!xdict && !pdf_is_dict(xdict))
            continue;
        
        /* 读取页的每一个XObject，载入是水印的XObject */
        int xobj_len = pdf_dict_len(xdict);
        for (int j=0; j<xobj_len; j++)
        {
            pdf_obj *xobj = pdf_dict_get_val(xdict, j);
            char *xobj_name = pdf_to_name(pdf_dict_get_key(xdict, j));
            
            /* 如果相等，则这个xobject为水印，否则不是 */
            if (strcmp(pdf_to_name(pdf_dict_getp(xobj, "PieceInfo/ADBE_CompoundType/Private")), "Watermark"))
                continue;
            
            pdf_obj *xobject_item = pdf_new_dict(doc, 1);
            pdf_dict_puts(xobject_item, xobj_name, xobj);
            
            /* 获取最后创建时间 */
            LastModified = pdf_to_str_buf(pdf_dict_getp(xobj, "PieceInfo/ADBE_CompoundType/LastModified"));
            
            itr = head;
            isSame = 0;
            if (!strcmp(itr->LastModified, LastModified))
            {
                isSame = 0;
            }
            else
            {
                itr = itr->next;
                while (itr)
                {
                    /* 判断最后修改日期，如果相等，则认为是同一个水印 */
                    if (!strcmp(itr->LastModified, LastModified))
                    {
                        isSame = 1;
                        break;
                    }
                    itr = itr->next;
                }
            }
            
            /* 如果是同一个水印，则将xobject_item加入到 itr->xobject中。否则，创建一个新的水印 */
            if (isSame)
            {
                watermark_array_push_drop(itr->XObject, xobject_item);
            }
            else
            {
                /* 创建水印结构体 */
                watermark = fz_malloc_struct(ctx, pdf_watermark);
                pdf_watermark_init(doc, watermark, LastModified);
                pdf_array_push_drop(watermark->XObject, xobject_item);
                end->next = watermark;
                end = watermark;
                
            }
            
        }
        
        if (!head->next)
            continue;
        
        /* 载入ExtGState和content */
        pdf_obj *contents = pdf_dict_gets(pageobj, "Contents");
        if (pdf_is_array(contents))
        {
            int contents_len = pdf_array_len(contents);
            for (int j=0; j<contents_len; j++)
            {
                pdf_obj *contents_item = pdf_array_get(contents, j);
                parse_watermark_content_stream(doc, contents_item, pageobj, head);
            }
        }
        else
        {
            parse_watermark_content_stream(doc, contents, pageobj, head);
        }
        
    }
    
    if (head->next)
    {
        doc->watermarks = head;
        return head;
    }
    else
    {
        pdf_drop_watermark(doc, head);
        doc->watermarks = NULL;
        return NULL;
    }
}

/* pdf_free_watermark
 * 功能：释放pdf_watermark结构体 */
void
pdf_free_watermark(pdf_document *doc, pdf_watermark *watermark)
{
    pdf_watermark *next;
    
    while (watermark)
    {
        next = watermark->next;
        pdf_drop_watermark(doc, watermark);
        watermark = next;
    }
}

/* pdf_create_gs_key: 创建一个新的 /GS key
 * 这个字典的 key名字是 Kdan_GS0,Kdan_GS1,...,Kdan_GS(n) 递增的.
 * 找到最大的Kdan_GS(n)，并新建一个Kdan_GS(n+1)
 * 参数：
 *   pageobj 页的对象
 *   new_gskey 创建的gskey,由外部传入 */
static void
pdf_create_gs_name(pdf_obj *pageobj, char *new_gsname)
{
    strcat(new_gsname, "Kdan_GS");
    
    char *old_gsname = NULL;
    int gsname_num = 0;
    /* 这个数组定义为5,最大支持4位整数 */
    char gsname_num_str[5];
    
    pdf_obj *gsobj = pdf_dict_getp(pageobj, "Resources/ExtGState");
    if(gsobj)
    {
        /* FIXME:Try Later */
        for (int j=0; j<pdf_dict_len(gsobj); j++)
        {
            old_gsname = pdf_to_name(pdf_dict_get_key(gsobj, j));
            if (memcmp("Kdan_GS", old_gsname, 7))
                continue;
            gsname_num++;
        }
    }
    
    sprintf(gsname_num_str, "%d",gsname_num);
    strcat(new_gsname, gsname_num_str);
}

/* pdf_create_fm_key：和pdf_create_gs_key类似 */
static void
pdf_create_xobj_name(pdf_obj *pageobj, char *new_xname)
{
    strcat(new_xname, "Kdan_Fm");
    
    char *old_xname = NULL;
    int xname_num = 0;
    char xname_num_str[5];
    
    pdf_obj *xobj = pdf_dict_getp(pageobj, "Resources/XObject");
    if (xobj)
    {
        for (int j=0; j<pdf_dict_len(xobj); j++)
        {
            old_xname = pdf_to_name(pdf_dict_get_key(xobj, j));
            if (memcmp("Kdan_Fm", old_xname, 7))
                continue;
            xname_num++;
        }
    }
    
    sprintf(xname_num_str, "%d", xname_num);
    strcat(new_xname, xname_num_str);
}

/* matrix_to_str_buf：将矩阵变成字符串 */
static void
matrix_to_str_buf(fz_matrix mat, char *str)
{
    char point[14] = {};
    
    sprintf(point, "%f",mat.a);
    strcat(str, point);
    strcat(str," ");
    sprintf(point, "%f",mat.b);
    strcat(str, point);
    strcat(str," ");
    sprintf(point, "%f",mat.c);
    strcat(str, point);
    strcat(str," ");
    sprintf(point, "%f",mat.d);
    strcat(str, point);
    strcat(str," ");
    sprintf(point, "%f",mat.e);
    strcat(str, point);
    strcat(str," ");
    sprintf(point, "%f",mat.f);
    strcat(str, point);
    strcat(str," cm\n");
}

/* color_rgb_to_str_buf: 将rgb的颜色转换成字符串 */
static void
color_rgb_to_str_buf(float rgb[], char *rgb_str)
{
    char float_str[9];
    for (int i=0; i<3; i++)
    {
        if (rgb[i] < 0)
            rgb[i] = 0;
        if (rgb[i] > 1)
            rgb[i] = 1;
        
        sprintf(float_str, "%f", rgb[i]);
        strcat(rgb_str, float_str);
        strcat(rgb_str, " ");
    }
    strcat(rgb_str, "rg\n");
}

/* watermark_create_content_stream:创建水印的内容流
 * 参数：
 *   pageobj 页的对象
 *   mat     内容流的变换矩阵 */
static char *
pdf_update_content_stream(fz_matrix mat, char *gs_name, char *xobj_name)
{
    /* 特别指出：这个地方分配内存之后，会赋值给fz_buffer->data
     * 调用 fz_drop_buffer 会使引用计数—1，当引用计数为0时，释放内存。
     * 在其他地方使用这个函数时需要注意，还需自己写内存的释放 */
    char *stream = malloc(sizeof(char) *256);
    char *head = "/Artifact <</Subtype /Watermark /Type /Pagination >>BDC\nq\n";
    strcpy(stream, head);
    
    char gsstate[17] = "/";
    strcat(gsstate, gs_name);
    strcat(gsstate, " gs\n");
    
    char fmstate[17] = "/";
    strcat(fmstate, xobj_name);
    strcat(fmstate, " Do\n");
    
    /* 将mat转换为char */
    char cm[88] = {};
    matrix_to_str_buf(mat, cm);
    
    strcat(stream, cm);
    strcat(stream, gsstate);
    strcat(stream, fmstate);
    strcat(stream, "Q\nEMC\n");
   // printf("\n%s\n",stream);
    return stream;
}

/* watermark_create_xobject_stream：创建水印的xobject中的流
 * 参数
 *   text       文字
 *   font_name  字体
 *   font_size  字体大小
 *   rgb[]      字体颜色
 *   xy[]       文字的起始位置 */
static char *
pdf_update_xobject_stream(char *text, char *font_name, int font_size, float rgb[], float xy[])
{
    char *stream = malloc(sizeof(char) * (256 + sizeof(text)));
    char point[14] = {};
    char *head = "0 g 0 G 0 i 0 J []0 d 0 j 1 w 10 M 0 Tc 0 Tw 100 Tz 0 TL 0 Tr 0 Ts \nBT\n";
//    char *head = "BT\n";
    strcpy(stream, head);
    
    /* 字体名字 大小 */
    char font_str[40] = {};
    sprintf(point, "%d", font_size);
    strcat(font_str, "/");
    strcat(font_str, font_name);
    strcat(font_str, " ");
    strcat(font_str, point);
    strcat(font_str, " Tf\n");
    
    /* 字体颜色 */
    char rgb_str[35] = {};
    color_rgb_to_str_buf(rgb,rgb_str);
    
    /* 文字起始位置 */
    char xy_str[32] = {};
    for (int i=0; i<2; i++)
    {
        sprintf(point, "%.1f", xy[i]);
        strcat(xy_str, point);
        strcat(xy_str, " ");
    }
    strcat(xy_str, "Td\n");
    
    strcat(stream, font_str);
    strcat(stream, rgb_str);
    strcat(stream, xy_str);
    strcat(stream, "(");
    
//    int n = strlen(stream);
//    memcpy(stream + n, text, strlen(text));
//    stream[n + strlen(text)] = '\0';
    strcat(stream, text);
    
    strcat(stream, ")'\nET\n");
    printf("%s",stream);
    return stream;
}

/* pdf_update_watermark_extgstate: 创建ExtGState对象,用于控制内容的透明度等等
 * 参数
 *   doc        文档
 *   opacity    不透明度 */
pdf_obj *
pdf_create_extgstate(pdf_document *doc, float opacity)
{
    int idict_num;
    pdf_obj *ExtGState = NULL;
    pdf_obj *obj = NULL;
    pdf_obj *dict = NULL;
    
    fz_context *ctx = doc->ctx;
    
    fz_var(ExtGState);
    fz_var(dict);
    fz_var(obj);
    
    fz_try(ctx)
    {
        dict = pdf_new_dict(doc, 3);
        
        obj = pdf_new_real(doc, opacity);
        pdf_dict_puts(dict, "CA", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_new_name(doc, "ExtGState");
        pdf_dict_puts(dict, "Type", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_new_real(doc, opacity);
        pdf_dict_puts(dict, "ca", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        idict_num = pdf_create_object(doc);
        pdf_update_object(doc, idict_num, dict);
        ExtGState = pdf_new_indirect(doc, idict_num, 0);
        pdf_drop_obj(dict);
        dict = NULL;
    }
    fz_catch(ctx)
    {
        pdf_drop_obj(obj);
        pdf_drop_obj(dict);
        pdf_drop_obj(ExtGState);
    }
    return ExtGState;
}

/* pdf_update_watermark_docset_stream: docset的流
 * 参数类似于：type,Type0,size,24.0 */
static char *
pdf_update_docset_stream(char *docset, int count, const char *fmt, va_list old_args)
{
    char *stream  = malloc(sizeof(char) * strlen(docset));
    char *set_str = malloc(sizeof(char) * strlen(docset));
    strcpy(set_str, docset);
    
    char *key;
    char val[32];
    
    if (docset == NULL)
        return NULL;
    
    /* 设置水印的内容参数 */
    if (fmt != NULL)
    {
        char *to_fo = strstr(set_str, "Font");
        char *to_ab = strstr(to_fo, ">");
        
        memcpy(stream, set_str, to_ab - set_str + 1);
        stream[to_ab - set_str + 1] = '\0';
        
        strcat(stream, fmt);
        
        char *to_sc = strstr(set_str, "<Scale");
        strcat(stream, to_sc);
        
        strcpy(set_str, stream);
    }
    
    /* 获取相应的参数进行设置 */
    va_list args;
    
#ifdef _MSC_VER
    args = old_args;
#else
    va_copy(args, old_args);
#endif
    
    for (int i=0; i<count; i++)
    {
        key = va_arg(args, char *);
        if (!strcmp(key, "type") || !strcmp(key, "name"))
            sprintf(val, "%s",va_arg(args, char *));
        else
            sprintf(val, "%f",va_arg(args, double));
        
        char *s = strstr(set_str, key);
        if (NULL == s)
            continue;
        
        int n = 0;
        while (n != 2)
        {
            if (*s == '"' && n == 0)
            {
                strncpy(stream, set_str, s - set_str + 1);
                stream[s - set_str + 1] = '\0';
                strcat(stream, val);
                n += 1;
            }
            else if (*s == '"' && n == 1)
            {
                strcat(stream, s);
                strcpy(set_str, stream);
                n += 1;
            }
            else if (*s == '\0')
            {
                break;
            }
            
            s++;
        }
        
    }
    
#ifndef _MSC_VER
    va_end(args);
#endif
    
    free(set_str);
    
    if (!strcmp(stream, ""))
        strcpy(stream, docset);
    return stream;
}

pdf_obj *
pdf_create_docset(pdf_document *doc, char *setstr, int count, const char *fmt,...)
{
    int idict_num;
    pdf_obj *obj = NULL;
    pdf_obj *dict = NULL;
    pdf_obj *docset = NULL;
    
    fz_context *ctx = doc->ctx;
    
    fz_var(dict);
    fz_var(obj);
    fz_var(docset);
    
    fz_try(ctx)
    {
        va_list args;
        va_start(args, fmt);
        char *xml = pdf_update_docset_stream(setstr, count, fmt, args);
        va_end(args);
        
        dict = pdf_new_dict(doc, 1);
        
        obj = pdf_new_int(doc, strlen(xml));
        pdf_dict_puts(dict, "Length", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        idict_num = pdf_create_object(doc);
        pdf_update_object(doc, idict_num, dict);
        docset = pdf_new_indirect(doc, idict_num, 0);
        pdf_drop_obj(dict);
        dict = NULL;
        
        fz_buffer *docset_buffer = fz_new_buffer_from_data(ctx, (unsigned char *)xml, strlen(xml));
        pdf_update_stream(doc, idict_num, docset_buffer);
        fz_drop_buffer(ctx, docset_buffer);
        docset_buffer = NULL;
    }
    fz_catch(ctx)
    {
        pdf_drop_obj(obj);
        pdf_drop_obj(dict);
        pdf_drop_obj(docset);
    }
    
    return docset;
}

pdf_obj*
pdf_create_CIDSystemInfo(pdf_document *doc)
{
    int idict_num;
    pdf_obj *obj = NULL;
    pdf_obj *dict = NULL;
    pdf_obj *CIDSystemInfo = NULL;
    
    fz_context *ctx = doc->ctx;
    
    fz_var(dict);
    fz_var(obj);
    fz_var(CIDSystemInfo);
    
    fz_try(ctx)
    {
        
        dict = pdf_new_dict(doc, 3);
        
        obj = pdf_new_string(doc, "GB1", strlen("GB1"));
        pdf_dict_puts(dict, "Ordering", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_new_string(doc, "Adobe", strlen("Adobe"));
        pdf_dict_puts(dict, "Registry", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_new_int(doc, 4);
        pdf_dict_puts(dict, "Supplement", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        idict_num = pdf_create_object(doc);
        pdf_update_object(doc, idict_num, dict);
        CIDSystemInfo = pdf_new_indirect(doc, idict_num, 0);
        pdf_drop_obj(dict);
        dict = NULL;
    }
    fz_catch(ctx)
    {
        pdf_drop_obj(obj);
        pdf_drop_obj(dict);
        pdf_drop_obj(CIDSystemInfo);
    }
    
    return CIDSystemInfo;
}

/* reference: "PDF3200_2008 9.8 Font Descriptors" */
pdf_obj*
pdf_create_FontDescriptor(pdf_document *doc)
{
    int idict_num;
    pdf_obj *obj = NULL;
    pdf_obj *dict = NULL;
    pdf_obj *docset = NULL;
    
    fz_context *ctx = doc->ctx;
    
    fz_var(dict);
    fz_var(obj);
    fz_var(docset);
    
    fz_try(ctx)
    {
        dict = pdf_new_dict(doc, 1);
        
        obj = pdf_new_int(doc, 1000);
        pdf_dict_puts(dict, "Ascent", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_new_int(doc, 667);
        pdf_dict_puts(dict, "CapHeight", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_new_int(doc, -167);
        pdf_dict_puts(dict, "Descent", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_new_int(doc, 4);
        pdf_dict_puts(dict, "Flags", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_new_name(doc, "Hei");
        pdf_dict_puts(dict, "FontName", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_new_int(doc, 0);
        pdf_dict_puts(dict, "ItalicAngle", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_new_int(doc, 80);
        pdf_dict_puts(dict, "StemV", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_new_name(doc, "FontDescriptor");
        pdf_dict_puts(dict, "Type", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_new_array(doc, 4);
        pdf_array_push_drop(obj, pdf_new_int(doc, -5));
        pdf_array_push_drop(obj, pdf_new_int(doc, -167));
        pdf_array_push_drop(obj, pdf_new_int(doc, 1012));
        pdf_array_push_drop(obj, pdf_new_int(doc, 1000));
        pdf_dict_puts(dict, "FontBBox", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        idict_num = pdf_create_object(doc);
        pdf_update_object(doc, idict_num, dict);
        docset = pdf_new_indirect(doc, idict_num, 0);
        pdf_drop_obj(dict);
        dict = NULL;
    }
    fz_catch(ctx)
    {
        pdf_drop_obj(obj);
        pdf_drop_obj(dict);
        pdf_drop_obj(docset);
    }
    
    return docset;
}

pdf_obj*
pdf_create_DescendantFonts(pdf_document *doc)
{
    int idict_num;
    pdf_obj *obj = NULL;
    pdf_obj *dict = NULL;
    pdf_obj *docset = NULL;
    
    fz_context *ctx = doc->ctx;
    
    fz_var(dict);
    fz_var(obj);
    fz_var(docset);
    
    fz_try(ctx)
    {
        dict = pdf_new_dict(doc, 1);
        
        obj = pdf_new_name(doc, "AdobeSongStd-Light");
        pdf_dict_puts(dict, "BaseFont", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_create_CIDSystemInfo(doc);
        pdf_dict_puts(dict, "CIDSystemInfo", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_new_int(doc, 1000);
        pdf_dict_puts(dict, "DW", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_create_FontDescriptor(doc);
        pdf_dict_puts(dict, "FontDescriptor", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_new_name(doc, "CIDFontType2");
        pdf_dict_puts(dict, "Subtype", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_new_name(doc, "Font");
        pdf_dict_puts(dict, "Type", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        /* FIXME: 还缺少/w，/w是用来调整字符间距的 */
        
        idict_num = pdf_create_object(doc);
        pdf_update_object(doc, idict_num, dict);
        docset = pdf_new_indirect(doc, idict_num, 0);
        pdf_drop_obj(dict);
        dict = NULL;
    }
    fz_catch(ctx)
    {
        pdf_drop_obj(obj);
        pdf_drop_obj(dict);
        pdf_drop_obj(docset);
    }
    
    return docset;
}

pdf_obj*
pdf_create_DescendantFonts_array(pdf_document *doc)
{
    int idict_num;
    pdf_obj *obj = NULL;
    pdf_obj *docset = NULL;
    
    fz_context *ctx = doc->ctx;
    
    fz_var(obj);
    fz_var(docset);
    
    fz_try(ctx)
    {
        
        obj = pdf_new_array(doc, 1);
        pdf_array_push_drop(obj, pdf_create_DescendantFonts(doc));
        
        idict_num = pdf_create_object(doc);
        pdf_update_object(doc, idict_num, obj);
        docset = pdf_new_indirect(doc, idict_num, 0);
        pdf_drop_obj(obj);
        obj = NULL;
    }
    fz_catch(ctx)
    {
        pdf_drop_obj(obj);
        pdf_drop_obj(docset);
    }
    
    return docset;
}

/* pdf_update_watermark_font: 创建字体对象
 * 只是简单的使用了一个默认的字体，各种字体的添加还需要修改 */
pdf_obj *
pdf_create_font(pdf_document *doc, char *font_name)
{
    int idict_num;
    pdf_obj *obj = NULL;
    pdf_obj *dict = NULL;
    pdf_obj *font = NULL;
    
    fz_context *ctx = doc->ctx;
    
    fz_var(dict);
    fz_var(obj);
    fz_var(font);
    
    fz_try(ctx)
    {
        dict = pdf_new_dict(doc, 4);
        
        obj = pdf_new_name(doc, "Font");
        pdf_dict_puts(dict, "Type", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_new_name(doc, "Type1");
        pdf_dict_puts(dict, "Subtype", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_new_name(doc, "Helvetica");
        pdf_dict_puts(dict, "BaseFont", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_new_name(doc,font_name);
        pdf_dict_putp_drop(dict, "Name", obj);
//        obj = pdf_create_DescendantFonts_array(doc);
//        pdf_dict_puts(dict, "DescendantFonts", obj);
//        pdf_drop_obj(obj);
//        obj = NULL;
        
//        obj = pdf_new_name(doc, "UniGB-UTF16-H");
        obj = pdf_new_name(doc, "MacRomanEncoding");
        pdf_dict_puts(dict, "Encoding", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        idict_num = pdf_create_object(doc);
        pdf_update_object(doc, idict_num, dict);
        font = pdf_new_indirect(doc, idict_num, 0);
        pdf_drop_obj(dict);
        dict = NULL;
    }
    fz_catch(ctx)
    {
        pdf_drop_obj(obj);
        pdf_drop_obj(dict);
        pdf_drop_obj(font);
    }
    
    return font;
}

pdf_obj *
pdf_create_xobject(pdf_document *doc, char *text, char *font_name, int font_size, float rgb[], fz_rect rect)
{
    int idict_num;
    pdf_obj *obj = NULL;
    pdf_obj *dict = NULL;
    pdf_obj *XObject = NULL;
    pdf_obj *font = NULL;
    pdf_obj *docset = NULL;
    
    fz_context *ctx = doc->ctx;
    
    fz_var(dict);
    fz_var(obj);
    fz_var(XObject);
    fz_var(font);
    fz_var(docset);
    
    font = pdf_create_font(doc, font_name);
    docset = pdf_create_docset(doc, DEFAUL_TEXT_DOCSET, 0, text);
    float xy[2];
    xy[0] = rect.x0;
    xy[1] = rect.y0 ;
    
    fz_matrix mat = {1,0,0,1,0,0};
    char *stream = pdf_update_xobject_stream(text, font_name, font_size, rgb, xy);
    fz_try(ctx)
    {
        dict = pdf_new_dict(doc, 8);
        
        obj = pdf_new_rect(doc, &rect);
		pdf_dict_puts(dict, "BBox", obj);
		pdf_drop_obj(obj);
		obj = NULL;
        
        obj = pdf_new_matrix(doc, &mat);
		pdf_dict_puts(dict, "Matrix", obj);
		pdf_drop_obj(obj);
		obj = NULL;
        
        obj = pdf_new_name(doc, "Form");
		pdf_dict_puts(dict, "Subtype", obj);
		pdf_drop_obj(obj);
		obj = NULL;
        
        obj = pdf_new_int(doc, strlen(stream));
		pdf_dict_puts(dict, "Length", obj);
		pdf_drop_obj(obj);
		obj = NULL;
        
        char font_key[48] = "Resources/Font/";
        strcat(font_key, font_name);
        pdf_dict_putp(dict, font_key, font);
        pdf_drop_obj(font);
        
        obj = pdf_new_name(doc, "Watermark");
        pdf_dict_putp(dict, "PieceInfo/ADBE_CompoundType/Private", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_new_string(doc, "D:20130801170220+08'00'", strlen("D:20130801170220+08'00'"));
        pdf_dict_putp(dict, "PieceInfo/ADBE_CompoundType/LastModified", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        pdf_dict_putp(dict, "PieceInfo/ADBE_CompoundType/DocSettings",docset);
        pdf_drop_obj(docset);
        
        idict_num = pdf_create_object(doc);
        pdf_update_object(doc, idict_num, dict);
        XObject = pdf_new_indirect(doc, idict_num, 0);
        pdf_drop_obj(dict);
        dict = NULL;
        
        fz_buffer *buffer = fz_new_buffer_from_data(ctx, (unsigned char *)stream, strlen(stream));
        pdf_update_stream(doc, idict_num, buffer);
        fz_drop_buffer(ctx, buffer);
        buffer = NULL;
        
    }
    fz_catch(ctx)
    {
        pdf_drop_obj(obj);
        pdf_drop_obj(dict);
        pdf_drop_obj(XObject);
        pdf_drop_obj(font);
        pdf_drop_obj(docset);
    }
    
    return XObject;
}

pdf_obj *
pdf_create_content(pdf_document *doc, pdf_obj *pageobj, fz_matrix mat, char *gs_name, char *xobj_name)
{
    int idict_num;
    pdf_obj *obj = NULL;
    pdf_obj *dict = NULL;
    pdf_obj *content = NULL;
    
    fz_context *ctx = doc->ctx;
    
    fz_var(dict);
    fz_var(obj);
    fz_var(content);
    
    char *content_data = pdf_update_content_stream(mat, gs_name, xobj_name);
    
    fz_try(ctx)
    {
        dict = pdf_new_dict(doc, 1);
        //pdf_dict_puts(dict, "Filter", pdf_new_name(doc, "FlateDecode"));
        
        obj = pdf_new_int(doc, strlen(content_data));
        pdf_dict_puts(dict, "Length", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        idict_num = pdf_create_object(doc);
        pdf_update_object(doc, idict_num, dict);
        content = pdf_new_indirect(doc, idict_num, 0);
        pdf_drop_obj(dict);
        dict = NULL;
        
        fz_buffer *buffer = fz_new_buffer_from_data(ctx, (unsigned char *)content_data, strlen(content_data));
        pdf_update_stream(doc, idict_num, buffer);
        fz_drop_buffer(ctx, buffer);
        buffer = NULL;
    }
    fz_catch(ctx)
    {
        pdf_drop_obj(obj);
        pdf_drop_obj(dict);
        pdf_drop_obj(content);
    }
    
    return content;
}
/*
 TODO:get the date of current day
 */
static
void getDate(char da[256])
{
    time_t date = time(NULL);
    //printf("%ld",date);
    struct tm *ldate = localtime(&date);
    //char *da= (char *)malloc(25*sizeof(char));
    sprintf(da, "%s","D:");
    strcat(da, asctime(ldate));
   // return da;
}

/*
 TODO:modify the time of the last modified time
 */
void pdf_update_lastModified(pdf_document *doc,pdf_obj *Xobj)
{
    char date[256];
    getDate(date);
    pdf_dict_putp_drop(Xobj, "PieceInfo/ADBE_CompoundType/LastModified", pdf_new_string(doc, date, strlen(date)));
    pdf_dict_putp_drop(Xobj, "LastModified", pdf_new_string(doc, date, strlen(date)));
}
/*
 TODO:创建一个图片xobject
 */
static
int create_watermark_xobject_img(pdf_document *doc,const char *imgName,const float width,const float height)
{
    pdf_obj *imgXobject =pdf_new_dict(doc, 0);
    int img = pdf_create_object(doc);
    
    fz_try(doc->ctx)
    {
        pdf_dict_putp_drop(imgXobject, "Type", pdf_new_name(doc, "XObject"));
        pdf_dict_putp_drop(imgXobject, "Filter", pdf_new_name(doc, "DCTDecode"));
        pdf_dict_putp_drop(imgXobject, "Subtype", pdf_new_name(doc, "Image"));
        pdf_dict_putp_drop(imgXobject, "Width", pdf_new_real(doc, width));
        pdf_dict_putp_drop(imgXobject,"Height", pdf_new_real(doc, height));
        pdf_dict_putp_drop(imgXobject, "ColorSpace", pdf_new_name(doc, "DeviceRGB"));
        pdf_dict_putp_drop(imgXobject, "BitsPerComponent", pdf_new_int(doc, 8));
        
        fz_stream *imgStream =  fz_open_file(doc->ctx,imgName);
        fz_buffer *imgBuffer = fz_read_all(imgStream, 1024);
        pdf_dict_putp_drop(imgXobject, "Length", pdf_new_int(doc, imgBuffer->len));
        
        pdf_update_object(doc, img, imgXobject);
        pdf_update_stream(doc, img, imgBuffer);
    }
    fz_always(doc->ctx)
    {
        pdf_drop_obj(imgXobject);
    }
    fz_catch(doc->ctx)
    {
        fz_throw(doc->ctx, 1, "create imge error");
    }
    return img;
    
}

/*
 TODO:创建一个form 
 img:图片xobject的num
 */
static
int create_watermark_xobject_form(pdf_document *doc,int img,const fz_matrix matrix,const fz_rect *bbox)
{
    
    pdf_obj *xobject =  pdf_new_xobject(doc,bbox, &matrix);
    pdf_obj *xoo = pdf_load_object(doc, pdf_to_num(xobject), 0);//the xobject's dictionary
    
    //add group 
    pdf_obj *group = pdf_new_dict(doc, 3);
    pdf_dict_putp_drop(group, "K", pdf_new_bool(doc, 1));
    pdf_dict_putp_drop(group, "S", pdf_new_name(doc, "Transparency"));
    pdf_dict_putp_drop(group, "Type", pdf_new_name(doc, "Group"));
    pdf_dict_putp_drop(xoo, "Group", pdf_new_ref(doc, group));
    pdf_drop_obj(group);
    
    //add the ocg
    pdf_obj *OCG =  pdf_new_dict(doc, 10);
    pdf_dict_putp_drop(OCG, "Name", pdf_new_string(doc,"Watermark",9) );
    pdf_dict_putp_drop(OCG, "Type", pdf_new_name(doc, "OCG"));
    pdf_obj *state = pdf_new_dict(doc, 1);
    pdf_dict_putp_drop(state, "ViewState", pdf_new_name(doc, "ON"));
    pdf_obj *view = pdf_new_dict(doc, 1);
    pdf_dict_putp_drop(view, "View",  state);
    pdf_dict_putp_drop(OCG, "Usage", view);
    pdf_dict_putp_drop(xoo, "OC", pdf_new_ref(doc,OCG));
    pdf_obj *ar = pdf_new_array(doc, 1);
    pdf_array_push_drop(ar, pdf_new_ref(doc,OCG));
    pdf_drop_obj(OCG);
    
    //set ocg in  catalog
    pdf_obj *catalog =  pdf_dict_getp(pdf_trailer((pdf_document *)doc), "Root");
    pdf_obj *as = pdf_new_dict(doc, 3);
    pdf_dict_putp(as, "OCGs", ar);
    pdf_dict_putp_drop(as, "Event", pdf_new_name(doc, "View"));
    pdf_obj *ca = pdf_new_array(doc, 1);
    pdf_array_push_drop(ca, pdf_new_name(doc, "View"));
    pdf_dict_putp_drop(as, "Category", ca);
    pdf_obj *arr = pdf_new_array(doc, 1);
    pdf_array_push_drop(arr, as);
    pdf_obj *d = pdf_new_dict(doc, 2);
    pdf_dict_putp_drop(d, "AS", arr);
    pdf_dict_putp(d, "ON", ar);
    pdf_obj *opc = pdf_new_dict(doc, 2);
    pdf_dict_putp_drop(opc, "D", d);
    pdf_dict_putp_drop(opc, "OCGs", ar);
    pdf_dict_putp_drop(catalog, "OCProperties", opc);
    
    //add the graphic state
    pdf_obj *res = pdf_new_dict(doc, 3);
    pdf_obj *extgstate = pdf_create_extgstate(doc, 0.5);
    pdf_obj * GSW = pdf_new_dict(doc, 1);
    pdf_dict_putp_drop(GSW, "GSW", extgstate);
    pdf_dict_putp_drop(res, "ExtGState", GSW);
    
    // the doc settings
    pdf_obj *procSet = pdf_new_array(doc, 2);
    pdf_array_push_drop(procSet, pdf_new_name(doc, "PDF"));
    pdf_array_push_drop(procSet, pdf_new_name(doc, "ImageC"));
    pdf_dict_putp_drop(res, "ProcSet", procSet);
    pdf_obj *ximg = pdf_new_dict(doc, 1);
    pdf_dict_putp_drop(ximg, "waterImg", pdf_new_indirect(doc, img, 0));
    pdf_dict_putp_drop(res, "XObject", ximg);
    pdf_dict_putp_drop(xoo,"Resources", res);
    
    pdf_obj *imga = pdf_load_object(doc, img, 0);
    float with = pdf_to_real(pdf_dict_gets(imga,  "Width"));
    float height = pdf_to_real(pdf_dict_gets(imga, "Height"));
    //printf("%f",with);
    char *data= (char*)malloc(100*sizeof(char));
    sprintf(data, "%s","q\n");
    
    char *w = (char *)malloc(10*sizeof(char));
    sprintf(w, "%.1f ",with);
    strcat(data, w);
    free(w);
    strcat(data,"0 0 ");
    
    char *h = (char *)malloc(10*sizeof(char));
    sprintf(h, "%.1f ",height);
    strcat(data, h);
    free(h);
    char *b = (char*)malloc(20*sizeof(char));
    sprintf(b, "%.1f %.1f ",bbox->x0,bbox->y0);//the location of the image in the form frame
    strcat(data, b);
    free(b);
    
   strcat(data,"cm\n/GSW gs\n/waterImg Do\nQ\n");
    
    fz_buffer *buffer = fz_new_buffer_from_data(doc->ctx,( unsigned char *)data , strlen(data));
    //peiceinfo in xobject
    
    int docSetNum = pdf_create_object(doc);
    pdf_obj *docSet = pdf_new_dict(doc, 0);
    
    fz_buffer *bf = fz_new_buffer_from_data(doc->ctx, (unsigned char *)DEFAUL_IMG_DOCSET, strlen(DEFAUL_IMG_DOCSET));
    pdf_dict_putp_drop(docSet, "Length", pdf_new_int(doc, strlen(DEFAUL_IMG_DOCSET)));
    pdf_update_object(doc, docSetNum, docSet);
    pdf_drop_obj(docSet);
    pdf_update_stream(doc, docSetNum, bf);
    pdf_dict_putp_drop(xoo, "PieceInfo/ADBE_CompoundType/DocSettings",pdf_new_indirect(doc, docSetNum, 0) );
    pdf_dict_putp_drop(xoo, "PieceInfo/ADBE_CompoundType/Private", pdf_new_name(doc, "Watermark"));
    pdf_update_lastModified(doc,xoo);
    pdf_update_xobject_contents(doc, pdf_load_xobject(doc, xobject), buffer);
   
    return pdf_to_num(xobject);

}
/*
 TODO:在page的content里设置水印
 */
static
void pdf_add_watermark_toPage(pdf_document *doc,int pageNum,int formXobject,const fz_rect *bbox)
{
    
    pdf_obj *content = NULL;
    pdf_page *page = pdf_load_page(doc, pageNum);
    pdf_obj *pageObj = page->me;
    int form  = formXobject;
    fz_context *ctx = doc->ctx;
    
    //1:procset
    pdf_obj *res = pdf_dict_gets(pageObj, "Resources");
    pdf_obj *procSet= pdf_new_array(doc, 3);
    pdf_array_push_drop(procSet, pdf_new_name(doc, "PDF"));
    pdf_array_push_drop(procSet, pdf_new_name(doc, "Text"));
    pdf_array_push_drop(procSet, pdf_new_name(doc, "ImageC"));
    pdf_dict_putp_drop(res, "ProcSet", procSet);
    
    //2:watermark xobject
    pdf_obj *xobject = pdf_dict_gets(res, "XObject");
    if(NULL != xobject)
    {
        pdf_dict_putp_drop(xobject, "waterMark",pdf_new_indirect(doc,form, 0));
    }
    else
    {
        pdf_obj *waterMark = pdf_new_dict(doc, 1);
        pdf_dict_putp_drop(waterMark, "waterMark",pdf_new_indirect(doc,form, 0));
        pdf_dict_putp_drop(res,"XObject",waterMark);
    }
    
    
    //3:set page contents stream
    int newContent = pdf_create_object(doc);
    
    char *data2 = (char *)malloc(200*sizeof(char));
    sprintf(data2, "%s","/Artifact <</Subtype /Watermark /Type /Pagination >>BDC\n\
            q\n\
            1 0 0 1 ");
    char * cm = (char*)(malloc(20*sizeof(char)));
    sprintf(cm, "%.1f %.1f ",bbox->x0,bbox->y0);//the location of the form in the page
    strcat(data2, cm);
    free(cm);
    strcat(data2,"cm\n/waterMark Do\nQ\nEMC\n");
    
    pdf_obj *new = pdf_new_dict(doc, 1);
    pdf_dict_putp_drop(new, "Length", pdf_new_int(doc, strlen(data2)));//the content stream
    pdf_update_object(doc, newContent, new);
    pdf_drop_obj(new);
    
    fz_buffer *buffer2 = fz_new_buffer_from_data(ctx, (unsigned char *)data2, strlen(data2));
    pdf_update_stream(doc, newContent, buffer2);
    
    content = pdf_dict_getp(pageObj, "Contents");
    pdf_obj *newContents ;
    if(pdf_is_array(content))
    {
        newContents = pdf_new_array(doc,0);
        newContents= content;
        pdf_array_push_drop(newContents, pdf_new_indirect(doc, newContent, 0));
    }
    else
    {
        newContents = pdf_new_array(doc, 2);
        pdf_array_push(newContents, content);
        pdf_array_push_drop(newContents, pdf_new_indirect(doc, newContent, 0));
    }
    pdf_dict_putp_drop(pageObj, "Contents", newContents);
}
/*
 TODO:添加图片水印
 *parameter imgWidth :the width of the image
 *parameter imgHeight :the height of the image
 *parameter bbox :the rect of the form xobject which to show the watermark
 没有返回值，待修改
 */
pdf_watermark *pdf_create_watermark_with_img(pdf_document *doc,int pageNum,const char *imgName,const float imgWidth,const float imgHeight,const fz_rect *bbox)
{

    pdf_watermark *watermark= NULL;
    int img = 0 ;
    int form = 0 ;
    int i = 0 ;
    
    /*
     step1:create a image xobject
     */
    img = create_watermark_xobject_img(doc, imgName, imgWidth, imgHeight);
    
    /*
     step2:create a form xobject that to show the image xobject
     */
    const fz_matrix matrix = {1,0,0,1,0,0};
    fz_rect rect;
    rect.x0 = 0;
    rect.y0 = 0;
    rect.x1 = bbox->x1-bbox->x0;
    rect.y1 = bbox->y1-bbox->y0;
    form = create_watermark_xobject_form(doc,img,matrix,&rect);
       
    /*
     step3:add the image watermark to the content(s) of the page(s)
     */
    if(ALL == pageNum)//add the watermark  to all pages
    {
        int count = pdf_count_pages(doc);
        for(i =0 ;i<count;i++)
        {
            pdf_add_watermark_toPage(doc, i, form, bbox);
        }
    }
    else{//add the watermark to specified page
        pdf_add_watermark_toPage(doc,pageNum,form,bbox);
    }
    
//    watermark = fz_malloc_struct(doc->ctx, pdf_watermark);
//    pdf_watermark_init(doc, watermark, "D:20130801170220+08'00'");
//    watermark_array_push_drop(watermark->XObject, xobject_item);
//    watermark_array_push_drop(watermark->ExtGState, ExtGState_item);
//    pdf_array_push_drop(watermark->content, content);
//    
//    if (doc->watermarks == NULL)
//    {
//        doc->watermarks = fz_malloc_struct(ctx, pdf_watermark);
//        pdf_watermark_init(doc, doc->watermarks, "");
//    }
//    
//    watermark->next = doc->watermarks->next;
//    doc->watermarks->next = watermark;
    return NULL;
}
static
void update_watermark_with_img(pdf_document *doc,int pageNum ,const char* imageName)
{
    return;
}
/*
 TODO:update the img of the watermark
 */
void pdf_update_watermark_with_img(pdf_document *doc,int pageNum,const char *imageName,const float imgWidth,const float imgHeight)
{
    if(ALL == pageNum)
    {
        pageNum = 0;
    }
    fz_try(doc->ctx)
    {
        int img = 0;
        pdf_obj *pageObj = doc->page_objs[pageNum];
        pdf_obj *watermark = pdf_dict_getp(pageObj, "Resources/XObject/waterMark");
        if(NULL != watermark)
        {
            pdf_obj *form = pdf_load_object(doc, pdf_to_num(watermark), 0);
            watermark = NULL;
            watermark = pdf_dict_getp(form, "Resources/XObject/waterImg");
            img = create_watermark_xobject_img(doc, imageName, imgWidth, imgHeight);//create a new image for the watermark
            pdf_dict_putp_drop(form, "Resources/XObject/waterImg", pdf_new_indirect(doc, img, 0));
        }
    }
    fz_catch(doc->ctx)
    {
        fz_throw(doc->ctx, 1, "failed to update the stream of the image");
    }

    return ;
}

/* pdf_create_watermark_with_string: 通过文字创建水印
 * 参数：
 *   doc pdf文档
 *   str 水印的文字内容 */
pdf_watermark *pdf_create_watermark_with_string(pdf_document *doc, char *text, fz_rect rect)
{
    pdf_watermark *watermark = NULL;
    pdf_obj *ExtGState = NULL;
    pdf_obj *XObject = NULL;
    pdf_obj *content = NULL;
    
    fz_context *ctx = doc->ctx;
    
    /* step1: new ExtGState */
    ExtGState = pdf_create_extgstate(doc, 0.5);
    
    /* step2: new Xobject */
    /* 默认颜色 */
    float rgb[] = {0,0,0};
    XObject = pdf_create_xobject(doc , text, "Hevi", 28, rgb, rect);
    
    /* 变换矩阵，内容的旋转，缩放 */
    /* 默认变换矩阵 */
    fz_matrix content_mat = {1, 0, 0, 1,0, 0};
    //fz_pre_rotate(&content_mat, 45);
    
    int count = pdf_count_pages(doc);
    for (int i = 0; i<count; i++)
    {
        pdf_obj *pageobj = doc->page_objs[i];
        
        char new_gskey[13] = {};
        pdf_create_gs_name(pageobj, new_gskey);
        char gskey[40] = "Resources/ExtGState/";
        strcat(gskey, new_gskey);
        
        char new_fmkey[13] = {};
        pdf_create_xobj_name(pageobj, new_fmkey);
        char fmkey[40] = "Resources/XObject/";
        strcat(fmkey, new_fmkey);
        
        /* step3: new content */
        content = pdf_create_content(doc, pageobj, content_mat, new_gskey, new_fmkey);
        
        fz_try(ctx)
        {
            /* step4: add gs,fm and content to page */
            /* add GS */
            pdf_dict_putp(pageobj, gskey, ExtGState);
            
            /* add Fm */
            pdf_dict_putp(pageobj, fmkey, XObject);
            
            /* add content*/
            pdf_obj *old_contents = pdf_dict_gets(pageobj, "Contents");
            if (pdf_is_array(old_contents))
            {
                pdf_array_push(old_contents, content);
            }
            else
            {
                pdf_obj *new_contents = pdf_new_array(doc, 2);
                pdf_array_push(new_contents, old_contents);
                pdf_array_push(new_contents, content);
                pdf_dict_puts(pageobj, "Contents", new_contents);
                pdf_drop_obj(new_contents);
                new_contents = NULL;
            }
            
            /* step5: create watermark struct */
            pdf_obj *xobject_item = pdf_new_dict(doc, 1);
            pdf_dict_puts(xobject_item, new_fmkey, XObject);
            
            pdf_obj *ExtGState_item = pdf_new_dict(doc, 1);
            pdf_dict_puts(ExtGState_item, new_gskey, ExtGState);
            
            watermark = fz_malloc_struct(ctx, pdf_watermark);
            pdf_watermark_init(doc, watermark, "D:20130801170220+08'00'");
            watermark_array_push_drop(watermark->XObject, xobject_item);
            watermark_array_push_drop(watermark->ExtGState, ExtGState_item);
            pdf_array_push_drop(watermark->content, content);

            if (doc->watermarks == NULL)
            {
                doc->watermarks = fz_malloc_struct(ctx, pdf_watermark);
                pdf_watermark_init(doc, doc->watermarks, "");
            }

            watermark->next = doc->watermarks->next;
            doc->watermarks->next = watermark;
        }
        fz_always(ctx)
        {
            pdf_drop_obj(content);
        }
        fz_catch(ctx)
        {
            
        }
    }
    pdf_drop_obj(ExtGState);
    pdf_drop_obj(XObject);
    
    return watermark;
}

/* 删除内容流中指定水印的一段，返回删除后的字符串，如果不能成功删除，则返回NULL
 */
static char*
delete_watermark_from_stream(pdf_document *doc, pdf_obj *w_xobj, char *stream)
{
    int stream_len = strlen(stream);
    
    char *new_stream = malloc(sizeof(char) * stream_len);
    char *trans = malloc(sizeof(char) * stream_len);
    
    int xobj_len = pdf_array_len(w_xobj);
    for (int j=0; j<xobj_len; j++)
    {
        pdf_obj *xobj = pdf_dict_get_key(pdf_array_get(w_xobj, j), 0);
        char *xobj_name = pdf_to_name(xobj);
        
        char *name = strstr(stream, xobj_name);
        if (name)
        {
            strncpy(trans, stream, name - stream);
            trans[name - stream] = '\0';
            
            char *before = stream_before_name(trans);
            char *after = stream_after_name(name);
            
            if (before && after)
            {
                int len = strlen(stream) - strlen(name) - strlen(before);
                strncpy(new_stream, stream, len);
                new_stream[len] = '\0';
                strcat(new_stream, after);
            }
        }
    }
    
    free(trans);
    return new_stream;
}

/* 从内容流当中获取指定水印的一段，如果成功返回这段字符串，不成功返回NULL
 */
static char*
find_watermark_from_stream(pdf_document *doc, pdf_obj *xobj, char *stream)
{
    int stream_len = strlen(stream);
    
    char *new_stream = malloc(sizeof(char) * stream_len);
    char *trans = malloc(sizeof(char) * stream_len);
    
    char *xobj_name = pdf_to_name(xobj);
    
    char *name = strstr(stream, xobj_name);
    if (name)
    {
        strncpy(trans, stream, name - stream);
        trans[name - stream] = '\0';
        
        char *before = stream_before_name(trans);
        char *after = stream_after_name(name);
        
        if (before && after)
        {
            int begin = strlen(trans) - strlen(before);
            int len = strlen(stream) - strlen(after) - begin;
            strncpy(new_stream, stream + begin, len);
            new_stream[len] = '\0';
        }
    }
    
    free(trans);
    return new_stream;
}

/* pdf_delete_watermark: 删除水印
 * 参数：
 *   watermark   要删除的水印 */
void
pdf_delete_watermark(pdf_document *doc, pdf_watermark *watermark)
{
    pdf_obj *w_gs;
    pdf_obj *w_xobj;
    pdf_obj *w_cont;
    
    if (watermark == NULL)
        return;
    
    w_gs = watermark->ExtGState;
    w_xobj = watermark->XObject;
    w_cont = watermark->content;
    
    fz_context *ctx = doc->ctx;
    
    /* step1: delete page resources */
    int count = pdf_count_pages(doc);
    for (int i = 0; i<count; i++)
    {
        pdf_obj *pageobj = doc->page_objs[i];
        pdf_obj *p_gs = pdf_dict_getp(pageobj, "Resources/ExtGState");
        pdf_obj *p_xobj = pdf_dict_getp(pageobj, "Resources/XObject");
        
        /* 删除xobj */
        int len = pdf_array_len(w_gs);
        for (int j=0; j<len; j++)
        {
            char *gs_name = pdf_to_name(pdf_dict_get_key(pdf_array_get(w_gs, j), 0));
            pdf_dict_dels(p_gs, gs_name);
        }
        
        /* 删除gs */
        for (int j=0; j<len; j++)
        {
            char *xobj_name = pdf_to_name(pdf_dict_get_key(pdf_array_get(w_xobj, j),0));
            pdf_dict_dels(p_xobj, xobj_name);
        }
    }
    
    /* step2: delete page content stream */
    int cont_len = pdf_array_len(w_cont);
    for (int i=0; i<cont_len; i++)
    {
        pdf_obj *content = pdf_array_get(w_cont, i);
        
        /* 读取老的流 */
        fz_buffer *old_buff = pdf_load_stream(doc, pdf_to_num(content), pdf_to_gen(content));
        fz_buffer *new_buff;
        
        char *stream = (char *)old_buff->data;
        stream[old_buff->len] = '\0';
        
        /* 获取删除指定字符串之后的流 */
        char *new_stream = delete_watermark_from_stream(doc, w_xobj, stream);
        
        if (new_stream == NULL)
            strcpy(new_stream, stream);
        
        new_buff = fz_new_buffer_from_data(ctx, (unsigned char*)new_stream, strlen(new_stream));
        pdf_update_stream(doc, pdf_to_num(content), new_buff);
        fz_drop_buffer(ctx, new_buff);
        
        /* 更新字典 */
        pdf_dict_puts_drop(content, "Length", pdf_new_int(doc, strlen(new_stream)));
        pdf_dict_dels(content, "Filter");
        
        fz_drop_buffer(ctx, old_buff);
        old_buff = NULL;
    }
    
    /* step3: delete ExtGState */
    int gs_len = pdf_array_len(w_gs);
    for (int i=0; i<gs_len; i++)
    {
        pdf_obj *gs = pdf_dict_get_val(pdf_array_get(w_gs, i), 0);
        pdf_delete_object(doc, pdf_to_num(gs));
        pdf_drop_obj(gs);
    }
    
    /* step4: delete XObject */
    int xobj_len = pdf_array_len(w_xobj);
    for (int i=0; i<xobj_len; i++)
    {
        pdf_obj *xobj = pdf_dict_get_val(pdf_array_get(w_xobj, i), 0);
        pdf_delete_object(doc, pdf_to_num(xobj));
        pdf_drop_obj(xobj);
    }
    
    /* step5: delete other resources */
    /* FIXME: remind me */
    
    /* step6: free watermark struct */
    pdf_watermark *itr = doc->watermarks;
    char *delete_id = watermark->LastModified;
    while (itr->next)
    {
        char *itr_id = itr->next->LastModified;
        if (!strcmp(delete_id, itr_id))
        {
            itr->next = watermark->next;
            pdf_drop_watermark(doc, watermark);
            break;
        }
    }
}

/* pdf_updata_docset:更新水印的docset 
 * 参数：
 *   xobj   docset所在的xobj
 *   count  要设置的参数个数，比如要设置Opacity, 传入的参数为 Opacity，1.0，此时count为1
 *   fmt    水印的内容，如果不想改变则传NULL
 *   ...    va_list */
void
pdf_update_docset(pdf_document *doc, pdf_obj *xobj, int count, const char *fmt,...)
{
    pdf_obj *obj;
    
    fz_context *ctx = doc->ctx;
    
    pdf_obj *docset = pdf_dict_getp(xobj, "PieceInfo/ADBE_CompoundType/DocSettings");
    fz_buffer *old_buff = pdf_load_stream(doc, pdf_to_num(docset), pdf_to_gen(docset));
    char *old_stream = (char *)old_buff->data;
    old_stream[strlen(old_stream)] = '\0';
    
    fz_try(ctx)
    {
        va_list args;
        va_start(args, fmt);
        char *xml = pdf_update_docset_stream(old_stream, count, fmt, args);
        va_end(args);
        
        obj = pdf_new_int(doc, strlen(xml));
        pdf_dict_puts(docset, "Length", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        fz_buffer *docset_buffer = fz_new_buffer_from_data(ctx, (unsigned char *)xml, strlen(xml));
        pdf_update_stream(doc, pdf_to_num(docset), docset_buffer);
        fz_drop_buffer(ctx, docset_buffer);
        docset_buffer = NULL;
    }
    fz_always(ctx)
    {
        fz_drop_buffer(ctx, old_buff);
    }
    fz_catch(ctx)
    {
        pdf_drop_obj(obj);
    }
}

/* pdf_set_watermark_font_color：设置字体颜色 */
void
pdf_set_watermark_font_color(pdf_document *doc, pdf_watermark *watermark, float rgb[])
{
    pdf_obj *w_xobj = watermark->XObject;
    
    fz_context *ctx = doc->ctx;
    
    int xobj_len = pdf_array_len(w_xobj);
    for (int i=0; i<xobj_len; i++)
    {
        pdf_obj *xobj = pdf_dict_get_val(pdf_array_get(w_xobj, i), 0);
        
        fz_buffer *old_buff = pdf_load_stream(doc, pdf_to_num(xobj), pdf_to_gen(xobj));
        char *stream = (char *)old_buff->data;
        stream[old_buff->len] = '\0';
        
        char *name = strstr(stream, "rg");
        
//        char *name_tf = strstr(stream, "Tf");
        
        char *after;
        if (name != NULL)
            after = next_stream_skip_string(name, "rg");
        else
            name = strstr(stream, "g");
            
        if (name != NULL)
            after = next_stream_skip_string(name, "g");
        else
            continue;
        
        
        //printf("%s",after);
    }
    
//    pdf_obj *obj;
//    fz_buffer *old_buffer;
//    fz_buffer *new_buffer;
//    int rgb_len;
//    int key_len;
//    char *new_stream;
//    
//    fz_context *ctx = doc->ctx;
//    
//    pdf_obj *xobj = watermark->XObject;
//    int xobj_len = pdf_array_len(xobj);
//    
//    for (int i=0; i<xobj_len; i++)
//    {
//        rgb_len = 0;
//        obj = pdf_dict_get_val(pdf_array_get(xobj, i), 0);
//        
//        /* 更新流 */
//        old_buffer = pdf_load_stream(doc, pdf_to_num(obj), pdf_to_gen(obj));
//        
//        char *stream = (char *)old_buffer->data;
//        stream[old_buffer->len] = '\0';
//        
//        char *after_BT = strstr(stream, "BT");
//        if (!after_BT)
//            continue;
//        
//        char *end = strstr(after_BT, "rg");
//        key_len = 2;
//        if (!end)
//        {
//            end = strstr(after_BT, "g");
//            key_len = 1;
//        }
//        
//        if (!end)
//            continue;
//        
//        char c;
//        do
//        {
//            rgb_len++;
//            c = *(end - rgb_len);
//        }
//        while ((48<=c && c<=57) || c==' ' || c=='.' || c =='-' || c=='+');
//        
//        char *after_head = end - rgb_len + 1;
//        char head[256] = {};
//        long n = strlen((char*)old_buffer->data)-strlen(after_head);
//        strncpy(head, (char*)old_buffer->data, n);
//        head[n] = '\0';
//        
//        char rgb_str[35] = {};
//        color_rgb_to_str_buf(rgb, rgb_str);
//        char rgb_new[34] = {};
//        strncpy(rgb_new, rgb_str, strlen(rgb_str) - 1);
//        
//        new_stream = malloc(sizeof(char) * (256 + strlen(end)));
//        strcat(new_stream, head);
//        strcat(new_stream, rgb_new);
//        strcat(new_stream, end + key_len);
//        
//        new_buffer = fz_new_buffer_from_data(ctx, (unsigned char*)new_stream, strlen(new_stream));
//        pdf_update_stream(doc, pdf_to_num(obj), new_buffer);
//        fz_drop_buffer(ctx, new_buffer);
//        new_buffer = NULL;
    
//        /* 更新字典 */
//        pdf_dict_dels(obj, "Filter");
//        pdf_dict_puts_drop(obj, "Length", pdf_new_int(doc, strlen(new_stream)));
//        
//        /* 更新docset */
//        pdf_update_docset(doc, obj, 3, NULL,"r=",rgb[0],"g=",rgb[1],"b=",rgb[2]);
//        
//        fz_drop_buffer(ctx, old_buffer);
//    }
}

/* pdf_set_watermark_opacity：设置水印透明度
 * 参数：
 *   watermark   要修改的水印
 *   opacity     不透明度 */
void
pdf_set_watermark_opacity(pdf_document *doc, pdf_watermark *watermark, float opacity)
{
    pdf_obj *obj;
    pdf_obj *dict;
    
    fz_context *ctx = doc->ctx;
    
    pdf_obj *gs = watermark->ExtGState;
    pdf_obj *xobj = watermark->XObject;
    
    int gs_len = pdf_array_len(gs);
    for (int i=0; i<gs_len; i++)
    {
        dict  = pdf_dict_get_val(pdf_array_get(gs, i), 0);
        
        fz_try(ctx)
        {
            obj = pdf_new_real(doc, opacity);
            pdf_dict_puts(dict, "CA", obj);
            pdf_drop_obj(obj);
            obj = NULL;
            
            obj = pdf_new_real(doc, opacity);
            pdf_dict_puts(dict, "ca", obj);
            pdf_drop_obj(obj);
            obj = NULL;
        }
        fz_catch(ctx)
        {
            pdf_drop_obj(obj);
        }
    }
    
    int xobj_len = pdf_array_len(xobj);
    for (int i=0; i<xobj_len; i++)
    {
        dict  = pdf_dict_get_val(pdf_array_get(xobj, i), 0);
        pdf_update_docset(doc, dict, 1, NULL,"Opacity",opacity);
    }
}

/* pdf_update_watermark_martrix: 用传入的mat更新水印的转换矩阵
 */
static void
pdf_update_watermark_martrix(pdf_document *doc, pdf_watermark *watermark, fz_matrix *mat, int method)
{
    fz_context *ctx = doc->ctx;
    pdf_obj *w_cont = watermark->content;
    pdf_obj *w_xobj = watermark->XObject;
    
    int cont_len = pdf_array_len(w_cont);
    for (int i=0; i<cont_len; i++)
    {
        pdf_obj *content = pdf_array_get(w_cont, i);
        
        fz_buffer *old_buff = pdf_load_stream(doc, pdf_to_num(content), pdf_to_gen(content));
        fz_buffer *new_buff;
        
        char *stream = (char *)old_buff->data;
        stream[old_buff->len] = '\0';
        
        /* new_stream要传入到fz_buffer当中，由fz_buffer释放 */
        char *new_stream = malloc(sizeof(char) * strlen(stream));
        memset(new_stream, 0, sizeof(char) * strlen(stream));
        
        char *trans_stream = malloc(sizeof(char) * strlen(stream));
        memset(new_stream, 0, sizeof(char) * strlen(stream));
        
        int xobj_len = pdf_array_len(w_xobj);
        for (int j=0; j<xobj_len; j++)
        {
            pdf_obj *xobj = pdf_dict_get_key(pdf_array_get(w_xobj, j), 0);
            /* 找出指定水印的流 */
            char *find_stream = find_watermark_from_stream(doc, xobj, stream);
            
            if (find_stream == NULL)
                continue;
            
            /* 从流当中解析出原来的矩阵 */
            fz_stream *s = fz_open_memory(ctx, (unsigned char*)find_stream, strlen(find_stream));
            watermark_attribute *wa = fz_malloc_struct(ctx, watermark_attribute);
            watermark_parse_content_stream(watermark, s, wa);
            
            /* 对矩阵进行变换 */
            /* 矩阵相乘的时候，旋转，缩放，原矩阵在后； 平移，原矩阵在前*/
            switch (method)
            {
                case PDF_WATERMARK_ROTATE:
                    fz_concat(&wa->mat, mat, &wa->mat);
                    break;
                case PDF_WATERMARK_SCALE:
                    fz_concat(&wa->mat, mat, &wa->mat);
                    break;
                case PDF_WATERMARK_TRANSLATE:
                    fz_concat(&wa->mat, &wa->mat, mat);
                    break;
                default:
                    break;
            }
            
            /* 用变换过的矩阵重新生成流 */
            char *new_c = pdf_update_content_stream(wa->mat, wa->gs_name, wa->xobj_name);
            strcat(trans_stream, new_c);
            
            /* 用完就释放 */
            fz_free(ctx, wa);
            wa = NULL;
            free(new_c);
            new_c = NULL;
            free(find_stream);
            find_stream = NULL;
            fz_close(s);
            s = NULL;
            break;
        }
        
        /* 得到删除指定水印的流后的字符串 */
        char *del_stream = delete_watermark_from_stream(doc, w_xobj, stream);
        
        /* 如果有一个为空，那么就当做删除失败，保留原来的流
         * 否则 将新的流与删除水印部分的流重新拼接 */
        if (trans_stream == NULL || del_stream == NULL)
            strcpy(new_stream, stream);
        else
        {
            strcat(new_stream, del_stream);
            strcat(new_stream, trans_stream);
        }
        
        /* 更新流 */
        new_buff = fz_new_buffer_from_data(ctx, (unsigned char*)new_stream, strlen(new_stream));
        pdf_update_stream(doc, pdf_to_num(content), new_buff);
        fz_drop_buffer(ctx, new_buff);
        
        /* 更新字典 */
        pdf_dict_puts_drop(content, "Length", pdf_new_int(doc, strlen(new_stream)));
        pdf_dict_dels(content, "Filter");
        
        free(del_stream);
        del_stream = NULL;
        free(trans_stream);
        trans_stream = NULL;
        fz_drop_buffer(ctx, old_buff);
        old_buff = NULL;
    }
}

/* pdf_set_watermark_rotate: 设置水印的旋转 */
void
pdf_set_watermark_rotate(pdf_document *doc, pdf_watermark *watermark, float degrees)
{
    pdf_obj *xobj = watermark->XObject;
    
    /* 转换矩阵实现旋转 */
    fz_matrix mat;
    fz_rotate(&mat, degrees);
    pdf_update_watermark_martrix(doc, watermark, &mat, PDF_WATERMARK_ROTATE);
    
    /* 设置docset以实现与ADOBE的同步 */
    int xobj_len = pdf_array_len(xobj);
    for (int i=0; i<xobj_len; i++)
    {
        pdf_obj *obj  = pdf_dict_get_val(pdf_array_get(xobj, i), 0);
        pdf_update_docset(doc, obj, 1, NULL,"Rotation",degrees);
    }
}

/* pdf_set_watermark_translate: 设置水印的平移 */
void
pdf_set_watermark_translate(pdf_document *doc, pdf_watermark *watermark, float tx, float ty)
{
    fz_matrix mat;
    fz_translate(&mat, tx, ty);
    pdf_update_watermark_martrix(doc, watermark, &mat, PDF_WATERMARK_TRANSLATE);
    
    /* FIXME:还要设置docset */
}

/* pdf_set_watermark_scale: 设置水印的缩放 */
void
pdf_set_watermark_scale(pdf_document *doc, pdf_watermark *watermark, float sx, float sy)
{
    fz_matrix mat;
    fz_scale(&mat, sx, sy);
    pdf_update_watermark_martrix(doc, watermark, &mat, PDF_WATERMARK_SCALE);
    
    /* FIXME:还要设置docset */
}

/* pdf_first_watermark:
 * 功能：返回第一个水印 */
pdf_watermark *
pdf_first_watermark(pdf_document *doc)
{
    return doc->watermarks ? doc->watermarks->next : NULL;
}

/* pdf_next_watermark:
 * 功能：返回水印的下一个水印 */
pdf_watermark *
pdf_next_watermark(pdf_document *doc, pdf_watermark *watermark)
{
    return watermark ? watermark->next : NULL;
}

/* pdf_next_watermark:
 * 功能：返回水印的XObject */
pdf_obj *
pdf_get_watermark_xobject(pdf_document *doc, pdf_watermark *watermark)
{
    return watermark ? watermark->XObject : NULL;
}

/* pdf_next_watermark:
 * 功能：返回水印的ExtGState */
pdf_obj *
pdf_get_watermark_extgstate(pdf_document *doc, pdf_watermark *watermark)
{
    return watermark ? watermark->ExtGState : NULL;
}

/* pdf_next_watermark:
 * 功能：返回水印的content */
pdf_obj *
pdf_get_watermark_content(pdf_document *doc, pdf_watermark *watermark)
{
    return watermark ? watermark->content : NULL;
}
