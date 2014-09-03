//
//  MuDocumentViewController.m
//  MuPDF
//
//  Created by Ｙuancheng SU on 14-6-25.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import "MuDocumentViewController.h"
#import "MuPageViewer.h"
#include"pdf-password.h"
#import "MuSecurityDataSource.h"
#include "pdf-multimedia.h"
#include "watermark.h"

#pragma mark - background function
#define GAP 20
#define INDICATOR_Y -44-24
#define SLIDER_W (width - GAP - 24)
extern fz_context *ctx;
extern dispatch_queue_t queue;
//extern void flattenOutline(NSMutableArray *titles, NSMutableArray *pages, fz_outline *outline, int level);
 int hit_count = 0;
fz_rect hit_bbox[500];

static int
search_page(fz_document *doc, int number, char *needle, fz_cookie *cookie)
{
	fz_page *page = fz_load_page(doc, number);
    
	fz_text_sheet *sheet = fz_new_text_sheet(ctx);
	fz_text_page *text = fz_new_text_page(ctx);
	fz_device *dev = fz_new_text_device(ctx, sheet, text);
	fz_run_page(doc, page, dev, &fz_identity, cookie);
	fz_free_device(dev);
    
	hit_count = fz_search_text_page(ctx, text, needle, hit_bbox, nelem(hit_bbox));
    
	fz_free_text_page(ctx, text);
	fz_free_text_sheet(ctx, sheet);
	fz_free_page(doc, page);
    
	return hit_count;
}

static fz_rect
search_result_bbox(fz_document *doc, int i)
{
	return hit_bbox[i];
}
static void flattenOutline(NSMutableArray *titles, NSMutableArray *pages, fz_outline *outline, int level)
{
	char indent[8*4+1];
	if (level > 8)
		level = 8;
	memset(indent, ' ', level * 4);
	indent[level * 4] = 0;
	while (outline)
	{
		if (outline->dest.kind == FZ_LINK_GOTO)
		{
			int page = outline->dest.ld.gotor.page;
			if (page >= 0 && outline->title)
			{
				NSString *title = [NSString stringWithUTF8String: outline->title];
				[titles addObject: [NSString stringWithFormat: @"%s%@", indent, title]];
				[pages addObject: [NSNumber numberWithInt: page]];
			}
		}
		flattenOutline(titles, pages, outline->down, level + 1);
		outline = outline->next;
	}
}

#pragma mark - implement class
@interface MuDocumentViewController ()

@end

@implementation MuDocumentViewController
@synthesize isRecording = _isRecording;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}
- (id) initWithFilename: (NSString*)nsfilename document: (fz_document *)aDoc
{
    self = [super init];
    if(self)
    {
        UIImage *image = [UIImage imageNamed:@"Home.png"];
        UIButton *backButton = [UIButton buttonWithType:UIButtonTypeCustom];
        backButton.frame = CGRectMake(0, 0, 40, 27);
        [backButton setImage: image forState:UIControlStateNormal];
        [backButton addTarget:self action:@selector(backAction) forControlEvents:UIControlEventTouchUpInside];
        Home = [[UIBarButtonItem alloc]initWithCustomView:backButton];
        Home.style = UIBarButtonItemStylePlain;
        [self navigationItem].leftBarButtonItem = Home;
        
        
        key = [nsfilename retain];
        filename = (char*)malloc(1024*sizeof(char));
        longPress = 0;
        strcpy(filename, [NSHomeDirectory() UTF8String]);
        strcat(filename, "/Documents/");
        strcat(filename, [key UTF8String]);
        

        doc = aDoc;
        [self unlock];
        fz_outline *root = fz_load_outline(doc);
        if (root) {
            NSMutableArray *titles = [[NSMutableArray alloc] init];
            NSMutableArray *pages = [[NSMutableArray alloc] init];
            flattenOutline(titles, pages, root, 0);
            if ([titles count])
                //  outline = [[MuOutlineController alloc] initWithTarget: self titles: titles pages: pages];
                [titles release];
            [pages release];
            fz_free_outline(ctx, root);
            
            
        }
        
    }
    return self;
}


- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.
//     NSFileManager *fileManager = [NSFileManager defaultManager];
//
//    [fileManager createFileAtPath:[NSTemporaryDirectory() stringByAppendingString:@"RecordedFile.wav"] contents:nil attributes:nil];
    self.isRecording = NO;
    recordedFile = [[NSURL alloc]initFileURLWithPath:[NSTemporaryDirectory() stringByAppendingString:@"RecordedFile.wav"]];//use alloc method is a good way
    AVAudioSession *session = [AVAudioSession sharedInstance];
    
    NSError *sessionError;
    [session setCategory:AVAudioSessionCategoryPlayAndRecord error:&sessionError];
    
    if(session == nil)
        NSLog(@"Error creating session: %@", [sessionError description]);
    else
        [session setActive:YES error:nil];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
}
- (void)loadView
{
    pages = fz_count_pages(doc);
    
    background = [UIImage imageNamed:@"BarButtonBackGround.png"];
    /*
     opendcoument -> key
     key    ->   page number  *** in scrollViewDidScroll
     */
    [[NSUserDefaults standardUserDefaults]setObject:key forKey:@"OpenDocumentKey"];
    current = [[NSUserDefaults standardUserDefaults]integerForKey:key];
    if(current<0||current>=pages)
        current =0;
    
    UIView *view = [[UIView alloc] initWithFrame: CGRectZero];
	[view setAutoresizingMask: UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight];
	[view setAutoresizesSubviews: YES];
    /*
     set the canvas
     */
    canvas = [[UIScrollView alloc]initWithFrame:CGRectMake(0, 0, GAP,0)];
    [canvas setAutoresizingMask:UIViewAutoresizingFlexibleWidth|UIViewAutoresizingFlexibleHeight];
    [canvas setShowsHorizontalScrollIndicator:NO];
    [canvas setShowsVerticalScrollIndicator:NO];
    [canvas setDelegate:self];
    [canvas setPagingEnabled:YES];
    [canvas addGestureRecognizer: [[[UITapGestureRecognizer alloc] initWithTarget: self action: @selector(onTap:)] autorelease]];
    [canvas addGestureRecognizer:[[[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(longPressAction:)] autorelease]];
    scroll_animating = NO;
    /*
     set the indicator
     */
    indicator = [[UILabel alloc]initWithFrame:CGRectZero];
    [indicator setAutoresizingMask: UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleTopMargin];//set the location
    [indicator setText:@"0000 of 9999"];
    [indicator sizeToFit];//change the bounds size
	[indicator setCenter: CGPointMake(0, INDICATOR_Y)];
	[indicator setTextAlignment: UITextAlignmentCenter];
	[indicator setBackgroundColor: [[UIColor blackColor] colorWithAlphaComponent: 0.5]];
	[indicator setTextColor: [UIColor whiteColor]];
    [view addSubview:canvas];
    [view addSubview:indicator];
    
    /*
     set slider to acompply the indicator
     */
    slider = [[UISlider alloc]initWithFrame:CGRectZero];
   // [slider sizeToFit];
    [slider setMinimumValue:0];
    [slider setMaximumValue:fz_count_pages(doc)-1];
    [slider addTarget:self action:@selector(onSlide:) forControlEvents:UIControlEventValueChanged];
    sliderWrapper = [[UIBarButtonItem alloc]initWithCustomView:slider];
    [self setToolbarItems:[NSArray arrayWithObjects:sliderWrapper, nil]];
   // [view addSubview:slider];
    
    /*
     set the search bar
     */
//    searchButton = [[UIBarButtonItem alloc]initWithBarButtonSystemItem:UIBarButtonSystemItemSearch target:self action:@selector(onShowSearch:)];
    UIImage *imsearch = [UIImage imageNamed:@"search1.png"];
    UIImageView *imsearchView = [[UIImageView alloc]initWithImage:imsearch];
    imsearchView.userInteractionEnabled = YES;
    UITapGestureRecognizer *search = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onShowSearch:)];
    [imsearchView addGestureRecognizer:search];
    searchButton = [[UIBarButtonItem alloc]initWithCustomView:imsearchView];
    [imsearch release];
    [imsearchView release];
    [search release];

    /*
     set the navigation bar and tool bar
     */
   
    UIImage* imaget = [UIImage imageNamed:@"DefaultToolBar.png"];
    [[self navigationController].toolbar setBackgroundImage:imaget forToolbarPosition:UIToolbarPositionBottom barMetrics:UIBarMetricsDefault];
    [[self navigationController].toolbar setAlpha:0.7];
    [imaget release];
