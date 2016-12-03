'use strict';
/*
 * Contains a service to communicate with the TRACK TV API
 */
angular.module('app.services').factory('ResultsService', ['$rootScope','$websocket',
	function ($rootScope,$websocket) {
	    var self = this;
	    try{
	        var dataStream = $websocket('ws://localhost:8081/ws');
	    }catch(err)
	    {
	        console.log("[ResultsService]: " + err);
	    }
	    return self;
	}]);
