//
//  MyActionSheet.m
//  MuPDF
//
//  Created by Ｙuancheng SU on 14-7-15.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import "MyActionSheet.h"

@implementation MyActionSheet
@synthesize View =view;
@synthesize cancel;
@synthesize done;
@synthesize MyDelegate;
- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:CGRectMake(0, 44,320, frame.size.height)];
  
    if (self) {

    }
    return self;
}
- (id)initWithtitle:(NSString*)title frame:(CGSize)size{
    self = [super init];
    if (self) {
        view = [[UIView alloc]initWithFrame:CGRectMake(0, 44,size.width, size.height)];
        view.backgroundColor = [UIColor groupTableViewBackgroundColor];
    
        [self addButtonWithTitle:@""];
        [self addButtonWithTitle:@""];
        [self addButtonWithTitle:@""];
        [self addButtonWithTitle:@""];
        [self addButtonWithTitle:@""];
        [self addButtonWithTitle:@""];
        [self addButtonWithTitle:@""];
        [self addButtonWithTitle:@""];
        [self addButtonWithTitle:@""];
        [self addButtonWithTitle:@""];
        
        [self setDelegate:self];

        toolbar = [[UIToolbar alloc]initWithFrame:CGRectMake(0, 0, size.width, 44)];
        toolbar.barStyle = UIBarStyleBlack;
        
        titleb = [[UIBarButtonItem alloc]initWithTitle:title style:UIBarButtonItemStylePlain target:nil action:nil];
        space = [[UIBarButtonItem alloc]initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil];    
        [self addSubview:toolbar];        
        [self addSubview:view];

           }
    return self;

}
- (void)addCancelBtn
{
    cancel = [[UIBarButtonItem alloc]initWithTitle:@"Cancel" style:UIBarButtonSystemItemDone target:self action:@selector(onCancel)];
    if(nil!=done)
    [toolbar setItems:[NSArray arrayWithObjects:cancel,titleb,space,done, nil]];
    else {
        [toolbar setItems:[NSArray arrayWithObjects:cancel,titleb,nil]];
    }
    [self setNeedsLayout];
}
- (void)addDoneBtn
{
    done = [[UIBarButtonItem alloc]initWithTitle:@"OK" style:UIBarButtonSystemItemDone target:self action:@selector(onDone)];
    if(cancel!=nil)
        [toolbar setItems:[NSArray arrayWithObjects:cancel,titleb,space,done, nil]];
    else {
         [toolbar setItems:[NSArray arrayWithObjects:space,titleb,space,done ,nil]];
    }
    [self setNeedsLayout];
}
- (void)dealloc
{
    [cancel release];
    [done release];
    [toolbar release];
    [space release];
    [titleb release];

    [super dealloc];
}
- (void)onCancel
{
    [MyDelegate onCancel:self];
}
- (void)onDone
{
    [MyDelegate onDone:self];
}

@end
