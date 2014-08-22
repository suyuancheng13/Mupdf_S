#import <UIKit/UIKit.h>
#import<AVFoundation/AVFoundation.h>
#undef ABS
#undef MIN
#undef MAX

#include "mupdf/fitz.h"
#include "mupdf/pdf.h"
#include "mupdf/pdf.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include<alloca.h>
#include"watermark.h"
#include"pdf-password.h"
#include"ExtractPage.h"
#include"pdf-merge.h"
#import "mach/mach.h"
#include"pdf-sound.h"
#include "pdf-video.h"

#import "MuLibrayController.h"
int Sbox[256];
//char key[16]="cdefgihicds2lca";//可以是任意字符串，长度可以是<=256的任何值
//unsigned char key[4]="Key";
unsigned char *oldKey ;
int oldflag = 0 ;
unsigned char key[5]="Wiki";
int keylength=4;
char *filename_g;
int fl=0;

enum  {
    SAVEAS = 0,
    SAVE = 1
    };

#define GAP 20
#define INDICATOR_Y -44-24
#define SLIDER_W (width - GAP - 24)
#define SEARCH_W (width - GAP - 170)
#define CHANGED 1
#define UNCHANGED 0


 dispatch_queue_t queue;
 float screenScale = 1;
 fz_context *ctx = NULL;

//the sigal to present the file wether has changed
/*当切换文件时清理全局变量*/
int who =UNCHANGED;

@interface MuLibraryController : UITableViewController <UIActionSheetDelegate>
{
	NSArray *files;
	NSTimer *timer;
	fz_document *_doc; // temporaries for juggling password dialog
	NSString *_filename;
}
- (void) openDocument: (NSString*)filename;
- (void) askForPassword: (NSString*)prompt;
- (void) onPasswordOkay;
- (void) onPasswordCancel;
- (void) reload;
@end

@interface MuOutlineController : UITableViewController
{
	id target;
	NSMutableArray *titles;
	NSMutableArray *pages;
}
- (id) initWithTarget: (id)aTarget titles: (NSMutableArray*)aTitles pages: (NSMutableArray*)aPages;
@end

@interface MuHitView : UIView
{
	CGSize pageSize;
	int hitCount;
	CGRect hitRects[500];
	int linkPage[500];
	char *linkUrl[500];
	UIColor *color;
}
- (id) initWithSearchResults: (int)n forDocument: (fz_document *)doc;
- (id) initWithLinks: (fz_link*)links forDocument: (fz_document *)doc;
- (void) setPageSize: (CGSize)s;
@end

@interface MuPageView : UIScrollView <UIScrollViewDelegate>
{
	fz_document *doc;
	fz_page *page;
	int number;
	UIActivityIndicatorView *loadingView;
	UIImageView *imageView;
	UIImageView *tileView;
	MuHitView *hitView;
	MuHitView *linkView;
	CGSize pageSize;
	CGRect tileFrame;
	float tileScale;
	BOOL cancel;
}
- (id) initWithFrame: (CGRect)frame document: (fz_document*)aDoc page: (int)aNumber;
- (void) displayImage: (UIImage*)image;
- (void) resizeImage;
- (void) loadPage;
- (void) loadTile;
- (void) willRotate;
- (void) resetZoomAnimated: (BOOL)animated;
- (void) showSearchResults: (int)count;
- (void) clearSearchResults;
- (void) showLinks;
- (void) hideLinks;
- (int) number;


@end

@interface MuDocumentController : UIViewController <UIScrollViewDelegate, UISearchBarDelegate>
{
	fz_document *doc;
	NSString *key;
	MuOutlineController *outline;
	UIScrollView *canvas;
	UILabel *indicator;
	UISlider *slider;
	UISearchBar *searchBar;
	UIBarButtonItem *nextButton, *prevButton, *cancelButton, *searchButton, *outlineButton, *linkButton;
	UIBarButtonItem *sliderWrapper;
	int searchPage;
	int cancelSearch;
	int showLinks;
	int width; // current screen size
	int height;
	int current; // currently visible page
	int scroll_animating; // stop view updates during scrolling animations
}
- (id) initWithFilename: (NSString*)nsfilename document: (fz_document *)aDoc;
- (void) createPageView: (int)number;
- (void) gotoPage: (int)number animated: (BOOL)animated;
- (void) onShowOutline: (id)sender;
- (void) onShowSearch: (id)sender;
- (void) onCancelSearch: (id)sender;
- (void) resetSearch;
- (void) showSearchResults: (int)count forPage: (int)number;
- (void) onSlide: (id)sender;
- (void) onTap: (UITapGestureRecognizer*)sender;
- (void) showNavigationBar;
- (void) hideNavigationBar;

//unlock the file
- (void)unlock;
- (void)addWatermark:(pdf_document *)pdfdoc pageNum:(int)pageNum;

@end

@interface MuAppDelegate : NSObject <UIApplicationDelegate, UINavigationControllerDelegate>
{
	UIWindow *window;
	UINavigationController *navigator;
	MuLibrayController *library;
}
@end

#pragma mark -

static int hit_count = 0;
static fz_rect hit_bbox[500];

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

static void showAlert(NSString *msg, NSString *filename)
{
	UIAlertView *alert = [[UIAlertView alloc]
		initWithTitle: msg
		message: filename
		delegate: nil
		cancelButtonTitle: @"Okay"
		otherButtonTitles: nil];
	[alert show];
	[alert release];
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

static void releasePixmap(void *info, const void *data, size_t size)
{
	fz_drop_pixmap(ctx, info);
}

static UIImage *newImageWithPixmap(fz_pixmap *pix)
{
	unsigned char *samples = fz_pixmap_samples(ctx, pix);
	int w = fz_pixmap_width(ctx, pix);
	int h = fz_pixmap_height(ctx, pix);
	CGDataProviderRef cgdata = CGDataProviderCreateWithData(pix, samples, w * 4 * h, releasePixmap);
	CGColorSpaceRef cgcolor = CGColorSpaceCreateDeviceRGB();
	CGImageRef cgimage = CGImageCreate(w, h, 8, 32, 4 * w,
			cgcolor, kCGBitmapByteOrderDefault,
			cgdata, NULL, NO, kCGRenderingIntentDefault);
	UIImage *image = [[UIImage alloc]
		initWithCGImage: cgimage
		scale: screenScale
		orientation: UIImageOrientationUp];
	CGDataProviderRelease(cgdata);
	CGColorSpaceRelease(cgcolor);
	CGImageRelease(cgimage);
	return image;
}

static CGSize fitPageToScreen(CGSize page, CGSize screen)
{
	float hscale = screen.width / page.width;
	float vscale = screen.height / page.height;
	float scale = fz_min(hscale, vscale);
	hscale = floorf(page.width * scale) / page.width;
	vscale = floorf(page.height * scale) / page.height;
	return CGSizeMake(hscale, vscale);
}

static CGSize measurePage(fz_document *doc, fz_page *page)
{
	CGSize pageSize;
	fz_rect bounds;
	fz_bound_page(doc, page, &bounds);
	pageSize.width = bounds.x1 - bounds.x0;
	pageSize.height = bounds.y1 - bounds.y0;
	return pageSize;
}

#pragma mark- memory handle

vm_size_t usedMemory(void) {
    struct task_basic_info info;
    mach_msg_type_number_t size = sizeof(info);
    kern_return_t kerr = task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &size);
    return (kerr == KERN_SUCCESS) ? info.resident_size : 0; // size in bytes
}

vm_size_t freeMemory(void) {
    mach_port_t host_port = mach_host_self();
    mach_msg_type_number_t host_size = sizeof(vm_statistics_data_t) / sizeof(integer_t);
    vm_size_t pagesize;
    vm_statistics_data_t vm_stat;
    
    host_page_size(host_port, &pagesize);
    (void) host_statistics(host_port, HOST_VM_INFO, (host_info_t)&vm_stat, &host_size);
    return vm_stat.free_count * pagesize;
}

void logMemUsage(void) {
    // compute memory usage and log if different by >= 100k
    static long prevMemUsage = 0;
    long curMemUsage = usedMemory();
    long memUsageDiff = curMemUsage - prevMemUsage;
    
    if (memUsageDiff > 100000 || memUsageDiff < -100000) {
        prevMemUsage = curMemUsage;
        NSLog(@"Memory used %7.1f (%+5.0f), free %7.1f kb", curMemUsage/1000.0f, memUsageDiff/1000.0f, freeMemory()/1000.0f);
    }
}
static UIImage *renderPage(fz_document *doc, fz_page *page, CGSize screenSize)
{
	CGSize pageSize;
	fz_irect bbox;
	fz_matrix ctm;
	fz_device *dev;
	fz_pixmap *pix;
	CGSize scale;

	screenSize.width *= screenScale;
	screenSize.height *= screenScale;

	pageSize = measurePage(doc, page);
	scale = fitPageToScreen(pageSize, screenSize);
	fz_scale(&ctm, scale.width, scale.height);
	bbox = (fz_irect){0, 0, pageSize.width * scale.width, pageSize.height * scale.height};

	pix = fz_new_pixmap_with_bbox(ctx, fz_device_rgb(ctx), &bbox);
	fz_clear_pixmap_with_value(ctx, pix, 255);

	dev = fz_new_draw_device(ctx, pix);
	fz_run_page(doc, page, dev, &ctm, NULL);
	fz_free_device(dev);
    //logMemUsage();
	return newImageWithPixmap(pix);
}

static UIImage *renderTile(fz_document *doc, fz_page *page, CGSize screenSize, CGRect tileRect, float zoom)
{
	CGSize pageSize;
	fz_irect bbox;
	fz_matrix ctm;
	fz_device *dev;
	fz_pixmap *pix;
	CGSize scale;

	screenSize.width *= screenScale;
	screenSize.height *= screenScale;
	tileRect.origin.x *= screenScale;
	tileRect.origin.y *= screenScale;
	tileRect.size.width *= screenScale;
	tileRect.size.height *= screenScale;

	pageSize = measurePage(doc, page);
	scale = fitPageToScreen(pageSize, screenSize);
	fz_scale(&ctm, scale.width * zoom, scale.height * zoom);

	bbox.x0 = tileRect.origin.x;
	bbox.y0 = tileRect.origin.y;
	bbox.x1 = tileRect.origin.x + tileRect.size.width;
	bbox.y1 = tileRect.origin.y + tileRect.size.height;

	pix = fz_new_pixmap_with_bbox(ctx, fz_device_rgb(ctx), &bbox);
	fz_clear_pixmap_with_value(ctx, pix, 255);

	dev = fz_new_draw_device(ctx, pix);
	fz_run_page(doc, page, dev, &ctm, NULL);
	fz_free_device(dev);

	return newImageWithPixmap(pix);
}

