

/*
*
 * \file
 *     File Contains all function related to the CoreHub connection, e.g. query data, send data etc.
 */


// GateWay object




/*
Description: 

Parameters:
[IN]:   connection: signalR connection


*/
function CoreHubGateway(connection) {


    this.hubInitiated = false;

    this.nicorehub = connection;

    this.transformableObjectCallback = "";
    this.loginCallback = "";
    this.failedLoginCallback = "";
    this.profileCallback = "";
    this.subscribeCallback = "";

    this.splitObjectCache = {};
}

/*
Description: 

Parameters
[IN]:
{OUT}:

*/
CoreHubGateway.prototype.SetCacheObject = function(guid, data_parts_num, index, part) {
    if (typeof this.splitObjectCache[guid] == "undefined") {
        this.splitObjectCache[guid] = {};
        this.splitObjectCache[guid].parts_num = data_parts_num;
        this.splitObjectCache[guid].parts = [];
        this.splitObjectCache[guid].remaining = [];
        for (var i = 0; i < data_parts_num; i++) {
            this.splitObjectCache[guid].parts[i] = "";
            this.splitObjectCache[guid].remaining[i] = i;
        }
    }

    if (typeof this.splitObjectCache[guid].parts[index] != "undefined")
        this.splitObjectCache[guid].parts[index] = part;

    var index_int = parseInt(index);
    var i = this.splitObjectCache[guid].remaining.indexOf(index_int);
    if (i != -1) {
        this.splitObjectCache[guid].remaining.splice(i, 1);
    }

    if (this.splitObjectCache[guid].remaining.length == 0) {
        var jsondata = "";
        for (var i in this.splitObjectCache[guid].parts)
            jsondata += this.splitObjectCache[guid].parts[i];

        this.splitObjectCache[guid] = null;
        delete this.splitObjectCache[guid]

        return JSON.parse(jsondata);
    }
    return;
}


