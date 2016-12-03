'use strict';
/*
 * Contains a controller to do the user login
 */
angular.module('app.controllers').controller('NewsFeedController', ['$scope', 'NewsFeedService', 'PaginationService', 'StoreService', 'BoothService',
function ($scope, NewsFeedService, PaginationService, StoreService, BoothService) {

    var newsFeeds = NewsFeedService;
    var pagination = PaginationService;
    var stores = StoreService;
    var booths = BoothService;
    var sliderOptions = newsFeeds.getSliderOptions();
    $scope.fromDateTest = new Date();
    $scope.date_to = new Date();
    $scope.slider = {};
    $scope.format = "dd-MMM-yyyy";

    $scope.boothList = { booths: null };
    $scope.boothList = booths.getBooths();
    $scope.storeList = { stores: null };
    $scope.storeList = stores.getStores();
    $scope.newsFeedList = { newsFeeds: null };
    $scope.newsFeedList = newsFeeds.getNewsFeeds();
    $scope.StepObject = newsFeeds.getSteps();


    $scope.SortTypeArray = ["title", "store_id", "actiontype"];
    $scope.sortType = 'title'; // set the default sort type
    $scope.sortReverse = false;
    $scope.changeSortType = function (sortType, sortReverse) {
        $scope.sortType = sortType;
        if (sortReverse == 1) {
            $scope.sortReverse = !$scope.sortReverse
        }
    }

    $scope.activeTab = 1;
    $scope.setActiveTab = function (tabToSet) { $scope.activeTab = tabToSet; };
    $scope.editTab = "tab1";
    $scope.setEditTab = function (name) { $scope.editTab = name; };

    $scope.newsFeedDetailsShow = function (newsFeedObject) {
        $scope.newsFeedDetails = angular.copy(newsFeedObject);   //Sets up store object to be edited
    };



    $scope.nameInput = function () {
        if ($scope.tempNewsFeed.name != undefined)
            $scope.isValid = true;
        else
            $scope.isValid = false;

    }

    $scope.updateNewsFeed = function () {
        if ($scope.isNewNewsFeed == true)
            newsFeeds.addNewsFeed($scope.editNewsFeed);
        else
            newsFeeds.updateNewsFeed($scope.editNewsFeed);
    };
    $scope.deleteNewsFeed = function () {
        newsFeeds.deleteNewsFeed($scope.tempNewsFeed);
    };

    $scope.addEditNewsFeedSetup = function (newsFeed, num) {
        if (num == 1) {
            $scope.tempNewsFeed = new S_CMSNewsFeedJS();
            $scope.tempNewsFeed.list_item = new S_CMSListItemJS();
            $scope.tempNewsFeed.valid_to = new Date();
            $scope.tempNewsFeed.valid_from = new Date();
            $scope.newsFeedOption = 1;
        }
        else {
            $scope.tempNewsFeed = angular.copy(newsFeed);
            $scope.tempNewsFeed.valid_to = epochToDate($scope.tempNewsFeed.valid_to * 1000);
            $scope.tempNewsFeed.valid_from = epochToDate($scope.tempNewsFeed.valid_from * 1000);
            $scope.newsFeedOption = 2;
        };
    };
    $scope.tempNewsFeed = new S_CMSNewsFeedJS();
    $scope.tempNewsFeed.list_item = new S_CMSListItemJS();
    $scope.newsFeedOption = 1;
    $scope.addUpdateNewsFeed = function () {
        $scope.tempNewsFeed.valid_to = parseInt($scope.tempNewsFeed.valid_to.getTime() / 1000);
        $scope.tempNewsFeed.valid_from = parseInt($scope.tempNewsFeed.valid_from.getTime() / 1000);
        if ($scope.newsFeedOption == 1) {
            if ($scope.tempNewsFeed.name != "") {
                newsFeeds.addNewsFeed($scope.tempNewsFeed);
            };
        }
        else {
            newsFeeds.updateNewsFeed($scope.tempNewsFeed);
        };
    };
    $scope.deleteNewsFeedModal = function (newsFeed) {
        $("#deleteNewsFeedModal").modal("toggle");
        $scope.tempNewsFeed = angular.copy(newsFeed);
    };

    //Area for all Pagination details
    $scope.paginationDetails = {};
    $scope.paginationDetails = pagination.getDetails();
    $scope.changePageNum = function (number) {
        pagination.changePageNum(number, "Store");
    };
    $scope.firstPage = function () {
        pagination.firstPage();
    };
    $scope.lastPage = function () {
        pagination.lastPage();
    }
    $scope.prevPage = function () {
        pagination.prevPage();
    };
    $scope.nextPage = function () {
        pagination.nextPage();
    }

    $scope.$watch('newsFeedList.newsFeeds.length', function () {
        if ($scope.newsFeedList.newsFeeds != null) {
            pagination.setDetails("NewsFeed");
        };
    });

    $scope.previewNewsFeedImg = false;
    $scope.showNewsFeedImg = true;
    $scope.newsFeedImg = null;
    $scope.onLoad = function (e, reader, file, fileList, fileOjects, fileObj) {
        $scope.newsFeedImg = fileObj;
        $scope.previewNewsFeedImg = true;
    };
    $scope.deleteNewsFeedImage = function () {
        $scope.tempNewsFeed.image_string = "delete";
        $scope.showNewsFeedImg = false;
    };
    $scope.changeNewsFeedImage = function () {
        $scope.tempNewsFeed.image_string = angular.copy("data:image/png;base64," + $scope.newsFeedImg.base64);
        $scope.showNewsFeedImg = true;
        $scope.previewNewsFeedImg = false;
        $scope.newsFeedImg.base64 = null;
        $scope.newsFeedImg = null;
    };
    $scope.discardUploadedImage = function () {
        $scope.previewNewsFeedImg = false;
        $scope.newsFeedImg.base64 = null;
        $scope.newsFeedImg = null;
    };

    $scope.loadNewsFeedImage = function (newsFeed, isVisible) {
        newsFeeds.loadNewsFeedImage(newsFeed, isVisible);
    };


    $scope.$watch('tempNewsFeed.recurrence_period', function (newValues) {

        switch ($scope.tempNewsFeed.recurrence_period) {
            case "S_CMSRecurrencePeriod_hourly":
                $scope.slider.to = $scope.tempNewsFeed.recurrence_to;
                $scope.slider.from = $scope.tempNewsFeed.recurrence_from;
                $scope.slider.toOptions = {
                    floor: sliderOptions.hourly.min,
                    ceil: sliderOptions.hourly.max,
                    onChange: function (sliderId, modelValue, highValue, pointerType) {
                        $scope.tempNewsFeed.recurrence_to = modelValue;
                    }
                };
                $scope.slider.fromOptions = {
                    floor: sliderOptions.hourly.min,
                    ceil: sliderOptions.hourly.max,
                    onChange: function (sliderId, modelValue, highValue, pointerType) {
                        $scope.tempNewsFeed.recurrence_from = modelValue;
                    }
                };

                break;
            case "S_CMSRecurrencePeriod_daily":
                $scope.slider.to = $scope.tempNewsFeed.recurrence_to;
                $scope.slider.from = $scope.tempNewsFeed.recurrence_from;
                $scope.slider.toOptions = {
                    floor: sliderOptions.daily.min,
                    ceil: sliderOptions.daily.max,
                    onChange: function (sliderId, modelValue, highValue, pointerType) {
                        $scope.tempNewsFeed.recurrence_to = modelValue;
                    }
                };
                $scope.slider.fromOptions = {
                    floor: sliderOptions.daily.min,
                    ceil: sliderOptions.daily.max,
                    onChange: function (sliderId, modelValue, highValue, pointerType) {
                        $scope.tempNewsFeed.recurrence_from = modelValue;
                    }
                };
                break;
            case "S_CMSRecurrencePeriod_weekly":

                $scope.slider.to = sliderOptions.weekly.data[$scope.tempNewsFeed.recurrence_to];
                $scope.slider.from = sliderOptions.weekly.data[$scope.tempNewsFeed.recurrence_from];
                $scope.slider.toOptions = {
                    stepsArray: sliderOptions.weekly.data,
                    onChange: function (sliderId, modelValue, highValue, pointerType) {
                        $scope.tempNewsFeed.recurrence_to = sliderOptions.weekly.data.indexOf(modelValue);
                    }
                };
                $scope.slider.fromOptions = {
                    stepsArray: sliderOptions.weekly.data,
                    onChange: function (sliderId, modelValue, highValue, pointerType) {
                        $scope.tempNewsFeed.recurrence_from = sliderOptions.weekly.data.indexOf(modelValue);;
                    }
                };
                break;
            case "S_CMSRecurrencePeriod_monthly":
                $scope.slider.to = $scope.tempNewsFeed.recurrence_to;
                $scope.slider.from = $scope.tempNewsFeed.recurrence_from;
                $scope.slider.toOptions = {
                    floor: sliderOptions.monthly.min,
                    ceil: sliderOptions.monthly.max,
                    onChange: function (sliderId, modelValue, highValue, pointerType) {
                        $scope.tempNewsFeed.recurrence_to = modelValue;
                    }
                };
                $scope.slider.fromOptions = {
                    floor: sliderOptions.monthly.min,
                    ceil: sliderOptions.monthly.max,
                    onChange: function (sliderId, modelValue, highValue, pointerType) {
                        $scope.tempNewsFeed.recurrence_from = modelValue;
                    }
                };
                break;
            case "S_CMSRecurrencePeriod_yearly":

                $scope.slider.to = sliderOptions.yearly.data[$scope.tempNewsFeed.recurrence_to];
                $scope.slider.from = sliderOptions.yearly.data[$scope.tempNewsFeed.recurrence_from];
                $scope.slider.toOptions = {
                    stepsArray: sliderOptions.yearly.data,
                    onChange: function (sliderId, modelValue, highValue, pointerType) {
                        $scope.tempNewsFeed.recurrence_to = sliderOptions.yearly.data.indexOf(modelValue);
                    }
                };
                $scope.slider.fromOptions = {
                    stepsArray: sliderOptions.yearly.data,
                    onChange: function (sliderId, modelValue, highValue, pointerType) {
                        $scope.tempNewsFeed.recurrence_from = sliderOptions.yearly.data.indexOf(modelValue);;
                    }
                };
                break;
        }

    });

    function dateChanged() {
        console.log("Date from: " + $scope.date_from);
    }
}]);