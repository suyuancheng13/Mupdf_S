//
//  MuActionSheetActionDeleglate.h
//  MuPDF
//
//  Created by Ｙuancheng SU on 14-7-15.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol MuActionSheetActionDeleglate <NSObject>
@optional
/*
 *for MyActionSheet
 */
- (void)onDone:(id)sender;
- (void)onCancel:(id)sender;
/*
 *for MuAlertView
 */
/*
 add password the ok button action
 */
- (void)OKforAddPassword:(id)sender;
/*
 confirm the user and own password to change the permission
 */
- (void)OKforPermissionChange:(id)sender;
/*
 for MufileManageWithType
 */
- (void)OkforAddMusic:(id)sender;
//- (void)OkforAddPicWatermark:(id)sender;
- (void)OkforAddTextWatermark:(id)sender;
/*
 for mupopview action
 */
- (void)onButtonClick:(NSInteger)tag sender:(id)sender;
@end
