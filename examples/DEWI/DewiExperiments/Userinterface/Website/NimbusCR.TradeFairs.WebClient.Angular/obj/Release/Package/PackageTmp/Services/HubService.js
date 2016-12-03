'use strict';
/*
 * Contains a service to communicate with the TRACK TV API
 */
angular.module('app.services').factory('CoreHubService', ['$', "$rootScope", "Hub", "cDB", '$location',
    function ($, $rootScope, Hub, cDB, $location) {
        var parent = $rootScope;
        var clientDB = cDB;
        var CoreHub = new Hub("TradeFairsHub", {
            listeners: {
                "setReceiveMessage": function (callback, data) {
                    if (typeof data === "undefined") {
                        data = callback;
                    }

                    if (data.length == 5 && data[0] == 'split') {
                        //split 

                        var guid = data[1];
                        var data_parts_num = data[2];
                        var index = data[3];
                        var data_part = data[4];

                        data = CoreHub.SetCacheObject(guid, data_parts_num, index, data_part);

                        if (typeof data == "undefined")
                            return;
                    }


                    var exchangeName = data['exchangeName'];
                    var serviceType = data['serviceType'];
                    var dataType = data['dataType'];
                    var dataParameter = data['dataParameter'];
                    var result = {};


                    console.log("[HubService]: Received Message from: " + serviceType);
                    switch (serviceType) {

                        /////////////////////////////////////////////////////////////////////////
                        /////////////////////////////////////////////////////////////////////////
                        //General Hub functions
                        /////////////////////////////////////////////////////////////////////////
                        /////////////////////////////////////////////////////////////////////////
                        case "ClientLogin":
                            clientDB.loadDatabase();
                            parent.$broadcast("setCookie", { data: dataParameter });
                            break;

                        case "TradeFairs_GetCMSImage":
                            if (dataType == "protobuf_zip") {
                                if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                                    var myS_S_object = CoreHub.ProtoDecode_zip(dataParameter, "S_CMSImage");

                                    result = new S_CMSImageJS(myS_S_object);
                                }
                                else
                                    result = null;
                            }
                            else if (dataType == "json") {
                                result = JSON.parse(dataParameter);
                            }
                            else if (dataType == "binary") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_GetCMSImage", { data: result });
                            break;
                        case "TradeFairs_GetCMSProjectSetting":

                            if (dataType == "protobuf_zip") {
                                if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                                    var myS_S_Object = CoreHub.ProtoDecode_zip(dataParameter, "S_CMSProjectSetting");

                                    result = new S_CMSProjectSettingJS(myS_S_Object);
                                }
                                else
                                    result = null;
                            }
                            else if (dataType == "json") {
                                result = JSON.parse(dataParameter);
                            }
                            else if (dataType == "binary") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_GetCMSProjectSetting", { data: result });
                            break;

                        case "GetProjectProfile":
                            var result = null;

                            if (dataType == "protobuf_zip") {
                                if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                                    var myS_S_Object = CoreHub.ProtoDecode_zip(dataParameter, "S_TransformableObjectProfile");

                                    result = new S_TransformableObjectProfileJS(myS_S_Object);
                                }
                                else
                                    result = null;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("GetProjectProfile", { data: result });
                            break;
                        case "Context_GetTransformableObject":
                            var result = null;

                            if (dataType == "protobuf_zip") {
                                if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                                    var myS_S_Object = CoreHub.ProtoDecode_zip(dataParameter, "S_CMSTransformableObject");

                                    result = new S_CMSTransformableObjectJS(myS_S_Object);

                                    //  var eoPolyAreas = [];
                                    //  for (var i in result.polyAreas) {
                                    //      eoPolyAreas.push(result.polyAreas[i].polyArea);
                                    //  }

                                    var eoPolyAreas = result.polyAreas;
                                    result = result.transformableObject;
                                    result.polyAreas = eoPolyAreas;

                                }
                                else
                                    result = null;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("Context_GetTransformableObject", { data: result });
                            break;

                            /////////////////////////////////////////////////////////////////////////
                            /////////////////////////////////////////////////////////////////////////
                            //Profile Account functions
                            /////////////////////////////////////////////////////////////////////////
                            /////////////////////////////////////////////////////////////////////////

                        case "TradeFairs_GetCMSAccountByName":
                            if (dataType == "protobuf_zip") {
                                if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                                    var myS_S_object = CoreHub.ProtoDecode_zip(dataParameter, "S_CMSAccount");

                                    result = new S_CMSAccountJS(myS_S_object);
                                }
                                else
                                    result = null;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_GetCMSAccountByName", { data: result });

                            break;

                        case "TradeFairs_GetCMSAccounts":

                            if (dataType == "protobuf_zip") {
                                if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                                    var myS_S_object = CoreHub.ProtoDecode_zip(dataParameter, "ListCMSAccount");

                                    result = new ListCMSAccountJS(myS_S_object);
                                }
                                else
                                    result = null;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_GetCMSAccounts", { data: result.items });
                            break;

                        case "TradeFairs_UpdateCMSAccount":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_UpdateCMSAccount", { data: result });
                            break;
                        case "TradeFairs_CMSUsernameExist":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_CMSUsernameExist", { data: result });
                            break;

                        case "TradeFairs_DeleteCMSAccounts":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_DeleteCMSAccounts", { data: result });
                            break;

                        case "TradeFairs_UpdateCMSAccountLastLoginProject":
                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_UpdateCMSAccountLastLoginProject", { data: result });
                            break;

                            /////////////////////////////////////////////////////////////////////////
                            /////////////////////////////////////////////////////////////////////////
                            //Store Manager Functions
                            /////////////////////////////////////////////////////////////////////////
                            /////////////////////////////////////////////////////////////////////////
                        case "TradeFairs_GetCMSStores":
                        case "TradeFairs_GetCMSStoresOfLinkedProfile":
                            if (dataType == "protobuf_zip") {

                                if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                                    var myS_S_object = CoreHub.ProtoDecode_zip(dataParameter, "ListCMSStore");

                                    result = new ListCMSStoreJS(myS_S_object);
                                }
                                else {
                                    result = null;
                                }
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            if (result == null) {
                                parent.$broadcast("TradeFairs_GetCMSStores", { data: null });
                                parent.$broadcast("TradeFairs_GetCMSStoresOfLinkedProfile", { data: null });
                            }
                            else {
                                parent.$broadcast("TradeFairs_GetCMSStores", { data: result.items });
                                parent.$broadcast("TradeFairs_GetCMSStoresOfLinkedProfile", { data: result.items });
                            }

                            break;
                        case "TradeFairs_UpdateCMSStore":
                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }

                            parent.$broadcast("TradeFairs_UpdateCMSStore", { data: result });

                            break;
                        case "TradeFairs_DeleteCMSStores":
                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }

                            parent.$broadcast("TradeFairs_DeleteCMSStores", { data: result });
                            break;

                            /////////////////////////////////////////////////////////////////////////
                            /////////////////////////////////////////////////////////////////////////
                            //Booth fucntions
                            /////////////////////////////////////////////////////////////////////////
                            /////////////////////////////////////////////////////////////////////////
                        case "TradeFairs_GetCMSBooths":
                        case "TradeFairs_GetCMSBoothsOfStore":

                            if (dataType == "protobuf_zip") {
                                if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                                    var myS_S_object = CoreHub.ProtoDecode_zip(dataParameter, "ListCMSBooth");

                                    result = new ListCMSBoothJS(myS_S_object);
                                }
                                else
                                    result = null;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }

                            if (result == null) {
                                parent.$broadcast("TradeFairs_GetCMSBooths", { data: null });
                                parent.$broadcast("TradeFairs_GetCMSBoothsOfStore", { data: null });
                            }
                            else {
                                parent.$broadcast("TradeFairs_GetCMSBooths", { data: result.items });
                                parent.$broadcast("TradeFairs_GetCMSBoothsOfStore", { data: result.items });
                            }
                            break;
                        case "TradeFairs_GetCMSBoothHalls":

                            if (dataType == "protobuf_zip") {
                                if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                                    var myS_S_object = CoreHub.ProtoDecode_zip(dataParameter, "ListString");

                                    result = new ListCMSBoothJS(myS_S_object);
                                }
                                else
                                    result = null;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_GetCMSBoothHalls", { data: result });
                            break;
                        case "TradeFairs_UpdateCMSBooth":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_UpdateCMSBooth", { data: result });
                            break;
                        case "TradeFairs_DeleteCMSBooths":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_DeleteCMSBooths", { data: result });
                            break;


                            /////////////////////////////////////////////////////////////////////////
                            /////////////////////////////////////////////////////////////////////////
                            //Profile Categories fucntions
                            /////////////////////////////////////////////////////////////////////////
                            /////////////////////////////////////////////////////////////////////////
                        case "TradeFairs_GetProfileCategories":
                            if (dataType == "protobuf_zip") {
                                if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                                    var myS_S_object = CoreHub.ProtoDecode_zip(dataParameter, "ListCMSProfileCategory");

                                    result = new ListCMSProfileCategoryJS(myS_S_object);
                                }
                                else
                                    result = new ListCMSProfileCategoryJS();
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            if (result == null)
                                parent.$broadcast("TradeFairs_GetProfileCategories", { data: null });
                            else
                                parent.$broadcast("TradeFairs_GetProfileCategories", { data: result.items });

                            break;
                        case "TradeFairs_UpdateCMSProfileCategory":
                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }

                            parent.$broadcast("TradeFairs_UpdateCMSProfileCategory", { data: result });
                            break;
                        case "TradeFairs_DeleteCMSProfileCategories":
                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }

                            parent.$broadcast("TradeFairs_DeleteCMSProfileCategories", { data: result });
                            break;
                        case "TradeFairs_GetCountryFlags":

                            if (dataType == "protobuf_zip") {
                                if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                                    result = CoreHub.ProtoDecode_zip(dataParameter, "DictionaryStringString");
                                }
                                else
                                    result = [];
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_GetCountryFlags", { data: result });
                            break;
                        case "TradeFairs_GetCountryFlag":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_GetCountryFlag", { data: result });
                            break;
                        case "TradeFairs_DecodeCountryName":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_DecodeCountryName", { data: result });
                            break;


                            /////////////////////////////////////////////////////////////////////////
                            /////////////////////////////////////////////////////////////////////////
                            //Beacon fucntions
                            /////////////////////////////////////////////////////////////////////////
                            /////////////////////////////////////////////////////////////////////////

                        case "TradeFairs_GetCMSBeacons":

                            if (dataType == "protobuf_zip") {
                                if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                                    var myS_S_object = CoreHub.ProtoDecode_zip(dataParameter, "ListCMSBeacon");

                                    result = new ListCMSBeaconJS(myS_S_object);
                                }
                                else
                                    result = null;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }


                            if (result == null) {
                                parent.$broadcast("TradeFairs_GetCMSBeacons", { data: null });
                            }
                            else {
                                parent.$broadcast("TradeFairs_GetCMSBeacons", { data: result.items });
                            }
                            parent.$broadcast("TradeFairs_GetCMSBeacons", { data: result.items });
                            break;
                        case "TradeFairs_UpdateCMSBeacon":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_UpdateCMSBeacon", { data: result });
                            break;
                        case "TradeFairs_DeleteCMSBeacons":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_DeleteCMSBeacons", { data: result });
                            break;


                            /////////////////////////////////////////////////////////////////////////
                            /////////////////////////////////////////////////////////////////////////
                            //News feed fucntions
                            /////////////////////////////////////////////////////////////////////////
                            /////////////////////////////////////////////////////////////////////////

                        case "TradeFairs_GetCMSNewsFeeds":
                            if (dataType == "protobuf_zip") {
                                if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                                    var myS_S_object = CoreHub.ProtoDecode_zip(dataParameter, "ListCMSNewsFeed");

                                    result = new ListCMSNewsFeedJS(myS_S_object);
                                }
                                else
                                    result = null;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }

                            
                            if (result == null) {
                                parent.$broadcast("TradeFairs_GetCMSNewsFeeds", { data: null });
                            }
                            else {
                                parent.$broadcast("TradeFairs_GetCMSNewsFeeds", { data: result.items });
                            }
                            break;
                        case "TradeFairs_UpdateCMSNewsFeed":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_UpdateCMSNewsFeed", { data: result });
                            break;
                        case "TradeFairs_DeleteCMSNewsFeed":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_DeleteCMSNewsFeed", { data: result });
                            break;


                            /////////////////////////////////////////////////////////////////////////
                            /////////////////////////////////////////////////////////////////////////
                            //MenuItem fucntions
                            /////////////////////////////////////////////////////////////////////////
                            /////////////////////////////////////////////////////////////////////////

                        case "TradeFairs_GetMenuItems":

                            if (dataType == "protobuf_zip") {
                                if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                                    var myS_S_Object = CoreHub.ProtoDecode_zip(dataParameter, "ListCMSMenuItem");

                                    result = new ListCMSMenuItemJS(myS_S_Object);
                                }
                                else
                                    result = null;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            if(result)
                            parent.$broadcast("TradeFairs_GetMenuItems", { data: result.items });
                            break;
                        case "TradeFairs_UpdateMenuItem":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_UpdateMenuItem", { data: result });
                            break;
                        case "TradeFairs_DeleteMenuItems":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_DeleteMenuItems", { data: result });
                            break;


                            /////////////////////////////////////////////////////////////////////////
                            /////////////////////////////////////////////////////////////////////////
                            //Race fucntions
                            /////////////////////////////////////////////////////////////////////////
                            /////////////////////////////////////////////////////////////////////////
                        case "TradeFairs_GetCMSRaces":

                            if (dataType == "protobuf_zip") {
                                if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                                    var myS_S_Object = CoreHub.ProtoDecode_zip(dataParameter, "ListCMSRace");

                                    result = new ListCMSRaceJS(myS_S_Object);
                                }
                                else
                                    result = null;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_GetCMSRaces", { data: result.items });
                            break;
                        case "TradeFairs_GetCMSRaceResults":

                            if (dataType == "protobuf_zip") {
                                if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                                    var myS_S_Object = CoreHub.ProtoDecode_zip(dataParameter, "ListCMSRaceResult");

                                    result = new ListCMSRaceResultJS(myS_S_Object);
                                }
                                else
                                    result = null;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_GetCMSRaceResults", { data: result.items });
                            break;
                        case "TradeFairs_UpdateCMSRace":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_UpdateCMSRace", { data: result });
                            break;
                        case "TradeFairs_UpdateCMSRaceResults":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_UpdateCMSRaceResults", { data: result });
                            break;
                        case "TradeFairs_DeleteCMSRace":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_DeleteCMSRace", { data: result });
                            break;
                        case "TradeFairs_DeleteCMSRaceResults":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_DeleteCMSRaceResults", { data: result });
                            break;
                        case "TradeFairs_DeleteCMSRaceResultsByCategory":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_DeleteCMSRaceResultsByCategory", { data: result });
                            break;


                            /////////////////////////////////////////////////////////////////////////
                            /////////////////////////////////////////////////////////////////////////
                            //Seminar and Event fucntions
                            /////////////////////////////////////////////////////////////////////////
                            /////////////////////////////////////////////////////////////////////////

                        case "TradeFairs_GetCMSSeminars":

                            if (dataType == "protobuf_zip") {
                                if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                                    var myS_S_Object = CoreHub.ProtoDecode_zip(dataParameter, "ListCMSSeminar");

                                    result = new ListCMSSeminarJS(myS_S_Object);
                                }
                                else
                                    result = null;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_GetCMSSeminars", { data: result });
                            break;
                        case "TradeFairs_UpdateCMSSeminar":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_UpdateCMSSeminar", { data: result });
                            break;
                        case "TradeFairs_DeleteCMSSeminars":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_DeleteCMSSeminars", { data: result });
                            break;
                        case "TradeFairs_UpdateCMSSpeaker":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_UpdateCMSSpeaker", { data: result });
                            break;
                        case "TradeFairs_GetCMSSpeakersOfSeminar":
                        case "TradeFairs_GetCMSSpeakers":

                            if (dataType == "protobuf_zip") {
                                if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                                    var myS_S_Object = CoreHub.ProtoDecode_zip(dataParameter, "ListCMSSpeaker");

                                    result = new ListCMSSpeakerJS(myS_S_Object);
                                }
                                else
                                    result = null;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_GetCMSSpeakersOfSeminar", { data: result });
                            parent.$broadcast("TradeFairs_GetCMSSpeakers", { data: result });
                            break;
                        case "TradeFairs_DeleteCMSSpeakers":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_DeleteCMSSpeakers", { data: result });
                            break;
                        case "TradeFairs_GetCMSEventGroups":

                            if (dataType == "protobuf_zip") {
                                if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                                    var myS_S_Object = that.ProtoDecode_zip(dataParameter, "ListCMSEventGroup");

                                    result = new ListCMSEventGroupJS(myS_S_Object);
                                }
                                else
                                    result = null;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_GetCMSEventGroups", { data: result });
                            break;
                        case "TradeFairs_UpdateCMSEventGroup":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_UpdateCMSEventGroup", { data: result });
                            break;
                        case "TradeFairs_DeleteCMSEventGroupSubgroup":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_DeleteCMSEventGroupSubgroup", { data: result });
                            break;
                        case "TradeFairs_GetCMSEventLocations":

                            if (dataType == "protobuf_zip") {
                                if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                                    var myS_S_Object = that.ProtoDecode_zip(dataParameter, "ListCMSEventLocation");

                                    result = new ListCMSEventLocationJS(myS_S_Object);
                                }
                                else
                                    result = null;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_GetCMSEventLocations", { data: result });
                            break;
                        case "TradeFairs_UpdateCMSEventLocation":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_UpdateCMSEventLocation", { data: result });
                            break;
                        case "TradeFairs_DeleteCMSEventLocations":

                            if (dataType == "protobuf_zip") {
                                result = dataParameter;
                            }
                            else if (dataType == "json") {
                                result = dataParameter;
                            }
                            parent.$broadcast("TradeFairs_DeleteCMSEventLocations", { data: result });
                            break;
                    }
                    result = {};
                },

                "setReceiveBroadcast": function (message) {
                    console.log(message);

                    var exchangeName = message['exchangeName'];
                    var serviceType = message['serviceType'];
                    var dataType = message['dataType'];
                    var dataParameter = message['dataParameter'];
                    var result = {};
                    if (dataType == "protobuf_zip") {
                        if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                            var myS_S_object = CoreHub.ProtoDecode_zip(dataParameter, "S_CMSDataUpdateBroadcast");

                            result = new S_CMSDataUpdateBroadcastJS(myS_S_object);
                        }
                        else
                            result = null;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    switch (exchangeName) {

                        case "SocialContext_SocialUpdateRequest":
                            switch (serviceType) {
                                case "Context_ReloadSocialDB":
                                    parent.$broadcast("Context_ReloadSocialDB", { data: result });
                                    break;
                            }
                            break;
                        case "UserUpdates":
                            switch (serviceType) {
                                case "ReloadUserAccount":
                                    parent.$broadcast("ReloadUserAccount", { data: result });
                                    break;
                            }
                            break;
                        case "Context_DataMiningUpdateRequest":
                            switch (serviceType) {
                                case "Reload3DData":
                                    parent.$broadcast("Reload3DData", { data: result });
                                    break;
                                case "Context_ReloadDataMiningDB":
                                    parent.$broadcast("Context_ReloadDataMiningDB", { data: result });
                                    break;
                            }
                            break;
                        case "TradeFairsUpdateRequest":
                            switch (serviceType) {
                                case "TradeFairs_ReloadTradefairsDB":
                                    parent.$broadcast("TradeFairs_ReloadTradefairsDB", { data: result });
                                    break;
                            }
                            break;
                    }

                },

                "setErrorMessage": function (data) {
                    if (data.dataType == "Login Failed") {
                        parent.$broadcast("TradeFairs_FailedLogin", data);
                    }
                    //console.log(data);

                }
            },
            hubDisconnected: function () {
                if (CoreHub.connection.lastError) {
                    CoreHub.connection.start();
                }
            },
            useSharedConnection: false,
            rootPath: 'http://157.190.53.99:12128',
            methods: ['ClientLogin', 'Query', 'Subscribe'],
            errorHandler: function (error) {
                console.error(error);
            },
            stateChanged: function (state) {
                switch (state.newState) {
                    case $.signalR.connectionState.connecting:
                        //your code here
                        break;
                    case $.signalR.connectionState.connected:
                        //your code here
                        break;
                    case $.signalR.connectionState.reconnecting:
                        //your code here
                        break;
                    case $.signalR.connectionState.disconnected:
                        //your code here
                        break;
                }
            },
            logging: true
        });

        CoreHub.splitObjectCache = {};

        CoreHub.SetCacheObject = function (guid, data_parts_num, index, part) {
            if (typeof this.splitObjectCache[guid] == "undefined") {
                CoreHub.splitObjectCache[guid] = {};
                CoreHub.splitObjectCache[guid].parts_num = data_parts_num;
                CoreHub.splitObjectCache[guid].parts = [];
                CoreHub.splitObjectCache[guid].remaining = [];
                for (var i = 0; i < data_parts_num; i++) {
                    CoreHub.splitObjectCache[guid].parts[i] = "";
                    CoreHub.splitObjectCache[guid].remaining[i] = i;
                }
            }

            if (typeof CoreHub.splitObjectCache[guid].parts[index] != "undefined")
                CoreHub.splitObjectCache[guid].parts[index] = part;

            var index_int = parseInt(index);
            var i = CoreHub.splitObjectCache[guid].remaining.indexOf(index_int);
            if (i != -1) {
                CoreHub.splitObjectCache[guid].remaining.splice(i, 1);
            }

            if (CoreHub.splitObjectCache[guid].remaining.length == 0) {
                var jsondata = "";
                for (var i in CoreHub.splitObjectCache[guid].parts)
                    jsondata += CoreHub.splitObjectCache[guid].parts[i];

                CoreHub.splitObjectCache[guid] = null;
                delete CoreHub.splitObjectCache[guid]

                return JSON.parse(jsondata);
            }
            return;
        }

        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        //General Hub functions
        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////

        CoreHub.login = function (username, password) {
            CoreHub.ClientLogin(username, password);
        }

        CoreHub.SubscribeToBroadcast = function (subscriptionconfig) {
            var id = cDB.getAuthID();
            CoreHub.Subscribe(id,"", subscriptionconfig);
        }

        CoreHub.GetProfiles = function (project_id) {
            var id = cDB.getAuthID();
            var dataParameter = project_id;
            CoreHub.SendDataParts(id, "CoreHubGatewayQuery", "GetProjectProfile", dataParameter, "protobuf_zip", "");
        }

        CoreHub.GetTransformableObject = function (S_EOIdentifierJS) {
            var dataParameter = CoreHub.ProtoEncode_zip(S_EOIdentifierJS, "S_EOIdentifier");
            var id = cDB.getAuthID();
            CoreHub.SendDataParts(id, "CoreHubGatewayQuery", "GetTransformableObject", dataParameter, "protobuf_zip", "");
        }

        CoreHub.GetProjectProfile = function (project_id) {
            var dataParameter = [project_id];
            var id = cDB.getAuthID();
            CoreHub.SendDataParts(id, "CoreHubGatewayQuery", "GetProjectProfile", dataParameter, "protobuf_zip", "");
        }

        CoreHub.GetCMSImage = function (name, size) {
            var dataParameter = [name, size];
            var id = cDB.getAuthID();
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSImage", dataParameter, "protobuf_zip", "");
        }

        CoreHub.GetCMSProjectSetting = function (project_id) {     
            console.log("[Core Hub]: GetProjectSetting: " + project_id);
            var id = cDB.getAuthID();
            var dataParameter = [project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSProjectSetting", dataParameter, "protobuf_zip", "");
        }        

        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        //Profile Account functions
        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////

        CoreHub.GetCMSAccounts = function (project_id) {            
            var id = cDB.getAuthID();
            var dataParameter = [project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSAccounts", dataParameter, "protobuf_zip", "");

        }

        CoreHub.GetCMSAccountsByName = function (username, project_id) {
            var id = cDB.getAuthID();
            var dataParameter = [username, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSAccountByName", dataParameter, "protobuf_zip", "");
        }

        CoreHub.UpdateCMSAccounts = function (account, project_id) {
            var id = cDB.getAuthID();
            var img_str = account.image_string;           
            var account_ = CoreHub.ProtoEncode_zip(account, "S_CMSAccount");
            var dataParameter = [account_, project_id, img_str];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSAccount", dataParameter, "protobuf_zip", "");
        }

        CoreHub.UpdateCMSAccountLastLoginProject = function (username, last_login_project) {
            var id = cDB.getAuthID();
            var dataParameter = [username, last_login_project];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSAccountLastLoginProject", dataParameter, "protobuf_zip", "");
        }

        CoreHub.DeleteCMSAccounts = function (usernames, project_id) {
            var id = cDB.getAuthID();
            var usernames_ = CoreHub.ProtoEncode_zip(usernames, "ListString");
            var dataParameter = [usernames_, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSAccounts", dataParameter, "protobuf_zip", "");     
        }

        CoreHub.CMSUsernameExist = function (username, project_id) {
            var id = cDB.getAuthID();
            var dataParameter = [usernames, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_CMSUsernameExist", dataParameter, "protobuf_zip", "");
        }

        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        //Store Manager Functions
        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////

        CoreHub.GetCMSStores = function (project_id) {
            var id = cDB.getAuthID();
            var dataParameter = [project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSStores", dataParameter, "protobuf_zip", "");

        }

        CoreHub.GetCMSStoresOfLinkedProfile = function (profile_id, project_id) {
            var id = cDB.getAuthID();
            var dataParameter = [profile_id.toString(), project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSStoresOfLinkedProfile", dataParameter, "protobuf_zip", "");
        }

        CoreHub.UpdateCMSStore = function (store, project_id) {
            var id = cDB.getAuthID();
            var img_str = store.image_string;
            var store_ = CoreHub.ProtoEncode_zip(store, "S_CMSStore");
            var dataParameter = [store_, project_id, img_str];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSStore", dataParameter, "protobuf_zip", "");
        }

        CoreHub.DeleteCMSStore = function (store_id, project_id) {
            var id = cDB.getAuthID();
            var store_ = CoreHub.ProtoEncode_zip(parseInt(store_id), "ListInt");
            var dataParameter = [store_, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSStores", dataParameter, "protobuf_zip", "");
        }

        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        //Booth fucntions
        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////

        CoreHub.GetCMSBoothHalls = function (project_id) {
            var id = cDB.getAuthID();
            var dataParameter = [project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSBoothHalls", dataParameter, "protobuf_zip", "");
        }

        CoreHub.GetCMSBooths = function (project_id) {
            var id = cDB.getAuthID();
            var dataParameter = [project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSBooths", dataParameter, "protobuf_zip", "");
        }
        CoreHub.GetCMSBoothsOfStore = function (store_id, project_id) {
            var id = cDB.getAuthID();
            var dataParameter = [store_id, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSBoothsOfStore", dataParameter, "protobuf_zip", "");
        }
        CoreHub.UpdateCMSBooth = function (booth, project_id) {
            var id = cDB.getAuthID();
            var booth_ = CoreHub.ProtoEncode_zip(booth, "S_CMSBooth");
            var dataParameter = [booth_, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSBooth", dataParameter, "protobuf_zip", "");
        }
        CoreHub.DeleteCMSBooths = function (booths_ids, project_id) {
            var id = cDB.getAuthID();
            var boothsids_ = CoreHub.ProtoEncode_zip(booths_ids, "ListInt");
            var dataParameter = [boothsids_, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSBooths", dataParameter, "protobuf_zip", "");
        }



        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        //Profile Categories fucntions
        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////

        //type: "product", "sector", "profile"
        CoreHub.GetCMSProfileCategories = function (project_id, profileCategoryType) {
            var id = cDB.getAuthID();
            var dataParameter = [project_id, profileCategoryType];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetProfileCategories", dataParameter, "protobuf_zip", "");
        }

        //type: "product", "sector", "profile"
        CoreHub.UpdateCMSProfileCategory = function (profile_category, project_id) {
            var id = cDB.getAuthID();
            var profile_category_ = CoreHub.ProtoEncode_zip(profile_category, "S_CMSProfileCategory");
            var dataParameter = [profile_category_, project_id];
            console.log(dataParameter);
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSProfileCategory", dataParameter, "protobuf_zip", "");
        }

        //type: "product", "sector", "profile"
        CoreHub.DeleteCMSProfileCategories = function (profileCategories_ids, profileCategoryType, project_id) {
            var id = cDB.getAuthID();
            var profileCategories_ids_ = CoreHub.ProtoEncode_zip(parseInt(profileCategories_ids), "ListInt");
            var dataParameter = [profileCategories_ids_, profileCategoryType, project_id];
            console.log(dataParameter);
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSProfileCategories", dataParameter, "protobuf_zip", "");
        }

        CoreHub.GetCountryFlags = function (project_id) {
            var id = cDB.getAuthID();
            var dataParameter = [project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCountryFlags", dataParameter, "protobuf_zip", "");
        }

        CoreHub.GetCountryFlag = function (project_id, country_name) {
            var id = cDB.getAuthID();
            var dataParameter = [project_id, country_name];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCountryFlag", dataParameter, "protobuf_zip", "");
        }

        CoreHub.DecodeCountryName = function (project_id, country_name) {
            var id = cDB.getAuthID();
            var dataParameter = [project_id, country_name];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_DecodeCountryName", dataParameter, "protobuf_zip", "");
        }


        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        //Beacon fucntions
        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        CoreHub.GetCMSBeacons = function (project_id) {
            var id = cDB.getAuthID();
            var dataParameter = [project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSBeacons", dataParameter, "protobuf_zip", "");
        }

        CoreHub.UpdateCMSBeacons = function (beacon, project_id) {
            var id = cDB.getAuthID();
            var beacon_ = CoreHub.ProtoEncode_zip(beacon, "S_CMSBeacon");
            var dataParameter = [beacon_, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSBeacon", dataParameter, "protobuf_zip", "");
        }

        CoreHub.DeleteCMSBeacons = function (beacon_ids, project_id) {
            var id = cDB.getAuthID();
            var beacon_ids_ = CoreHub.ProtoEncode_zip(beacon_ids, "ListInt");
            var dataParameter = [beacon_ids_, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSBeacons", dataParameter, "protobuf_zip", "");
        }

        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        //News feed fucntions
        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        CoreHub.GetCMSNewsFeeds = function (project_id, store_id) {
            var id = cDB.getAuthID();
            var dataParameter = [project_id, store_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSNewsFeeds", dataParameter, "protobuf_zip", "");
        }

        CoreHub.UpdateCMSNewsFeed = function (message, project_id) {
            var id = cDB.getAuthID();
            var message_ = CoreHub.ProtoEncode_zip(message, "S_CMSNewsFeed");
            var dataParameter = [message_, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSNewsFeed", dataParameter, "protobuf_zip", "");
        }

        CoreHub.DeleteCMSNewsFeed = function (message_ids, project_id) {
            var id = cDB.getAuthID();
            var message_ids_ = CoreHub.ProtoEncode_zip(message_ids, "ListInt");
            var dataParameter = [message_ids_, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSNewsFeed", dataParameter, "protobuf_zip", "");
        }

        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        //MenuItem fucntions
        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        CoreHub.GetCMSMenuItems = function (project_id) {
            var id = cDB.getAuthID();
            var dataParameter = [project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetMenuItems", dataParameter, "protobuf_zip", "");
        }

        CoreHub.UpdateCMSMenuItem = function (menuitem, project_id) {
            var id = cDB.getAuthID();
            var menuitem_ = CoreHub.ProtoEncode_zip(menuitem, "S_CMSMenuItem");
            var dataParameter = [menuitem_, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateMenuItem", dataParameter, "protobuf_zip", "");
        }

        CoreHub.DeleteCMSMenuItems = function (menuitems_ids, project_id) {
            var id = cDB.getAuthID();
            var menuitems_ids_ = CoreHub.ProtoEncode_zip(menuitems_ids, "ListInt");
            var dataParameter = [menuitems_ids_, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSNewsFeeds", dataParameter, "protobuf_zip", "");
        }

        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        //Race fucntions
        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        CoreHub.GetCMSRaces = function (project_id) {
            var id = cDB.getAuthID();
            var dataParameter = [project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSRaces", dataParameter, "protobuf_zip", "");
        }

        CoreHub.GetCMSRaceResults = function (race_category_id) {
            var id = cDB.getAuthID();
            var dataParameter = [race_category_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSRaceResults", dataParameter, "protobuf_zip", "");
        }

        CoreHub.UpdateCMSRace = function (race, project_id) {
            var id = cDB.getAuthID();
            var race_ = CoreHub.ProtoEncode_zip(race, "S_CMSRace");
            var dataParameter = [race_, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSRace", dataParameter, "protobuf_zip", "");
        }

        CoreHub.UpdateCMSRaceResults = function (race_category_id, listCMSRaceResult, project_id) {
            var id = cDB.getAuthID();
            var listCMSRaceResult_ = CoreHub.ProtoEncode_zip(listCMSRaceResult, "ListCMSRaceResult");
            var dataParameter = [race_category_id, listCMSRaceResult_, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSRaceResults", dataParameter, "protobuf_zip", "");
        }

        CoreHub.DeleteCMSRace = function (race_id, category_ids, project_id) {
            var id = cDB.getAuthID();
            var category_ids_ = CoreHub.ProtoEncode_zip(category_ids, "ListInt");
            var dataParameter = [race_id, category_ids_, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSRace", dataParameter, "protobuf_zip", "");
        }

        CoreHub.DeleteCMSRaceResults = function (raceresults_ids, project_id) {
            var id = cDB.getAuthID();
            var raceresults_ids_ = CoreHub.ProtoEncode_zip(raceresults_ids, "ListInt");
            var dataParameter = [raceresults_ids_, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSRaceResults", dataParameter, "protobuf_zip", "");
        }

        CoreHub.DeleteCMSRaceResultsByCategory = function (category_id, project_id) {
            var id = cDB.getAuthID();
            var dataParameter = [category_id, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSRaceResultsByCategory", dataParameter, "protobuf_zip", "");
        }
        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        //Seminar and Event fucntions
        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        CoreHub.GetCMSSeminars = function (project_id, withspeakers) {
            var id = cDB.getAuthID();
            var dataParameter = [project_id, withspeakers];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSSeminars", dataParameter, "protobuf_zip", "");
        }

        CoreHub.UpdateCMSSeminar = function (seminar, updateSpeakers, project_id) {
            var id = cDB.getAuthID();
            var seminar_ = CoreHub.ProtoEncode_zip(seminar, "S_CMSSeminar");
            var dataParameter = [seminar_, updateSpeakers, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSSeminar", dataParameter, "protobuf_zip", "");
        }

        CoreHub.DeleteCMSSeminars = function (seminar_ids, project_id) {
            var id = cDB.getAuthID();
            var seminar_ids_ = CoreHub.ProtoEncode_zip(seminar_ids, "ListInt");
            var dataParameter = [seminar_ids_, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSSeminars", dataParameter, "protobuf_zip", "");
        }

        CoreHub.UpdateCMSSpeaker = function (speaker, project_id) {
            var id = cDB.getAuthID();
            var speaker_ = CoreHub.ProtoEncode_zip(speaker, "S_CMSSpeaker");
            var dataParameter = [speaker_, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSSpeaker", dataParameter, "protobuf_zip", "");
        }

        CoreHub.GetCMSSpeakersOfSeminar = function (seminar_id, project_id, seminarSpeakerLinkType) {
            var id = cDB.getAuthID();
            var dataParameter = [category_id, project_id, seminarSpeakerLinkType];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSSpeakersOfSeminar", dataParameter, "protobuf_zip", "");
        }

        CoreHub.GetCMSSpeakers = function (project_id) {
            var id = cDB.getAuthID();
            var dataParameter = [project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSSpeakers", dataParameter, "protobuf_zip", "");
        }

        CoreHub.DeleteCMSSpeakers = function (speaker_ids, project_id) {
            var id = cDB.getAuthID();
            var speaker_ids_ = CoreHub.ProtoEncode_zip(speaker_ids, "ListInt");
            var dataParameter = [speaker_ids_, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSSpeakers", dataParameter, "protobuf_zip", "");
        }

        CoreHub.GetCMSEventGroups = function (project_id) {
            var id = cDB.getAuthID();
            var dataParameter = [project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSEventGroups", dataParameter, "protobuf_zip", "");
        }

        CoreHub.UpdateCMSEventGroup = function (eventgroup, project_id) {
            var id = cDB.getAuthID();
            var eventgroup_ = CoreHub.ProtoEncode_zip(eventgroup, "S_CMSEventGroup");
            var dataParameter = [eventgroup_, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSEventGroup", dataParameter, "protobuf_zip", "");
        }

        CoreHub.DeleteCMSEventGroupSubgroup = function (groups_subgroups, project_id) {
            var id = cDB.getAuthID();
            var groups_subgroups_ = CoreHub.ProtoEncode_zip(groups_subgroups, "DictionaryStringListString");
            var dataParameter = [groups_subgroups_, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSEventGroupSubgroup", dataParameter, "protobuf_zip", "");
        }

        CoreHub.GetCMSEventLocations = function (project_id) {
            var id = cDB.getAuthID();
            var dataParameter = [project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSEventLocations", dataParameter, "protobuf_zip", "");
        }

        CoreHub.UpdateCMSEventLocation = function (eventlocation, project_id) {
            var id = cDB.getAuthID();
            var eventlocation_ = CoreHub.ProtoEncode_zip(eventlocation, "S_CMSEventLocation");
            var dataParameter = [eventlocation_, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSEventLocation", dataParameter, "protobuf_zip", "");
        }

        CoreHub.DeleteCMSEventLocations = function (locations, project_id) {
            var id = cDB.getAuthID();
            var locations_ = CoreHub.ProtoEncode_zip(locations, "ListString");
            var dataParameter = [locations_, project_id];
            CoreHub.SendDataParts(id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSEventLocations", dataParameter, "protobuf_zip", "");
        }

        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        //Encoding functions
        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        
        const MAX_LENGTH = 50000;

        CoreHub.SendDataParts = function (authentication_key, exchangeName, serviceType, dataParameter, dataType,callback) {

            var data_str = JSON.stringify(dataParameter);

            if (data_str.length <= MAX_LENGTH) { // works as before without splitting
                //Added call back string to make method work - TOH
                CoreHub.Query(authentication_key, exchangeName, serviceType, dataParameter, dataType, "");
            }
            else { // splitting
                var arr_str = data_str.split('');

                var data_parts_num = 0;
                var data_parts = [];

                while (arr_str.length > MAX_LENGTH)
                    data_parts[data_parts_num++] = arr_str.splice(0, MAX_LENGTH).join('');

                data_parts[data_parts_num++] = arr_str.join('');

                var guid = CoreHub.Guid_gen();

                for (var i = 0; i < data_parts_num; i++) {
                    dataParameter = ['split', guid, data_parts_num.toString(), i.toString(), data_parts[i]];
                    //Added call back string to make method work - TOH
                    CoreHub.Query(authentication_key, exchangeName, serviceType, dataParameter, dataType,"");
                }
            }
        }

        CoreHub.Guid_gen = function () {
            function _p8(s) {
                var p = (Math.random().toString(16) + "000000000").substr(2, 8);
                return s ? "-" + p.substr(0, 4) + "-" + p.substr(4, 4) : p;
            }
            return _p8() + _p8(true) + _p8(true) + _p8();
        }

        CoreHub.ProtoDecode_zip = function (string64_data, object_type) {
            var ProtoBuf = dcodeIO.ProtoBuf;
            var builder = ProtoBuf.newBuilder();//{ convertFieldsToCamelCase: true });                                         

            //Used when application is live - path variable differs when published
            //ProtoBuf.loadProtoFile('../MahonPoint/Scripts/Internal/Hub/STypesCore.txt', builder);
            //ProtoBuf.loadProtoFile('../MahonPoint/Scripts/Internal/Hub/STypesContext.txt', builder);
            //ProtoBuf.loadProtoFile('../MahonPoint/Scripts/Internal/Hub/STypesTradeFairs.txt', builder);

            //Used for internal development environment
            ProtoBuf.loadProtoFile(getRelativePath() + '/Scripts/Internal/Hub/STypesCore.txt', builder);
            ProtoBuf.loadProtoFile(getRelativePath() + '/Scripts/Internal/Hub/STypesContext.txt', builder);
            ProtoBuf.loadProtoFile(getRelativePath() + '/Scripts/Internal/Hub/STypesTradeFairs.txt', builder);
            //  ProtoBuf.loadProtoFile('Local/Data/STypesTradeFairs.txt', builder);

            var object_builder = builder.build(object_type);

            //http://stackoverflow.com/questions/14620769/decompress-gzip-and-zlib-string-in-javascript

            // Decode base64 (convert ascii to binary)
            var strData = atob(string64_data);

            // Convert binary string to character-number array
            var charData = strData.split('').map(function (x) {
                return x.charCodeAt(0);
            });

            // Turn number array into byte-array
            var binData = new Uint8Array(charData);

            // Pako magic
            var data = pako.inflate(binData);

            // Convert gunzipped byteArray back to ascii string:
            //var strData = String.fromCharCode.apply(null, new Uint16Array(data));

            var myS_Object = object_builder.decode(data);

            return myS_Object;
            //var myProjectProfileJS = new S_TransformableObjectProfileJS(myS_ProjectProfile);
        }

        CoreHub.ProtoEncode_zip = function (input_object, object_type) {

            var ProtoBuf = dcodeIO.ProtoBuf;
            var builder = ProtoBuf.newBuilder();//{ convertFieldsToCamelCase: true });                                         

            //Used when application is live - path variable differs when published
            //ProtoBuf.loadProtoFile('../MahonPoint/Scripts/Internal/Hub/STypesCore.txt', builder);
            //ProtoBuf.loadProtoFile('../MahonPoint/Scripts/Internal/Hub/STypesContext.txt', builder);
            //ProtoBuf.loadProtoFile('../MahonPoint/Scripts/Internal/Hub/STypesTradeFairs.txt', builder);

            //Used for internal development environment
            ProtoBuf.loadProtoFile(getRelativePath() + '/Scripts/Internal/Hub/STypesCore.txt', builder);
            ProtoBuf.loadProtoFile(getRelativePath() + '/Scripts/Internal/Hub/STypesContext.txt', builder);
            ProtoBuf.loadProtoFile(getRelativePath() + '/Scripts/Internal/Hub/STypesTradeFairs.txt', builder);

            var object_builder = builder.build(object_type);

            var myS_object = new object_builder(input_object);

            var byteBuffe_protobuffed = myS_object.encode();

            var buffer = byteBuffe_protobuffed.toArrayBuffer();

            var zipped_data = pako.gzip(buffer);

            var string64_data = this._arrayBufferToBase64(zipped_data);

            return string64_data
        }

        CoreHub._arrayBufferToBase64 = function (buffer) {
            var binary = '';
            var bytes = new Uint8Array(buffer);
            var len = bytes.byteLength;
            for (var i = 0; i < len; i++) {
                binary += String.fromCharCode(bytes[i]);
            }
            return window.btoa(binary);
        }

        CoreHub.encode32 = function (num) {
            return (num << 1) ^ (num >> 31)
        }






        return CoreHub;

    }]);



function getRelativePath() {
    return location.origin + '' + (location.pathname).substring(0, ((location.pathname).substring(1)).indexOf('/') + 1);
}