static void event_cb(pdf_doc_event *event, void *data)
{
    switch (event->type)
    {
        case PDF_DOCUMENT_EVENT_ALERT:
            /* the alert window  */
        {
            pdf_alert_event *alert_event = pdf_access_alert_event(event);
            NSString *title = [[NSString alloc] initWithCString:alert_event->title encoding:NSStringEncodingConversionExternalRepresentation];
             NSString *mes = [[NSString alloc] initWithCString:alert_event->message encoding:NSStringEncodingConversionExternalRepresentation];
            UIAlertView *alert = [[UIAlertView alloc] initWithTitle:title message:mes delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
            [alert show];
            
            //release the memeory
            [title release];
            [mes release];
            [alert release];
            break;
        }
        case PDF_DOCUMENT_EVENT_SUBMIT:
        {
            pdf_submit_event *submit_f = pdf_access_submit_event(event);
            
            NSString *uStr = [NSString stringWithUTF8String:submit_f->url];
            NSURL *cURL = [NSURL URLWithString:uStr];
            NSString *dStr = [NSString stringWithUTF8String:submit_f->data];
            NSData *data = [dStr dataUsingEncoding:NSUTF8StringEncoding];
            NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:cURL cachePolicy:NSURLRequestUseProtocolCachePolicy timeoutInterval:50];
            
            if (submit_f->get == PDF_SUBMIT_METHOD_GET) {
                [request setHTTPMethod:@"GET"];
                
                NSData *received = [NSURLConnection sendSynchronousRequest:request returningResponse:nil error:nil];
                unsigned char *rece_buf = (unsigned char *)[received bytes];
                printf("%s",rece_buf);
               // pdf_obj
                fz_context *newCtx = fz_new_context(NULL, NULL, 128<<20);
                fz_buffer *buf = fz_new_buffer_from_data(newCtx, rece_buf, strlen(rece_buf));//the length of the string 
                fz_stream *stream = fz_open_buffer(ctx , buf);
                pdf_document *fdf = pdf_open_document_no_run_with_stream(newCtx, stream);
                pdf_obj *obj = pdf_load_object(fdf, 1, 0);
            
                pdf_obj *xobj = pdf_dict_getp(obj, "FDF/JavaScript/Before");
                char *code = pdf_to_utf8(fdf, xobj);
               // char *code = "a('0xFFCD11A0E6F5CAE0', false);";
                pdf_js_execute(event->doc->js, code);
                
                //release the memroy
                fz_drop_buffer(newCtx, buf);
                fz_free(newCtx, stream);
                fz_free(newCtx, fdf);
                fz_free(ctx, obj);
                fz_free(newCtx, xobj);
                fz_free_context(newCtx);
                
               
            }
            else if (submit_f->get == PDF_SUBMIT_METHOD_POST)
            {
                [request setHTTPMethod:@"POST"];
                [request setHTTPBody:data];
                
                NSData *received = [NSURLConnection sendSynchronousRequest:request returningResponse:nil error:nil];
                char *rece_buf = [received bytes];
                printf("%s",rece_buf);
                
                fz_context *newCtx = fz_new_context(NULL, NULL, 128<<20);
                fz_buffer *buf = fz_new_buffer_from_data(newCtx, rece_buf, 500);
                fz_stream *stream = fz_open_buffer(newCtx , buf);
                pdf_document *fdf = pdf_open_document_no_run_with_stream(newCtx, stream);
                pdf_obj *obj = pdf_load_object(fdf, 1, 0);
                
                pdf_obj *xobj = pdf_dict_getp(obj, "FDF/JavaScript/Before");
                //                pdf_print_obj(xobj);
                char *code = pdf_to_utf8(fdf, xobj);
                pdf_js_execute(event->doc->js, code);
            }
            
            break;
        }
        default:
            break;
    }
}
#pragma mark - adddddddd

static pdf_ui_event
set_ui_evet(CGPoint p, fz_document *doc, fz_page *page, CGSize screenSize)
{
    pdf_ui_event event;
    
    CGSize pageSize = measurePage(doc, page);
    CGSize scale = fitPageToScreen(pageSize, screenSize);
    
    event.etype = PDF_EVENT_TYPE_POINTER;
    
    event.event.pointer.ptype = PDF_POINTER_DOWN;
    
    event.event.pointer.pt.x = p.x / scale.width;
    event.event.pointer.pt.y = p.y / scale.height;
    NSLog(@"px=%f,py=%f",event.event.pointer.pt.x,event.event.pointer.pt.y);
    return  event;
}

static pdf_obj*
tapInArea(pdf_ui_event *ui_event, pdf_document *doc, int current)
{
    pdf_annot *annot;
    fz_point *pt = &(ui_event->event.pointer.pt);
    
    pdf_page *page = pdf_load_page(doc, current);
    annot = pdf_first_annot(doc, page);
    
    while (annot) {
        if (pt->x >= annot->pagerect.x0 && pt->x <= annot->pagerect.x1)
			if (pt->y >= annot->pagerect.y0 && pt->y <= annot->pagerect.y1)
            {
                //                pdf_obj *a = pdf_load_object((pdf_document *)doc, pdf_to_num(annot->obj), pdf_to_gen(annot->obj));
                //                pdf_fprint_obj(stdout, a, 0);
                return annot->obj;
            }
        annot = pdf_next_annot(doc, annot);
    }
    
    return NULL;
}



#pragma mark -

@implementation MuLibraryController

- (void) viewWillAppear: (BOOL)animated
{
	[self setTitle: @"PDF, XPS and CBZ Documents"];
	[self reload];
	printf("library viewWillAppear (starting reload timer)\n");
	timer = [NSTimer timerWithTimeInterval: 3
		target: self selector: @selector(reload) userInfo: nil
		repeats: YES];
	[[NSRunLoop currentRunLoop] addTimer: timer forMode: NSDefaultRunLoopMode];
}

- (void) viewWillDisappear: (BOOL)animated
{
	printf("library viewWillDisappear (stopping reload timer)\n");
	[timer invalidate];
	timer = nil;
}

- (void) reload
{
	if (files) {
		[files release];
		files = nil;
	}

	NSFileManager *fileman = [NSFileManager defaultManager];//文件夹管理类
	NSString *docdir = [NSString stringWithFormat: @"%@/Documents", NSHomeDirectory()];//当前文件目录
	NSMutableArray *outfiles = [[NSMutableArray alloc] init];
	NSDirectoryEnumerator *direnum = [fileman enumeratorAtPath:docdir];//为了遍历文件夹
	
    NSString *file;
	BOOL isdir;
	while (file = [direnum nextObject]) {
		NSString *filepath = [docdir stringByAppendingPathComponent:file];
		NSLog(@"file %@\n", file);
		if ([fileman fileExistsAtPath:filepath isDirectory:&isdir] && !isdir) {//文件存在就加入outfiles
			[outfiles addObject:file];
		}
	}

	files = outfiles;

	[[self tableView] reloadData];
}

- (void) dealloc
{
	[files release];
	[super dealloc];
}

- (BOOL) shouldAutorotateToInterfaceOrientation: (UIInterfaceOrientation)o
{
	return YES;
}

- (NSInteger) numberOfSectionsInTableView: (UITableView*)tableView
{
	return 3; //three kind of file:pdf,XPS and CBZ Documents
}

- (NSInteger) tableView: (UITableView*)tableView numberOfRowsInSection: (NSInteger)section
{
	return [files count];
}
- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section    // fixed font style. use custom view (UILabel) if you want something different
{
    switch (section) {
        case 0:
            return @"PDF files";
            
        case 1:
            return @"XPS files";
            
        case 2:
            return @"CBZ files";
    }
}

- (void) actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
	if (buttonIndex == [actionSheet destructiveButtonIndex])
	{
		char filename[PATH_MAX];
		int row = [actionSheet tag];

		dispatch_sync(queue, ^{});

		strcpy(filename, [NSHomeDirectory() UTF8String]);
		strcat(filename, "/Documents/");
		strcat(filename, [[files objectAtIndex: row - 1] UTF8String]);

		printf("delete document '%s'\n", filename);

		unlink(filename);//delete the link of the file

		[self reload];
	}
}

- (void) onTapDelete: (UIControl*)sender
{
	int row = [sender tag];
	NSString *title = [NSString stringWithFormat: @"Delete %@?", [files objectAtIndex: row - 1]];
	UIActionSheet *sheet = [[UIActionSheet alloc]
							initWithTitle: title
							delegate: self
							cancelButtonTitle: @"Cancel"
							destructiveButtonTitle: @"Delete"
							otherButtonTitles: nil];
	[sheet setTag: row];
	[sheet showInView: [self tableView]];
	[sheet release];
}

- (UITableViewCell*) tableView: (UITableView*)tableView cellForRowAtIndexPath: (NSIndexPath*)indexPath
{
    //if(0==[indexPath section])
    {
	static NSString *cellid = @"MuCellIdent";
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier: cellid];
	if (!cell)
		cell = [[[UITableViewCell alloc] initWithStyle: UITableViewCellStyleDefault reuseIdentifier: cellid] autorelease];
	int row = [indexPath row];
	[[cell textLabel] setText: [files objectAtIndex: row]];
	[[cell textLabel] setFont: [UIFont systemFontOfSize: 20]];

	UIButton *deleteButton = [UIButton buttonWithType:UIButtonTypeCustom];
	[deleteButton setImage: [UIImage imageNamed: @"x_alt_blue.png"] forState: UIControlStateNormal];
	[deleteButton setFrame: CGRectMake(0, 0, 35, 35)];
	[deleteButton addTarget: self action: @selector(onTapDelete:) forControlEvents: UIControlEventTouchUpInside];
	[deleteButton setTag: row];
    
	//[cell setEditingAccessoryView: deleteButton];
   
	return cell;
    }
}

- (void) tableView: (UITableView*)tableView didSelectRowAtIndexPath: (NSIndexPath*)indexPath
{
	int row = [indexPath row];
    
    who = CHANGED;//present the file has changed
    
	[self openDocument: [files objectAtIndex: row]];
}

/*
 TODO:create a image and return the image object's number
 */
int createImage(pdf_document *doc,char * imgeFile)
{
    pdf_obj *imgXobject =pdf_new_dict(doc, 0);
    int img = pdf_create_object(doc);
    pdf_dict_putp_drop(imgXobject, "Type", pdf_new_name(doc, "XObject"));
    pdf_dict_putp_drop(imgXobject, "Filter", pdf_new_name(doc, "DCTDecode"));
    pdf_dict_putp_drop(imgXobject, "Subtype", pdf_new_name(doc, "Image"));
    pdf_dict_putp_drop(imgXobject, "Width", pdf_new_int(doc, 220));
    pdf_dict_putp_drop(imgXobject,"Height", pdf_new_int(doc, 158));
    pdf_dict_putp_drop(imgXobject, "ColorSpace", pdf_new_name(doc, "DeviceRGB"));
    pdf_dict_putp_drop(imgXobject, "BitsPerComponent", pdf_new_int(doc, 8));
    
//    pdf_dict_putp_drop(imgXobject, "PieceInfo/ADBE_CompoundType/Private", pdf_new_name(doc, "Watermark"));
//    
//    //doc setting in pieceinfo
//    int docSetNum = pdf_create_object(doc);
//    pdf_obj *docSet = pdf_new_dict(doc, 0);
//    
//    char * xml = "...<?xml version = \"1.0\" encoding = \"UTF-8\" ?>\
//<WatermarkSettings version = \"8.0\"><SourceFile type=\"\" name=\"CurrentImage\"/>\
//<Scale value=\"1.0\"/><Rotation value=\"0\"/><Opacity value=\"1\"/><Location ontop=\"0\"/>\
//<Color r=\"1\" b=\"0\" g=\"1\"/>\
//<Alignment vertalign=\"1\" horizalign=\"1\" vertvalue= \"0.0\" horizvalue=\"0.0\" unit=\"3\"/>\
//<Appearance onscreen=\"1\" onprint=\"1\" fixedprint=\"0\"/><PageRange start=\"-1\" end=\"-1\" even=\"1\" odd=\"1\"/>\
//</WatermarkSettings>";
//    
//    fz_buffer *bf = fz_new_buffer_from_data(ctx, (unsigned char *)xml, strlen(xml));
//    pdf_dict_putp(docSet, "Length", pdf_new_int(doc, strlen(xml)));
//    pdf_update_object(doc, docSetNum, docSet);
//    pdf_update_stream(doc, docSetNum, bf);
//    
//    pdf_dict_putp(imgXobject, "PieceInfo/ADBE_CompoundType/DocSettings",pdf_new_indirect(doc, docSetNum, 0) );
//    pdf_dict_putp(imgXobject, "PieceInfo/ADBE_CompoundType/LastModified", pdf_new_string(doc, "D:20130801170220+08'00'", strlen("D:20130801170220+08'00'")));
    fz_stream *imgStream =  fz_open_file(ctx,imgeFile);
    fz_buffer *imgBuffer = fz_read_all(imgStream, 1024);
    pdf_dict_putp(imgXobject, "Length", pdf_new_int(doc, imgBuffer->len));
    
    pdf_update_object(doc, img, imgXobject);
    pdf_update_stream(doc, img, imgBuffer);
    pdf_drop_obj(imgXobject);
    return img;

}
/*
 TODO:create a xobject for a image waterMark,and return the xobject number
 by the mupdf function
 */
