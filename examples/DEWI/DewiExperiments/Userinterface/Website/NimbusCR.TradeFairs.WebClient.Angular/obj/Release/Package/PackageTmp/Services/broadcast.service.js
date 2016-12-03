'use strict';
/*
 * Contains a service to communicate with the TRACK TV API
 */
angular.module('app.services').factory('BroadcastService', ['$rootScope', 'cDB', 'CoreHubService', 'AccountService', 'BoothService', 'BeaconService', 'CategoryService', 'NewsFeedService', 'StoreService',
	function ($rootScope, cDB, CoreHubService, AccountService, BoothService, BeaconService, CategoryService, NewsFeedService, StoreService) {
	    var self = this;
	    var clientDB = cDB;
	    var hub = CoreHubService;

	    var accountList = AccountService.getAccount();
	    var beaconList = BeaconService.getBeacons();
	    var boothList = BoothService.getBooths();
	    var categoryList = CategoryService.getCategories();
	    var storeList = StoreService.getStores();
	    var newsFeedList = NewsFeedService.getNewsFeeds();


	    var sub_ = {
	        subscriptionproperties: {},
	        topics: {
	            'SocialContext_SocialUpdateRequest': ['Context_ReloadSocialDB'], //to notify about: EOBooth, EOProfile, ProfileUserAccount, Profile_Category, EOProximity, EOSeminar, EOSpeaker, EOEventLocation, EOEventGroup, EOProduct
	            'UserUpdates': ['ReloadUserAccount'],//to notify about: UserAccount, UserAccountStatus
	            'Context_DataMiningUpdateRequest': ['Reload3DData'],//to notify about: EOSurface, Destination
	            'Context_DataMiningUpdateRequest': ['Context_ReloadDataMiningDB'],//to notify about: MoreItem
	            'TradeFairsUpdateRequest': ['TradeFairs_ReloadTradefairsDB']//to notify about: NewsFeed, Race, RaceResult, 
	        },
	        project_id: cDB.getCurrentProject(),
	    };

	    $rootScope.$on("Context_ReloadSocialDB", ReloadSocialDB);
	    $rootScope.$on("ReloadUserAccount", ReloadUserAccount);

	    $rootScope.$on("Reload3DData", Reload3DData);
	    $rootScope.$on("Context_ReloadDataMiningDB", Context_ReloadDataMiningDB);
	    $rootScope.$on("TradeFairs_ReloadTradefairsDB", TradeFairs_ReloadTradefairsDB);

	    function ReloadSocialDB(event, args) {
	        console.log("ReloadSocialDB");
	        var data = args.data;
	        var tempIDS = JSON.parse(JSON.stringify(data.object_dbids));
	        cDB.checkOutOfDate(data.object_dbids, data.project_id, data.object_type);
	        AccountService.setAccount(cDB.getCMSAccounts(cDB.getCurrentProject()));
	        accountList = AccountService.getAccount();
	        BeaconService.setBeacons(cDB.getCMSBeacons(cDB.getCurrentProject()));
	        beaconList = BeaconService.getBeacons();
	        BoothService.setBooths(cDB.getCMSBooths(cDB.getCurrentProject()));
	        boothList = BoothService.getBooths();
	        CategoryService.setCategories(cDB.getCMSStoreCategories(cDB.getCurrentProject()));
	        categoryList = CategoryService.getCategories();
	        StoreService.setStores(cDB.getCMSStores(cDB.getCurrentProject()));
	        storeList = StoreService.getStores();
	        //if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	        $rootScope.$apply();
	        $rootScope.$digest();
	    }

	    function ReloadUserAccount(event, args) {
	        console.log("ReloadUserAccount");
	    }

	    function Reload3DData(event, args) {
	        console.log("Reload3DData");
	    }

	    function Context_ReloadDataMiningDB(event, args) {
	        console.log("Context_ReloadDataMiningDB");
	    }

	    function TradeFairs_ReloadTradefairsDB(event, args) {
	        console.log("TradeFairs_ReloadTradefairsDB");
	        var data = args.data;
	        var tempIDS = JSON.parse(JSON.stringify(data.object_dbids));
	        cDB.checkCMSNewsFeedsOutOfDate(data.object_dbids, data.project_id);
	        NewsFeedService.setNewsFeeds(cDB.getCMSNewsFeed(cDB.getCurrentProject()));
	        newsFeedList = NewsFeedService.getNewsFeeds();
	        //if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	        $rootScope.$apply();
	        $rootScope.$digest();
	    }

	    this.subscribe = function () {
	        hub.SubscribeToBroadcast(sub_);
	    }



	    return self;
	}]);
