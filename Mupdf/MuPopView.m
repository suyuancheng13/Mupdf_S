//
//  MuPopView.m
//  MuPDF
//
//  Created by Ｙuancheng SU on 14-7-20.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import "MuPopView.h"

@implementation MuPopView
@synthesize style;
@synthesize myDelegate;
@synthesize count;

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
        [self setBackgroundColor:[UIColor clearColor]];
        count =1;
    }
    return self;
}

- (void)drawRect:(CGRect)rect
{
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSetLineWidth(context, 0.5);
    [[UIColor whiteColor]setStroke];
   // CGContextSetGrayFillColor(context, 125, 0.6);
    CGContextSetFillColorWithColor(context, [UIColor blackColor].CGColor);
    CGRect rrect = self.bounds;
    CGFloat minX = CGRectGetMinX(rrect);
    CGFloat midX = CGRectGetMidX(rrect);
    CGFloat maxX = CGRectGetMaxX(rrect);
    CGFloat minY = CGRectGetMinY(rrect);
    CGFloat maxY = CGRectGetMaxY(rrect)-10;
    CGContextMoveToPoint(context, midX+10, maxY);
    CGContextAddLineToPoint(context, midX, maxY+10);
    CGContextAddLineToPoint(context, midX-10, maxY);
    
    CGContextAddArcToPoint(context, minX, maxY, minX, minY, 10);
    CGContextAddArcToPoint(context, minX, minY, maxX, minY, 10);
    CGContextAddArcToPoint(context, maxX, minY, maxX, maxX, 10);
    CGContextAddArcToPoint(context, maxX, maxY, midX, maxY, 10);
    CGContextClosePath(context);
    CGContextFillPath(context);
    //[self setAlpha:0.8];
    CGContextMoveToPoint(context, 50, minY);
    CGContextAddLineToPoint(context, 50, maxY);
    CGContextMoveToPoint(context, 100, minY);
    CGContextAddLineToPoint(context, 100, maxY);
    CGContextMoveToPoint(context, 150, minY);
    CGContextAddLineToPoint(context, 150, maxY);
    CGContextMoveToPoint(context, 200, minY);
    CGContextAddLineToPoint(context, 200, maxY);
    CGContextDrawPath(context, kCGPathFillStroke);
    
}
- (void)hiddenPopView:(BOOL)animated
{
    if(animated)
    {
        [UIView beginAnimations:nil context:nil];
        [UIView setAnimationDuration:0.5];
        [self setAlpha:0.0];
        [UIView commitAnimations];
    }
}
- (void)showPopView:(BOOL)animated
{
    [UIView beginAnimations:nil context:nil];
    [UIView setAnimationDuration:0.5];
    [self setAlpha:1.0];
    [UIView commitAnimations];
}
- (void)dismissPopView:(BOOL)animated
{
    [UIView beginAnimations:nil context:nil];
    [UIView setAnimationDuration:0.5];
    [self removeFromSuperview];
    [UIView commitAnimations];
}

- (void)addButtonWithTitle:(NSArray *)titles
{
    for(int i = 0 ;i<[titles count];i++)
    {
        UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
        [button setFrame:CGRectMake(i*50, 0, 50, 40)];
        [button setBackgroundColor:[UIColor clearColor]];
        [button setTitle:[titles objectAtIndex:i] forState:UIControlStateNormal];
        [button setTag:i];
        [button setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [button setTitleColor:[UIColor brownColor] forState:UIControlStateHighlighted];
        [button.titleLabel setFont:[UIFont systemFontOfSize:12]];
        [button addTarget:self action:@selector(onButtonClick:) forControlEvents:UIControlEventTouchUpInside ];
        [self addSubview:button];
        //[button release];
    }
}

- (void) onButtonClick:(id)sender
{
    
    [self dismissPopView:YES];
    count = 0;
    [myDelegate onButtonClick:((UIButton *)sender).tag sender:self];
    //[self release];
    if(3==((UIButton *)sender).tag)
    [myDelegate OkforAddTextWatermark:self];
    
}

@end
