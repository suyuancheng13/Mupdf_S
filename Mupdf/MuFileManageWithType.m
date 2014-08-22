//
//  MuFileManageWithType.m
//  MuPDF
//
//  Created by Ｙuancheng SU on 14-7-18.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import "MuFileManageWithType.h"
#include "CFunction.h"
#include <string.h>

@implementation MuFileManageWithType
//@synthesize view;
@synthesize MyDelegate;
@synthesize selectedFile;
- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}

- (id)initWithType:(NSString *)fileType
{
    if(self = [super init])
    {
       // [[[NSBundle mainBundle]infoDictionary]objectForKey:(NSString *)kcfbundle];
        _fileType = fileType;
        if(![_fileType compare:@"P"])
        {
            NSString *path = [NSString stringWithString:@"/private/var/mobile/Media/DCIM/100APPLE"];
            [self loadTypeFile:path];
        }else{
        _files = [[NSMutableArray alloc]init];
        _filesPath = [[NSMutableArray alloc]init];
        NSFileManager *fileManager = [NSFileManager defaultManager];
        const char *AppHome =[NSHomeDirectory() UTF8String];
        char *thisAppHome = rindex(AppHome, '/');
        *thisAppHome = '\0';
        NSString *Home = [[NSString alloc]initWithUTF8String:AppHome];
        NSDirectoryEnumerator *iterator = [fileManager enumeratorAtPath:Home];
        NSString *subDir;
        while (subDir = [iterator nextObject]) {
            NSString *subHome = [Home stringByAppendingPathComponent:subDir];
            subHome = [subHome stringByAppendingPathComponent:@"Documents"];
            if([fileManager fileExistsAtPath:subHome])
            {
                 //NSLog(@"%@",subHome);
                [self loadTypeFile:subHome];
            }
            //[subHome release];
        }
        //[subDir release];
        [Home release];

        }
    }
    return  self;
}

- (void)dealloc
{
    [_files release];
    [_filesPath release];
    [super dealloc];
}
- (void)loadTypeFile:(NSString *)dir
{
    NSFileManager *manager = [NSFileManager defaultManager];
    NSDirectoryEnumerator *iterator = [manager enumeratorAtPath:dir];
    NSString *file;
    while (file = [iterator nextObject]) {
        BOOL isDir = false;
        if([manager fileExistsAtPath:[dir stringByAppendingPathComponent:file] isDirectory:&isDir]&&isDir)
        {
            [self loadTypeFile:[dir stringByAppendingPathComponent:file]];
            return;
            
        }
        if(![_fileType compare:@"M"])
        {
            if(!strcmp("mp3",rindex([file UTF8String], '.')+1))
            {
                [_files addObject:file];
                [_filesPath addObject:[dir stringByAppendingPathComponent:file]];
                
            }
            if(!strcmp("wav",rindex([file UTF8String], '.')+1))
            {
                [_files addObject:file];
                [_filesPath addObject:[dir stringByAppendingPathComponent:file]];
                
            }
        }
        if(![_fileType compare:@"V"])
        {
            if(!strcmp("mp4",rindex([file UTF8String], '.')+1))
            {
                [_files addObject:file];
                [_filesPath addObject:[dir stringByAppendingPathComponent:file]];
                
            }
            if(!strcmp("mov",rindex([file UTF8String], '.')+1))
            {
                [_files addObject:file];
                [_filesPath addObject:[dir stringByAppendingPathComponent:file]];
                
            }
        }
        if(![_fileType compare:@"P"])
        {
            if(!strcmp("jpg",rindex([file UTF8String], '.')+1))
            {
                [_files addObject:file];
                [_filesPath addObject:[dir stringByAppendingPathComponent:file]];
                
            }
//            if(!strcmp("png",rindex([file UTF8String], '.')+1))
//            {
//                [_files addObject:file];
//                [_filesPath addObject:[dir stringByAppendingPathComponent:file]];
//                
//            }
        }

    }
    
}
//- (void)
#pragma mark - table data source
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [_files count];
}
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"Cell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if(!cell)
    {
        cell = [[[UITableViewCell alloc] initWithStyle: UITableViewCellStyleDefault reuseIdentifier: CellIdentifier] autorelease];
        [[cell textLabel]setText:[_files objectAtIndex:indexPath.row]];
        //NSLog(@"%@",[_files objectAtIndex:indexPath.row]);
    }
    return cell;
}
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    selectedFile = [_filesPath objectAtIndex:indexPath.row];
    return;
}
#pragma mark - muactionsheetdelegate
- (void)onDone:(id)sender
{
    [sender dismissWithClickedButtonIndex:1 animated:YES];
    if(![_fileType compare:@"M"])
    [MyDelegate OkforAddMusic:self];
    if(![_fileType compare:@"P"])
        [MyDelegate OkforAddPicWatermark:self];
    
    
}
@end
