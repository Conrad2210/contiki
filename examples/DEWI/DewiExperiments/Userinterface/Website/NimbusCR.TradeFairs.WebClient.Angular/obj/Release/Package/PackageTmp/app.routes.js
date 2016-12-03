/*
* This is the control module for the app navigation
* Basic layout below but controllers, linking, scope inheritance call all be
* set in this module
*/

'use strict';
angular.module('app.routes', ['ngRoute'])

.config(['$routeProvider', function ($routeProvider) {
    $routeProvider

        .when('/', {
            templateUrl: 'Templates/login.html',
            controller: "LoginController",

        })
        .when('/index', {
            templateUrl: 'Templates/index.html',
            controller: "IndexController",
        })
        .when('/cookie_policy', {
            templateUrl: 'Templates/cookieinfo.html',
            controller: "CookieController",
        })
		.otherwise({
		    redirectTo: '/'
		});
}]);