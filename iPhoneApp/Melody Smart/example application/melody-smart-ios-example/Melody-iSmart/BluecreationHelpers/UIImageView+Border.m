//
//  UIImageView+Border.m
//  MelodySmart
//
//  Copyright (c) 2013 Cambridge Executive Limited (BlueCreation). All rights reserved.
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// This code can only be used to connect to a Product containing Melody. This can be Melody
// software or a BlueCreation module.

#import "UIImageView+Border.h"
#import "QuartzCore/QuartzCore.h"

@interface UIImageView (private)
-(UIImage*)rescaleImage:(UIImage*)image;
@end

@implementation UIImageView (ImageViewBorder)

-(void)setImage:(UIImage*)image withBorderWidth:(CGFloat)borderWidth
{
    [self configureImageViewBorder:borderWidth];
    UIImage* scaledImage = [self rescaleImage:image];
    self.image = scaledImage;
}

-(UIImage*)rescaleImage:(UIImage*)image{
    UIImage* scaledImage = image;
    
    CALayer* layer = self.layer;
    CGFloat borderWidth = layer.borderWidth;
    
    //if border is defined
    if (borderWidth > 0)
    {
        //rectangle in which we want to draw the image.
        CGRect imageRect = CGRectMake(0.0, 0.0, self.bounds.size.width - 2 * borderWidth,self.bounds.size.height - 2 * borderWidth);
        //Only draw image if its size is bigger than the image rect size.
        if (image.size.width > imageRect.size.width || image.size.height > imageRect.size.height)
        {
            UIGraphicsBeginImageContext(imageRect.size);
            [image drawInRect:imageRect];
            scaledImage = UIGraphicsGetImageFromCurrentImageContext();
            UIGraphicsEndImageContext();
        }
    }
    return scaledImage;
}


-(void)configureImageViewBorder:(CGFloat)borderWidth{
    CALayer* layer = [self layer];
    [layer setBorderWidth:borderWidth];
    [self setContentMode:UIViewContentModeCenter];
    //[layer setBorderColor:[UIColor whiteColor].CGColor];
    //[layer setShadowOffset:CGSizeMake(0.0, 0.0)];
    //[layer setShadowRadius:0.0];
    //[layer setShadowOpacity:0.0];
}

@end
