//
//  FirstViewController.h
//  Melody S
//
//  Copyright (c) 2013 Cambridge Executive Limited (BlueCreation). All rights reserved.
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// This code can only be used to connect to a Product containing Melody. This can be Melody
// software or a BlueCreation module.

#import <UIKit/UIKit.h>
#import "MelodyManager.h"
#import "MelodySmart.h"
#import "UAModalPanel.h"

@interface DataViewController : UIViewController <MelodySmartDelegate, MelodyManagerDelegate,
UITextFieldDelegate>
@property (strong, nonatomic) MelodySmart *melody;
@end
