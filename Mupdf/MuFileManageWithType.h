//
//  MuFileManageWithType.h
//  MuPDF
//
//  Created by Ｙuancheng SU on 14-7-18.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import "MuActionSheetActionDeleglate.h"
#import "MyActionSheet.h"
@interface MuFileManageWithType :MyActionSheet <UITableViewDataSource,UITableViewDelegate,MuActionSheetActionDeleglate>
{
    NSString *_fileType;
    NSMutableArray *_files;
    NSMutableArray *_filesPath;
//    UIToolbar *toolbar;
//    UIBarButtonItem *titleb;
//    UIBarButtonItem *space;
//    UIView *view;
}
@property (nonatomic,assign)id<MuActionSheetActionDeleglate>MyDelegate;
@property(nonatomic,retain)NSString *selectedFile;
//@property(nonatomic,retain)UIView *view;
- (id)initWithType:(NSString *)fileType;
- (void)loadTypeFile:(NSString *)dir;
@end