/*
Description: Initialise the hub
Defines what to do with messages from the hub
Parameters
[IN]:
{OUT}:

*/
CoreHubGateway.prototype.InitHub = function () {
    var that = this;
    $.connection.hub.logging = true;

    $.connection.hub.disconnected(function () {
        that.hubInitiated = false;
    });

    $.extend(this.nicorehub.client, {
        setReceiveBroadcast: function (message) {
            console.log(message);

            var exchangeName = message['exchangeName'];
            var serviceType = message['serviceType'];
            var dataType = message['dataType'];
            var dataParameter = message['dataParameter'];

            if ((exchangeName == "Context_SocialUpdateRequest" && serviceType == "Context_ReloadSocialDB") ||
                (exchangeName == "UserUpdates" && serviceType == "ReloadUserAccount") ||
                (exchangeName == "Context_DataMiningUpdateRequest" && serviceType == "Reload3DData") ||
                (exchangeName == "Context_DataMiningUpdateRequest" && serviceType == "Context_ReloadDataMiningDB") ||
                (exchangeName == "TradeFairsUpdateRequest" && serviceType == "TradeFairs_ReloadTradefairsDB")) {

                if (dataType == "protobuf_zip") {
                    if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                        var myS_S_object = that.ProtoDecode_zip(dataParameter, "S_CMSDataUpdateBroadcast");

                        result = new S_CMSDataUpdateBroadcastJS(myS_S_object);
                    }
                    else
                        result = null;
                }
                else if (dataType == "json") {
                    result = dataParameter;
                }
                window[subscribeCallback](result);
                console.log(result);
            }
        },
        setLoginSuccess: function (message) {//not used

        },
        setLoginUnsuccess: function (message) {
            window[failedLoginCallback](message);
        },
        setErrorMessage: function (message) {
            console.log(message);
        },
        setReceiveMessage: function (callback, message) {

            if (typeof message !== "undefined") {

                if (message.length == 5 && message[0] == 'split') {
                    //split 

                    var guid = message[1];
                    var data_parts_num = message[2];
                    var index = message[3];
                    var data_part = message[4];

                    message = that.SetCacheObject(guid, data_parts_num, index, data_part);

                    if (typeof message == "undefined")
                        return;
                }

                //no split as it was before

                var exchangeName = message['exchangeName'];
                var serviceType = message['serviceType'];
                var dataType = message['dataType'];
                var dataParameter = message['dataParameter'];

                if (serviceType == "GetProjectProfile") {
                    var result = null;

                    if (dataType == "protobuf_zip") {
                        var myS_S_TransformableObjectProfile = that.ProtoDecode_zip(dataParameter, "S_TransformableObjectProfile");

                        result = new S_TransformableObjectProfileJS(myS_S_TransformableObjectProfile);
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "GetTransformableObject") {

                    var result = null;

                    if (dataType == "protobuf_zip") {
                        var myS_S_TransformableObject = that.ProtoDecode_zip(dataParameter, "S_TransformableObject");

                        result = new S_TransformableObjectJS(myS_S_TransformableObject);
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "Context_GetTransformableObject") {
                    var result = null;

                    if (dataType == "protobuf_zip") {
                        if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                            var myS_S_Object = that.ProtoDecode_zip(dataParameter, "S_CMSTransformableObject");

                            result = new S_CMSTransformableObjectJS(myS_S_Object);

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
                    window[callback](result);
                }
                else if (serviceType == "GetDestinationList") {
                    var result = null;

                    if (dataType == "protobuf_zip") {
                        if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                            var myS_S_Object = that.ProtoDecode_zip(dataParameter, "ListCMSDestination");

                            result = new ListCMSDestinationJS(myS_S_Object);
                        }
                        else
                            result = null;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_GetCMSImage") {

                    if (dataType == "protobuf_zip") {
                        if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                            var myS_S_object = that.ProtoDecode_zip(dataParameter, "S_CMSImage");

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
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_GetCMSProjectSetting") {

                    if (dataType == "protobuf_zip") {
                        if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                            var myS_S_Object = that.ProtoDecode_zip(dataParameter, "S_CMSProjectSetting");

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
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_GetCMSAccounts") {

                    if (dataType == "protobuf_zip") {
                        if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                            var myS_S_object = that.ProtoDecode_zip(dataParameter, "ListCMSAccount");

                            result = new ListCMSAccountJS(myS_S_object);
                        }
                        else
                            result = null;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_GetCMSAccountByName") {

                    if (dataType == "protobuf_zip") {
                        console.log(dataParameter);
                        if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                            var myS_S_object = that.ProtoDecode_zip(dataParameter, "S_CMSAccount");

                            result = myS_S_object;
                        }
                        else
                            result = null;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_UpdateCMSAccount") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_CMSUsernameExist") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_DeleteCMSAccounts") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_GetCMSStores" || serviceType == "TradeFairs_GetCMSStoresOfLinkedProfile") {

                    if (dataType == "protobuf_zip") {

                        if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                            var myS_S_object = that.ProtoDecode_zip(dataParameter, "ListCMSStore");

                            result = new ListCMSStoreJS(myS_S_object);
                        }
                        else
                            result = null;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_UpdateCMSStore") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_DeleteCMSStores") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_GetCMSBooths" || serviceType == "TradeFairs_GetCMSBoothsOfStore") {

                    if (dataType == "protobuf_zip") {
                        if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                            var myS_S_object = that.ProtoDecode_zip(dataParameter, "ListCMSBooth");

                            result = new ListCMSBoothJS(myS_S_object);
                        }
                        else
                            result = null;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_GetCMSBoothHalls") {

                    if (dataType == "protobuf_zip") {
                        if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                            var myS_S_object = that.ProtoDecode_zip(dataParameter, "ListString");

                            result = new ListCMSBoothJS(myS_S_object);
                        }
                        else
                            result = null;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_UpdateCMSBooths") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_DeleteCMSBooths") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_GetProfileCategories") {

                    if (dataType == "protobuf_zip") {
                        if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                            var myS_S_object = that.ProtoDecode_zip(dataParameter, "ListCMSProfileCategory");

                            result = new ListCMSProfileCategoryJS(myS_S_object);
                        }
                        else
                            result = new ListCMSProfileCategoryJS();
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_UpdateCMSProfileCategory") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_DeleteCMSProfileCategories") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_GetCountryFlags") {

                    if (dataType == "protobuf_zip") {
                        if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                            result = that.ProtoDecode_zip(dataParameter, "DictionaryStringString");
                        }
                        else
                            result = [];
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_GetCountryFlag") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_DecodeCountryName") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_GetCMSBeacons") {

                    if (dataType == "protobuf_zip") {
                        if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                            var myS_S_object = that.ProtoDecode_zip(dataParameter, "ListCMSBeacon");

                            result = new ListCMSBeaconJS(myS_S_object);
                        }
                        else
                            result = null;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_UpdateCMSBeacon") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_DeleteCMSBeacons") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_GetCMSNewsFeeds") {

                    if (dataType == "protobuf_zip") {
                        if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                            var myS_S_object = that.ProtoDecode_zip(dataParameter, "ListCMSNewsFeed");

                            result = new ListCMSNewsFeedJS(myS_S_object);
                        }
                        else
                            result = null;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_UpdateCMSNewsFeed") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_DeleteCMSNewsFeed") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_GetMenuItems") {

                    if (dataType == "protobuf_zip") {
                        if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                            var myS_S_Object = that.ProtoDecode_zip(dataParameter, "ListCMSMenuItem");

                            result = new ListCMSMenuItemJS(myS_S_Object);
                        }
                        else
                            result = null;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_UpdateMenuItem") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_DeleteMenuItems") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_GetCMSRaces") {

                    if (dataType == "protobuf_zip") {
                        if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                            var myS_S_Object = that.ProtoDecode_zip(dataParameter, "ListCMSRace");

                            result = new ListCMSRaceJS(myS_S_Object);
                        }
                        else
                            result = null;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_GetCMSRaceResults") {

                    if (dataType == "protobuf_zip") {
                        if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                            var myS_S_Object = that.ProtoDecode_zip(dataParameter, "ListCMSRaceResult");

                            result = new ListCMSRaceResultJS(myS_S_Object);
                        }
                        else
                            result = null;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_UpdateCMSRace") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_UpdateCMSRaceResults") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_DeleteCMSRace") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_DeleteCMSRaceResults") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_DeleteCMSRaceResultsByCategory") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_UpdateCMSAccountLastLoginProject") {
                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_GetCMSSeminars") {

                    if (dataType == "protobuf_zip") {
                        if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                            var myS_S_Object = that.ProtoDecode_zip(dataParameter, "ListCMSSeminar");

                            result = new ListCMSSeminarJS(myS_S_Object);
                        }
                        else
                            result = null;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_UpdateCMSSeminar") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_DeleteCMSSeminars") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                } 
                else if (serviceType == "TradeFairs_UpdateCMSSpeaker") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }                
                else if (serviceType == "TradeFairs_GetCMSSpeakersOfSeminar" || serviceType == "TradeFairs_GetCMSSpeakers") {

                    if (dataType == "protobuf_zip") {
                        if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                            var myS_S_Object = that.ProtoDecode_zip(dataParameter, "ListCMSSpeaker");

                            result = new ListCMSSpeakerJS(myS_S_Object);
                        }
                        else
                            result = null;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_DeleteCMSSpeakers") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                } 
                else if (serviceType == "TradeFairs_GetCMSEventGroups") {

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
                    window[callback](result);
                }                
                else if (serviceType == "TradeFairs_UpdateCMSEventGroup") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                } 
                else if (serviceType == "TradeFairs_DeleteCMSEventGroupSubgroup") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }                
                else if (serviceType == "TradeFairs_GetCMSEventLocations") {

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
                    window[callback](result);
                }                 
                else if (serviceType == "TradeFairs_UpdateCMSEventLocation") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                } 
                else if (serviceType == "TradeFairs_DeleteCMSEventLocations") {

                    if (dataType == "protobuf_zip") {
                        result = dataParameter;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_GetAllActiveMonths") {
                    var result = null;

                    if (dataType == "protobuf_zip") {
                        if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                            result = that.ProtoDecode_zip(dataParameter, "ListInt");
                        }
                        else
                            result = null;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }
                else if (serviceType == "TradeFairs_GetMonthlyStatistics") {
                    var result = null;

                    if (dataType == "protobuf_zip") {
                        if (dataParameter != "" && !jQuery.isEmptyObject(dataParameter)) {
                            var myS_S_Object = that.ProtoDecode_zip(dataParameter, "ListCMSGraph");

                            result = new ListCMSGraphJS(myS_S_Object);
                        }
                        else
                            result = null;
                    }
                    else if (dataType == "json") {
                        result = dataParameter;
                    }
                    window[callback](result);
                }

            } else {//message is undefined so therfore callback is message
                console.log(message);
                message = callback;
                var exchangeName = message['exchangeName'];
                var serviceType = message['serviceType'];
                var dataType = message['dataType'];
                var dataParameter = message['dataParameter'];

                if (serviceType == "ClientLogin") {
                    console.log('client login');
                    window[loginCallback](dataParameter);
                }
            }
        }


    });

    this.ReconnectHub();
}
/*
Description: Handles reconnection to the hub

Parameters
[IN]: callback: function
{OUT}:

*/
CoreHubGateway.prototype.ReconnectHub = function(callback) {
    if (this.hubInitiated == false) {
        $.connection.hub.start().done(function () {
            this.hubInitiated = true;
            if (callback != null) callback();
        });
    }
    else if (callback != null) callback();
}


