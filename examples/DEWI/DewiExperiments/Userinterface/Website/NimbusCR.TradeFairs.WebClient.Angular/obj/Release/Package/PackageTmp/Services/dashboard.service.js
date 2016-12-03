'use strict';
/*
 * Contains a service to communicate with the TRACK TV API
 */

angular.module('app.services').factory('DashboardService', ['$rootScope', 'cDB', 'CoreHubService',
	function ($rootScope, cDB, CoreHubService) {
	    var self = this;
	    var clientDB = cDB;
	    var hub = CoreHubService;


	    return self;
	}]);