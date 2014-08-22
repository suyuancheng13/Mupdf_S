//
//  MuPageViewer.m
//  MuPDF
//
//  Created by Ｙuancheng SU on 14-6-25.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import "MuPageViewer.h"
extern dispatch_queue_t queue;
extern fz_context *ctx;
extern float screenScale;
static void releasePixmap(void *info, const void *data, size_t size)
{
	fz_drop_pixmap(ctx, info);
}


@implementation MuPageViewer
- (id) initWithFrame:(CGRect)frame document:(fz_document *)aDoc page:(int)aNumber
{
    self = [super initWithFrame:frame];
    if(self)
    {
        doc = aDoc;
        number = aNumber;
        [self setDelegate:self];

        /*
         wait view
         */
        loadingView = [[UIActivityIndicatorView alloc]initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
        [loadingView setFrame:CGRectMake(100, 100, 100, 100)];
        [loadingView startAnimating];
        [self addSubview:loadingView];
        
        /*
         load the pdf page to display
         */
       [self loadPage];
        /*
         add the zoom gesture
         */
//    
//        UILongPressGestureRecognizer *longPress = [[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(longPressAction:)];
        UIPinchGestureRecognizer *touch = [[UIPinchGestureRecognizer alloc]initWithTarget:self action:@selector(doubleTap:)];
        touch.scale = 2.0;
        //touch.velocity = 
        [touch setDelegate:self];
       // [self addGestureRecognizer:touch]; 
      //  [self addGestureRecognizer:longPress];
        [touch release];
    }
    return self;
}
- (void)dealloc
{
    [loadingView release];
    [imageView release];
    [super dealloc];
}
- (void)loadPage
{
    if(number<0||number>=fz_count_pages(doc))
        return;
    dispatch_async(queue, ^{
        if(!page)
         page = fz_load_page(doc,number);
        CGSize psize = [self pageSize:doc page:page];
        UIImage *image = [self renderPage:doc page:page size:self.bounds.size];
        dispatch_async(dispatch_get_main_queue(), ^{
            pageSize = psize;
            [self displayImage:image];
            [image release];
        });        
    });
   
    
}
- (void)displayImage:(UIImage *)image
{
    if(loadingView)
    {
        [loadingView removeFromSuperview];
        [loadingView release];
        loadingView = nil;
    }
    if(!imageView)
    {
        imageView = [[UIImageView alloc]initWithImage:image];
        [imageView setOpaque:NO];
        [self addSubview:imageView];
    }
    else {
        [imageView setImage:image];
    }
   // [imageView sizeToFit];
[self setMinimumZoomScale: 1];
[self setMaximumZoomScale: 5];
//[self setZoomScale: 1 animated: YES];
    [self resizeImage];
}
- (void)resizeImage
{
    if (imageView) {
		CGSize imageSize = imageView.image.size;
		CGSize scale = [self fitPagetoScreen:imageSize screen:self.bounds.size];
		if (fabs(scale.width - 1) > 0.1) {
			CGRect frame = [imageView frame];
			frame.size.width = imageSize.width * scale.width;
			frame.size.height = imageSize.height * scale.height;
			[imageView setFrame: frame];
            
			printf("resized view; queuing up a reload (%d)\n", number);
			dispatch_async(queue, ^{
				dispatch_async(dispatch_get_main_queue(), ^{
					CGSize scale = [self fitPagetoScreen:imageView.image.size screen:self.bounds.size];
					if (fabs(scale.width - 1) > 0.01)
						[self loadPage];
				});
			});
		} else {
			[imageView sizeToFit];
		}
        
		[self setContentSize: imageView.frame.size];
        
		//[self layoutIfNeeded];
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
- (void) showSearchResults:(int)count
{
    if(hitView)
    {
        [hitView release];
        hitView = nil;
    }
   {
        hitView = [[MuHitViewer alloc]initWithResults:count forDocument:doc];
       if(imageView)
       {
           //[imageView addSubview:hitView];
           [hitView setFrame:imageView.frame];
           [hitView setPagsize:pageSize];
       }
        [self addSubview:hitView];
    }
}
- (int)number
{
    return number;
}
#pragma mark - scrollview delegate function
- (UIView *)viewForZoomingInScrollView:(UIScrollView *)scrollView
{
    return imageView;
}
- (void)scrollViewWillBeginZooming:(UIScrollView *)scrollView withView:(UIView *)view 
{
    
}
- (void)scrollViewDidEndZooming:(UIScrollView *)scrollView withView:(UIView *)view atScale:(float)scale
{
    //[self setZoomScale:scale];
}
- (void) doubleTap:(UIGestureRecognizer *)sender
{
//    CGFloat newZoom = self.zoomScale*1.5f;
//    newZoom = MIN(newZoom, self.maximumZoomScale);
//    [self setZoomScale:newZoom];
   // UIPickerView *pick = [[UIPickerView alloc]initWithFrame:];
   
   // [self setZoomScale:((UIPinchGestureRecognizer*)sender).velocity];
}
//- (void)longPressAction:(id)sender
//{
//    CGPoint point = [sender locationInView:imageView];
//    MuPopView *m = [[MuPopView alloc]initWithFrame:CGRectMake(point.x , point.y, 150, 50)];
//    [m addButtonWithTitle:[NSArray arrayWithObjects:@"Music",@"Vdeio", nil]];
//    [self addSubview:m];
//}
#pragma mark - assist functions

- (CGSize)pageSize:(fz_document *)Doc page:(fz_page *)Page
{
    CGSize pagesize;
	fz_rect bounds;
	fz_bound_page(doc, page, &bounds);
	pagesize.width = bounds.x1 - bounds.x0;
	pagesize.height = bounds.y1 - bounds.y0;
	return pagesize;
}
- (CGSize)fitPagetoScreen:(CGSize)Page screen:(CGSize)screen
{
    float hscale = screen.width / Page.width;
	float vscale = screen.height / Page.height;
	float scale = fz_min(hscale, vscale);
	hscale = floorf(Page.width * scale) / Page.width;
	vscale = floorf(Page.height * scale) / Page.height;
	return CGSizeMake(hscale, vscale);
}
- (UIImage*)newImageWithPix:(fz_pixmap *)pix
{
    unsigned char *samples = fz_pixmap_samples(ctx, pix);
	int w = fz_pixmap_width(ctx, pix);
	int h = fz_pixmap_height(ctx, pix);
    CGDataProviderRef dataProvider = CGDataProviderCreateWithData(pix, samples, w*4*h, releasePixmap);
    CGColorSpaceRef color = CGColorSpaceCreateDeviceRGB();
    CGImageRef cgimage = CGImageCreate(w, h, 8, 32, w*4, color, kCGBitmapByteOrderDefault, dataProvider, NULL, NO, kCGRenderingIntentDefault);
    UIImage *image = [[UIImage alloc]initWithCGImage:cgimage scale:screenScale orientation:UIImageOrientationUp];
    CGDataProviderRelease(dataProvider);
    CGColorSpaceRelease(color);
    CGImageRelease(cgimage);
    
    return image;
}
- (UIImage *)renderPage:(fz_document *)Doc page:(fz_page *)Page size:(CGSize)screenSize
{
    CGSize pagesize;
	fz_irect bbox;
	fz_matrix ctm;
	fz_device *dev;
	fz_pixmap *pix;
	CGSize scale;
    
	screenSize.width *= screenScale;
	screenSize.height *= screenScale;
    
	pagesize = [self pageSize:Doc page:Page];
	scale = [self fitPagetoScreen:pagesize screen:screenSize];//fitPageToScreen(pagesize, screenSize);
	fz_scale(&ctm, scale.width, scale.height);
	bbox = (fz_irect){0, 0, pagesize.width * scale.width, pagesize.height * scale.height};
    
	pix = fz_new_pixmap_with_bbox(ctx, fz_device_rgb(ctx), &bbox);
	fz_clear_pixmap_with_value(ctx, pix, 255);
    
	dev = fz_new_draw_device(ctx, pix);
	fz_run_page(Doc, Page, dev, &ctm, NULL);
	fz_free_device(dev);
   // logMemUsage();
	return [self newImageWithPix:pix];
}


@end
