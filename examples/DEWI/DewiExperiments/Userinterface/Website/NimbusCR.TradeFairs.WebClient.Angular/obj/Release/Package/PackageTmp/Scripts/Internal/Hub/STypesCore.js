// this file should be autogenereted together with SType.proto files

//------------------------------------------------------------------------------------------------------------------------------------------------------------
// core attributtes ###################################
//------------------------------------------------------------------------------------------------------------------------------------------------------------
ListEOAttributeJS.prototype.init = function (ListEOAttribute) {

    if (ListEOAttribute == null) return;

    this.items = [];

    if (typeof ListEOAttribute != "undefined") {
        for (var i in ListEOAttribute.items) {
            this.items.push(new S_EOAttributeJS(ListEOAttribute.items[i]));
        }
    }
}

function ListEOAttributeJS(ListEOAttribute) {
    this.init(ListEOAttribute);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_EOAttributeJS.prototype.init = function (S_EOAttribute) {

    if (S_EOAttribute == null) {
        this.db_id = -1;
        this.object_id = -1;
        this.object_type = "";
        this.project_link = null;
        this.data = null;
    }
    else {
        this.db_id = S_EOAttribute.db_id;
        this.object_id = S_EOAttribute.object_id;
        this.object_type = S_EOAttribute.object_type;
        this.project_link = S_EOAttribute.project_link == null ? null : new S_EOIdentifierJS(S_EOAttribute.project_link);
        this.data = S_EOAttribute.data == null ? null : new S_EOAttributeDataJS(S_EOAttribute.data);
    }
}

function S_EOAttributeJS(S_EOAttribute) {
    this.init(S_EOAttribute)
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_EOIdentifierJS.prototype.init = function (S_EOIdentifier) {

    if (S_EOIdentifier == null) {
        this.project_id = "";
        this.building_id = "";
        this.floorplan_id = "";
    }
    else {
        this.project_id = S_EOIdentifier.project_id;
        this.building_id = S_EOIdentifier.building_id;
        this.floorplan_id = S_EOIdentifier.floorplan_id;
    }
}
S_EOIdentifierJS.prototype.Convert = function () {

    var result = new S_EOIdentifier();
    result.project_id = this.project_id;
    result.building_id = this.building_id;
    result.floorplan_id = this.floorplan_id;
    return result;
}

function S_EOIdentifierJS(S_EOIdentifier) {
    this.init(S_EOIdentifier)
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_EOAttributeDataJS.prototype.init = function (S_EOAttributeData) {

    if (S_EOAttributeData == null) {
        this.type = "";
        this.child = null;
    }
    else {
        this.type = S_EOAttributeData.type;
        this.child = null;
        if (S_EOAttributeData.S_DefaultDrawingObjectAttributes != null)
            this.child = new S_DefaultDrawingObjectAttributesJS(S_EOAttributeData.S_DefaultDrawingObjectAttributes);
        else if (S_EOAttributeData.S_MaterialAttribute != null)
            this.child = new S_MaterialAttributeJS(S_EOAttributeData.S_MaterialAttribute);
    }
}

function S_EOAttributeDataJS(S_EOAttributeData) {
    this.init(S_EOAttributeData)
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_DefaultDrawingObjectAttributesJS.prototype.init = function (S_DefaultDrawingObjectAttributes) {

    if (S_DefaultDrawingObjectAttributes == null) {
        this.drawingObjectAttributes = {};
    }
    else {
        this.drawingObjectAttributes = {};

        for (var i in S_DefaultDrawingObjectAttributes.drawingObjectAttributes) {
            //  this.drawingObjectAttributes.push({
            //      key: S_DefaultDrawingObjectAttributes.drawingObjectAttributes[i].Key,
            //      value: S_DefaultDrawingObjectAttributes.drawingObjectAttributes[i].Value == null ? null : new S_DrawingObjectAttributeJS(S_DefaultDrawingObjectAttributes.drawingObjectAttributes[i].Value)
            //  });
            this.drawingObjectAttributes[S_DefaultDrawingObjectAttributes.drawingObjectAttributes[i].Key] =
                S_DefaultDrawingObjectAttributes.drawingObjectAttributes[i].Value == null ? null : new S_DrawingObjectAttributeJS(S_DefaultDrawingObjectAttributes.drawingObjectAttributes[i].Value);
        }
    }
}

function S_DefaultDrawingObjectAttributesJS(S_DefaultDrawingObjectAttributes) {
    this.init(S_DefaultDrawingObjectAttributes)
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_DrawingObjectAttributeJS.prototype.init = function (S_DrawingObjectAttribute) {

    if (S_DrawingObjectAttribute == null) {
        this.name = "";
        this.width = 0.0;
        this.own_color = false;
        this.visible_from = 0;
        this.visible_to = 0;
        this.color = [0, 150, 150, 150];
    }
    else {
        this.name = S_DrawingObjectAttribute.name;
        this.width = S_DrawingObjectAttribute.width;
        this.own_color = S_DrawingObjectAttribute.own_color;
        this.visible_from = S_DrawingObjectAttribute.visible_from;
        this.visible_to = S_DrawingObjectAttribute.visible_to;

        if (typeof S_DrawingObjectAttribute.color == 'object')
            if (S_DrawingObjectAttribute.color.length > 0) {
                var element0 = S_DrawingObjectAttribute.color[0];
                this.color = [0, 150, 150, 150];
                for (var i = element0.offset; i < element0.limit; i++) {
                    this.color[i - element0.offset] = element0.view[i];
                    //  for (var i = element0.limit - 1; i >= element0.offset; i--) {

                    //  for (var i = element0.offset; i < element0.limit; i++) {
                    //      this.color[element0.limit - i - 1] = element0.view[i];
                }
            }
    }
}

function S_DrawingObjectAttributeJS(S_DrawingObjectAttribute) {
    this.init(S_DrawingObjectAttribute)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_MaterialAttributeJS.prototype.init = function (S_MaterialAttribute) {
    if (S_MaterialAttribute == null) {
        this.drawingObjectAttribute = null;
    }
    else {
        this.drawingObjectAttribute = S_MaterialAttribute.drawingObjectAttribute == null ? null : new S_DrawingObjectAttributeJS(S_MaterialAttribute.drawingObjectAttribute)
    }
}

function S_MaterialAttributeJS(S_MaterialAttribute) {
    this.init(S_MaterialAttribute)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
// Transformable Object Profile ###################################
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_TransformableObjectProfileJS.prototype.init = function (S_TransformableObjectProfile) {
    if (S_TransformableObjectProfile == null) {
        this.db_id = -1;
        this.longname = "";
        this.description = "";
        this.model = "";
        this.transformation = null;
        this.child = null;
    }
    else {
        this.db_id = S_TransformableObjectProfile.db_id;
        this.longname = S_TransformableObjectProfile.longname;
        this.description = S_TransformableObjectProfile.description;
        this.model = S_TransformableObjectProfile.model;
        this.transformation = S_TransformableObjectProfile.transformation == null ? null : new S_LayerTransformationJS(S_TransformableObjectProfile.transformation);
        this.child = null;
        if (S_TransformableObjectProfile.S_ProjectProfile != null)
            this.child = new S_ProjectProfileJS(S_TransformableObjectProfile.S_ProjectProfile);
        else if (S_TransformableObjectProfile.S_BuildingProfile != null)
            this.child = new S_BuildingProfileJS(S_TransformableObjectProfile.S_BuildingProfile);
        else if (S_TransformableObjectProfile.S_FloorplanProfile != null)
            this.child = new S_FloorplanProfileJS(S_TransformableObjectProfile.S_FloorplanProfile);
    }
}

/* var S_TransformableObjectProfileJS = (function () {
    var _db_id = -1;
    var _longname = "";
    var _description = "";
    var _model = "";
    var _transformation = null;
    var _child = null;

    return {
        db_id: _db_id,
        longname: _longname,
        description: _description,
        model: _model,
        transformation: _transformation,
        child : _child,

        init: function (S_TransformableObjectProfile) {
            if (S_TransformableObjectProfile == null) {
                _db_id = -1;
                _longname = "";
                _description = "";
                _model = "";
                _transformation = null;
                _child = null;
            }
            else {
                _db_id = S_TransformableObjectProfile.db_id;
                _longname = S_TransformableObjectProfile.longname;
                _description = S_TransformableObjectProfile.description;
                _model = S_TransformableObjectProfile.model;
                _transformation = S_TransformableObjectProfile.transformation == null ? null : new S_LayerTransformationJS(S_TransformableObjectProfile.transformation);
                _child = null;
                if (S_TransformableObjectProfile.S_ProjectProfile != null)
                    _child = new S_ProjectProfileJS(S_TransformableObjectProfile.S_ProjectProfile);
                else if (S_TransformableObjectProfile.S_BuildingProfile != null)
                    _child = new S_BuildingProfileJS(S_TransformableObjectProfile.S_BuildingProfile);
                else if (S_TransformableObjectProfile.S_FloorplanProfile != null)
                    _child = new S_FloorplanProfileJS(S_TransformableObjectProfile.S_FloorplanProfile);
            }
        }
    };
})(); */

function S_TransformableObjectProfileJS(S_TransformableObjectProfile) {
    this.init(S_TransformableObjectProfile);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_ProjectProfileJS.prototype.init = function (S_ProjectProfile) {
    if (S_ProjectProfile == null) {
        this.status = "";
        this.startdate = null;
        this.owner = -1;
        this.BuildingProfiles = [];
    }
    else {
        this.status = S_ProjectProfile.status;
        this.startdate = new S_DateTimeJS(S_ProjectProfile.startdate);
        this.owner = S_ProjectProfile.owner || -1;
        this.BuildingProfiles = [];
        for (var i in S_ProjectProfile.BuildingProfiles) {
            if (S_ProjectProfile.BuildingProfiles[i] != null)
                this.BuildingProfiles.push(new S_TransformableObjectProfileJS(S_ProjectProfile.BuildingProfiles[i]));
        }
    }
}
function S_ProjectProfileJS(S_ProjectProfile) {
    this.init(S_ProjectProfile);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_BuildingProfileJS.prototype.init = function (S_BuildingProfile) {
    if (S_BuildingProfile == null) {
        this.floorplanProfiles = [];
    }
    else {
        this.floorplanProfiles = [];
        for (var i in S_BuildingProfile.floorplanProfiles) {
            if (S_BuildingProfile.floorplanProfiles[i] != null)
                this.floorplanProfiles.push(new S_TransformableObjectProfileJS(S_BuildingProfile.floorplanProfiles[i]));
        }
    }
}
function S_BuildingProfileJS(S_BuildingProfile) {
    this.init(S_BuildingProfile);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_FloorplanProfileJS.prototype.init = function (S_FloorplanProfile) {
}
function S_FloorplanProfileJS(S_FloorplanProfile) {
    this.init(S_FloorplanProfile);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_LayerTransformationJS.prototype.init = function (S_LayerTransformation) {
    if (S_LayerTransformation == null) {
        this.type = S_LayerTypeJS_Enum[0];
        this.identif = null;
        this.min_local = null;
        this.max_local = null;
        this.min_wgs84 = null;
        this.max_wgs84 = null;
        this.transformation_translation = null;
        this.transformation_rotation = 0.0;
        this.transformation_scale = 0.0;
        this.elevation_3d = 0.0;
        this.show_flag = "";
        this.indoor_area = null;
        this.childernLayers = [];
    }
    else {
        this.type = S_LayerTypeJS_Enum[S_LayerTransformation.type];
        this.identif = S_LayerTransformation.identif == null ? null : new S_EOIdentifierJS(S_LayerTransformation.identif);
        this.min_local = S_LayerTransformation.min_local == null ? null : new S_EOPointJS(S_LayerTransformation.min_local);
        this.max_local = S_LayerTransformation.max_local == null ? null : new S_EOPointJS(S_LayerTransformation.max_local);
        this.min_wgs84 = S_LayerTransformation.min_wgs84 == null ? null : new S_WGS84JS(S_LayerTransformation.min_wgs84);
        this.max_wgs84 = S_LayerTransformation.max_wgs84 == null ? null : new S_WGS84JS(S_LayerTransformation.max_wgs84);
        this.transformation_translation = S_LayerTransformation.transformation_translation == null ? null : new S_EOPointJS(S_LayerTransformation.transformation_translation);
        this.transformation_rotation = S_LayerTransformation.transformation_rotation;
        this.transformation_scale = S_LayerTransformation.transformation_scale;
        this.elevation_3d = S_LayerTransformation.elevation_3d;
        this.show_flag = S_LayerTransformation.show_flag;
        this.indoor_area = S_LayerTransformation.indoor_area == null ? null : new S_EOPolyAreaJS(S_LayerTransformation.indoor_area);
        this.childernLayers = [];
        for (var i in S_LayerTransformation.childernLayers) {
            this.childernLayers.push(new S_LayerTransformationJS(S_LayerTransformation.childernLayers[i]));
        }
    }
}
function S_LayerTransformationJS(S_LayerTransformation) {
    this.init(S_LayerTransformation);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_DateTimeJS.prototype.init = function (S_DateTime) {
    if (S_DateTime == null) {
        this.year = 1;
        this.month = 1;
        this.day = 0;
        this.hour = 0;
        this.minute = 0;
        this.second = 0;
        this.milisecond = 0;
    }
    else {
        this.year = S_DateTime.year;
        this.month = S_DateTime.month;
        this.day = S_DateTime.day;
        this.hour = S_DateTime.hour;
        this.minute = S_DateTime.minute;
        this.second = S_DateTime.second;
        this.milisecond = S_DateTime.milisecond;
    }
}
function S_DateTimeJS(S_DateTime) {
    this.init(S_DateTime);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_PositionJS.prototype.init = function (S_Position) {
    if (S_Position == null) {
        this.project_id = "";
        this.building_id = "";
        this.floorplan_id = "";
        this.point = new S_EOPointJS();
    }
    else {
        this.project_id = S_Position.project_id;
        this.building_id = S_Position.building_id;
        this.floorplan_id = S_Position.floorplan_id;
        this.point = S_Position.point == null ? null : new S_EOPointJS(S_Position.point);
    }
}
function S_PositionJS(S_Position) {
    this.init(S_Position);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_EOPointJS.prototype.init = function (S_EOPoint) {
    if (S_EOPoint == null) {
        this.x = 0.0;
        this.y = 0.0;
        this.z = 0.0;
    }
    else {
        this.x = S_EOPoint.x;
        this.y = S_EOPoint.y;
        this.z = S_EOPoint.z;
    }
}
function S_EOPointJS(S_EOPoint) {
    this.init(S_EOPoint);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_WGS84JS.prototype.init = function (S_WGS84) {
    if (S_WGS84 == null) {
        this.lat = 0.0;
        this.lon = 0.0;
    }
    else {
        this.lat = S_WGS84.lat;
        this.lon = S_WGS84.lon;
    }
}
function S_WGS84JS(S_WGS84) {
    this.init(S_WGS84);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
//http://stackoverflow.com/questions/287903/enums-in-javascript
/*
var MyEnum;
(function (MyEnum) {
    MyEnum[MyEnum["Foo"] = 0] = "Foo";
    MyEnum[MyEnum["FooBar"] = 2] = "FooBar";
    MyEnum[MyEnum["Bar"] = 1] = "Bar";
})(MyEnum|| (MyEnum= {}));

  //  if (this.type == MyEnum[MyEnum.Foo])
  //      var dd = 4;
*/
//------------------------------------------------------------------------------------------------------------------------------------------------------------
var S_PointObjectTypeJS_Enum;
(function (S_PointObjectTypeJS_Enum) {
    S_PointObjectTypeJS_Enum[S_PointObjectTypeJS_Enum["S_PointObjectType_undefined"] = 0] = "S_PointObjectType_undefined";
    S_PointObjectTypeJS_Enum[S_PointObjectTypeJS_Enum["S_PointObjectType_accesspoint"] = 1] = "S_PointObjectType_accesspoint";
    S_PointObjectTypeJS_Enum[S_PointObjectTypeJS_Enum["S_PointObjectType_sensorpoint"] = 2] = "S_PointObjectType_sensorpoint";
})(S_PointObjectTypeJS_Enum || (S_PointObjectTypeJS_Enum = {}));
//------------------------------------------------------------------------------------------------------------------------------------------------------------
var S_LineObjectTypeJS_Enum;
(function (S_LineObjectTypeJS_Enum) {
    S_LineObjectTypeJS_Enum[S_LineObjectTypeJS_Enum["S_LineObjectType_undefined"] = 0] = "S_LineObjectType_undefined";
    S_LineObjectTypeJS_Enum[S_LineObjectTypeJS_Enum["S_LineObjectType_wall"] = 1] = "S_LineObjectType_wall";
    S_LineObjectTypeJS_Enum[S_LineObjectTypeJS_Enum["S_LineObjectType_window"] = 2] = "S_LineObjectType_window";
    S_LineObjectTypeJS_Enum[S_LineObjectTypeJS_Enum["S_LineObjectType_door"] = 3] = "S_LineObjectType_door";
    S_LineObjectTypeJS_Enum[S_LineObjectTypeJS_Enum["S_LineObjectType_edge"] = 4] = "S_LineObjectType_edge";
    S_LineObjectTypeJS_Enum[S_LineObjectTypeJS_Enum["S_LineObjectType_helpline"] = 5] = "S_LineObjectType_helpline";
})(S_LineObjectTypeJS_Enum || (S_LineObjectTypeJS_Enum = {}));
//------------------------------------------------------------------------------------------------------------------------------------------------------------
var S_PolygonObjectTypeJS_Enum;
(function (S_PolygonObjectTypeJS_Enum) {
    S_PolygonObjectTypeJS_Enum[S_PolygonObjectTypeJS_Enum["S_PolygonObjectType_undefined"] = 0] = "S_PolygonObjectType_undefined";
    S_PolygonObjectTypeJS_Enum[S_PolygonObjectTypeJS_Enum["S_PolygonObjectType_transarea"] = 1] = "S_PolygonObjectType_transarea";
    S_PolygonObjectTypeJS_Enum[S_PolygonObjectTypeJS_Enum["S_PolygonObjectType_contextarea"] = 2] = "S_PolygonObjectType_contextarea";
    S_PolygonObjectTypeJS_Enum[S_PolygonObjectTypeJS_Enum["S_PolygonObjectType_indoorarea"] = 3] = "S_PolygonObjectType_indoorarea";
    S_PolygonObjectTypeJS_Enum[S_PolygonObjectTypeJS_Enum["S_PolygonObjectType_patharea"] = 4] = "S_PolygonObjectType_patharea";
    S_PolygonObjectTypeJS_Enum[S_PolygonObjectTypeJS_Enum["S_PolygonObjectType_containmentarea"] = 5] = "S_PolygonObjectType_containmentarea";
    S_PolygonObjectTypeJS_Enum[S_PolygonObjectTypeJS_Enum["S_PolygonObjectType_voidarea"] = 6] = "S_PolygonObjectType_voidarea";
    S_PolygonObjectTypeJS_Enum[S_PolygonObjectTypeJS_Enum["S_PolygonObjectType_outlinearea"] = 7] = "S_PolygonObjectType_outlinearea";
    S_PolygonObjectTypeJS_Enum[S_PolygonObjectTypeJS_Enum["S_PolygonObjectType_builtuparea"] = 8] = "S_PolygonObjectType_builtuparea";
    S_PolygonObjectTypeJS_Enum[S_PolygonObjectTypeJS_Enum["S_PolygonObjectType_generalarea"] = 9] = "S_PolygonObjectType_generalarea";
})(S_PolygonObjectTypeJS_Enum || (S_PolygonObjectTypeJS_Enum = {}));
//------------------------------------------------------------------------------------------------------------------------------------------------------------
var S_LayerTypeJS_Enum;
(function (S_LayerTypeJS_Enum) {
    S_LayerTypeJS_Enum[S_LayerTypeJS_Enum["S_LayerType_unknown"] = 0] = "S_LayerType_unknown";
    S_LayerTypeJS_Enum[S_LayerTypeJS_Enum["S_LayerType_project"] = 1] = "S_LayerType_project";
    S_LayerTypeJS_Enum[S_LayerTypeJS_Enum["S_LayerType_building"] = 2] = "S_LayerType_building";
    S_LayerTypeJS_Enum[S_LayerTypeJS_Enum["S_LayerType_floor"] = 3] = "S_LayerType_floor";
})(S_LayerTypeJS_Enum || (S_LayerTypeJS_Enum = {}));
//------------------------------------------------------------------------------------------------------------------------------------------------------------
// TransformableObject ###################################
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_TransformableObjectJS.prototype.init = function (S_TransformableObject) {
    if (S_TransformableObject == null) {
        this.db_id = -1;
        this.identif = null;
        this.pointObjects = [];
        this.lineObjects = [];
        this.polyAreas = [];
        this.navigationNodes = {};
        this.drawingAtts = null;
        this.child = null;
    }
    else {
        this.db_id = S_TransformableObject.db_id;
        this.identif = S_TransformableObject.identif == null ? null : new S_EOIdentifierJS(S_TransformableObject.identif);
        this.drawingAtts = S_TransformableObject.drawingAtts == null ? null : new S_EOAttributeDataJS(S_TransformableObject.drawingAtts);
        this.pointObjects = [];
        for (var i in S_TransformableObject.pointObjects)
            this.pointObjects.push(new S_PointObjectJS(S_TransformableObject.pointObjects[i]));
        this.lineObjects = [];
        for (var i in S_TransformableObject.lineObjects)
            this.lineObjects.push(new S_LineObjectJS(S_TransformableObject.lineObjects[i]));
        this.polyAreas = [];
        for (var i in S_TransformableObject.polyAreas)
            this.polyAreas.push(new S_EOPolyAreaJS(S_TransformableObject.polyAreas[i]));
        this.navigationNodes = {};
        for (var i in S_TransformableObject.navigationNodes)
            this.navigationNodes[S_TransformableObject.navigationNodes[i].index] = (new S_NavigationNodeJS(S_TransformableObject.navigationNodes[i]));
        this.child = null;
        if (S_TransformableObject.S_ProjectData != null)
            this.child = new S_ProjectDataJS(S_TransformableObject.S_ProjectData);
        else if (S_TransformableObject.S_BuildingData != null)
            this.child = new S_BuildingDataJS(S_TransformableObject.S_BuildingData);
        else if (S_TransformableObject.S_FloorplanData != null)
            this.child = new S_FloorplanDataJS(S_TransformableObject.S_FloorplanData);
    }
}
function S_TransformableObjectJS(S_TransformableObject) {
    this.init(S_TransformableObject);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_ProjectDataJS.prototype.init = function (S_ProjectData) {
}
function S_ProjectDataJS(S_ProjectData) {
    this.init(S_ProjectData);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_BuildingDataJS.prototype.init = function (S_BuildingData) {
}
function S_BuildingDataJS(S_BuildingData) {
    this.init(S_BuildingData);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_FloorplanDataJS.prototype.init = function (S_FloorplanData) {
}
function S_FloorplanDataJS(S_FloorplanData) {
    this.init(S_FloorplanData);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_PointObjectJS.prototype.init = function (S_PointObject) {
    if (S_PointObject == null) {
        this.id = -1;
        this.name = "";
        this.centre = null;
        this.object_type = S_PointObjectTypeJS_Enum[0];
        this.object_id = -1;
        this.material = "";
        this.drawingAtt = null;
        this.tag_0 = "";
        this.tag_1 = "";
        this.tag_2 = "";
        this.child = null;
    }
    else {
        this.id = S_PointObject.id;
        this.name = S_PointObject.name;
        this.object_type = S_PointObjectTypeJS_Enum[S_PointObject.object_type];
        this.centre = S_PointObject.centre == null ? null : new S_PositionJS(S_PointObject.centre);
        this.object_id = S_PointObject.object_id;
        this.material = S_PointObject.material;
        this.drawingAtt = S_PointObject.drawingAtt == null ? null : new S_DrawingObjectAttributeJS(S_PointObject.drawingAtt);
        this.tag_0 = S_PointObject.tag_0;
        this.tag_1 = S_PointObject.tag_1;
        this.tag_2 = S_PointObject.tag_2;
        this.child = null;
        if (S_PointObject.S_EOAccessPoint != null)
            this.child = new S_EOAccessPointJS(S_PointObject.S_EOAccessPoint);
        else if (S_PointObject.S_EOSensorPoint != null)
            this.child = new S_EOSensorPointJS(S_PointObject.S_EOSensorPoint);
    }
}
function S_PointObjectJS(S_PointObject) {
    this.init(S_PointObject);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_EOAccessPointJS.prototype.init = function (S_EOAccessPoint) {
}
function S_EOAccessPointJS(S_EOAccessPoint) {
    this.init(S_EOAccessPoint);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_EOSensorPointJS.prototype.init = function (S_EOSensorPoint) {
    if (S_EOSensorPoint == null) {
        this.sensors = [];
    }
    else {
        this.sensors = [];
        for (var i in S_EOSensorPoint.sensors)
            this.sensors.push(new S_EOSensorJS(S_EOSensorPoint.sensors[i]));
    }
}
function S_EOSensorPointJS(S_EOSensorPoint) {
    this.init(S_EOSensorPoint);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_EOSensorJS.prototype.init = function (S_EOSensor) {
    if (S_EOSensor == null) {
        this.type = "";
    }
    else {
        this.type = S_EOSensor.type;
    }
}
function S_EOSensorJS(S_EOSensor) {
    this.init(S_EOSensor);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_LineObjectJS.prototype.init = function (S_LineObject) {
    if (S_LineObject == null) {
        this.id = -1;
        this.name = "";
        this.object_type = S_LineObjectTypeJS_Enum[0];
        this.material = "";
        this.drawingAtt = null;
        this.startpt = null;
        this.endpt = null;
    }
    else {
        this.id = S_LineObject.id;
        this.name = S_LineObject.name;
        this.object_type = S_LineObjectTypeJS_Enum[S_LineObject.object_type];
        this.material = S_LineObject.material;
        this.drawingAtt = S_LineObject.drawingAtt == null ? null : new S_DrawingObjectAttributeJS(S_LineObject.drawingAtt);
        this.startpt = S_LineObject.startpt == null ? null : new S_EOPointJS(S_LineObject.startpt);
        this.endpt = S_LineObject.endpt == null ? null : new S_EOPointJS(S_LineObject.endpt);
    }
}
function S_LineObjectJS(S_LineObject) {
    this.init(S_LineObject);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_EOPolyAreaJS.prototype.init = function (S_EOPolyArea) {
    if (S_EOPolyArea == null) {
        this.id = -1;
        this.name = "";
        this.object_type = S_PolygonObjectTypeJS_Enum[0];
        this.material = "";
        this.drawingAtt = null;
        this.points = [];
    }
    else {
        this.id = S_EOPolyArea.id;
        this.name = S_EOPolyArea.name;
        this.object_type = S_PolygonObjectTypeJS_Enum[S_EOPolyArea.object_type];
        this.material = S_EOPolyArea.material;
        this.drawingAtt = S_EOPolyArea.drawingAtt == null ? null : new S_DrawingObjectAttributeJS(S_EOPolyArea.drawingAtt);
        this.points = [];
        for (var i in S_EOPolyArea.points) {
            this.points.push(new S_EOPointJS(S_EOPolyArea.points[i]));
        }
    }
}
function S_EOPolyAreaJS(S_EOPolyArea) {
    this.init(S_EOPolyArea);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_NavigationNodeJS.prototype.init = function (S_NavigationNode) {
    if (S_NavigationNode == null) {
        this.db_id = -1;
        this.index = -1;
        this.position = null;
        this.projectPoint = null;
        this.distance = 0.0;
        this.visited = false;
        this.dijsktraindex = 0;
        this.previousNode = -1;
        this.neighbours = [];
    }
    else {
        this.db_id = S_NavigationNode.db_id;
        this.index = S_NavigationNode.index;
        this.position = S_NavigationNode.position == null ? null : new S_PositionJS(S_NavigationNode.position);
        this.projectPoint = S_NavigationNode.projectPoint == null ? null : new S_EOPointJS(S_NavigationNode.projectPoint);
        this.distance = S_NavigationNode.distance;
        this.visited = S_NavigationNode.visited;
        this.dijsktraindex = S_NavigationNode.dijsktraindex;
        this.previousNode = S_NavigationNode.previousNode;
        this.neighbours = [];
        for (var i in S_NavigationNode.neighbours) {
            this.neighbours.push(new S_NavigationNeighbourJS(S_NavigationNode.neighbours[i]));
        }
    }
}
function S_NavigationNodeJS(S_NavigationNode) {
    this.init(S_NavigationNode);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_NavigationNeighbourJS.prototype.init = function (S_NavigationNeighbour) {
    if (S_NavigationNeighbour == null) {
        this.identifier = null;
        this.index = -1;
        this.direction = 0;
        this.type = "";
        this.distance = 0.0;
    }
    else {
        this.identifier = S_NavigationNeighbour.identifier == null ? null : new S_EOIdentifierJS(S_NavigationNeighbour.identifier);
        this.index = S_NavigationNeighbour.index;
        this.direction = S_NavigationNeighbour.direction;
        this.type = S_NavigationNeighbour.type;
        this.distance = S_NavigationNeighbour.distance;
    }
}
function S_NavigationNeighbourJS(S_NavigationNeighbour) {
    this.init(S_NavigationNeighbour);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
var myRevealingModule = (function () {

    var privateCounter = 0;

    function privateFunction() {
        privateCounter++;
    }

    function publicFunction() {
        publicIncrement();
    }

    function publicIncrement() {
        privateFunction();
    }

    function publicGetCount() {
        return privateCounter;
    }

    // Reveal public pointers to
    // private functions and properties

    return {
        start: publicFunction,
        increment: publicIncrement,
        count: publicGetCount
    };

})();