/*
Description: Login function

Parameters
[IN]: 
Username: email address,
password: user password
_successcallback: callback function if login is successful
_faildCallback: callback function if login is unsuccessful
{OUT}:

*/
CoreHubGateway.prototype.Login = function(username, password, _successCallback, _failedCallback) {
    var that = this;
    this.ReconnectHub(function () {
        window.loginCallback = _successCallback;
        window.failedLoginCallback = _failedCallback;
        
        that.nicorehub.server.clientLogin(username, password);
    });
}

/*
Description: Subscribes to broadcast function on server

Parameters
[IN]: 
subscriptionconfig: contains the configuration for which data should the client listen
_subscribeCallback : callback function
{OUT}:

*/

CoreHubGateway.prototype.Subscribe = function ( subscriptionconfig, _subscribeCallback) {
    var that = this;
    this.ReconnectHub(function () {
        window.subscribeCallback = _subscribeCallback;

        that.nicorehub.server.subscribe($.connection.hub.id, "", subscriptionconfig);
    });
}


CoreHubGateway.prototype.LoadProfiles = function (project_id, _Callback) {
    var that = this;

    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        that.nicorehub.server.query($.connection.hub.id, "CoreHubGatewayQuery", "GetProjectProfile", project_id, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.LoadTransformableObject = function (S_EOIdentifierJS, _Callback) {
    var that = this;

    this.ReconnectHub(function () {
        window.transformableObjectCallback = _Callback;
        var dataParameter = that.ProtoEncode_zip(S_EOIdentifierJS, "S_EOIdentifier");

        that.nicorehub.server.query($.connection.hub.id, "CoreHubGatewayQuery", "GetTransformableObject", dataParameter, "protobuf_zip", _Callback);
    });
} 

CoreHubGateway.prototype.LoadContextTransformableObject = function (S_EOIdentifierJS, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        var dataParameter = that.ProtoEncode_zip(S_EOIdentifierJS, "S_EOIdentifier");
        console.log("[LoadTransformableObject]: " + S_EOIdentifierJS);
        that.nicorehub.server.query($.connection.hub.id, "Context_ContextCMSGatewayQuery", "Context_GetTransformableObject", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.GetCMSDestinations = function (project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        var dataParameter = [project_id];
        that.nicorehub.server.query($.connection.hub.id, "Context_ContextCMSGatewayQuery", "GetDestinationList", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.GetAllActiveMonths = function (project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        var dataParameter = [project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetAllActiveMonths", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.GetMonthlyStatistics = function (project_id, _month, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        var dataParameter = [project_id, _month];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetMonthlyStatistics", dataParameter, "protobuf_zip", _Callback);
    });
}

/*
Description: loads image from server

Parameters
[IN]: 
name: Image name, string
size : [ENUM] Image size
_Callback: callback function

*/
CoreHubGateway.prototype.LoadCMSImage = function (name, size, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        console.log("[Core Hub]: Load Image: " + name);
        window.profileCallback = _Callback;
        var dataParameter = [name, size];
        console.log(dataParameter);
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSImage", dataParameter, "protobuf_zip", _Callback);
    });
}
/*
Description: loads project specific settings, eg colors

Parameters
[IN]: 
project_id: Project id, string, e.g. CIT-PROJECT
_Callback: callback function

*/
CoreHubGateway.prototype.GetCMSProjectSetting = function (project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        console.log("[Core Hub]: GetProjectSetting: " + project_id);
        window.profileCallback = _Callback;
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSProjectSetting", [project_id], "protobuf_zip", _Callback);
    });
}

