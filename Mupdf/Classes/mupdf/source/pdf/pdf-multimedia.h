//
//  pdf-multimedia.h
//  MuPDF
//
//  Created by SuYuanCheng on 14-1-6.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#ifndef MuPDF_pdf_multimedia_h
#define MuPDF_pdf_multimedia_h
#include"mupdf/pdf.h"
/*
 * Set the multimedia apprence image
 */
void pdf_set_multimedia_ap(char *Ap);
/*
 * add multimedia to a pdf file
 * param mediafile :media file, such as: mp3, wav ,mp4 ,mov, and so on .
 */
pdf_document * pdf_add_multimedia(pdf_document * doc, int page , fz_rect location,char *mediafile);

/*
 *get the video annotation
 *param page: the page which to check
 *param point: the point get from tap event, that scaled to pdf space
 */
pdf_annot *pdf_get_multimedia_annot(pdf_document *doc,int page,fz_point point);

/*get the video file data, and use the data to play in objective-C
 *param page: the annot belongs to the page
 *param sound: ensure the annotation is sound annotation
 */
unsigned char *pdf_get_multimedia_data(pdf_document *doc,pdf_annot *mutimedia,int *length,fz_rect *rect);
#endif
