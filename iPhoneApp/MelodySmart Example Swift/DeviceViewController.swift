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

class DeviceViewController: UIViewController, MelodySmartDelegate, UITableViewDelegate, UITableViewDataSource {
    var melodySmart: MelodySmart!

    @IBOutlet var tfIncomingData: UITextField!
    @IBOutlet var tfOutgoingData: UITextField!
    
    @IBOutlet var webAddressField: UITextField!
    @IBOutlet var webIncomingData: UITextField!
    @IBOutlet var webStlData: UITextField!
    @IBOutlet var webOutgoingData: UITextField!
    @IBOutlet var liveQuerySwitch: UISwitch!
    
    @IBOutlet var altField: UITextField!
    @IBOutlet var inclineField: UITextField!
    @IBOutlet var distField: UITextField!
    @IBOutlet var speedField: UITextField!

    private weak var i2cController: I2cCommandsViewController?
    private weak var commandsController: RemoteCommandsViewController?
    private weak var otauController: OtauViewController?
    
    private var queryNumber = 0
    private var dataIndex = 0
    private let dataAccumLimit = 4
    private var dataAccum: Array<(String, Double)> = []
    private var lastQueryResponse = ""
    private var presetQueries = [
        "Did I bike 500 meters this week?",
        "Did my average speed exceed 10 m/s",
        "Did I stay above an altitude of 50 meters",
        "Did my speed ever exceed 10 m/s",
        "Was I always faster than 10 m/s",]

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
        UIApplication.sharedApplication().idleTimerDisabled = true
        
