'use strict';

angular.module('app.directives').directive('newsfeedDirective', function () {

    return {
        templateUrl: "Templates/NewsFeed/newsFeed.html",
        controller: "NewsFeedController"
    }
});


//Following driectives are nested in storedirective so no controller is added as of yet. 
angular.module('app.directives').directive('newsfeedListDirective', function () {

    return {
        //No controller added to this yet
        templateUrl: "Templates/NewsFeed/newsFeedlist.html"
    }
});

//Following driectives are nested in storedirective so no controller is added as of yet. 
angular.module('app.directives').directive('newsfeedCardDirective', function () {

    return {
        //No controller added to this yet
        templateUrl: "Templates/NewsFeed/newsFeedcard.html"
    }
});
//Following driectives are nested in storedirective so no controller is added as of yet. 
angular.module('app.directives').directive('newsfeedEditDirective', function () {

    return {
        //No controller added to this yet
        templateUrl: "Templates/NewsFeed/newsFeed_edit.html"
    }
});
//Following driectives are nested in storedirective so no controller is added as of yet. 
angular.module('app.directives').directive('newsfeedDetailsDirective', function () {

    return {
        //No controller added to this yet
        templateUrl: "Templates/NewsFeed/newsFeed_details.html"
    }
});
