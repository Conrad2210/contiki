'use strict';
/*
 * Contains a service to communicate with the TRACK TV API
 */
angular.module('app.services').factory('ProfileService', ['$rootScope', 'cDB', 'CoreHubService',
	function ($rootScope, cDB, CoreHubService) {
	    var self = this;
	    var clientDB = cDB;
	    var hub = CoreHubService;

	    $rootScope.$on("TradeFairs_GetCMSAccountByName", loadProfile);
	    $rootScope.$on("TradeFairs_GetCMSImage", loadProfileImage);
	    $rootScope.$on("TradeFairs_UpdateCMSAccount", updateUserProfile);

	    $rootScope.$on("setCookie", loadUserAccount);
	    $rootScope.$on("TradeFairs_UpdateCMSAccountLastLoginProject", updateProject);

	    var profileInfo = { profile: null };
	    var emptyProfileImage = { status: true };
	    var userAccount = { account: null };

	    this.setUserAccount = function (account) {
	        userAccount.account = account;
	    };

	    this.getUserAccount = function () {
	        return userAccount;
	    };

	    function loadUserAccount(event, args) {
	        clientDB.addUserAccount(args.data);
	        userAccount.account = args.data;
	        clientDB.addCurrentProject(userAccount.account.preferences.gui.default_project);
	        hub.GetCMSAccountsByName(clientDB.getUserName(), clientDB.getCurrentProject());
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    this.LastLoginProject = function () {
	        hub.UpdateCMSAccountLastLoginProject(clientDB.getUserName(), clientDB.getCurrentProject());
	    }
	    function updateProject(event, args) {
	        if (args.data) {
	            console.log("Last Login Project succesful stored");
	            window.location.reload();
	        } else {
	            console.log("Last Login Project could not be stored");
	        }
	    }
	    this.setProfile = function (profile) {
	        profileInfo.profile = profile;
	        if (profileInfo.profile.image_string != "") {
	            emptyProfileImage.status = false;
	        };
	    };

	    this.getProfile = function () {
	        return profileInfo;
	    };

	    this.setProfileImage = function (image) {
	        profileInfo.profile.image_string = image;
	    };

	    this.getProfileImageStatus = function () {
	        return emptyProfileImage;
	    };

	    this.updateProfile = function (profile) {
	        profile.status = "S_CMSProfileUserStatus_valid";//overwrite status from "S_CMSProfileUserStatus_unknown"+- 
	        if (profile.image_string == "delete") {
	            hub.UpdateCMSAccounts(profile, cDB.getCurrentProject());
	            profileInfo.profile.image_string = "";
	        }
	        else if (profile.image_string == "") {
	            hub.UpdateCMSAccounts(profile, cDB.getCurrentProject());
	            var temp = profileInfo.profile.image_string;
	            profileInfo.profile = angular.copy(profile);
	            profileInfo.profile.image_string = temp;
	        }
	        else {
	            profileInfo.profile = angular.copy(profile);
	            hub.UpdateCMSAccounts(profile, cDB.getCurrentProject());
	        };
	    };

	    function loadProfile(event, args) {
	        clientDB.addUserProfile(args.data);
	        profileInfo.profile = args.data;
	        if (args.data.image_string == "")
	            hub.GetCMSImage(args.data.image_url, "small");
	        else {
	            loadProfileImage("", { data: { image_string: args.data.image_str } });
	        }

	        $rootScope.$broadcast("loggedIn");
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    function loadProfileImage(event, args) {
	        var data = args.data;
	        if (profileInfo.profile.image_url == data.name) {
	            if (args.data.image_str == "") {
	                clientDB.addUserProfileImage(args.data.image_str);
	                emptyProfileImage.status = true;
	            }
	            else {
	                clientDB.addUserProfileImage(args.data.image_str);
	                profileInfo.profile.image_string = args.data.image_str;
	                emptyProfileImage.status = false;
	            };
	        };
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    function updateUserProfile(event, args) {
	        clientDB.addUserProfile(profileInfo.profile);
	        clientDB.addUserProfileImage(profileInfo.profile.image_string);
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    return self;
	}]);
