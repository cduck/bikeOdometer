//
//  MSI2cCommandsViewController.swift
//  MelodySmart Example v2
//
//  Created by Stanislav Nikolov on 14/01/2015.
//  Copyright (c) 2015 Blue Creation. All rights reserved.
//

import UIKit
import CoreBluetooth

extension String {
    func dataFromHexadecimalString() -> NSData? {
        let trimmedString = self.stringByTrimmingCharactersInSet(NSCharacterSet(charactersInString: "<> ")).stringByReplacingOccurrencesOfString(" ", withString: "")
        
        // make sure the cleaned up string consists solely of hex digits, and that we have even number of them
        
        let regex: NSRegularExpression?
        do {
            regex = try NSRegularExpression(pattern: "^[0-9a-f]*$", options: .CaseInsensitive)
        } catch {
            regex = nil
        }

        let found = regex?.firstMatchInString(trimmedString, options: [], range: NSMakeRange(0, trimmedString.characters.count))
        if found == nil || found!.range.location == NSNotFound || trimmedString.characters.count % 2 != 0 {
            return nil
        }
        
        // everything ok, so now let's build NSData
        
        let data = NSMutableData(capacity: trimmedString.characters.count / 2)
        
        for var index = trimmedString.startIndex; index < trimmedString.endIndex; index = index.successor().successor() {
            let byteString = trimmedString.substringWithRange(Range<String.Index>(start: index, end: index.successor().successor()))
            let num = UInt8(byteString.withCString { strtoul($0, nil, 16) })
            data?.appendBytes([num] as [UInt8], length: 1)
        }
        
        return data
    }
}

class I2cCommandsViewController: MelodyController, UITextFieldDelegate {

    @IBOutlet var tfDeviceAddress: UITextField!
    @IBOutlet var tfRegisterAddress: UITextField!
    @IBOutlet var tfOutgoingData: UITextField!
    @IBOutlet var tfIncomingData: UITextField!
    @IBOutlet var lblStatus: UILabel!

    func didReceiveI2CReplyWithSuccess(success: Bool, data: NSData) {
        lblStatus.text = "Operation " + (success ? "successful" : "failed")
        tfIncomingData.text = data.description.stringByTrimmingCharactersInSet(NSCharacterSet(charactersInString: "<>"))
    }

    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Do any additional setup after loading the view.
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    @IBAction func btnRead_TouchUpInside() {
        lblStatus.text = ""
        tfIncomingData.text = ""

        if let ret = validateDeviceAddress() {
            lblStatus.text = "Reading...";
            
            let length: UInt8 = 16

            melodySmart.readI2cDataFromDevAddress(ret.deviceAddress, writePortion: ret.registerAddress, length: length)
        }
    }
    
    @IBAction func btnWrite_TouchUpInside() {
        lblStatus.text = "";

        guard let ret = validateDeviceAddress() else {
            return
        }

        let data = NSMutableData(data: ret.registerAddress)
        let payload = tfOutgoingData.text!.dataFromHexadecimalString()
        
        if payload != nil && payload!.length + ret.registerAddress.length <= 19 {
            data.appendData(payload!)
            
            lblStatus.text = "Writing...";
            
            melodySmart.writeI2cData(data, toDevAddress: ret.deviceAddress)
        } else {
            UIAlertView(title: "Error",
                message: "The outgoing data should consist of an even number of hex digits and be no more than 16 bytes!",
                delegate: nil,
                cancelButtonTitle: "OK").show()
        }
    }

    func validateDeviceAddress() -> (deviceAddress: UInt8, registerAddress: NSData)? {
        guard tfDeviceAddress.text!.characters.count == 2 else {
            UIAlertView(title: "Error", message: "The device address should consit of 2 hex digits!", delegate: nil, cancelButtonTitle: "OK").show()
            return nil;
        }

        guard let regAddr = tfRegisterAddress.text!.dataFromHexadecimalString() else {
            UIAlertView(title: "Error", message: "The register address should consist of an even number of hex digits!", delegate: nil, cancelButtonTitle: "OK").show()
            return nil
        }

        var deviceAddr: UInt32 = 0
        let scanner = NSScanner(string: tfDeviceAddress.text!)
        scanner.scanHexInt(&deviceAddr)

        return (UInt8(deviceAddr), regAddr)
    }

    func textFieldShouldReturn(textField: UITextField) -> Bool {
        textField.resignFirstResponder()

        return true
    }
}
