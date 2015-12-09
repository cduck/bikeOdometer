//
//  AboutPanel.h
//  Melody S
//
//  Copyright (c) 2013 Cambridge Executive Limited (BlueCreation). All rights reserved.
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// This code can only be used to connect to a Product containing Melody. This can be Melody
// software or a BlueCreation module.

#import <Foundation/Foundation.h>
#import "UATitledModalPanel.h"
#import "MelodySmart.h"

@interface AboutPanel : UATitledModalPanel  {
    UIView          *v;
}

- (id)initWithFrame:(CGRect)frame title:(NSString *)title;

@end