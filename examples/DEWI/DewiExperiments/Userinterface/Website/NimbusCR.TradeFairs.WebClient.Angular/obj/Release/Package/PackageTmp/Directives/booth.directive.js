'use strict';

angular.module('app.directives').directive('boothDirective', function () {

    return {
        templateUrl: "Templates/Booth/booth.html",
        controller: "BoothController"
    }
});


//Following driectives are nested in storedirective so no controller is added as of yet. 
angular.module('app.directives').directive('boothListDirective', function () {

    return {
        //No controller added to this yet
        templateUrl: "Templates/Booth/boothlist.html"
    }
});
//Following driectives are nested in storedirective so no controller is added as of yet. 
angular.module('app.directives').directive('boothEditDirective', function () {

    return {
        //No controller added to this yet
        templateUrl: "Templates/Booth/booth_edit.html"
    }
});
