//
//  MuOutlineTable.h
//  MuPDF
//
//  Created by Suyuancheng on 14-10-12.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface MuOutlineTable : UITableView<UITableViewDataSource,UITableViewDelegate>
{
    NSMutableArray *_titles;
    NSMutableArray *_pages;
    id          _target;
}
- (id)initWithtile:(NSMutableArray*)titles pages:(NSMutableArray*)pages frame:(CGRect )frame target:(id)target;
@end
