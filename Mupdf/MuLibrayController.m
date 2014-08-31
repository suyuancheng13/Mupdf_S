//
//  MuLibrayController.m
//  MuPDF
//
//  Created by Ｙuancheng SU on 14-6-25.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import "MuLibrayController.h"
#import "MuDocumentViewController.h"

#import "mach/mach.h"
extern fz_context *ctx;
extern dispatch_queue_t queue;
@interface MuLibrayController ()

@end
@implementation MuLibrayController

- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        // Custom initialization
        
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    /*
     *author: suyuancheng
     */
    [self setTitle:@"文档"];
    [self reload];
    
    _timer = [NSTimer timerWithTimeInterval:3 target:self selector:@selector(reload) userInfo:nil repeats:YES];
    [[NSRunLoop currentRunLoop]addTimer:_timer forMode:NSDefaultRunLoopMode];
    
    
    /*
     add by suyuancheng
     */
    choosenFiles = [[NSMutableArray alloc]init];
    edit = [[UIBarButtonItem alloc]initWithTitle:@"编辑" style:UIBarButtonItemStylePlain target:self action:@selector(onEdit)];
    [[self navigationItem]setRightBarButtonItem:edit];
    
    
    
    UIImage* imaget = [UIImage imageNamed:@"DefaultToolBar.png"];
    [[self navigationController].toolbar setBackgroundImage:imaget forToolbarPosition:UIToolbarPositionBottom barMetrics:UIBarMetricsDefault];
    [[self navigationController].toolbar setAlpha:0.7];
    
}
- (void)onEdit
{
    if(!self.tableView.editing)
    {
        [edit setTitle:@"完成"];
        
        self.tableView.allowsMultipleSelection = YES;
        self.tableView.allowsMultipleSelectionDuringEditing = YES;
        [self.tableView setEditing:YES];
        
        [[self navigationController ]setToolbarHidden:NO animated:YES];
        UIImage *image = [UIImage imageNamed:@"trash.png"];
        UIImageView *imageview = [[UIImageView alloc]initWithImage:image];
        UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onTrashFiles)];
        [imageview addGestureRecognizer:tap];
        imageview.userInteractionEnabled = YES;
        UIBarButtonItem*  trash = [[UIBarButtonItem alloc]initWithCustomView:imageview];
        
        [imageview release];
        [image release];
        [tap release];
        
        image = [UIImage imageNamed:@"merge.png"];
        imageview = [[UIImageView alloc]initWithImage:image];
        tap = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onMergeFiles)];
        [imageview addGestureRecognizer:tap];
        imageview.userInteractionEnabled = YES;
        UIBarButtonItem* merge = [[UIBarButtonItem alloc]initWithCustomView:imageview];
        
        [imageview release];
        [image release];
        [tap release];
        UIBarButtonItem *space = [[UIBarButtonItem alloc]initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:self action:nil];
        
        [self setToolbarItems:[NSArray arrayWithObjects:space,merge,space,trash,space, nil] animated:YES];
        //        [trash release];
        //        [merge release];
        //        [space release];
    }
    else {
        [edit setTitle:@"编辑"];
        [self.tableView setEditing:NO];
        self.tableView.allowsMultipleSelection = NO;
        self.tableView.allowsMultipleSelectionDuringEditing = NO;
        [[self navigationController ]setToolbarHidden:YES animated:YES];
        [choosenFiles removeAllObjects];
    }
}
- (void)reload
{
    if(_pdfs)
    {
        [_pdfs release];
        _pdfs = nil;
    }
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *dir = [NSString stringWithFormat:@"%@/Documents",NSHomeDirectory()];
    NSMutableArray *outFile = [[NSMutableArray alloc]init];
    NSDirectoryEnumerator *ireator = [fileManager enumeratorAtPath:dir];
    NSString *file;
    BOOL isDir;
    /*
     load the home document files
     */
    while (file = [ireator nextObject]) {
        NSString *filePath = [dir stringByAppendingPathComponent:file];
        if([fileManager fileExistsAtPath:filePath isDirectory:&isDir ]&&!isDir)
        {
            char *filetype =  rindex([file UTF8String], '.');
            if(!strcmp(filetype+1, "pdf"))
                [outFile addObject:file];
        }
    }
    _pdfs = outFile;
    [[self tableView]reloadData];
    
}
- (void)viewWillDisappear:(BOOL)animated
{
    [_timer invalidate];
    _timer = nil;
}
- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
    
}
- (void)dealloc
{
    [edit release];
    edit =nil;
    [choosenFiles release];
    choosenFiles = nil;
    [super dealloc];
}


- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
#warning Potentially incomplete method implementation.
    // Return the number of sections.
    return 2;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
#warning Incomplete method implementation.
    // Return the number of rows in the section.
    switch (section) {
        case 0:
            return [_pdfs count];
        case 1:
            return [_xpss count];
        case 2:
            return [_cbzs count];
            
    }
    
}
- (void)onSelect:(UIControl*)sender
{
    [((UIButton*)sender)setImage:[UIImage imageNamed:@"choosen.png"] forState:UIControlStateNormal];
    
    
}
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"Cell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    UIButton *deleteButton = [UIButton buttonWithType:UIButtonTypeCustom];
	//[deleteButton setImage: [UIImage imageNamed: @"x_alt_blue.png"] forState: UIControlStateNormal];
	[deleteButton setFrame: CGRectMake(0, 0, 35, 35)];
	[deleteButton addTarget: self action: @selector(onSelect:) forControlEvents: UIControlEventTouchUpInside];
	[deleteButton setTag: indexPath.row];
    // Configure the cell...
    switch (indexPath.section) {
        case 0:
            if (!cell)
                cell = [[[UITableViewCell alloc] initWithStyle: UITableViewCellStyleDefault reuseIdentifier: CellIdentifier] autorelease];
            
            [[cell textLabel] setText: [_pdfs objectAtIndex: [indexPath row]]];
            //cell.AccessoryType = UITableViewCellAccessoryCheckmark;
            //[[cell imageView]set:deleteButton];
            [cell setAccessoryView:deleteButton];
            break;
        case 1:
            if (!cell)
                cell = [[[UITableViewCell alloc] initWithStyle: UITableViewCellStyleDefault reuseIdentifier: CellIdentifier] autorelease];
            [[cell textLabel] setText: [_pdfs objectAtIndex:  [indexPath row]]];
            break;
        case 2:
            if (!cell)
                cell = [[[UITableViewCell alloc] initWithStyle: UITableViewCellStyleDefault reuseIdentifier: CellIdentifier] autorelease];
            [[cell textLabel] setText: [_cbzs objectAtIndex:  [indexPath row]]];
            break;
            
    }
    [[cell textLabel] setFont: [UIFont systemFontOfSize: 20]];
    return cell;
}


// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the specified item to be editable.
    return YES;
}

- (UITableViewCellEditingStyle)tableView:(UITableView *)tableView editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return UITableViewCellEditingStyleInsert;
}

// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
        //[tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:UITableViewRowAnimationFade];
    }   
    else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }   
}


/*
 // Override to support rearranging the table view.
 - (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath
 {
 }
 */

/*
 // Override to support conditional rearranging of the table view.
 - (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath
 {
 // Return NO if you do not want the item to be re-orderable.
 return YES;
 }
 */

