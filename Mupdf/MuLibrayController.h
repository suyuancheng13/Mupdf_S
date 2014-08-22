//
//  MuLibrayController.h
//  MuPDF
//
//  Created by Ｙuancheng SU on 14-6-25.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import <UIKit/UIKit.h>
#include"mupdf/pdf.h"
#include "pdf-merge.h"
@interface MuLibrayController : UITableViewController<UIActionSheetDelegate>
{
    NSArray *_pdfs;
    NSArray *_xpss;
    NSArray *_cbzs;
    NSTimer *_timer;
    fz_document *_doc;
    NSString *_filename;
    NSString *url;
    UIBarButtonItem *edit;
    NSMutableArray *choosenFiles;
}
- (void)openDocument:(NSString*)filename;
- (void)askForPassword:(NSString *)pass;
- (void)onPasswordOkay;
- (void)onPasswordCancel;
- (void) reload;
- (void)setUrl:(NSString *)te;
@end
