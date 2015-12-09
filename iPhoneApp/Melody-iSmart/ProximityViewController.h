//
//  ProximityViewController.h
//  Melody-iSmart
//
//  Created by Stanislav Nikolov on 31/01/2014.
//  Copyright (c) 2014 BlueCreation. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BcProximityManager.h"


@interface ProximityViewController : UIViewController <MelodySmartDelegate, BcProximityDelegate>
@property (strong, nonatomic) MelodySmart *melody;
@end
