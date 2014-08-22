//
//  MuSecurityDataSource.h
//  MuPDF
//
//  Created by Ｙuancheng SU on 14-7-15.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "MuActionSheetActionDeleglate.h"
#define PERMISSONS 7
@interface MuSecurityDataSource : NSObject<UITableViewDataSource,UITableViewDelegate, MuActionSheetActionDeleglate>
{
    int *_permissions;
    NSArray *permit;
    
}

- (id)initWithdPermission:(int*)permissions;
@end