//    imaget = [UIImage imageNamed:@"DefaultNavigationBar.png"];
//    [[self navigationController].navigationBar setBackgroundImage:imaget forBarMetrics:UIBarMetricsDefault];
//    [[self navigationController].navigationBar setAlpha:0.6];
//    [imaget release];

    /*
     set the security barbutton
     */  
    UIImage *image = [UIImage imageNamed:@"Tools.png"];
    UIImageView *imview = [[UIImageView alloc]initWithImage:image];
    imview.userInteractionEnabled = YES;
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(Tools)];
    [imview addGestureRecognizer:tap];
    [tap release];
    ToolBar = [[UIBarButtonItem alloc]initWithCustomView:imview];

    [imview release];
    [image release];
  
    UIImage *images = [UIImage imageNamed:@"share.png"];
    UIImageView *imviews = [[UIImageView alloc]initWithImage:images];
    imviews.userInteractionEnabled = YES;
    UITapGestureRecognizer *taps = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(OpenIn)];
    [imviews addGestureRecognizer:taps];
    [taps release];
    share = [[UIBarButtonItem alloc]initWithCustomView:imviews];
    
    [images release];
    [imviews release];
    
    [[self navigationItem]setRightBarButtonItems:[NSArray arrayWithObjects:searchButton, ToolBar,share,nil]];
   
   // [[self navigationController].navigationBar setBackgroundColor:[UIColor grayColor]];
  
    [self setView:view];
    
  
  
    [view release];
   
}
- (void)dealloc
{
    if (doc) {
		fz_document *self_doc = doc; // don't auto-retain self here!
		dispatch_async(queue, ^{
			//printf("close document\n");
			fz_close_document(self_doc);
		});
	}
    [[self navigationController].toolbar setBackgroundImage:nil forToolbarPosition:UIToolbarPositionBottom barMetrics:UIBarMetricsDefault];
    
	[indicator release]; indicator = nil;
	[slider release]; slider = nil;
	[sliderWrapper release]; sliderWrapper = nil;
	[searchBar release]; searchBar = nil;
	[outlineButton release]; outlineButton = nil;
	[searchButton release]; searchButton = nil;
	[cancelButton release]; cancelButton = nil;
	[prevButton release]; prevButton = nil;
	[nextButton release]; nextButton = nil;
	[canvas release]; canvas = nil;
    
	[outline release];outline = nil;
	[key release];key = nil;
    [alert release];alert = nil;
   // [handling release];handling = nil;
    [background release];background=nil;
    [Home release]; Home = nil;
    [password release];
    [password2 release];
  //  [PassView release];
    [security release];security = nil;
   // [PassView release];
    [permission release];permission = nil;
    [showSecurity release];showSecurity = nil;
    [addPassBoerd release];addPassBoerd = nil;
    [watermarkTool release];
    [annotationTool release];
    [multimdiaTool release];
    [popView release]; popView = nil;
   // [session release];session = nil;
    [recorder release]; recorder =nil;
    [share release];share = nil;
    [super dealloc];
}
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
#pragma  mark - text field delegate
- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    [textField resignFirstResponder];
    return YES;
}
#pragma mark - author suyuancheng
- (void) backAction
{
    [self.navigationController popViewControllerAnimated:YES];
}
- (void) viewWillLayoutSubviews
{
    CGSize size = [canvas frame].size;
    int max_width = fz_maxi(width, size.width);
    int max_height = fz_maxi(height, size.height);
    
    width = size.width;
    height = size.height;
    [canvas setContentSize:CGSizeMake(max_width*fz_count_pages(doc), height)];
    [canvas setContentInset:UIEdgeInsetsZero];
    [canvas setContentOffset:CGPointMake(current*width, 0) animated:YES];
    /*
     set the slider
     */
    [sliderWrapper setWidth:SLIDER_W];
    [[[self navigationController]toolbar] setNeedsLayout];
    /*
     set the canvas 
     */

//    [canvas setContentSize: CGSizeMake(fz_count_pages(doc) * max_width, height)];
//	[canvas setContentOffset: CGPointMake(current * width, 0)];
    /*
     layout the pageview
     */
    for(MuPageViewer *view in [canvas subviews])
    {
        [view setFrame:CGRectMake([view number]*width, 0, width-GAP, height)];
        [view resizeImage];
    }
    
    
    
    
}
- (void) viewWillDisappear:(BOOL)animated
{
    [[NSUserDefaults standardUserDefaults]removeObjectForKey:@"OpenDocumentKey"];
    [[NSUserDefaults standardUserDefaults]synchronize];
    [[self navigationController]setToolbarHidden:YES];
    
}
- (void) viewWillAppear:(BOOL)animated
{
    
    [slider setValue:current];
    [indicator setText:[NSString stringWithFormat: @"%d of %d",current+1,fz_count_pages(doc)]];
    [[self navigationController] setToolbarHidden: NO animated: animated];
    
}
- (void) viewDidAppear: (BOOL)animated
{
	[self scrollViewDidScroll: canvas];
}
/*
  the scrollview scrolled event
 */
