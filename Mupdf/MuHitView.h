//
//  MuHitView.h
//  MuPDF
//
//  Created by Ｙuancheng SU on 14-7-2.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import <UIKit/UIKit.h>
#include "mupdf/pdf.h"
@interface MuHitViewer : UIView
{
    CGRect hit_rects[500];
    int hit_count;
    UIColor *color;
    CGSize pagesize;
}

-(id) initWithResults:(int)count forDocument:(fz_document*)doc;
- (CGSize)fitToScreen:(CGSize)page Screen:(CGSize)screen;
- (void) setPagsize:(CGSize)size;
@end
