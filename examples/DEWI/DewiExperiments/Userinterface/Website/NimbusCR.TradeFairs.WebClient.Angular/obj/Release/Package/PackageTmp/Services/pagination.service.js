'use strict';

angular.module('app.services').factory('PaginationService', ['AccountService', 'BoothService', 'BeaconService', 'CategoryService', 'NewsFeedService', 'StoreService',
    function (AccountService, BoothService, BeaconService, CategoryService, NewsFeedService, StoreService) {
        var self = this;
        var account = AccountService;
        var beacons = BeaconService;
        var booths = BoothService;
        var categories = CategoryService;
        var newsFeeds = NewsFeedService;
        var stores = StoreService;

        var accountList = account.getAccount();
        var beaconList = beacons.getBeacons();
        var boothList = booths.getBooths();
        var categoryList = categories.getCategories();
        var storeList = stores.getStores();
        var newsFeedList = newsFeeds.getNewsFeeds();
        var paginationDetails = {
            pageSize: [10, 20, 30, 40, 50],
            totalPages: 0,
            pageNum: 1,
            displayNum: 10,
            startNum: 0,
            endNum: 10
        };

        this.getDetails = function () {
            return paginationDetails;
        };

        this.setDetails = function (listType) {
            paginationDetails.displayNum = 10;
            paginationDetails.pageNum = 1;
            paginationDetails.startNum = 0;
            paginationDetails.endNum = paginationDetails.displayNum;
            
            switch(listType){
                case "Account":
                    paginationDetails.totalPages = Math.ceil(accountList.account.length / paginationDetails.displayNum);
                    break;
                case "Beacon":
                    paginationDetails.totalPages = Math.ceil(beaconList.beacons.length / paginationDetails.displayNum);
                    break;
                case "Booth":
                    paginationDetails.totalPages = Math.ceil(boothList.booths.length / paginationDetails.displayNum);
                    break;
                case "Category":
                    paginationDetails.totalPages = Math.ceil(categoryList.categories.length / paginationDetails.displayNum);
                    break;
                case "NewsFeed":
                    paginationDetails.totalPages = Math.ceil(newsFeedList.newsFeeds.length / paginationDetails.displayNum);
                    break;
                case "Store":
                    paginationDetails.totalPages = Math.ceil(storeList.stores.length / paginationDetails.displayNum);
                    break;
            }
        };

        this.changePageNum = function (num, listType) {
            paginationDetails.displayNum = num;
            paginationDetails.pageNum = 1;
            paginationDetails.startNum = 0;
            paginationDetails.endNum = paginationDetails.displayNum;
            switch (listType) {
                case "Account":
                    paginationDetails.totalPages = Math.ceil(accountList.account.length / paginationDetails.displayNum);
                    break;
                case "Beacon":
                    paginationDetails.totalPages = Math.ceil(beaconList.beacons.length / paginationDetails.displayNum);
                    break;
                case "Booth":
                    paginationDetails.totalPages = Math.ceil(boothList.booths.length / paginationDetails.displayNum);
                    break;
                case "Category":
                    paginationDetails.totalPages = Math.ceil(categoryList.categories.length / paginationDetails.displayNum);
                    break;
                case "NewsFeed":
                    paginationDetails.totalPages = Math.ceil(newsFeedList.newsFeeds.length / paginationDetails.displayNum);
                    break;
                case "Store":
                    paginationDetails.totalPages = Math.ceil(storeList.stores.length / paginationDetails.displayNum);
                    break;
            }
        };

        this.firstPage = function () {
            paginationDetails.pageNum = 1;
            paginationDetails.startNum = (paginationDetails.pageNum * paginationDetails.displayNum) - paginationDetails.displayNum;
            paginationDetails.endNum = (paginationDetails.pageNum * paginationDetails.displayNum);
        };

        this.lastPage = function () {
            paginationDetails.pageNum = paginationDetails.totalPages;
            paginationDetails.startNum = (paginationDetails.pageNum * paginationDetails.displayNum) - paginationDetails.displayNum;
            paginationDetails.endNum = (paginationDetails.pageNum * paginationDetails.displayNum);
        };

        this.nextPage = function () {
            if (paginationDetails.pageNum < paginationDetails.totalPages) {
                paginationDetails.pageNum++;
                paginationDetails.startNum = (paginationDetails.pageNum * paginationDetails.displayNum) - paginationDetails.displayNum;
                paginationDetails.endNum = (paginationDetails.pageNum * paginationDetails.displayNum);
            }
        };

        this.prevPage = function () {
            if (paginationDetails.pageNum > 1) {
                paginationDetails.pageNum--;
                paginationDetails.startNum = (paginationDetails.pageNum * paginationDetails.displayNum) - paginationDetails.displayNum;
                paginationDetails.endNum = (paginationDetails.pageNum * paginationDetails.displayNum);
            }
        };

        return self;
    }]);
