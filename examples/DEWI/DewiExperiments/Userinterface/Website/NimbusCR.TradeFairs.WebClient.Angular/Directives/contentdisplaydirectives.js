'use strict';

//Returns filter nav bar for main display of zones, booths, beacons
angular.module('app.directives').directive('filterNavDisplayDirective', function(){
				
	return {
		//No controller added to this yet
		templateUrl: "Templates/filternavbar.html"
	}
});


/*
* Return the main dashboard display to the user
*/
angular.module('app.directives').directive('dashCardDisplayDirective', function () {

    return {
        controller: "DashboardController",
        templateUrl: "Templates/DashBoardDisplay/dashboardcarddisplay.html"
    }
});

angular.module('app.directives').directive('dashListDisplayDirective', function () {

    return {
        controller: "DashboardController",
        templateUrl: "Templates/DashBoardDisplay/dashboardlistdisplay.html"
    }
});

/*
* Return the zone display to the user
*/
angular.module('app.directives').directive('zoneCardDirective', function () {

    return {
        templateUrl: "Templates/Zone/zonecard.html"
    }
});

angular.module('app.directives').directive('zoneListDirective', function () {

    return {
        templateUrl: "Templates/Zone/zonelist.html"
    }
});

/*
* Return the booth display to the user
*/
angular.module('app.directives').directive('boothCardDirective', function () {

    return {
        controller: 'BoothController',
        templateUrl: "Templates/Booth/boothcard.html"
    }
});

angular.module('app.directives').directive('boothListDirective', function () {

    return {
        controller: 'BoothController',
        templateUrl: "Templates/Booth/boothlist.html"
    }
});

/*
* Return the store display to the user
*/
angular.module('app.directives').directive('storeCardDirective', function () {

    return {
        controller: 'StoreController',
        templateUrl: "Templates/Store/storecard.html"
    }
});

angular.module('app.directives').directive('storeListDirective', function () {

    return {
        controller: 'StoreController',
        templateUrl: "Templates/Store/storelist.html"
    }
});

/*
* Return the beacon display to the user
*/
angular.module('app.directives').directive('beaconCardDirective', function () {

    return {
        controller: 'BeaconController',
        templateUrl: "Templates/Beacon/beaconcard.html"
    }
});

angular.module('app.directives').directive('beaconListDirective', function () {

    return {
        controller: 'BeaconController',
        templateUrl: "Templates/Beacon/beaconlist.html"
    }
});


/*
* Return the category display to the user
*/
angular.module('app.directives').directive('categoryCardDirective', function () {

    return {
        controller: 'CategoryController',
        templateUrl: "Templates/Category/categorycard.html"
    }
});

angular.module('app.directives').directive('categoryListDirective', function () {

    return {
        controller: 'CategoryController',
        templateUrl: "Templates/Category/categorylist.html"
    }
});


/*
* Return the news feed display to the user
*/
angular.module('app.directives').directive('newsfeedCardDirective', function () {

    return {
        controller: 'NewsFeedController',
        templateUrl: "Templates/NewsFeed/newsfeedcard.html"
    }
});

angular.module('app.directives').directive('newsfeedListDirective', function () {

    return {
        controller: 'NewsFeedController',
        templateUrl: "Templates/NewsFeed/newsfeedlist.html"
    }
});

/*
* Return the main directory section
*/
angular.module('app.directives').directive('directoryDirective', function () {

    return {
        templateUrl: "Templates/Directory/directory.html"
    }
});

angular.module('app.directives').directive('categoryDirective', function () {

    return {
        templateUrl: "Templates/Directory/category.html"
    }
});

angular.module('app.directives').directive('shopDirectoryDirective', function () {

    return {
        templateUrl: "Templates/Directory/shopdirectory.html"
    }
});

angular.module('app.directives').directive('addCatToShopDirective', function () {

    return {
        templateUrl: "Templates/Directory/addcategorytoshop.html"
    }
});

angular.module('app.directives').directive('storeEditorDirective', function () {

    return {
        templateUrl: "Templates/StoreEditor/storeeditor.html"
    }
});
