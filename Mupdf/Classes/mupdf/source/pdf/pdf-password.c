//
//  pdf-password.c
//  MuPDF
//
//  Created by SuYuanCheng on 13-8-26.
//  Copyright (c) 2013年 Artifex Software, Inc. All rights reserved.
//
#include "mupdf/fitz.h"
#include <stdio.h>
#include <zlib.h>
#include "jpeglib.h"
#include "pdf-password.h"


#define TEMP_FILE "temp.pdf"
char *file =NULL;
typedef struct fz_dctd_s fz_dctd;

struct fz_dctd_s
{
	fz_stream *chain;
	fz_context *ctx;
	int color_transform;
	int init;
	int stride;
	int l2factor;
	unsigned char *scanline;
	unsigned char *rp, *wp;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_source_mgr srcmgr;
	struct jpeg_error_mgr errmgr;
	jmp_buf jb;
	char msg[JMSG_LENGTH_MAX];
};
struct pdf_crypt_s
{
	pdf_obj *id;//the id entry on the trailer dictionary
    
	int v;// the encrypt algorithm version
	int length;//the encrypt key length
	pdf_obj *cf;//
	pdf_crypt_filter stmf;
	pdf_crypt_filter strf;
    
	int r;
	unsigned char o[48];//owner password
	unsigned char u[48];//user password
	unsigned char oe[32];
	unsigned char ue[32];
	int p;//
	int encrypt_metadata;
    
	unsigned char key[32]; /* decryption key generated from password */
	fz_context *ctx;
};

int ADDPASS = 0;

/*
 *compress the image with DCTDecode filter
 */

void compress_with_dct(fz_buffer *buf,fz_context *ctx,int width,int height)
{
    
    unsigned char *outstream = (unsigned char *)malloc((buf->len)*sizeof(unsigned char));
    struct jpeg_compress_struct jcs;
    struct jpeg_error_mgr jem;
    jcs.err = jpeg_std_error(&jem);
    
    jpeg_create_compress(&jcs);
    unsigned long len = 0;//压缩了的数据长度
    jpeg_mem_dest (&jcs,&outstream, &len);//设置内存buffer，存储压缩了的流
    
    jcs.input_components =3;
    jcs.in_color_space = JCS_RGB;
    jcs.image_height = height;
    jcs.image_width = width;
    
    jpeg_set_defaults(&jcs);
    jpeg_set_quality (&jcs, 80, TRUE);
    
    jpeg_start_compress(&jcs, TRUE);
    JSAMPROW row_pointer[1];	/* pointer to a single row */
	int row_stride;			/* physical row width in buffer */
    
	row_stride = jcs.image_width * 3;	/* JSAMPLEs per row in image_buffer */
    //开始压缩
	while (jcs.next_scanline < jcs.image_height) {
	    row_pointer[0] = &buf->data[jcs.next_scanline * row_stride];
	    jpeg_write_scanlines(&jcs, row_pointer, 1);
	}
    
    jpeg_finish_compress(&jcs);
    jpeg_destroy_compress(&jcs);
    
    //存储压缩了的数据
    memcpy(buf->data, outstream,len );
    buf->len = len;
    free(outstream);
    return ;
}
/*
 *Compress the stream
 *just handle the flatedecode filter
 */

