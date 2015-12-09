//
//  ConnectedDevices.m
//  Melody-iSmart
//
//  Copyright (c) 2014 BlueCreation. All rights reserved.
//

#import "ConnectedDevices.h"
#import "DataViewController.h"
#import "AppDelegate.h"
#import "MMSideDrawerSectionHeaderView.h"
#import "MMDrawerController.h"

@interface ConnectedDevices () {
    MelodySmart     *melody;
}
@property (strong, nonatomic) IBOutlet UITableView  *deviceTable;

@end

@implementation ConnectedDevices

@synthesize deviceTable;

-(void) viewDidLoad {
}

-(void) viewWillAppear:(BOOL)animated {
    [deviceTable reloadData];
}

-(void) viewWillDisappear:(BOOL)animated {
    [[NSNotificationCenter defaultCenter] postNotificationName:@"MelodyDeviceUpdateNotification" object: nil];
}

#pragma mark - TableView
-(UIView*)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section{
    MMSideDrawerSectionHeaderView * headerView =  [[MMSideDrawerSectionHeaderView alloc] initWithFrame:CGRectMake(0, 0, CGRectGetWidth(tableView.bounds), 20.0f)];
    [headerView setAutoresizingMask:UIViewAutoresizingFlexibleHeight|UIViewAutoresizingFlexibleWidth];
    //[headerView setTitle:[tableView.dataSource tableView:tableView titleForHeaderInSection:section]];
    [headerView setTitle:@"Connected Devices"];
    return headerView;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    NSInteger	res = 0;
    if (section == 0) {
        res =  [MelodyManager numberOfConnectedDevices];
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
    
    d = (MelodySmart*)[MelodyManager connectedDeviceAtIndex:row];
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
        melody = [MelodyManager connectedDeviceAtIndex:row];
        UIAppDelegate.melody = melody;
        [[NSNotificationCenter defaultCenter] postNotificationName:@"MelodyDeviceUpdateNotification" object: nil];
        MMDrawerController * drawerController = (MMDrawerController *)UIAppDelegate.window.
        rootViewController;
        [drawerController closeDrawerAnimated:YES completion:nil];
    }
}

#pragma mark ------------------------
#pragma mark MelodyManagerDelegate
- (void) melodyManagerDiscoveryDidRefresh:(MelodyManager*)manager
{
    [deviceTable reloadData];
}

- (void) melodyManagerDiscoveryStatePoweredOff:(MelodyManager*)manager
{
    NSString *title     = @"Bluetooth Power";
    NSString *message   = @"You must turn on Bluetooth in Settings in order to use LE";
    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:title message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alertView show];
}

#pragma mark ------------------------
#pragma mark MelodySmartDelegate

- (void) melodySmart:(MelodySmart*)melody didReceiveData:(NSData*)data {
}

-(void) melodySmartDidPopulateMelodyService:(MelodySmart*)melody {
}

- (void) melodySmart:(MelodySmart*)melody didConnectToMelody:(BOOL)result {
}

- (void) melodySmartDidDisconnectFromMelody:(MelodySmart*) melody {
    
}


- (void) melodySmart:(MelodySmart*)melody didReceivePioChange:(unsigned char)state WithLocation:(BcSmartPioLocation)location {
}

- (void) melodySmart:(MelodySmart*)melody didReceivePioSettingChange:(unsigned char)state WithLocation:(BcSmartPioLocation)location {
}

@end