        let str = NSString(data: data, encoding: NSUTF8StringEncoding)
        tfIncomingData.text = str as? String
        let trimmed = str!.stringByTrimmingCharactersInSet(NSCharacterSet.whitespaceCharacterSet())
        let len = trimmed.characters.count
        if len == 17 && trimmed.hasPrefix("x") {
            let altStr = trimmed[1..<5]
            let incStr = trimmed[5..<9]
            let distStr = trimmed[9..<13]
            let speedStr = trimmed[13..<17]
            var alt2: Double = Double.NaN, inc2: Double = Double.NaN, dist2: Double = Double.NaN, speed2: Double = Double.NaN
            if let altVal = hexStrToInt(altStr) {
                let alt = Double(altVal) / 100.0
                alt2 = alt
                self.altField.text = "\(alt) meters"
            }
            if let incVal = hexStrToInt(incStr) {
                let inc = Double(incVal) / 100.0
                inc2 = inc
                self.inclineField.text = "\(inc) rad"
            }
            if let distVal = hexStrToInt(distStr) {
                let dist = Double(distVal) / 100.0
                dist2 = dist
                self.distField.text = "\(dist) meters"
            }
            if let speedVal = hexStrToInt(speedStr) {
                let speed = Double(speedVal) / 100.0
                speed2 = speed
                self.speedField.text = "\(speed) meters/sec"
            }
            
            self.dataAccum.append((trimmed, NSDate().timeIntervalSince1970))
            if self.dataAccum.count >= dataAccumLimit {
                self.sendDataUpdate(self.dataAccum)
                self.dataAccum.removeAll()
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
                               result = resp["result"] {
                            self.handleWebResponse(path, query: query, result: result)
                        } else {
                            print("Malformed response: \(resp)")
                        }
                    }else {
                        print("Response is not a dictionary: \(response)")
                    }
            }
            return self.queryNumber
        }
        return -1
    }
    func sendDatumUpdate(raw: String, alt: Double, incl: Double, dist: Double, speed: Double) -> Int {
        let ts = NSDate().timeIntervalSince1970
        let check = liveQuerySwitch.on ? "y" : "n"
        let query: Dictionary<String, String> = ["raw":raw, "ts":String(ts), "index":String(self.dataIndex),
            "alt":String(alt), "incl":String(incl), "dist":String(dist), "speed":String(speed), "check":check]
        self.sendWebRequest(["datum"], query: query)
        
        self.dataIndex += 1
        return self.dataIndex - 1
    }
    func sendDataUpdate(rawTs: Array<(String, Double)>) -> Int {
        var rawStr = "", tsStr = "", indexStr = ""
        var first = true
        var i = self.dataIndex
        for (raw, ts) in rawTs {
            if !first {
                rawStr += ","
                tsStr += ","
                indexStr += ","
            }
            first = false
            rawStr += raw
            tsStr += String(ts)
            indexStr += String(i)
            i += 1
        }
        
        let check = liveQuerySwitch.on ? "y" : "n"
        let query: Dictionary<String, String> = ["raw":rawStr, "ts":tsStr, "index":indexStr, "check":check]
        self.sendWebRequest(["data"], query: query)
        
        self.dataIndex += rawTs.count
        return self.dataIndex - rawTs.count
    }
    func sendStlQuery(question: String) {
        let query: Dictionary<String, String> = ["question":question]
        self.sendWebRequest(["stl"], query: query)
    }
    
    func handleWebResponse(path: Array<String>, query: Dictionary<String, String>,
                           result: AnyObject) {
        //print("Response success: path=\(path), query=\(query), result=\(result)")
        if path.count >= 1 {
            switch path[0] {
            case "datum":
                handleDatumUpdateResponse(query, result: result)
            case "data":
                handleDataUpdateResponse(query, result: result)
            case "stl":
                handleStlQueryResponse(query, result: result)
            default:
                print("Unknown response path: \(path)")
            }
        }else {
            print("No response path: \(path)")
        }
    }
    func handleDatumUpdateResponse(query: Dictionary<String, String>, result: AnyObject) {
        if let response = result as? String {
            print("Data update: \(response)")
        }else {
            print("Data update response invalid: \(query) -> \(result)")
        }
    }
    func handleDataUpdateResponse(query: Dictionary<String, String>, result: AnyObject) {
        if let response = result as? String {
            print("Data update: \(response)")
        }else if let response = result as? Dictionary<String, AnyObject>, check = query["check"] where check == "y" {
            self.handleStlQueryResponse(query, result: response)
        }else {
            print("Data update response invalid: \(query) -> \(result)")
        }
    }
    func handleStlQueryResponse(query: Dictionary<String, String>, result: AnyObject) {
        if let result2 = result as? Dictionary<String, AnyObject> {
            if let answer = result2["answer"] as? String,
                   stl = result2["stl"] as? String,
                   error = result2["error"] as? Int,
                   errorDesc = result2["errorDesc"] as? String {
                if error != 0 {
                    webIncomingData.text = "Error \(error): \(errorDesc)"
                    webStlData.text = ""
                } else {
                    webIncomingData.text = "Answer: \(answer)"
                    webStlData.text = "STL: \(stl)"
                    print("STL Query response: \(query)")
                    if let check = query["check"] where check == "y" {
                        if answer != self.lastQueryResponse {
                            let alertController = UIAlertController(title: "Congrats", message:
                                "The result of your query has changed from \(self.lastQueryResponse) to \(answer).", preferredStyle: UIAlertControllerStyle.Alert)
                            alertController.addAction(UIAlertAction(title: "Dismiss", style: UIAlertActionStyle.Default,handler: nil))
                            self.presentViewController(alertController, animated: true, completion: nil)
                        }
                    }
                    self.lastQueryResponse = answer
                }
            }else {
                print("STL query response invalid: \(query) -> \(result2)")
                webIncomingData.text = "Server error"
                webStlData.text = ""
            }
        }else if let result2 = result as? String {
            print("STL query server crash: \(result2)")
            webIncomingData.text = "Error 99: Unknown error"
            webStlData.text = ""
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
            sendStlQuery(textField.text!)
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
    
    
    // MARK: - Table view data source
    
    func numberOfSectionsInTableView(tableView: UITableView) -> Int {
        return 1
    }
    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        if section == 0 {
            return presetQueries.count
        }
        return 0;
    }
    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCellWithIdentifier("reuseIdentifier", forIndexPath: indexPath)
        
        if let label = cell.viewWithTag(99) as? UILabel {
            label.text = self.presetQueries[indexPath.row]
        }
        
        return cell
    }
    func tableView(tableView: UITableView, titleForHeaderInSection section: Int) -> String? {
        if section == 0 {
            return "Preset queries"
        }
        return nil;
    }
    
    // MARK: - Table view delegate
    
    func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        let query = self.presetQueries[indexPath.row]
        self.webOutgoingData.text = query;
        sendStlQuery(query)
        tableView.deselectRowAtIndexPath(indexPath, animated: true)
    }
}
