//
//  MelodyPeripheral.h
//  melodyperipheral
//
//  Created by Ahmad on 05/03/2014.
//  Copyright (c) 2014 BlueCreation. All rights reserved.
//

#import <Foundation/Foundation.h>

@class MelodyPeripheral;

/*!
 *  @protocol MelodyPeripheralDelegate
 *
 *  @discussion The delegate of a MelodySmart object must adopt the <code>MelodyPeripheralDelegate</code> protocol. The requirement of methods are
 *  dependent on the implementation.
 *
 */
@protocol MelodyPeripheralDelegate <NSObject>

/*!
 *  @method melodyPeripheralIsReady:
 *
 *  @discussion     Invoked whenever the Melody Peripheral is ready to send data
 *
 */
- (void) melodyPeripheralIsReady:(MelodyPeripheral*)melody;

/*!
 *  @method melodyPeripheralIsConnected:
 *
 *  @discussion     Invoked whenever the Melody Peripheral is connected. Being connected does not mean App can send data.
    App needs to wait for <code>melodyPeripheralIsReady</code>.
 *
 */
- (void) melodyPeripheralIsConnected:(MelodyPeripheral*)melody;

/*!
 *  @method melodyPeripheral:
 *
 *  @discussion     Invoked whenever the Melody Peripheral recieved data from the Central
 *
 */
- (void) melodyPeripheral:(MelodyPeripheral*)melody didReceiveData:(NSData*)data;

@end

@interface MelodyPeripheral : NSObject

@property (nonatomic, assign) NSObject<MelodyPeripheralDelegate> *delegate;

@property (readonly) int MTU;

/*!
 *  @method setForService
 *
 *  @param  serviceUUid     Change the last 8 bytes of the Primary Service UUID.
 *
 *  @param  dataUUID        Change the last 8 bytes of the Data Characteristic UUID.
 *
 *
 *  @discussion Initialises MelodySmart with modified UUIDs, To comply with the Melody Central, only the last 8 bytes are allowed to change. This will make sure that the application can find the correct peripheral. Setting any value to nil will keep the default value.
 *
 */
- (void) setForService:(NSString*)serviceUUID andDataCharacterisitc:(NSString*)dataUUID;

/*!
 *  @method startAdvertising
 *
 *  @param Name     Set the Device name in the advertising data.
 *
 *  @discussion Start Advertising with the correct Primary UUID
 *
 */
- (void) startAdvertising:(NSString*)Name;

/*!
 *  @method sendData
 *
 *  @param data The data to send to the Central. Due to BLE protocol limitations, the maximum size of a single data chunk is 20 bytes. No data is transmitted if the chunk exceeds that size.
 *
 *  @discussion Sends data using the Data service to the connected Melody Central.
 *
 */
- (BOOL) sendData:(NSData*)data;

@end
