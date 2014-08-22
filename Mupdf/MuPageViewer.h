//
//  MuPageViewer.h
//  MuPDF
//
//  Created by Ｙuancheng SU on 14-6-25.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import <UIKit/UIKit.h>
#include"mupdf/pdf.h"
#import "MuHitView.h"
#import "MuPopView.h"
@interface MuPageViewer : UIScrollView<UISearchBarDelegate,UIScrollViewDelegate,UIGestureRecognizerDelegate>
{
	fz_document *doc;
	fz_page *page;
	int number;
	UIActivityIndicatorView *loadingView;
	UIImageView *imageView;
	UIImageView *tileView;
	MuHitViewer *hitView;
	//MuHitView *linkView;
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
- (void) doubleTap:(UIGestureRecognizer*)sender;

- (UIImage *)newImageWithPix:(fz_pixmap*)pixmap;
- (CGSize) pageSize:(fz_document*)doc page:(fz_page*)page;
- (CGSize) fitPagetoScreen:(CGSize)Page screen:(CGSize)screen;
- (UIImage *)renderPage:(fz_document *)Doc page:(fz_page *)Page size:(CGSize)pagesize;
@end