void compressStream(fz_buffer *buf,fz_context *ctx)
{
	int code = Z_OK;
    z_stream zp;
    int len =buf->len;
    
    /*
     由于压缩函数在处理较短的stream时，会变得更长。否则会出现Z_BUF_ERROR或者会在后面的read中出现Z_DATA_ERROR问题
     */
    if(len < MIN_BUF_LEN)
    {
        len = MIN_BUF_LEN;
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
    deflateEnd(&zp);
    free(out);
    return ;
}

/*
 write back to self
 */
static
void save(pdf_document *doc,char *filename)
{
    fz_write_options ops;
    int erro = 0 ;
    ops.do_incremental = 0;
    ops.continue_on_error = 1;
    ops.do_linear = 0;
    ops.do_garbage = 0;
    ops.do_expand = 0 ;
    ops.do_ascii = 0 ;
    ops.errors = &erro;
    pdf_write_document(doc, filename, &ops);
}
/*
 * handle the file with object stream in 
 *将文件写出去（mupdf 将其线性化），再读进来以便进行加密
 */
static
pdf_document *linearize_file(pdf_document *doc,char *filename)
{
    fz_write_options ops;
    pdf_document *tdoc;
    file = (char *)malloc(strlen(filename)*sizeof(char));
    strncpy(file, filename, strlen(filename));
    int erro = 0 ;
    ops.do_incremental = 0;
    ops.continue_on_error = 1;
    ops.do_linear = 0;
    ops.do_garbage = 0;
    ops.do_expand = 0 ;
    ops.do_ascii = 0 ;
    ops.errors = &erro;
    
    char *temp = rindex(file, '/');
    strncpy(temp+1, TEMP_FILE,strlen(TEMP_FILE));
    *(temp+1+strlen(TEMP_FILE))='\0';
    pdf_write_document(doc, file, &ops);
    tdoc = pdf_open_document(doc->ctx, file);
    //free(file);
    return tdoc;
}
/*
 the process to add password or permission
 */
static
pdf_document * password_or_permission(pdf_document *doc,char *filename,const char *ownerPWD,const char* userPWD,int permission)
{
    if(NULL != pdf_dict_gets(pdf_trailer(doc), "Encrypt"))
        return doc;//has password
    
    if(!ADDPASS)
    {
        /*
         TODO:judge the doc is a linear doc or not,if not then do linearize the file
         */
        doc = linearize_file(doc, filename);
        remove(TEMP_FILE);
        ADDPASS = 0;
    }
    
    unsigned char OPwd[32] ;
    unsigned char UPwd[32];
    unsigned char key[32];
    int count = pdf_count_objects(doc);
    pdf_xref_entry *x;
    
    pdf_obj *trailer = pdf_trailer(doc);
    pdf_obj *encrypt = pdf_new_dict(doc, 10);
    
    pdf_dict_putp_drop(encrypt, "R", pdf_new_int(doc, 3));
    pdf_dict_putp_drop(encrypt, "P", pdf_new_int(doc, permission));
    pdf_dict_putp_drop(encrypt, "Filter", pdf_new_name(doc, "Standard"));
    pdf_dict_putp_drop(encrypt, "Length", pdf_new_int(doc, 128));
    pdf_dict_putp_drop(encrypt, "V", pdf_new_int(doc, 2));
    
    pdf_crypt *crypt = fz_malloc_struct((doc->ctx), pdf_crypt);
    crypt->r =3;//revision
    crypt->p = permission;//permission
    crypt->length = 128;
    crypt->v = 2;//version
    
    pdf_obj *id = pdf_dict_gets(trailer, "ID");
    if (pdf_is_array(id) && pdf_array_len(id) == 2)//document id :only need the first one
	{
		pdf_obj *obj = pdf_array_get(id, 0);
		if (pdf_is_string(obj))
			crypt->id = pdf_keep_obj(obj);
	}
    else if(NULL == id)//id is required in encrypt document
    {
        pdf_obj *id1 = pdf_new_string(doc,"createbysuyuancheng313forencrypt",32);
        pdf_obj *id2 = pdf_new_string(doc,"createbysuyuanchengforencrypt313",32);
        id = pdf_new_array(doc, 2);
        pdf_array_push_drop(id, id1);
        pdf_array_push_drop(id, id2);
        pdf_dict_putp_drop(trailer, "ID", id);
        crypt->id = pdf_keep_obj(id1);
    }
    doc->crypt = crypt;
    
    /*
     step 1 :CREATE the owner password
     */
    pdf_create_owner_password(doc, ownerPWD, userPWD, OPwd);
    pdf_dict_puts_drop(encrypt, "O", pdf_new_string(doc, (const char *)(doc->crypt->o), 32));
    pdf_obj * o = pdf_dict_gets(encrypt, "O");
    memcpy(doc->crypt->o, pdf_to_str_buf(o), 32);
    
    /*
     step 2:CREATE the user password
     */
    pdf_create_user_password(doc->ctx, doc->crypt, userPWD, strlen(userPWD), UPwd);
    pdf_dict_puts_drop(encrypt, "U", pdf_new_string(doc, (const char *)UPwd, 32));
    
    /*
     step 3:new a crypt object
     */
    memcpy(key, doc->crypt->key, 32);
    doc->crypt = NULL;
    doc->crypt = crypt = pdf_new_crypt(doc->ctx, encrypt, id);
    memcpy(doc->crypt->key, key, 32);
    
    /*
     step 4:compress and encrypt the content stream
     */
    for(int i = 1 ;i < count;i++)
    {
        int dct =0;
        fz_try(doc->ctx)
        {
            doc->crypt = NULL;//处理outlines 变乱码，为了在pdf_load_object里不对object进行加密，在后面的pdf_crypt_obj进行加密
            /*handle some file not start from 0 object */
            fz_try(doc->ctx)
            {
                pdf_load_object(doc, i, 0);////如果是加了密码的文档，在这个加载过程中会对其进行加密
            }
            fz_catch(doc->ctx)
            {
                continue;
            }
            
            x =  pdf_get_xref_entry(doc, i);
            
            if(pdf_is_stream(doc, i, 0))
            {
                //step 1:get the raw stream
                doc->crypt = NULL;
                fz_stream *stream = pdf_open_stream( doc,i, 0);
                fz_buffer *buf = fz_read_all(stream, 1024);
                
                //step 2:compress the stream
                if(NULL != buf->data)
                {
                    if(strcmp("DCTDecode",pdf_to_name( pdf_dict_gets(x->obj,"Filter"))))
                    {
                        printf("");//这一句必须要，否则数据就没有，原因还不知道
                        compressStream(buf, doc->ctx);
                    }
                    else
                    {
                        compress_with_dct(buf, doc->ctx, pdf_to_int(pdf_dict_gets(x->obj, "Width")),pdf_to_int(pdf_dict_gets(x->obj, "Height")));
                        dct = 1;
                    }
                }
                
                //step 3:encrypt the stream
                doc->crypt  = crypt ;
                fz_stream *stream1 =  pdf_open_crypt(doc->file, doc->crypt, i, 0);
                fz_arc4_encrypt(&((fz_arc4c *)(stream1->state))->arc4, buf->data, buf->data,buf->len);
                
                //step 4:update stream and object
                pdf_delete_object(doc, i);
                pdf_crypt_obj(doc->ctx, doc->crypt, x->obj, i, 0);
                pdf_dict_puts_drop(x->obj,"Length", pdf_new_int(doc,buf->len));
                if(!dct)
                {
                    pdf_dict_puts_drop(x->obj,"Filter", pdf_new_name(doc,"FlateDecode"));
                }
                /*
                 处理一些图片会显示不出， 在后续的版本里还要完善的
                 */
                if(NULL != pdf_dict_gets(x->obj, "DecodeParms"))
                {
                    pdf_dict_dels(x->obj, "DecodeParms");
                }
                
                pdf_update_object(doc, i, x->obj);
                pdf_update_stream(doc,i,buf);
                
                fz_drop_buffer(doc->ctx, buf);
                fz_close(stream);
            }
            else
            {
                doc->crypt = crypt;
                pdf_crypt_obj(doc->ctx, doc->crypt, x->obj, i, 0);
                pdf_update_object(doc, i, x->obj);
            }
        }
        fz_catch(doc->ctx)
        {
            fz_throw(doc->ctx, 1, "compress or encrypt content stream failed!");
        }
    }
    fz_free(doc->ctx, crypt);
    /*
     step 5:set encrypt entry into the trailer of the file
     */
    pdf_dict_putp_drop(trailer, "Encrypt", pdf_new_ref(doc, encrypt));
    doc->xref_sections[0].trailer = trailer;
    
    /*
     reopen the file with the password（there is some problem when save out of the file）
     */
    save(doc,filename);
    doc = pdf_open_document(doc->ctx, filename);
    fz_authenticate_password((fz_document *)doc, userPWD);
    char *file = (char *)malloc(strlen(filename)*sizeof(char));
    strncpy(file, filename, strlen(filename));
    
    char *temp = rindex(file, '/');
    strncpy(temp+1, TEMP_FILE,strlen(TEMP_FILE));
    *(temp+1+strlen(TEMP_FILE))='\0';
    remove(file);
    free(file);
    return doc;
    
}
/*
 * Add a password to a document
 *parameter ownerPWD:the owner password
 *parameter userPWD:the user password
 *parameter permission:the permisison to authenticate to user
 *return value :pdf_document
 */
pdf_document * pdf_add_password(pdf_document *doc,char *filename,const char *ownerPWD, const char *userPWD,int permission)
{
    return  password_or_permission(doc, filename,ownerPWD, userPWD, permission);
}

/*
 * Modify the password of the document
 *parameter doc:the document to modify the  password
 *parameter ownerPWD:the owner password
 *parameter newPwd:the new user password 
 */
pdf_document * pdf_modify_password(pdf_document *doc, char *filename,char *ownerPWD,char *newPWD)
{
    if(NULL == pdf_dict_gets(pdf_trailer(doc), "Encrypt"))
        return doc;//no  password to modify
    
    /*
     处理与用adobe加密等操作的兼容，因为用adobe加密的文件存在objstm之前的代码不能处理。
     */
    /*
     step 1: remove the password
     */
    pdf_obj *trailer = pdf_trailer(doc);
    pdf_obj *enctypt = pdf_dict_gets(trailer, "Encrypt");
    pdf_dict_dels(pdf_trailer(doc), "Encrypt");
    doc = linearize_file(doc, filename);
    /*
     step 2 :add new password 
     */
    ADDPASS = 1;
    return pdf_add_password(doc, filename, ownerPWD, newPWD, pdf_to_int(pdf_dict_gets(enctypt, "P")));
    
}
/*
 *  Remove the password of the document
 */
pdf_document * pdf_remove_password(pdf_document *doc, const char *filename)
{
    
    if(NULL == pdf_dict_gets(pdf_trailer(doc), "Encrypt"))
        return doc;//no  password to move
    /*
     处理与用adobe加密等操作的兼容，因为用adobe加密的文件存在objstm之前的代码不能处理。
     */
    pdf_dict_dels(pdf_trailer(doc), "Encrypt");
    doc = linearize_file(doc, filename);
    save(doc, filename);
    remove(file);
    free(file);
    doc = pdf_open_document(doc->ctx, filename);
    return doc;
    
}

/*
 * Modify the permission
 *
 *parameter OwnerPWD:the owner password （adobe need the owner password to modify the access permission）
 *parameter userPWD :the old user password (because the key that encrypt the user password will change)
 *parameter newPermission :the new permission to authenticate
 *
 * *注意的是，当要删除某一些权限时多个权限之间要采用'&'运算，但是要增加的时候要采用‘|’运算，不可以同时进行删除与增加。
 */
pdf_document* pdf_modify_permission(pdf_document *doc,char *filename,char *ownerPWD,char *userPWD,int newPermission)
{
    
    if(NULL == pdf_dict_gets(pdf_trailer(doc), "Encrypt"))
        return doc;//no  permission to modify
    
    pdf_obj *trailer = pdf_trailer(doc);
    pdf_obj *enctypt = pdf_dict_gets(trailer, "Encrypt");
    
    if(newPermission < 0)//delete permission
    {
        if(newPermission == ALLOWALL)
            doc->crypt->p = ALLOWALL;
        else
            doc->crypt->p &= newPermission;
    }
    else//add new permission
    {
        doc->crypt->p |= newPermission;
    }
    pdf_dict_putp_drop(enctypt, "P", pdf_new_int(doc, doc->crypt->p));
    doc = pdf_modify_password(doc, filename,ownerPWD,userPWD);//modify the password,because the P entry in the process of computing the key
    
    return  doc;
}
/*
 *get the permission of the document
 *return value: 0--allow the action  1--doesn't allow the action
 */
int pdf_allow_high_print(pdf_document *doc)
{
    if(NULL == pdf_dict_gets(pdf_trailer(doc), "Encrypt"))
        return 1;
    pdf_obj *trailer = pdf_trailer(doc);
    pdf_obj *enctypt = pdf_dict_gets(trailer, "Encrypt");
    int per = pdf_to_int(pdf_dict_gets(enctypt, "P"));
    if(per&((1<<(12-1))))
        if(per&((1<<(3-1))))//hight resolution 
            return 0;
    return 1;
}
int pdf_allow_low_print(pdf_document *doc)
{
    if(NULL == pdf_dict_gets(pdf_trailer(doc), "Encrypt"))
        return 1;
    pdf_obj *trailer = pdf_trailer(doc);
    pdf_obj *enctypt = pdf_dict_gets(trailer, "Encrypt");
    int per = pdf_to_int(pdf_dict_gets(enctypt, "P"));
    if(per&((1<<(3-1))))//low resolution
        return 0;
    else
        return 1;
}
int pdf_allow_content_changes(pdf_document *doc)
{
    if(NULL == pdf_dict_gets(pdf_trailer(doc), "Encrypt"))
        return 1;
    pdf_obj *trailer = pdf_trailer(doc);
    pdf_obj *enctypt = pdf_dict_gets(trailer, "Encrypt");
    int per = pdf_to_int(pdf_dict_gets(enctypt, "P"));
    if(per&(1<<(4-1)))
        return 0;
    else
        return 1;
}
int pdf_allow_copy(pdf_document *doc)
{
    if(NULL == pdf_dict_gets(pdf_trailer(doc), "Encrypt"))
        return 1;
    pdf_obj *trailer = pdf_trailer(doc);
    pdf_obj *enctypt = pdf_dict_gets(trailer, "Encrypt");
    int per = pdf_to_int(pdf_dict_gets(enctypt, "P"));
    if(per&(1<<(5-1)))
        return 0;
    else
        return 1;
    
}
int pdf_allow_add_annotation(pdf_document *doc)
{
    if(NULL == pdf_dict_gets(pdf_trailer(doc), "Encrypt"))
        return 1;
    pdf_obj *trailer = pdf_trailer(doc);
    pdf_obj *enctypt = pdf_dict_gets(trailer, "Encrypt");
    int per = pdf_to_int(pdf_dict_gets(enctypt, "P"));
    if(per&(1<<(6-1)))
        return 0;
    else
        return 1;
}
int pdf_allow_fill_form(pdf_document *doc)
{
    if(NULL == pdf_dict_gets(pdf_trailer(doc), "Encrypt"))
        return 1;
    pdf_obj *trailer = pdf_trailer(doc);
    pdf_obj *enctypt = pdf_dict_gets(trailer, "Encrypt");
    int per = pdf_to_int(pdf_dict_gets(enctypt, "P"));
    if(per&((1<<(9-1))|(1<<(6-1))))
        return 0;
    else
        return 1;
    
}
int pdf_allow_extract(pdf_document *doc)
{
    if(NULL == pdf_dict_gets(pdf_trailer(doc), "Encrypt"))
        return 1;
    pdf_obj *trailer = pdf_trailer(doc);
    pdf_obj *enctypt = pdf_dict_gets(trailer, "Encrypt");
    int per = pdf_to_int(pdf_dict_gets(enctypt, "P"));
    if(per&(1<<(10-1)))
        return 0;
    else
        return 1;
    
}
int pdf_allow_page_handle(pdf_document *doc)
{
    if(NULL == pdf_dict_gets(pdf_trailer(doc), "Encrypt"))
        return 1;
    pdf_obj *trailer = pdf_trailer(doc);
    pdf_obj *enctypt = pdf_dict_gets(trailer, "Encrypt");
    int per = pdf_to_int(pdf_dict_gets(enctypt, "P"));
    if(per&(1<<(11-1)))
        return 0;
    else
        return 1;
    
}
int pdf_has_encrypt(pdf_document*doc)
{
    if(NULL == pdf_dict_gets(pdf_trailer(doc), "Encrypt"))
        return 1;
    else {
        return 0;
    }
}