- (void) scrollViewDidScroll:(UIScrollView *)scrollView
{
    if(width == 0)
        return;
    if(scroll_animating)
        return;
    /*
     get the curren page
     */
    float x = [canvas contentOffset].x+width*0.5f;
    current = x / width;
    [indicator setText:[NSString stringWithFormat:@"%d of %d",current+1,pages]];
    [slider setValue:current];
    //remember the current page number
    [[NSUserDefaults standardUserDefaults]setInteger:current forKey:key];
    /*
     delete the view that out the three distance for save memory
     */
    NSMutableSet *invisibleView = [[NSMutableSet alloc]init];
    for(MuPageViewer *view in [canvas subviews])
    {
        if([view number]<current-2||[view number]>current+2)
        {
            [invisibleView addObject:view];
        }
    }
    for(MuPageViewer *view in invisibleView)
    {
        [view removeFromSuperview];
        [view release];
    }
    [invisibleView release];
    
    [self createPageView:current-1];
    [self createPageView:current];
    [self createPageView:current+1];
    
}
- (void) createPageView: (int)number
{
    if(number<0||number>=pages)
        return;
 
    int find=0;
    for(MuPageViewer *p in [canvas subviews])
    {
         if( [p number]== number)
             find =1;
    }
    if(!find)
    {
           MuPageViewer *pageview = [[MuPageViewer alloc]initWithFrame:CGRectMake(width*number,0, width-GAP, height) document:doc page:number];
           [canvas addSubview:pageview];
          //  [pageview release];
      
    }
}
- (void) gotoPage: (int)number animated: (BOOL)animated
{
    if(number<0||number>=pages)
        return;
    if(current == number)
        return;
    
    if(animated){
        scroll_animating = YES;
        [UIView beginAnimations: @"MuScroll" context: NULL];
        [UIView setAnimationDuration: 0.4];
        [UIView setAnimationBeginsFromCurrentState: YES];
        [UIView setAnimationDelegate: self];
        [UIView setAnimationDidStopSelector: @selector(onGotoPageFinished)];
       
        [canvas setContentOffset: CGPointMake(number * width, 0)];
        [slider setValue: number];
        [indicator setText: [NSString stringWithFormat: @" %d of %d ", number+1, fz_count_pages(doc)]];
        
        [UIView commitAnimations];
        current = number;
    }
    else {
        [canvas setContentOffset:CGPointMake(number*width, 0)];
        current = number;
    }
}
- (void) onGotoPageFinished
{
	scroll_animating = NO;
	[self scrollViewDidScroll: canvas];
}
- (void) onShowOutline: (id)sender{}
- (void) onShowSearch: (id)sender
{
    searchBar = [[UISearchBar alloc]initWithFrame:CGRectMake(0, 0, 300, 32)];
     UIImage *imaget = [UIImage imageNamed:@"DefaultNavigationBar.png"];
    [searchBar setBackgroundImage:imaget];
    [imaget release];
    cancelButton = [[UIBarButtonItem alloc]initWithTitle:@"取消" style:UIBarButtonSystemItemCancel target:self action:@selector(onCancelSearch:)];
    [cancelButton setBackgroundImage:background forState:UIControlStateNormal barMetrics:UIBarMetricsDefault];
    [[self navigationItem]setRightBarButtonItem:cancelButton];
    [[self navigationItem]setTitleView:searchBar];
    [[self navigationItem]setLeftBarButtonItem:nil];
    [[self navigationItem]setHidesBackButton:YES];
    [searchBar setBarStyle:UIBarStyleDefault];
    [searchBar becomeFirstResponder ];
    [searchBar setDelegate:self];
    
}
- (void) onCancelSearch: (id)sender
{
    cancelSearch = YES;
    [searchBar removeFromSuperview];
    [searchBar resignFirstResponder];
    [pre removeFromSuperview];
    [next removeFromSuperview];
    
    [self.navigationItem setTitleView:nil];
    [[self navigationItem]setRightBarButtonItems:[NSArray arrayWithObjects:searchButton,ToolBar,share, nil]];
    [[self navigationItem]setLeftBarButtonItem:Home];
    
}
- (void) onSearchNext:(id)sender
{
    [self onSearch:1];
}
- (void) onSearchPre:(id)sender
{

    [self onSearch:-1];
}
- (void) onSearch:(int)direction
{
    char * needled = strdup([[ searchBar text]UTF8String]);
    int number = current+direction;;
    if(searchPage != current)
        number = current;
    
    cancelSearch = NO;
    [searchBar resignFirstResponder];
    [prevButton setEnabled:NO];
    [nextButton setEnabled:NO];
    dispatch_async(queue, ^{
        for(int i = number;i>=0&&i<fz_count_pages(doc);i+=direction)
        {
            int hit_n = search_page(doc,i,needled,NULL);
            if(hit_n)
            {
                dispatch_async(dispatch_get_main_queue(), ^{
                    [prevButton setEnabled:YES];
                    [nextButton setEnabled:YES];
                    [self showSearchResults:hit_n forPage:i];
                    free(needled);
                });
                return;
            }
            else 
            {
                if(cancelSearch){
                dispatch_async(dispatch_get_main_queue(), ^{
                    [prevButton setEnabled:YES];
                    [nextButton setEnabled:YES];
                    free(needled);
                });
                return;
                }
            }
        }
    });
    
}
/*
 the delegate function, the search button clicked on keyboard
 */
- (void)searchBarSearchButtonClicked:(UISearchBar *)sender
{
    //[[self navigationController]setToolbarHidden:YES];
    CGRect screen = [canvas frame];//[UIScreen mainScreen].bounds;
    pre = [[UIButton alloc]initWithFrame:CGRectMake(screen.size.width/2-100, screen.size.height-80, 100, 80)];
    UIImage *prebackground = [UIImage imageNamed:@"pre.png"];
    [pre setBackgroundImage:prebackground forState:UIControlStateNormal];
    [pre addTarget:self action:@selector(onSearchPre:) forControlEvents:UIControlEventTouchUpInside];
    
    next = [[UIButton alloc]initWithFrame:CGRectMake(screen.size.width/2,screen.size.height-80,100,80)];
    UIImage *nextbackground = [UIImage imageNamed:@"next.png"];
    [next setBackgroundImage:nextbackground forState:UIControlStateNormal];
    [next addTarget:self action:@selector(onSearchNext:) forControlEvents:UIControlEventTouchUpInside];
    
    [self.view addSubview:pre];
    [self.view addSubview:next];
    [self onSearchPre: sender];
    
    [prebackground release];
    [nextbackground release];
    [pre release];
    [next release];
}
- (void) resetSearch{}
/*
 the interface to mupageviewer
 */