/*
Description: loads all CMS accounts for specific project

Parameters
[IN]: 
project_id: Project id, string, e.g. CIT-PROJECT
_Callback: callback function

*/
CoreHubGateway.prototype.LoadCMSAccounts = function (project_id, _Callback) {
    var that = this;

    this.ReconnectHub(function () {
        console.log("[Core Hub]: Load CMSAccount");
        window.profileCallback = _Callback;
        console.log("LoadCMSAccounts: " + project_id);
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSAccounts", [project_id], "protobuf_zip", _Callback);
    });
}


/*
Description: loads all CMS account for specific project and username

Parameters
[IN]: 
username: username, e.g. conrad.dandelski@mycit.ie
project_id: Project id, string, e.g. CIT-PROJECT
_Callback: callback function

*/
CoreHubGateway.prototype.LoadCMSAccountsByName = function (username, project_id, _Callback) {
    var that = this;

    this.ReconnectHub(function () {
        console.log("[Core Hub]: Load CMSAccount by name");
        window.profileCallback = _Callback;
        console.log("LoadCMSAccount: " + username + ", for: " + project_id);
        var dataparameter = [username, project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSAccountByName", dataparameter, "protobuf_zip", _Callback);
    });
}



/*
Description: uploads updated CMS account to hub

Parameters
[IN]: 
account: account object, look up in STypesTradeFairs.js
project_id: Project id, string, e.g. CIT-PROJECT
_Callback: callback function

*/
CoreHubGateway.prototype.UpdateCMSAccounts = function (account, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;
        console.log("[Core Hub]: Update CMSAccount");
        console.log(account);
        var img_str = account.image_string;
        account.image_string = "";
        var account_ = that.ProtoEncode_zip(account, "S_CMSAccount");
        var dataParameter = [account_, project_id, img_str];
        console.log(dataParameter);
        that.SendDataParts($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSAccount", dataParameter, "protobuf_zip", _Callback);
    });
}

/*
Description: updates the last login project for a specific account

Parameters
[IN]: 
username: username, email
last_login_project: Project id, string, e.g. CIT-PROJECT
_Callback: callback function

*/
CoreHubGateway.prototype.UpdateCMSAccountLastLoginProject = function (username, last_login_project, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        console.log("[Core Hub]: Update CMSAccount last login project");
        window.profileCallback = _Callback;

        var dataParameter = [username, last_login_project];
        console.log("UpdateCMSAccountLastLoginProject:" + dataParameter);
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSAccountLastLoginProject", dataParameter, "protobuf_zip", _Callback);
    });
}

