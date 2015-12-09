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

@interface DeviceInfoPanel : UATitledModalPanel  {
    UIView          *v;
    NSArray         *nibObjects;
}
@property (weak, nonatomic) IBOutlet UILabel *system_id;
@property (weak, nonatomic) IBOutlet UILabel *model_number;
@property (weak, nonatomic) IBOutlet UILabel *serial_number;
@property (weak, nonatomic) IBOutlet UILabel *hw_rev;
@property (weak, nonatomic) IBOutlet UILabel *fw_rev;
@property (weak, nonatomic) IBOutlet UILabel *sw_rev;
@property (weak, nonatomic) IBOutlet UILabel *manufacturer_name;

- (id)initWithFrame:(CGRect)frame title:(NSString *)title withSystemID:(NSString*)systemID withModelNo:(NSString*)ModelNo withSerialNo:(NSString*)SerialNo withHwRev:(NSString*)HwRev withFwRev:(NSString*)FwRev withSwRev:(NSString*)SwRev withManufName:(NSString*)ManufName;

- (void) setSystemID:(NSString*)systemID withModelNo:(NSString*)ModelNo withSerialNo:(NSString*)SerialNo withHwRev:(NSString*)HwRev withFwRev:(NSString*)FwRev withSwRev:(NSString*)SwRev withManufName:(NSString*)ManufName;

@end