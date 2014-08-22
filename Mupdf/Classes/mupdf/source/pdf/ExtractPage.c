//
//  ExtractPage.c
//  MuPDF
//
//  Created by batman on 13-8-27.
//  Copyright (c) 2013年 Artifex Software, Inc. All rights reserved.
//

#include "ExtractPage.h"
#define _EXTRACT_PAGE
static void write_document(pdf_document *doc, const char *filename)
{
    fz_write_options opts;
    int errors = 0;
    opts.do_incremental = 0;
    opts.do_garbage = 1;//collect the object which not used
    opts.do_expand = 0;
    opts.do_ascii = 0;
    opts.do_linear = 0;
    opts.continue_on_error = 1;
    opts.errors = &errors;
    
    pdf_write_document(doc, (char*)filename, &opts);
}

static void export_page_range(pdf_document *doc, int start, int end)
{
    int i;
    
	if (start > end)
	{
		int tmp = start;
		start = end;
		end = tmp;
	}
    
	if (!doc || start >= doc->page_len || end < 0)
		return;
    //删掉不在start-end之间（不包括start，end）的页
    for (i = 0; i < start; i++)
    {
        pdf_drop_obj(doc->page_objs[i]);
        pdf_drop_obj(doc->page_refs[i]);
        pdf_delete_object(doc, pdf_to_int(doc->page_refs[i]));
        
    }
    for (i = end + 1; i < doc->page_len; ++i) {
        pdf_drop_obj(doc->page_objs[i]);
        pdf_drop_obj(doc->page_refs[i]);
        pdf_delete_object(doc, pdf_to_int(doc->page_refs[i]));
    }
    
    if (doc->page_len > end)
    {
        memmove(&doc->page_objs[0], &doc->page_objs[start], sizeof(pdf_page *) * (end - start + 1));
        memmove(&doc->page_refs[0], &doc->page_refs[start], sizeof(pdf_page *) * (end - start + 1));
    }
    
	doc->page_len = end - start + 1;
	doc->needs_page_tree_rebuild = 1;
}

void ExtractPage(int pre, int end, char *filename, const char *outfilename)
{
    fz_context *ctx = fz_new_context(NULL, NULL, 128<<20);
    pdf_document *wdoc = (pdf_document *)fz_open_document(ctx, filename);
    int page_count = pdf_count_pages(wdoc);
    
    if (pre > end) {
        pre = pre + end;
        end = pre - end;
        pre = pre - end;
    }
    if (pre < 0) {
        pre = 0;
#ifdef _EXTRACT_PAGE
        printf("\n%s : %s\n", __FUNCTION__, "warning!!! The start page num less 0");
#endif
    }
    if (end >= page_count) {
        end = page_count - 1;
#ifdef _EXTRACT_PAGE
        printf("\n%s : %s\n", __FUNCTION__, "warning!!! The end page num than page_count");
#endif
    }
    
    if (pre >= 0 && pre < page_count && end >= 0 && end < page_count) {
        export_page_range(wdoc, pre, end);
    }else{
        printf("\n%s : %s\n", __FUNCTION__, "warning!!! parameter error!!! output original document!");
    }
    write_document(wdoc, outfilename);
    
    pdf_close_document(wdoc);
}