/*
Description: deletes CMS accounts

Parameters
[IN]: 
usernames: array containing usernames
project_id: Project id, string, e.g. CIT-PROJECT
_Callback: callback function

*/
CoreHubGateway.prototype.DeleteCMSAccounts = function (usernames, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        console.log("[Core Hub]: Delete CMSAccount");
        window.profileCallback = _Callback;

        var usernames_ = that.ProtoEncode_zip(usernames, "ListString");
        var dataParameter = [usernames_, project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSAccounts", dataParameter, "protobuf_zip", _Callback);
    });
}

/*
Description: checks if user already exists

Parameters
[IN]: 
username: username to check
project_id: Project id, string, e.g. CIT-PROJECT
_Callback: callback function

*/
CoreHubGateway.prototype.CMSUsernameExist = function (username, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        console.log("[Core Hub]: Check for existing CMSAccount");
        window.profileCallback = _Callback;

        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_CMSUsernameExist", [username, project_id], "protobuf_zip", _Callback);
    });
}

/*
Description: loads all cms stores for specific project

Parameters
[IN]: 
project_id: Project id, string, e.g. CIT-PROJECT
_Callback: callback function

*/
CoreHubGateway.prototype.LoadCMSStores = function (project_id, _Callback) {
    var that = this;

    this.ReconnectHub(function () {
        window.profileCallback = _Callback;
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSStores", [project_id], "protobuf_zip", _Callback);
    });
}

/*
Description: loads all cms stores for specific project and specific accounts

Parameters
[IN]: 
profile_id: specific profile id
project_id: Project id, string, e.g. CIT-PROJECT
_Callback: callback function

*/
CoreHubGateway.prototype.LoadCMSStoresOfLinkedProfile = function (profile_id, project_id, _Callback) {
    var that = this;

    this.ReconnectHub(function () {
        window.profileCallback = _Callback;
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSStoresOfLinkedProfile", [profile_id.toString(), project_id], "protobuf_zip", _Callback);
    });
}


/*
Description: updates a store, send data to server

Parameters
[IN]: 
store: store object, look up in STypesTradeFairs.js
project_id: Project id, string, e.g. CIT-PROJECT
_Callback: callback function

*/
CoreHubGateway.prototype.UpdateCMSStores = function (store, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;
        var img_str = store.image_string;
        store.image_string = "";

        var store_ = that.ProtoEncode_zip(store, "S_CMSStore");
        var dataParameter = [store_, project_id, img_str];
        console.log(dataParameter);
        that.SendDataParts($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSStore", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.encode32 = function(num) {
    return (num << 1) ^ (num >> 31)
}


/*
Description: delete selected stores

Parameters
[IN]: 
store: store ids
project_id: Project id, string, e.g. CIT-PROJECT
_Callback: callback function

*/
CoreHubGateway.prototype.DeleteCMSStores = function (stores, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        //   var stores_encoded = [];
        //   for (var i in stores)
        //      stores_encoded.push(encode32(stores[i]));

        var storesids_ = that.ProtoEncode_zip(stores, "ListInt");

        var dataParameter = [storesids_, project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSStores", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.GetCMSBoothHalls = function (project_id, _Callback) {
    var that = this;

    this.ReconnectHub(function () {
        window.profileCallback = _Callback;
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSBoothHalls", [project_id], "protobuf_zip", _Callback);
    });
}


/*
Description: loads all cms booths for project

Parameters
[IN]: 
project_id: Project id, string, e.g. CIT-PROJECT
_Callback: callback function

*/
CoreHubGateway.prototype.GetCMSBooths = function (project_id, _Callback) {
    var that = this;

    this.ReconnectHub(function () {
        console.log("[GetCMSBooths]: " + project_id);
        window.profileCallback = _Callback;
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSBooths", [project_id], "protobuf_zip", _Callback);
    });
}


/*
Description: loads all cms boohs for specific store

Parameters
[IN]: 
store_id: specific store id to load booths
project_id: Project id, string, e.g. CIT-PROJECT
_Callback: callback function

*/
CoreHubGateway.prototype.GetCMSBoothsOfStore = function (store_id, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        console.log("[GetCMSBooths]: Store:" + store_id + " for Project: " + project_id);
        window.profileCallback = _Callback;
        var dataparameter = [store_id, project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSBoothsOfStore", dataparameter, "protobuf_zip", _Callback);
    });
}

/*
Description: updates cms booth

Parameters
[IN]: 
booth: booth object, look up in STypesTradeFairs.js
project_id: Project id, string, e.g. CIT-PROJECT
_Callback: callback function

*/
CoreHubGateway.prototype.UpdateCMSBooth = function (booth, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        var booth_ = that.ProtoEncode_zip(booth, "S_CMSBooth");
        var dataParameter = [booth_, project_id];
        console.log(dataParameter);
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSBooths", dataParameter, "protobuf_zip", _Callback);
    });
}