- (void) showSearchResults: (int)count forPage: (int)number{

    printf("search found match on page %d\n", number);
    [self gotoPage:number animated:YES];
    searchPage = number;
    for(MuPageViewer *view in [canvas subviews])
    {
        if([view number] == number)
            [view showSearchResults:count];
    }
}
- (void) onSlide: (id)sender
{
    int number = [slider value];
    if([slider isTracking])
    {
        [indicator setText:[NSString stringWithFormat: @"%d of %d",number+1,fz_count_pages(doc)]];
    }
    else {
        [self gotoPage:number animated:NO];
    }
}
- (void) onTap: (UITapGestureRecognizer*)sender{

    if(popView)
    {
        [popView removeFromSuperview];
        [popView release];
         popView = nil;
    }
    CGPoint point = [sender locationInView:canvas];
    CGPoint ofs = [canvas contentOffset];
    CGSize screenSize = self.view.bounds.size;
    
    CGSize pagesize;
	fz_rect bounds;
	fz_bound_page(doc, fz_load_page(doc, current), &bounds);
	pagesize.width = bounds.x1 - bounds.x0;
	pagesize.height = bounds.y1 - bounds.y0;
    float hscale = pagesize.width / screenSize.width;
    float vscale = pagesize.height / screenSize.height;
    
    CGPoint po;
    po.x = point.x-ofs.x;
    
	point.x -= ofs.x;
	point.y -= ofs.y;
    fz_point pop;
    pop.x = point.x*hscale;
    pop.y =point.y*vscale;
    
    float x0 = (width - GAP)/3;
    float x1 = (width - GAP)-x0;
    
    pdf_annot *annot = pdf_get_multimedia_annot((pdf_document*)doc, current, pop);
    if(annot){
        int length =0;
        unsigned char * data= pdf_get_multimedia_data((pdf_document *)doc,annot, &length,NULL);
        NSData *nsdata = [[NSData alloc]initWithBytes:data length:length];
        AVAudioPlayer *player = [[AVAudioPlayer alloc]initWithData:nsdata  error:nil];
        [player setVolume:1.0];
        [player play];
    }
    else{
        
        if(po.x<x0)
        {
            [self gotoPage:current-1 animated:YES];
        }
        else if(po.x>x1)
        {
            [self gotoPage:current+1 animated:YES];
        }
        /*
         show/hidden the navigationbar
         */
        else {
            
            if ([[self navigationController]isNavigationBarHidden])
                [self showNavigationBar];
            else {
                [self hideNavigationBar];
            }
        }
        
    }
    
    
 
}
- (void)longPressAction:(id)sender
{
   // [[UIApplication sharedApplication]openURL:[NSURL URLWithString:@"http://www.baidu.com"]];
    //CGPoint point = [sender locationInView:imageView];
    //longPress = 1;
	p = [sender locationInView: canvas];
	CGPoint ofs = [canvas contentOffset];
    CGSize screenSize = self.view.bounds.size;
    
    CGSize pagesize;
	fz_rect bounds;
	fz_bound_page(doc, fz_load_page(doc, current), &bounds);
	pagesize.width = bounds.x1 - bounds.x0;
	pagesize.height = bounds.y1 - bounds.y0;
    float hscale = pagesize.width / screenSize.width;
    float vscale = pagesize.height / screenSize.height;
    
    
	p.x -= ofs.x;
	p.y -= ofs.y;
    fz_point po;
    po.x = p.x*hscale;///1.215126;
    po.y =p.y*vscale;///1.2161521;
    pressPosion = po;

    if(nil ==popView||0 == popView.count)
    {
        popView = [[MuPopView alloc]initWithFrame:CGRectMake(p.x-100, p.y-50, 200, 50)];
//        [popView addButtonWithTitle:[NSArray arrayWithObjects:@"Music",@"Vdeio",@"waterMark", nil]];
        [popView addButtonWithTitle:[NSArray arrayWithObjects:@"音乐",@"视频",@"水印",@"文字", nil]];
        [popView setMyDelegate:self];
        [self.view addSubview:popView];
    }
}
- (void) showNavigationBar{
    if ([[self navigationController]isNavigationBarHidden])
    {
        [[self navigationController]setNavigationBarHidden:NO];
        [[self navigationController]setToolbarHidden:NO];
        [indicator setHidden:NO];
        [slider setHidden:NO];
        
        [UIView beginAnimations:@"HIDDENBAR" context:NULL];
        [UIView setAnimationBeginsFromCurrentState:YES];
        [UIView setAnimationDuration:1];
        [UIView setAnimationDelegate:self];
        [UIView setAnimationDidStopSelector:@selector(hideIdicator)];

        [[[self navigationController] navigationBar] setAlpha: 0.8];
		[[[self navigationController] toolbar] setAlpha: 0.7];
		[indicator setAlpha: 1];
        
        [UIView commitAnimations];

    }
}
-(void) hideIdicator
{
    [indicator setAlpha: 0];
}
- (void) hideNavigationBar{
    if (![[self navigationController]isNavigationBarHidden]) {

        [UIView beginAnimations:@"HIDDENBAR" context:NULL];
        [UIView setAnimationDelegate:self];
        [UIView setAnimationDidStopSelector:@selector(onhiden)];
        
        [[[self navigationController] navigationBar] setAlpha: 0];
		[[[self navigationController] toolbar] setAlpha: 0];
		[indicator setAlpha: 0];
        
        [UIView commitAnimations];
           }

}
- (void)onhiden
{
    [[self navigationController]setNavigationBarHidden:YES];
    [[self navigationController]setToolbarHidden:YES];
    [indicator setHidden:YES];
//    [slider setHidden:YES];

}
#pragma mark - Tools
- (void)Tools
{
    /*
     set tool bar items, new funtion
     */
    [[self navigationController]setToolbarHidden:NO];
    UIBarButtonItem *done = [[UIBarButtonItem alloc]initWithTitle:@"完成" style:UIBarButtonItemStyleDone target:self action:@selector(cancelTools)];
    [done setBackgroundImage:background forState:UIControlStateNormal barMetrics:UIBarMetricsDefault];
    [[self navigationItem]setRightBarButtonItems:[NSArray arrayWithObjects: done,nil]];
    [[self navigationItem]setHidesBackButton:YES];
    [self navigationItem].leftBarButtonItem = nil;
    [[self navigationItem]setTitle:@"选择工具"];
    
    UIImage *image = [UIImage imageNamed:@"lock.png"];
    UIImageView *imageview = [[UIImageView alloc]initWithImage:image];
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(passWordTool)];
    [imageview addGestureRecognizer:tap];
    imageview.userInteractionEnabled = YES;
    passTool = [[UIBarButtonItem alloc]initWithCustomView:imageview];
    
    [imageview release];
    [image release];
    [tap release];
    
    image = [UIImage imageNamed:@"MultiMedia.png"];
    imageview = [[UIImageView alloc]initWithImage:image];
    tap = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(multimediaTool)];
    [imageview addGestureRecognizer:tap];
    imageview.userInteractionEnabled = YES;
    multimdiaTool = [[UIBarButtonItem alloc]initWithCustomView:imageview];
   
    [imageview release];
    [image release];
    [tap release];
    
   image = [UIImage imageNamed:@"Annotation.png"];
    imageview = [[UIImageView alloc]initWithImage:image];
    tap = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(annotationTool)];
    [imageview addGestureRecognizer:tap];
    imageview.userInteractionEnabled = YES;
    annotationTool = [[UIBarButtonItem alloc]initWithCustomView:imageview];
   
    [imageview release];
    [image release];
    [tap release];
    
    image = [UIImage imageNamed:@"WaterMark.png"];
    imageview = [[UIImageView alloc]initWithImage:image];
    tap = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(watermarkTool)];
    [imageview addGestureRecognizer:tap];
    imageview.userInteractionEnabled = YES;
    watermarkTool = [[UIBarButtonItem alloc]initWithCustomView:imageview];
    
    [imageview release];
    [image release];
    [tap release];

    [self setToolbarItems:[NSArray arrayWithObjects:passTool,multimdiaTool,watermarkTool,annotationTool, nil]  animated:YES];
}
- (void)cancelTools
{
    if(security)
        [security removeFromSuperview];
    [[self navigationItem]setTitle:nil];
    [[self navigationItem]setHidesBackButton:YES];
    [[self navigationItem]setLeftBarButtonItem:Home];
    [self setToolbarItems:[NSArray arrayWithObjects:sliderWrapper, nil]];
    [[self navigationController]setToolbarHidden:NO];
    [self.navigationItem setTitleView:nil];
    [[self navigationItem]setRightBarButtonItems:[NSArray arrayWithObjects:searchButton,ToolBar,share, nil]];

}

