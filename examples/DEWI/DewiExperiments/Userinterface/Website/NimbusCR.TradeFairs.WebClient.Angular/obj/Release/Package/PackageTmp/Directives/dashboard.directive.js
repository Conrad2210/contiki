'use strict';

angular.module('app.directives').directive('dashboardDirective', function () {

    return {
        templateUrl: "Templates/Dashboard/dashboard.html",
        controller: "DashboardController"
    }
});


//Following driectives are nested in storedirective so no controller is added as of yet. 
angular.module('app.directives').directive('dashboardListDirective', function () {

    return {
        //No controller added to this yet
        templateUrl: "Templates/Dashboard/dashboardlist.html"
    }
});

//Following driectives are nested in storedirective so no controller is added as of yet. 
angular.module('app.directives').directive('dashboardCardDirective', function () {

    return {
        //No controller added to this yet
        templateUrl: "Templates/Dashboard/dashboardcard.html"
    }
});