#pragma mark - Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    if(self.tableView.editing)
    {   
        if(NSNotFound ==[choosenFiles indexOfObject:[_pdfs objectAtIndex:indexPath.row]])
            [choosenFiles addObject:[_pdfs objectAtIndex:indexPath.row]];
        else
        {
            [choosenFiles removeObjectAtIndex:indexPath.row];
        }
        return;
    }
    else {
        switch (indexPath.section) {
            case 0:
                [self openDocument:[_pdfs objectAtIndex:indexPath.row]];
                break;
            case 1:
                [self openDocument:[_xpss objectAtIndex:indexPath.row]];
                break;
            case 2:
                [self openDocument:[_cbzs objectAtIndex:indexPath.row]];
                break;
        }
        
    }
    
}
- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    return 40;
}
- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section
{
    UIView *headerView = [[UIView alloc]init];
    headerView.backgroundColor = [UIColor grayColor];   
    
    UILabel *titlelabel = [[UILabel alloc]initWithFrame:CGRectMake(10, 0, 92, 22)];
    [titlelabel setBackgroundColor:[UIColor grayColor]];
    switch (section) {
        case 0:
            [titlelabel setText:@"PDF Files"];
            break;
        case 1:
            [titlelabel setText: url];
            break;
        case 2:
            [titlelabel setText: @"CBZ Files"];
            break;
    }
    // [titlelabel setText:@"PDF Files"];
    [headerView addSubview:titlelabel];
    return headerView;
    
    
}
- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    // [tableView.tableHeaderView setBackgroundColor:[UIColor whiteColor]];
    switch (section) {
        case 0:
            return @"PDF Files";
            break;
        case 1:
            return url;
            break;
        case 2:
            return @"CBZ Files";
            break;
    }
}
#pragma mark - Pdf handler

- (void)openDocument:(NSString *)filename
{
    NSString *file = [NSString stringWithFormat:@"%@/Documents/%@", NSHomeDirectory(),filename];
    
    //**emphance the robust of opeing file
    fz_try(ctx){
        _doc = fz_open_document(ctx, [file UTF8String]);
    }
    fz_catch(ctx)
    {
    
        UIAlertView *alert = [[UIAlertView alloc]initWithTitle:@"Open failed" message:filename delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"OK", nil];
        [alert show];
        [alert release];
        
        return;
    }
    _filename = [filename retain];
    if(!_doc)
    {
        UIAlertView *alert = [[UIAlertView alloc]initWithTitle:@"Open failed" message:filename delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"OK", nil];
        [alert show];
        [alert release];
        return;
    }
    else {
        if(fz_needs_password(_doc))
            [self askForPassword:@"this file need a password"];
        else {
            [self onPasswordOkay];
        }
    }
}
- (void)askForPassword:(NSString *)pass
{
    UIAlertView *passalert = [[UIAlertView alloc]initWithTitle:@"Password protected" message:pass delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"OK", nil];
    [passalert setAlertViewStyle:UIAlertViewStyleSecureTextInput];
    [passalert setTag:1];
    [passalert show];
    [passalert release];
}

