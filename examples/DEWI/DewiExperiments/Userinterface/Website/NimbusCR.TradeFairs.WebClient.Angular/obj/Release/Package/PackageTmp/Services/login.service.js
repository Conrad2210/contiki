'use strict';
/*
 * Contains a service to communicate with the TRACK TV API
 */
angular.module('app.services').factory('LoginService', ["$rootScope", "$location", "$timeout", "cDB", "CoreHubService",
	function ($rootScope, $location, $timeout, cDB, CoreHubService) {
	    var self = this;
	    var hub = CoreHubService;
	    var clientDB = cDB;

	    var cookieLogin = { status: $.cookie("mapUmeLoggedIn") };
	    var loginStatus = { status: false };
	    var loginFail = { failed: false };

	    
	    $rootScope.$on("loggedIn", successLogin);
	    $rootScope.$on("TradeFairs_FailedLogin", failedLogin);
	    

	    this.getLoginStatus = function () {
	        return loginStatus;
	    };
	    this.setLoginStatus = function (status) {
	        loginStatus.status = status;
	    }
	    this.getCookieStatus = function () {
	        return cookieLogin;
	    };
	    this.getLoginFail = function () {
	        return loginFail;
	    };
	    this.setLoginFail = function () {
	        loginFail.failed = false;
	    };

	    this.login = function (username, password) {
	        clientDB.addCredentials({ "username": username, "password": password });
	        hub.ClientLogin(username, password);
	    };

	    this.logout = function () {
	        console.log("User has logged out");
	        clientDB.deleteDatabase();
	        window.localStorage.clear();
	        loginStatus.status = false;
	        $.cookie("mapUmeLoggedIn", false, { path: '/', expires: -1 });
	        $("#body").toggleClass("toggled"); //Change backgrounf from image to matching index colour
	        cookieLogin.status = false;
	    }

	    //callback functions from the hub service
	    function failedLogin(event, args) {
	        console.log("Login has failed");
	        loginFail.failed = true;
	        loginStatus.status = false;
	        $.cookie("mapUmeLoggedIn", false);
	        cookieLogin.status = false;
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }

	    };
	    
	    function successLogin() {
	        loginFail.failed = false;
	        loginStatus.status = true;
	        $.cookie("mapUmeLoggedIn", true);
	        cookieLogin.status = true;
	        $("#body").toggleClass("toggled"); //Change backgrounf from image to matching index colour
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    return self;
	}]);
