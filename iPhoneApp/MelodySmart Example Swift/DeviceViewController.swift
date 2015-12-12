//
//  DeviceViewController.swift
//  MelodySmart Example Swift
//
//  Created by Stanislav Nikolov on 22/01/2015.
//  Copyright (c) 2015 BlueCreation. All rights reserved.
//

import Foundation
import Alamofire

extension String {
    subscript (r: Range<Int>) -> String {
        get {
            let startIndex = self.startIndex.advancedBy(r.startIndex)
            let endIndex = self.startIndex.advancedBy(r.endIndex)
            
            return self[Range(start: startIndex, end: endIndex)]
        }
    }
    
    
    func URLEncodedString() -> String? {
        let customAllowedSet =  NSCharacterSet.URLQueryAllowedCharacterSet()
        let escapedString = self.stringByAddingPercentEncodingWithAllowedCharacters(customAllowedSet)
        return escapedString
    }
    static func queryStringFromParameters(parameters: Dictionary<String,String>) -> String? {
        if (parameters.count == 0)
        {
            return nil
        }
        var queryString : String? = nil
        for (key, value) in parameters {
            if let encodedKey = key.URLEncodedString() {
                if let encodedValue = value.URLEncodedString() {
                    if queryString == nil
                    {
                        queryString = "?"
                    }
                    else
                    {
                        queryString! += "&"
                    }
                    queryString! += encodedKey + "=" + encodedValue
                }
            }
        }
        return queryString
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
    
    @IBOutlet var webAddressField: UITextField!
    @IBOutlet var webIncomingData: UITextField!
    @IBOutlet var webOutgoingData: UITextField!
    
    @IBOutlet var altField: UITextField!
    @IBOutlet var inclineField: UITextField!
    @IBOutlet var distField: UITextField!
    @IBOutlet var speedField: UITextField!

    private weak var i2cController: I2cCommandsViewController?
    private weak var commandsController: RemoteCommandsViewController?
    private weak var otauController: OtauViewController?
    
    private var queryNumber = 0;

    override func viewDidLoad() {
        super.viewDidLoad()
        
        if let _ = melodySmart {
            self.title = melodySmart.name()
            self.melodySmart.delegate = self

            melodySmart.connect()
        }
    }

    deinit {
        if let _ = melodySmart {
            melodySmart.disconnect()
        }
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
    
    
    
    func sendWebRequest(path: Array<String>, query: Dictionary<String, String>) -> Int {
        if let addr = webAddressField.text where addr.characters.count >= 8 {
            self.queryNumber += 1
            
            var url = "http://" + addr + ":8080/"
            url += path.joinWithSeparator("/")
            url += "?"
            for (k, v) in query {
                // Make query strings URL safe
                let convertChars = "!*'();:@&=+$,/?%#[]"
                let kk = CFURLCreateStringByAddingPercentEscapes(
                    nil,
                    k,
                    nil,
                    convertChars,
                    CFStringBuiltInEncodings.UTF8.rawValue
                )
                let vv = CFURLCreateStringByAddingPercentEscapes(
                    nil,
                    v,
                    nil,
                    convertChars,
                    CFStringBuiltInEncodings.UTF8.rawValue
                )
                url += "\(kk)=\(vv)&"
            }
            url += "i=\(self.queryNumber)"
            Alamofire.request(.GET, url)
                .responseJSON { response in
                    let result = response.result
                    if result.isFailure {
                        print("Response result failure: \(result.debugDescription), \(result)")
                    } else if let resp = result.value as? Dictionary<String, AnyObject> {
                        if let path = resp["path"] as? Array<String>,
                               query = resp["query"] as? Dictionary<String, String>,
                               result = resp["result"] as? Dictionary<String, AnyObject> {
                            self.handleWebResponse(path, query: query, result: result)
                        } else {
                            print("Malformed response: \(resp)");
                        }
                    }else {
                        print("Response is not a dictionary: \(response)")
                    }
            }
            return self.queryNumber
        }
        return -1
    }
    func handleWebResponse(path: Array<String>, query: Dictionary<String, String>,
                           result: Dictionary<String, AnyObject>) {
        print("Response success: path=\(path), query=\(query), result=\(result)")
        if let resStr = result["result"] {
            webIncomingData.text = "\(resStr)"
        }else {
            webIncomingData.text = "Error 123"
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
        if textField == self.tfOutgoingData {
            if let _ = melodySmart {
                if !melodySmart.sendData(textField.text!.dataUsingEncoding(NSUTF8StringEncoding, allowLossyConversion: true)) {
                    print("Send error")
                }
            }
        }else if textField == self.webOutgoingData {
            // Send request
            sendWebRequest(["textInput"], query:["q":textField.text!])
        }else if textField == self.webAddressField {
            // Do nothing
        }else {
            print("Unknown text field")
        }

        textField.resignFirstResponder()
        return true
    }

    // MARK: - Navigation
    
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        if let _ = melodySmart {
            (segue.destinationViewController as! MelodyController).melodySmart = melodySmart
        }

        switch segue.identifier! {
        case "i2c":         i2cController = segue.destinationViewController as? I2cCommandsViewController
        case "commands":    commandsController = segue.destinationViewController as? RemoteCommandsViewController
        case "otau":        otauController = segue.destinationViewController as? OtauViewController
        default: break;
        }
    }
}
