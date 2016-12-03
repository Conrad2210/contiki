'use strict';
/*
 * Contains a controller to do the user login
 */
angular.module('app.controllers').controller('BoothController', ['$scope', 'BoothService', 'PaginationService',
function ($scope, BoothService,  PaginationService) {

    var booths = BoothService;
    var pagination = PaginationService;

    $scope.boothList = { booths: null };
    $scope.boothList = booths.getBooths();
    $scope.StepObject = booths.getSteps();

    $scope.SortTypeArray = ["booth", "hall"];
    $scope.sortType = 'booth'; // set the default sort type
    $scope.sortReverse = false;
    $scope.changeSortType = function (sortType, sortReverse) {
        $scope.sortType = sortType;
        if (sortReverse == 1)
        {
            $scope.sortReverse = !$scope.sortReverse
        }
    }

    $scope.nameInput = function () {
        if ($scope.tempBooth.booth != undefined)
            $scope.isValid = true;
        else
            $scope.isValid = false;

    }

    $scope.updateBooth = function () {
        if ($scope.isNewBooth == true)
            booths.addBooth($scope.editBooth);
        else
            booths.updateBooth($scope.editBooth);
    };
    $scope.deleteBooth = function () {
        booths.deleteBooth($scope.tempBooth);
    };

    $scope.addEditBoothSetup = function (booth, num) {
        if (num == 1) {
            $scope.tempBooth = new S_CMSBoothJS();
            $scope.tempBooth.list_item = new S_CMSListItemJS();
            $scope.boothOption = 1;
        }
        else {
            $scope.tempBooth = angular.copy(booth);
            $scope.boothOption = 2;
        };
    };
    $scope.tempBooth = new S_CMSBoothJS();
    $scope.tempBooth.list_item = new S_CMSListItemJS();
    $scope.boothOption = 1;
    $scope.addUpdateBooth = function () {
        if ($scope.boothOption == 1) {
            if ($scope.tempBooth.booth != "") {
                booths.addBooth($scope.tempBooth);
            };
        }
        else {
            booths.updateBooth($scope.tempBooth);
        };
    };
    $scope.deleteBoothModal = function (booth) {
        $("#deleteBoothModal").modal("toggle");
        $scope.tempBooth = angular.copy(booth);
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

    $scope.$watch('boothList.booths.length', function () {
        if ($scope.boothList.booths != null) {
            pagination.setDetails("Booth");
        };
    });

   
}]);