- (void)cancelTool
{
    [[self navigationController]setToolbarHidden:NO];
    UIBarButtonItem *done = [[UIBarButtonItem alloc]initWithTitle:@"完成" style:UIBarButtonItemStyleDone target:self action:@selector(cancelTools)];
    [done setBackgroundImage:background forState:UIControlStateNormal barMetrics:UIBarMetricsDefault];
    [[self navigationItem]setRightBarButtonItems:[NSArray arrayWithObjects: done,nil]];
    [done release];
    
    [[self navigationItem]setHidesBackButton:YES];
    [self navigationItem].leftBarButtonItem = nil;
    [[self navigationItem]setTitle:@"选择工具"];
    [self setToolbarItems:[NSArray arrayWithObjects:passTool,multimdiaTool,watermarkTool,annotationTool, nil]  animated:YES];
}
#pragma mark - multimedia
- (void)multimediaTool
{
    /*
     set the tool bar and navigation bar
     */
    UIBarButtonItem *done = [[UIBarButtonItem alloc]initWithTitle:@"完成" style:UIBarButtonItemStyleDone target:self action:@selector(cancelTool)];
    [done setBackgroundImage:background forState:UIControlStateNormal barMetrics:UIBarMetricsDefault];
    [[self navigationItem]setRightBarButtonItems:[NSArray arrayWithObjects: done,nil]];
    [[self navigationItem]setTitle:@"添加音视频"];
    [done release];
    
    UIImage *image = [UIImage imageNamed:@"Music.png"];
    UIImageView *imageview = [[UIImageView alloc]initWithImage:image];
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(addMusic)];
    [imageview addGestureRecognizer:tap];
    imageview.userInteractionEnabled = YES;
    UIBarButtonItem *Music = [[UIBarButtonItem alloc]initWithCustomView:imageview];
    [imageview release];
    [image release];
    [tap release];
    
    image = [UIImage imageNamed:@"Vedio.png"];
    imageview = [[UIImageView alloc]initWithImage:image];
    tap = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(addVedio)];
    [imageview addGestureRecognizer:tap];
    imageview.userInteractionEnabled = YES;        
    UIBarButtonItem * Vedio = [[UIBarButtonItem alloc]initWithCustomView:imageview ];
    [imageview release];
    [image release];
    [tap release];

    UIBarButtonItem *space = [[UIBarButtonItem alloc]initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil];
    [self setToolbarItems:[NSArray arrayWithObjects:space,Music,space,Vedio, space,nil]];
    [space release];
    [Music release];
    [Vedio release];
}
- (void)annotationTool
{
    /*
     set the tool bar and navigation bar
     */
    UIBarButtonItem *done = [[UIBarButtonItem alloc]initWithTitle:@"完成" style:UIBarButtonItemStyleDone target:self action:@selector(cancelTool)];
    [done setBackgroundImage:background forState:UIControlStateNormal barMetrics:UIBarMetricsDefault];
    [[self navigationItem]setRightBarButtonItems:[NSArray arrayWithObjects: done,nil]];
    [[self navigationItem]setTitle:@"添加注释"];
    [done release];
}
- (void)watermarkTool
{
    /*
     set the tool bar and navigation bar
     */
    UIBarButtonItem *done = [[UIBarButtonItem alloc]initWithTitle:@"完成" style:UIBarButtonItemStyleDone target:self action:@selector(cancelTool)];
    [done setBackgroundImage:background forState:UIControlStateNormal barMetrics:UIBarMetricsDefault];
    [[self navigationItem]setRightBarButtonItems:[NSArray arrayWithObjects: done,nil]];
    [[self navigationItem]setTitle:@"添加水印"];
    [done release];
    
    UIImage *image = [UIImage imageNamed:@"PicWatermark.png"];
    UIImageView *imageview = [[UIImageView alloc]initWithImage:image];
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(addPicWatermark)];
    [imageview addGestureRecognizer:tap];
    imageview.userInteractionEnabled = YES;
    UIBarButtonItem *Picture = [[UIBarButtonItem alloc]initWithCustomView:imageview];
    [imageview release];
    [image release];
    [tap release];
    
    image = [UIImage imageNamed:@"WordWatermark.png"];
    imageview = [[UIImageView alloc]initWithImage:image];
    tap = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(addTextWatermark)];
    [imageview addGestureRecognizer:tap];
    imageview.userInteractionEnabled = YES;        
    UIBarButtonItem * Word = [[UIBarButtonItem alloc]initWithCustomView:imageview ];
    [imageview release];
    [image release];
    [tap release];
    
    UIBarButtonItem *space = [[UIBarButtonItem alloc]initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil];
    [self setToolbarItems:[NSArray arrayWithObjects:space,Picture,space,Word, space,nil]];
    [space release];
    [Picture release];
    [Word release];
}
# pragma  mark - multimedia music and vedio
- (void)addMusic
{
//    CGSize size = [UIScreen mainScreen].bounds.size;
//    MyActionSheet *ac = [[MyActionSheet alloc]initWithtitle:@"文件选取"frame:size];
//    MuFileManageWithType *m = [[MuFileManageWithType alloc]initWithType:@"M"];
//  
//    UITableView *table = [[UITableView alloc]initWithFrame:CGRectMake(0, 0,size.width, size.height-32) style:UITableViewStyleGrouped];
//    [table setDelegate: m];
// 
//    [table setDataSource:m];
//    [ac.View addSubview:table];
//    [ac setMyDelegate:m];
//    [ac addDoneBtn];
//    [m setMyDelegate:self];
//    [ac showFromToolbar:[self navigationController].toolbar];
    [self recoderSound];
    
}
- (void) recoderSound
{
    UIImage *image = [UIImage imageNamed:@"recoder_start.png"];
    recoderbtn = [UIButton buttonWithType:UIButtonTypeCustom];
    recoderbtn.frame = CGRectMake(p.x ,p.y, 40, 27);
    [recoderbtn setImage: image forState:UIControlStateNormal];
    [recoderbtn addTarget:self action:@selector(recoderAction) forControlEvents:UIControlEventTouchUpInside];
    [recoderbtn setHidden:NO];
    [self.view addSubview:recoderbtn];

}
- (void)recoderAction
{
   
    if(!self.isRecording)
    {
        self.isRecording = YES;
        UIImage *image = [UIImage imageNamed:@"recoder.png"];
        [recoderbtn setImage:image forState:UIControlStateNormal];
        recorder = [[AVAudioRecorder alloc] initWithURL:recordedFile settings:nil error:nil];
        if(recorder !=nil)
        {
         if([recorder prepareToRecord])
            [recorder record];
        }
    }
    else {
        self.isRecording = NO;
       
        [recoderbtn removeFromSuperview];
        [recorder stop];
        recorder = nil;
        [self OkforAddMusic:self];
    }
}
- (void)addVedio
{

}
- (void)addPicWatermark
{
    UIActionSheet *sheet;
    
    // 判断是否支持相机
    if([UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeCamera])
    {
        sheet  = [[UIActionSheet alloc] initWithTitle:@"选择图像" delegate:self cancelButtonTitle:nil destructiveButtonTitle:@"取消" otherButtonTitles:@"拍照", @"从相册选择", nil];
    }
    else {
        sheet = [[UIActionSheet alloc] initWithTitle:@"选择图像" delegate:self cancelButtonTitle:nil destructiveButtonTitle:@"取消" otherButtonTitles:@"从相册选择", nil];
    }
    
    sheet.tag = 255;
    
    [sheet showFromToolbar:[self navigationController].toolbar];
}
- (void)addTextWatermark
{
    CGSize si = [UIScreen mainScreen].bounds.size;
    pressPosion.x = si.width/2;
    pressPosion.y = si.height/2;
    [self OkforAddTextWatermark:self];
}

#pragma mark - action sheet delegte
- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (actionSheet.tag == 255) {
        NSUInteger sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
        // 判断是否支持相机
        if([UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeCamera]) {
            switch (buttonIndex) {
                case 0:
                    return;
                case 1: //相机
                    sourceType = UIImagePickerControllerSourceTypeCamera;
                    break;
                case 2: //相册
                    sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
                    break;
            }
        }
        else {
            if (buttonIndex == 0) {
                return;
            } else {
                sourceType = UIImagePickerControllerSourceTypeSavedPhotosAlbum;
            }
        }
        // 跳转到相机或相册页面
        UIImagePickerController *imagePickerController = [[UIImagePickerController alloc] init];
        imagePickerController.delegate = self;
        imagePickerController.allowsEditing = YES;
        imagePickerController.sourceType = sourceType;
       // kUTTypeImage
        
        [self presentViewController:imagePickerController animated:YES completion:^{}];
    }
}

