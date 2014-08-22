//
//  pdf-video.h
//  MuPDF
//
//  Created by batman on 13-12-25.
//  Copyright (c) 2013年 Artifex Software, Inc. All rights reserved.
//

#ifndef MuPDF_pdf_video_h
#define MuPDF_pdf_video_h

#include"mupdf/pdf.h"
/*
 *add a video rich media to a page
 *param doc: the document which the annot be added to
 *param page: the page number
 *param location: the point from screen, which is scaled to pdf space
 *param audiofile: the rich media source name
 */
pdf_document * pdf_add_video(pdf_document *doc,int page,fz_point location,char *audiofile);

/*
 *get the video annotation
 *param page: the page which to check
 *param point: the point get from tap event, that scaled to pdf space
 */
pdf_annot *pdf_get_video_annot(pdf_document *doc,int page,fz_point point);

/*get the video file data, and use the data to play in objective-C
 *param page: the annot belongs to the page
 *param sound: ensure the annotation is sound annotation
 */
unsigned char *pdf_get_video_data(pdf_document *doc ,pdf_annot *sound,int *length);
void set_video_path(char *flashPath);
void set_video_image_path(char *soundImagePath);
void set_SkinOverAllNoFullNoCaption_path(char *SkinPath);

#endif
