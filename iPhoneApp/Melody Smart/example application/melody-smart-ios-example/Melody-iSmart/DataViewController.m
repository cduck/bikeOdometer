//
//  FirstViewController.m
//  Melody S
//
//  Copyright (c) 2013 Cambridge Executive Limited (BlueCreation). All rights reserved.
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// This code can only be used to connect to a Product containing Melody. This can be Melody
// software or a BlueCreation module.

#import "UIViewController+MMDrawerController.h"

#import "DataViewController.h"

#import "AppDelegate.h"
#import "AboutPanel.h"
#import "DeviceInfoPanel.h"

@interface DataViewController () {
}

@property (weak, nonatomic) IBOutlet UILabel *lblConnection;
@property (weak, nonatomic) IBOutlet UILabel *lblDeviceName;
@property (weak, nonatomic) IBOutlet UITextField *tfReceivedData;
@property (weak, nonatomic) IBOutlet UITextField *tfDataSend;

@end

@implementation DataViewController

@synthesize lblConnection, lblDeviceName;
@synthesize tfReceivedData, tfDataSend;

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)viewDidAppear:(BOOL)animated {
    
    // register for keyboard notifications
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(keyboardWillShow)
                                                 name:UIKeyboardWillShowNotification
                                               object:nil];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(keyboardWillHide)
                                                 name:UIKeyboardWillHideNotification
                                               object:nil];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(melodyDeviceUpdated)
                                                 name:@"MelodyDeviceUpdateNotification"
                                               object: nil];
}

- (void) viewWillAppear:(BOOL)animated {
    if (UIAppDelegate.melody) {
        self.melody = UIAppDelegate.melody;
        self.melody.delegate = self;
    }
    
    if (self.melody && self.melody.isConnected) {
        lblConnection.text = @"Connected";
        lblDeviceName.text = self.melody.name;
        //[self.melody setDataNotification:YES];
    } else {
        lblConnection.text = @"Not Connected";
        lblDeviceName.text = @"";
    }
}

- (void) viewWillDisappear:(BOOL)animated {
    if (self.melody) {
        [self.melody disconnect];
        UIAppDelegate.melody = self.melody;
    }
    
    // unregister for keyboard notifications while not visible.
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:UIKeyboardWillShowNotification
                                                  object:nil];
    
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:UIKeyboardWillHideNotification
                                                  object:nil];
    
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                 name:@"MelodyDeviceUpdateNotification"
                                               object: nil];
}

- (IBAction)About:(id)sender {    
    AboutPanel *about = [[AboutPanel alloc] initWithFrame:self.view.bounds title:@"About"];
    
    [self.view addSubview:about];
	[about showFromPoint:self.view.center];
}

- (IBAction)OnDeviceInfo:(id)sender {
    if (self.melody != nil && self.melody.isConnected) {
        DeviceInfoPanel *devInfo = [[DeviceInfoPanel alloc] initWithFrame:self.view.bounds title:@"Device Info" withSystemID:self.melody.deviceInfoSystemId  withModelNo:self.melody.deviceInfoModelNumber withSerialNo:self.melody.deviceInfoSerialNumber withHwRev:self.melody.deviceInfoHardwareRevision withFwRev:self.melody.deviceInfoFirmwareRevision withSwRev:self.melody.deviceInfoSoftwareRevisin withManufName:self.melody.deviceInfoManufacturerName];

        [self.view addSubview:devInfo];
        [devInfo showFromPoint:self.view.center];
    }
}


- (IBAction)SendData:(id)sender {
    NSData* data = [tfDataSend.text dataUsingEncoding:NSUTF8StringEncoding];
    if (data)
        [self.melody sendData:data];
}
- (IBAction)DisconnectDevice:(id)sender {
    if (self.melody && self.melody.isConnected) {
        [self.melody disconnect];
    }
}

-(void) melodyDeviceUpdated {
    UIAppDelegate.manager.delegate = self;
    
    [self.melody setDataNotification:NO];
    self.melody.delegate = nil;
    if (UIAppDelegate.melody) {
        self.melody = UIAppDelegate.melody;
        self.melody.delegate = self;
    }
    
    if (self.melody && self.melody.isConnected) {
        lblConnection.text = @"Connected";
        lblDeviceName.text = self.melody.name;
        [self.melody setDataNotification:YES];
    } else {
        lblConnection.text = @"Not Connected";
        lblDeviceName.text = @"";
    }
}

