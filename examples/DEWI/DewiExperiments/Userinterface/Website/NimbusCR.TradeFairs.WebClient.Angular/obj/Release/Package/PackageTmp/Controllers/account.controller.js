'use strict';
/*
 * Contains a controller to do the user login
 */
angular.module('app.controllers').controller('AccountController', ['$scope', 'AccountService', 'PaginationService',
function ($scope, AccountService, PaginationService) {
    var account = AccountService;
    var pagination = PaginationService;


    $scope.SortTypeArray = ["username", "firstname", "lastname"];
    $scope.sortType = 'username'; // set the default sort type
    $scope.sortReverse = false;
    $scope.changeSortType = function (sortType, sortReverse) {
        $scope.sortType = sortType;
        if (sortReverse == 1) {
            $scope.sortReverse = !$scope.sortReverse
        }
    }

    $scope.accountList = { account: [] };
    $scope.accountList = account.getAccount();
    $scope.$watch('accountList.account.length', function () {
        if ($scope.accountList.account != null) {
            pagination.setDetails("Account");
        };
    });

    $scope.accountDetailsShow = function (accountObject) {
        $scope.accountDetails = angular.copy(accountObject);   //Sets up store object to be edited
    };

    $scope.deleteAccountModal = function (accountObject) {
        $("#deleteAccountModal").modal("toggle");
        $scope.tempAccount = angular.copy(accountObject);
    };
    
    $scope.deleteAccount = function () {
        account.deleteAccount($scope.tempAccount);
    };
    $scope.addEditAccountSetup = function (accountObject, num) {
        console.log(account);
        $scope.statuss = account.statuss;
        $scope.admintypes = account.admintypes;
        if (num == 1) {
            $scope.tempAccount = new S_CMSAccountJS();
            $scope.tempAccount.list_item = new S_CMSListItemJS();
            $scope.isNewAccount = true;
            $scope.accountOption = 1;
            $scope.showAccountImg = false;

        }
        else {
            $scope.tempAccount = angular.copy(accountObject);
            $scope.showAccountImg = true;
            $scope.isNewAccount = false;
            $scope.accountOption = 2;
        };
    };

    $scope.previewAccountImg = false;
    $scope.showAccountImg = true;
    $scope.accountImg = null;
    $scope.onLoad = function (e, reader, file, fileList, fileOjects, fileObj) {
        $scope.accountImg = fileObj;
        $scope.previewAccountImg = true;
    };
    $scope.deleteAccountImage = function () {
        if ($scope.accountOption == 1) {
            $scope.tempAccount.image_string = "";
            $scope.showAccountImg = false;
        }
        else {
            $scope.tempAccount.image_string = "delete";
            $scope.showAccountImg = false;
        };
    };
    $scope.changeAccountImage = function () {
        $scope.tempAccount.image_string = angular.copy("data:image/png;base64," + $scope.accountImg.base64);
        $scope.showAccountImg = true;
        $scope.previewAccountImg = false;
        $scope.accountImg.base64 = null;
        $scope.accountImg = null;
    };
    $scope.discardUploadedImage = function () {
        $scope.previewAccountImg = false;
        $scope.accountImg.base64 = null;
        $scope.accountImg = null;
    };

    $scope.addUpdateAccount = function () {
        if ($scope.accountOption == 1) {
            if ($scope.tempAccount.username != "") {
                account.addAccount($scope.tempAccount);
            };
        }
        else {
            account.updateAccount($scope.tempAccount);
        };
    };




    //Area for all Pagination details
    $scope.paginationDetails = {};
    $scope.paginationDetails = pagination.getDetails();
    $scope.changePageNum = function (number) {
        pagination.changePageNum(number, "Account");
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

    //This gets called when store comes into view and automatically loads
    //the image from server
    $scope.loadAccountImage = function (accountObject, isVisible) {
        account.loadAccountImage(accountObject, isVisible);
    };
}]);