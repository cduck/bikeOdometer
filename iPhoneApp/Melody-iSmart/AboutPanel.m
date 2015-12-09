//
//  AboutPanel.m
//  Melody S
//
//  Copyright (c) 2013 Cambridge Executive Limited (BlueCreation). All rights reserved.
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// This code can only be used to connect to a Product containing Melody. This can be Melody
// software or a BlueCreation module.

#import "AboutPanel.h"
#define BLACK_BAR_COMPONENTS				{ 0.22, 0.22, 0.22, 1.0, 0.07, 0.07, 0.07, 1.0 }

@implementation AboutPanel

- (id)initWithFrame:(CGRect)frame title:(NSString *)title {
    NSArray *nibObjects = [[NSBundle mainBundle] loadNibNamed:@"AboutView" owner:self options:nil];
    v = [nibObjects objectAtIndex:0];
    
    frame.size.height -= 50;
    frame.size.width -= 10;
    frame.origin.y += 20;
	if ((self = [super initWithFrame:frame])) {
		CGFloat colors[8] = BLACK_BAR_COMPONENTS;
		[self.titleBar setColorComponents:colors];
		self.headerLabel.text = title;
		self.padding = UIEdgeInsetsMake(0.0f, 0.0f, 0.0f, 0.0f);
        [self.contentView addSubview:v];
	}
	return self;
}

- (void)dealloc {
}

- (void)layoutSubviews {
	[super layoutSubviews];
    [v setFrame:self.contentView.bounds];
}
@end
