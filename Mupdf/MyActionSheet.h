//
//  MyActionSheet.h
//  MuPDF
//
//  Created by Ｙuancheng SU on 14-7-15.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import "MuActionSheetActionDeleglate.h"
@interface MyActionSheet : UIActionSheet<UIActionSheetDelegate>
{
    UIView *view;
    UIToolbar *toolbar;
    UIBarButtonItem *titleb;
    UIBarButtonItem *space;
}
@property(nonatomic,retain)UIView *View;
@property(nonatomic,retain)UIBarButtonItem *cancel;
@property(nonatomic,retain)UIBarButtonItem *done;
@property(nonatomic,assign)id<MuActionSheetActionDeleglate>MyDelegate;
- (id)initWithtitle:(NSString*)title frame:(CGSize)size;
- (void)addCancelBtn;
- (void)addDoneBtn;
@end