int createIWXobject_(pdf_document *doc,int img)
{
   
    const fz_matrix matrix = {1,0,0,1,0,0};
    const fz_rect bbox = {10 ,-10 , 230,148};
    
    pdf_obj *xobject =  pdf_new_xobject(doc,&bbox, &matrix);
    pdf_obj *xoo = pdf_load_object(doc, pdf_to_num(xobject), 0);//the xobject's dictionary

    pdf_obj *group = pdf_new_dict(doc, 3);
    pdf_dict_putp_drop(group, "K", pdf_new_bool(doc, 1));
    pdf_dict_putp_drop(group, "S", pdf_new_name(doc, "Transparency"));
    pdf_dict_putp_drop(group, "Type", pdf_new_name(doc, "Group"));
    
    pdf_dict_putp_drop(xoo, "Group", pdf_new_ref(doc, group));
    pdf_drop_obj(group);
   
    //add the ocg
    pdf_obj *OCG =  pdf_new_dict(doc, 10);
    pdf_dict_putp_drop(OCG, "Name", pdf_new_string(doc,"Watermark",9) );
    pdf_dict_putp_drop(OCG, "Type", pdf_new_name(doc, "OCG"));
    pdf_dict_putp_drop(OCG, "Usage/View/ViewSate", pdf_new_name(doc, "ON"));
    
    pdf_dict_putp_drop(xoo, "OC", pdf_new_ref(doc,OCG));
    
    pdf_obj *ar = pdf_new_array(doc, 1);
    pdf_array_push(ar, pdf_new_ref(doc,OCG));
    pdf_drop_obj(OCG);
    
    //in catalog
    pdf_obj *catalog =  pdf_dict_getp(pdf_trailer((pdf_document *)doc), "Root");
    pdf_dict_putp(catalog, "OCProperties/OCGs", ar);
    pdf_obj *as = pdf_new_dict(doc, 3);
    pdf_dict_putp_drop(as, "OCGs", ar);
    pdf_dict_putp_drop(as, "Event", pdf_new_name(doc, "View"));
    pdf_obj *ca = pdf_new_array(doc, 1);
    pdf_array_push(ca, pdf_new_name(doc, "View"));
    pdf_dict_putp_drop(as, "Category", ca);
    pdf_obj *arr = pdf_new_array(doc, 1);
    pdf_array_push(arr, as);
    pdf_dict_putp_drop(catalog, "OCProperties/D/AS", arr);
    pdf_dict_putp_drop(catalog, "OCProperties/D/ON", ar);
    
    // the doc settings
    pdf_obj *procSet = pdf_new_array(doc, 2);
    pdf_array_push(procSet, pdf_new_name(doc, "PDF"));
    pdf_array_push(procSet, pdf_new_name(doc, "ImageC"));
    pdf_dict_putp_drop(xoo,"Resources/ProcSet", procSet);
    pdf_dict_putp_drop(xoo, "Resources/XObject/waterImg", pdf_new_indirect(doc, img, 0));
    
    char *data = "q\n\
    220 0 0 158 10 -10 cm\n\
    /waterImg Do\n\
    Q\n";
    fz_buffer *buffer = fz_new_buffer_from_data(ctx,( unsigned char *)data , strlen(data));
    
    //peiceinfo in xobject
    int docSetNum = pdf_create_object(doc);
    pdf_obj *docSet = pdf_new_dict(doc, 0);
    
    char * xml = "<?xml version = \"1.0\" encoding = \"UTF-8\" ?>\
<WatermarkSettings version = \"8.0\"><SourceFile type=\"\" name=\"CurrentImage\"/>\
<Scale value=\"1.0\"/><Rotation value=\"0\"/><Opacity value=\"1\"/><Location ontop=\"0\"/>\
<Color r=\"1\" b=\"0\" g=\"1\"/>\
<Alignment vertalign=\"1\" horizalign=\"1\" vertvalue= \"0.0\" horizvalue=\"0.0\" unit=\"3\"/>\
<Appearance onscreen=\"1\" onprint=\"1\" fixedprint=\"0\"/><PageRange start=\"-1\" end=\"-1\" even=\"1\" odd=\"1\"/>\
</WatermarkSettings>";
    
    fz_buffer *bf = fz_new_buffer_from_data(ctx, (unsigned char *)xml, strlen(xml));
    pdf_dict_putp(docSet, "Length", pdf_new_int(doc, strlen(xml)));
    pdf_update_object(doc, docSetNum, docSet);
    pdf_update_stream(doc, docSetNum, bf);
    pdf_dict_putp(xoo, "PieceInfo/ADBE_CompoundType/DocSettings",pdf_new_indirect(doc, docSetNum, 0) );
    pdf_dict_putp_drop(xoo, "PieceInfo/ADBE_CompoundType/Private", pdf_new_name(doc, "Watermark"));
    pdf_dict_putp(xoo, "PieceInfo/ADBE_CompoundType/LastModified", pdf_new_string(doc, "D:20130801170220+08'00'", strlen("D:20130801170220+08'00'")));
    pdf_dict_putp(xoo, "LastModified", pdf_new_string(doc, "D:20130801170220+08'00'", strlen("D:20130801170220+08'00'")));
    
    pdf_update_xobject_contents(doc, pdf_load_xobject(doc, xobject), buffer);
    return pdf_to_num(xobject);
    

}
/*
 TODO:create a xobject for a image waterMark,and return the xobject number
 */
int createIWXobject(pdf_document * doc,int img)
{
    pdf_obj *dict;
    const fz_matrix matrix = {1,0,0,1,0,0};
    const fz_rect bbox = {10 ,-10 , 230,148};

    pdf_obj *group = pdf_new_dict(doc, 3);
    pdf_dict_putp_drop(group, "K", pdf_new_bool(doc, 1));
    pdf_dict_putp_drop(group, "S", pdf_new_name(doc, "Transparency"));
    pdf_dict_putp_drop(group, "Type", pdf_new_name(doc, "Group"));
    
    pdf_obj *xoo = pdf_new_dict(doc,0);
    int xo = pdf_create_object(doc);
    
    dict = pdf_new_rect(doc, &bbox);
    pdf_dict_putp(xoo, "BBox", dict);
    dict = NULL;
    
    pdf_dict_putp_drop(xoo, "Group", pdf_new_ref(doc, group));
    pdf_drop_obj(group);
    
    dict = pdf_new_matrix(doc, &matrix);
    pdf_dict_putp(xoo, "Matrix",dict);
    dict = NULL;
    
   
    
    dict = pdf_new_name(doc, "XObject");
    pdf_dict_putp(xoo, "Type", dict);
    dict = NULL;
    
    dict = pdf_new_name(doc,"Form");
    pdf_dict_putp(xoo, "Subtype", dict);
    dict = NULL;
    
    //    dict = pdf_new_int(doc, 1);
    //    pdf_dict_putp(xoo, "FormType", dict);
    //    dict = NULL;
   
    //add the ocg
    pdf_obj *OCG =  pdf_new_dict(doc, 10);
    pdf_dict_putp_drop(OCG, "Name", pdf_new_string(doc,"Watermark",9) );
    pdf_dict_putp_drop(OCG, "Type", pdf_new_name(doc, "OCG"));
    pdf_dict_putp_drop(OCG, "Usage/View/ViewSate", pdf_new_name(doc, "ON"));

    pdf_dict_putp_drop(xoo, "OC", pdf_new_ref(doc,OCG));
    
    pdf_obj *ar = pdf_new_array(doc, 1);
    pdf_array_push(ar, pdf_new_ref(doc,OCG));
    pdf_drop_obj(OCG);
    
    //in catalog
    pdf_obj *catalog =  pdf_dict_getp(pdf_trailer((pdf_document *)doc), "Root");
    pdf_dict_putp(catalog, "OCProperties/OCGs", ar);
    pdf_obj *as = pdf_new_dict(doc, 3);
    pdf_dict_putp_drop(as, "OCGs", ar);
    pdf_dict_putp_drop(as, "Event", pdf_new_name(doc, "View"));
    pdf_obj *ca = pdf_new_array(doc, 1);
    pdf_array_push(ca, pdf_new_name(doc, "View"));
    pdf_dict_putp_drop(as, "Category", ca);
    pdf_obj *arr = pdf_new_array(doc, 1);
    pdf_array_push(arr, as);
    pdf_dict_putp_drop(catalog, "OCProperties/D/AS", arr);
    pdf_dict_putp_drop(catalog, "OCProperties/D/ON", ar);
  
    // the doc settings
    pdf_obj *procSet = pdf_new_array(doc, 2);
    pdf_array_push(procSet, pdf_new_name(doc, "PDF"));
    pdf_array_push(procSet, pdf_new_name(doc, "ImageC"));
    pdf_dict_putp_drop(xoo,"Resources/ProcSet", procSet);
    //  pdf_dict_putp(xoo, "Resources/ExtGState/GSW", pdf_new_indirect(doc, abc, 0));
    // pdf_drop_obj(exGstate);
    pdf_dict_putp_drop(xoo, "Resources/XObject/Im0", pdf_new_indirect(doc, img, 0));
    char *data = "q\n\
220 0 0 158 10 -10 cm\n\
/Im0 Do\n\
Q\n";
    
    fz_buffer *buffer = fz_new_buffer_from_data(ctx,( unsigned char *)data , strlen(data));
    
    //peiceinfo in xobject
    pdf_dict_putp_drop(xoo, "PieceInfo/ADBE_CompoundType/Private", pdf_new_name(doc, "Watermark"));
     pdf_dict_putp(xoo, "PieceInfo/ADBE_CompoundType/LastModified", pdf_new_string(doc, "D:20130801170220+08'00'", strlen("D:20130801170220+08'00'")));
    pdf_dict_putp(xoo, "LastModified", pdf_new_string(doc, "D:20130801170220+08'00'", strlen("D:20130801170220+08'00'")));

    //doc setting in pieceinfo
    int docSetNum = pdf_create_object(doc);
    pdf_obj *docSet = pdf_new_dict(doc, 0);
    
    char * xml = "<?xml version = \"1.0\" encoding = \"UTF-8\" ?>\
<WatermarkSettings version = \"8.0\"><SourceFile type=\"\" name=\"CurrentImage\"/>\
<Scale value=\"1.0\"/><Rotation value=\"0\"/><Opacity value=\"1\"/><Location ontop=\"0\"/>\
<Color r=\"1\" b=\"0\" g=\"1\"/>\
<Alignment vertalign=\"1\" horizalign=\"1\" vertvalue= \"0.0\" horizvalue=\"0.0\" unit=\"3\"/>\
<Appearance onscreen=\"1\" onprint=\"1\" fixedprint=\"0\"/><PageRange start=\"-1\" end=\"-1\" even=\"1\" odd=\"1\"/>\
</WatermarkSettings>";
    
    fz_buffer *bf = fz_new_buffer_from_data(ctx, (unsigned char *)xml, strlen(xml));
    pdf_dict_putp(docSet, "Length", pdf_new_int(doc, strlen(xml)));
    pdf_update_object(doc, docSetNum, docSet);
    pdf_update_stream(doc, docSetNum, bf);
    
    pdf_dict_putp(xoo, "PieceInfo/ADBE_CompoundType/DocSettings",pdf_new_indirect(doc, docSetNum, 0) );
    
    pdf_update_object(doc, xo, xoo);
    pdf_update_stream(doc, xo, buffer);

    //pdf_print_obj(xoo);
    return xo;

}

/*
 TODO:save the document
 */
void save(pdf_document *doc,int another)
{
    fz_write_options ops;
    int erro = 0 ;
    ops.do_incremental = another;
    ops.continue_on_error = 1;
    ops.do_linear = 0;
    ops.do_garbage = 0;
    ops.do_expand = 0 ;
    ops.do_ascii = 0 ;
    ops.errors = &erro;
    
    if(!another)
    {
       pdf_write_document(doc, "/Users/houyushen/Desktop/out.pdf", &ops);
    }
    else{
        pdf_write_document(doc,filename_g, &ops);
    }
//    pdf_close_document(doc);
//    doc = (pdf_document *)fz_open_document(ctx, filename_g);//reopen the document
    
}

 /*
  TODO: add water mark
  */
