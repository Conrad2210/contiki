'use strict';
/*
 * Contains a service to communicate with the TRACK TV API
 */
angular.module('app.services').factory('NewsFeedService', ['$rootScope', 'cDB', 'CoreHubService',
	function ($rootScope, cDB, CoreHubService) {
	    var self = this;
	    var clientDB = cDB;
	    var hub = CoreHubService;

	    $rootScope.$on("TradeFairs_GetCMSNewsFeeds", loadNewsFeeds);
	    $rootScope.$on("TradeFairs_GetCMSImage", loadNewsFeedImage);

	    $rootScope.$on("TradeFairs_UpdateCMSNewsFeed", updateNewsFeedObject);
	    $rootScope.$on("TradeFairs_DeleteCMSNewsFeed", deleteNewsFeedObject);
	    $rootScope.sortReverse = false;
	    var newsFeedList = { newsFeeds: null };
	    var newsFeedAdded = false;
	    var StepObject = {
	        Steps: {
	            1: {
	                name: "Basic",
	                index: 1
	            },
	            2: {
	                name: "Display",
	                index: 2
	            },
	            3: {
	                name: "Image",
	                index: 3
	            },
	            4: {
	                name: "Location",
	                index: 4
	            }
	        },
	        chosenStep: "step1",
	        editStepNum: 4,
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

	    var sliderOptions = {
	        hourly: {
	            min: 0,
	            max: 60
	        },
	        daily: {
	            min: 0,
	            max: 24
	        },
	        weekly: {
	            min: "Monday + 0",
	            max: "Sunday + 23",
	            data: ["Monday + 0h", "Monday + 1h", "Monday + 2h", "Monday + 3h", "Monday + 4h", "Monday + 5h", "Monday + 6h", "Monday + 7h", "Monday + 8h", "Monday + 9h", "Monday + 10h", "Monday + 11h", "Monday + 12h", "Monday + 13h", "Monday + 14h", "Monday + 15h", "Monday + 16h", "Monday + 17h", "Monday + 18h", "Monday + 19h", "Monday + 20h", "Monday + 21h", "Monday + 22h", "Monday + 23h",
	            "Tuesday + 0h", "Tuesday + 1h", "Tuesday + 2h", "Tuesday + 3h", "Tuesday + 4h", "Tuesday + 5h", "Tuesday + 6h", "Tuesday + 7h", "Tuesday + 8h", "Tuesday + 9h", "Tuesday + 10h", "Tuesday + 11h", "Tuesday + 12h", "Tuesday + 13h", "Tuesday + 14h", "Tuesday + 15h", "Tuesday + 16h", "Tuesday + 17h", "Tuesday + 18h", "Tuesday + 19h", "Tuesday + 20h", "Tuesday + 21h", "Tuesday + 22h", "Tuesday + 23h",
	            "Wednesday + 0h", "Wednesday + 1h", "Wednesday + 2h", "Wednesday + 3h", "Wednesday + 4h", "Wednesday + 5h", "Wednesday + 6h", "Wednesday + 7h", "Wednesday + 8h", "Wednesday + 9h", "Wednesday + 10h", "Wednesday + 11h", "Wednesday + 12h", "Wednesday + 13h", "Wednesday + 14h", "Wednesday + 15h", "Wednesday + 16h", "Wednesday + 17h", "Wednesday + 18h", "Wednesday + 19h", "Wednesday + 20h", "Wednesday + 21h", "Wednesday + 22h", "Wednesday + 23h",
	            "Thursday + 0h", "Thursday + 1h", "Thursday + 2h", "Thursday + 3h", "Thursday + 4h", "Thursday + 5h", "Thursday + 6h", "Thursday + 7h", "Thursday + 8h", "Thursday + 9h", "Thursday + 10h", "Thursday + 11h", "Thursday + 12h", "Thursday + 13h", "Thursday + 14h", "Thursday + 15h", "Thursday + 16h", "Thursday + 17h", "Thursday + 18h", "Thursday + 19h", "Thursday + 20h", "Thursday + 21h", "Thursday + 22h", "Thursday + 23h",
	            "Friday + 0h", "Friday + 1h", "Friday + 2h", "Friday + 3h", "Friday + 4h", "Friday + 5h", "Friday + 6h", "Friday + 7h", "Friday + 8h", "Friday + 9h", "Friday + 10h", "Friday + 11h", "Friday + 12h", "Friday + 13h", "Friday + 14h", "Friday + 15h", "Friday + 16h", "Friday + 17h", "Friday + 18h", "Friday + 19h", "Friday + 20h", "Friday + 21h", "Friday + 22h", "Friday + 23h",
	            "Saturday + 0h", "Saturday + 1h", "Saturday + 2h", "Saturday + 3h", "Saturday + 4h", "Saturday + 5h", "Saturday + 6h", "Saturday + 7h", "Saturday + 8h", "Saturday + 9h", "Saturday + 10h", "Saturday + 11h", "Saturday + 12h", "Saturday + 13h", "Saturday + 14h", "Saturday + 15h", "Saturday + 16h", "Saturday + 17h", "Saturday + 18h", "Saturday + 19h", "Saturday + 20h", "Saturday + 21h", "Saturday + 22h", "Saturday + 23h",
	            "Sunday + 0h", "Sunday + 1h", "Sunday + 2h", "Sunday + 3h", "Sunday + 4h", "Sunday + 5h", "Sunday + 6h", "Sunday + 7h", "Sunday + 8h", "Sunday + 9h", "Sunday + 10h", "Sunday + 11h", "Sunday + 12h", "Sunday + 13h", "Sunday + 14h", "Sunday + 15h", "Sunday + 16h", "Sunday + 17h", "Sunday + 18h", "Sunday + 19h", "Sunday + 20h", "Sunday + 21h", "Sunday + 22h", "Sunday + 23h"],
	        },
	        monthly: {
	            min: 1,
	            max: 31
	        },
	        yearly:{
	            min: "January",
	            max: "December",
	            data: ["January","February","March","April","May","June","July","August","September","October","November","December"]
	        }
            
	    }
	    this.getSliderOptions = function(){return sliderOptions;}
	    this.setNewsFeeds = function (newsFeeds) {
	        newsFeedList.newsFeeds = newsFeeds;
	    }

	    this.getNewsFeeds = function () {
	        return newsFeedList;
	    };

	    this.getSteps = function () {
	        return StepObject;
	    }

	    this.QueryNewsFeeds = function () {
	        hub.GetCMSNewsFeeds(clientDB.getCurrentProject(),-1);
	    }

	    this.loadNewsFeedImage = function (newsFeed, isVisible) {
	        //isVisible attr is not part of store structure
	        //causing errors when a store is being updated.
	        if (isVisible && newsFeed.isVisible == undefined) {
	            if (!jQuery.isEmptyObject(newsFeed)) {
	                newsFeed.isVisible = true;
	                if (newsFeed.image_url != "") {
	                    if (newsFeed.image_string == "") {
	                        hub.GetCMSImage(newsFeed.image_url, "small");
	                    }
	                }
	                else {
	                    if (newsFeed.image_string == "")
	                        newsFeed.image_string = "./img/blank_profile.jpg"

	                }
	            }
	        }
	    };

	    this.addNewsFeed = function (newsFeed) {
	        newsFeedAdded = true;
	        newsFeed.project_id = cDB.getCurrentProject();
	        newsFeed.from_user = cDB.getUserName();
	        newsFeed.display_duration = parseInt(newsFeed.display_duration);
	        newsFeed.display_period = parseInt(newsFeed.display_period);
	        newsFeed.store_id = parseInt(newsFeed.store_id);
	        newsFeed.booth_id = parseInt(newsFeed.booth_id);
	        newsFeed.timestamp = new Date().getTime();
	        newsFeed.image_string = newsFeed.image_string.replace("data:image/png;base64,", "");
	        delete newsFeed["$$hashKey"];
	        delete newsFeed["isVisible"];
	        hub.UpdateCMSNewsFeed(newsFeed, cDB.getCurrentProject());
	        newsFeed.image_string = "data:image/png;base64," + newsFeed.image_string;
	        newsFeedList.newsFeeds.push(angular.copy(newsFeed));
	    };

	    this.updateNewsFeed = function (newsFeed) {
	        console.log("NewsFeed Service Update");
	        for (var i = 0; i < newsFeedList.newsFeeds.length; i++) {
	            if (newsFeedList.newsFeeds[i].id == newsFeed.id) {
	                //var tempNewsFeed = copyNewsFeed(newsFeed);
	                newsFeed.project_id = cDB.getCurrentProject();
	                newsFeed.from_user = cDB.getUserName();
	                newsFeed.display_duration = parseInt(newsFeed.display_duration);
	                newsFeed.display_period = parseInt(newsFeed.display_period);
	                newsFeed.store_id = parseInt(newsFeed.store_id);
	                newsFeed.booth_id = parseInt(newsFeed.booth_id);
	                newsFeed.timestamp = new Date().getTime();
	                newsFeed.image_string = newsFeed.image_string.replace("data:image/png;base64,", "");
	                delete newsFeed["$$hashKey"];
	                delete newsFeed["outOfDate"];
	                delete newsFeed["isVisible"];
	                hub.UpdateCMSNewsFeed(newsFeed, cDB.getCurrentProject());
	                newsFeed.image_string = "data:image/png;base64," + newsFeed.image_string;
	                newsFeedList.newsFeeds[i] = angular.copy(newsFeed);
	                break;
	            };
	        };
	    };

	    this.deleteNewsFeed = function (newsFeed) {
	        for (var i = 0; i < newsFeedList.newsFeeds.length; i++) {
	            if (newsFeedList.newsFeeds[i].id == newsFeed.id) {
	                newsFeedList.newsFeeds.splice(i, 1);
	                hub.DeleteCMSNewsFeed(newsFeed.id, cDB.getCurrentProject());
	                break;
	            };
	        };
	    };

	    //Call back functions

	    function loadNewsFeeds(event, args) {
	        if (args.data == null)
	            args.data = [];
	        newsFeedList.newsFeeds = args.data;
	        clientDB.addCMSNewsFeed(clientDB.getCurrentProject(), args.data)
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    
	    function updateNewsFeedObject(event, args) {
	        console.log("A newsFeed has been updated or included id: " + args.data);
	        //Server responds with id of new store.
	        //If store recently added then following is done
	        if (newsFeedAdded) {
	            for (var i = newsFeedList.newsFeeds.length - 1; i >= 0; i--) {
	                if (newsFeedList.newsFeeds[i].id == -1) {
	                    console.log("ID with -1 found at position: " + i);
	                    newsFeedList.newsFeeds[i].id = args.data;
	                    clientDB.addCMSNewsFeed(clientDB.getCurrentProject(), newsFeedList.newsFeeds);
	                    break;
	                };
	            };
	            newsFeedAdded = false;
	        };
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    function deleteNewsFeedObject(event, args) {
	        //True/False returned from server
	        console.log("A news feed has been deleted: " + args.data);
	        clientDB.addCMSNewsFeed(clientDB.getCurrentProject(), newsFeedList.newsFeeds);
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };
	    function loadNewsFeedImage(event, args) {
	        var data = args.data;
	        if (newsFeedList.newsFeeds != null) {

	            for (var i = 0; i < newsFeedList.newsFeeds.length; i++) {
	                if (newsFeedList.newsFeeds[i].image_url == data.name) {
	                    var imageStr = "data:image/png;base64," + data.image_str;
	                    //console.log(imageStr);
	                    clientDB.addCMSNewsFeedImageString(cDB.getCurrentProject(), newsFeedList.newsFeeds[i].image_url, imageStr)
	                    newsFeedList.newsFeeds[i].image_string = imageStr;
	                    break;
	                };
	            }

	        }
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };




	    return self;
	}]);