/*
Description: deletes selected cms boohs

Parameters
[IN]: 
booth_ids: contains selected booth_ids
project_id: Project id, string, e.g. CIT-PROJECT
_Callback: callback function

*/
CoreHubGateway.prototype.DeleteCMSBooths = function (booths_ids, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        var boothsids_ = that.ProtoEncode_zip(booths_ids, "ListInt");

        var dataParameter = [boothsids_, project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSBooths", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.GetCMSBoothHalls = function (project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        console.log("[GetCMSBooths]: " + project_id);
        window.profileCallback = _Callback;
        var dataparameter = [project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSBoothHalls", dataparameter, "protobuf_zip", _Callback);
    });
}

//type: "product", "sector", "profile"
CoreHubGateway.prototype.GetCMSProfileCategories = function (project_id, profileCategoryType, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        console.log("[GetCMSProfileCategories]: " + project_id + ", " + profileCategoryType);
        window.profileCallback = _Callback;
        var dataParameter = [project_id, profileCategoryType];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetProfileCategories", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.UpdateCMSProfileCategory = function (profile_category, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;
        var profile_category_ = that.ProtoEncode_zip(profile_category, "S_CMSProfileCategory");
        var dataParameter = [profile_category_, project_id];
        console.log(dataParameter);
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSProfileCategory", dataParameter, "protobuf_zip", _Callback);
    });
}

