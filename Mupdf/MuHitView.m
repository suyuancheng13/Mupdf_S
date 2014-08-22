//
//  MuHitView.m
//  MuPDF
//
//  Created by Ｙuancheng SU on 14-7-2.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import "MuHitView.h"
#include"mupdf/pdf.h"
extern fz_rect hit_bbox[500];

@implementation MuHitViewer

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}
-(id) initWithResults:(int)count forDocument:(fz_document*)doc
{
    self = [super init];
    if(self)
    {
        [self setOpaque: NO];
        hit_count = 0 ;
        pagesize = CGSizeMake(100, 100);
        color = [[UIColor colorWithRed: 0xAC/255.0 green: 0x22/255.0 blue: 0x15/255.0 alpha: 0.5] retain];
        for(int i =0;i<count && i<nelem(hit_rects);i++)
        {
            hit_rects[i].origin.x = hit_bbox[i].x0;
            hit_rects[i].origin.y = hit_bbox[i].y0;
            hit_rects[i].size.width = hit_bbox[i].x1-hit_bbox[i].x0;
            hit_rects[i].size.height = hit_bbox[i].y1 - hit_bbox[i].y0;
            hit_count++;
        }
    }
    return self;
}

// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (CGSize)fitToScreen:(CGSize)page Screen:(CGSize)screen
{
	float hscale = screen.width / page.width;
	float vscale = screen.height / page.height;
	float scale = fz_min(hscale, vscale);
	hscale = floorf(page.width * scale) / page.width;
	vscale = floorf(page.height * scale) / page.height;
	return CGSizeMake(hscale, vscale);
}
- (void)drawRect:(CGRect)rect
{
    // Drawing code
    CGSize scale = [self fitToScreen:pagesize Screen:self.bounds.size];
    [color set];
    for (int i =0; i<hit_count; i++) {
        CGRect rect = hit_rects[i];
        rect.origin.x *=scale.width;
        rect.origin.y *=scale.height;
        rect.size.height *=scale.height;
        rect.size.width *= scale.width;        
        UIRectFill(rect);
    }
    
}
- (void) setPagsize:(CGSize)size
{
    pagesize = size;
    [self setNeedsDisplay];
}
- (void)dealloc
{
    [color release];
    [super dealloc];
}

@end
