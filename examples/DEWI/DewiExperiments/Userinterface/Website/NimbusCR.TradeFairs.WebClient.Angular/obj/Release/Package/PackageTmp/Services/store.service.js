'use strict';
/*
 * Contains a service to communicate with the TRACK TV API
 */
angular.module('app.services').factory('StoreService', ['$rootScope', 'cDB', 'CoreHubService',
	function ($rootScope, cDB, CoreHubService) {
	    var self = this;
	    var clientDB = cDB;
	    var hub = CoreHubService;

	    $rootScope.$on("TradeFairs_GetCMSStores", loadStores);
	    $rootScope.$on("TradeFairs_GetCMSImage", loadStoreImage);

	    $rootScope.$on("TradeFairs_UpdateCMSStore", updateStoreObject);
	    $rootScope.$on("TradeFairs_DeleteCMSStores", deleteStoreObject);

	    var storeList = { stores: null };
	    var storeAdded = false;
	    var StepObject = {
	        Steps: {
	            1: {
	                name: "Address",
	                index: 1
	            },
	            2: {
	                name: "Description",
	                index: 2
	            },
	            3: {
	                name: "Phone & Web",
	                index: 3
	            },
	            4: {
	                name: "Image",
	                index: 4
	            },
	            5: {
	                name: "Categories",
	                index: 5
	            },
	            6: {
	                name: "Miscellaneous",
	                index: 6
	            }
	        },
	        chosenStep: "step1",
	        editStepNum: 1,
	        editStepName: "Address",
	        maxSteps: 6,
	        nextStep: function () {
	            console.log("Next page: ")
	            if (StepObject.editStepNum < StepObject.maxSteps) {
	                StepObject.editStepNum++;
	                StepObject.setStep(StepObject.Steps[StepObject.editStepNum]);

	            };
	        },
	        prevStep: function () {
	            console.log("test");
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
	    }

	    this.setStores = function (stores) {
	        storeList.stores = stores;
	    }

	    this.getStores = function () {

	        return storeList;
	    };

	    this.getSteps = function () {
	        return StepObject;
	    }

	    this.QueryStores = function () {
	        hub.GetCMSStores(clientDB.getCurrentProject());
	    }

	    this.loadStoreImage = function (store, isVisible) {
	        //isVisible attr is not part of store structure
	        //causing errors when a store is being updated.
	        if (isVisible && store.isVisible == undefined) {
	            if (!jQuery.isEmptyObject(store)) {
	                store.isVisible = true;
	                if (store.image_url != "") {
	                    if (store.image_string == "") {
	                        hub.GetCMSImage(store.image_url, "small");
	                    }
	                }
	                else {
	                    if (store.image_string == "")
	                        store.image_string = "./img/blank_profile.jpg"
	                }
	                if (store.bg_image_url != "") {
	                    if (store.bg_image_string == "") {
	                        hub.GetCMSImage(store.bg_image_url, "small");
	                    }
	                }
	                else {
	                    if (store.bg_image_string == "")
	                        store.bg_image_string = "./img/blank_profile.jpg"
	                }

	            };
	        };
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    this.addStore = function (store) {
	        storeAdded = true;
	        store.image_string = store.image_string.replace("data:image/png;base64,", "");
	        store.bg_image_string = store.bg_image_string.replace("data:image/png;base64,", "");
	        store.isActive = (store.isActive === 'true');
	        delete store["$$hashKey"];
	        delete store["outOfDate"];
	        delete store["isVisible"];
	        hub.UpdateCMSStore(store, cDB.getCurrentProject());
	        store.image_string = "data:image/png;base64," + store.image_string;
	        store.bg_image_string = "data:image/png;base64," + store.bg_image_string;
	        storeList.stores.push(angular.copy(store));
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    this.updateStore = function (store) {
	        console.log("Store Service Update");
	        for (var i = 0; i < storeList.stores.length; i++) {
	            if (storeList.stores[i].id == store.id) {
	                store.image_string = store.image_string.replace("data:image/png;base64,", "");
	                store.bg_image_string = store.bg_image_string.replace("data:image/png;base64,", "");
	                store.isActive = (store.isActive === 'true');
	                storeList.stores[i] = angular.copy(store);
	                delete store["$$hashKey"];
	                delete store["isVisible"];
	                hub.UpdateCMSStore(store, cDB.getCurrentProject());
	                store.image_string = "data:image/png;base64," + store.image_string;
	                store.bg_image_string = "data:image/png;base64," + store.bg_image_string;
	                storeList.stores[i] = angular.copy(store);
	                break;
	            };
	        };
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    this.deleteStore = function (store) {
	        for (var i = 0; i < storeList.stores.length; i++) {
	            if (storeList.stores[i].id == store.id) {
	                storeList.stores.splice(i, 1);
	                hub.DeleteCMSStore(store.id, cDB.getCurrentProject());
	                break;
	            };
	        };
	    };

	    //Call back functions

	    function loadStores(event, args) {
	        if (args.data == null)
	            args.data = [];
	        storeList.stores = args.data;
	        clientDB.addCMSStores(clientDB.getCurrentProject(), args.data)
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    function loadStoreImage(event, args) {
	        var data = args.data;
	        if (storeList.stores != null) {
	            for (var i = 0; i < storeList.stores.length; i++) {
	                if (storeList.stores[i].image_url == data.name) {
	                    var imageStr = "data:image/png;base64," + data.image_str;
	                    clientDB.addCMSStoreImageString(cDB.getCurrentProject(), storeList.stores[i].image_url, imageStr)
	                    storeList.stores[i].image_string = imageStr;
	                    break;
	                }
	                else if (storeList.stores[i].bg_image_url == data.name) {
	                    var imageStr = "data:image/png;base64," + data.image_str;
	                    clientDB.addCMSStoreBGImageString(cDB.getCurrentProject(), storeList.stores[i].image_url, imageStr)
	                    storeList.stores[i].bg_image_string = imageStr;
	                    break;
	                };
	            };
	        }


	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    function updateStoreObject(event, args) {
	        console.log("A store has been updated or included id: " + args.data);
	        //Server responds with id of new store.
	        //If store recently added then following is done
	        if (storeAdded) {
	            for (var i = storeList.stores.length - 1; i >= 0; i--) {
	                if (storeList.stores[i].id == -1) {
	                    console.log("ID with -1 found at position: " + i);
	                    storeList.stores[i].id = args.data;
	                    clientDB.addCMSStores(clientDB.getCurrentProject(), storeList.stores);
	                    break;
	                };
	            };
	            storeAdded = false;
	        };
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    function deleteStoreObject(event, args) {
	        //True/False returned from server
	        console.log("A store has been deleted: " + args.data);
	        clientDB.addCMSStores(clientDB.getCurrentProject(), storeList.stores);
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    return self;
	}]);
