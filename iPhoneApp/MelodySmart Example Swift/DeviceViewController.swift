//
//  DeviceViewController.swift
//  MelodySmart Example Swift
//
//  Created by Stanislav Nikolov on 22/01/2015.
//  Copyright (c) 2015 BlueCreation. All rights reserved.
//

import UIKit

class MelodyController: UIViewController {
    var melodySmart: MelodySmart!
}

class DeviceViewController: UIViewController, MelodySmartDelegate, UITextFieldDelegate {
    var melodySmart: MelodySmart!

    @IBOutlet var tfIncomingData: UITextField!
    @IBOutlet var tfOutgoingData: UITextField!

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
        tfIncomingData.text = NSString(data: data, encoding: NSUTF8StringEncoding) as? String
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
