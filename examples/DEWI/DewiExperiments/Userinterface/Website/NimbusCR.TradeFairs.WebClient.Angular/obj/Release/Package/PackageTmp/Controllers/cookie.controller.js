'use strict';
/*
 * Contains a controller to do the user login
 */
angular.module('app.controllers').controller('CookieController', ['$scope', '$location', '$anchorScroll', 'LoginService',
function ($scope, $location, $anchorScroll, LoginService) {

    var loginService = LoginService;

    $scope.username = "";
    $scope.password = "";
    $scope.loginStatus = { status: false };
    $scope.loginStatus = loginService.getLoginStatus();
    $scope.loginFail = { failed: false };
    $scope.loginFail = loginService.getLoginFail();

    $scope.$watch('loginStatus.status', function () {
        if ($scope.loginStatus.status == true ) {
            $location.path('/index');
        }
    });
    $scope.$watch('loginFail.failed', function () {
        if ($scope.loginFail.failed == true) {
            $location.path('/');
        };
    });

    $scope.userLogin = function () {
        loginService.login($scope.username, $scope.password);
        $scope.password = "";
    };

    $scope.gotoSection = function (name) {
        // set the location.hash to the id of
        // the element you wish to scroll to.
        $location.hash(name);

        // call $anchorScroll()
        $anchorScroll();
    };

}]);