#pragma mark - image picker delegte
- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    [picker dismissViewControllerAnimated:YES completion:^{}];
    
    {
        //先把图片转成NSData
        UIImage* image = [info objectForKey:@"UIImagePickerControllerOriginalImage"];
        NSData *data;
       // if (UIImagePNGRepresentation(image) == nil)
        {
            data = UIImageJPEGRepresentation(image, 1.0);
            
            //图片保存的路径
            //这里将图片放在沙盒的documents文件夹中
            NSString * DocumentsPath = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"];  
            
            //文件管理器
            NSFileManager *fileManager = [NSFileManager defaultManager];
            
            //把刚刚图片转换的data对象拷贝至沙盒中 并保存为image.png
            [fileManager createDirectoryAtPath:DocumentsPath withIntermediateDirectories:YES attributes:nil error:nil];
            if([fileManager fileExistsAtPath:[DocumentsPath stringByAppendingString:@"/image.jpg"] ])
            {
                [fileManager removeItemAtPath:[DocumentsPath stringByAppendingString:@"/image.jpg"] error:nil];
            }
            [fileManager createFileAtPath:[DocumentsPath stringByAppendingString:@"/image.jpg"] contents:data attributes:nil];
            
            //得到选择后沙盒中图片的完整路径
             imagePath = [[NSString alloc]initWithFormat:@"%@%@",DocumentsPath,  @"/image.jpg"];
            [self OkforAddPicWatermark:self];
         
        }
    }
    //关闭相册界面
}
- (void) imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
    [picker dismissModalViewControllerAnimated:YES];
}
#pragma mark - watermark
//- (void)addPicWatermark
//{
//   
//}
- (void)choosePicWatermark
{
    CGSize size = [UIScreen mainScreen].bounds.size;
    MyActionSheet *ac = [[MyActionSheet alloc]initWithtitle:@"文件选取" frame:size];
    MuFileManageWithType *m = [[MuFileManageWithType alloc]initWithType:@"P"];
    
    UITableView *table = [[UITableView alloc]initWithFrame:CGRectMake(0, 0,size.width, size.height-32) style:UITableViewStyleGrouped];
    [table setDelegate: m];
    
    [table setDataSource:m];
    [ac.View addSubview:table];
    [ac setMyDelegate:m];
    [ac addDoneBtn];
    [m setMyDelegate:self];
    [ac showFromToolbar:[self navigationController].toolbar];
}
#pragma mark - password
- (void)passWordTool
{
    /*
     set tool bar items, new funtion
     */
//    [[self navigationController]setToolbarHidden:NO];
    UIBarButtonItem *done = [[UIBarButtonItem alloc]initWithTitle:@"完成" style:UIBarButtonItemStyleDone target:self action:@selector(cancelTool)];
    [done setBackgroundImage:background forState:UIControlStateNormal barMetrics:UIBarMetricsDefault];
    [[self navigationItem]setRightBarButtonItems:[NSArray arrayWithObjects: done,nil]];
//    [[self navigationItem]setHidesBackButton:YES];
//    [self navigationItem].leftBarButtonItem = nil;
    [[self navigationItem]setTitle:@"密码操作"];
    [done release];
     //per={0};
    memset(per, 0, sizeof(int));
    if(!pdf_allow_high_print((pdf_document*)doc))
        per[0] = 1;    
    if(!pdf_allow_low_print((pdf_document*)doc))
        per[1]=1;    
    if(!pdf_allow_content_changes((pdf_document*)doc))
        per[2]=1;    
    if(!pdf_allow_copy((pdf_document*)doc))
        per[3]=1;    
    if(!pdf_allow_add_annotation((pdf_document*)doc))
        per[4]=1;    
    if(!pdf_allow_extract((pdf_document*)doc))
        per[5]=1;    
    if(!pdf_allow_fill_form((pdf_document*)doc))
        per[6]=1;
    if(!pdf_allow_page_handle((pdf_document*)doc))
        per[7]=1;
    
    UIImage *image = [UIImage imageNamed:@"AddPassword1.png"];
    UIImageView *imageview = [[UIImageView alloc]initWithImage:image];
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(addPassword)];
    [imageview addGestureRecognizer:tap];
    imageview.userInteractionEnabled = YES;
    UIBarButtonItem *addPass = [[UIBarButtonItem alloc]initWithCustomView:imageview];
    [imageview release];
    [image release];
    [tap release];
    
    image = [UIImage imageNamed:@"RemovePassword.png"];
    imageview = [[UIImageView alloc]initWithImage:image];
    tap = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(removePassword)];
    [imageview addGestureRecognizer:tap];
    imageview.userInteractionEnabled = YES;        
    UIBarButtonItem * remove = [[UIBarButtonItem alloc]initWithCustomView:imageview ];
    [imageview release];
    [image release];
    [tap release];
    
    image = [UIImage imageNamed:@"ModifyPassword.png"];
    imageview = [[UIImageView alloc]initWithImage:image];
    tap = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(showSecuritys)];
    [imageview addGestureRecognizer:tap];
    imageview.userInteractionEnabled = YES;   
    //UIBarButtonItem * modify = [[UIBarButtonItem alloc]initWithCustomView:imageview];
   securitybarbutton = [[UIBarButtonItem alloc]initWithCustomView:imageview];
    [imageview release];
    [image release];
    [tap release];
    
    image = [UIImage imageNamed:@"ModifyPermission.png"];
    imageview = [[UIImageView alloc]initWithImage:image];
    tap = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(modifyPermission)];
    [imageview addGestureRecognizer:tap];
    imageview.userInteractionEnabled = YES;
    UIBarButtonItem * permission = [[UIBarButtonItem alloc]initWithCustomView:imageview];
    [imageview release];
    [image release];
    [tap release];
    UIBarButtonItem *space = [[UIBarButtonItem alloc]initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil];
    [self setToolbarItems:[NSArray arrayWithObjects:addPass,space,remove,space,securitybarbutton,space,permission, nil]];
}
- (void)addPassword
{
    if(!pdf_has_encrypt((pdf_document*)doc))
    {
        UIAlertView *al = [[UIAlertView alloc]initWithTitle:@"Password Handle" message:@"本文档已经加密" delegate:self cancelButtonTitle:@"OK" otherButtonTitles: nil];
        [al show];
        [al release];
        return;
    }
    addPassBoerd = [[MuAlertView alloc]initWithStyle:FORADDPASSWORD];
    [addPassBoerd setMyDelegate:self];
    [addPassBoerd show];


}

- (void)removePassword
{
    if(pdf_has_encrypt((pdf_document*)doc))
    {
        UIAlertView *al = [[UIAlertView alloc]initWithTitle:@"Password Handle" message:@"本文档没有加密" delegate:self cancelButtonTitle:@"OK" otherButtonTitles: nil];
        [al show];
        [al release];
        return;
    }
    _passwordAction = REMOVEPASSWORD;
//    alert = [[UIAlertView alloc]initWithFrame:CGRectMake(50, 50, 200, 300)];
    alert = [[UIAlertView alloc]initWithTitle:@"WARNNING" message:@"是否确认移除密码" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"OK", nil];
    [alert setTag:2];
    [alert show];
    //[alert release];
}
- (void)showSecuritys
{
   // [[self navigationController]setToolbarHidden:YES];
    if(nil == showSecurity)
    {
        CGSize sizes = [[UIScreen mainScreen]bounds].size;
        showSecurity = [[MyActionSheet alloc]initWithtitle:@"文档控制权限" frame:sizes];
        UITableView *table = [[UITableView alloc]initWithFrame:CGRectMake(0, 0,sizes.width, sizes.height-32) style:UITableViewStyleGrouped];
        

        MuSecurityDataSource *dataSource = [[MuSecurityDataSource alloc]initWithdPermission:per];
        [table setDataSource:dataSource];
        [table setDelegate:dataSource];
        
        [showSecurity addDoneBtn];
        [showSecurity.View addSubview:table];
        [showSecurity setMyDelegate:dataSource];
        
        [dataSource release];
        [table release];
    }
//    [showSecurity showInView:self.view];
    [showSecurity showFromToolbar:[self navigationController].toolbar];
   



}

