//
//  BeaconViewController.m
//  Melody-iSmart
//
//  Created by Stanislav Nikolov on 19/02/2014.
//  Copyright (c) 2014 BlueCreation. All rights reserved.
//

#import "BeaconViewController.h"
#import "BcBeaconManager.h"

@interface BeaconViewController () <BcBeaconManagerDelegate> {
    NSArray *beaconArray;
}

@end

@implementation BeaconViewController

- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    [BcBeaconManager sharedInstance].delegate = self;
    
    self.title = @"BcBeacon Demo";
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


- (void)bcObservedBeaconsDidChange:(NSArray *)beacons {
    beaconArray = beacons;
    
    [self.tableView reloadData];
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

-(NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
    return @"Current BcBeacons";
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return beaconArray.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"Cell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier forIndexPath:indexPath];
    
    BcBeacon *beacon = beaconArray[indexPath.row];
    
    NSString *proximity = nil;
    switch (beacon.proximityClass) {
        case BcProximityClassUnknown: proximity = @"Unknown";
            break;
        case BcProximityClassImmediate: proximity = @"Immediate";
            break;
        case BcProximityClassNear: proximity = @"Near";
            break;
        case BcProximityClassFar: proximity = @"Far";
            break;
    }
    
    cell.textLabel.text = [NSString stringWithFormat:@"Major: %d, Minor: %d", beacon.majorNumber, beacon.minorNumber];
    cell.detailTextLabel.text = [NSString stringWithFormat:@"Prox: %@, Distance: %.2fm", proximity, beacon.approximateDistance];
    
    return cell;
}

@end
