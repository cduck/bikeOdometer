//
//  ProximityViewController.m
//  Melody-iSmart
//
//  Created by Stanislav Nikolov on 31/01/2014.
//  Copyright (c) 2014 BlueCreation. All rights reserved.
//

#import "ProximityViewController.h"
#import "UIViewController+MMDrawerController.h"
#import "MelodySmart.h"

@interface ProximityViewController ()

@end

@implementation ProximityViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    [[BcProximityManager sharedInstance] startProximityDetection];
    [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
    
    self.mm_drawerController.rightDrawerViewController = nil;
    self.mm_drawerController.leftDrawerViewController = nil;
}

- (void)viewDidDisappear:(BOOL)animated {
    [[BcProximityManager sharedInstance] stopProximityDetection];
    [[UIApplication sharedApplication] setIdleTimerDisabled:NO];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


- (void) bcProximityDidConnect:(MelodySmart*)m {
    _melody = m;
    _melody.delegate = self;
}

- (void) melodySmart:(MelodySmart*)melody didConnectToMelody:(BOOL)result {
    
}

- (void) melodySmartDidDisconnectFromMelody:(MelodySmart*)melody {
    
}
-(void) melodySmartDidPopulateMelodyService:(MelodySmart*)melody {
    
}
- (void) melodySmart:(MelodySmart*)melody didSendData:(NSError*)error {
    
}
- (void) melodySmart:(MelodySmart*)melody didReceiveData:(NSData*)data {
    
}
- (void) meldodySmart:(MelodySmart*)melody didReceiveLinkLossLevel:(BcSmartAlertLevel)level {
    
}

- (void) meldodySmart:(MelodySmart*)melody didReceiveTxPower:(NSInteger)power {
    
}

@end
