//
//  DiscoverDevices.m
//  Melody-iSmart
//
//  Copyright (c) 2014 BlueCreation. All rights reserved.
//

#import "DiscoverDevices.h"
#import "DataViewController.h"
#import "AppDelegate.h"
#import "MMSideDrawerSectionHeaderView.h"
#import "MMDrawerController.h"

@interface DiscoverDevices () {
    MelodySmart     *melody;
}
@property (strong, nonatomic) IBOutlet UITableView *foundDevices;

@end

@implementation DiscoverDevices

@synthesize foundDevices;

-(void) viewDidLoad {
}

-(void) viewWillAppear:(BOOL)animated {
    UIAppDelegate.manager.delegate = self;
    [UIAppDelegate.manager scanForMelody];
    [foundDevices reloadData];
}

-(void) viewWillDisappear:(BOOL)animated {
    [UIAppDelegate.manager stopScanning];
    [[NSNotificationCenter defaultCenter] postNotificationName:@"MelodyDeviceUpdateNotification" object: nil];
}

#pragma mark - TableView
-(UIView*)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section{
    MMSideDrawerSectionHeaderView * headerView =  [[MMSideDrawerSectionHeaderView alloc] initWithFrame:CGRectMake(0, 0, CGRectGetWidth(tableView.bounds), 20.0f)];
    [headerView setAutoresizingMask:UIViewAutoresizingFlexibleHeight|UIViewAutoresizingFlexibleWidth];
    //[headerView setTitle:[tableView.dataSource tableView:tableView titleForHeaderInSection:section]];
    [headerView setTitle:@"Found Devices"];
    return headerView;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    NSInteger	res = 0;
    if (section == 0) {
        res =  [MelodyManager numberOfFoundDevices];
    }
    
	return res;
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell	*cell;
	MelodySmart     *d;
	NSInteger		row	= [indexPath row];
    static NSString *cellID = @"DeviceList";
    
	cell = [tableView dequeueReusableCellWithIdentifier:cellID];
	if (!cell)
		cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:cellID];
    
    d = (MelodySmart*)[MelodyManager foundDeviceAtIndex:row];
    cell.selectionStyle = UITableViewCellSelectionStyleBlue;
    cell.userInteractionEnabled = YES;
    cell.textLabel.textColor = [UIColor blackColor];
    
    if ([[d name] length])
        [[cell textLabel] setText:[d name]];
    else
        [[cell textLabel] setText:@"Peripheral"];
    
    [[cell detailTextLabel] setText: [NSString stringWithFormat:@"%d", [d RSSI].intValue]];
    
	return cell;
}

- (void) tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	NSInteger		row	= [indexPath row];
    NSUInteger      section = indexPath.section;
	
    if (section == 0) {
        melody = [MelodyManager foundDeviceAtIndex:row];
        melody.delegate = self;
        [melody connect];
    }
}

#pragma mark ------------------------
#pragma mark MelodyManagerDelegate
- (void) melodyManagerDiscoveryDidRefresh:(MelodyManager*)manager
{
    [foundDevices reloadData];
}

- (void) melodyManagerDiscoveryStatePoweredOff:(MelodyManager*)manager
{
    NSString *title     = @"Bluetooth Power";
    NSString *message   = @"You must turn on Bluetooth in Settings in order to use LE";
    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:title message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alertView show];
}


- (void) melodySmart:(MelodySmart*)m didConnectToMelody:(BOOL) result {
    if (m == melody && !result) {
        melody = nil;
    }
}

-(void) melodySmartDidPopulateMelodyService:(MelodySmart*)m {
    if (m == melody) {
        UIAppDelegate.melody = melody;
        [[NSNotificationCenter defaultCenter] postNotificationName:@"MelodyDeviceUpdateNotification" object: nil];
        MMDrawerController * drawerController = (MMDrawerController *)UIAppDelegate.window.rootViewController;
        [drawerController closeDrawerAnimated:YES completion:nil];
    }
}

- (void) melodySmartDidDisconnectFromMelody:(MelodySmart*) melody {
    
}

- (void) melodySmart:(MelodySmart*)m didReceiveData:(NSData*)data {
}

- (void) melodySmart:(MelodySmart*)m didReceivePioChange:(unsigned char)state WithLocation:(BcSmartPioLocation)location {
}

- (void) melodySmart:(MelodySmart*)m didReceivePioSettingChange:(unsigned char)state WithLocation:(BcSmartPioLocation)location {
}

@end
