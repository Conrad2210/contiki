'use strict';
/*
 * Contains a service to communicate with the TRACK TV API
 */
angular.module('app.services').factory('AccountService', ['$base64','$rootScope', 'cDB', 'CoreHubService',
	function ($base64,$rootScope, cDB, CoreHubService) {
	    var self = this;
	    var clientDB = cDB;
	    var hub = CoreHubService;

	    var accountList = { account: null };
	    var accountAdded = false;




	    $rootScope.$on("TradeFairs_GetCMSAccounts", loadCMSAccounts);
	    $rootScope.$on("TradeFairs_GetCMSImage", loadAccountImage);

	    $rootScope.$on("TradeFairs_UpdateCMSAccount", updateAccountObject);
	    $rootScope.$on("TradeFairs_DeleteCMSAccounts", deleteAccountObject);

	 



	    this.admintypes = S_CMSAdminTypeJS_Array;
	    this.statuss = S_CMSProfileUserStatusJS_Array;

	    this.setAccount = function (account) {
	        accountList.account = account;
	    }

	    this.getAccount = function () {
	        return accountList;
	    };


	    this.QueryAccount = function () {
	        hub.GetCMSAccounts(clientDB.getCurrentProject());
	    }
	    this.loadAccountImage = function (account, isVisible) {
	        //isVisible attr is not part of store structure
	        //causing errors when a store is being updated.
	        if (isVisible && account.isVisible == undefined) {
	            if (!jQuery.isEmptyObject(account)) {
	                account.isVisible = true;
	                if (account.image_url != "") {
	                    if (account.image_string == "") {
	                        hub.GetCMSImage(account.image_url, "small");
	                    }
	                }
	                else {
	                    if (account.image_string == "")
	                    account.image_string = "./img/blank_profile.jpg";
	                    
	                }
	            }
	        }
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    this.addAccount = function (accountObject) {
	        accountAdded = true;
	        accountObject.last_login_project = cDB.getCurrentProject();
	        accountObject.image_string = accountObject.image_string.replace("data:image/png;base64,", "");
	        accountList.account.push(angular.copy(accountObject));
	        delete accountObject["$$hashKey"];
	        delete accountObject["outOfDate"];
	        delete accountObject["isVisible"];
	        hub.UpdateCMSAccounts(accountObject, cDB.getCurrentProject());
	        accountObject.image_string = "data:image/png;base64," + accountObject.image_string;
	        accountList.account.push(angular.copy(accountObject));
	    };

	    this.updateAccount = function (accountObject) {
	        console.log("Account Service Update");
	        for (var i = 0; i < accountList.account.length; i++) {
	            if (accountList.account[i].account_id == accountObject.account_id) {
	                accountObject.image_string = accountObject.image_string.replace("data:image/png;base64,", "");
	                accountList.account[i] = angular.copy(accountObject);
	                delete accountObject["$$hashKey"];
	                delete accountObject["outOfDate"];
	                delete accountObject["isVisible"];
	                hub.UpdateCMSAccounts(accountObject, cDB.getCurrentProject());
	                accountObject.image_string = "data:image/png;base64," + accountObject.image_string;
	                accountList.account[i] = angular.copy(accountObject);
	                break;
	            };
	        };
	    };

	    this.deleteAccount = function (accountObject) {
	        for (var i = 0; i < accountList.account.length; i++) {
	            if (accountList.account[i].username == accountObject.username) {
	                accountList.account.splice(i, 1);
	                hub.DeleteCMSAccounts(accountObject.username, cDB.getCurrentProject());
	                break;
	            };
	        };
	    };

	    //Call back functions

	    function loadCMSAccounts(event, args) {
	        if (args.data == null)
	            args.data = [];
	        accountList.account = args.data;
	        clientDB.addCMSAccounts(clientDB.getCurrentProject(), args.data)
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    function loadAccountImage(event, args) {
	        var data = args.data;
	        if (accountList.account != null) {
	            
	                for (var i = 0; i < accountList.account.length; i++) {
	                    if (accountList.account[i].image_url == data.name) {
	                        var imageStr = "data:image/png;base64," + data.image_str;
	                        //console.log(imageStr);
	                        clientDB.addCMSAccountImageString(cDB.getCurrentProject(), accountList.account[i].image_url, imageStr)
	                        accountList.account[i].image_string = imageStr;
	                        break;
	                    };
	                }
	            
	        }
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    function updateAccountObject(event, args) {
	        console.log("A Account Account has been updated or included id: " + args.data);
	        //Server responds with id of new store.
	        //If store recently added then following is done
	        if (accountAdded) {
	            for (var i = accountList.account.length - 1; i >= 0; i--) {
	                if (accountList.account[i].account_id == -1) {
	                    console.log("ID with -1 found at position: " + i);
	                    accountList.account[i].id = args.data;
	                    clientDB.addCMSAccounts(clientDB.getCurrentProject(), accountList.account);
	                    break;
	                };
	            };
	            accountAdded = false;
	        };
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    function deleteAccountObject(event, args) {
	        //True/False returned from server
	        console.log("A Account Account has been deleted: " + args.data);
	        clientDB.addCMSAccounts(clientDB.getCurrentProject(), accountList.account);
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    return self;
	}]);
