'use strict';
/*
 * Contains a controller to do the user login
 */
angular.module('app.controllers').controller('StoreController', ['$scope', '$timeout', 'StoreService', 'CategoryService', 'PaginationService',
function ($scope,$timeout, StoreService, CategoryService, PaginationService) {

    var stores = StoreService;
    var categories = CategoryService;
    var pagination = PaginationService;


    $scope.SortTypeArray = ["name"];
    $scope.sortType = 'name'; // set the default sort type
    $scope.sortReverse = false;
    $scope.changeSortType = function (sortType, sortReverse) {
        $scope.sortType = sortType;
        if (sortReverse == 1) {
            $scope.sortReverse = !$scope.sortReverse
        }
    }

    $scope.storeList = { stores: null };
    $scope.storeList = stores.getStores();
    $scope.StepObject = stores.getSteps();
    $scope.categoryList = { categories: null };
    $scope.categoryList = categories.getCategories();

    $scope.$watch('storeList.stores.length', function () {
        if ($scope.storeList.stores != null) {
            pagination.setDetails("Store");
        };
    });

    //Tabs to navigate between details for adding, editing a store in modals
    $scope.activeTab = 1;
    $scope.setActiveTab = function (tabToSet) { $scope.activeTab = tabToSet; };
    $scope.editTab = "tab1";
    $scope.setEditTab = function (name) { $scope.editTab = name; };

    //The following section is for adding categories to a store
    //Creates tags that the current store has
    //Creates a list of all tags that are available for a store
    $scope.tags = [];
    $scope.storeCategorySetup = function (store) {
        if (store === undefined) {
            store = new S_CMSStoreJS();
            store.list_item = new S_CMSListItemJS();
        }
        $scope.tempStore = angular.copy(store);
        $scope.tags = angular.copy($scope.categoryList.categories);
        for (var i = 0; i < $scope.tags.length; i++) {
            $scope.tags[i].isActive = false;
        };
        for (var i = 0; i < $scope.tempStore.product_categories.length; i++) {  //Loop through store categories
            for (var j = 0; j < $scope.tags.length; j++) {  //Loop through all categories
                //If category is found in store categories, it is set as active
                if ($scope.tempStore.product_categories[i].category == $scope.tags[j].category) {
                    $scope.tags[j].isActive = true;
                };
            };
        };
    };
    //Method to control toggle of tags from checked to unchecked
    $scope.toggleTags = function (tag, id) {
        if (tag.isActive == true) {
            var tagID = '#' + id;
            $(tagID).prop('checked', false);
            tag.isActive = false;
        }
        else {
            var tagID = '#' + id;
            $(tagID).prop('checked', true);
            tag.isActive = true;
        };
    };
    //Add new tags to a store
    $scope.addStoreCategories = function (store) {
        //Delete all categories in the store
        store.product_categories.splice(0, store.product_categories.length);
        //Loop over tags[], if active delete key and add to store
        for (var i = 0; i < $scope.tags.length; i++) {
            if ($scope.tags[i].isActive) {
                var temp = angular.copy($scope.tags[i]);
                delete temp["isActive"];
                store.product_categories.push(angular.copy(temp));
            };
        };
        stores.updateStore(store);
    };

    //Store image upload for a new + existing store
    $scope.previewStoreImg = false;
    $scope.showStoreImg = true;
    $scope.storeImg = null;


    $scope.onLoad = function (e, reader, file, fileList, fileOjects, fileObj) {
        $scope.storeImg = fileObj;
        $scope.previewStoreImg = true;
    };
    $scope.deleteStoreImage = function () {
        if ($scope.storeOption == 1) {
            $scope.tempStore.image_string = "";
            $scope.showStoreImg = false;
        }
        else {
            $scope.tempStore.image_string = "delete";
            $scope.showStoreImg = false;
        };
    };
    $scope.changeStoreImage = function () {
        $scope.tempStore.image_string = angular.copy("data:image/png;base64," + $scope.storeImg.base64);
        $scope.showStoreImg = true;
        $scope.previewStoreImg = false;
        $scope.storeImg.base64 = null;
        $scope.storeImg = null;
    };
    $scope.discardUploadedImage = function () {
        $scope.previewStoreImg = false;
        $scope.storeImg.base64 = null;
        $scope.storeImg = null;
    };

    $scope.previewStoreBkgImg = false;
    $scope.showBkgImg = true;
    $scope.bkgImg = null;
    $scope.onLoad1 = function (e, reader, file, fileList, fileOjects, fileObj) {
        $scope.bkgImg = fileObj;
        $scope.previewStoreBkgImg = true;
    };
    $scope.deleteStoreBkgImage = function () {
        if ($scope.storeOption == 1) {
            $scope.tempStore.bg_image_string = "";
            $scope.showBkgImg = false;
        }
        else {
            $scope.tempStore.bg_image_string = "delete";
            $scope.showBkgImg = false;
        };
    };
    $scope.changeStoreBkgImage = function () {
        $scope.tempStore.bg_image_string = angular.copy("data:image/png;base64," + $scope.bkgImg.base64);
        $scope.showBkgImg = true;
        $scope.previewStoreBkgImg = false;
        $scope.bkgImg.base64 = null;
        $scope.bkgImg = null;
    };
    $scope.discardUploadedImage = function () {
        $scope.previewStoreBkgImg = false;
        $scope.bkgImg.base64 = null;
        $scope.bkgImg = null;
    };

    $scope.updateStore = function () {
        if ($scope.isNewShop == true)
            stores.addStore($scope.editShop);
        else
            stores.updateStore($scope.editShop);
    };
    $scope.deleteStore = function () {
        stores.deleteStore($scope.tempStore);
    };

    $scope.addEditStoreSetup = function (store, num) {
        if (num == 1) {
            $scope.tempStore = new S_CMSStoreJS();
            $scope.tempStore.list_item = new S_CMSListItemJS();
            $scope.storeOption = 1;
            $scope.showStoreImg = false;
        }
        else {
            $scope.tempStore = angular.copy(store);
            $scope.showStoreImg = true;
            $scope.storeOption = 2;
        };
    };
    $scope.tempStore = new S_CMSStoreJS();
    $scope.tempStore.list_item = new S_CMSListItemJS();
    $scope.storeOption = 1;
    $scope.addUpdateStore = function () {
        if ($scope.storeOption == 1) {
            if ($scope.tempStore.name1 != "") {
                stores.addStore($scope.tempStore);
            };
        }
        else {
            stores.updateStore($scope.tempStore);
        };
    };
    $scope.deleteStoreModal = function (store) {
        $("#deleteModal").modal("toggle");
        $scope.tempStore = angular.copy(store);
    };

    $scope.storeDetailsShow = function (shop) {
        $scope.storeDetails = angular.copy(shop);   //Sets up store object to be edited
    };

    //Area for all Pagination details
    $scope.paginationDetails = {};
    $scope.paginationDetails = pagination.getDetails();
    $scope.changePageNum = function (number) {
        pagination.changePageNum(number, "Store");
    };
    $scope.firstPage = function () {
        pagination.firstPage();
    };
    $scope.lastPage = function () {
        pagination.lastPage();
    }
    $scope.prevPage = function () {
        pagination.prevPage();
    };
    $scope.nextPage = function () {
        pagination.nextPage();
    }

    $scope.nameInput = function () {
        if ($scope.tempStore.name1 != undefined)
            $scope.isValid = true;
        else
            $scope.isValid = false;

    }

    //This gets called when store comes into view and automatically loads
    //the image from server

    $scope.loadStoreImage = function (store, isVisible) {
        stores.loadStoreImage(store, isVisible);
    };
}]);