#pragma mark -------------------------------------
#pragma mark - UAModalDisplayPanelViewDelegate
- (void)didShowModalPanel:(UAModalPanel *)modalPanel {
}

- (void)didCloseModalPanel:(UAModalPanel *)modalPanel {
}

#pragma mark -------------------------------------
#pragma mark - Melody Delegates
- (void) melodySmartDidPopulateMelodyService:(MelodySmart *)m { }
- (void) melodyManagerDiscoveryDidRefresh:(MelodyManager*)manager { }
- (void) melodyManagerDiscoveryStatePoweredOff:(MelodyManager*)manager { }

- (void) melodySmart:(MelodySmart*)melody didConnectToMelody:(BOOL)result {
}

- (void) melodySmartDidDisconnectFromMelody:(MelodySmart*) melody {
    lblConnection.text = @"Not Connected";
    lblDeviceName.text = @"";
    self.melody = nil;
}

- (void) melodySmart:(MelodySmart*)melody didReceiveData:(NSData*)data {
#ifndef RAW_DATA
        NSString* str = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        tfReceivedData.text = str;
#else
        tfReceivedData.text = [[NSString alloc] initWithFormat:@"%@", data];
#endif
}

- (void) melodySmart:(MelodySmart*)melody didRecveivePioChange:(unsigned char)state WithLocation:(BcSmartPioLocation)location { }
- (void) melodySmart:(MelodySmart*)melody didRecveivePioSettingChange:(unsigned char)state WithLocation:(BcSmartPioLocation)location { }

- (void) melodySmart:(MelodySmart*)melody didSendData:(NSError*)error {
    NSLog(@"returned with error %@",error);
}

#pragma mark -------------------------------------------------
#pragma mark keyboard
-(void)keyboardWillShow {
    // Animate the current view out of the way
    if (self.view.frame.origin.y >= 0) {
        [self setViewMovedUp:YES];
    } else if (self.view.frame.origin.y < 0) {
        [self setViewMovedUp:NO];
    }
    
    tfDataSend.delegate = self;
}

-(void)keyboardWillHide {
    if (self.view.frame.origin.y >= 0) {
        [self setViewMovedUp:YES];
    } else if (self.view.frame.origin.y < 0) {
        [self setViewMovedUp:NO];
    }
    
    tfDataSend.delegate = nil;
}

- (BOOL)textFieldShouldBeginEditing:(UITextField *)textField {
    if (textField == tfReceivedData)
        return NO;
    
    return YES;
}

-(void)textFieldDidBeginEditing:(UITextField *)sender
{
    if ([sender isEqual:tfDataSend])
    {
        //move the main view, so that the keyboard does not hide it.
        if  (self.view.frame.origin.y >= 0)
        {
            [self setViewMovedUp:YES];
        }
    }
}
#define kOFFSET_FOR_KEYBOARD 80.0
//method to move the view up/down whenever the keyboard is shown/dismissed
-(void)setViewMovedUp:(BOOL)movedUp
{
    [UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDuration:0.3]; // if you want to slide up the view
    
    CGRect rect = self.view.frame;
    if (movedUp)
    {
        // 1. move the view's origin up so that the text field that will be hidden come above the keyboard
        // 2. increase the size of the view so that the area behind the keyboard is covered up.
        rect.origin.y -= kOFFSET_FOR_KEYBOARD;
        rect.size.height += kOFFSET_FOR_KEYBOARD;
    }
    else
    {
        // revert back to the normal state.
        rect.origin.y += kOFFSET_FOR_KEYBOARD;
        rect.size.height -= kOFFSET_FOR_KEYBOARD;
    }
    self.view.frame = rect;
    
    [UIView commitAnimations];
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    NSData* data = [tfDataSend.text dataUsingEncoding:NSUTF8StringEncoding];
    [self.melody sendData:data];
    
    [self.tfDataSend resignFirstResponder];
    return YES;
}

@end
