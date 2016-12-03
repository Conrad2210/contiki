'use strict';

angular.module('app.directives').directive('experimentsDirective', function () {

    return {
        templateUrl: "Templates/Experiments/experiments.html",
        controller: "ExperimentsController"
    }
});