//the delegate  of alertview
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
	
	[alertView dismissWithClickedButtonIndex: buttonIndex animated: TRUE];
	if (buttonIndex == 1) 
    {
        if(1== [alertView tag])
        {
            char *password = (char*) [[[alertView textFieldAtIndex: 0] text] UTF8String];
            
            if (fz_authenticate_password(_doc, password))
            {
                [self onPasswordOkay];
            }
            else
                [self askForPassword: @"Wrong password for '%@'. Try again:"];
        }
        if ([alertView tag] == 2) {
            if([choosenFiles count]<=1)
                return;
            // logMemUsage();
            dispatch_async(queue, ^{
                NSString *tem  =  [NSString stringWithFormat:@"%@/Documents/%@", NSHomeDirectory(),[[alertView textFieldAtIndex:0] text]];
                const char *newfilename =[tem UTF8String];
                NSString *filename[10];
                pdf_document *doc[10];
                filename[0] =  [NSString stringWithFormat:@"%@/Documents/%@", NSHomeDirectory(),[choosenFiles objectAtIndex:0]];
                doc[0] = pdf_open_document(ctx, [filename[0] UTF8String]);
                for (int i=1; i<[choosenFiles count]; i++) {
                    
                    filename[i] = [NSString stringWithFormat:@"%@/Documents/%@", NSHomeDirectory(),[choosenFiles objectAtIndex:i]];
                    doc[i] = pdf_open_document(ctx, [filename[i] UTF8String]);
                    doc[0]= pdf_merge_file(newfilename, 2,doc[0],doc[i]);
                }
                
                pdf_save_merged_file(doc[0], newfilename);
            });
        }
        //logMemUsage();
        
	} else 
    {
        if(1== [alertView tag])
            [self onPasswordCancel];
    }
}
- (void)onPasswordOkay
{
    MuDocumentViewController *document = [[MuDocumentViewController alloc]initWithFilename:_filename document:_doc];
    if (document) {
		[self setTitle: @"文档"];
		[[self navigationController] pushViewController: document animated: YES];
		[document release];
	}
	[_filename release];
	_doc = NULL;
}
- (void)onPasswordCancel
{
    [_filename release];
    fz_close_document(_doc);
    _doc = NULL;
}
- (void)setUrl:(NSString *)te
{
    //UIActivityViewController
    // UIDocumentInteractionController
    UITextView *v = [[UITextView alloc]initWithFrame:CGRectMake(10,300, 300, 40)];
    
    [v setScrollEnabled:YES];
    [v setText:te];
    [self.view addSubview:v];
    //    UILabel *titlelabel = [[UILabel alloc]initWithFrame:CGRectMake(10,300, 300, 40)];
    //    [titlelabel setBackgroundColor:[UIColor grayColor]];
    //    [titlelabel setText:te];
    //    [titlelabel setHidden:NO];
    //    [self.view addSubview:titlelabel];
}

#pragma mark- file handle
- (void)onTrashFiles
{
    //int row = [sender tag];
	NSString *title = [NSString stringWithFormat: @"Delete Choosen Files?"];
	UIActionSheet *sheet = [[UIActionSheet alloc]
							initWithTitle: title
							delegate: self
							cancelButtonTitle: @"Cancel"
							destructiveButtonTitle: @"Delete"
							otherButtonTitles: nil];
	[sheet setTag: 1];
    [sheet showFromToolbar: [self navigationController].toolbar];

	[sheet release];
    
}


- (void)onMergeFiles
{
    NSString *title = [NSString stringWithFormat: @"Merge Choosen Files?"];
	UIActionSheet *sheet = [[UIActionSheet alloc]
							initWithTitle: title
							delegate: self
							cancelButtonTitle: @"Cancel"
							destructiveButtonTitle: @"Merge"
							otherButtonTitles: nil];
	[sheet setTag: 2];
    [sheet showFromToolbar: [self navigationController].toolbar];

	[sheet release];
}
- (void) actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    [actionSheet dismissWithClickedButtonIndex:buttonIndex animated:YES];
	if (buttonIndex == [actionSheet destructiveButtonIndex])
    {    
        if( 1 == [actionSheet tag])
        {
            char filename[PATH_MAX];
            dispatch_sync(queue, ^{});
            for (int i=0;i<[choosenFiles count]; i++) {
                
                strcpy(filename, [NSHomeDirectory() UTF8String]);
                strcat(filename, "/Documents/");
                strcat(filename, [[choosenFiles objectAtIndex:i] UTF8String]);
                
                printf("delete document '%s'\n", filename);
                
                unlink(filename);//delete the link of the file
                
                [self reload];
            }
        }
        if(2 == [actionSheet tag])
        {
            UIAlertView *test = [[UIAlertView alloc]initWithTitle:@"文件名" message:@"合并后的文件名" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"OK", nil];
            [test setAlertViewStyle:UIAlertViewStylePlainTextInput];
            [test setTag:2];
            [[test textFieldAtIndex:0]setPlaceholder:@"text"];
            [test show];
            [test release];
        }
    }
}

@end
