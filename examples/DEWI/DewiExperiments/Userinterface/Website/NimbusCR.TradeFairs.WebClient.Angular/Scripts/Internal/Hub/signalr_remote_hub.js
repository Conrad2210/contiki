/// <reference path="signalr_remote_hub.js" />
/*!
 * ASP.NET SignalR JavaScript Library v2.0.3
 * http://signalr.net/
 *
 * Copyright Microsoft Open Technologies, Inc. All rights reserved.
 * Licensed under the Apache 2.0
 * https://github.com/SignalR/SignalR/blob/master/LICENSE.md
 *
 */

/// <reference path="..\..\SignalR.Client.JS\Scripts\jquery-1.6.4.js" />
/// <reference path="jquery.signalR.js" />
(function ($, window, undefined) {
    /// <param name="$" type="jQuery" />
    "use strict";

    if (typeof ($.signalR) !== "function") {
        throw new Error("SignalR: SignalR is not loaded. Please ensure jquery.signalR-x.js is referenced before ~/signalr/js.");
    }

    var signalR = $.signalR;

    function makeProxyCallback(hub, callback) {
        return function () {
            // Call the client hub method
            callback.apply(hub, $.makeArray(arguments));
        };
    }

    function registerHubProxies(instance, shouldSubscribe) {
        var key, hub, memberKey, memberValue, subscriptionMethod;

        for (key in instance) {
            if (instance.hasOwnProperty(key)) {
                hub = instance[key];

                if (!(hub.hubName)) {
                    // Not a client hub
                    continue;
                }

                if (shouldSubscribe) {
                    // We want to subscribe to the hub events
                    subscriptionMethod = hub.on;
                } else {
                    // We want to unsubscribe from the hub events
                    subscriptionMethod = hub.off;
                }

                // Loop through all members on the hub and find client hub functions to subscribe/unsubscribe
                for (memberKey in hub.client) {
                    if (hub.client.hasOwnProperty(memberKey)) {
                        memberValue = hub.client[memberKey];

                        if (!$.isFunction(memberValue)) {
                            // Not a client hub function
                            continue;
                        }

                        subscriptionMethod.call(hub, memberKey, makeProxyCallback(hub, memberValue));
                    }
                }
            }
        }
    }

    $.hubConnection.prototype.createHubProxies = function () {
        var proxies = {};
        this.starting(function () {
            // Register the hub proxies as subscribed
            // (instance, shouldSubscribe)
            registerHubProxies(proxies, true);

            this._registerSubscribedHubs();
        }).disconnected(function () {
            // Unsubscribe all hub proxies when we "disconnect".  This is to ensure that we do not re-add functional call backs.
            // (instance, shouldSubscribe)
            registerHubProxies(proxies, false);
        });


        proxies.CoreHub = this.createHubProxy('TradeFairsHub')//'CoreHub');

        proxies.CoreHub.client = {};
        proxies.CoreHub.server = {
            /*  getAlerts: function (auth_key, project_id, deployment_id, page_limit) {
                  return proxies.deploymentHub.invoke.apply(proxies.deploymentHub, $.merge(["GetAlerts"], $.makeArray(arguments)));
              },
  
              getDeployment: function (auth_key, project, deployment) {
                  return proxies.deploymentHub.invoke.apply(proxies.deploymentHub, $.merge(["GetDeployment"], $.makeArray(arguments)));
              },
  
              getDeploymentData: function (auth_key, data_request_object) {
                  return proxies.deploymentHub.invoke.apply(proxies.deploymentHub, $.merge(["GetDeploymentData"], $.makeArray(arguments)));
              },
  
              getDeploymentTopology: function (auth_key, project_id, deployment_id) {
                  return proxies.deploymentHub.invoke.apply(proxies.deploymentHub, $.merge(["GetDeploymentTopology"], $.makeArray(arguments)));
              },
  
              getFloorplanData: function (auth_key, project_id, building_id, floorplan_id) {
                  return proxies.deploymentHub.invoke.apply(proxies.deploymentHub, $.merge(["GetFloorplanData"], $.makeArray(arguments)));
              },
  
              getLinkHistoryData: function (auth_key, project_id, deployment_id, src_db_id, dst_db_id, from_time, to_time) {
                  return proxies.deploymentHub.invoke.apply(proxies.deploymentHub, $.merge(["GetLinkHistoryData"], $.makeArray(arguments)));
              },
  
              getProjectDeployments: function (auth_key, project_id) {
                  return proxies.deploymentHub.invoke.apply(proxies.deploymentHub, $.merge(["GetProjectDeployments"], $.makeArray(arguments)));
              },
  
              getProjectList: function (auth_key, username) {
                  return proxies.deploymentHub.invoke.apply(proxies.deploymentHub, $.merge(["GetProjectList"], $.makeArray(arguments)));
              }, */

            getProjectProfile: function (auth_key, project_id) {
                return proxies.CoreHub.invoke.apply(proxies.CoreHub, $.merge(["GetProjectProfile"], $.makeArray(arguments)));
            },

            query: function (authentication_key, exchangeName, serviceType, dataParameter, dataType) {
                return proxies.CoreHub.invoke.apply(proxies.CoreHub, $.merge(["Query"], $.makeArray(arguments)));
            },

            clientLogin: function (username, password) {
                return proxies.CoreHub.invoke.apply(proxies.CoreHub, $.merge(["ClientLogin"], $.makeArray(arguments)));
            },

            //requestData: function (auth_key, properties) {
            //    return proxies.deploymentHub.invoke.apply(proxies.deploymentHub, $.merge(["RequestData"], $.makeArray(arguments)));
            //},

            //startSession: function (api_key) {
            //    return proxies.deploymentHub.invoke.apply(proxies.deploymentHub, $.merge(["StartSession"], $.makeArray(arguments)));
            //},

            subscribe: function (authentication_key, subscription_config) {
                return proxies.CoreHub.invoke.apply(proxies.CoreHub, $.merge(["Subscribe"], $.makeArray(arguments)));
            },

            //test: function (properties) {
            //    return proxies.deploymentHub.invoke.apply(proxies.deploymentHub, $.merge(["Test"], $.makeArray(arguments)));
            //},

            //testObject: function (_input) {
            //    return proxies.deploymentHub.invoke.apply(proxies.deploymentHub, $.merge(["TestObject"], $.makeArray(arguments)));
            //},

            //updateUserPreferences: function (auth_key, preferences) {
            //    return proxies.deploymentHub.invoke.apply(proxies.deploymentHub, $.merge(["UpdateUserPreferences"], $.makeArray(arguments)));
            //},

            //broadcast: function (exchangeName, messageType, parameter, exchangeType) {
            //    return proxies.deploymentHub.invoke.apply(proxies.deploymentHub, $.merge(["broadcast"], $.makeArray(arguments)));
            //} 

        };

        return proxies;
    };

    // signalR.hub = $.hubConnection("http://157.190.54.15:45812/signalr", { useDefaultPath: false });
    //  signalR.hub = $.hubConnection("http://localhost:12128", { useDefaultPath: true });
    // signalR.hub = $.hubConnection("http://157.190.54.60:12125", { useDefaultPath: true });

    //signalR.hub = $.hubConnection("http://157.190.53.99:12128", { useDefaultPath: true });
    //signalR.hub = $.hubConnection("http://157.190.53.99:12128", { useDefaultPath: true });


    //var location = window.location;
    //var port = '12128';
    //var url = location.protocol + '//' + location.hostname + ':' + port;
    //// if (isDebugMode) {
    var url = 'http://157.190.53.99:12128';
    //// }
    signalR.hub = $.hubConnection(url, { useDefaultPath: true });

    $.extend(signalR, signalR.hub.createHubProxies());

}(window.jQuery, window));


