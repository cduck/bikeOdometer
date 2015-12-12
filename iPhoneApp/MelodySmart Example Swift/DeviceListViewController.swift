//
//  DeviceListViewController.swift
//  MelodySmart Example Swift
//
//  Created by Stanislav Nikolov on 22/01/2015.
//  Copyright (c) 2015 BlueCreation. All rights reserved.
//

import UIKit

class DeviceListViewController: UITableViewController, MelodyManagerDelegate {

    var detailViewController: DeviceViewController?

    private var objects: [MelodySmart]!
    private var melodyManager: MelodyManager!

    override func viewDidLoad() {
        super.viewDidLoad()

        objects = []

        melodyManager = MelodyManager()
        melodyManager.setForService(nil, andDataCharacterisitc:nil, andPioReportCharacteristic:nil, andPioSettingCharacteristic:nil)
        melodyManager.delegate = self

        print("Using MelodyManager v\(melodyManager.version)")

        navigationItem.leftBarButtonItem = UIBarButtonItem(title: "Rescan", style: .Plain, target: self, action: Selector("scan"))
    }

    override func viewDidAppear(animated: Bool) {
        scan()
    }

    override func viewWillDisappear(animated: Bool) {
        melodyManager.stopScanning()
    }

    func scan() {
        clearObjects()
        melodyManager.scanForMelody()
    }

    func clearObjects() {
        let indexPaths = (0..<objects.count).map({ i in NSIndexPath(forRow: i, inSection: 0)})

        objects.removeAll(keepCapacity: true)

        tableView.deleteRowsAtIndexPaths(indexPaths, withRowAnimation: .Automatic)
    }

    func insertNewObject(device: MelodySmart) {
        objects.append(device)

        let indexPath = NSIndexPath(forRow: objects.count - 1, inSection: 0)
        tableView.insertRowsAtIndexPaths([indexPath], withRowAnimation: .Automatic)
    }

    // MARK: - MelodyManager delegate

    func melodyManagerDiscoveryDidRefresh(manager: MelodyManager!) {
//        println("discoveryDidRefresh")
        for i in objects.count..<MelodyManager.numberOfFoundDevices() {
            insertNewObject(MelodyManager.foundDeviceAtIndex(i))
        }

        tableView.reloadData()
    }

    // MARK: - Table view data source

    override func numberOfSectionsInTableView(tableView: UITableView) -> Int {
        return 1
    }

    override func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return objects.count
    }

    override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCellWithIdentifier("Cell", forIndexPath: indexPath) 

        let device = objects[indexPath.row]
        cell.textLabel?.text = "name: \(device.name()), rssi: \(device.RSSI())"
        cell.detailTextLabel?.text = "data: \(device.manufacturerAdvData)"

        return cell
    }

    // MARK: - Navigation

    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        switch segue.identifier! {
        case "showDetail":
            let destination = segue.destinationViewController as! DeviceViewController
            destination.melodySmart = objects[tableView.indexPathForSelectedRow!.row]
        case "skipToDetail":
            let destination = segue.destinationViewController as! DeviceViewController
            //destination.melodySmart = nil
            
        default:
            break
        }
    }
}