- (void)modifyPermission
{
    if(nil == permission)
    {
        CGSize size = [[UIScreen mainScreen]bounds].size;
        permission = [[MyActionSheet alloc]initWithtitle:@"访问权限控制" frame:size];
        UITableView *table = [[UITableView alloc]initWithFrame:CGRectMake(0, 0,size.width, size.height-32) style:UITableViewStyleGrouped];
        [table setDelegate: self];
        [table setDataSource:self];
        
        [permission addDoneBtn];
        [permission addCancelBtn];
        [permission.View addSubview:table];
        [permission setMyDelegate:self];
        [table release];
    }
    [permission showFromToolbar:[self navigationController].toolbar];
}
#pragma mark - muactionsheetactiondeleglate
- (void)onCancel:(id)sender
{
    [sender dismissWithClickedButtonIndex:0 animated:YES];
}
- (void)onDone:(id)sender
{
    [sender dismissWithClickedButtonIndex:1 animated:YES];
  //  if([permissions count] > 0)
    {
    MuAlertView *confirm = [[MuAlertView alloc]initWithStyle:FORPERMISSIONCONTROL];
    [confirm setMyDelegate: self];
    [confirm show];
    }
    
}
- (void)OKforAddPassword:(id)sender
{
    
    if([addPassBoerd.userPWD length]==0||[addPassBoerd.ownPWD length]==0)
    { 
        UILabel *warnning = [[UILabel alloc]initWithFrame:CGRectMake(50, 10, 200, 20)];
        [warnning setTextColor:[UIColor redColor]];
        [warnning setText:@"密码不能为空！"];
        return;
    }
    else {
        
        handling =[[UIActivityIndicatorView alloc]initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
        [handling setFrame:CGRectMake(100, 100, 100, 100)];
        [handling setBackgroundColor:[UIColor blackColor]];
        [handling setAlpha:0.5];
        [handling startAnimating];
        [self.view addSubview:handling];
        [handling release];
        
        
        dispatch_async(queue,^{
            printf("%s",filename);
            const char *pass = [addPassBoerd.userPWD UTF8String];
            const char *own = [addPassBoerd.ownPWD UTF8String];
            doc = (fz_document*)pdf_add_password((pdf_document*)doc, filename, own,pass, ANNOTATION);
          
            dispatch_async(dispatch_get_main_queue(), ^{//stop the activity in time
                if(handling)
                {
                    [handling stopAnimating];
                }
            });

        });
        
    }
    
    
}
- (void)OKforPermissionChange:(id)sender
{
    const char *pass = [((MuAlertView *)sender).userPWD UTF8String];
    const char *own = [((MuAlertView *)sender).ownPWD UTF8String];
    if(!pdf_authenticate_password((pdf_document *)doc, pass))
        return;
    
    if(!pdf_authenticate_password((pdf_document *)doc, own))
        return;   
    
    int permissionsForChange = 0;
    
    if(permissionChanges[0])
        permissionsForChange |= HIGHPRINT; 
    else {
        permissionsForChange &= NOHIGHPRINT;
    }
    if(permissionChanges[1])
        permissionsForChange |= LOWPRINT; 
    else {
        permissionsForChange &= NOLOWPRINT;
    }
    if(permissionChanges[2])
        permissionsForChange |= CHANGES; 
    else {
        permissionsForChange &= NOCHANGES;
    }
    if(permissionChanges[3])
        permissionsForChange |= COPY; 
    else {
        permissionsForChange &= NOCOPY;
    }
    if(permissionChanges[4])
        permissionsForChange |= ANNOTATION; 
    else {
        permissionsForChange &= NOANNOTATION;
    }
    if(permissionChanges[5])
        permissionsForChange |= FILL; 
    else {
        permissionsForChange &= NOFILL;
    }
    if(permissionChanges[6])
        permissionsForChange |= EXTRACT; 
    else {
        permissionsForChange &= NOEXTRACT;
    }
    if(permissionChanges[7])
        permissionsForChange |= PAGEHANDLE; 
    else {
        permissionsForChange &= NOPAGEHANDLE;
    }
    /*
     密码进不去则不能用多线程
     */
    pdf_modify_permission((pdf_document*)doc, filename, pass, own, permissionsForChange);
    
//    handling =[[UIActivityIndicatorView alloc]initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
//    [handling setFrame:CGRectMake(100, 100, 100, 100)];
//    [handling setBackgroundColor:[UIColor blackColor]];
//    [handling setAlpha:0.5];
//    [handling startAnimating];
//    [self.view addSubview:handling];
//    [handling release];
//    
//    dispatch_async(queue, ^{
//        pdf_modify_permission((pdf_document*)doc, filename, pass, own, permissionsForChange);
//        dispatch_async(dispatch_get_main_queue(), ^{
//            if(handling)
//            {
//                [handling stopAnimating];
//            }
//        });
//    });

}

- (void)onButtonClick:(NSInteger)tag sender:(id)sender
{
    if( 0== tag)
    {
        [self addMusic];
    }
    if( 1== tag)
    {
        //[self addMusic];
    }
    if( 2== tag)
    {
        [self addPicWatermark];
       // [self addMusic];
    }
}
- (void)OkforAddMusic:(id)sender
{

    fz_rect rect = {pressPosion.x-16,pressPosion.y-16,pressPosion.x+16,pressPosion.y+16};
    pdf_set_multimedia_ap( [[[NSBundle mainBundle]pathForResource:@"sound" ofType:@"jpg"]UTF8String]);
//    doc = (fz_document *)pdf_add_multimedia((pdf_document*)doc, current, rect, [((MuFileManageWithType *)sender).selectedFile UTF8String] );
    
     doc = (fz_document *)pdf_add_multimedia((pdf_document*)doc, current, rect, [[recordedFile path]UTF8String]);
    fz_write_options op = {0};
    op.do_incremental = 1;
    op.do_garbage =1;
    fz_write_document(doc, filename,&op);
 
    fz_close_document(doc);
    doc = fz_open_document(ctx, filename);
    fz_load_page(doc, current);
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    [fileManager removeItemAtURL:recordedFile error:nil];
    recordedFile = nil;
}
- (void)OkforAddPicWatermark:(id)sender
{
//    if(0 == [((MuFileManageWithType *)sender).selectedFile length])
//        return;
// 
    if( 0 == [imagePath length])
        return;
   UIImage *image = [UIImage imageWithContentsOfFile:imagePath];
//    UIImage *image = [UIImage imageWithContentsOfFile:((MuFileManageWithType *)sender).selectedFile];
    float _width = image.size.width;
    float _height = image.size.height;

    
     CGSize size = [UIScreen mainScreen].bounds.size;
    CGSize pagesize;
	fz_rect bounds;
	fz_bound_page(doc, fz_load_page(doc, current), &bounds);
	pagesize.width = bounds.x1 - bounds.x0;
	pagesize.height = bounds.y1 - bounds.y0;
    
    
    float centerx = pagesize.width/2;
    float centery = pagesize.height/2;
    float offsetX = (_width/2);
    float offsetY = (_height/2);

    fz_rect rect = {centerx-offsetX,centery-offsetY,centerx+offsetX,centery+offsetY};
//    pdf_create_watermark_with_img((pdf_document*)doc, current,  [((MuFileManageWithType *)sender).selectedFile UTF8String], _width, _height, &rect);
    pdf_create_watermark_with_img((pdf_document*)doc, current,  [imagePath UTF8String], _width, _height, &rect);
    fz_write_options op = {0};
    op.do_incremental = 1;
    op.do_garbage =1;
    fz_write_document(doc, filename,&op);
    fz_close_document(doc);
    doc = fz_open_document(ctx, filename);
    fz_load_page(doc, current);
}

- (void)OkforAddTextWatermark:(id)sender
{

    UIAlertView *test = [[UIAlertView alloc]initWithTitle:@"添加水印" message:@"添加文字水印" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"OK", nil];
    [test setAlertViewStyle:UIAlertViewStylePlainTextInput];
    [[test textFieldAtIndex:0]setPlaceholder:@"text"];
    [test show];
    [test release];

}
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
	
	[alertView dismissWithClickedButtonIndex: buttonIndex animated: TRUE];
    
	if (buttonIndex == 1) {
        if(2 == [alertView tag])
            {
                handling =[[UIActivityIndicatorView alloc]initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
                [handling setFrame:CGRectMake(100, 100, 100, 100)];
                [handling setBackgroundColor:[UIColor blackColor]];
                [handling setAlpha:0.5];
                [handling startAnimating];
                [self.view addSubview:handling];
                [handling release];
                dispatch_async(queue, ^{
                    doc = (fz_document*)pdf_remove_password((pdf_document*)doc,filename );
                    dispatch_async(dispatch_get_main_queue(), ^{
                        if(handling)
                        {
                            [handling stopAnimating];
                        }
                    });
                });
                
            }

        else {
            
            NSString *text = [[alertView textFieldAtIndex:0]text];
            if(0 != [text length])
            {                   
                fz_rect bounds;
                fz_bound_page(doc, fz_load_page(doc, current), &bounds);
                float height = bounds.y1 - bounds.y0;
                fz_rect rect = {pressPosion.x, height-pressPosion.y,pressPosion.x+20*[text length],height-pressPosion.y+100};
                //    fz_rect rect = {pressPosion.x-150,0,pressPosion.x+150,20};
                printf("%s\n", [text UTF8String]);
                pdf_create_watermark_with_string((pdf_document*)doc, [text UTF8String], rect);
                fz_write_options op = {0};
                op.do_incremental = 1;
                op.do_garbage =1;
                fz_write_document(doc, filename,&op);
                fz_close_document(doc);
                doc = fz_open_document(ctx, filename);
                fz_load_page(doc, current);
                
            }
            
        } 
    }
    else {
        //[self onPasswordCancel];        
    }

}

#pragma  mark - table view data source and deleglate
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return 8;
}
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"Cell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    NSArray *permit = [NSArray arrayWithObjects:@"高质打印",@"低质打印",@"内容可变", @"允许复制",@"添加注释",@"填写表单", @"拉取文本",@"页面操作", nil];
    if (!cell)
    {
        cell = [[[UITableViewCell alloc] initWithStyle: UITableViewCellStyleDefault reuseIdentifier: CellIdentifier] autorelease];
        UISwitch *sw = [[UISwitch alloc]initWithFrame:CGRectMake(0, 0, 35, 35)];
        if(per[indexPath.row])
            sw.on = YES;
        [sw setTag:indexPath.row];
        [sw addTarget:self action:@selector(swithcAction:) forControlEvents:UIControlEventValueChanged];
        [cell setAccessoryView:sw];
        [[cell textLabel]setText:[permit objectAtIndex:indexPath.row]];
        [sw release];
    }
    return  cell;
    
}


