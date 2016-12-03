/*
* This is the main module of the app.
* Used as a center point to inject all other modules( Controllers, Services, Directives, etc... )
*/
'use strict';
angular.module("cmsApp", ['indexedDB', 'ngRoute','ngWebSocket', 'app.services', 'app.controllers', 'app.directives', 'app.routes', 'ClientDB', 'SignalR', 'ngCkeditor', 'ngSanitize', 'flow', 'naif.base64', 'angular-inview', 'ui.bootstrap.datetimepicker', 'base64', 'rzModule'])
.config(function ($indexedDBProvider) {
    $indexedDBProvider
      .connection('CMSData', { version: 1, storage: "temporary" })
      .upgradeDatabase(1, function(event, db, tx){
          var objStore = db.createObjectStore('MapData', { keyPath: 'projectID' });
          objStore.createIndex('CMSLayerData3D', 'CMSLayerData3D', { unique: false });
          objStore.createIndex('CMSData3D', 'CMSData3D', { unique: false });
          objStore.createIndex('CMSData3DDecomp', 'CMSData3DDecomp', { unique: false });
          objStore.createIndex('NeedsReload', 'NeedsReload', { unique: false });
          objStore.clear();
      })
    .upgradeDatabase(2, function(event, db, tx){
        var objStore = db.createObjectStore('Pictures', { keyPath: 'PictureID'});
        objStore.createIndex('PictureID', 'PictureID', { unique: true });
        objStore.createIndex('base64String', 'base64String', { unique: false });
        objStore.clear();
    });

})

	.run(['$', '$rootScope', '$location', function ($, $rootScope, $location) {

	}]);