//
//  MSOtauViewController.swift
//  MelodySmart_Example
//
//  Created by Stanislav Nikolov on 16/01/2015.
//  Copyright (c) 2015 Blue Creation. All rights reserved.
//

import UIKit

class MelodyRelease {
    var version: String!
    var imageFileUrl: NSURL!
    var keyFileUrl: NSURL!
    var releaseDate: NSDate!

    init() {
    }
}

class OtauViewController: MelodyController {

    @IBOutlet var lblImage: UILabel!
    @IBOutlet var lblDeviceVersion: UILabel!
    @IBOutlet var lblProgress: UILabel!
    @IBOutlet var lblStatus: UILabel!
    @IBOutlet var btnStartOtau: UIButton!
    @IBOutlet var pvProgress: UIProgressView!

    private var imageData: NSData?
    private var keyData: NSData?

    func didDetectBootMode(bootMode: BootMode) {
        print("didDetectBootMode: \(bootMode)")

        switch bootMode.rawValue {
        case BOOT_MODE_APPLICATION.rawValue:
            updateUi();

        case BOOT_MODE_BOOTLOADER.rawValue:
            btnStartOtau.enabled = false
            melodySmart.startOtauWithImageData(imageData, keyFileData: keyData)

        default:
            break
        }
    }

    func didUpdateOtauState(state: OtauState, percent: Int) {
        print("didUpdateOtauState: \(state.rawValue), percent = \(percent)")

        switch state.rawValue {
        case OTAU_IN_PROGRESS.rawValue:
            lblStatus.text = "Update In Progress..."
            pvProgress.progress = Float(percent) / 100.0
            lblProgress.text = "\(percent)%"

        case OTAU_COMPLETE.rawValue:
            lblStatus.text = "Update successful!"

        case OTAU_FAILED.rawValue:
            lblStatus.text = "Update failed!"

        case OTAU_IDLE.rawValue:
            lblStatus.text = "Idle"

        default:
            break
        }
    }

    @IBAction func btnStartOtau_onTouchedUpInside() {
        lblStatus.text = "Downloading FW image..."

        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), { () -> Void in
            self.imageData = NSData(contentsOfURL: self.melodyRelease!.imageFileUrl)
            self.keyData = NSData(contentsOfURL: self.melodyRelease!.keyFileUrl)

            if self.imageData != nil && self.keyData != nil {

                dispatch_async(dispatch_get_main_queue(), { () -> Void in
                    self.lblStatus.text = "Rebooting device"
                    self.melodySmart.rebootToOtauMode()
                })
            } else {
                self.lblStatus.text = "Error"
                UIAlertView(title: "Error", message: "Error downloading FW image, please try again!", delegate: nil, cancelButtonTitle: "OK").show()
            }
        })
    }

    var melodyRelease: MelodyRelease? {
        didSet { updateUi() }
    }

    func updateUi() {
        let formatter = NSDateFormatter()
        formatter.dateStyle = NSDateFormatterStyle.ShortStyle
        if melodyRelease != nil {
            lblImage.text = "Selected FW: \(melodyRelease!.version) @ \(formatter.stringFromDate(melodyRelease!.releaseDate))"
        }
        btnStartOtau.enabled = melodyRelease != nil
        lblDeviceVersion.text = "Device FW: \(melodySmart.firmwareVersion)"
    }

    override func viewDidLoad() {
        super.viewDidLoad()

        updateUi()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    

    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        if segue.identifier == "image_selection" {
            (segue.destinationViewController as! OtauImageSelectionTableViewController).delegate = self;
        }
    }

}
