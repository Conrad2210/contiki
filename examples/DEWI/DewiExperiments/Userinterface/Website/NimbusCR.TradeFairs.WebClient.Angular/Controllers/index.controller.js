'use strict';
/*
 * Contains a controller to do the user login
 */
angular.module('app.controllers').controller('IndexController', ['$scope', '$indexedDB', '$location',
function ($scope, $indexedDB, $location) {
      
   

    //Following is to show/hide sidebar
    $scope.showSidebar = true;

    //Used to switch the content display area chosen by the user ( Store, Category, etc...)
    $scope.displaySelection = "Default"
    $scope.changedDispaySelection = function (choice) {
        $scope.displaySelection = choice;
    };

   

}]);

