//
//  pdf-password.h
//  MuPDF
//
//  Created by SuYuanCheng on 13-8-26.
//  Copyright (c) 2013年 Artifex Software, Inc. All rights reserved.
//
#include "mupdf/pdf.h"

#ifndef MuPDF_pdf_password_h
#define MuPDF_pdf_password_h

typedef struct fz_arc4c_s fz_arc4c;

struct fz_arc4c_s
{
	fz_stream *chain;
	fz_arc4 arc4;
};
#define MIN_BUF_LEN 1024
/*
 user permission of security handle revision 3 or greater
 */
enum
{
    NOLOWPRINT = (0xffffffff ^(1<<(3-1))),//low resolution print
    NOHIGHPRINT = (0xffffffff ^(1<<(12-1))),//high resolution print
    NOCHANGES = (0xffffffff ^(1<<(4-1))),//the content change except controlled by bit 6,9,11
    NOCOPY = (0xffffffff ^(1<<(5-1))),//copy and extract text and graphic
    NOANNOTATION = (0xffffffff ^(1<<(6-1))),//comment,fill the interactive form
    NOFILL = (0xffffffff ^(1<<(9-1))),//fill the existing interactive form and singture filed
    NOEXTRACT = (0xffffffff ^(1<<(10-1))),//extract text and graphic
    NOPAGEHANDLE = (0xffffffff ^(1<<(11-1))),//rotate page,insert page,delete page
    LOWPRINT = (1<<(3-1)),
    HIGHPRINT = (1<<(12-1)),
    CHANGES = (1<<(4-1)),
    COPY = (1<<(5-1)),
    ANNOTATION = (1<<(6-1)),
    FILL = (1<<(9-1)),
    EXTRACT = (1<<(10-1)),
    PAGEHANDLE = (1<<11-1),
    
    ALLOWALL =-4
};

/*
 * Add a password to a document .Pass "" to userPWD,if only to add permission
 *parameter ownerPWD:the owner password
 *parameter userPWD:the user password
 *parameter permission:the permission to authenticate to user
 *return value:the document that has add password 
 */
pdf_document* pdf_add_password(pdf_document *doc, char *filename,const char *ownerPWD,const char *userPWD,int permission);
/*
 *  modify the password of the document
 *
 *parameter doc:the document to modify the  password
 *parameter ownerPWD:the owner password 
 *parameter newPWD :the new user password  
 *
 */
pdf_document* pdf_modify_password(pdf_document *doc, char *filename,char *ownerPWD,char *newPWD);
/*
 * remove the password of the document
 */
pdf_document* pdf_remove_password(pdf_document *doc ,const char *filename);

/*
 *Modify the permission 
 *
 *parameter ownerPWD:the owner password (adobe need the pass word to modify the access permission )
 *parameter userPWD :the old user password  (cause the key that encrypt the user password will change)
 *parameter newPermission :the new permission to authenticate
 
 *I注意的是，当要删除某一些权限时多个权限之间要采用'&'运算，但是要增加的时候要采用‘|’运算，不可以同时进行删除与增加。
 */
pdf_document* pdf_modify_permission(pdf_document *doc, char *filename,char *ownerPWD,char *userPWD,int newPermission);


/*
 *get the permission of the document
 *return value: 0--allow the action  1--doesn't allow the action
 */
int pdf_allow_high_print(pdf_document *doc);
int pdf_allow_low_print(pdf_document *doc);
int pdf_allow_content_changes(pdf_document *doc);
int pdf_allow_copy(pdf_document *doc);
int pdf_allow_add_annotation(pdf_document *doc);
int pdf_allow_fill_form(pdf_document *doc);
int pdf_allow_extract(pdf_document *doc);
int pdf_allow_page_handle(pdf_document *doc);//rotate,insert,delete page

int pdf_has_encrypt(pdf_document *doc);
//compress function
void compressStream(fz_buffer *buf,fz_context *ctx);
void compress_with_dct(fz_buffer *buf,fz_context *ctx,int width,int height);
#endif
