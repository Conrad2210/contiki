'use strict';

angular.module('app.directives').directive('accountDirective', function () {

    return {
        templateUrl: "Templates/Account/account.html",
        controller: "AccountController"
    }
});


//Following driectives are nested in storedirective so no controller is added as of yet. 
angular.module('app.directives').directive('accountListDirective', function () {

    return {
        //No controller added to this yet
        templateUrl: "Templates/Account/accountlist.html"
    }
});

//Following driectives are nested in storedirective so no controller is added as of yet. 
angular.module('app.directives').directive('accountCardDirective', function () {

    return {
        //No controller added to this yet
        templateUrl: "Templates/Account/accountcard.html"
    }
});
//Following driectives are nested in storedirective so no controller is added as of yet. 
angular.module('app.directives').directive('accountEditDirective', function () {

    return {
        //No controller added to this yet
        templateUrl: "Templates/Account/account_edit.html"
    }
});
//Following driectives are nested in storedirective so no controller is added as of yet. 
angular.module('app.directives').directive('accountDetailsDirective', function () {

    return {
        //No controller added to this yet
        templateUrl: "Templates/Account/account_details.html"
    }
});
