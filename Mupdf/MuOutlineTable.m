//
//  MuOutlineTable.m
//  MuPDF
//
//  Created by Suyuancheng on 14-10-12.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import "MuOutlineTable.h"
#import "MuOutlineView.h"
@implementation MuOutlineTable

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}
- (id)initWithtile:(NSMutableArray*)titles pages:(NSMutableArray*)pages frame:(CGRect)frame target:(id)target
{
    if(self = [super init])
    {
        [self setFrame:CGRectMake(frame.origin.x+5, frame.origin.y+25, frame.size.width-10, frame.size.height-30)];
        _titles = titles;
        _pages = pages;
        _target = target;
        [self setDelegate:self];
        [self setDataSource: self];
    }
    return self;
}
/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/
#pragma mark- data source
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [_titles count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSString *cellIdentifer = @"outlineCell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:cellIdentifer];
    if(!cell)
        cell = [[[UITableViewCell alloc]initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cellIdentifer]autorelease];
    [[cell textLabel]setFont:[UIFont systemFontOfSize:[UIFont smallSystemFontSize]]];
    [[cell textLabel]setTextAlignment:UITextAlignmentLeft];
    [[cell textLabel]textRectForBounds:CGRectMake(0,0, 200, 20) limitedToNumberOfLines:2];
    NSNumber *n = [_pages objectAtIndex:indexPath.row];
    
    NSString *text = [[NSString alloc]initWithFormat:@"%@--%d",[_titles objectAtIndex:indexPath.row],[n intValue]+1];
    [[cell textLabel] setText:text];
    return cell;
}
- (NSString *)tableView:(UITableView*)tableView titleForHeaderInSection:(NSInteger)section
{
    return @"Outline";
}
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}
#pragma mark - tableview delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSNumber *page = [_pages objectAtIndex:indexPath.row];
    [((MuOutlineView*)_target).target  gotoPage:[page intValue] animated: YES];
  //  [self removeFromSuperview];
    [_target dismissView];
}
- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    return 40;
}
@end
