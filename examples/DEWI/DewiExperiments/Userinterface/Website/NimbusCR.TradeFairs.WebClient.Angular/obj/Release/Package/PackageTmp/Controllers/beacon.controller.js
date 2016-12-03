'use strict';
/*
 * Contains a controller to do the user login
 */
angular.module('app.controllers').controller('BeaconController', ['$scope', 'BeaconService', 'PaginationService', 'StoreService', 'BoothService',
function ($scope, BeaconService, PaginationService, StoreService, BoothService) {

    var beacons = BeaconService;
    var pagination = PaginationService;
    var stores = StoreService;
    var booths = BoothService;

    $scope.boothList = { booths: null };
    $scope.boothList = booths.getBooths();
    $scope.storeList = { stores: null };
    $scope.storeList = stores.getStores();
    $scope.beaconList = { beacons: null };
    $scope.beaconList = beacons.getBeacons();
    $scope.StepObject = beacons.getSteps();

    $scope.SortTypeArray = ["name", "subsystem uuid"];
    $scope.sortType = 'name'; // set the default sort type
    $scope.sortReverse = false;
    $scope.changeSortType = function (sortType, sortReverse) {
        $scope.sortType = sortType;
        if (sortReverse == 1)
        {
            $scope.sortReverse = !$scope.sortReverse
        }
    }

    $scope.activeTab = 1;
    $scope.setActiveTab = function (tabToSet) { $scope.activeTab = tabToSet; };
    $scope.editTab = "tab1";
    $scope.setEditTab = function (name) { $scope.editTab = name; };

    $scope.nameInput = function () {
        if ($scope.tempBeacon.name != undefined)
            $scope.isValid = true;
        else
            $scope.isValid = false;

    }
    $scope.isOutOfDate = function (item) {
        if (item.outOfDate === undefined)
            return false;
        else
            return item.outOfDate;
    }
    $scope.updateBeacon = function () {
        if ($scope.isNewBeacon == true)
            beacons.addBeacon($scope.editBeacon);
        else
            beacons.updateBeacon($scope.editBeacon);
    };
    $scope.deleteBeacon = function () {
        beacons.deleteBeacon($scope.tempBeacon);
    };

    $scope.addEditBeaconSetup = function (beacon, num) {
        if (num == 1) {
            $scope.tempBeacon = new S_CMSBeaconJS();
            $scope.tempBeacon.list_item = new S_CMSListItemJS();
            $scope.beaconOption = 1;
        }
        else {
            $scope.tempBeacon = angular.copy(beacon);
            $scope.beaconOption = 2;
        };
    };
    $scope.tempBeacon = new S_CMSBeaconJS();
    $scope.tempBeacon.list_item = new S_CMSListItemJS();
    $scope.beaconOption = 1;
    $scope.addUpdateBeacon = function () {
        if ($scope.beaconOption == 1) {
            if ($scope.tempBeacon.name != "") {
                beacons.addBeacon($scope.tempBeacon);
            };
        }
        else {
            beacons.updateBeacon($scope.tempBeacon);
        };
    };
    $scope.deleteBeaconModal = function (beacon) {
        $("#deleteBeaconModal").modal("toggle");
        $scope.tempBeacon = angular.copy(beacon);
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

    $scope.$watch('beaconList.beacons.length', function () {
        if ($scope.beaconList.beacons != null) {
            pagination.setDetails("Beacon");
        };
    });

   
}]);