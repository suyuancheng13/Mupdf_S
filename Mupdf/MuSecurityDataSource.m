//
//  MuSecurityDataSource.m
//  MuPDF
//
//  Created by Ｙuancheng SU on 14-7-15.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import "MuSecurityDataSource.h"

@implementation MuSecurityDataSource
- (id)initWithdPermission:(int *)permissions
{
    if(self = [super init])
    {
    _permissions = permissions;
    permit = [NSArray arrayWithObjects:@"高质打印",@"低质打印",@"内容可变", @"允许复制",@"添加注释",@"填写表单", @"拉取文本",@"页面操作", nil];
    }
    return self;
}
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return 8;
}
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"Cell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];

    if (!cell)
    {
        cell = [[[UITableViewCell alloc] initWithStyle: UITableViewCellStyleDefault reuseIdentifier: CellIdentifier] autorelease];
        if(_permissions[indexPath.row])
         {
            [[cell textLabel]setText:[((NSString*)[permit objectAtIndex:indexPath.row])stringByAppendingString:@":允许"]];
        }
        else {
            [[cell textLabel]setText:[((NSString*)[permit objectAtIndex:indexPath.row])stringByAppendingString:@":不允许"]];
        }
    }

    return  cell;
    
}
- (void)onDone:(id)sender
{
    [sender dismissWithClickedButtonIndex:0 animated:YES];
}
- (void)onCancel:(id)sender
{
    [sender dismissWithClickedButtonIndex:1 animated:YES];
}
@end
