//
//  DeviceViewController.swift
//  MelodySmart Example Swift
//
//  Created by Stanislav Nikolov on 22/01/2015.
//  Copyright (c) 2015 BlueCreation. All rights reserved.
//

import Foundation

extension String {
    subscript (r: Range<Int>) -> String {
        get {
            let startIndex = self.startIndex.advancedBy(r.startIndex)
            let endIndex = self.startIndex.advancedBy(r.endIndex)
            
            return self[Range(start: startIndex, end: endIndex)]
        }
    }
}

func hexStrToInt(str: String) -> Int? {
    if var a = Int(str, radix: 16) {
        a & 0x8000
        if a & 0x8000 > 0 {
            a = -((a ^ 0xFFFF) + 1)
        }
        return a
    }
    return nil
}







import UIKit

class MelodyController: UIViewController {
    var melodySmart: MelodySmart!
}

class DeviceViewController: UIViewController, MelodySmartDelegate, UITextFieldDelegate {
    var melodySmart: MelodySmart!

    @IBOutlet var tfIncomingData: UITextField!
    @IBOutlet var tfOutgoingData: UITextField!
    
    @IBOutlet var altField: UITextField!
    @IBOutlet var inclineField: UITextField!
    @IBOutlet var distField: UITextField!
    @IBOutlet var speedField: UITextField!

    private weak var i2cController: I2cCommandsViewController?
    private weak var commandsController: RemoteCommandsViewController?
    private weak var otauController: OtauViewController?

    override func viewDidLoad() {
        super.viewDidLoad()

        self.title = melodySmart.name()
        self.melodySmart.delegate = self

        melodySmart.connect()
    }

    deinit {
        melodySmart.disconnect()
    }

    // MARK: - MelodySmart delegate

    func melodySmart(melody: MelodySmart!, didConnectToMelody result: Bool) {
    }

    func melodySmartDidDisconnectFromMelody(melody: MelodySmart!) {
    }

    func melodySmart(melody: MelodySmart!, didSendData error: NSError!) {
    }

    func melodySmart(melody: MelodySmart!, didReceiveData data: NSData!) {
        let str = NSString(data: data, encoding: NSUTF8StringEncoding)
        tfIncomingData.text = str as? String
        let trimmed = str!.stringByTrimmingCharactersInSet(NSCharacterSet.whitespaceCharacterSet())
        let len = trimmed.characters.count
        print("Trimmed length: \(trimmed.characters.count)")
        if len == 17 && trimmed.hasPrefix("x") {
            let altStr = trimmed[1..<5]
            let incStr = trimmed[5..<9]
            let distStr = trimmed[9..<13]
            let speedStr = trimmed[13..<17]
            if let altVal = hexStrToInt(altStr) {
                let alt = Double(altVal) / 100.0
                self.altField.text = "\(alt) meters"
                print("Success, \(altStr), \("\(alt) meters")");
            } else{
                print("Fail, \(altStr)");
            }
            if let incVal = hexStrToInt(incStr) {
                let inc = Double(incVal) / 100.0
                self.inclineField.text = "\(inc) rad"
            }
            if let distVal = hexStrToInt(distStr) {
                let dist = Double(distVal) / 100.0
                self.distField.text = "\(dist) meters"
            }
            if let speedVal = hexStrToInt(speedStr) {
                let speed = Double(speedVal) / 100.0
                self.speedField.text = "\(speed) meters/sec"
            }
        }
    }

    func melodySmart(melody: MelodySmart!, didReceivePioChange state: UInt8, withLocation location: BcSmartPioLocation) {
    }

    func melodySmart(melody: MelodySmart!, didReceivePioSettingChange state: UInt8, withLocation location: BcSmartPioLocation) {
    }

    func melodySmart(melody: MelodySmart!, didReceiveI2cReplyWithSuccess success: Bool, data: NSData!) {
        i2cController?.didReceiveI2CReplyWithSuccess(success, data: data)
    }

    func melodySmart(melody: MelodySmart!, didReceiveCommandReply reply: NSData!) {
        commandsController?.didReceiveCommandReply(reply)
    }

    func melodySmart(melody: MelodySmart!, didDetectBootMode bootMode: BootMode) {
        otauController?.didDetectBootMode(bootMode)
    }

    func melodySmart(melody: MelodySmart!, didUpdateOtauState state: OtauState, withProgress percent: Int32) {
        otauController?.didUpdateOtauState(state, percent: Int(percent))
    }

    // MARK: - UITextField delegate

    func textFieldShouldReturn(textField: UITextField) -> Bool {
        if !melodySmart.sendData(textField.text!.dataUsingEncoding(NSUTF8StringEncoding, allowLossyConversion: true)) {
            print("Send error")
        }

        textField.resignFirstResponder()

        return true
    }

    // MARK: - Navigation
    
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        (segue.destinationViewController as! MelodyController).melodySmart = melodySmart

        switch segue.identifier! {
        case "i2c":         i2cController = segue.destinationViewController as? I2cCommandsViewController
        case "commands":    commandsController = segue.destinationViewController as? RemoteCommandsViewController
        case "otau":        otauController = segue.destinationViewController as? OtauViewController
        default: break;
        }
    }
}
