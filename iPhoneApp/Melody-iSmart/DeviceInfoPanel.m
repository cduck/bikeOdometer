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

#import "DeviceInfoPanel.h"
#define BLACK_BAR_COMPONENTS				{ 0.22, 0.22, 0.22, 1.0, 0.07, 0.07, 0.07, 1.0 }

@implementation DeviceInfoPanel
@synthesize system_id, model_number,serial_number,hw_rev,sw_rev,fw_rev,manufacturer_name;

- (id)initWithFrame:(CGRect)frame title:(NSString *)title withSystemID:(NSString*)systemID withModelNo:(NSString*)ModelNo withSerialNo:(NSString*)SerialNo withHwRev:(NSString*)HwRev withFwRev:(NSString*)FwRev withSwRev:(NSString*)SwRev withManufName:(NSString*)ManufName {
    
    nibObjects = [[NSBundle mainBundle] loadNibNamed:@"DeviceInfoView" owner:self options:nil];
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
    DeviceInfoPanel* info = (DeviceInfoPanel*)v;
    info.system_id.text = systemID;
    info.model_number.text = ModelNo;
    info.serial_number.text = SerialNo;
    info.hw_rev.text = HwRev;
    info.sw_rev.text = SwRev;
    info.fw_rev.text = FwRev;
    info.manufacturer_name.text = ManufName;
	return self;
}

- (void)dealloc {
}

- (void)layoutSubviews {
	[super layoutSubviews];
    [v setFrame:self.contentView.bounds];
}
@end
