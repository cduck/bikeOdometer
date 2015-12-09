//
//  MSRemoteCommandsViewController.swift
//  MelodySmart_Example
//
//  Created by Stanislav Nikolov on 15/01/2015.
//  Copyright (c) 2015 Blue Creation. All rights reserved.
//

import UIKit

class RemoteCommandsViewController: MelodyController, UITextFieldDelegate {
    @IBOutlet var tfCommand: UITextField!
    @IBOutlet var tvOutput: UITextView!

    func didReceiveCommandReply(reply: NSData) {
        tvOutput.text? += NSString(data: reply, encoding: NSUTF8StringEncoding)! as String

        tvOutput.scrollRangeToVisible(NSRange(location: tvOutput.text.characters.count, length: 1))
    }

    func textFieldShouldReturn(textField: UITextField) -> Bool {
        melodySmart.sendRemoteCommand(tfCommand.text)

        textField.resignFirstResponder()

        return true
    }

}
