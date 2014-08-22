//
//  pdf-multimedia.c
//  MuPDF
//
//  Created by SuYuanCheng on 14-1-6.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#include <stdio.h>
#include"pdf-multimedia.h"
#include<zlib.h>


static char * multimedia_ap;

typedef struct fz_arc4c_s fz_arc4c;

struct fz_arc4c_s
{
	fz_stream *chain;
	fz_arc4 arc4;
};

void pdf_set_multimedia_ap(char *Ap)
{
    if(NULL == Ap)
        return ;
    char *p = strstr(Ap,".jpg");
    p+=4;
    p='\0';
    multimedia_ap = (char *)malloc(sizeof(char)*strlen(Ap)+1);
    strncpy(multimedia_ap,Ap,strlen(Ap));
   *( multimedia_ap+strlen(Ap)) = '\0';
    return;
}
static
void compressStream(fz_buffer *buf,fz_context *ctx)
{
	int code = Z_OK;
    z_stream zp;
    int len =buf->len;
    
    /*
     由于压缩函数在处理较短的stream时，会变得更长。否则会出现Z_BUF_ERROR或者会在后面的read中出现Z_DATA_ERROR问题
     */
    if(len < 2046)
    {
        len = 2046;
    }
    unsigned char *out = (unsigned char *)malloc((len)*sizeof(unsigned char ));
    unsigned char *header = out;
	fz_var(code);
    
    //initial the z_stream
	fz_try(ctx)
	{
		zp.zalloc = Z_NULL;
		zp.zfree = Z_NULL;
		zp.opaque = ctx;
		zp.next_in = NULL;
		zp.avail_in = 0;
        
		code = deflateInit(&zp,Z_DEFAULT_COMPRESSION);
		if (code != Z_OK)
			fz_throw(ctx, FZ_ERROR_GENERIC, "zlib error: inflateInit: %s", zp.msg);
	}
	fz_catch(ctx)
	{
		if ( code == Z_OK)
			deflateEnd(&zp);
		fz_rethrow(ctx);
	}
	zp.next_out = out;//the result of compress the stream
    zp.next_in = buf->data;//the stream to compress
    zp.avail_in = buf->len;//the length of the stream
    zp.avail_out = buf->len;
    {
        //compress the stream
        code = deflate(&zp, Z_SYNC_FLUSH);
        
        if (code == Z_STREAM_END)
        {
            return ;
        }
        else if (code == Z_BUF_ERROR)
        {
            fz_warn(ctx, "premature end of data in flate filter");
        }
        else if (code == Z_DATA_ERROR && zp.avail_in == 0)
        {
            fz_warn(ctx, "ignoring zlib error: %s", zp.msg);
        }
        else if (code != Z_OK)
        {
            fz_throw(ctx, FZ_ERROR_GENERIC, "zlib error: %s", zp.msg);
        }
    }
    if(buf->len < zp.total_out)
    {
        fz_resize_buffer(ctx, buf, zp.total_out);
    }
    memcpy(buf->data, header,zp.total_out);
    buf->len = buf->cap=zp.total_out;
    free(out);
    return ;
}
static
void pdf_utf82str(char *dest,char *utf8,int n)
{
    int retval;
    for (retval = 0; n > 0; n -= 1, retval += 1) {
        if(utf8)
            *dest=*utf8;
        utf8=utf8+1;
        if(!*utf8)
        {
            utf8++;
            n--;
        }
        dest=dest+1;
    }
    *dest = '\0';
    return retval;
}

static
int create_xobject_img(pdf_document *doc,const char *imgName,const float width,const float height)
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
        
        //cypt the stream
        if(doc->crypt)
        {
            fz_stream *stream1 =  pdf_open_crypt(doc->file, doc->crypt, img, 0);
            fz_arc4_encrypt(&((fz_arc4c *)(stream1->state))->arc4, imgBuffer->data, imgBuffer->data,imgBuffer->len);
        }
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
static
pdf_obj * create_extgstate(pdf_document *doc, float opacity)
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
/*
 创建一个form
 img:图片xobject的num
 */
