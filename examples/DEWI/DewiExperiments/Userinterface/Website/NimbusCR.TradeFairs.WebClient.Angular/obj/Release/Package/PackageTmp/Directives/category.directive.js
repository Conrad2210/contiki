'use strict';

//Returns filter nav bar for main display of zones, booths, beacons
angular.module('app.directives').directive('categoryDirective', function () {

    return {
        //No controller added to this yet
        templateUrl: "Templates/Category/category.html",
        controller: "CategoryController"
    }
});

//Following driectives are nested in storedirective so no controller is added as of yet. 
angular.module('app.directives').directive('categoryListDirective', function () {

    return {
        //No controller added to this yet
        templateUrl: "Templates/Category/categorylist.html"
    }
});
//Following driectives are nested in storedirective so no controller is added as of yet. 
angular.module('app.directives').directive('categoryEditDirective', function () {

    return {
        //No controller added to this yet
        templateUrl: "Templates/Category/category_edit.html"
    }
});
