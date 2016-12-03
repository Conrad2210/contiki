'use strict';

angular.module('app.directives').directive('storeDirective', function () {

    return {
        templateUrl: "Templates/Store/store.html",
        controller: "StoreController"
    }
});


//Following driectives are nested in storedirective so no controller is added as of yet. 
angular.module('app.directives').directive('storeListDirective', function () {

    return {
        //No controller added to this yet
        templateUrl: "Templates/Store/storelist.html"
    }
});

//Following driectives are nested in storedirective so no controller is added as of yet. 
angular.module('app.directives').directive('storeCardDirective', function () {

    return {
        //No controller added to this yet
        templateUrl: "Templates/Store/storecard.html"
    }
});
//Following driectives are nested in storedirective so no controller is added as of yet. 
angular.module('app.directives').directive('storeEditDirective', function () {

    return {
        //No controller added to this yet
        templateUrl: "Templates/Store/store_edit.html"
    }
});
angular.module('app.directives').directive('storeDetailsDirective', function () {

    return {
        //No controller added to this yet
        templateUrl: "Templates/Store/store_details.html"
    }
});