static
int create_xobject_form(pdf_document *doc,int img,const fz_matrix matrix,const fz_rect *bbox)
{
    
    pdf_obj *xobject =  pdf_new_xobject(doc,bbox, &matrix);
    pdf_obj *xoo = pdf_load_object(doc, pdf_to_num(xobject), 0);//the xobject's dictionary
    
    
    //add the graphic state
    pdf_obj *res = pdf_new_dict(doc, 3);
    pdf_obj *extgstate = create_extgstate(doc, 1);
    pdf_obj * GSW = pdf_new_dict(doc, 1);
    pdf_dict_putp_drop(GSW, "GSW", extgstate);
    pdf_dict_putp_drop(res, "ExtGState", GSW);
    
    // the doc settings
    pdf_obj *procSet = pdf_new_array(doc, 2);
    pdf_array_push_drop(procSet, pdf_new_name(doc, "PDF"));
    pdf_array_push_drop(procSet, pdf_new_name(doc, "ImageC"));
    pdf_dict_putp_drop(res, "ProcSet", procSet);
    pdf_obj *ximg = pdf_new_dict(doc, 1);
    pdf_dict_putp_drop(ximg, "Img0", pdf_new_indirect(doc, img, 0));
    pdf_dict_putp_drop(res, "XObject", ximg);
    pdf_dict_putp_drop(xoo,"Resources", res);
    
    pdf_obj *imga = pdf_load_object(doc, img, 0);
    float with = pdf_to_real(pdf_dict_gets(imga,  "Width"));
    float height = pdf_to_real(pdf_dict_gets(imga, "Height"));
    
    char *data = (malloc(100*sizeof(char)));
    sprintf(data, "%s","q\n");
    
    char *w = (char *)malloc(10*sizeof(char));
    sprintf(w, "%.1f ",with);
    strcat(data, w);
    free(w);
    strcat(data,"0 0 ");
    
    char *h = (char *)malloc(10*sizeof(char));
    sprintf(h, "%0.1f ",height);
    strcat(data, h);
    free(h);
    char *b = (char*)malloc(20*sizeof(char));
    sprintf(b, "%0.1f %0.1f ",bbox->x0,bbox->y0);//the location of the form
    strcat(data, b);
    free(b);
    
    strcat(data,"cm\n/GSW gs\n/Img0 Do\nQ\n");
    
    fz_buffer *buffer = fz_new_buffer_from_data(doc->ctx,( unsigned char *)data , strlen(data));
    
    //cypt the stream
    if(doc->crypt)
    {
        fz_stream *stream1 =  pdf_open_crypt(doc->file, doc->crypt, pdf_to_num(xobject), 0);
        fz_arc4_encrypt(&((fz_arc4c *)(stream1->state))->arc4, buffer->data, buffer->data,buffer->len);
    }
    pdf_update_xobject_contents(doc, pdf_load_xobject(doc, xobject), buffer);
    return pdf_to_num(xobject);
    
}
static
pdf_obj*set_multimedia(pdf_document *doc,pdf_annot *annot,char *mediafile)
{
    fz_try(doc->ctx)
    {
        fz_buffer *buffer=NULL;
        fz_stream *filestream=NULL;
        char *name = rindex(mediafile, '/');
        char *format = rindex(mediafile, '.');
        format ++;
        name ++;
        
        /*step 1； file specification of the embed file*/
        int file = pdf_create_object(doc);
        pdf_obj*file_o = pdf_new_dict(doc, 2);
        filestream =fz_open_file(doc->ctx, mediafile);
        buffer = fz_read_all(filestream, 1024);
       
        compressStream(buffer, doc->ctx);
        
        if(doc->crypt)
        {
            fz_stream *stream1 =  pdf_open_crypt(doc->file, doc->crypt, file, 0);
            fz_arc4_encrypt(&((fz_arc4c *)(stream1->state))->arc4, buffer->data, buffer->data,buffer->len);
        }
        pdf_dict_putp_drop(file_o, "Filter", pdf_new_name(doc, "FlateDecode"));
        pdf_dict_putp_drop(file_o,"Length",pdf_new_int(doc, buffer->len));
        pdf_update_object(doc, file, file_o);
        pdf_update_stream(doc, file, buffer);
        fz_drop_buffer(doc->ctx, buffer);
    
        
        int filespec = pdf_create_object(doc);
        pdf_obj *filespec_o = pdf_new_dict(doc, 4);
        pdf_dict_putp_drop(filespec_o, "Type", pdf_new_name(doc, "Filespec"));
        pdf_dict_putp_drop(filespec_o, "F", pdf_new_string(doc, name, strlen(name)));
        pdf_dict_putp_drop(filespec_o, "UF", pdf_new_string(doc, name, strlen(name)));
        pdf_dict_putp_drop(filespec_o, "EF/F", pdf_new_indirect(doc, file, 0));
        pdf_update_object(doc, filespec, filespec_o);
        
        if(doc->crypt)
        {
            pdf_crypt_obj(doc->ctx, doc->crypt, filespec_o, filespec, 0);
        }
        /*step 2 ; set the media clip data*/
        int mediaclip = pdf_create_object(doc);
        pdf_obj *mediaclip_o = pdf_new_dict(doc, 6);
        pdf_dict_putp_drop(mediaclip_o, "Type", pdf_new_name(doc, "MediaClip"));
        pdf_dict_putp_drop(mediaclip_o, "S", pdf_new_name(doc, "MCD"));
        pdf_dict_putp_drop(mediaclip_o, "N", pdf_new_string(doc, name, strlen(name)));
        /*
         format of media, mp3 ,mp4 mpeg
         */
        if(!strcasecmp("mp3", format))
        {
            pdf_dict_putp_drop(mediaclip_o, "CT", pdf_new_string(doc, "audio/mp3", 9));
        }
        if (!strcasecmp("wav", format)) {
            pdf_dict_putp_drop(mediaclip_o, "CT", pdf_new_string(doc, "audio/wav", 9));
        }
        if (!strcasecmp("mp4", format)) {
            pdf_dict_putp_drop(mediaclip_o, "CT", pdf_new_string(doc, "video/mp4", 9));
        }
        if (!strcasecmp("mov", format)) {
            pdf_dict_putp_drop(mediaclip_o, "CT", pdf_new_string(doc, "video/quicktime", 15));
        }
        
        pdf_dict_putp_drop(mediaclip_o, "P/TF", pdf_new_string(doc, "TEMPACCESS", 10));
        pdf_dict_putp_drop(mediaclip_o, "D", pdf_new_indirect(doc, filespec, 0));
        pdf_update_object(doc, mediaclip, mediaclip_o);
        if(doc->crypt)
        {
            pdf_crypt_obj(doc->ctx, doc->crypt, mediaclip_o, mediaclip, 0);
        }
        /*step 3: set the rendition dictionary*/
        int rendition = pdf_create_object(doc);
        pdf_obj *rendition_o = pdf_new_dict(doc, 4);
        pdf_dict_putp_drop(rendition_o, "Type", pdf_new_name(doc, "Rendition"));
        pdf_dict_putp_drop(rendition_o, "S", pdf_new_name(doc, "MR"));
        pdf_dict_putp_drop(rendition_o, "C", pdf_new_indirect(doc,mediaclip, 0));
        pdf_dict_putp_drop(rendition_o, "N", pdf_new_string(doc, name, strlen(name)));
        pdf_update_object(doc, rendition, rendition_o);
        if(doc->crypt)
        {
            pdf_crypt_obj(doc->ctx, doc->crypt, rendition_o, rendition, 0);
        }
        /*step 4: set the rendition action */
        int rendition_action = pdf_create_object(doc);
        pdf_obj *rendition_action_o = pdf_new_dict(doc ,5);
        pdf_dict_putp_drop(rendition_action_o, "Type", pdf_new_name(doc, "Action"));
        pdf_dict_putp_drop(rendition_action_o, "S", pdf_new_name(doc, "Rendition"));
        pdf_dict_putp_drop(rendition_action_o, "OP", pdf_new_int(doc, 0));
        pdf_dict_putp_drop(rendition_action_o, "AN", annot->obj);
        pdf_dict_putp_drop(rendition_action_o, "R", pdf_new_indirect(doc, rendition, 0));
        pdf_update_object(doc, rendition_action, rendition_action_o);
        
        /*step 5: set the action in scree annotation*/
        pdf_dict_putp_drop(annot->obj, "A", pdf_new_indirect(doc, rendition_action, 0));
        pdf_dict_putp_drop(annot->obj, "F", pdf_new_int(doc, 4));
        
    }
    fz_catch(doc->ctx)
    {
        fz_throw(doc->ctx,1 , "create media file failed!");
    }
    
}
static
pdf_obj *set_multimedia_apperence(pdf_document *doc, pdf_annot *annotation,fz_rect location)
{
    fz_try(doc->ctx)
    {
        fz_matrix matrix = {1,0,0,1,0,0};
        const fz_rect bbox = {0,0,32,32};
        pdf_obj *annot_obj = annotation->obj;
        
        fz_rect frect = location;
//        frect.x0 =location.x-32;
//        frect.x1=location.x+32;
//        frect.y0 = location.y-32;
//        frect.y1 = location.y+32;
        fz_transform_rect(&frect, &annotation->page->ctm);
        pdf_obj *rect = pdf_new_array(doc, 4);
        pdf_array_push_drop(rect, pdf_new_real(doc, frect.x0));
        pdf_array_push_drop(rect, pdf_new_real(doc, frect.y0));
        pdf_array_push_drop(rect, pdf_new_real(doc, frect.x1));
        pdf_array_push_drop(rect, pdf_new_real(doc, frect.y1));
        pdf_dict_putp_drop(annot_obj, "Rect", rect);
        int img = create_xobject_img(doc, multimedia_ap, 32,32);
        int form = create_xobject_form(doc,img,matrix,&bbox);
        
        pdf_dict_putp_drop(annot_obj, "AP/N", pdf_new_indirect(doc, form, 0));
    }
    fz_catch(doc->ctx)
    {
        fz_throw(doc->ctx, 1, "set apperence failed!");
    }
    
}