void addWatermark(pdf_document * doc,int pageNum)
{
//    pdf_page *page = pdf_load_page(doc,pageNum);
//    pdf_obj *pageObj = page->me;
//    
//    //graphics state
//   // pdf_obj *exGstate = pdf_new_dict(doc, 0);
//    int abc = pdf_create_object(doc);
//    pdf_obj *gS0 = pdf_new_dict(doc, 0);
//    
//    pdf_dict_putp_drop(gS0, "Type", pdf_new_name(doc, "ExtGState"));
//    pdf_dict_putp_drop(gS0, "ca", pdf_new_real(doc, 0.5));
//    pdf_dict_putp_drop(gS0, "CA", pdf_new_real(doc, 1));
//    //pdf_dict_putp_drop(exGstate, "GSW", pdf_new_indirect(doc, abc,0));
//    
//    pdf_update_object(doc, abc, gS0);
//    pdf_drop_obj(gS0);
//    pdf_dict_putp_drop( pdf_dict_getp(pageObj,"Resources") ,"ExtGState/GSW",pdf_new_indirect(doc, abc,0));
//    
//    /*
//     image data,
//     img means the number of image xobject
//     */
//    int img = createImage(doc,"/Users/houyushen/Library/Application Support/iPhone Simulator/6.1/Applications/2AEB2534-8AE0-4007-833C-06692800D127/Documents/tiger.jpg");
//    
//    /*
//     xobject for image watermark
//     */
//    int xo = createIWXobject_(doc,img);
//     pdf_dict_putp_drop(pdf_dict_getp(pageObj,"Resources"),"XObject/waterMark",pdf_new_indirect(doc,xo, 0));
//     
//    
//    /*
//     set page contents stream
//     */
//    pdf_obj *dictc = pdf_new_dict(doc, 0);
//    int newContent = pdf_create_object(doc);
//    pdf_update_object(doc, newContent, dictc);
//
//    char *data2 = "/Artifact <</Subtype /Watermark /Type /Pagination >>BDC\n\
//q\n\
//1.2 0 0 1.2 220 158 cm\n\
///GSW gs\n\
///waterMark Do\n\
//Q\n\
//EMC\n";    
//    fz_buffer *buffer2 = fz_new_buffer_from_data(ctx, (unsigned char *)data2, strlen(data2));
//    pdf_update_stream(doc, newContent, buffer2);
//
//    pdf_obj *new =  pdf_load_object(doc, newContent, 0);
//    pdf_dict_putp(new, "Length", pdf_new_int(doc, strlen(data2)));
//    
//    pdf_print_obj(new);
//    
//    pdf_obj *content = pdf_dict_getp(pageObj, "Contents");
//    pdf_obj *newContents ;
//    if(pdf_is_array(content))
//    {
//        newContents = pdf_new_array(doc,0);
//        newContents= content;
//        pdf_array_insert_drop(newContents,  pdf_new_indirect(doc, newContent, 0), 0);
//        pdf_array_push(newContents, pdf_new_indirect(doc, newContent, 0));
//    }
//    else
//    {
//        newContents = pdf_new_array(doc, 2);
//        pdf_array_push(newContents, content);
//        pdf_array_push(newContents, pdf_new_indirect(doc, newContent, 0));
//    }
//    pdf_dict_puts(pageObj, "Contents", newContents);
//    
//    pdf_obj *procSet= pdf_new_array(doc, 3);    
//    pdf_array_push(procSet, pdf_new_name(doc, "PDF"));
//    pdf_array_push(procSet, pdf_new_name(doc, "Text"));
//    pdf_array_push(procSet, pdf_new_name(doc, "ImageC"));
//    pdf_dict_putp_drop(pageObj, "Resources/ProcSet", procSet);
//    pdf_print_obj(procSet);
//    pdf_print_obj(pageObj);
//    const char *imagename = "/Users/houyushen/Library/Application Support/iPhone Simulator/6.1/Applications/2AEB2534-8AE0-4007-833C-06692800D127/Documents/tiger.jpg";
//    fz_rect bbox = {100,200,320,358};
//    pdf_create_watermark_with_img(doc, pageNum, imagename, 220, 158, &bbox);
    /*
     write out
     */
//   save(doc);
//    pdf_close_document(doc);
//    doc = pdf_open_document(ctx, filename_g);
    
   
    
  }



- (void) openDocument: (NSString*)nsfilename
{
	char filename[PATH_MAX];
    filename_g =(char *)malloc(1024*sizeof(char));
    
	dispatch_sync(queue, ^{});

	strcpy(filename, [NSHomeDirectory() UTF8String]);
	strcat(filename, "/Documents/");
	strcat(filename, [nsfilename UTF8String]);

	printf("open document '%s'\n", filename);

	_filename = [nsfilename retain];

    //keep the filename
    strcpy(filename_g, filename);
	_doc = fz_open_document(ctx, filename);

   pdf_document *doc = ((pdf_document*)_doc);
   
    //add the event_cb to add the event handle
    ((pdf_document *)_doc)->event_cb = event_cb;
    
	if (!_doc) {
		showAlert(@"Cannot open document", nsfilename);
		return;
	}
//    fz_authenticate_password(_doc, "123");
//    [self onPasswordOkay];
	if (fz_needs_password(_doc))
		[self askForPassword: @"'%@' needs a password:"];
	else
	{
        [self onPasswordOkay];
    }

}

- (void) askForPassword: (NSString*)prompt
{
	UIAlertView *passwordAlertView = [[UIAlertView alloc]
		initWithTitle: @"Password Protected"
		message: [NSString stringWithFormat: prompt, [_filename lastPathComponent]]
		delegate: self
		cancelButtonTitle: @"Cancel"
		otherButtonTitles: @"Done", nil];
	[passwordAlertView setAlertViewStyle: UIAlertViewStyleSecureTextInput];
	[passwordAlertView show];
	[passwordAlertView release];
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
	char *password = (char*) [[[alertView textFieldAtIndex: 0] text] UTF8String];
	[alertView dismissWithClickedButtonIndex: buttonIndex animated: TRUE];
	if (buttonIndex == 1) {
		if (fz_authenticate_password(_doc, password))
        {
          //  printf("%s\n",((pdf_document *)_doc)->crypt->key);
            [self onPasswordOkay];
        }
		else
			[self askForPassword: @"Wrong password for '%@'. Try again:"];
	} else {
		[self onPasswordCancel];
	}
}

- (void) onPasswordOkay
{
	MuDocumentController *document = [[MuDocumentController alloc] initWithFilename: _filename document: _doc];
	if (document) {
		[self setTitle: @"Library"];
		[[self navigationController] pushViewController: document animated: YES];
		[document release];
	}
	[_filename release];
	_doc = NULL;
}

- (void) onPasswordCancel
{
	[_filename release];
	printf("close document (password cancel)\n");
	fz_close_document(_doc);
	_doc = NULL;
}

@end

#pragma mark -

@implementation MuOutlineController

- (id) initWithTarget: (id)aTarget titles: (NSMutableArray*)aTitles pages: (NSMutableArray*)aPages
{
	self = [super initWithStyle: UITableViewStylePlain];
	if (self) {
		[self setTitle: @"Table of Contents"];
		target = aTarget; // only keep a weak reference, to avoid retain cycles
		titles = [aTitles retain];
		pages = [aPages retain];
		[[self tableView] setSeparatorStyle: UITableViewCellSeparatorStyleNone];
	}
	return self;
}

- (void) dealloc
{
	[titles release];
	[pages release];
	[super dealloc];
}

- (BOOL) shouldAutorotateToInterfaceOrientation: (UIInterfaceOrientation)o
{
	return YES;
}

- (NSInteger) numberOfSectionsInTableView: (UITableView*)tableView
{
	return 1;
}

- (NSInteger) tableView: (UITableView*)tableView numberOfRowsInSection: (NSInteger)section
{
	return [titles count];
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
	return 28;
}

- (UITableViewCell*) tableView: (UITableView*)tableView cellForRowAtIndexPath: (NSIndexPath*)indexPath
{
	static NSString *cellid = @"MuCellIdent";
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier: cellid];
	if (!cell)
	{
		cell = [[[UITableViewCell alloc] initWithStyle: UITableViewCellStyleValue1 reuseIdentifier: cellid] autorelease];
		[[cell textLabel] setFont: [UIFont systemFontOfSize: 16]];
		[[cell detailTextLabel] setFont: [UIFont systemFontOfSize: 16]];
	}
	NSString *title = [titles objectAtIndex: [indexPath row]];
	NSString *page = [pages objectAtIndex: [indexPath row]];
	[[cell textLabel] setText: title];
	[[cell detailTextLabel] setText: [NSString stringWithFormat: @"%d", [page intValue]+1]];
	return cell;
}

- (void) tableView: (UITableView*)tableView didSelectRowAtIndexPath: (NSIndexPath*)indexPath
{
	NSNumber *page = [pages objectAtIndex: [indexPath row]];
	[target gotoPage: [page intValue] animated: NO];
	[[self navigationController] popViewControllerAnimated: YES];
}

@end

#pragma mark -

@implementation MuHitView

- (id) initWithSearchResults: (int)n forDocument: (fz_document *)doc
{
	self = [super initWithFrame: CGRectMake(0,0,100,100)];
	if (self) {
		[self setOpaque: NO];

		color = [[UIColor colorWithRed: 0x25/255.0 green: 0x72/255.0 blue: 0xAC/255.0 alpha: 0.5] retain];

		pageSize = CGSizeMake(100,100);

		for (int i = 0; i < n && i < nelem(hitRects); i++) {
			fz_rect bbox = search_result_bbox(doc, i); // this is thread-safe enough
			hitRects[i].origin.x = bbox.x0;
			hitRects[i].origin.y = bbox.y0;
			hitRects[i].size.width = bbox.x1 - bbox.x0;
			hitRects[i].size.height = bbox.y1 - bbox.y0;
		}
		hitCount = n;
	}
	return self;
}

- (id) initWithLinks: (fz_link*)link forDocument: (fz_document *)doc
{
	self = [super initWithFrame: CGRectMake(0,0,100,100)];
	if (self) {
		[self setOpaque: NO];

		color = [[UIColor colorWithRed: 0xAC/255.0 green: 0x72/255.0 blue: 0x25/255.0 alpha: 0.5] retain];

		pageSize = CGSizeMake(100,100);

		while (link && hitCount < nelem(hitRects)) {
			if (link->dest.kind == FZ_LINK_GOTO || link->dest.kind == FZ_LINK_URI) {
				fz_rect bbox = link->rect;
				hitRects[hitCount].origin.x = bbox.x0;
				hitRects[hitCount].origin.y = bbox.y0;
				hitRects[hitCount].size.width = bbox.x1 - bbox.x0;
				hitRects[hitCount].size.height = bbox.y1 - bbox.y0;
				linkPage[hitCount] = link->dest.kind == FZ_LINK_GOTO ? link->dest.ld.gotor.page : -1;
				linkUrl[hitCount] = link->dest.kind == FZ_LINK_URI ? strdup(link->dest.ld.uri.uri) : nil;
				hitCount++;
			}
			link = link->next;
		}
	}
	return self;
}

- (void) setPageSize: (CGSize)s
{
	pageSize = s;
	// if page takes a long time to load we may have drawn at the initial (wrong) size
	[self setNeedsDisplay];
}

- (void) drawRect: (CGRect)r
{
	CGSize scale = fitPageToScreen(pageSize, self.bounds.size);

	[color set];

	for (int i = 0; i < hitCount; i++) {
		CGRect rect = hitRects[i];
		rect.origin.x *= scale.width;
		rect.origin.y *= scale.height;
		rect.size.width *= scale.width;
		rect.size.height *= scale.height;
		UIRectFill(rect);
	}
}

- (void) dealloc
{
	int i;
	[color release];
	for (i = 0; i < hitCount; i++)
		free(linkUrl[i]);
	[super dealloc];
}

@end

@implementation MuPageView

