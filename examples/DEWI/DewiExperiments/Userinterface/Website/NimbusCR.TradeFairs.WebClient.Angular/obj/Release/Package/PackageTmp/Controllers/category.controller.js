'use strict';
/*
 * Contains a controller to do the user login
 */
angular.module('app.controllers').controller('CategoryController', ['$scope', 'CategoryService', 'PaginationService',
function ($scope, CategoryService, PaginationService) {

    var categories = CategoryService;
    var pagination = PaginationService;

    $scope.SortTypeArray = ["category"];
    $scope.sortType = 'category'; // set the default sort type
    $scope.sortReverse = false;
    $scope.changeSortType = function (sortType, sortReverse) {
        $scope.sortType = sortType;
        if (sortReverse == 1) {
            $scope.sortReverse = !$scope.sortReverse
        }
    }

    $scope.categoryList = { categories: null };
    $scope.categoryList = categories.getCategories();

    $scope.$watch('categoryList.categories.length', function () {
        if ($scope.categoryList != null) {
            pagination.setDetails("Category");
        };
    });

    //Area for adding, editing, deleting Category items
    $scope.newCategory = { name: "" };

    $scope.updateCategory = function () {
        if ($scope.isNewCategory)
            categories.addCategory($scope.tempCategory);
        else
            categories.updateCategory($scope.tempCategory);
    };
    $scope.deleteCategory = function () {
        categories.deleteCategory($scope.editCategory);
    };


    //Area to pop up modals
    $scope.addEditcategorySetup = function (categoryObject, num) {
        if (num == 1) {
            $scope.tempCategory = new S_CMSProfileCategoryJS();
            $scope.tempCategory.list_item = new S_CMSListItemJS();
            $scope.isNewCategory = true;
            $scope.categoryOption = 1;
        }
        else {
            $scope.tempCategory = angular.copy(categoryObject);
            $scope.isNewCategory = false;
            $scope.categoryOption = 2;
        };
    };

    $scope.deleteModal = function (category) {
        $("#deleteModal").modal("toggle");
        $scope.editCategory = angular.copy(category);
    };

    //Area for Pagination details
    $scope.paginationDetails = {};
    $scope.paginationDetails = pagination.getDetails();
    $scope.changePageNum = function (number) {
        pagination.changePageNum(number, "Category");
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

}]);

