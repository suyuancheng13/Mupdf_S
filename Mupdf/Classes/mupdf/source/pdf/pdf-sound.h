//
//  pdf_sound.h
//  MuPDF
//
//  Created by SuYuanCheng on 13-12-4.
//  Copyright (c) 2013年 Artifex Software, Inc. All rights reserved.
//

#ifndef MuPDF_pdf_sound_h
#define MuPDF_pdf_sound_h

#include"mupdf/pdf.h"
/*
 *add a sound rich media to a page
 *param doc: the document which the annot be added to
 *param page: the page number
 *param location: the point from screen, which is scaled to pdf space
 *param audiofile: the rich media source name
 */
pdf_document * pdf_add_sound(pdf_document *doc,int page,fz_point location,char *audiofile);

/*
 *get the sound annotation 
 *param page: the page which to check
 *param point: the point get from tap event, that scaled to pdf space
 */
pdf_annot *pdf_get_sound_annot(pdf_document *doc,int page,fz_point point);

/*get the sound file data, and use the data to play in objective-C
 *param page: the annot belongs to the page
 *param sound: ensure the annotation is sound annotation
 */
unsigned char *pdf_get_sound_data(pdf_document *doc ,pdf_annot *sound,int *length);
void set_flash_path(char *flashPath);
void set_sound_image_path(char *soundImagePath);

//for test
int mmain(int argc, char **argv);
#endif