- (id) initWithFrame: (CGRect)frame document: (fz_document*)aDoc page: (int)aNumber
{
	self = [super initWithFrame: frame];
	if (self) {
		doc = aDoc;
		number = aNumber;
		cancel = NO;

		[self setShowsVerticalScrollIndicator: NO];
		[self setShowsHorizontalScrollIndicator: NO];
		[self setDecelerationRate: UIScrollViewDecelerationRateFast];
		[self setDelegate: self];

		// zoomDidFinish/Begin events fire before bounce animation completes,
		// making a mess when we rearrange views during the animation.
		[self setBouncesZoom: NO];

		[self resetZoomAnimated: NO];

		// TODO: use a one shot timer to delay the display of this?
		loadingView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
		[loadingView startAnimating];
        
		[self addSubview: loadingView];

		[self loadPage];
	}
	return self;
}

- (void) dealloc
{
	// dealloc can trigger in background thread when the queued block is
	// our last owner, and releases us on completion.
	// Send the dealloc back to the main thread so we don't mess up UIKit.
	if (dispatch_get_current_queue() != dispatch_get_main_queue()) {
		__block id block_self = self; // don't auto-retain self!
		dispatch_async(dispatch_get_main_queue(), ^{ [block_self dealloc]; });
	} else {
		__block fz_page *block_page = page;
		__block fz_document *block_doc = doc;
       // fz_free_page(doc, page);
//		dispatch_async(queue, ^{
//			if (block_page)
//				fz_free_page(block_doc, block_page);
//			block_page = nil;
//		});
		[linkView release];
		[hitView release];
		[tileView release];
		[loadingView release];
		[imageView release];
		[super dealloc];
	}
}

- (int) number
{
	return number;
}

- (void) showLinks
{
	if (!linkView) {
		dispatch_async(queue, ^{
			if (!page)
				page = fz_load_page(doc, number);
			fz_link *links = fz_load_links(doc, page);
			dispatch_async(dispatch_get_main_queue(), ^{
				linkView = [[MuHitView alloc] initWithLinks: links forDocument: doc];
				dispatch_async(queue, ^{
					fz_drop_link(ctx, links);
				});
				if (imageView) {
					[linkView setFrame: [imageView frame]];
					[linkView setPageSize: pageSize];
				}
				[self addSubview: linkView];
			});
		});
	}
}

- (void) hideLinks
{
	[linkView removeFromSuperview];
	[linkView release];
	linkView = nil;
}

- (void) showSearchResults: (int)count
{
	if (hitView) {
		[hitView removeFromSuperview];
		[hitView release];
		hitView = nil;
	}
	hitView = [[MuHitView alloc] initWithSearchResults: count forDocument: doc];
	if (imageView) {
		[hitView setFrame: [imageView frame]];
		[hitView setPageSize: pageSize];
	}
	[self addSubview: hitView];
}

- (void) clearSearchResults
{
	if (hitView) {
		[hitView removeFromSuperview];
		[hitView release];
		hitView = nil;
	}
}

- (void) resetZoomAnimated: (BOOL)animated
{
	// discard tile and any pending tile jobs
	tileFrame = CGRectZero;
	tileScale = 1;
	if (tileView) {
		[tileView removeFromSuperview];
		[tileView release];
		tileView = nil;
	}

	[self setMinimumZoomScale: 1];
	[self setMaximumZoomScale: 5];
	[self setZoomScale: 1 animated: animated];
}

- (void) removeFromSuperview
{
	cancel = YES;
	[super removeFromSuperview];
}

- (void) loadPage
{
	if (number < 0 || number >= fz_count_pages(doc))
		return;
	dispatch_async(queue, ^{
		if (!cancel) {
			printf("render page %d\n", number);
			if (!page)
				page = fz_load_page(doc, number);
			CGSize size = measurePage(doc, page);
         //   logMemUsage();
			UIImage *image = renderPage(doc, page, self.bounds.size);
			dispatch_async(dispatch_get_main_queue(), ^{
				pageSize = size;
				[self displayImage: image];
				[image release];
			});
		} else {
			printf("cancel page %d\n", number);
		}
	});
}

- (void) displayImage: (UIImage*)image
{
	if (loadingView) {
		[loadingView removeFromSuperview];
		[loadingView release];
		loadingView = nil;
	}

	if (hitView)
		[hitView setPageSize: pageSize];

	if (!imageView) {
		imageView = [[UIImageView alloc] initWithImage: image];
		imageView.opaque = YES;
		[self addSubview: imageView];
		if (hitView)
			[self bringSubviewToFront: hitView];
	} else {
		[imageView setImage: image];
	}

	[self resizeImage];
}

- (void) resizeImage
{
	if (imageView) {
		CGSize imageSize = imageView.image.size;
		CGSize scale = fitPageToScreen(imageSize, self.bounds.size);
		if (fabs(scale.width - 1) > 0.1) {
			CGRect frame = [imageView frame];
			frame.size.width = imageSize.width * scale.width;
			frame.size.height = imageSize.height * scale.height;
			[imageView setFrame: frame];

			printf("resized view; queuing up a reload (%d)\n", number);
			dispatch_async(queue, ^{
				dispatch_async(dispatch_get_main_queue(), ^{
					CGSize scale = fitPageToScreen(imageView.image.size, self.bounds.size);
					if (fabs(scale.width - 1) > 0.01)
						[self loadPage];
				});
			});
		} else {
			[imageView sizeToFit];
		}

		[self setContentSize: imageView.frame.size];

		[self layoutIfNeeded];
	}

}

- (void) willRotate
{
	if (imageView) {
		[self resetZoomAnimated: NO];
		[self resizeImage];
        
	}
}

- (void) layoutSubviews
{
	[super layoutSubviews];

	// center the image as it becomes smaller than the size of the screen

	CGSize boundsSize = self.bounds.size;
	CGRect frameToCenter = loadingView ? loadingView.frame : imageView.frame;

	// center horizontally
	if (frameToCenter.size.width < boundsSize.width)
		frameToCenter.origin.x = floor((boundsSize.width - frameToCenter.size.width) / 2);
	else
		frameToCenter.origin.x = 0;

	// center vertically
	if (frameToCenter.size.height < boundsSize.height)
		frameToCenter.origin.y = floor((boundsSize.height - frameToCenter.size.height) / 2);
	else
		frameToCenter.origin.y = 0;

	if (loadingView)
		loadingView.frame = frameToCenter;
	else
		imageView.frame = frameToCenter;

	if (hitView && imageView)
		[hitView setFrame: [imageView frame]];
}

- (UIView*) viewForZoomingInScrollView: (UIScrollView*)scrollView
{
	return imageView;
}

- (void) loadTile
{
	CGSize screenSize = self.bounds.size;

	tileFrame.origin = self.contentOffset;
	tileFrame.size = self.bounds.size;
	tileFrame = CGRectIntersection(tileFrame, imageView.frame);
	tileScale = self.zoomScale;

	CGRect frame = tileFrame;
	float scale = tileScale;

	CGRect viewFrame = frame;
	if (self.contentOffset.x < imageView.frame.origin.x)
		viewFrame.origin.x = 0;
	if (self.contentOffset.y < imageView.frame.origin.y)
		viewFrame.origin.y = 0;

	if (scale < 1.01)
		return;

	dispatch_async(queue, ^{
		__block BOOL isValid;
		dispatch_sync(dispatch_get_main_queue(), ^{
			isValid = CGRectEqualToRect(frame, tileFrame) && scale == tileScale;
		});
		if (!isValid) {
			printf("cancel tile\n");
			return;
		}

		if (!page)
			page = fz_load_page(doc, number);

		printf("render tile\n");
		UIImage *image = renderTile(doc, page, screenSize, viewFrame, scale);

		dispatch_async(dispatch_get_main_queue(), ^{
			isValid = CGRectEqualToRect(frame, tileFrame) && scale == tileScale;
			if (isValid) {
				tileFrame = CGRectZero;
				tileScale = 1;
				if (tileView) {
					[tileView removeFromSuperview];
					[tileView release];
					tileView = nil;
				}

				tileView = [[UIImageView alloc] initWithFrame: frame];
				[tileView setImage: image];
				[self addSubview: tileView];
				if (hitView)
					[self bringSubviewToFront: hitView];
			} else {
				printf("discard tile\n");
			}
			[image release];
		});
	});
}

- (void) scrollViewDidScrollToTop:(UIScrollView *)scrollView { [self loadTile]; }
- (void) scrollViewDidEndScrollingAnimation:(UIScrollView *)scrollView { [self loadTile]; }
- (void) scrollViewDidEndDecelerating:(UIScrollView *)scrollView { [self loadTile]; }
- (void) scrollViewDidEndDragging:(UIScrollView *)scrollView willDecelerate:(BOOL)decelerate
{
	if (!decelerate)
		[self loadTile];
}

- (void) scrollViewWillBeginZooming: (UIScrollView*)scrollView withView: (UIView*)view
{
	// discard tile and any pending tile jobs
	tileFrame = CGRectZero;
	tileScale = 1;
	if (tileView) {
		[tileView removeFromSuperview];
		[tileView release];
		tileView = nil;
	}
}

- (void) scrollViewDidEndZooming: (UIScrollView*)scrollView withView: (UIView*)view atScale: (float)scale
{
	[self loadTile];
}

- (void) scrollViewDidZoom: (UIScrollView*)scrollView
{
	if (hitView && imageView)
		[hitView setFrame: [imageView frame]];
}

@end

#pragma mark -

@implementation MuDocumentController

- (id) initWithFilename: (NSString*)filename document: (fz_document *)aDoc
{
	self = [super init];
	if (!self)
		return nil;

	key = [filename retain];
	doc = aDoc;
    
    /*
    *unlock the file ,if it has been locked
     */
  // [self unlock];
    
	dispatch_sync(queue, ^{});

	fz_outline *root = fz_load_outline(doc);
	if (root) {
		NSMutableArray *titles = [[NSMutableArray alloc] init];
		NSMutableArray *pages = [[NSMutableArray alloc] init];
		flattenOutline(titles, pages, root, 0);
		if ([titles count])
			outline = [[MuOutlineController alloc] initWithTarget: self titles: titles pages: pages];
		[titles release];
		[pages release];
		fz_free_outline(ctx, root);
	}

	return self;
}

