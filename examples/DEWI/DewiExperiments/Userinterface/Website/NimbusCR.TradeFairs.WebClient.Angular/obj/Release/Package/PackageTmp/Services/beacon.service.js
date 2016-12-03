'use strict';
/*
 * Contains a service to communicate with the TRACK TV API
 */
angular.module('app.services').factory('BeaconService', ['$rootScope', 'cDB', 'CoreHubService',
	function ($rootScope, cDB, CoreHubService) {
	    var self = this;
	    var clientDB = cDB;
	    var hub = CoreHubService;

	    $rootScope.$on("TradeFairs_GetCMSBeacons", loadBeacons);

	    $rootScope.$on("TradeFairs_UpdateCMSBeacon", updateBeaconObject);
	    $rootScope.$on("TradeFairs_DeleteCMSBeacons", deleteBeaconObject);
	    $rootScope.sortReverse = false;
	    var beaconList = { beacons: null };
	    var beaconAdded = false;
	    var StepObject = {
	        Steps: {
	            1: {
	                name: "Basic",
	                index: 1
	            },
	            2: {
	                name: "Advanced",
	                index: 2
	            },
	            3: {
	                name: "Store selection",
	                index: 3
	            }
	        },
	        chosenStep: "step1",
	        editStepNum: 3,
	        editStepName: "Basic",
	        maxSteps:1,
	        nextStep: function () {
	            console.log("Next page: ")
	            if (StepObject.editStepNum < StepObject.maxSteps) {
	                StepObject.editStepNum++;
	                StepObject.setStep(StepObject.Steps[StepObject.editStepNum]);

	            };
	        },
	        prevStep: function () {
	            if (StepObject.editStepNum > 1) {
	                StepObject.editStepNum--;


	                StepObject.setStep(StepObject.Steps[StepObject.editStepNum]);
	            };
	        },
	        setStep: function (object) {
	            StepObject.editStepName = object.name;
	            StepObject.chosenStep = "step" + object.index;
	            StepObject.editStepNum = object.index
	        }
	    };

	    this.setBeacons = function (beacons) {
	        beaconList.beacons = beacons;
	    }

	    this.getBeacons = function () {
	        return beaconList;
	    };

	    this.getSteps = function () {
	        return StepObject;
	    }

	    this.QueryBeacons = function () {
	        hub.GetCMSBeacons(clientDB.getCurrentProject());
	    }

	   

	    this.addBeacon = function (beacon) {
	        beaconAdded = true;
	        var tempBeacon = copyBeacon(beacon);
	        beaconList.beacons.push(angular.copy(tempBeacon));
	        delete tempBeacon["$$hashKey"];
	        delete tempBeacon["outOfDate"];
	        delete tempBeacon["isVisible"];
	        hub.UpdateCMSBeacons(tempBeacon, cDB.getCurrentProject());
	    };

	    this.updateBeacon = function (beacon) {
	        console.log("Beacon Service Update");
	        for (var i = 0; i < beaconList.beacons.length; i++) {
	            if (beaconList.beacons[i].id == beacon.id) {
	                var tempBeacon = copyBeacon(beacon);
	                beaconList.beacons[i] = angular.copy(tempBeacon);
	                delete tempBeacon["$$hashKey"];
	                delete tempBeacon["outOfDate"];
	                delete tempBeacon["isVisible"];
	                hub.UpdateCMSBeacons(tempBeacon, cDB.getCurrentProject());
	                break;
	            };
	        };
	    };

	    this.deleteBeacon = function (beacon) {
	        for (var i = 0; i < beaconList.beacons.length; i++) {
	            if (beaconList.beacons[i].id == beacon.id) {
	                beaconList.beacons.splice(i, 1);
	                hub.DeleteCMSBeacons(beacon.id, cDB.getCurrentProject());
	                break;
	            };
	        };
	    };

	    //Call back functions

	    function loadBeacons(event, args) {
	        if (args.data == null)
	            args.data = [];
	        beaconList.beacons = args.data;
	        clientDB.addCMSBeacons(clientDB.getCurrentProject(), args.data)
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    
	    function updateBeaconObject(event, args) {
	        console.log("A beacon has been updated or included id: " + args.data);
	        //Server responds with id of new store.
            //If store recently added then following is done
	        if (beaconAdded) {
	            for (var i = beaconList.beacons.length - 1; i >= 0; i--) {
	                if (beaconList.beacons[i].id == -1) {
	                    console.log("ID with -1 found at position: " + i);
	                    beaconList.beacons[i].id = args.data;
	                    clientDB.addCMSBeacons(clientDB.getCurrentProject(), beaconList.beacons);
	                    break;
	                };
	            };
	            beaconAdded = false;
	        };
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    function deleteBeaconObject(event, args) {
            //True/False returned from server
	        console.log("A both has been deleted: " + args.data);
	        clientDB.addCMSBeacons(clientDB.getCurrentProject(), beaconList.beacons);
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };


	    function copyBeacon (beacon) {
	        var tempBeacon = new S_CMSBeaconJS();
	        tempBeacon.description = beacon.description;
	        tempBeacon.id = parseInt(beacon.id);
	        tempBeacon.list_item = beacon.list_item;
	        tempBeacon.name = beacon.name;
	        tempBeacon.store_id = parseInt(beacon.store_id);
	        tempBeacon.booth_id = parseInt(beacon.booth_id);
	        tempBeacon.project_id = clientDB.getCurrentProject();
	        tempBeacon.minor = parseInt(beacon.minor);
	        tempBeacon.major = parseInt(beacon.major);
	        tempBeacon.subsystem_name = beacon.subsystem_name;
	        tempBeacon.subsystem_uuid = beacon.subsystem_uuid;
	        tempBeacon.isActive = beacon.isActive;
	        tempBeacon.display_duration = parseInt(beacon.display_duration);
	        tempBeacon.display_period = parseInt(beacon.display_period);
	        return tempBeacon;
	    }

	    return self;
	}]);
