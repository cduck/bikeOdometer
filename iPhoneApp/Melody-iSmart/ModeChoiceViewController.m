//
//  ModeChoiceViewController.m
//  Melody-iSmart
//
//  Created by Stanislav Nikolov on 31/01/2014.
//  Copyright (c) 2014 BlueCreation. All rights reserved.
//

#import "UIViewController+MMDrawerController.h"

#import "ModeChoiceViewController.h"

@interface ModeChoiceViewController ()

@end

@implementation ModeChoiceViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)btnSmartDemo_Touched:(id)sender {
    //UINavigationController *nav = [[UINavigationController alloc] initWithRootViewController:[self.storyboard instantiateViewControllerWithIdentifier:@"Data"]];
    
    //[self.mm_drawerController setCenterViewController:nav withCloseAnimation:YES completion:nil];
    
    /*UIStoryboard *storyboard = [UIStoryboard storyboardWithName:@"MainStoryboard" bundle:nil];
    UIViewController *leftSideDrawerViewController = [storyboard instantiateViewControllerWithIdentifier:@"Connected"];
    
    UIViewController *centerViewController = [storyboard instantiateViewControllerWithIdentifier:@"Data"];
    
    UIViewController *rightSideDrawerViewController = [storyboard instantiateViewControllerWithIdentifier:@"Discover"];
    
    
    UINavigationController * navigationController = [[UINavigationController alloc] initWithRootViewController:centerViewController];
    [navigationController setRestorationIdentifier:@"MMExampleCenterNavigationControllerRestorationKey"];

        UINavigationController * rightSideNavController = [[UINavigationController alloc] initWithRootViewController:rightSideDrawerViewController];
		[rightSideNavController setRestorationIdentifier:@"MMExampleRightNavigationControllerRestorationKey"];
        UINavigationController * leftSideNavController = [[UINavigationController alloc] initWithRootViewController:leftSideDrawerViewController];
		[leftSideNavController setRestorationIdentifier:@"MMExampleLeftNavigationControllerRestorationKey"];
    [self.mm_drawerController setCenterViewController:centerViewController withCloseAnimation:YES completion:nil];
    [self.mm_drawerController setLeftDrawerViewController:leftSideDrawerViewController];
    [self.mm_drawerController setRightDrawerViewController:rightSideDrawerViewController];
        [self.mm_drawerController setShowsShadow:NO];*/
}

- (IBAction)btnProximityDemo_Touched:(id)sender {
   /* UINavigationController *nav = [[UINavigationController alloc] initWithRootViewController:[self.storyboard instantiateViewControllerWithIdentifier:@"ProximityMode"]];
    
    [self.mm_drawerController setCenterViewController:nav withCloseAnimation:YES completion:nil];*/
}

@end