- (void) loadView
{
	[[NSUserDefaults standardUserDefaults] setObject: key forKey: @"OpenDocumentKey"];

	current = [[NSUserDefaults standardUserDefaults] integerForKey: key];
	if (current < 0 || current >= fz_count_pages(doc))
		current = 0;

	UIView *view = [[UIView alloc] initWithFrame: CGRectZero];
	[view setAutoresizingMask: UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight];
	[view setAutoresizesSubviews: YES];

	canvas = [[UIScrollView alloc] initWithFrame: CGRectMake(0,0,GAP,0)];
	[canvas setAutoresizingMask: UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight];
	[canvas setPagingEnabled: YES];
	[canvas setShowsHorizontalScrollIndicator: NO];
	[canvas setShowsVerticalScrollIndicator: NO];
	[canvas setDelegate: self];

	[canvas addGestureRecognizer: [[[UITapGestureRecognizer alloc] initWithTarget: self action: @selector(onTap:)] autorelease]];

	scroll_animating = NO;

	indicator = [[UILabel alloc] initWithFrame: CGRectZero];
	[indicator setAutoresizingMask: UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleTopMargin];
	[indicator setText: @"0000 of 9999"];
	[indicator sizeToFit];
	[indicator setCenter: CGPointMake(0, INDICATOR_Y)];
	[indicator setTextAlignment: UITextAlignmentCenter];
	[indicator setBackgroundColor: [[UIColor blackColor] colorWithAlphaComponent: 0.5]];
	[indicator setTextColor: [UIColor whiteColor]];

	[view addSubview: canvas];
	[view addSubview: indicator];

	slider = [[UISlider alloc] initWithFrame: CGRectZero];
	[slider setMinimumValue: 0];
	[slider setMaximumValue: fz_count_pages(doc) - 1];
	[slider addTarget: self action: @selector(onSlide:) forControlEvents: UIControlEventValueChanged];

	sliderWrapper = [[UIBarButtonItem alloc] initWithCustomView: slider];

	[self setToolbarItems: [NSArray arrayWithObjects: sliderWrapper, nil]];

	// Set up the buttons on the navigation and search bar

	if (outline) {
		outlineButton = [[UIBarButtonItem alloc]
			initWithBarButtonSystemItem: UIBarButtonSystemItemBookmarks
			target:self action:@selector(onShowOutline:)];
	}
	linkButton = [[UIBarButtonItem alloc]
		initWithBarButtonSystemItem: UIBarButtonSystemItemAction
		target:self action:@selector(onToggleLinks:)];
	cancelButton = [[UIBarButtonItem alloc]
		initWithTitle: @"Cancel" style: UIBarButtonItemStyleBordered
		target:self action:@selector(onCancelSearch:)];
	searchButton = [[UIBarButtonItem alloc]
		initWithBarButtonSystemItem: UIBarButtonSystemItemSearch
		target:self action:@selector(onShowSearch:)];
	prevButton = [[UIBarButtonItem alloc]
		initWithBarButtonSystemItem: UIBarButtonSystemItemRewind
		target:self action:@selector(onSearchPrev:)];
	nextButton = [[UIBarButtonItem alloc]
		initWithBarButtonSystemItem: UIBarButtonSystemItemFastForward
		target:self action:@selector(onSearchNext:)];

    UIBarButtonItem * addPass = [[UIBarButtonItem alloc]
                                 initWithTitle: @"AddPass" style: UIBarButtonItemStyleBordered
                                 target:self action:@selector(addPassword:)];
    UIBarButtonItem * remove = [[UIBarButtonItem alloc]
                                 initWithTitle: @"remove" style: UIBarButtonItemStyleBordered
                                 target:self action:@selector(removePassword:)];
    UIBarButtonItem * modify = [[UIBarButtonItem alloc]
                                 initWithTitle: @"modify" style: UIBarButtonItemStyleBordered
                                 target:self action:@selector(modifyPassword:)];
    UIBarButtonItem * permission = [[UIBarButtonItem alloc]
                                initWithTitle: @"permission" style: UIBarButtonItemStyleBordered
                                target:self action:@selector(modifyPermission:)];


	searchBar = [[UISearchBar alloc] initWithFrame: CGRectMake(0,0,50,32)];
	[searchBar setPlaceholder: @"Search"];
	[searchBar setDelegate: self];
	// HACK to make transparent background
	[[searchBar.subviews objectAtIndex:0] removeFromSuperview];

	[prevButton setEnabled: NO];
	[nextButton setEnabled: NO];

	[[self navigationItem] setRightBarButtonItems:
		[NSArray arrayWithObjects: searchButton, linkButton,addPass,remove,modify,permission,nil]];
    [addPass release];
    [remove release];
    [modify release];
	// TODO: add activityindicator to search bar

	[self setView: view];
	[view release];
}

/*
 unlock on load
 */

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
//    pdf_print_obj(D);
//    pdf_print_obj(On);
//    pdf_print_obj(Off);
    pdf_dict_dels(D, "ON");
    pdf_dict_dels(D, "OFF");
   
    
    int len = pdf_array_len(array);
    
    // for(i=0;i<len;i++)
    // {
    pdf_obj *o = pdf_array_get(array,0);
    pdf_obj *o1 = pdf_array_get(array, 1);
    // }
    pdf_obj *ao = pdf_new_array((pdf_document *)doc, 1);
    pdf_array_push_drop(ao, o);
     pdf_obj *ao1 = pdf_new_array((pdf_document *)doc, 1);
    pdf_array_push_drop(ao1, o1);
    pdf_dict_putp_drop(D, "ON", ao);
    pdf_dict_putp_drop(D, "OFF", ao1);
    
   // pdf_update_object(((pdf_document *)doc), pdf_to_num(D), 0);
    
    pdf_obj *oo = pdf_load_object(((pdf_document *)doc), pdf_to_num(o), pdf_to_gen(o));
    pdf_obj *oo1 = pdf_load_object(((pdf_document *)doc), pdf_to_num(o1), pdf_to_gen(o1));
    
    pdf_dict_putp_drop(oo, "Usage/View/ViewState",pdf_new_name(((pdf_document*)doc),"ON"));
    pdf_dict_putp_drop(oo1,"Usage/View/ViewState",pdf_new_name(((pdf_document*)doc),"OFF"));
    
    pdf_update_object(((pdf_document *)doc), pdf_to_num(oo), 0);
    pdf_update_object(((pdf_document *)doc), pdf_to_num(oo1), 0);
    
    ((pdf_document *)doc)->ocg[0].ocgs->state =1;

    fz_write_options ops;
    int error = 0 ;
    ops.do_incremental = 0;
    ops.continue_on_error = 1;
    ops.do_linear = 0;
    ops.do_garbage = 0;
    ops.do_expand = 0 ;
    ops.do_ascii = 0 ;
    ops.errors = &error;
    
    pdf_write_document((pdf_document *)doc,"/Users/houyushen/Desktop/out.pdf", &ops);
}

- (void) dealloc
{
	if (doc) {
		fz_document *self_doc = doc; // don't auto-retain self here!
		dispatch_async(queue, ^{
			printf("close document\n");
			fz_close_document(self_doc);
		});
	}

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

	[outline release];
	[key release];
	[super dealloc];
}

- (void) viewWillAppear: (BOOL)animated
{
	[self setTitle: [key lastPathComponent]];

	[slider setValue: current];

	[indicator setText: [NSString stringWithFormat: @" %d of %d ", current+1, fz_count_pages(doc)]];

	[[self navigationController] setToolbarHidden: NO animated: animated];
}

- (void) viewWillLayoutSubviews
{
	CGSize size = [canvas frame].size;
	int max_width = fz_max(width, size.width);

	width = size.width;
	height = size.height;

	[canvas setContentInset: UIEdgeInsetsZero];
	[canvas setContentSize: CGSizeMake(fz_count_pages(doc) * width, height)];
	[canvas setContentOffset: CGPointMake(current * width, 0)];

	[sliderWrapper setWidth: SLIDER_W];
	[searchBar setFrame: CGRectMake(0,0,SEARCH_W,32)];

	[[[self navigationController] toolbar] setNeedsLayout]; // force layout!

	// use max_width so we don't clamp the content offset too early during animation
	[canvas setContentSize: CGSizeMake(fz_count_pages(doc) * max_width, height)];
	[canvas setContentOffset: CGPointMake(current * width, 0)];

	for (MuPageView *view in [canvas subviews]) {
		if ([view number] == current) {
			[view setFrame: CGRectMake([view number] * width, 0, width-GAP, height)];
			[view willRotate];
		}
	}
	for (MuPageView *view in [canvas subviews]) {
		if ([view number] != current) {
			[view setFrame: CGRectMake([view number] * width, 0, width-GAP, height)];
			[view willRotate];
		}
	}
}

- (void) viewDidAppear: (BOOL)animated
{
	[self scrollViewDidScroll: canvas];
}

- (void) viewWillDisappear: (BOOL)animated
{
	[self setTitle: @"Resume"];
	[[NSUserDefaults standardUserDefaults] removeObjectForKey: @"OpenDocumentKey"];
	[[self navigationController] setToolbarHidden: YES animated: animated];
}

- (void) showNavigationBar
{
	if ([[self navigationController] isNavigationBarHidden]) {
		[[self navigationController] setNavigationBarHidden: NO];
		[[self navigationController] setToolbarHidden: NO];
		[indicator setHidden: NO];

		[UIView beginAnimations: @"MuNavBar" context: NULL];

		[[[self navigationController] navigationBar] setAlpha: 1];
		[[[self navigationController] toolbar] setAlpha: 1];
		[indicator setAlpha: 1];

		[UIView commitAnimations];
	}
}

- (void) hideNavigationBar
{
	if (![[self navigationController] isNavigationBarHidden]) {
		[searchBar resignFirstResponder];

		[UIView beginAnimations: @"MuNavBar" context: NULL];
		[UIView setAnimationDelegate: self];
		[UIView setAnimationDidStopSelector: @selector(onHideNavigationBarFinished)];

		[[[self navigationController] navigationBar] setAlpha: 0];
		[[[self navigationController] toolbar] setAlpha: 0];
		[indicator setAlpha: 0];

		[UIView commitAnimations];
	}
}

- (void) onHideNavigationBarFinished
{
	[[self navigationController] setNavigationBarHidden: YES];
	[[self navigationController] setToolbarHidden: YES];
	[indicator setHidden: YES];
}

- (void) onShowOutline: (id)sender
{
	[[self navigationController] pushViewController: outline animated: YES];
}

#pragma mark- 
#pragma mark  about the password handle
- (void) addPassword: (id)sender
{
    
    doc = (fz_document *) pdf_add_password((pdf_document *)doc,filename_g, "123", "321",ALLOWALL);
    fz_point p;
    p.x =300;
    p.y = 300;
    
    
    NSError *error=[[NSError alloc]init];
    unsigned char *data = NULL;//(unsigned char*)malloc(1024*sizeof(unsigned char));
    int length=0;
//    pdf_page *pp = pdf_load_page((pdf_document*)doc, 0);
//    data=pdf_get_sound_data((pdf_document *)doc,pp->annots, &length);
//    doc =(fz_document *)pdf_add_sound((pdf_document *)doc, 0,p, "/Users/houyushen/Desktop/a.wav");
//     doc =(fz_document *)pdf_add_video((pdf_document *)doc, 0,p, "/Users/houyushen/Desktop/kobe.flv");
//    NSData *nsdata = [[NSData alloc]initWithBytes:data length:length];
//    AVAudioPlayer *player = [[AVAudioPlayer alloc]initWithData:nsdata  error:&error];
//    [player setVolume:1.0];
//    bool b=[player play];
//    FILE *f = fopen("/Users/houyushen/Desktop/swf", "wb");
//    fz_stream *stream = pdf_open_stream( (pdf_document *)doc,111, 0);
//    fz_buffer *buf = fz_read_all(stream, 1024);
//    printf("\n%s\n",buf->data+buf->len-1420);
////    fwrite(buf->data, sizeof(unsigned char),buf->len-1417,  f);
//   int c =  fwrite(buf->data, sizeof(unsigned char),buf->len,  f);
//   int b =  ferror(f);
//    fclose(f);
//    fz_stream *s = fz_open_file(((pdf_document*)doc)->ctx, "/Users/houyushen/Desktop/swf");
//    fz_buffer *v = fz_read_all(s, 1024);
    
//
    save((pdf_document *)doc, SAVE);
    for(MuPageView *view in [canvas subviews])
    {
        [view removeFromSuperview];
    }
    [self createPageView:current -1];
    [self createPageView:current];
    [self createPageView:current+1];
   
}
- (void) removePassword: (id)sender
{
    doc = (fz_document *)pdf_remove_password((pdf_document*)doc,filename_g);
}
- (void) modifyPassword: (id)sender
{
    doc =(fz_document *) pdf_modify_password((pdf_document*)doc,filename_g, "123","456");
}
- (void) modifyPermission: (id)sender
{
    doc = (fz_document *)pdf_modify_permission((pdf_document *)doc,"/Users/houyushen/Desktop/out.pdf","123","321",ALLOWALL);
}

