'use strict';
/*
 * Contains a controller to do the user login
 */
angular.module('app.controllers').controller('DashboardController', ['$scope','DashboardService','AccountService', 'StoreService', 'CategoryService', 'PaginationService','BoothService','BeaconService','NewsFeedService',
function ($scope, DashboardService, AccountService, StoreService, CategoryService, PaginationService, BoothService, BeaconService, NewsFeedService) {
    var dashboard = DashboardService;
    var account = AccountService;
    var stores = StoreService;
    var categories = CategoryService;
    var pagination = PaginationService;
    var booths = BoothService;
    var beacons = BeaconService;
    var newsFeeds = NewsFeedService;


    $scope.accountList = { account: [] };
    $scope.accountList = account.getAccount();
    $scope.storeList = { stores: [] };
    $scope.storeList = stores.getStores();
    $scope.categoryList = { categories: [] };
    $scope.categoryList = categories.getCategories();
    $scope.boothList = { booths: [] };
    $scope.boothList = booths.getBooths();
    $scope.beaconList = { beacons: [] };
    $scope.beaconList = beacons.getBeacons();
    $scope.newsFeedList = { newsFeeds: [] };
    $scope.newsFeedList = newsFeeds.getNewsFeeds();

    console.log($scope);
    //This gets called when store comes into view and automatically loads
    //the image from server

    $scope.loadStoreImage = function (store, isVisible) {
        stores.loadStoreImage(store, isVisible);
    };
    $scope.loadAccountImage = function (accountObject, isVisible) {
        account.loadAccountImage(accountObject, isVisible);
    };
    $scope.loadNewsFeedImage = function (newsFeedObject, isVisible) {
        newsFeeds.loadNewsFeedImage(newsFeedObject, isVisible);
    };
}]);