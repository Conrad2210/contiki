'use strict';

angular.module('app.directives').directive('resultsDirective', function () {

    return {
        templateUrl: "Templates/Results/results.html",
        controller: "ResultsController"
    }
});

