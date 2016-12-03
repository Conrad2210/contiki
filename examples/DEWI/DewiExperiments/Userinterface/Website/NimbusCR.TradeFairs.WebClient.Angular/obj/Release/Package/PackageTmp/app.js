/*
* This is the main module of the app.
* Used as a center point to inject all other modules( Controllers, Services, Directives, etc... )
*/
'use strict';
angular.module("cmsApp", ['ngRoute', 'app.services', 'app.controllers', 'app.directives', 'app.routes', 'ClientDB', 'SignalR', 'ngCkeditor', 'ngSanitize', 'flow', 'naif.base64', 'angular-inview', 'ui.bootstrap.datetimepicker', 'base64', 'rzModule'])
	.run(['$', '$rootScope', '$location', function ($, $rootScope, $location) {

	}]);