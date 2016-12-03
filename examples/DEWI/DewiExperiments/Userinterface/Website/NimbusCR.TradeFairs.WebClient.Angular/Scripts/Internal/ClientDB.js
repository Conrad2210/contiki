angular.module('ClientDB', ['lokijs'])
.factory('cDB', ['$rootScope', 'Loki', '$indexedDB', function ($rootScope,Loki, $indexedDB) {



    var cDB = this;

    cDB.ClientDB = {};
    cDB.accountDB = {};
    cDB.storeDB = {};
    cDB.boothDB = {};
    cDB.beaconDB = {};
    cDB.newsFeedDB = {};
 
    cDB.InitClientDB = function (login) {

  
            cDB.ClientDB = new Loki("ClientDB.json", { autosave: true, autosaveInterval: 1000, autoload: true });
            cDB.accountDB = new Loki("accountDB.json", { autosave: true, autosaveInterval: 1000, autoload: true });
            cDB.storeDB = new Loki("storeDB.json", { autosave: true, autosaveInterval: 1000, autoload: true });
            cDB.boothDB = new Loki("boothDB.json", { autosave: true, autosaveInterval: 1000, autoload: true });
            cDB.beaconDB = new Loki("beaconDB.json", { autosave: true, autosaveInterval: 1000, autoload: true });
            cDB.newsFeedDB = new Loki("newsFeedDB.json", { autosave: true, autosaveInterval: 1000, autoload: true });

            if (!login) {
                cDB.ClientDB.loadDatabase();
                cDB.accountDB.loadDatabase();
                cDB.storeDB.loadDatabase();
                cDB.boothDB.loadDatabase();
                cDB.beaconDB.loadDatabase();
                cDB.newsFeedDB.loadDatabase();
                return;
            }
            else {
                cDB.ClientDB.deleteDatabase();
                cDB.accountDB.deleteDatabase();
                cDB.storeDB.deleteDatabase();
                cDB.boothDB.deleteDatabase();
                cDB.beaconDB.deleteDatabase();
                cDB.newsFeedDB.deleteDatabase();
            }


            cDB.ClientDB = new Loki("ClientDB.json", { autosave: true, autosaveInterval: 1000, autoload: true });
            cDB.accountDB = new Loki("accountDB.json", { autosave: true, autosaveInterval: 1000, autoload: true });
            cDB.storeDB = new Loki("storeDB.json", { autosave: true, autosaveInterval: 1000, autoload: true });
            cDB.boothDB = new Loki("boothDB.json", { autosave: true, autosaveInterval: 1000, autoload: true });
            cDB.beaconDB = new Loki("beaconDB.json", { autosave: true, autosaveInterval: 1000, autoload: true });
            cDB.newsFeedDB = new Loki("newsFeedDB.json", { autosave: true, autosaveInterval: 1000, autoload: true });
        }
    cDB.InitClientDB(false);

        cDB.saveDatabase = function () {
            try { cDB.ClientDB.saveDatabase(); } catch (err) { console.log(err) };
            try { cDB.accountDB.saveDatabase(); } catch (err) { console.log(err) };
            try { cDB.storeDB.saveDatabase(); } catch (err) { console.log(err) };
            try { cDB.boothDB.saveDatabase(); } catch (err) { console.log(err) };
            try { cDB.beaconDB.saveDatabase(); } catch (err) { console.log(err) };
            try { cDB.newsFeedDB.saveDatabase(); } catch (err) { console.log(err) };
        }

        cDB.loadDatabase = function () {
            try { cDB.ClientDB.loadDatabase(); } catch (err) { console.log(err) };
            try { cDB.accountDB.loadDatabase(); } catch (err) { console.log(err) };
            try { cDB.storeDB.loadDatabase(); } catch (err) { console.log(err) };
            try { cDB.boothDB.loadDatabase(); } catch (err) { console.log(err) };
            try { cDB.beaconDB.loadDatabase(); } catch (err) { console.log(err) };
            try { cDB.newsFeedDB.loadDatabase(); } catch (err) { console.log(err) };
        }

        cDB.closeDatabase = function () {
            try { cDB.ClientDB.close(); } catch (err) { console.log(err) };
            try { cDB.accountDB.close(); } catch (err) { console.log(err) };
            try { cDB.storeDB.close(); } catch (err) { console.log(err) };
            try { cDB.boothDB.close(); } catch (err) { console.log(err) };
            try { cDB.beaconDB.close(); } catch (err) { console.log(err) };
            try { cDB.newsFeedDB.close(); } catch (err) { console.log(err) };
        }

        cDB.deleteDatabase = function () {
            try { cDB.ClientDB.deleteDatabase(); } catch (err) { console.log(err) };
            try { cDB.accountDB.deleteDatabase(); } catch (err) { console.log(err) };
            try { cDB.storeDB.deleteDatabase(); } catch (err) { console.log(err) };
            try { cDB.boothDB.deleteDatabase(); } catch (err) { console.log(err) };
            try { cDB.beaconDB.deleteDatabase(); } catch (err) { console.log(err) };
            try { cDB.newsFeedDB.deleteDatabase(); } catch (err) { console.log(err) };
        }




        /////////////////////////////////////////////////////////
        /////             Utility functions      
        /////////////////////////////////////////////////////////


        cDB.addCredentials = function (data) {

            var collection = cDB.ClientDB.getCollection("userData");
            if (collection == null) {
                collection = cDB.ClientDB.addCollection("userData");
                collection.insert({ name: "Credentials", data: data });
            } else {
                try {
                    var tempData = collection.get(1);
                    tempData.data = data;
                }
                catch (err) {
                    collection.insert({ name: "Credentials", data: data });
                }
            }
            cDB.saveDatabase();
        }


        cDB.getCredentials = function () {
            var collection = cDB.ClientDB.getCollection("userData");
            if(collection == null)
                return null;

            var tempData = collection.find({ name: "Credentials" });
            var tempData1 = collection.find({ name: "Credentials" })[0];
            return tempData1.data;
        }

        //adds core/profile information
        cDB.addUserProfile = function (data) {
            var collection = cDB.ClientDB.getCollection("userData");
            if (collection == null) {
                collection = cDB.ClientDB.addCollection("userData");
                collection.insert({ name: "UserProfile", data: data });
            } else {
                try {

                    var tempData = collection.find({ name: "UserProfile" })[0];
                    tempData.data = data;
                }
                catch (err) {
                    collection.insert({ name: "UserProfile", data: data });
                }
            }
            cDB.saveDatabase();
        }

        cDB.getUserProfile = function () {
            var collection = cDB.ClientDB.getCollection("userData");

            return collection == null ? null : !jQuery.isEmptyObject(collection.find({ name: "UserProfile" })) ? collection.find({ name: "UserProfile" })[0].data : null;
        }

        cDB.addUserProfileImage = function (data) {

            $indexedDB.openStore('Pictures', function (store) {

                store.insert({ "PictureID": "ProfilePicture", "ProjectID": cDB.getCurrentProject(), "base64String": data }).then(function (e) { console.log("picture added") });

            });
        }

        //adds account information after login
        cDB.addUserAccount = function (data) {
            var collection = cDB.ClientDB.getCollection("userData");
            if (collection == null) {
                collection = cDB.ClientDB.addCollection("userData");
                collection.insert({ name: "UserAccount", data: data });
            } else {
                try {

                    var tempData = collection.find({ name: "UserAccount" })[0];
                    tempData.data = data;
                }
                catch (err) {
                    collection.insert({ name: "UserAccount", data: data });
                }
            }
            cDB.saveDatabase();
        }
        cDB.getUserAccount = function () {
            var collection = cDB.ClientDB.getCollection("userData");

            return collection == null ? null : jQuery.isEmptyObject(collection.find({ name: "UserAccount" })) ? null : collection.find({ name: "UserAccount" })[0].data;
        }
        cDB.getAuthID = function () {

            var collection = cDB.ClientDB.getCollection("userData");

            if (collection == null)
                return null;

            var data = collection.find({ name: "UserAccount" });

            if( jQuery.isEmptyObject(data))
                return null;

            var realData = data[0].data;

            return realData.auth_key;
        }
        cDB.getUserName = function () {
            var collection = cDB.ClientDB.getCollection("userData");

            return collection == null ? null : jQuery.isEmptyObject(collection.find({ name: "Credentials" })) ? null : collection.find({ name: "Credentials" })[0].data.username;
        }

        cDB.getDefaultProject = function () {
            var collection = cDB.ClientDB.getCollection("userData");

            return collection == null ? "" : jQuery.isEmptyObject(collection.find({ name: "UserAccount" })) ? null : collection.find({ name: "UserAccount" })[0].data.preferences.gui.default_project;
        }



        cDB.getUserType = function () {
            var collection = cDB.ClientDB.getCollection("userData");
            if (collection == null)
                return null;

            var userAccount = collection.find({ name: "UserAccount" });

            return !jQuery.isEmptyObject(userAccount) ? userAccount[0].data.adminType : null;
        }

        cDB.getUserId = function () {
            var collection = cDB.ClientDB.getCollection("userData");
            if (collection == null)
                return null;

            var userAccount = collection.find({ name: "UserAccount" });

            return !jQuery.isEmptyObject(userAccount) ? userAccount[0].data.userid : null;
        }

        cDB.addProjectSettings = function (project, data) {
            var collection = cDB.ClientDB.getCollection("projectSettings");
            if (collection == null) {
                collection = cDB.ClientDB.addCollection("projectSettings");
                collection.insert({ name: project, data: data });
            } else {
                try {

                    var tempData = getProjectSettings(project, true);
                    tempData.data = data;
                }
                catch (err) {
                    collection.insert({ name: project, data: data });
                }
            }
            cDB.saveDatabase();
        }

        cDB.getProjectSettings = function (project, object) {
            object = typeof object !== 'undefined' ? object : false;
            if (!object) {
                var collection = cDB.ClientDB.getCollection("projectSettings");
                if (collection == null)
                    return null;
                else {
                    var temp = collection.find({ name: project });
                    return !jQuery.isEmptyObject(temp) ? temp[0].data : null;
                }
            }
            else {
                var collection = cDB.ClientDB.getCollection("projectSettings");
                if (collection == null)
                    return null;
                else {
                    var temp = collection.find({ name: project });
                    return !jQuery.isEmptyObject(temp) ? temp[0] : null;
                }
            }
        }

        cDB.addCurrentProject = function (project) {

            var collection = cDB.ClientDB.getCollection("currentProject");
            if (collection == null) {
                collection = cDB.ClientDB.addCollection("currentProject");
                collection.insert({ name: "currentProject", data: project });
            } else {
                try {
                    var tempData = collection.get(1);
                    tempData.data = project;
                }
                catch (err) {
                    collection.insert({ name: "currentProject", data: project });
                }
            }
            cDB.saveDatabase();
        }

        cDB.getCurrentProject = function () {
            var collection = cDB.ClientDB.getCollection("currentProject");

            return collection == null ? null : collection.get(1).data;

        }

        cDB.addCountryData = function (data) {
            var collection = cDB.ClientDB.getCollection("countryData");
            if (collection == null) {
                collection = cDB.ClientDB.addCollection("countryData");
                collection.insert({ name: "countryData", data: data });
            } else {
                try {

                    var tempData = getCountryData(true);
                    tempData.data = data;
                }
                catch (err) {
                    collection.insert({ name: "countryData", data: data });
                }
            }
            cDB.saveDatabase();
        }

        cDB.getCountryData = function (object) {
            object = typeof object !== 'undefined' ? object : false;
            if (!object) {
                var collection = cDB.ClientDB.getCollection("countryData");
                if (collection == null)
                    return null;
                else {
                    var temp = collection.get(1);
                    console.log(temp);
                    return !jQuery.isEmptyObject(temp) ? temp.data : null;
                }
            }
            else {
                var collection = cDB.ClientDB.getCollection("countryData");
                if (collection == null)
                    return null;
                else {
                    var temp = collection.get(1);
                    return !jQuery.isEmptyObject(temp) ? temp : null;
                }
            }
        }


        //checks if data is out of date, if set Reload flag for data, checks all local databases

        cDB.checkOutOfDate = function (dbids, project) {
            //Check accounts
            var usedDBIDS = cDB.checkCMSAccountsOutOfDate(dbids, project);
            cDB.spliceDBIDS(usedDBIDS, dbids);
            if (jQuery.isEmptyObject(dbids))
                return;

            //check beacons
            usedDBIDS = cDB.checkCMSBeaconsOutOfDate(dbids, project);
            cDB.spliceDBIDS(usedDBIDS, dbids);
            if (jQuery.isEmptyObject(dbids))
                return;

            //check booths
            usedDBIDS = cDB.checkCMSBoothsOutOfDate(dbids, project);
            cDB.spliceDBIDS(usedDBIDS, dbids);
            if (jQuery.isEmptyObject(dbids))
                return;

            //check News Feed
            usedDBIDS = cDB.checkCMSNewsFeedsOutOfDate(dbids, project);
            cDB.spliceDBIDS(usedDBIDS, dbids);
            if (jQuery.isEmptyObject(dbids))
                return;

            //check Stores
            usedDBIDS = cDB.checkCMSStoresOutOfDate(dbids, project);
            cDB.spliceDBIDS(usedDBIDS, dbids);
            if (jQuery.isEmptyObject(dbids))
                return;

            //check Categories
            usedDBIDS = cDB.checkCMSStoreCategoriesOutOfDate(dbids, project);
            cDB.spliceDBIDS(usedDBIDS, dbids);
            if (jQuery.isEmptyObject(dbids))
                return;

            

        }

        cDB.spliceDBIDS = function (usedDBIDS, dbids) {
            if (!jQuery.isEmptyObject(usedDBIDS) || usedDBIDS != -1) {
                for (var i = 0; i < usedDBIDS.length; i++) {
                    var index = dbids.indexOf(usedDBIDS[i]);

                    if (index > -1) {
                        dbids.splice(index, 1);
                        usedDBIDS.splice(i, 1);
                        i = i - 1;
                    }
                }
            }
        }

        cDB.checkCMSAccountsOutOfDate = function (dbids, project) {
            var usedDBIDS = [];
            var collection = cDB.accountDB.getCollection("CMSAccounts");
            if (collection == null) {
                return -1;
            }
            var temp = collection.find({ name: project });

            if (jQuery.isEmptyObject(temp))
                return -1;

            var data = temp[0].data;

            if (jQuery.isEmptyObject(data))
                return -1;


            data.forEach(function (element, index, array) {
                for (var i = 0; i < dbids.length; i++) {
                    if (element.list_item.id == dbids[i]) {
                        usedDBIDS.push(dbids[i]);
                        element.outOfDate = true;
                        temp[0].update = true;
                        break;
                    }
                }
            })
            cDB.saveDatabase;
            return usedDBIDS;
        }

        cDB.checkCMSStoresOutOfDate = function (dbids, project) {
            var usedDBIDS = [];
            var collection = cDB.storeDB.getCollection("CMSStores");
            if (collection == null) {
                return -1;
            }
            var temp = collection.find({ name: project });

            if (jQuery.isEmptyObject(temp))
                return -1;

            var data = temp[0].data;

            if (jQuery.isEmptyObject(data))
                return -1;


            data.forEach(function (element, index, array) {
                for (var i = 0; i < dbids.length; i++) {
                    if (element.list_item.id == dbids[i]) {
                        usedDBIDS.push(dbids[i]);
                        element.outOfDate = true;
                        temp[0].update = true;
                        break;
                    }
                }
            })
            cDB.saveDatabase;
            return usedDBIDS;
        }

        cDB.checkCMSBoothsOutOfDate = function (dbids, project) {
            var usedDBIDS = [];
            var collection = cDB.boothDB.getCollection("CMSBooths");
            if (collection == null) {
                return -1;
            }
            var temp = collection.find({ name: project });

            if (jQuery.isEmptyObject(temp))
                return -1;

            var data = temp[0].data;

            if (jQuery.isEmptyObject(data))
                return -1;


            data.forEach(function (element, index, array) {
                for (var i = 0; i < dbids.length; i++) {
                    if (element.list_item.id == dbids[i]) {
                        usedDBIDS.push(dbids[i]);
                        element.outOfDate = true;
                        temp[0].update = true;
                        break;
                    }
                }
            })
            cDB.saveDatabase;
            return usedDBIDS;
        }

        cDB.checkCMSBeaconsOutOfDate = function (dbids, project) {
            var usedDBIDS = [];
            var collection = cDB.beaconDB.getCollection("CMSBeacons");
            if (collection == null) {
                return -1;
            }
            var temp = collection.find({ name: project });

            if (jQuery.isEmptyObject(temp))
                return -1;

            var data = temp[0].data;

            if (jQuery.isEmptyObject(data))
                return -1;


            data.forEach(function (element, index, array) {
                for (var i = 0; i < dbids.length; i++) {
                    if (element.list_item.id == dbids[i]) {
                        usedDBIDS.push(dbids[i]);
                        element.outOfDate = true;
                        temp[0].update = true;
                        break;
                    }
                }
            })
            cDB.saveDatabase;
            return usedDBIDS;
        }

        cDB.checkCMSNewsFeedsOutOfDate = function (dbids, project) {
            var usedDBIDS = [];
            var collection = cDB.newsFeedDB.getCollection("CMSNewsFeeds");
            if (collection == null) {
                return -1;
            }
            var temp = collection.find({ name: project });

            if (jQuery.isEmptyObject(temp))
                return -1;

            var data = temp[0].data;

            if (jQuery.isEmptyObject(data))
                return -1;


            data.forEach(function (element, index, array) {
                for (var i = 0; i < dbids.length; i++) {
                    if (element.list_item.id == dbids[i]) {
                        usedDBIDS.push(dbids[i]);
                        element.outOfDate = true;
                        temp[0].update = true;
                        break;
                    }
                }
            })
            cDB.saveDatabase;
            return usedDBIDS;
        }

        cDB.checkCMSStoreCategoriesOutOfDate = function (dbids, project) {
            var usedDBIDS = [];
            var collection = cDB.storeDB.getCollection("CMSStoreCategories");
            if (collection == null) {
                return -1;
            }
            var temp = collection.find({ name: project });

            if (jQuery.isEmptyObject(temp))
                return -1;

            var data = temp[0].data;

            if (jQuery.isEmptyObject(data))
                return -1;


            data.forEach(function (element, index, array) {
                for (var i = 0; i < dbids.length; i++) {
                    if (element.list_item.id == dbids[i]) {
                        usedDBIDS.push(dbids[i]);
                        element.outOfDate = true;
                        temp[0].update = true;
                        break;
                    }
                }
            })
            cDB.saveDatabase;
            return usedDBIDS;
        }

        ///////////////////////////////////////////////////////
        ///             Account Manager         
        ///////////////////////////////////////////////////////

        cDB.addCMSAccounts = function (project, accounts) {
            var collection = cDB.accountDB.getCollection("CMSAccounts");
            if (collection == null) {
                collection = cDB.accountDB.addCollection("CMSAccounts");
                collection.insert({ name: project, data: accounts, update: false });
            } else {

                try {
                    var tempData = collection.find({ name: project });
                    tempData[0].data = accounts;
                    tempData[0].update = false;
                }
                catch (err) {
                    collection.insert({ name: project, data: accounts, update: false });
                }
            }
            cDB.saveDatabase();
        }

        cDB.setReloadCMSAccounts = function (project) {
            var collection = cDB.accountDB.getCollection("CMSAccounts");
            if (collection == null)
                return null;
            else {
                var temp = collection.find({ name: project });
                temp[0].update = true;
            }

            cDB.saveDatabase();
        }
        cDB.needReloadCMSAccounts = function (project) {
            var collection = cDB.accountDB.getCollection("CMSAccounts");
            if (collection == null)
                return null;


            var temp = collection.find({ name: project });
            return !jQuery.isEmptyObject(temp) ? temp[0].update : null;

        }

        cDB.getCMSAccounts = function (project, object) {
            object = typeof object !== 'undefined' ? object : false;
            if (!object) {
                var collection = cDB.accountDB.getCollection("CMSAccounts");
                if (collection == null)
                    return null;

                var temp = collection.find({ name: project });
                return !jQuery.isEmptyObject(temp) ? temp[0].data : null;

            }
            else {
                var collection = cDB.accountDB.getCollection("CMSAccounts");
                if (collection == null)
                    return null;

                var temp = collection.find({ name: project });
                return !jQuery.isEmptyObject(temp) ? temp[0] : null;

            }
        }

        cDB.addCMSAccountImageString = function (project, image_url, image_string) {

            $indexedDB.openStore('Pictures', function (store) {

                store.insert({ "PictureID": image_url, "ProjectID": project, "base64String": image_string }).then(function (e) { console.log("picture added") });

            });

        }
        cDB.getImageString = function (project, image_url) {

            $indexedDB.openStore('Pictures', function (photos) {
                // build query
                var find = photos.query();
                find = find.$eq(image_url);
                find = find.$index("PictureID");

                // update scope
                photos.eachWhere(find).then(function (e) {
                    if (e.length == 0) {

                        $rootScope.$broadcast("QueryImage", { image_url: image_url });

                    }
                    else {
                        $rootScope.$broadcast("TradeFairs_GetCMSImage", { data: {name:image_url, image_str: e[0].base64String }});
                    }
                });
            });

        }

        ///////////////////////////////////////////////////////
        ///             Store Manager         
        ///////////////////////////////////////////////////////

        cDB.addCMSStores = function (project, stores) {
            var collection = cDB.storeDB.getCollection("CMSStores");
            if (collection == null) {
                collection = cDB.storeDB.addCollection("CMSStores");
                collection.insert({ name: project, data: stores, update: false });
            } else {

                try {
                    var tempData = collection.find({ name: project });
                    tempData[0].data = stores;
                    tempData[0].update = false;
                }
                catch (err) {
                    collection.insert({ name: project, data: stores, update: false });
                }
            }
            cDB.saveDatabase();
        }

        cDB.setReloadCMSStores = function (project) {
            var collection = cDB.storeDB.getCollection("CMSStores");
            if (collection == null)
                return null;
            else {
                var temp = collection.find({ name: project });
                temp[0].update = true;
            }

            cDB.saveDatabase();
        }
        cDB.needReloadCMSStores = function (project) {
            var collection = cDB.storeDB.getCollection("CMSStores");
            if (collection == null)
                return null;


            var temp = collection.find({ name: project });
            return !jQuery.isEmptyObject(temp) ? temp[0].update : null;

        }

        cDB.getCMSStores = function (project, object) {
            object = typeof object !== 'undefined' ? object : false;
            if (!object) {
                var collection = cDB.storeDB.getCollection("CMSStores");
                if (collection == null)
                    return null;

                var temp = collection.find({ name: project });
                return !jQuery.isEmptyObject(temp) ? temp[0].data : null;

            }
            else {
                var collection = cDB.storeDB.getCollection("CMSStores");
                if (collection == null)
                    return null;

                var temp = collection.find({ name: project });
                return !jQuery.isEmptyObject(temp) ? temp[0] : null;

            }
        }

        cDB.addCMSStoreImageString = function (project, image_url, image_string) {

            $indexedDB.openStore('Pictures', function (store) {

                store.insert({ "PictureID": image_url, "ProjectID": project, "base64String": image_string }).then(function (e) { console.log("picture added") });

            });

        }
        cDB.getCMSStoreImageString = function (project, image_url) {

            $indexedDB.openStore('Pictures', function (store) {

                store.insert({ "PictureID": image_url, "ProjectID": project, "base64String": image_string }).then(function (e) { console.log("picture added") });

            });

        }

        cDB.addCMSStoreBGImageString = function (project, bg_image_url, bg_image_string) {

            $indexedDB.openStore('Pictures', function (store) {

                store.insert({ "PictureID": bg_image_url, "ProjectID": project, "base64String": bg_image_string }).then(function (e) { console.log("picture added") });

            });

        }

        cDB.addCMSStoreSectors = function (project, sectores) {
            var collection = cDB.storeDB.getCollection("CMSStoreSectors");
            if (collection == null) {
                collection = cDB.storeDB.addCollection("CMSStoreSectors");
                collection.insert({ name: project, data: sectores, update: false });
            } else {

                try {
                    var tempData = collection.find({ name: project });
                    tempData[0].data = sectores;
                    tempData[0].update = false;
                }
                catch (err) {
                    collection.insert({ name: project, data: sectores, update: false });
                }
            }
            cDB.saveDatabase();
        }



        cDB.getCMSStoreSectors = function (project, object) {
            object = typeof object !== 'undefined' ? object : false;
            if (!object) {
                var collection = cDB.storeDB.getCollection("CMSStoreSectors");
                if (collection == null)
                    return null;

                var temp = collection.find({ name: project });
                return !jQuery.isEmptyObject(temp) ? temp[0].data : null;

            }
            else {
                var collection = cDB.storeDB.getCollection("CMSStoreSectors");
                if (collection == null)
                    return null;

                var temp = collection.find({ name: project });
                return !jQuery.isEmptyObject(temp) ? temp[0] : null;

            }
        }

        cDB.addCMSStoreCategories = function (project, categories) {
            var collection = cDB.storeDB.getCollection("CMSStoreCategories");
            if (collection == null) {
                collection = cDB.storeDB.addCollection("CMSStoreCategories");
                collection.insert({ name: project, data: categories, update: false });
            } else {

                try {
                    var tempData = collection.find({ name: project });
                    tempData[0].data = categories;
                    tempData[0].update = false;
                }
                catch (err) {
                    collection.insert({ name: project, data: categories, update: false });
                }
            }
            cDB.saveDatabase();
        }



        cDB.getCMSStoreCategories = function (project, object) {
            object = typeof object !== 'undefined' ? object : false;
            if (!object) {
                var collection = cDB.storeDB.getCollection("CMSStoreCategories");
                if (collection == null)
                    return null;

                var temp = collection.find({ name: project });
                return !jQuery.isEmptyObject(temp) ? temp[0].data : null;

            }
            else {
                var collection = cDB.storeDB.getCollection("CMSStoreCategories");
                if (collection == null)
                    return null;

                var temp = collection.find({ name: project });
                return !jQuery.isEmptyObject(temp) ? temp[0] : null;

            }
        }


        ///////////////////////////////////////////////////////
        ///             Booth Manager         
        ///////////////////////////////////////////////////////

        cDB.addCMSBooths = function (project, booths) {
            var collection = cDB.boothDB.getCollection("CMSBooths");
            if (collection == null) {
                collection = cDB.boothDB.addCollection("CMSBooths");
                collection.insert({ name: project, data: booths, update: false });
            } else {

                try {
                    var tempData = collection.find({ name: project });
                    tempData[0].data = booths;
                    tempData[0].update = false;
                }
                catch (err) {
                    collection.insert({ name: project, data: booths, update: false });
                }
            }
            cDB.saveDatabase();
        }

        cDB.setReloadCMSBooths = function (project) {
            var collection = cDB.boothDB.getCollection("CMSBooths");
            if (collection == null)
                return null;
            else {
                var temp = collection.find({ name: project });
                temp[0].update = true;
            }

            cDB.saveDatabase();
        }
        cDB.needReloadCMSBooths = function (project) {
            var collection = cDB.boothDB.getCollection("CMSBooths");
            if (collection == null)
                return null;


            var temp = collection.find({ name: project });
            return !jQuery.isEmptyObject(temp) ? temp[0].update : null;

        }

        cDB.getCMSBooths = function (project, object) {
            object = typeof object !== 'undefined' ? object : false;
            if (!object) {
                var collection = cDB.boothDB.getCollection("CMSBooths");
                if (collection == null)
                    return null;

                var temp = collection.find({ name: project });
                return !jQuery.isEmptyObject(temp) ? temp[0].data : null;

            }
            else {
                var collection = cDB.boothDB.getCollection("CMSBooths");
                if (collection == null)
                    return null;

                var temp = collection.find({ name: project });
                return !jQuery.isEmptyObject(temp) ? temp[0] : null;

            }
        }

        cDB.addCMSBoothsImageString = function (project, image_url, image_string) {

            $indexedDB.openStore('Pictures', function (store) {

                store.insert({ "PictureID": image_url, "ProjectID": project, "base64String": image_string }).then(function (e) { console.log("picture added") });

            });

        }

        ///////////////////////////////////////////////////////
        ///             Beacon Manager         
        ///////////////////////////////////////////////////////

        cDB.addCMSBeacons = function (project, beacon) {
            var collection = cDB.beaconDB.getCollection("CMSBeacons");
            if (collection == null) {
                collection = cDB.beaconDB.addCollection("CMSBeacons");
                collection.insert({ name: project, data: beacon, update: false });
            } else {

                try {
                    var tempData = collection.find({ name: project });
                    tempData[0].data = beacon;
                    tempData[0].update = false;
                }
                catch (err) {
                    collection.insert({ name: project, data: beacon, update: false });
                }
            }
            cDB.saveDatabase();
        }

        cDB.setReloadCMSBeacons = function (project) {
            var collection = cDB.beaconDB.getCollection("CMSBeacons");
            if (collection == null)
                return null;
            else {
                var temp = collection.find({ name: project });
                temp[0].update = true;
            }

            cDB.saveDatabase();
        }
        cDB.needReloadCMSBeacons = function (project) {
            var collection = cDB.beaconDB.getCollection("CMSBeacons");
            if (collection == null)
                return null;


            var temp = collection.find({ name: project });
            return !jQuery.isEmptyObject(temp) ? temp[0].update : null;

        }

        cDB.getCMSBeacons = function (project, object) {
            object = typeof object !== 'undefined' ? object : false;
            if (!object) {
                var collection = cDB.beaconDB.getCollection("CMSBeacons");
                if (collection == null)
                    return null;

                var temp = collection.find({ name: project });
                return !jQuery.isEmptyObject(temp) ? temp[0].data : null;

            }
            else {
                var collection = cDB.beaconDB.getCollection("CMSBeacons");
                if (collection == null)
                    return null;

                var temp = collection.find({ name: project });
                return !jQuery.isEmptyObject(temp) ? temp[0] : null;

            }
        }

        cDB.addCMSBeaconsImageString = function (project, image_url, image_string) {

            $indexedDB.openStore('Pictures', function (store) {

                store.insert({ "PictureID": image_url, "ProjectID": project, "base64String": image_string }).then(function (e) { console.log("picture added") });

            });

        }


        ///////////////////////////////////////////////////////
        ///             newsFeedDB Manager         
        ///////////////////////////////////////////////////////


        cDB.addCMSNewsFeed = function (project, newsfeed) {
            var collection = cDB.newsFeedDB.getCollection("CMSNewsFeeds");
            if (collection == null) {
                collection = cDB.newsFeedDB.addCollection("CMSNewsFeeds");
                collection.insert({ name: project, data: newsfeed, update: false });
            } else {

                try {
                    var tempData = collection.find({ name: project });
                    tempData[0].data = newsfeed;
                    tempData[0].update = false;
                }
                catch (err) {
                    collection.insert({ name: project, data: newsfeed, update: false });
                }
            }
            cDB.saveDatabase();
        }

        cDB.setReloadCMSNewsFeed = function (project) {
            var collection = cDB.newsFeedDB.getCollection("CMSNewsFeeds");
            if (collection == null)
                return null;
            else {
                var temp = collection.find({ name: project });
                temp[0].update = true;
            }

            cDB.saveDatabase();
        }
        cDB.needReloadCMSNewsFeed = function (project) {
            var collection = cDB.newsFeedDB.getCollection("CMSNewsFeeds");
            if (collection == null)
                return null;


            var temp = collection.find({ name: project });
            return !jQuery.isEmptyObject(temp) ? temp[0].update : null;

        }

        cDB.getCMSNewsFeed = function (project, object) {
            object = typeof object !== 'undefined' ? object : false;
            if (!object) {
                var collection = cDB.newsFeedDB.getCollection("CMSNewsFeeds");
                if (collection == null)
                    return null;

                var temp = collection.find({ name: project });
                return !jQuery.isEmptyObject(temp) ? temp[0].data : null;

            }
            else {
                var collection = cDB.newsFeedDB.getCollection("CMSNewsFeeds");
                if (collection == null)
                    return null;

                var temp = collection.find({ name: project });
                return !jQuery.isEmptyObject(temp) ? temp[0] : null;

            }
        }

        cDB.addCMSNewsFeedImageString = function (project, image_url, image_string) {

            $indexedDB.openStore('Pictures', function (store) {

                store.insert({ "PictureID": image_url, "ProjectID": project, "base64String": image_string }).then(function (e) { console.log("picture added") });

            });

        }



        return cDB;
    
}]);

if (typeof module !== 'undefined' && typeof exports !== 'undefined' && module.exports === exports) {
    module.exports = 'ClientDB';
}


