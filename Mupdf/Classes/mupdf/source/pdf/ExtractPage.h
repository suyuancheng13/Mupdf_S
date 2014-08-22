//
//  ExtractPage.h
//  MuPDF
//
//  Created by batman on 13-8-27.
//  Copyright (c) 2013年 Artifex Software, Inc. All rights reserved.
//

#ifndef MuPDF_ExtractPage_h
#define MuPDF_ExtractPage_h
#include "mupdf/fitz.h"
#include "mupdf/pdf.h"
/*
 以outfilename指定的文件名输出pre-end之间（包括pre，end）的页。
 pre:起始页（从0开始）
 end:结束页
 filename:抽取也的文件名
 outfilename:输出文件名
 */
void ExtractPage(int pre, int end, char *filename, const char *outfilename);

#endif
