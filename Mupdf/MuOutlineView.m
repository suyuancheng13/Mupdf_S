//
//  MuOutlineView.m
//  MuPDF
//
//  Created by Suyuancheng on 14-10-12.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import "MuOutlineView.h"

@implementation MuOutlineView
@synthesize target = _target;

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}
- (id)initWithTarget:(id) target titles: titles pages: pages
{
    if(self = [super init])
    {
        CGRect _frame ;
        CGRect screenSize = [[UIScreen mainScreen]bounds];
        _frame.origin.x = screenSize.size.width*1/4-10;
        //_frame.origin.y = location.y;
        _frame.size.width = screenSize.size.width*3/4; 
        _frame.size.height = screenSize.size.height*3/4;
        [self setFrame:_frame];
        [self setBackgroundColor:[UIColor clearColor]];
        _titles = [titles retain];
        _pages = [pages retain];
        _target = target;
        MuOutlineTable *table = [[MuOutlineTable alloc]initWithtile:_titles pages:_pages frame:self.bounds target:self];
        [self addSubview:table];
        [table release];
    }
    return  self;
}

-(void)setFFrame:(CGRect)frame
{
    [self setFrame:frame];

}
- (void)setLocation:(CGPoint)location
{
    _location.x = location.x-self.frame.origin.x;
}
- (void)dismissView
{
    [self removeFromSuperview];
}
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    CGFloat X = _location.x;
    CGFloat width = self.frame.size.width;
    CGFloat height = self.frame.size.height;
    // Drawing code
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSetLineWidth(context, 0.5);
    CGContextSetFillColorWithColor(context, [UIColor blackColor].CGColor);
    
    CGContextMoveToPoint(context, X, 0);
    CGContextAddLineToPoint(context, X+10, 10);
    CGContextAddArcToPoint(context, width, 10, width, height, 10);
    CGContextAddArcToPoint(context, width ,height, 0, height,10);
   
    CGContextAddArcToPoint(context, 0,height, 0, 10,10);
     CGContextAddArcToPoint(context, 0,10, X-10, 10,10);
    CGContextAddLineToPoint(context, X-10, 10);
    CGContextAddLineToPoint(context, X, 0);
    CGContextClosePath(context);
    CGContextFillPath(context);
    CGContextDrawPath(context, kCGPathFillStroke);
    
    
    
}

@end
