//
//  MuAlertView.m
//  MuPDF
//
//  Created by Ｙuancheng SU on 14-7-16.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#import "MuAlertView.h"

@implementation MuAlertView
@synthesize MyDelegate;
@synthesize ownPWD;
@synthesize userPWD;
- (id)initWithStyle:(int)style
{ 
    NSString *msg;
    if(0 == style)
        msg = @"添加密码";
    if(1 == style)
        msg = @"修改权限";
    self  = [super initWithTitle:@"Password Handle" message:msg delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"OK", nil];
    if(self )
    {
        [self setAlertViewStyle:UIAlertViewStyleLoginAndPasswordInput];
        [[self textFieldAtIndex:0]setPlaceholder:@"owner password"];
        [[self textFieldAtIndex:1]setPlaceholder:@"user password"];
        [[self textFieldAtIndex:1]setSecureTextEntry:NO];
        [self setDelegate:self];        
        _style =style;
        
    }
    return self;
}
- (void)layoutSubviews
{
}

- (void)show{
    [super show];

}
- (void)dealloc
{
    [super dealloc];
}
#pragma mark - UITextFieldDelegate
- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    [textField resignFirstResponder];
    return YES;
}
#pragma mark - action
- (void)cancel
{
    [self dismissWithClickedButtonIndex:0 animated:YES];
}
- (void)Okal:(id)sender
{
    [self dismissWithClickedButtonIndex:0 animated:YES];
    if(_style == FORADDPASSWORD)
    [self.MyDelegate OKforAddPassword:self];
    else if(_style == FORPERMISSIONCONTROL)
        [self.MyDelegate OKforPermissionChange:self];
}
#pragma mark - uialterview delegate
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
	
	[alertView dismissWithClickedButtonIndex: buttonIndex animated: TRUE];
	if (buttonIndex == 1) {
        self.ownPWD = [[alertView textFieldAtIndex: 0] text] ;
        self.userPWD = [[alertView textFieldAtIndex: 1] text];
        
        if(_style == FORADDPASSWORD)
            [self.MyDelegate OKforAddPassword:self];
        else if(_style == FORPERMISSIONCONTROL)
            [self.MyDelegate OKforPermissionChange:self];
        
	} else {
		//[self onPasswordCancel];
	}
}
@end