- (void) onToggleLinks: (id)sender
{

   // ExtractPage(0, 2, filename_g, "/Users/houyushen/Desktop/eout.pdf");
    //pdf_extract_pages((pdf_document *)doc, 0, 2, "/Users/houyushen/Desktop/eout1.pdf");
   // save((pdf_document*)doc, SAVEAS);
    char filename[PATH_MAX];
    char filename1[PATH_MAX];
    char filename2[PATH_MAX];
    char savefile[PATH_MAX];
	strcpy(filename, [NSHomeDirectory() UTF8String]);
	strcat(filename, "/Documents/");
	strcat(filename, "PDF32000_2008.pdf");//修改文件名
    
    strcpy(filename1, [NSHomeDirectory() UTF8String]);
	strcat(filename1, "/Documents/");
	strcat(filename1, "What's New.pdf");//修改文件名

    strcpy(filename2, [NSHomeDirectory() UTF8String]);
	strcat(filename2, "/Documents/");
	strcat(filename2, "The_C++_Programming_Language,Special_Edition.pdf");//修改文件名
    
    strcpy(savefile, [NSHomeDirectory() UTF8String]);
    strcat(savefile, "/Documents/");
    strcat(savefile, "new.pdf");
    fz_context *ctx = fz_new_context(NULL, NULL, 128<<20);
    
    pdf_document *new_doc = pdf_open_document(ctx, filename);
    pdf_document *new_doc1 = pdf_open_document(ctx, filename1);
//    pdf_document *new2 = pdf_open_document(ctx, filename2);
//    pdf_authenticate_password(new_doc, "321");
 
    doc = (fz_document*)pdf_merge_page((pdf_document*)doc, new_doc1, 1, 0,savefile);
    //doc =  (fz_document*)pdf_merge_file(filename_g,4,(pdf_document*)doc,new_doc,new_doc1,new2);
//    pdf_set_author((pdf_document*)doc, "suyuancheng");
//    pdf_set_title((pdf_document*)doc,"merged file");
//   // pdf_add_password((pdf_document*)doc, filename_g, "123", "321", ALLOWALL);
//    pdf_save_merged_file((pdf_document*)doc,filename_g);
    pdf_close_document(new_doc1);
    pdf_close_document(new_doc);
//    pdf_close_document(new2);
    save((pdf_document*)doc,SAVE);
   
    //
    //int per = pdf_allow_copy((pdf_document *)doc);
    //int per = pdf_allow_print((pdf_document *)doc);
    //int per = pdf_allow_add_annotation((pdf_document *)doc);
//    pdf_modify_permission((pdf_document *)doc,"123",NOPRINT&NOCOPY);
//    save((pdf_document*)doc, SAVE);
    //add watermark
    //addWatermark((pdf_document *)doc, current);
 //   [self addWatermark:(pdf_document *)doc pageNum:current];
    
    //pdf_remove_password((pdf_document *)doc);
   // oldflag = 1;
   //pdf_add_password((pdf_document *)doc, "123", "123");
    //pdf_modify_password((pdf_document *)doc, "456");
//    int  i =0 ;
//    int count =  pdf_count_objects((pdf_document *)doc);
//    for( i = 0;i<count;i++)
//    {
//        if( !pdf_is_stream((pdf_document *)doc, i, 0))continue;
//        fz_stream *stream2 = pdf_open_stream( (pdf_document *)doc,i, 0);
//        fz_buffer *buf2 = fz_read_all(stream2, 1024);
//        printf("plaitext:%s\n",buf2->data);
//    }
    

    
//	showLinks = !showLinks;
// 	for (MuPageView *view in [canvas subviews])
//	{
//		if (showLinks)
//			[view showLinks];
//		else
//			[view hideLinks];
//	}

    
}

#pragma mark- addwatermark
-(void)addWatermark:(pdf_document *)pdfdoc pageNum:(int)pageNum
{
    const char *imagename = "/Users/houyushen/Library/Application Support/iPhone Simulator/6.1/Applications/2AEB2534-8AE0-4007-833C-06692800D127/Documents/12.jpg";
    const fz_rect bbox = {0,0,2120,1400};
    pdf_create_watermark_with_img(pdfdoc, pageNum, imagename, 1920, 1200, &bbox);
    
    /*
     write out
     */
    save(pdfdoc,SAVE);
    pdf_close_document(pdfdoc);
    doc = fz_open_document(ctx, filename_g);
    
    for(MuPageView *view in [canvas subviews])
    {
        [view removeFromSuperview];
    }
    [self createPageView:current -1];
    [self createPageView:current];
    [self createPageView:current+1];
}
- (void) onShowSearch: (id)sender
{
//    pdf_update_watermark_with_img((pdf_document *)doc,current, "/Users/houyushen/Desktop/kobe.jpg", 220, 165);
//    save((pdf_document *)doc, SAVE);
//    pdf_close_document((pdf_document *)doc);
//    doc = fz_open_document(ctx, filename_g);
//    for(MuPageView *view in [canvas subviews])
//    {
//        [view removeFromSuperview];
//    }
//    [self createPageView:current -1];
//    [self createPageView:current];
//    [self createPageView:current+1];

	[[self navigationItem] setTitleView: searchBar];
	[[self navigationItem] setRightBarButtonItems:
		[NSArray arrayWithObjects: nextButton, prevButton, nil]];
	[[self navigationItem] setLeftBarButtonItem: cancelButton];
	[searchBar becomeFirstResponder];
}

- (void) onCancelSearch: (id)sender
{
	cancelSearch = YES;
	[searchBar resignFirstResponder];
	[[self navigationItem] setTitleView: nil];
	[[self navigationItem] setRightBarButtonItems:
		[NSArray arrayWithObjects: searchButton, linkButton, outlineButton, nil]];
	[[self navigationItem] setLeftBarButtonItem: nil];
	[self resetSearch];
}

- (void) resetSearch
{
	searchPage = -1;
	for (MuPageView *view in [canvas subviews])
		[view clearSearchResults];
}

- (void) showSearchResults: (int)count forPage: (int)number
{
	printf("search found match on page %d\n", number);
	searchPage = number;
	[self gotoPage: number animated: NO];
	for (MuPageView *view in [canvas subviews])
		if ([view number] == number)
			[view showSearchResults: count];
		else
			[view clearSearchResults];
}

- (void) searchInDirection: (int)dir
{
	UITextField *searchField;
	char *needle;
	int start;

	[searchBar resignFirstResponder];

	if (searchPage == current)
		start = current + dir;
	else
		start = current;

	needle = strdup([[searchBar text] UTF8String]);

	searchField = nil;
	for (id view in [searchBar subviews])
		if ([view isKindOfClass: [UITextField class]])
			searchField = view;

	[prevButton setEnabled: NO];
	[nextButton setEnabled: NO];
	[searchField setEnabled: NO];

	cancelSearch = NO;

	dispatch_async(queue, ^{
		for (int i = start; i >= 0 && i < fz_count_pages(doc); i += dir) {
			int n = search_page(doc, i, needle, NULL);
			if (n) {
				dispatch_async(dispatch_get_main_queue(), ^{
					[prevButton setEnabled: YES];
					[nextButton setEnabled: YES];
					[searchField setEnabled: YES];
					[self showSearchResults: n forPage: i];
					free(needle);
				});
				return;
			}
			if (cancelSearch) {
				dispatch_async(dispatch_get_main_queue(), ^{
					[prevButton setEnabled: YES];
					[nextButton setEnabled: YES];
					[searchField setEnabled: YES];
					free(needle);
				});
				return;
			}
		}
		dispatch_async(dispatch_get_main_queue(), ^{
			printf("no search results found\n");
			[prevButton setEnabled: YES];
			[nextButton setEnabled: YES];
			[searchField setEnabled: YES];
			UIAlertView *alert = [[UIAlertView alloc]
				initWithTitle: @"No matches found for:"
				message: [NSString stringWithUTF8String: needle]
				delegate: nil
				cancelButtonTitle: @"Close"
				otherButtonTitles: nil];
			[alert show];
			[alert release];
			free(needle);
		});
	});
}

- (void) onSearchPrev: (id)sender
{
	[self searchInDirection: -1];
}

- (void) onSearchNext: (id)sender
{
	[self searchInDirection: 1];
}

- (void) searchBarSearchButtonClicked: (UISearchBar*)sender
{
	[self onSearchNext: sender];
}

- (void) searchBar: (UISearchBar*)sender textDidChange: (NSString*)searchText
{
	[self resetSearch];
	if ([[searchBar text] length] > 0) {
		[prevButton setEnabled: YES];
		[nextButton setEnabled: YES];
	} else {
		[prevButton setEnabled: NO];
		[nextButton setEnabled: NO];
	}
}

- (void) onSlide: (id)sender
{
	int number = [slider value];
	if ([slider isTracking])
		[indicator setText: [NSString stringWithFormat: @" %d of %d ", number+1, fz_count_pages(doc)]];
	else
		[self gotoPage: number animated: NO];
}

//- (void) onTap: (UITapGestureRecognizer*)sender
//{
//	CGPoint p = [sender locationInView: canvas];
//	CGPoint ofs = [canvas contentOffset];
//	float x0 = (width - GAP) / 5;
//	float x1 = (width - GAP) - x0;
//	p.x -= ofs.x;
//	p.y -= ofs.y;
//	if (p.x < x0) {
//		[self gotoPage: current-1 animated: YES];
//	} else if (p.x > x1) {
//		[self gotoPage: current+1 animated: YES];
//	} else {
//		if ([[self navigationController] isNavigationBarHidden])
//			[self showNavigationBar];
//		else
//			[self hideNavigationBar];
//	}
//}

#pragma mark - action function

- (void) onTap: (UITapGestureRecognizer*)sender
{
	CGPoint p = [sender locationInView: canvas];
	CGPoint ofs = [canvas contentOffset];
    CGSize screenSize = self.view.bounds.size;
    
   
    
	float x0 = (width - GAP) / 5;
	float x1 = (width - GAP) - x0;
	p.x -= ofs.x;
	p.y -= ofs.y;
    fz_point po;
    po.x = p.x/1.215126;
    po.y =p.y/1.2161521;
    
    NSError *error=[[NSError alloc]init];
    unsigned char *data = NULL;//(unsigned char*)malloc(1024*sizeof(unsigned char));
    int length=0;
//    if(fl==0)
// {
//   doc =(fz_document *)pdf_add_sound((pdf_document *)doc, 0,po, "/Users/houyushen/Desktop/happy.wav");
//    save((pdf_document *)doc, SAVE);
//     fl =1;
//    }
//      pdf_page *pp = pdf_load_page((pdf_document*)doc, 0);
//

    pdf_annot *annot = pdf_get_sound_annot((pdf_document *)doc, current, po);
//////    int length =0;
//    pdf_page *pp = pdf_load_page((pdf_document*)doc, 0);
   data= pdf_get_sound_data((pdf_document *)doc,annot, &length);
    NSData *nsdata = [[NSData alloc]initWithBytes:data length:length];
    AVAudioPlayer *player = [[AVAudioPlayer alloc]initWithData:nsdata  error:&error];
    [player setVolume:1.0];
    bool b=[player play];
    
    
    //////////////////////////////////////////////////
    fz_page *page = fz_load_page(doc, current);
    pdf_ui_event event = set_ui_evet(p,doc,page,screenSize);
    
    pdf_obj *obj = tapInArea(&event, (pdf_document *)doc, current);
    
    if (obj){
        [self widgetOnTouch:obj andpage:(pdf_page *)page andevent:&event];
    }
	else if (p.x < x0) {
		[self gotoPage: current-1 animated: YES];
	} else if (p.x > x1) {
		[self gotoPage: current+1 animated: YES];
	} else {
		if ([[self navigationController] isNavigationBarHidden])
			[self showNavigationBar];
		else
			[self hideNavigationBar];
	}
}

