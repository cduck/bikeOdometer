//
//  UIImageView+Border.h
//  MelodySmart
//
//  Copyright (c) 2013 Cambridge Executive Limited (BlueCreation). All rights reserved.
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// This code can only be used to connect to a Product containing Melody. This can be Melody
// software or a BlueCreation module.

#import <Foundation/Foundation.h>

@interface UIImageView (ImageViewBorder)

-(void)setImage:(UIImage*)image withBorderWidth:(CGFloat)borderWidth;

-(void)configureImageViewBorder:(CGFloat)borderWidth;

@end

