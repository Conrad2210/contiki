'use strict';
/*
 * Contains a controller to do the user login
 */
angular.module('app.controllers').controller('SidebarController', ['$scope', '$location', 'ProfileService',
function ($scope, $location, ProfileService) {

    var profiles = ProfileService;

    $scope.userProfile = { profile: null };
    $scope.userProfile = profiles.getProfile();
    var profileImgChanged = false;
    $scope.emptyProfileImage = { status: null };
    $scope.emptyProfileImage = profiles.getProfileImageStatus();

    //Tabs to navigate between details for adding, editing a store in modals
    $scope.activeTab = 1;
    $scope.setActiveTab = function (tabToSet) { $scope.activeTab = tabToSet; };
    $scope.editTab = "tab1";
    $scope.setEditTab = function (name) { $scope.editTab = name; };

    //Store image upload for a new + existing store
    $scope.previewProfileImg = false;
    $scope.showProfileImg = true;
    $scope.profileImg = {};
    $scope.tempProfile = {};
    
    $scope.onLoad = function (e, reader, file, fileList, fileOjects, fileObj) {
        $scope.profileImg = fileObj;
        $scope.previewProfileImg = true;
    };
    $scope.deleteProfileImage = function () {
        $scope.emptyProfileImage.status = true;
        $scope.tempProfile.image_string = "delete";
        $scope.showProfileImg = false;
        profileImgChanged = true;
    }
    $scope.changeProfileImage = function () {
        $scope.emptyProfileImage.status = false;
        $scope.tempProfile.image_string = angular.copy($scope.profileImg.base64);
        $scope.userProfile.profile.image_string = angular.copy($scope.profileImg.base64);
        $scope.showProfileImg = true;
        $scope.previewProfileImg = false;
        $scope.profileImg.base64 = null;
        $scope.profileImg = null;
        profileImgChanged = true;
    };
    $scope.discardUploadedImage = function () {
        $scope.previewProfileImg = false;
        $scope.profileImg.base64 = null;
        $scope.profileImg = null;
    };

    $scope.updateProfile = function (profile) {
        if (profileImgChanged) {
            profiles.updateProfile(profile);
            profileImgChanged = false;
        }
        else {
            profile.image_string = "";
            profiles.updateProfile(profile);
        };
    };

    //Profile pop up modal
    $scope.profileModal = function () {
        $("#profileModal").modal("toggle");
        $scope.showProfileImg = true;
        $scope.tempProfile = angular.copy($scope.userProfile.profile);
    };

}]);

