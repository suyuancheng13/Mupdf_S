//
//  MuAlertView.h
//  MuPDF
//
//  Created by Ｙuancheng SU on 14-7-16.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MuActionSheetActionDeleglate.h"
enum{FORADDPASSWORD,FORPERMISSIONCONTROL};
@interface MuAlertView : UIAlertView<UITextFieldDelegate,MuActionSheetActionDeleglate>
{
    
    int _style;
}

@property(nonatomic,retain)NSString *userPWD;
@property(nonatomic,retain)NSString *ownPWD;

@property(nonatomic,assign)id<MuActionSheetActionDeleglate>MyDelegate;
- (id)initWithStyle:(int)style;
@end
