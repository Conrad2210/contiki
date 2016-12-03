'use strict';
/*
 * Contains a service to communicate with the TRACK TV API
 */
angular.module('app.services').factory('BoothService', ['$rootScope', 'cDB', 'CoreHubService',
	function ($rootScope, cDB, CoreHubService) {
	    var self = this;
	    var clientDB = cDB;
	    var hub = CoreHubService;

	    $rootScope.$on("TradeFairs_GetCMSBooths", loadBooths);

	    $rootScope.$on("TradeFairs_UpdateCMSBooth", updateBoothObject);
	    $rootScope.$on("TradeFairs_DeleteCMSBooths", deleteBoothObject);
	    $rootScope.sortReverse = false;
	    var boothList = { booths: null };
	    var boothAdded = false;
	    var StepObject = {
	        Steps: {
	            1: {
	                name: "Information",
	                index: 1
	            }
	        },
	        chosenStep: "step1",
	        editStepNum: 1,
	        editStepName: "Information",
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

	    this.setBooths = function (booths) {
	        boothList.booths = booths;
	    }

	    this.getBooths = function () {
	        return boothList;
	    };

	    this.getSteps = function () {
	        return StepObject;
	    }

	    this.QueryBooths = function () {
	        hub.GetCMSBooths(clientDB.getCurrentProject());
	    }

	   

	    this.addBooth = function (booth) {
	        boothAdded = true;
	        boothList.booths.push(angular.copy(booth));
	        delete booth["$$hashKey"];
	        delete booth["outOfDate"];
	        delete booth["isVisible"];
	        hub.UpdateCMSBooth(booth, cDB.getCurrentProject());
	    };

	    this.updateBooth = function (booth) {
	        console.log("Booth Service Update");
	        for (var i = 0; i < boothList.booths.length; i++) {
	            if (boothList.booths[i].id == booth.id) {
	                boothList.booths[i] = angular.copy(booth);
	                delete booth["$$hashKey"];
	                delete booth["outOfDate"];
	                delete booth["isVisible"];
	                hub.UpdateCMSBooth(booth, cDB.getCurrentProject());
	                break;
	            };
	        };
	    };

	    this.deleteBooth = function (booth) {
	        for (var i = 0; i < boothList.booths.length; i++) {
	            if (boothList.booths[i].id == booth.id) {
	                boothList.booths.splice(i, 1);
	                hub.DeleteCMSBooths(booth.id, cDB.getCurrentProject());
	                break;
	            };
	        };
	    };

	    //Call back functions

	    function loadBooths(event, args) {
	        if (args.data == null)
	            args.data = {};
	        boothList.booths = args.data;
	        clientDB.addCMSBooths(clientDB.getCurrentProject(), args.data)
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    
	    function updateBoothObject(event, args) {
	        console.log("A booth has been updated or included id: " + args.data);
	        //Server responds with id of new store.
            //If store recently added then following is done
	        if (boothAdded) {
	            for (var i = boothList.booths.length - 1; i >= 0; i--) {
	                if (boothList.booths[i].id == -1) {
	                    console.log("ID with -1 found at position: " + i);
	                    boothList.booths[i].id = args.data;
	                    clientDB.addCMSBooths(clientDB.getCurrentProject(), boothList.booths);
	                    break;
	                };
	            };
	            boothAdded = false;
	        };
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    function deleteBoothObject(event, args) {
            //True/False returned from server
	        console.log("A both has been deleted: " + args.data);
	        clientDB.addCMSBooths(clientDB.getCurrentProject(), boothList.booths);
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    return self;
	}]);
