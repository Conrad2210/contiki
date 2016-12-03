'use strict';
/*
 * Contains a controller to do the user login
 */
angular.module('app.controllers').controller('LoginController', ['$scope', '$location', 'LoginService', 'ProfileService',
function ($scope, $location, LoginService,  ProfileService) {

    var loginService = LoginService;

    $scope.username = "";
    $scope.password = "";
    $scope.loginStatus = { status: false };
    $scope.loginStatus = loginService.getLoginStatus();
    $scope.cookieBanner = { status: true };
    $scope.cookieLogin = loginService.getCookieStatus();
    $scope.loginFail = { failed: false };
    $scope.loginFail = loginService.getLoginFail();
    
    $scope.$watch('loginStatus.status', function () {
        if ($scope.loginStatus.status == true || $scope.cookieLogin.status) {
            loginService.setLoginFail();
            $location.path('/index');
        }
        else {
            $location.path('/');
        };
    });

    $scope.userLogin = function () {
        loginService.login($scope.username, $scope.password);
        //$scope.password = "";
    };


    $scope.hideCookieBanner = function () {
        $scope.cookieBanner.status = false;
    };

    //When login page is loaded it checks for cookie that hides/shows cookie policy
    //banner. cookie hold details on whether user wants to see the banner info
    $scope.cookieSettings = $.cookie('myCookie');
    if ($scope.cookieSettings == null) {
        $scope.cookieBanner.status = true;
    }
    else if ($scope.cookieSettings == "No") {
        $scope.cookieBanner.status = false;
    };

    //Allows user to choose if they want to see cookie policy again
    $scope.dontShowCookieBanner = function () {
        $.cookie('myCookie', 'No');
    };

    
    $scope.checkLogin = function () {
        if ($scope.cookieLogin.status) {
            $location.path('/index');
        };
    };

    $scope.checkLogin();
}]);

