/*
* This is the control module for the app navigation
* Basic layout below but controllers, linking, scope inheritance call all be
* set in this module
*/

'use strict';
angular.module('app.routes', ['ngRoute'])

.config(['$routeProvider', function ($routeProvider) {
    $routeProvider
        .when('/index', {
            templateUrl: 'Templates/index.html',
            controller: "IndexController",
        })
		.otherwise({
		    redirectTo: '/index'
		});
}]);