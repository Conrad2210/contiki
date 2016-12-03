'use strict';
/*
 * Contains a service to communicate with the TRACK TV API
 */
angular.module('app.services').factory('CategoryService', ['$rootScope', 'cDB', 'CoreHubService',
	function ($rootScope, cDB, CoreHubService) {
	    var self = this;
	    var clientDB = cDB;
	    var hub = CoreHubService;

	    $rootScope.$on("TradeFairs_GetProfileCategories", loadCategories);

	    $rootScope.$on("TradeFairs_UpdateCMSProfileCategory", updateCategoryObject);
	    $rootScope.$on("TradeFairs_DeleteCMSProfileCategories", deleteCategoryObject);

	    var categoryList = { categories: null };
	    var categoryAdded = false;


	    this.QueryProductCategories = function () {
	        hub.GetCMSProfileCategories(clientDB.getCurrentProject(), "product");
	    }

	    this.QuerySectorCategories = function () {
	        hub.GetCMSProfileCategories(clientDB.getCurrentProject(), "sector");
	    }

	    this.setCategories = function (categories) {
	        categoryList.categories = categories;
	    }

	    this.getCategories = function () {
	        return categoryList;
	    };

	    this.addCategory = function (category) {
	        categoryAdded = true;
	        var newCategory = new S_CMSProfileCategoryJS();
	        newCategory.list_item = new S_CMSListItemJS();
	        newCategory.type = "S_CMSProfileCategoryType_product";
	        newCategory.category = angular.copy(category.category);
	        categoryList.categories.push(newCategory);
	        delete category["$$hashKey"];
	        delete category["outOfDate"];
	        hub.UpdateCMSProfileCategory(newCategory, cDB.getCurrentProject());
	    };

	    this.updateCategory = function (category) {
	        for (var i = 0; i < categoryList.categories.length; i++) {
	            if (categoryList.categories[i].id == category.id) {
	                categoryList.categories[i].category = category.category;
	                delete category["outOfDate"];
	                delete category["$$hashKey"];
	                hub.UpdateCMSProfileCategory(category, cDB.getCurrentProject());
	                break;
	            }
	        }
	    };

	    this.deleteCategory = function (category) {
	        for (var i = 0; i < categoryList.categories.length; i++) {
	            if (categoryList.categories[i].id == category.id) {
	                categoryList.categories.splice(i, 1);
	                hub.DeleteCMSProfileCategories(category.id, "S_CMSProfileCategoryType_product", cDB.getCurrentProject());
	                break;
	            }
	        }
	    };

	    //Callback functions 

	    function loadCategories(event, args) {
	        if (args.data == null)
	            args.data = [];
	        categoryList.categories = args.data;
	        clientDB.addCMSStoreCategories(clientDB.getCurrentProject(), args.data);
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    function updateCategoryObject(event, args) {
	        console.log("A category has been updated or included id: " + args.data);
	        //Server responds with id of new category.
	        //If category recently added then following is done
	        if (categoryAdded) {
	            for (var i = categoryList.categories.length - 1; i >= 0; i--) {
	                if (categoryList.categories[i].id == -1) {
	                    console.log("ID with -1 found at position: " + i);
	                    categoryList.categories[i].id = args.data;
	                    clientDB.addCMSStoreCategories(clientDB.getCurrentProject(), categoryList.categories);
	                    break;
	                };
	            };
	            categoryAdded = false;
	        };
	        if ($rootScope.$$phase != '$apply' && $rootScope.$$phase != '$digest') { $rootScope.$apply(); }
	    };

	    function deleteCategoryObject(event, args) {
	        console.log("A category has been deleted :" + args.data);
	        clientDB.addCMSStoreCategories(clientDB.getCurrentProject(), categoryList.categories);
	    };

	    return self;
	}]);