//type: "product", "sector", "profile"
CoreHubGateway.prototype.DeleteCMSProfileCategories = function (profileCategories_ids, profileCategoryType, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;
        var profileCategories_ids_ = that.ProtoEncode_zip(profileCategories_ids, "ListInt");
        var dataParameter = [profileCategories_ids_, profileCategoryType, project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSProfileCategories", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.GetCountryFlags = function (project_id, _Callback) {
    var that = this;

    this.ReconnectHub(function () {
        window.profileCallback = _Callback;
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCountryFlags", [project_id], "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.GetCountryFlag = function (project_id, country_name, _Callback) {
    var that = this;

    this.ReconnectHub(function () {
        window.profileCallback = _Callback;
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCountryFlag", [project_id, country_name], "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.DecodeCountryName = function (project_id, country_name, _Callback) {
    var that = this;

    this.ReconnectHub(function () {
        window.profileCallback = _Callback;
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_DecodeCountryName", [project_id, country_name], "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.LoadCMSBeacons = function (project_id, _Callback) {
    var that = this;

    this.ReconnectHub(function () {
        window.profileCallback = _Callback;
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSBeacons", [project_id], "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.UpdateCMSBeacons = function (beacon, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        var beacon_ = that.ProtoEncode_zip(beacon, "S_CMSBeacon");
        console.log("[Update Beacon]: ");
        var dataParameter = [beacon_, project_id];
        console.log(beacon);
        console.log(dataParameter);
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSBeacon", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.DeleteCMSBeacons = function (beacon_ids, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        var beacon_ids_ = that.ProtoEncode_zip(beacon_ids, "ListInt");
        console.log("[Delete Beacons]: " + beacon_ids);

        var dataParameter = [beacon_ids_, project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSBeacons", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.LoadCMSNewsFeeds = function (project_id, store_id, _Callback) {
    var that = this;

    this.ReconnectHub(function () {
        window.profileCallback = _Callback;
        var dataparameter = [project_id, store_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSNewsFeeds", dataparameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.UpdateCMSNewsFeed = function (message, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        var message_ = that.ProtoEncode_zip(message, "S_CMSNewsFeed");
        console.log("[Update News Feed]: ");
        var dataParameter = [message_, project_id];
        console.log(message);
        console.log(dataParameter);
        that.SendDataParts($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSNewsFeed", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.DeleteCMSNewsFeed = function (message_ids, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        var message_ids_ = that.ProtoEncode_zip(message_ids, "ListInt");
        console.log("[Delete NewsFeed]: " + message_ids);

        var dataParameter = [message_ids_, project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSNewsFeed", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.LoadCMSMenuItems = function (project_id, _Callback) {
    var that = this;

    this.ReconnectHub(function () {
        window.profileCallback = _Callback;
        var dataparameter = [project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetMenuItems", dataparameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.UpdateCMSMenuItem = function (menuitem, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        var menuitem_ = that.ProtoEncode_zip(menuitem, "S_CMSMenuItem");
        console.log("[Update Menu item]: ");
        var dataParameter = [menuitem_, project_id];
        console.log(menuitem);
        console.log(dataParameter);
        that.SendDataParts($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateMenuItem", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.DeleteCMSMenuItems = function (menuitems_ids, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        var menuitems_ids_ = that.ProtoEncode_zip(menuitems_ids, "ListInt");
        console.log("[Delete MenuItems]: " + menuitems_ids);

        var dataParameter = [menuitems_ids_, project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteMenuItems", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.LoadCMSRaces = function (project_id, _Callback) {
    var that = this;

    this.ReconnectHub(function () {
        window.profileCallback = _Callback;
        var dataparameter = [project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSRaces", dataparameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.LoadCMSRaceResults = function (race_category_id, _Callback) {
    var that = this;

    this.ReconnectHub(function () {
        window.profileCallback = _Callback;
        var dataparameter = [race_category_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSRaceResults", dataparameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.UpdateCMSRace = function (race, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        var race_ = that.ProtoEncode_zip(race, "S_CMSRace");
        console.log("[Update Race]: ");
        var dataParameter = [race_, project_id];
        console.log(race);
        console.log(dataParameter);
        that.SendDataParts($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSRace", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.UpdateCMSRaceResults = function (race_category_id, listCMSRaceResult, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        var listCMSRaceResult_ = that.ProtoEncode_zip(listCMSRaceResult, "ListCMSRaceResult");
        console.log("[Update RaceResults]: ");
        var dataParameter = [race_category_id, listCMSRaceResult_, project_id];
        console.log(listCMSRaceResult);
        console.log(dataParameter);
        that.SendDataParts($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSRaceResults", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.DeleteCMSRace = function (race_id, category_ids, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        var category_ids_ = that.ProtoEncode_zip(category_ids, "ListInt");
        console.log("[Delete Race]: " + race_id);

        var dataParameter = [race_id, category_ids_, project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSRace", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.DeleteCMSRaceResults = function (raceresults_ids, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        var raceresults_ids_ = that.ProtoEncode_zip(raceresults_ids, "ListInt");
        console.log("[Delete RaceResults]: " + raceresults_ids);

        var dataParameter = [raceresults_ids_, project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSRaceResults", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.DeleteCMSRaceResultsByCategory = function (category_id, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        console.log("[Delete RaceResultsByCategory]: " + category_id);

        var dataParameter = [category_id, project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSRaceResultsByCategory", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.LoadCMSSeminars = function (project_id, withspeakers, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        console.log("[Load CMSSeminars]: " + withspeakers);

        var dataParameter = [project_id, withspeakers];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSSeminars", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.UpdateCMSSeminar = function ( seminar, updateSpeakers, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        var seminar_ = that.ProtoEncode_zip(seminar, "S_CMSSeminar");
        console.log("[Update Seminar]: " + updateSpeakers);
        var dataParameter = [seminar_, updateSpeakers, project_id];
        console.log(seminar);
        console.log(dataParameter);
        that.SendDataParts($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSSeminar", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.DeleteCMSSeminars = function (seminar_ids, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        var seminar_ids_ = that.ProtoEncode_zip(seminar_ids, "ListInt");
        console.log("[Delete Seminars]: " + seminar_ids);

        var dataParameter = [seminar_ids_, project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSSeminars", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.UpdateCMSSpeaker = function ( speaker, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        var speaker_ = that.ProtoEncode_zip(speaker, "S_CMSSpeaker");
        console.log("[Update Speaker]: ");
        var dataParameter = [speaker_, project_id];
        console.log(speaker);
        console.log(dataParameter);
        that.SendDataParts($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSSpeaker", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.LoadCMSSpeakersOfSeminar = function (seminar_id, project_id, seminarSpeakerLinkType, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        console.log("[Load CMSSpeakers Of Seminar]: " + seminar_id);

        var dataParameter = [category_id, project_id, seminarSpeakerLinkType];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSSpeakersOfSeminar", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.LoadCMSSpeakers = function (project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        console.log("[Load CMSSpeakers]:");

        var dataParameter = [project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSSpeakers", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.DeleteCMSSpeakers = function (speaker_ids, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        var speaker_ids_ = that.ProtoEncode_zip(speaker_ids, "ListInt");
        console.log("[Delete Speakers]: " + speaker_ids);

        var dataParameter = [speaker_ids_, project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSSpeakers", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.LoadCMSEventGroups = function (project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        console.log("[Load CMSEventGroups]:");

        var dataParameter = [project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSEventGroups", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.UpdateCMSEventGroup = function (eventgroup, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        var eventgroup_ = that.ProtoEncode_zip(eventgroup, "S_CMSEventGroup");
        console.log("[Update CMSEventGroup]: ");
        var dataParameter = [eventgroup_, project_id];
        console.log(eventgroup);
        that.SendDataParts($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSEventGroup", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.DeleteCMSEventGroupSubgroup = function (groups_subgroups, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        var groups_subgroups_ = that.ProtoEncode_zip(groups_subgroups, "DictionaryStringListString");
        console.log("[Delete CMSEventGroupSubgroup]: " + groups_subgroups);

        var dataParameter = [groups_subgroups_, project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSEventGroupSubgroup", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.LoadCMSEventLocations = function (project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        console.log("[Load CMSEventLocations]:");

        var dataParameter = [project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_GetCMSEventLocations", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.UpdateCMSEventLocation = function (eventlocation, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        var eventlocation_ = that.ProtoEncode_zip(eventlocation, "S_CMSEventLocation");
        console.log("[Update CMSEventLocation]: ");
        var dataParameter = [eventlocation_, project_id];
        console.log(eventlocation);
        that.SendDataParts($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_UpdateCMSEventLocation", dataParameter, "protobuf_zip", _Callback);
    });
}

CoreHubGateway.prototype.DeleteCMSEventLocations = function (locations, project_id, _Callback) {
    var that = this;
    this.ReconnectHub(function () {
        window.profileCallback = _Callback;

        var locations_ = that.ProtoEncode_zip(locations, "ListString");
        console.log("[Delete CMSEventGroupSubgroup]: " + locations);

        var dataParameter = [locations_, project_id];
        that.nicorehub.server.query($.connection.hub.id, "TradeFairsCMSGatewayQuery", "TradeFairs_DeleteCMSEventLocations", dataParameter, "protobuf_zip", _Callback);
    });
}


const MAX_LENGTH = 50000;

CoreHubGateway.prototype.SendDataParts = function(authentication_key, exchangeName, serviceType, dataParameter, dataType, _Callback) {

    var data_str = JSON.stringify(dataParameter);

    if (data_str.length <= MAX_LENGTH) { // works as before withou splitting
        this.nicorehub.server.query(authentication_key, exchangeName, serviceType, dataParameter, dataType, _Callback);
    }
    else { // splitting
        var arr_str = data_str.split('');

        var data_parts_num = 0;
        var data_parts = [];

        while (arr_str.length > MAX_LENGTH)
            data_parts[data_parts_num++] = arr_str.splice(0, MAX_LENGTH).join('');

        data_parts[data_parts_num++] = arr_str.join('');

        var guid = this.Guid_gen();

        for (var i = 0; i < data_parts_num; i++) {
            dataParameter = ['split', guid, data_parts_num.toString(), i.toString(), data_parts[i]];
            this.nicorehub.server.query(authentication_key, exchangeName, serviceType, dataParameter, dataType, _Callback);
        }
    }
}

CoreHubGateway.prototype.Guid_gen = function () {
    function _p8(s) {
        var p = (Math.random().toString(16) + "000000000").substr(2, 8);
        return s ? "-" + p.substr(0, 4) + "-" + p.substr(4, 4) : p;
    }
    return _p8() + _p8(true) + _p8(true) + _p8();
}


CoreHubGateway.prototype.ProtoDecode_zip = function(string64_data, object_type) {
    var ProtoBuf = dcodeIO.ProtoBuf;
    var builder = ProtoBuf.newBuilder();//{ convertFieldsToCamelCase: true });                                         

    ProtoBuf.loadProtoFile('../Library/Datasource/Hub/STypesCore.txt', builder);
    ProtoBuf.loadProtoFile('../Library/Datasource/Hub/STypesTradeFairs.txt', builder);
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

CoreHubGateway.prototype.ProtoEncode_zip = function(input_object, object_type) {

    var ProtoBuf = dcodeIO.ProtoBuf;
    var builder = ProtoBuf.newBuilder();//{ convertFieldsToCamelCase: true });                                         

    ProtoBuf.loadProtoFile('../Library/Datasource/Hub/STypesCore.txt', builder);
    ProtoBuf.loadProtoFile('../Library/Datasource/Hub/STypesTradeFairs.txt', builder);

    var object_builder = builder.build(object_type);

    var myS_object = new object_builder(input_object)

    var byteBuffe_protobuffed = myS_object.encode();

    var buffer = byteBuffe_protobuffed.toArrayBuffer();

    var zipped_data = pako.gzip(buffer);

    var string64_data = this._arrayBufferToBase64(zipped_data);

    return string64_data
}

CoreHubGateway.prototype._arrayBufferToBase64 = function(buffer) {
    var binary = '';
    var bytes = new Uint8Array(buffer);
    var len = bytes.byteLength;
    for (var i = 0; i < len; i++) {
        binary += String.fromCharCode(bytes[i]);
    }
    return window.btoa(binary);
}
/*
CoreHubGateway.prototype.ProtoDeconde_unzip = function( string64_data, object_type)
{
    var strData_dec = atob(string64_data);

    var charData_decod = strData_dec.split('').map(function (x) { return x.charCodeAt(0); });

    var zipped_data_decod = new Uint8Array(charData_decod);

    var unzipped_data = pako.inflate(zipped_data_decod);


    var ProtoBuf = dcodeIO.ProtoBuf;

    var builder = ProtoBuf.newBuilder();//{ convertFieldsToCamelCase: true });                                         

    ProtoBuf.loadProtoFile('Local/Data/FloorData/STypes.txt', builder);

    var object_builder = builder.build(object_type);

    var myS_object_rec = object_builder.decode(unzipped_data);

    return myS_object_rec;
} */