pdf_document * pdf_add_multimedia(pdf_document * doc, int page , fz_rect location,char *mediafile)
{
    if(page<0||pdf_count_pages(doc)<=page)
        return doc;
    if(!doc)
        return NULL;
    if (NULL == mediafile)
        return doc;
    fz_try(doc->ctx)
    {
        printf("%s",mediafile);
        pdf_annot *multimedia_annot;
        pdf_page *Page = pdf_load_page(doc,page);
        multimedia_annot = pdf_create_annot(doc, Page, FZ_ANNOT_SCREEN);
        
        pdf_obj *page_o = doc->page_objs[page];//pdf_lookup_page_obj(doc, page);
        /* key word p is neccessary*/
        pdf_dict_putp_drop(multimedia_annot->obj, "P", page_o);
        
        /*step 1: set screen annotation apperence*/
        set_multimedia_apperence(doc,multimedia_annot,location);
        /*step 2: set multimedia*/
        set_multimedia(doc,multimedia_annot,mediafile);
        free(multimedia_ap);
        return doc;
    }fz_catch(doc->ctx)
    {
        fz_throw(doc->ctx, 1, "add multimedia failed!");
    }
    
}
pdf_annot *pdf_get_multimedia_annot(pdf_document *doc,int page,fz_point point)
{
    fz_try(doc->ctx){
        pdf_annot *annot = NULL;
        pdf_page *p_page = pdf_load_page(doc, page);
        fz_point temp_point = point;
        for (annot = p_page->annots; annot; annot = annot->next) {
            if (annot->annot_type == FZ_ANNOT_SCREEN && !strcmp("Rendition", pdf_to_name(pdf_dict_getp(annot->obj,"A/S")))) {
                point = temp_point;
                fz_transform_point(&point, &annot->page->ctm);
                if (point.x > annot->rect.x0)
                    if (point.x < annot->rect.x1)
                        if (point.y > annot->rect.y0)
                            if (point.y < annot->rect.y1)
                                return annot;
            }
        }
    }
    fz_catch(doc->ctx){
        printf("doc is error!");
    }
    return NULL;
}

