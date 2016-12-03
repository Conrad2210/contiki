'use strict';

angular.module('app.directives').directive('beaconDirective', function () {

    return {
        templateUrl: "Templates/Beacon/beacon.html",
        controller: "BeaconController"
    }
});


//Following driectives are nested in storedirective so no controller is added as of yet. 
angular.module('app.directives').directive('beaconListDirective', function () {

    return {
        //No controller added to this yet
        templateUrl: "Templates/Beacon/beaconlist.html"
    }
});
//Following driectives are nested in storedirective so no controller is added as of yet. 
angular.module('app.directives').directive('beaconEditDirective', function () {

    return {
        //No controller added to this yet
        templateUrl: "Templates/Beacon/beacon_edit.html"
    }
});