- (void) widgetOnTouch: (pdf_obj *)obj andpage: (pdf_page *)page andevent:(pdf_ui_event *)event
{
    char *utf8;
    pdf_obj *root;
    pdf_obj *acroform;
    pdf_obj *form;
    int i = 0;
    char *creatorV = "Vitrium Systems Inc";
    char *creator;
    switch ( pdf_field_type((pdf_document *)doc, obj)) {
        case PDF_WIDGET_TYPE_TEXT:
          //  pdf_field_set_value((pdf_document *)doc, obj, "test!");
            
            //set the username and pass word
            if (!strcmp(pdf_field_name((pdf_document *)doc, obj), "boom.form.response.username"))
                pdf_field_set_value((pdf_document *)doc, obj, "wangw");
            else
                pdf_field_set_value((pdf_document *)doc, obj, "123");
            for(MuPageView *view in [canvas subviews])
            {
                if ([view number] == current) {
                    [view removeFromSuperview];
                    [self createPageView:current];
                }
            }
            break;
        case PDF_WIDGET_TYPE_PUSHBUTTON:
            //直接执行解锁部分函数
           // pdf_js_execute(((pdf_document *)doc)->js, "r();");
            
            //handler the vitrium system inc ebook
//            creator = pdf_to_str_buf(pdf_dict_getp(pdf_trailer((pdf_document *)doc), "Info/Creator"));
//            if(strncmp(creator, creatorV, 19))
//            {
//                break;
//            }
//            
//            /*
//             *与js不交互，直接解锁
//             */
//            utf8 = "boom.info";
//            root = pdf_dict_gets(pdf_trailer((pdf_document *)doc), "Root");
//            acroform = pdf_dict_gets(root, "AcroForm");
//            form = pdf_dict_gets(acroform, "Fields");
//            
//            //find the boom.info
//            pdf_obj * dict = pdf_lookup_field(form, utf8);
//           //hidden the boom.info
//            pdf_field_set_display((pdf_document *)(doc), dict, 1);
//            
//            //set the ocgs properties to show the content
//            pdf_obj  *ocg = pdf_dict_gets(pdf_dict_gets(pdf_trailer((pdf_document *)doc), "Root"), "OCProperties");
//            pdf_obj *array = pdf_dict_gets(ocg, "OCGs");//get the array of the ocgs
//              
//            int len = pdf_array_len(array);
//           // for(i=0;i<len;i++)
//           // {
//                pdf_obj *o = pdf_array_get(array,0);
//                pdf_obj *o1 = pdf_array_get(array, 1);
//           // }
//            pdf_obj *oo = pdf_load_object(((pdf_document *)doc), pdf_to_num(o), pdf_to_gen(o));
//            pdf_obj *oo1 = pdf_load_object(((pdf_document *)doc), pdf_to_num(o1), pdf_to_gen(o1));
//            pdf_dict_putp(oo, "Usage/View/ViewState",pdf_new_name(((pdf_document*)doc),"ON"));
//            pdf_dict_putp(oo1,"Usage/View/ViewState",pdf_new_name(((pdf_document*)doc),"OFF"));
//            ((pdf_document *)doc)->ocg[0].ocgs->state =1;
//            
//            
//          //  ((pdf_document *)doc)->ocg[1].ocgs->state =0;
//            //pdf_pass_event((pdf_document *)doc, (pdf_page *)page, event);
//            
//            //reload the page
//            for(MuPageView *view in [canvas subviews])
//            {
//                [view removeFromSuperview];
//            }
//            [self createPageView:current-1];
//            [self createPageView:current];
//            [self createPageView:current+1];
//
        
            break;
        default:
            break;
    }
}

- (void) scrollViewWillBeginDragging: (UIScrollView *)scrollView
{
	[self hideNavigationBar];
}

- (void) scrollViewDidScroll: (UIScrollView*)scrollview
{
	if (width == 0)
		return; // not visible yet

	if (scroll_animating)
		return; // don't mess with layout during animations

	float x = [canvas contentOffset].x + width * 0.5f;
	current = x / width;

	[[NSUserDefaults standardUserDefaults] setInteger: current forKey: key];

	[indicator setText: [NSString stringWithFormat: @" %d of %d ", current+1, fz_count_pages(doc)]];
	[slider setValue: current];

	// swap the distant page views out

	NSMutableSet *invisiblePages = [[NSMutableSet alloc] init];
	for (MuPageView *view in [canvas subviews]) {
		if ([view number] != current)
		//	[view resetZoomAnimated: YES];
		if ([view number] < current - 2 || [view number] > current + 2)
			[invisiblePages addObject: view];
	}
	for (MuPageView *view in invisiblePages)
		[view removeFromSuperview];
	[invisiblePages release]; // don't bother recycling them...

	[self createPageView: current];
	[self createPageView: current - 1];
	[self createPageView: current + 1];

	// reset search results when page has flipped
	if (current != searchPage)
		[self resetSearch];
}

- (void) createPageView: (int)number
{
	if (number < 0 || number >= fz_count_pages(doc))
		return;
	int found = 0;
	for (MuPageView *view in [canvas subviews])
		if ([view number] == number)
			found = 1;
	if (!found) {
		MuPageView *view = [[MuPageView alloc] initWithFrame: CGRectMake(number * width, 0, width-GAP, height) document: doc page: number];
        printf("%d\n",[view retainCount]);
		[canvas addSubview: view];
        printf("%d\n",[view retainCount]);
		if (showLinks)
			[view showLinks];
		[view release];
        printf("%d\n",[view retainCount]);
	}
}

- (void) gotoPage: (int)number animated: (BOOL)animated
{
	if (number < 0)
		number = 0;
	if (number >= fz_count_pages(doc))
		number = fz_count_pages(doc) - 1;
	if (current == number)
		return;
	if (animated) {
		// setContentOffset:animated: does not use the normal animation
		// framework. It also doesn't play nice with the tap gesture
		// recognizer. So we do our own page flipping animation here.
		// We must set the scroll_animating flag so that we don't create
		// or remove subviews until after the animation, or they'll
		// swoop in from origo during the animation.

		scroll_animating = YES;
		[UIView beginAnimations: @"MuScroll" context: NULL];
		[UIView setAnimationDuration: 0.4];
		[UIView setAnimationBeginsFromCurrentState: YES];
		[UIView setAnimationDelegate: self];
		[UIView setAnimationDidStopSelector: @selector(onGotoPageFinished)];

		for (MuPageView *view in [canvas subviews])
			[view resetZoomAnimated: NO];

		[canvas setContentOffset: CGPointMake(number * width, 0)];
		[slider setValue: number];
		[indicator setText: [NSString stringWithFormat: @" %d of %d ", number+1, fz_count_pages(doc)]];

		[UIView commitAnimations];
	} else {
		for (MuPageView *view in [canvas subviews])
			[view resetZoomAnimated: NO];
		[canvas setContentOffset: CGPointMake(number * width, 0)];
	}
	current = number;
}

- (void) onGotoPageFinished
{
	scroll_animating = NO;
	[self scrollViewDidScroll: canvas];
}

- (BOOL) shouldAutorotateToInterfaceOrientation: (UIInterfaceOrientation)o
{
	return YES;
}

- (void) didRotateFromInterfaceOrientation: (UIInterfaceOrientation)o
{
	[canvas setContentSize: CGSizeMake(fz_count_pages(doc) * width, height)];
	[canvas setContentOffset: CGPointMake(current * width, 0)];
}

@end

#pragma mark -

@implementation MuAppDelegate

- (BOOL) application: (UIApplication*)application didFinishLaunchingWithOptions: (NSDictionary*)launchOptions
{
	NSString *filename;

	queue = dispatch_queue_create("com.artifex.mupdf.queue", NULL);

	// use at most 128M for resource cache
	ctx = fz_new_context(NULL, NULL, 128<<20);

	screenScale = [[UIScreen mainScreen] scale];//the read only property
  
	library = [[MuLibrayController alloc] initWithStyle: UITableViewStylePlain];
	navigator = [[UINavigationController alloc] initWithRootViewController: library];
    UIImage *imaget = [UIImage imageNamed:@"DefaultNavigationBar.png"];
    [[navigator navigationBar]setBackgroundImage:imaget forBarMetrics:UIBarMetricsDefault];
   
    
    [imaget release];
    [[navigator navigationBar]setAlpha:0.6];
	[[navigator navigationBar] setTranslucent: NO];
	[[navigator toolbar] setTranslucent: NO];
	[navigator setDelegate: self];

     UIViewController *setting = [[UIViewController alloc]init];
    [setting setTitle:@"Settings"];
    UITabBarController *tab = [[UITabBarController alloc]init];
   
    
   // tab.viewControllers = [NSArray arrayWithObjects:navigator,setting, nil];
    
   
	window = [[UIWindow alloc] initWithFrame: [[UIScreen mainScreen] bounds]];
	[window setBackgroundColor: [UIColor scrollViewTexturedBackgroundColor]];
	[window setRootViewController: navigator];
	[window makeKeyAndVisible];
    
    [tab release];

//	filename = [[NSUserDefaults standardUserDefaults] objectForKey: @"OpenDocumentKey"];
//	if (filename)
//		[library openDocument: filename];
//
//	filename = [launchOptions objectForKey: UIApplicationLaunchOptionsURLKey];
//	NSLog(@"urlkey = %@\n", filename);

	return YES;
}
- (void)backAction
{
    [navigator popViewControllerAnimated:YES];
}
- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation
{
	if ([url isFileURL]) {
		NSString *path = [url path];
		NSString *dir = [NSString stringWithFormat: @"%@/Documents/", NSHomeDirectory()];
		path = [path stringByReplacingOccurrencesOfString:@"/private" withString:@""];
		path = [path stringByReplacingOccurrencesOfString:dir withString:@""];
		NSLog(@"file relative path: %@\n", path);
		[library openDocument:path];
		return YES;
	}
	return NO;
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	printf("applicationDidEnterBackground!\n");
	[[NSUserDefaults standardUserDefaults] synchronize];
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	printf("applicationWillEnterForeground!\n");
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	printf("applicationDidBecomeActive!\n");
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	printf("applicationWillTerminate!\n");
	[[NSUserDefaults standardUserDefaults] synchronize];
}

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
	printf("applicationDidReceiveMemoryWarning\n");
}

- (void) dealloc
{
	dispatch_release(queue);
	[library release];
	[navigator release];
	[window release];
	[super dealloc];
}

@end
#pragma mark -  rc4 encrypt


#pragma mark -
void test(int fmt,...)
{
    char te ;
    pdf_document * tes;
    va_list list;
    
    va_start(list, fmt);
    while(1)
    {
    tes= va_arg(list, pdf_document*);
   
    }
    printf("%s,%c",tes,te);
    
}
const int na = 0x80000000;
int te(double v)
{
    int va;
    int iv = v;
    if(iv == na)
        va = - 1;
        else
            va = v;
    return va;
}

int main(int argc, char *argv[])
{

   // te(0x800000000);
//    pdf_document *t1;
//    pdf_document *t2;
//test(120,t1,t2);

//    oldKey = (unsigned char *)malloc(32*sizeof(unsigned char));
    /*
     第一文件是output文件，第二个文件是要拆分的文件，最后一个参数是页号
     */
    char *argva[]={"", "-o","/Users/houyushen/Library/Application Support/iPhone Simulator/7.0/Applications/6C5EEFCE-016D-435A-8CBF-3A33C1042AA5/Documents/【iPhone开发】XCode4开发iPhone应用的体验 - 小草 - 博客园.pdf","/Users/houyushen/Library/Application Support/iPhone Simulator/7.0/Applications/6C5EEFCE-016D-435A-8CBF-3A33C1042AA5/Documents/jpgtest.pdf","1"};
//    char *argva = "-o【iPhone开发】XCode4开发iPhone应用的体验 - 小草 - 博客园.pdf 1";
//    mmain(5,argva);
  //  NSString *home = NSHomeDirectory();
    char filename[PATH_MAX];
    char filename1[PATH_MAX];
    char filename2[PATH_MAX];
    strcpy(filename, [NSHomeDirectory() UTF8String]);
	strcat(filename, "/Documents/");
    strcat(filename, "neww.pdf");
//    set_video_path(filename);
    strcpy(filename1, [NSHomeDirectory() UTF8String]);
	strcat(filename1, "/Documents/");
//    strcat(filename1, "sound.jpg");
//    set_video_image_path(filename1);
    strcat(filename1, "PDF32000_2008.pdf");
//    pdf_merge_or_split(filename,filename1," ","1-3");
    ///////////////////
    strcpy(filename2, [NSHomeDirectory() UTF8String]);
	strcat(filename2, "/Documents/");
    strcat(filename2, "SkinOverAllNoFullNoCaption");
    set_SkinOverAllNoFullNoCaption_path(filename2);
    
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	int retVal;

	@try {
		retVal = UIApplicationMain(argc, argv, nil, @"MuAppDelegate");
	}
	@catch (NSException* exception) {
		NSLog(@"Uncaught exception %@", exception);
		NSLog(@"Stack trace: %@", [exception callStackSymbols]);
	}

	[pool release];
	return retVal;
}

