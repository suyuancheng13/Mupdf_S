//
//  pdf_merge.h
//  MuPDF
//
//  Created by SuYuanCheng on 13-10-10.
//  Copyright (c) 2013年 Artifex Software, Inc. All rights reserved.
//

#ifndef MuPDF_pdf_merge_h
#define MuPDF_pdf_merge_h
#include "mupdf/pdf.h"
#include<stdarg.h>
/*
 *  pdf_merge_file
 *merge  files
 *parameter filename:the file that save the merged file
 *parameter count:the file count
 *parameter ...:type of the pdf_document *
 */
pdf_document * pdf_merge_file(char *filename,int count,...);
/*
 * set the author of the file
 */
pdf_document * pdf_set_author(pdf_document *doc,char *author);
/*
 * set the title of the file
 */
pdf_document * pdf_set_title(pdf_document *doc,char *title);
/*
 * save the merged file to a new file
 */
int pdf_save_merged_file(pdf_document*doc,char *filename );

pdf_document *pdf_merge_page(pdf_document *doc,pdf_document *new_doc,int pos,int page,char *savefile);

void pdf_extract_pages(pdf_document *doc,int start_page,int end_page,char *savefile);

/*
 *new method to merg file
 *param outputfile: the file which will save the final file.if the file is not exist ,the function is to split file
 *                  if the file is exist ,the function is to merge file
 *param filename: the file that to split/merged
 *param password: the pass word of the filename
 *param page: the split/merged page number
 */
int pdf_merge_or_split(char *outputfile, char *filename,char *passWord ,char *page);
#endif