unsigned char *pdf_get_multimedia_data(pdf_document *doc,pdf_annot *mutimedia,int *length,fz_rect *rect)
{
    fz_try(doc->ctx){
        if ((mutimedia->annot_type != FZ_ANNOT_SCREEN && !strcmp("Rendition", pdf_to_name(pdf_dict_getp(mutimedia->obj,"A/S"))))|| mutimedia == NULL) {
            printf("not mutimedia annotation");
            return NULL;
        }
        if(NULL != rect)
        {
            fz_rect video_rect;
            video_rect = mutimedia->rect;
            fz_transform_rect(&video_rect, &mutimedia->page->ctm);
            *rect = video_rect;
        }
        pdf_obj *mediaclip = pdf_dict_getp(mutimedia->obj, "A/R/C/D/EF/F");
        if (!mediaclip) {
            printf("not find mediastream!");
            return NULL;
        }
        int num = pdf_to_num(mediaclip);
        
        unsigned char *data = NULL;
        fz_stream *stream=NULL;
        fz_buffer *buf = NULL;
        if(!pdf_is_stream(doc, num, 0))
        {
            return  NULL;
        }
        stream = pdf_open_stream( doc,num, 0);
        buf = fz_read_all(stream, 1024);
        fz_close(stream);
        
        data = (unsigned char *)malloc((buf->len)*sizeof(unsigned char));
        memcpy(data, buf->data, buf->len);
        *length =buf->len;
        return data;
    }
    fz_catch(doc->ctx){
        printf("doc is error!"); 
    }
    return  NULL;
}
