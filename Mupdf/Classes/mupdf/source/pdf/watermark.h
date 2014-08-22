//
//  watermark.h
//  MuPDF
//
//  Created by SuYuanCheng on 13-8-21.
//  Copyright (c) 2013年 Artifex Software, Inc. All rights reserved.
//
#include "mupdf/pdf.h"
#ifndef MuPDF_watermark_h
#define MuPDF_watermark_h

#define ALL -1//add watermark to all pages 

pdf_watermark *pdf_create_watermark_with_img(pdf_document *doc,int pageNum,const char *imgName,const float imgWidth,const float imgHeight,const fz_rect *bbox);

void pdf_update_watermark_with_img(pdf_document *doc,int pageNum,const char *imageName,const float imgWidth,const float imgHeight);
pdf_watermark *pdf_create_watermark_with_string(pdf_document *doc, char *text, fz_rect rect);
#endif
