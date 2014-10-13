//
//  MuPopView.h
//  MuPDF
//
//  Created by Ｙuancheng SU on 14-7-20.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MuActionSheetActionDeleglate.h"
@interface MuPopView : UIView<MuActionSheetActionDeleglate>

@property(nonatomic,assign)NSInteger style;
@property(nonatomic,retain)id<MuActionSheetActionDeleglate>myDelegate;
@property(nonatomic,assign)int count;
- (void)hiddenPopView:(BOOL)animated;
- (void)showPopView:(BOOL)animated;
- (void)dismissPopView:(BOOL)animated;
- (void)addButtonWithTitle:(NSArray *)titles;
@end
