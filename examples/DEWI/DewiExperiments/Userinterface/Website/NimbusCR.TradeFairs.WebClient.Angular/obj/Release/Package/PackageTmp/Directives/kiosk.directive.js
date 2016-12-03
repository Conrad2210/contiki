'use strict';

angular.module('app.directives').directive('kioskDirective', function () {

    return {
        templateUrl: "Templates/Kiosk/kiosk.html",
        controller: "KioskController"
    }
});

