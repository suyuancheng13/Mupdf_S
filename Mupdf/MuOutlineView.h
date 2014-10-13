//
//  MuOutlineView.h
//  MuPDF
//
//  Created by Suyuancheng on 14-10-12.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MuOutlineTable.h"

@interface MuOutlineView : UIView
{
    NSMutableArray *_titles;
    NSMutableArray *_pages;
    id              _target;
    CGPoint         _location;
    
}
@property(nonatomic,retain)id target;

- (id)initWithTarget:(id) target titles: titles pages: pages;
- (void)setLocation:(CGPoint)location;
- (void)dismissView;
@end
