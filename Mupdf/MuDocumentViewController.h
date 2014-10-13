//
//  MuDocumentViewController.h
//  MuPDF
//
//  Created by Ｙuancheng SU on 14-6-25.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import <UIKit/UIKit.h>
#import<AVFoundation/AVFoundation.h>
#import "MuOutlineViewController.h"
#include "mupdf/pdf.h"
#import "MyActionSheet.h"
#import "MuActionSheetActionDeleglate.h"
#import "MuAlertView.h"
#import "MuFileManageWithType.h"
#import "MuPopView.h"
#import "MuOutlineView.h"

enum {ADDPASSWORD,REMOVEPASSWORD,MODIFYPASSWORD,MODIFYPERMISSION};
#define PERMISSIONNUMBERS 8
@interface MuDocumentViewController : UIViewController<UIScrollViewDelegate,UISearchBarDelegate,UITextFieldDelegate,UIActionSheetDelegate,UITableViewDataSource,UITableViewDelegate,MuActionSheetActionDeleglate,UINavigationControllerDelegate, UIImagePickerControllerDelegate,UIDocumentInteractionControllerDelegate>
{
   	fz_document *doc;
	NSString *key;
    MuOutlineView *outline;
//	MuOutlineViewController *outline;
	UIScrollView *canvas;
	UILabel *indicator;
	UISlider *slider;
	UISearchBar *searchBar;
    UIBarButtonItem *ToolBar;
    UIBarButtonItem *passTool,*annotationTool,*multimdiaTool,*watermarkTool;
	UIBarButtonItem *nextButton, *prevButton, *cancelButton, *searchButton, *outlineButton, *linkButton;
	UIBarButtonItem *sliderWrapper;
    UIAlertView *alert;
    UIActivityIndicatorView *handling;
    UIImage *background;
    UIBarButtonItem *Home ;
    UITextField *password;
    UITextField *password2;
    MuAlertView *addPassBoerd;
    UIView *security;
    UIButton *pre;
    UIButton *next;
    UIBarButtonItem *securitybarbutton;
    UIBarButtonItem *share;
    
    MyActionSheet *permission;
    MyActionSheet *showSecurity;
    MuPopView *popView;
    char *filename;
    int per[PERMISSIONNUMBERS];
    int permissions[PERMISSIONNUMBERS];
    int permissionChanges[PERMISSIONNUMBERS];
	int searchPage;
	int cancelSearch;
	int showLinks;
	int width; // current screen size
	int height;
	int current; // currently visible page
	int scroll_animating; // stop view updates during scrolling animations   
    int pages;
    int _passwordAction;
    int longPress;
    fz_point pressPosion;
    NSString *imagePath;
    BOOL isReocrding;
    //AVAudioSession *session ;
    AVAudioRecorder *recorder;
    NSURL *recordedFile;
    UIButton *recoderbtn ;
    CGPoint p;
}
@property (nonatomic) BOOL isRecording;
- (id) initWithFilename: (NSString*)nsfilename document: (fz_document *)aDoc;
- (void) createPageView: (int)number;
- (void) gotoPage: (int)number animated: (BOOL)animated;
- (void) onShowOutline: (id)sender;
/*
 search item
 */
- (void) onShowSearch: (id)sender;
- (void) onCancelSearch: (id)sender;
- (void) resetSearch;
- (void) showSearchResults: (int)count forPage: (int)number;
- (void) onSearchNext:(id)sender;
- (void) onSearchPre:(id)sender;
- (void) onSearch:(int)direction;

- (void) onSlide: (id)sender;
- (void) onTap: (UITapGestureRecognizer*)sender;
- (void) showNavigationBar;
- (void) hideNavigationBar;

/*
Zoom gesture
 */
/*
 Tools
 */
- (void)cancleTools;
- (void)cancleTool;
/*
 pass word
 */
- (void)passWordTool;
- (void)cancelPassTool;
- (void)addPassword;
- (void) cancelAddPassword;
- (void)removePassword;
- (void)modifyPassword;
- (void)modifyPermission;
- (void)showSecurity;
//unlock the file
- (void)unlock;
- (void)addWatermark:(pdf_document *)pdfdoc pageNum:(int)pageNum;
@end