#pragma  mark - uiswitch  event
- (void)swithcAction:(id)sender
{
    if(((UISwitch *)sender).on)   
        permissionChanges[((UISwitch*)sender).tag] =1;
    else {
        permissionChanges[((UISwitch*)sender).tag] =0;
    }
      //  [permissions addObject:((UISwitch*)sender).tag];
}

#pragma mark- open in other app
-(void)OpenIn
{
    NSURL *url = [NSURL fileURLWithPath:[NSString stringWithUTF8String:filename]];
    UIDocumentInteractionController *documentHandle = [UIDocumentInteractionController interactionControllerWithURL:url];
    documentHandle.UTI = @"com.adobe.pdf";
    documentHandle.delegate = self;
    [documentHandle presentPreviewAnimated:YES];
}

#pragma mark - documentinteraction delegate
- (UIViewController *)documentInteractionControllerViewControllerForPreview:(UIDocumentInteractionController *)controller
{
    return  self;
}

#pragma mark- unlock pheoniex ebbok
- (void) unlock
{
    char *utf8;
    pdf_obj *root;
    pdf_obj *acroform;
    pdf_obj *form;
    int i = 0;
    char *creatorV = "Vitrium Systems Inc";
    char *creator;
    //直接执行解锁部分函数
    // pdf_js_execute(((pdf_document *)doc)->js, "r();");
    
    /*
     handler the vitrium system inc ebook
     */
    creator = pdf_to_str_buf(pdf_dict_getp(pdf_trailer((pdf_document *)doc), "Info/Creator"));
    if(strncmp(creator, creatorV, 19))
    {
        return;
    }
    
    /*
     *与js不交互，直接解锁
     */
    utf8 = "boom";
    root = pdf_dict_gets(pdf_trailer((pdf_document *)doc), "Root");
    acroform = pdf_dict_gets(root, "AcroForm");
    form = pdf_dict_gets(acroform, "Fields");
    
    //find the boom.info
    pdf_obj * dict = pdf_lookup_field(form, utf8);
    pdf_obj * dictO = pdf_load_object((pdf_document *)doc, pdf_to_num(dict), 0);
    //hidden the boom.info
    pdf_field_set_display((pdf_document *)(doc), dictO, 1);
    // pdf_update_object((pdf_document *)doc, pdf_to_num(dictO), 0);
    
    //set the ocgs properties to show the content
    pdf_obj  *ocg = pdf_dict_gets(pdf_dict_gets(pdf_trailer((pdf_document *)doc), "Root"), "OCProperties");
    pdf_dict_dels(pdf_dict_gets(pdf_trailer((pdf_document*)doc), "Root"), "AA");
    pdf_obj *array = pdf_dict_gets(ocg, "OCGs");//get the array of the ocgs
    //    pdf_print_obj(ocg);
    pdf_obj *D = pdf_dict_gets(ocg, "D");
    pdf_obj *On = pdf_dict_gets(D, "ON");
    pdf_obj *Off = pdf_dict_gets(D,"OFF");

    pdf_dict_dels(D, "ON");
    pdf_dict_dels(D, "OFF");
    
    
    int len = pdf_array_len(array);

    pdf_obj *o = pdf_array_get(array,0);
    pdf_obj *o1 = pdf_array_get(array, 1);

    pdf_obj *ao = pdf_new_array((pdf_document *)doc, 1);
    pdf_array_push_drop(ao, o);
    pdf_obj *ao1 = pdf_new_array((pdf_document *)doc, 1);
    pdf_array_push_drop(ao1, o1);
    pdf_dict_putp_drop(D, "ON", ao);
    pdf_dict_putp_drop(D, "OFF", ao1);
    

    
    pdf_obj *oo = pdf_load_object(((pdf_document *)doc), pdf_to_num(o), pdf_to_gen(o));
    pdf_obj *oo1 = pdf_load_object(((pdf_document *)doc), pdf_to_num(o1), pdf_to_gen(o1));
    
    pdf_dict_putp_drop(oo, "Usage/View/ViewState",pdf_new_name(((pdf_document*)doc),"ON"));
    pdf_dict_putp_drop(oo1,"Usage/View/ViewState",pdf_new_name(((pdf_document*)doc),"OFF"));
    
    pdf_update_object(((pdf_document *)doc), pdf_to_num(oo), 0);
    pdf_update_object(((pdf_document *)doc), pdf_to_num(oo1), 0);
    
    ((pdf_document *)doc)->ocg[0].ocgs->state =1;
    
}

@end
