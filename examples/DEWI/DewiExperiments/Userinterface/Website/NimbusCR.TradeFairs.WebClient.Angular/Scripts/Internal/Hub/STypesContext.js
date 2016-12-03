// this file should be autogenereted together with SType.proto files

//------------------------------------------------------------------------------------------------------------------------------------------------------------
// Context CMS ###################################
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSTransformableObjectJS.prototype.init = function (S_CMSTransformableObject) {

    if (S_CMSTransformableObject == null) {
        this.transformableObject = null;
        this.polyAreas = [];
    }
    else {
        this.transformableObject = S_CMSTransformableObject.transformableObject == null ? null : new S_TransformableObjectJS(S_CMSTransformableObject.transformableObject);
        this.polyAreas = [];
        for (var i in S_CMSTransformableObject.polyAreas)
            this.polyAreas.push(new S_CMSPolyAreaJS(S_CMSTransformableObject.polyAreas[i]));
    }
}

function S_CMSTransformableObjectJS(S_CMSTransformableObject) {
    this.init(S_CMSTransformableObject)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSPolyAreaJS.prototype.init = function (S_CMSPolyArea) {

    if (S_CMSPolyArea == null) {
        this.polyArea = null;
        this.labels = [];
        this.images = [];
    }
    else {
        this.polyArea = S_CMSPolyArea.polyArea == null ? null : new S_EOPolyAreaJS(S_CMSPolyArea.polyArea);
        this.labels = [];
        for (var i in S_CMSPolyArea.labels) this.labels.push(new S_CMSEOLabelJS(S_CMSPolyArea.labels[i]));
        this.images = [];
        for (var i in S_CMSPolyArea.images) this.images.push(new S_CMSEOImageJS(S_CMSPolyArea.images[i]));
    }
}

function S_CMSPolyAreaJS(S_CMSPolyArea) {
    this.init(S_CMSPolyArea)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSEOImageJS.prototype.init = function (S_CMSEOImage) {

    if (S_CMSEOImage == null) {
        this.id = -1;
        this.image_url = "";
        this.image_string = "";
        this.transformation = null;
        this.opacity = 0.8;
        this.ref_type = "";
        this.ref_dbids = [];
    }
    else {
        this.id = S_CMSEOImage.id;
        this.image_url = S_CMSEOImage.image_url;
        this.image_string = S_CMSEOImage.image_string;
        this.transformation = S_CMSEOImage.transformation == null ? null : new S_LayerTransformationJS(S_CMSEOImage.transformation);
        this.opacity = S_CMSEOImage.opacity;
        this.ref_type = S_CMSEOImage.ref_type;

        this.ref_dbids = [];
        for (var i in S_CMSEOImage.ref_dbids) {
            this.ref_dbids.push(S_CMSEOImage.ref_dbids[i]);
        }
    }
}

function S_CMSEOImageJS(S_CMSEOImage) {
    this.init(S_CMSEOImage)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
var S_CMSFontFamilyJS_Enum;
(function (S_CMSFontFamilyJS_Enum) {
    S_CMSFontFamilyJS_Enum[S_CMSFontFamilyJS_Enum["S_CMSFontFamily_Arial"] = 0] = "S_CMSFontFamily_Arial";
    S_CMSFontFamilyJS_Enum[S_CMSFontFamilyJS_Enum["S_CMSFontFamily_Calibri"] = 1] = "S_CMSFontFamily_Calibri";
    S_CMSFontFamilyJS_Enum[S_CMSFontFamilyJS_Enum["S_CMSFontFamily_Verdana"] = 2] = "S_CMSFontFamily_Verdana";
})(S_CMSFontFamilyJS_Enum || (S_CMSFontFamilyJS_Enum = {}));
S_CMSFontFamilyJS_Array = [S_CMSFontFamilyJS_Enum[0], S_CMSFontFamilyJS_Enum[1], S_CMSFontFamilyJS_Enum[2]];
//------------------------------------------------------------------------------------------------------------------------------------------------------------
var S_CMSFontStyleJS_Enum;
(function (S_CMSFontStyleJS_Enum) {
    S_CMSFontStyleJS_Enum[S_CMSFontStyleJS_Enum["S_CMSFontStyle_normal"] = 0] = "S_CMSFontStyle_normal";
    S_CMSFontStyleJS_Enum[S_CMSFontStyleJS_Enum["S_CMSFontStyle_bold"] = 1] = "S_CMSFontStyle_bold";
    S_CMSFontStyleJS_Enum[S_CMSFontStyleJS_Enum["S_CMSFontStyle_italic"] = 2] = "S_CMSFontStyle_italic";
})(S_CMSFontStyleJS_Enum || (S_CMSFontStyleJS_Enum = {}));
S_CMSFontStyleJS_Array = [S_CMSFontStyleJS_Enum[0], S_CMSFontStyleJS_Enum[1], S_CMSFontStyleJS_Enum[2]];
//------------------------------------------------------------------------------------------------------------------------------------------------------------
var S_CMSTextAlignJS_Enum;
(function (S_CMSTextAlignJS_Enum) {
    S_CMSTextAlignJS_Enum[S_CMSTextAlignJS_Enum["S_CMSTextAlign_left"] = 0] = "S_CMSTextAlign_left";
    S_CMSTextAlignJS_Enum[S_CMSTextAlignJS_Enum["S_CMSTextAlign_center"] = 1] = "S_CMSTextAlign_center";
    S_CMSTextAlignJS_Enum[S_CMSTextAlignJS_Enum["S_CMSTextAlign_right"] = 2] = "S_CMSTextAlign_right";
})(S_CMSTextAlignJS_Enum || (S_CMSTextAlignJS_Enum = {}));
S_CMSTextAlignJS_Array = [S_CMSTextAlignJS_Enum[0], S_CMSTextAlignJS_Enum[1], S_CMSTextAlignJS_Enum[2]];
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSEOLabelJS.prototype.init = function (S_CMSEOLabel) {

    if (S_CMSEOLabel == null) {
        this.id = -1;
        this.object_name = "";
        this.text = "";
        this.transformation = null;
        this.fontFamily = S_CMSFontFamilyJS_Enum.S_CMSFontFamily_Arial;
        this.fontStyle = S_CMSFontStyleJS_Enum.S_CMSFontStyle_normal;
        this.align = S_CMSTextAlignJS_Enum.S_CMSTextAlign_left;
        this.fontSize = 1;
        this.background_argb = [0, 255, 255, 255];
        this.foreground_argb = [255, 0, 0, 0];
        this.opacity = 1;
        this.ref_type = "";
        this.ref_dbids = [];
    }
    else {
        this.id = S_CMSEOLabel.id;
        this.object_name = S_CMSEOLabel.object_name;
        this.text = S_CMSEOLabel.text;
        this.transformation = S_CMSEOLabel.transformation == null ? null : new S_LayerTransformationJS(S_CMSEOLabel.transformation);
        this.fontFamily = S_CMSFontFamilyJS_Enum[S_CMSEOLabel.fontFamily];
        this.fontStyle = S_CMSFontStyleJS_Enum[S_CMSEOLabel.fontStyle];
        this.align = S_CMSTextAlignJS_Enum[S_CMSEOLabel.align];
        this.fontSize = S_CMSEOLabel.fontSize;
        this.opacity = S_CMSEOLabel.opacity;
        this.ref_type = S_CMSEOLabel.ref_type;

        this.ref_dbids = [];
        for (var i in S_CMSEOLabel.ref_dbids) {
            this.ref_dbids.push(S_CMSEOLabel.ref_dbids[i]);
        }

        this.background_argb = [0, 255, 255, 255];
        if (typeof S_CMSEOLabel.background_argb == 'object')
            if (S_CMSEOLabel.background_argb.length > 0) {
                var element0 = S_CMSEOLabel.background_argb[0];
                for (var i = element0.offset; i < element0.limit; i++)
                    this.background_argb[i - element0.offset] = element0.view[i];
            }

        this.foreground_argb = [255, 0, 0, 0];
        if (typeof S_CMSEOLabel.foreground_argb == 'object')
            if (S_CMSEOLabel.foreground_argb.length > 0) {
                var element0 = S_CMSEOLabel.foreground_argb[0];
                for (var i = element0.offset; i < element0.limit; i++)
                    this.foreground_argb[i - element0.offset] = element0.view[i];
            }
    }
}

function S_CMSEOLabelJS(S_CMSEOLabel) {
    this.init(S_CMSEOLabel)
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

ListNavNodeLinkJS.prototype.init = function (ListNavNodeLink) {

    this.items = [];
    if (typeof ListNavNodeLink != "undefined")
        for (var i in ListNavNodeLink.items)
            this.items.push(new S_NavNodeLinkJS(ListNavNodeLink.items[i]));
}

function ListNavNodeLinkJS(ListNavNodeLink) {
    this.init(ListNavNodeLink);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_NavNodeLinkJS.prototype.init = function (S_NavNodeLink) {

    if (S_NavNodeLink == null) {
        this.nav_node_dbid = -1;
        this.prime = false;
        this.start = false;
        this.nav_node_id = -1;
    }
    else {
        this.nav_node_dbid = S_NavNodeLink.nav_node_dbid;
        this.prime = S_NavNodeLink.prime;
        this.start = S_NavNodeLink.start;
        this.nav_node_id = S_NavNodeLink.nav_node_id;
    }
}
S_NavNodeLinkJS.prototype.Convert = function () {

    var result = new S_NavNodeLink();
    result.nav_node_dbid = this.nav_node_dbid;
    result.prime = this.prime;
    result.start = this.start;
    result.nav_node_id = this.nav_node_id;
    return result;
}
function S_NavNodeLinkJS(S_NavNodeLink) {
    this.init(S_NavNodeLink)
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

ListCMSDestinationJS.prototype.init = function (ListCMSDestination) {

    this.items = [];
    if (typeof ListCMSDestination != "undefined")
        for (var i in ListCMSDestination.items)
            this.items.push(new S_CMSDestinationJS(ListCMSDestination.items[i]));
}

function ListCMSDestinationJS(ListCMSDestination) {
    this.init(ListCMSDestination);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSDestinationJS.prototype.init = function (S_CMSDestination) {

    if (S_CMSDestination == null) {
        this.id = -1;
        this.entity_id = -1;
        this.entity_type = "";
        this.text = "";
        this.position = null;
        this.navNode_dbid_Links = [];
    }
    else {
        this.id = S_CMSDestination.id;
        this.entity_id = S_CMSDestination.entity_id;
        this.entity_type = S_CMSDestination.entity_type;
        this.text = S_CMSDestination.text;
        this.position = S_CMSDestination.position == null ? null : new S_PositionJS(S_CMSDestination.position);

        this.navNode_dbid_Links = [];
        for (var i in S_CMSDestination.navNode_dbid_Links) {
            this.navNode_dbid_Links.push(new S_NavNodeLinkJS(S_CMSDestination.navNode_dbid_Links[i]));
        }
    }
}
S_CMSDestinationJS.prototype.Convert = function () {

    var result = new S_CMSDestination();
    result.id = this.id;
    result.entity_id = this.entity_id;
    result.entity_type = this.entity_type;
    result.text = this.text;
    result.position = this.position;

    result.navNode_dbid_Links = [];
    for (var i in this.navNode_dbid_Links) {
        if (typeof this.navNode_dbid_Links[i].Convert() != "undefined")
            result.navNode_dbid_Links.push(this.navNode_dbid_Links[i].Convert());
    }
    return result;
}
function S_CMSDestinationJS(S_CMSDestination) {
    this.init(S_CMSDestination)
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

function GetColorBytes(color) {
    var result = [0x88, 0x88, 0x88, 0xFF];
    if (typeof color == 'object')
        if (color.length > 0) {
            var element0 = color[0];
            for (var i = element0.offset; i < element0.limit; i++) {
                result[i - element0.offset] = element0.view[i];
            }
        }
    return result;
}
//---------------------------------------------------------------------------------------------------------------------------------------------
var S_ShowFlagsJS_Enum;
(function (S_ShowFlagsJS_Enum) {
    S_ShowFlagsJS_Enum[S_ShowFlagsJS_Enum["S_ShowFlags_default"] = 0] = "S_ShowFlags_default";
    S_ShowFlagsJS_Enum[S_ShowFlagsJS_Enum["S_ShowFlags_alwaysshow"] = 1] = "S_ShowFlags_alwaysshow";
    S_ShowFlagsJS_Enum[S_ShowFlagsJS_Enum["S_ShowFlags_nevershow"] = 2] = "S_ShowFlags_nevershow";
    S_ShowFlagsJS_Enum[S_ShowFlagsJS_Enum["S_ShowFlags_projecttile"] = 3] = "S_ShowFlags_projecttile";
    S_ShowFlagsJS_Enum[S_ShowFlagsJS_Enum["S_ShowFlags_showwithparent"] = 4] = "S_ShowFlags_showwithparent";
    S_ShowFlagsJS_Enum[S_ShowFlagsJS_Enum["S_ShowFlags_showwithproject"] = 5] = "S_ShowFlags_showwithproject";
    S_ShowFlagsJS_Enum[S_ShowFlagsJS_Enum["S_ShowFlags_skipparent"] = 6] = "S_ShowFlags_skipparent";
    S_ShowFlagsJS_Enum[S_ShowFlagsJS_Enum["S_ShowFlags_detail"] = 7] = "S_ShowFlags_detail";
})(S_ShowFlagsJS_Enum || (S_ShowFlagsJS_Enum = {}));
S_ShowFlagsJS_Array = [S_ShowFlagsJS_Enum[0], S_ShowFlagsJS_Enum[1], S_ShowFlagsJS_Enum[2], S_ShowFlagsJS_Enum[3], S_ShowFlagsJS_Enum[4], S_ShowFlagsJS_Enum[5], S_ShowFlagsJS_Enum[6], S_ShowFlagsJS_Enum[7]];

//---------------------------------------------------------------------------------------------------------------------------------------------
ListCMSLayerData3DJS.prototype.init = function (ListCMSLayerData3D) {

    this.items = [];
    if (typeof ListCMSLayerData3D != "undefined")
        for (var i in ListCMSLayerData3D.items)
            this.items.push(new S_LayerCMSData3DJS(ListCMSLayerData3D.items[i]));
}

function ListCMSLayerData3DJS(ListCMSLayerData3D) {
    this.init(ListCMSLayerData3D);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_LayerCMSData3DJS.prototype.init = function (S_CMSLayerData3D) {

    if (S_CMSLayerData3D == null) {
        this.sizeStatistics = null;
        this.viz = [];
        this.data3D = "";
        this.type = "";
    }
    else {
        this.sizeStatistics = new S_SizeStatisticsJS(S_CMSLayerData3D.sizeStatistics);
        this.viz = S_CMSLayerData3D.viz;
        this.type = S_CMSLayerData3D.type;

        this.data3D = "";
        for (var i in S_CMSLayerData3D.data3D) {
            this.data3D += S_CMSLayerData3D.data3D[i];
        }
    }
}

function S_LayerCMSData3DJS(S_CMSLayerData3D) {
    this.init(S_CMSLayerData3D)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_SizeStatisticsJS.prototype.init = function (S_SizeStatistics) {

    if (S_SizeStatistics == null) {
        this.triangles = 0;
        this.texturePixels = 0;
        this.lowdetail = true;
        this.normaldetail = true;
        this.highdetail = true;
    }
    else {
        this.triangles = S_SizeStatistics.triangles;
        this.texturePixels = S_SizeStatistics.texturePixels;
        this.lowdetail = S_SizeStatistics.lowdetail;
        this.normaldetail = S_SizeStatistics.normaldetail;
        this.highdetail = S_SizeStatistics.highdetail;
    }
}

function S_SizeStatisticsJS(S_SizeStatistics) {
    this.init(S_SizeStatistics)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
ListCMSData3DJS.prototype.init = function (ListCMSData3D) {

    this.items = [];
    if (typeof ListCMSData3D != "undefined")
        for (var i in ListCMSData3D.items)
            this.items.push(new S_CMSData3DJS(ListCMSData3D.items[i]));
}

function ListCMSData3DJS(ListCMSData3D) {
    this.init(ListCMSData3D);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSData3DJS.prototype.init = function (S_CMSData3D) {

    if (S_CMSData3D == null) {
        this.type = "";
        this.child = null;
    }
    else {
        this.type = S_CMSData3D.type;

        this.child = null;
        if (S_CMSData3D.S_CMSLayer3DSetting != null)
            this.child = new S_CMSLayer3DSettingJS(S_CMSData3D.S_CMSLayer3DSetting);
        else if (S_CMSData3D.S_CMSLayerNavigation3DSetting != null)
            this.child = new S_CMSLayerNavigation3DSettingJS(S_CMSData3D.S_CMSLayerNavigation3DSetting);
        else if (S_CMSData3D.S_CMSScenes3D != null)
            this.child = new S_CMSScenes3DJS(S_CMSData3D.S_CMSScenes3D);
        else if (S_CMSData3D.S_CMSEntityGroup3D != null)
            this.child = new S_CMSEntityGroup3DJS(S_CMSData3D.S_CMSEntityGroup3D);
        else if (S_CMSData3D.S_CMSEventScenes3D != null)
            this.child = new S_CMSEventScenes3DJS(S_CMSData3D.S_CMSEventScenes3D);
        else if (S_CMSData3D.S_CMSTransformation3D != null)
            this.child = new S_CMSTransformation3DJS(S_CMSData3D.S_CMSTransformation3D);
        else if (S_CMSData3D.S_CMSTextureRepository3D != null)
            this.child = new S_CMSTextureRepository3DJS(S_CMSData3D.S_CMSTextureRepository3D);
        else if (S_CMSData3D.S_CMSModelRepository3D != null)
            this.child = new S_CMSModelRepository3DJS(S_CMSData3D.S_CMSModelRepository3D);
    }
}

function S_CMSData3DJS(S_CMSData3D) {
    this.init(S_CMSData3D)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSLayer3DSettingJS.prototype.init = function (S_CMSLayer3DSetting) {

    if (S_CMSLayer3DSetting == null) {
        this.init_camera_point = null;
        this.init_camera_distance = 1000;
        this.init_camera_azimuth = 0;
        this.init_camera_tilt = 0.5;
        this.max_camera_distance = 10000;
        this.min_camera_distance = 1;
        this.min_x = -1000;
        this.min_y = -1000;
        this.max_x = 1000;
        this.max_y = 1000;
        this.z_near = 4;
        this.z_far = 10000;
        this.min_tilt = 0;
        this.max_tilt = 1;
    }
    else {
        this.init_camera_point = S_CMSLayer3DSetting.init_camera_point == null ? null : new S_EOPointJS(S_CMSLayer3DSetting.init_camera_point);
        this.init_camera_distance = S_CMSLayer3DSetting.init_camera_distance;
        this.init_camera_azimuth = S_CMSLayer3DSetting.init_camera_azimuth;
        this.init_camera_tilt = S_CMSLayer3DSetting.init_camera_tilt;
        this.max_camera_distance = S_CMSLayer3DSetting.max_camera_distance;
        this.min_camera_distance = S_CMSLayer3DSetting.min_camera_distance;
        this.min_x = S_CMSLayer3DSetting.min_x;
        this.min_y = S_CMSLayer3DSetting.min_y;
        this.max_x = S_CMSLayer3DSetting.max_x;
        this.max_y = S_CMSLayer3DSetting.max_y;
        this.z_near = S_CMSLayer3DSetting.z_near;
        this.z_far = S_CMSLayer3DSetting.z_far;
        this.min_tilt = S_CMSLayer3DSetting.min_tilt;
        this.max_tilt = S_CMSLayer3DSetting.max_tilt;
    }
}

function S_CMSLayer3DSettingJS(S_CMSLayer3DSetting) {
    this.init(S_CMSLayer3DSetting)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_Orientation3DJS.prototype.init = function (S_Orientation3D) {

    if (S_Orientation3D == null) {
        this.lookX = 0;
        this.lookY = 0;
        this.elevation = 0;
        this.tiltAngle = 0.5;
        this.rotationAngle = 0;
        this.distance = 1000;
    }
    else {
        this.lookX = S_Orientation3D.lookX;
        this.lookY = S_Orientation3D.lookY;
        this.elevation = S_Orientation3D.elevation;
        this.tiltAngle = S_Orientation3D.tiltAngle;
        this.rotationAngle = S_Orientation3D.rotationAngle;
        this.distance = S_Orientation3D.distance;
    }
}

function S_Orientation3DJS(S_Orientation3D) {
    this.init(S_Orientation3D)
}
//---------------------------------------------------------------------------------------------------------------------------------------------
var S_CameraSettingApplicationJS_Enum;
(function (S_CameraSettingApplicationJS_Enum) {
    S_CameraSettingApplicationJS_Enum[S_CameraSettingApplicationJS_Enum["S_CameraSettingApplication_full"] = 0] = "S_CameraSettingApplication_full";
    S_CameraSettingApplicationJS_Enum[S_CameraSettingApplicationJS_Enum["S_CameraSettingApplication_none"] = 1] = "S_CameraSettingApplication_none";
    S_CameraSettingApplicationJS_Enum[S_CameraSettingApplicationJS_Enum["S_CameraSettingApplication_distancelooktilt"] = 2] = "S_CameraSettingApplication_distancelooktilt";
    S_CameraSettingApplicationJS_Enum[S_CameraSettingApplicationJS_Enum["S_CameraSettingApplication_distancelook"] = 3] = "S_CameraSettingApplication_distancelook";
})(S_CameraSettingApplicationJS_Enum || (S_CameraSettingApplicationJS_Enum = {}));
S_CameraSettingApplicationJS_Array = [S_CameraSettingApplicationJS_Enum[0], S_CameraSettingApplicationJS_Enum[1], S_CameraSettingApplicationJS_Enum[2], S_CameraSettingApplicationJS_Enum[3]];
//------------------------------------------------------------------------------------------------------------------------------------------------------------
var S_ViewControlModeJS_Enum;
(function (S_ViewControlModeJS_Enum) {
    S_ViewControlModeJS_Enum[S_ViewControlModeJS_Enum["S_ViewControlMode_default"] = 0] = "S_ViewControlMode_default";
    S_ViewControlModeJS_Enum[S_ViewControlModeJS_Enum["S_ViewControlMode_fixtilttodistance"] = 1] = "S_ViewControlMode_fixtilttodistance";
})(S_ViewControlModeJS_Enum || (S_ViewControlModeJS_Enum = {}));
S_ViewControlModeJS_Array = [S_ViewControlModeJS_Enum[0], S_ViewControlModeJS_Enum[1]];
//------------------------------------------------------------------------------------------------------------------------------------------------------------
var S_SceneTypeJS_Enum;
(function (S_SceneTypeJS_Enum) {
    S_SceneTypeJS_Enum[S_SceneTypeJS_Enum["S_SceneType_project"] = 0] = "S_SceneType_project";
    S_SceneTypeJS_Enum[S_SceneTypeJS_Enum["S_SceneType_building"] = 1] = "S_SceneType_building";
    S_SceneTypeJS_Enum[S_SceneTypeJS_Enum["S_SceneType_floor"] = 2] = "S_SceneType_floor";
    S_SceneTypeJS_Enum[S_SceneTypeJS_Enum["S_SceneType_projecttile"] = 3] = "S_SceneType_projecttile";
    S_SceneTypeJS_Enum[S_SceneTypeJS_Enum["S_SceneType_destination"] = 4] = "S_SceneType_destination";
})(S_SceneTypeJS_Enum || (S_SceneTypeJS_Enum = {}));
S_SceneTypeJS_Array = [S_SceneTypeJS_Enum[0], S_SceneTypeJS_Enum[1], S_SceneTypeJS_Enum[2], S_SceneTypeJS_Enum[3], S_SceneTypeJS_Enum[4]];

//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSScenes3DJS.prototype.init = function (S_CMSScenes3D) {

    this.scenes = [];
    if (typeof S_CMSScenes3D != "undefined")
        for (var i in S_CMSScenes3D.scenes)
            this.scenes.push(new S_Scene3DJS(S_CMSScenes3D.scenes[i]));
}

function S_CMSScenes3DJS(S_CMSScenes3D) {
    this.init(S_CMSScenes3D);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
/*ListScene3DJS.prototype.init = function (ListScene3D) {

    this.items = [];
    if (typeof ListScene3D != "undefined")
        for (var i in ListScene3D.items)
            this.items.push(new S_Scene3DJS(ListScene3D.items[i]));
}

function ListScene3DJS(ListScene3D) {
    this.init(ListScene3D);
} */
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_Scene3DJS.prototype.init = function (S_Scene3D) {

    if (S_Scene3D == null) {
        this.fullviz = [];
        this.halfviz = [];
        this.zNear = 4;
        this.zFar = 10000;
        this.orientation = null;
        this.scene_index = -1;
        this.camerasettingapplication = S_CameraSettingApplicationJS_Enum.S_CameraSettingApplication_distancelooktilt;
        this.distance_min = 10;
        this.distance_max = 500;
        this.tilt_min = 10;
        this.tilt_max = 89;
        this.x_min = -1000;
        this.x_max = 1000;
        this.y_min = -1000;
        this.y_max = 1000;
        this.move_sensitivity_offset = 50;
        this.move_sensitivity_factor = 1.5;
        this.rotation_sensitivity = 50;
        this.motion_mode_treshold = 100000;
        this.destination_db_id = -1;
        this.back_scene_index = -1;
        this.title = "";
        this.rotation_enabled = true;
        this.compass_origin = 0;
        this.camera_distance_from_position = 100;
        this.gps_enabled = true;
        this.position_disk_elevation = 1;
        this.position_disk_radious = 15;
        this.zoom_sensitivity_offset = 50;
        this.zoom_sensitivity_factor = 1.5;
        this.show_flag = S_ShowFlagsJS_Enum.S_ShowFlags_default;
        this.start_colour = [0, 255, 150, 150];
        this.end_colour = [0, 150, 150, 255];
        this.highlight_distance_from = 10;
        this.highlight_distance_to = 500;
        this.suppress_distance_from = 10;
        this.suppress_distance_to = 100;
        this.view_control_mode = S_ViewControlModeJS_Enum.S_ViewControlMode_fixtilttodistance
        this.camera_distance_for_navigation = 200;
        this.scene_transition_off_for = -1;
        this.type = S_SceneTypeJS_Enum.S_SceneType_project;
        this.compass_step = 15;
        this.startup_delay = 0;
        this.startup_orientation = null;
        this.center_view_offset = 0.3;
        this.background = [0, 255, 255, 255];
    }
    else {
        this.fullviz = S_Scene3D.fullviz;
        this.halfviz = S_Scene3D.halfviz;
        this.zNear = S_Scene3D.zNear;
        this.zFar = S_Scene3D.zFar;
        this.orientation = S_Scene3D.orientation == null ? null : new S_Orientation3DJS(S_Scene3D.orientation);
        this.scene_index = S_Scene3D.scene_index;
        this.camerasettingapplication = S_CameraSettingApplicationJS_Enum[S_Scene3D.camerasettingapplication];
        this.distance_min = S_Scene3D.distance_min;
        this.distance_max = S_Scene3D.distance_max;
        this.tilt_min = S_Scene3D.tilt_min;
        this.tilt_max = S_Scene3D.tilt_max;
        this.x_min = S_Scene3D.x_min;
        this.x_max = S_Scene3D.x_max;
        this.y_min = S_Scene3D.y_min;
        this.y_max = S_Scene3D.y_max;
        this.move_sensitivity_offset = S_Scene3D.move_sensitivity_offset;
        this.move_sensitivity_factor = S_Scene3D.move_sensitivity_factor;
        this.rotation_sensitivity = S_Scene3D.rotation_sensitivity;
        this.motion_mode_treshold = S_Scene3D.motion_mode_treshold;
        this.destination_db_id = S_Scene3D.destination_db_id;
        this.back_scene_index = S_Scene3D.back_scene_index;
        this.title = S_Scene3D.title;
        this.rotation_enabled = S_Scene3D.rotation_enabled;
        this.compass_origin = S_Scene3D.compass_origin;
        this.camera_distance_from_position = S_Scene3D.camera_distance_from_position;
        this.gps_enabled = S_Scene3D.gps_enabled;
        this.position_disk_elevation = S_Scene3D.position_disk_elevation;
        this.position_disk_radious = S_Scene3D.position_disk_radious;
        this.zoom_sensitivity_offset = S_Scene3D.zoom_sensitivity_offset;
        this.zoom_sensitivity_factor = S_Scene3D.zoom_sensitivity_factor;
        this.show_flag = S_ShowFlagsJS_Enum[S_Scene3D.show_flag];
        this.start_colour = GetColorBytes(S_Scene3D.start_colour);
        this.end_colour = GetColorBytes(S_Scene3D.end_colour);
        this.highlight_distance_from = S_Scene3D.highlight_distance_from;
        this.highlight_distance_to = S_Scene3D.highlight_distance_to;
        this.suppress_distance_from = S_Scene3D.suppress_distance_from;
        this.suppress_distance_to = S_Scene3D.suppress_distance_to;
        this.view_control_mode = S_Scene3D.view_control_mode;
        this.view_control_mode = S_ViewControlModeJS_Enum[S_Scene3D.view_control_mode];
        this.camera_distance_for_navigation = S_Scene3D.camera_distance_for_navigation;
        this.scene_transition_off_for = S_Scene3D.scene_transition_off_for;
        this.type = S_SceneTypeJS_Enum[S_Scene3D.type];
        this.compass_step = S_Scene3D.compass_step;
        this.startup_delay = S_Scene3D.startup_delay;
        this.startup_orientation = S_Scene3D.startup_orientation == null ? null : new S_Orientation3DJS(S_Scene3D.startup_orientation);
        this.center_view_offset = S_Scene3D.center_view_offset;
        this.background = GetColorBytes(S_Scene3D.background);
    }
}

function S_Scene3DJS(S_Scene3D) {
    this.init(S_Scene3D)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
ListCMSEntity3DJS.prototype.init = function (ListCMSEntity3D) {

    this.items = [];
    if (typeof ListCMSEntity3D != "undefined")
        for (var i in ListCMSEntity3D.items)
            this.items.push(new S_Entity3DJS(ListCMSEntity3D.items[i]));
}

function ListCMSEntity3DJS(ListCMSEntity3D) {
    this.init(ListCMSEntity3D);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSEntity3DJS.prototype.init = function (S_CMSEntity3D) {

    if (S_CMSEntity3D == null) {
        this.modifier_index = -1;
        this.visualisation_order = 0;
        this.type = "";
        this.child = null;
    }
    else {
        this.modifier_index = S_CMSEntity3D.modifier_index;
        this.visualisation_order = S_CMSEntity3D.visualisation_order;
        this.child = null;
        if (S_CMSEntity3D.S_CMSBillboard != null) {
            this.child = new S_CMSBillboardJS(S_CMSEntity3D.S_CMSBillboard);
            this.type = "S_CMSBillboard";
        }
        else if (S_CMSEntity3D.S_CMSNavigationVertex3D != null) {
            this.child = new S_CMSNavigationVertex3DJS(S_CMSEntity3D.S_CMSNavigationVertex3D);
            this.type = "S_CMSNavigationVertex3D";
        }
        else if (S_CMSEntity3D.S_CMSNavigationEdge3D != null) {
            this.child = new S_CMSNavigationEdge3DJS(S_CMSEntity3D.S_CMSNavigationEdge3D);
            this.type = "S_CMSNavigationEdge3D";
        }
        else if (S_CMSEntity3D.S_CMSModel3D != null) {
            this.child = new S_CMSModel3DJS(S_CMSEntity3D.S_CMSModel3D);
            this.type = "S_CMSModel3D";
        }
    }
}

function S_CMSEntity3DJS(S_CMSEntity3D) {
    this.init(S_CMSEntity3D)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
var S_EventTypeJS_Enum;
(function (S_EventTypeJS_Enum) {
    S_EventTypeJS_Enum[S_EventTypeJS_Enum["S_EventType_action2D"] = 0] = "S_EventType_action2D";
    S_EventTypeJS_Enum[S_EventTypeJS_Enum["S_EventType_scenechange"] = 1] = "S_EventType_scenechange";
    S_EventTypeJS_Enum[S_EventTypeJS_Enum["S_EventType_novisualeffect"] = 2] = "S_EventType_novisualeffect";
})(S_EventTypeJS_Enum || (S_EventTypeJS_Enum = {}));
S_EventTypeJS_Array = [S_EventTypeJS_Enum[0], S_EventTypeJS_Enum[1], S_EventTypeJS_Enum[2]];
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_Event3DJS.prototype.init = function (S_Event3D) {

    if (S_Event3D == null) {
        this.type = S_EventTypeJS_Enum.S_EventType_action2D;
        this.scene_index = -1;
    }
    else {
        this.type = S_EventTypeJS_Enum[S_Event3D.type];
        this.scene_index = S_Event3D.scene_index;
    }
}

function S_Event3DJS(S_Event3D) {
    this.init(S_Event3D)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSEventScenes3DJS.prototype.init = function (S_CMSEventScenes3D) {

    this.eventscenes = [];
    if (typeof S_CMSEventScenes3D != "undefined")
        for (var i in S_CMSEventScenes3D.eventscenes)
            this.eventscenes.push(new S_EventSceneJS(S_CMSEventScenes3D.eventscenes[i]));
}

function S_CMSEventScenes3DJS(S_CMSEventScenes3D) {
    this.init(S_CMSEventScenes3D);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
/*ListEventSceneJS.prototype.init = function (ListEventScene) {

    this.items = [];
    if (typeof ListEventScene != "undefined")
        for (var i in ListEventScene.items)
            this.items.push(new S_EventSceneJS(ListEventScene.items[i]));
}

function ListEventSceneJS(ListEventScene) {
    this.init(ListEventScene);
} */
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_EventSceneJS.prototype.init = function (S_EventScene) {

    if (S_EventScene == null) {
        this.type = S_EventTypeJS_Enum.S_EventType_action2D;
        this.index = -1;
        this.name = "";
        this.scene = null;
        this.billboardModifierIndexes = [];
    }
    else {
        this.type = S_EventTypeJS_Enum[S_EventScene.type];
        this.index = S_EventScene.index;
        this.name = S_EventScene.name;
        this.scene = S_EventScene.scene == null ? null : new S_Scene3DJS(S_EventScene.scene);
        this.billboardModifierIndexes = S_EventScene.billboardModifierIndexes;
    }
}

function S_EventSceneJS(S_EventScene) {
    this.init(S_EventScene)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSEntityGroup3DJS.prototype.init = function (S_CMSEntityGroup3D) {

    if (S_CMSEntityGroup3D == null) {
        this.modifier_index = -1;
        this.visualisation_order = 0;
        this.groupindex = -1;
        this.sizeStatistics = null;
        this.viz = [];
        this.entitites = [];
    }
    else {
        this.modifier_index = S_CMSEntityGroup3D.modifier_index;
        this.visualisation_order = S_CMSEntityGroup3D.visualisation_order;
        this.groupindex = S_CMSEntityGroup3D.groupindex;
        this.sizeStatistics = S_CMSEntityGroup3D.sizeStatistics == null ? null : new S_SizeStatisticsJS(S_CMSEntityGroup3D.sizeStatistics);
        this.viz = S_CMSEntityGroup3D.viz;
        this.entitites = [];
        if (typeof S_CMSEntityGroup3D.entitites != "undefined")
            for (var i in S_CMSEntityGroup3D.entitites)
                this.entitites.push(new S_CMSEntity3DJS(S_CMSEntityGroup3D.entitites[i]));

    }
}

function S_CMSEntityGroup3DJS(S_CMSEntityGroup3D) {
    this.init(S_CMSEntityGroup3D)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
/*S_Object3DJS.prototype.init = function (S_Object3D) {

    if (S_Object3D == null) {
        this.distance_from = 1;
        this.distance_to = 10000;
        this.distance_fadeout = 12000;
        this.event3D = null;
        this.child = null;
    }
    else {
        this.distance_from = S_Object3D.distance_from;
        this.distance_to = S_Object3D.distance_to;
        this.distance_fadeout = S_Object3D.distance_fadeout;
        this.event3D = S_Object3D.event3D == null ? null : new S_Event3DJS(S_Object3D.event3D);
        this.child = null;
        if (S_Object3D.S_Polygon3D != null)
            this.child = new S_Polygon3DJS(S_Object3D.S_Polygon3D);
        else if (S_Object3D.S_Billboard != null)
            this.child = new S_BillboardJS(S_Object3D.S_Billboard);
        else if (S_Object3D.S_Surface3D != null)
            this.child = new S_Surface3DJS(S_Object3D.S_Surface3D);
    }
}

function S_Object3DJS(S_Object3D) {
    this.init(S_Object3D)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_Polygon3DJS.prototype.init = function (S_Polygon3D) {

    if (S_Polygon3D == null) {
        this.points = [];
        this.show_top = true;
        this.show_sides = true;
        this.show_bottom = false;
        this.top = 2;
        this.bottom = 0;
        this.foreground = [0,150,150,150];
        this.background = [0,150,150,150];
        this.child = null;
    }
    else {
        this.points = [];
        if (typeof S_Polygon3D.points != "undefined")
            for (var i in S_Polygon3D.points)
                this.points.push(new S_EOPointJS(S_Polygon3D.points[i]));

        this.show_top = S_Polygon3D.show_top;
        this.show_sides = S_Polygon3D.show_sides;
        this.show_bottom = S_Polygon3D.show_bottom;
        this.top = S_Polygon3D.top;
        this.bottom = S_Polygon3D.bottom;
        this.foreground = GetColorBytes(S_Polygon3D.foreground);
        this.background = GetColorBytes(S_Polygon3D.background);

        this.child = null;
        if (S_Polygon3D.S_Line3D != null)
            this.child = new S_Line3DJS(S_Polygon3D.S_Line3D);
    }
}

function S_Polygon3DJS(S_Polygon3D) {
    this.init(S_Polygon3D)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_Line3DJS.prototype.init = function (S_Line3D) {

    if (S_Line3D == null) {
        this.top_2 = 2;
        this.bottom_2 = 0;
        this.p1 = null;
        this.p2 = null;
        this.width = 0.2;
    }
    else {
        this.top_2 = S_Line3D.top_2;
        this.bottom_2 = S_Line3D.bottom_2;
        this.p1 = S_Line3D.p1 == null ? null : new S_EOPointJS(S_Line3D.p1);
        this.p2 = S_Line3D.p2 == null ? null : new S_EOPointJS(S_Line3D.p2);
        this.width = S_Line3D.width;

    }
}

function S_Line3DJS(S_Line3D) {
    this.init(S_Line3D)
} */
//------------------------------------------------------------------------------------------------------------------------------------------------------------
var S_BillboardContentTypeJS_Enum;
(function (S_BillboardContentTypeJS_Enum) {
    S_BillboardContentTypeJS_Enum[S_BillboardContentTypeJS_Enum["S_BillboardContentType_text"] = 0] = "S_BillboardContentType_text";
    S_BillboardContentTypeJS_Enum[S_BillboardContentTypeJS_Enum["S_BillboardContentType_image"] = 1] = "S_BillboardContentType_image";
    S_BillboardContentTypeJS_Enum[S_BillboardContentTypeJS_Enum["S_BillboardContentType_xml"] = 2] = "S_BillboardContentType_xml";
})(S_BillboardContentTypeJS_Enum || (S_BillboardContentTypeJS_Enum = {}));
S_BillboardContentTypeJS_Array = [S_BillboardContentTypeJS_Enum[0], S_BillboardContentTypeJS_Enum[1], S_BillboardContentTypeJS_Enum[2]];
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSBillboardJS.prototype.init = function (S_CMSBillboard) {

    if (S_CMSBillboard == null) {
        this.distance_from = 1;
        this.distance_to = 10000;
        this.distance_fadeout = 10000;
        this.event3D = null;
        this.point = null;
        this.content = "";
        this.content_type = S_BillboardContentTypeJS_Enum.S_BillboardContentType_text;
        this.top = 5;
        this.bottom = 5;
        this.foreground = [0, 255, 255, 255];
        this.background = [0, 0, 0, 0];
        this.border = [0, 0, 0, 255];
        this.border_width = 2;
        this.minWidth = 100;
        this.minHeight = 30;
        this.horizontalMargin = 7;
        this.verticalMargin = 7;
        this.fontName = "";
        this.fontStyle = "";
        this.fontSize = 24;
        this.triggerMotion = true;
        this.alternative_elevation = 0;
        this.triangle_size = 10;
        this.pinShapeName = "";
    }
    else {
        this.distance_from = S_CMSBillboard.distance_from;
        this.distance_to = S_CMSBillboard.distance_to;
        this.distance_fadeout = S_CMSBillboard.distance_fadeout;
        this.event3D = S_CMSBillboard.event3D == null ? null : new S_Event3DJS(S_CMSBillboard.event3D);
        this.point = S_CMSBillboard.point == null ? null : new S_EOPointJS(S_CMSBillboard.point);
        this.content = S_CMSBillboard.content;
        this.content_type = S_BillboardContentTypeJS_Enum[S_CMSBillboard.content_type];
        this.top = S_CMSBillboard.top;
        this.bottom = S_CMSBillboard.bottom;
        this.foreground = GetColorBytes(S_CMSBillboard.foreground);
        this.background = GetColorBytes(S_CMSBillboard.background);
        this.border = GetColorBytes(S_CMSBillboard.border);
        this.border_width = S_CMSBillboard.border_width;
        this.minWidth = S_CMSBillboard.minWidth;
        this.minHeight = S_CMSBillboard.minHeight;
        this.horizontalMargin = S_CMSBillboard.horizontalMargin;
        this.verticalMargin = S_CMSBillboard.verticalMargin;
        this.fontName = S_CMSBillboard.fontName;
        this.fontStyle = S_CMSBillboard.fontStyle;
        this.fontSize = S_CMSBillboard.fontSize;
        this.triggerMotion = S_CMSBillboard.triggerMotion;
        this.alternative_elevation = S_CMSBillboard.alternative_elevation;
        this.triangle_size = S_CMSBillboard.triangle_size;
        this.pinShapeName = S_CMSBillboard.pinShapeName;
    }
}

function S_CMSBillboardJS(S_CMSBillboard) {
    this.init(S_CMSBillboard)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
DictStringDynamicObjVizJS.prototype.init = function (DictStringDynamicObjViz) {

    if (DictStringDynamicObjViz == null) {
        this.items = {};
    }
    else {
        this.items = {};

        for (var i in DictStringDynamicObjViz) {
            this.items[DictStringDynamicObjViz[i].Key] =
                DictStringDynamicObjViz[i].Value == null ? null : new S_DynamicObjVizJS(DictStringDynamicObjViz[i].Value);
        }
    }
}

function DictStringDynamicObjVizJS(DictStringDynamicObjViz) {
    this.init(DictStringDynamicObjViz)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
ListDynamicObjVizJS.prototype.init = function (ListDynamicObjViz) {

    this.items = [];
    if (typeof ListDynamicObjViz != "undefined")
        for (var i in ListDynamicObjViz.items)
            this.items.push(new S_DynamicObjVizJS(ListDynamicObjViz.items[i]));
}

function ListDynamicObjVizJS(ListDynamicObjViz) {
    this.init(ListDynamicObjViz);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_DynamicObjVizJS.prototype.init = function (S_DynamicObjViz) {

    if (S_DynamicObjViz == null) {
        this.object_o_viz_indicator = -1;
        this.full_viz_enabler = [];
        this.half_viz_enabler = [];
        this.object_o_viz_string = "";
        // this.child = null;
    }
    else {
        this.object_o_viz_indicator = S_DynamicObjViz.object_o_viz_indicator;
        this.full_viz_enabler = S_DynamicObjViz.full_viz_enabler;
        this.half_viz_enabler = S_DynamicObjViz.half_viz_enabler;
        this.object_o_viz_string = S_DynamicObjViz.object_o_viz_string;

        /*   this.child = null;
           if (S_DynamicObjViz.S_NavNodeViz != null)
               this.child = new S_NavNodeVizJS(S_DynamicObjViz.S_NavNodeViz);
           else if (S_DynamicObjViz.S_NavEdgeViz != null)
               this.child = new S_NavEdgeVizJS(S_DynamicObjViz.S_NavEdgeViz);
           else if (S_DynamicObjViz.S_DestAreaViz != null)
               this.child = new S_DestAreaVizJS(S_DynamicObjViz.S_DestAreaViz); */
    }
}

function S_DynamicObjVizJS(S_DynamicObjViz) {
    this.init(S_DynamicObjViz)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
DictIntCMSNavNodeVizJS.prototype.init = function (ListCMSNavNodeViz) {

    if (ListCMSNavNodeViz == null) {
        this.items = {};
    }
    else {
        this.items = {};

        for (var i in ListCMSNavNodeViz) {
            this.items[ListCMSNavNodeViz[i].node_db_id] =
                ListCMSNavNodeViz[i] == null ? null : new S_CMSNavNodeVizJS(ListCMSNavNodeViz[i]);
        }
    }
}

function DictIntCMSNavNodeVizJS(DictIntCMSNavNodeViz) {
    this.init(DictIntCMSNavNodeViz)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSNavNodeVizJS.prototype.init = function (S_CMSNavNodeViz) {

    if (S_CMSNavNodeViz == null) {
        this.object_o_viz_indicator = -1;
        this.full_viz_enabler = [];
        this.half_viz_enabler = [];
        this.object_o_viz_string = "";
        this.node_db_id = -1;
    }
    else {
        this.object_o_viz_indicator = S_CMSNavNodeViz.object_o_viz_indicator;
        this.full_viz_enabler = S_CMSNavNodeViz.full_viz_enabler;
        this.half_viz_enabler = S_CMSNavNodeViz.half_viz_enabler;
        this.object_o_viz_string = S_CMSNavNodeViz.object_o_viz_string;
        this.node_db_id = S_CMSNavNodeViz.node_db_id;
    }
}

function S_CMSNavNodeVizJS(S_CMSNavNodeViz) {
    this.init(S_CMSNavNodeViz)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
/*DictIntNavEdgeVizJS.prototype.init = function (DictIntNavEdgeViz) {

    if (DictIntNavEdgeViz == null) {
        this.items = {};
    }
    else {
        this.items = {};

        for (var i in DictIntNavEdgeViz.items) {
            this.items[DictIntNavEdgeViz.items[i].Key] =
                DictIntNavEdgeViz.items[i].Value == null ? null : new S_NavEdgeVizJS(DictIntNavEdgeViz.items[i].Value);
        }
    }
}

function DictIntNavEdgeVizJS(DictIntNavEdgeViz) {
    this.init(DictIntNavEdgeViz)
} */
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSNavEdgeVizJS.prototype.init = function (S_CMSNavEdgeViz) {

    if (S_CMSNavEdgeViz == null) {
        this.object_o_viz_indicator = -1;
        this.full_viz_enabler = [];
        this.half_viz_enabler = [];
        this.object_o_viz_string = "";
        this.node_db_id = -1;
        this.neighbour_db_id = -1;
    }
    else {
        this.object_o_viz_indicator = S_CMSNavEdgeViz.object_o_viz_indicator;
        this.full_viz_enabler = S_CMSNavEdgeViz.full_viz_enabler;
        this.half_viz_enabler = S_CMSNavEdgeViz.half_viz_enabler;
        this.object_o_viz_string = S_CMSNavEdgeViz.object_o_viz_string;
        this.node_db_id = S_CMSNavEdgeViz.node_db_id;
        this.neighbour_db_id = S_CMSNavEdgeViz.neighbour_db_id;
    }
}

function S_CMSNavEdgeVizJS(S_CMSNavEdgeViz) {
    this.init(S_CMSNavEdgeViz)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
DictIntCMSDestAreaVizJS.prototype.init = function (ListCMSDestAreaViz) {

    if (ListCMSDestAreaViz == null) {
        this.items = {};
    }
    else {
        this.items = {};

        for (var i in ListCMSDestAreaViz) {
            this.items[ListCMSDestAreaViz[i].dest_db_id] =
                ListCMSDestAreaViz[i] == null ? null : new S_CMSDestAreaVizJS(ListCMSDestAreaViz[i]);
        }
    }
}

function DictIntCMSDestAreaVizJS(ListCMSDestAreaViz) {
    this.init(ListCMSDestAreaViz)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSDestAreaVizJS.prototype.init = function (S_CMSDestAreaViz) {

    if (S_CMSDestAreaViz == null) {
        this.object_o_viz_indicator = -1;
        this.full_viz_enabler = [];
        this.half_viz_enabler = [];
        this.object_o_viz_string = "";
        this.node_db_ids = [];
        this.dest_db_id = -1;
    }
    else {
        this.object_o_viz_indicator = S_CMSDestAreaViz.object_o_viz_indicator;
        this.full_viz_enabler = S_CMSDestAreaViz.full_viz_enabler;
        this.half_viz_enabler = S_CMSDestAreaViz.half_viz_enabler;
        this.object_o_viz_string = S_CMSDestAreaViz.object_o_viz_string;
        this.node_db_ids = S_CMSDestAreaViz.node_db_ids;
        this.dest_db_id = S_CMSDestAreaViz.dest_db_id;
    }
}

function S_CMSDestAreaVizJS(S_CMSDestAreaViz) {
    this.init(S_CMSDestAreaViz)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
DictIntListDynamicObjVizJS.prototype.init = function (DictIntListDynamicObjViz_nest) {

    if (DictIntListDynamicObjViz_nest == null) {
        this.items = {};
    }
    else {
        this.items = {};

        for (var i in DictIntListDynamicObjViz_nest.keys)
            if (i < DictIntListDynamicObjViz_nest.values.items.length) {
                this.items[DictIntListDynamicObjViz_nest.keys[i]] =
                DictIntListDynamicObjViz_nest.values.items[i] == null ? null : new ListDynamicObjVizJS(DictIntListDynamicObjViz_nest.values.items[i]);
            }
    }
}

function DictIntListDynamicObjVizJS(DictIntListDynamicObjViz_nest) {
    this.init(DictIntListDynamicObjViz_nest)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
DictIntDictIntNavEdgeVizJS.prototype.init = function (ListCMSNavEdgeViz) {

    if (ListCMSNavEdgeViz == null) {
        this.items = {};
    }
    else {
        this.items = {};

        for (var i in ListCMSNavEdgeViz) {
            if (this.items[ListCMSNavEdgeViz[i].node_db_id] == null)
                this.items[ListCMSNavEdgeViz[i].node_db_id] = {};

            this.items[ListCMSNavEdgeViz[i].node_db_id][ListCMSNavEdgeViz[i].neighbour_db_id] = new S_CMSNavEdgeVizJS(ListCMSNavEdgeViz[i]);
        }
    }
}

function DictIntDictIntNavEdgeVizJS(DictIntDictIntNavEdgeViz) {
    this.init(DictIntDictIntNavEdgeViz)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
DictStringLayerAttsJS.prototype.init = function (DictStringLayerAtts) {

    if (DictStringLayerAtts == null) {
        this.items = {};
    }
    else {
        this.items = {};

        for (var i in DictStringLayerAtts) {
            this.items[DictStringLayerAtts[i].Key] =
                DictStringLayerAtts[i].Value == null ? null : new S_LayerAttributesJS(DictStringLayerAtts[i].Value);
        }
    }
}

function DictStringLayerAttsJS(DictStringLayerAtts) {
    this.init(DictStringLayerAtts)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_LayerAttributesJS.prototype.init = function (S_LayerAttributes) {

    if (S_LayerAttributes == null) {
        this.layer_fullname = "";
        this.layer_showflag = "default";
        this.gps_enabled = false;
    }
    else {
        this.layer_fullname = S_LayerAttributes.layer_fullname;
        this.layer_showflag = S_LayerAttributes.layer_showflag;
        this.gps_enabled = S_LayerAttributes.gps_enabled;
    }
}

function S_LayerAttributesJS(S_LayerAttributes) {
    this.init(S_LayerAttributes)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSLayerNavigation3DSettingJS.prototype.init = function (S_CMSLayerNavigation3DSetting) {

    if (S_CMSLayerNavigation3DSetting == null) {
        this.colour = [0, 255, 0, 0];
        this.diameter = 20;
        this.name = "";
        this.distance_from = 1;
        this.distance_to = 10000;
        this.distance_fadeout = 10000;
        this.nodeVizs = {};
        this.edgeVizs = {};
        this.navObjectVizs = {};
        this.layer_attributes = {};
        this.destination_modifierindex = {};
        this.destAreaVizs = {};

    }
    else {
        this.colour = GetColorBytes(S_CMSLayerNavigation3DSetting.colour);
        this.diameter = S_CMSLayerNavigation3DSetting.diameter;
        this.name = S_CMSLayerNavigation3DSetting.name;
        this.distance_from = S_CMSLayerNavigation3DSetting.distance_from;
        this.distance_to = S_CMSLayerNavigation3DSetting.distance_to;
        this.distance_fadeout = S_CMSLayerNavigation3DSetting.distance_fadeout;
        this.nodeVizs = S_CMSLayerNavigation3DSetting.nodeVizs == null ? {} : new DictIntCMSNavNodeVizJS(S_CMSLayerNavigation3DSetting.nodeVizs);
        this.edgeVizs = S_CMSLayerNavigation3DSetting.edgeVizs == null ? {} : new DictIntDictIntNavEdgeVizJS(S_CMSLayerNavigation3DSetting.edgeVizs);
        this.navObjectVizs = S_CMSLayerNavigation3DSetting.navObjectVizs == null ? {} : new DictStringDynamicObjVizJS(S_CMSLayerNavigation3DSetting.navObjectVizs);
        this.layer_attributes = S_CMSLayerNavigation3DSetting.layer_attributes == null ? {} : new DictStringLayerAttsJS(S_CMSLayerNavigation3DSetting.layer_attributes);
        this.destination_modifierindex = S_CMSLayerNavigation3DSetting.destination_modifierindex == null ? {} : new DictIntListDynamicObjVizJS(S_CMSLayerNavigation3DSetting.destination_modifierindex);
        this.destAreaVizs = S_CMSLayerNavigation3DSetting.destAreaVizs == null ? {} : new DictIntCMSDestAreaVizJS(S_CMSLayerNavigation3DSetting.destAreaVizs);
    }
}

function S_CMSLayerNavigation3DSettingJS(S_CMSLayerNavigation3DSetting) {
    this.init(S_CMSLayerNavigation3DSetting)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSNavigationVertex3DJS.prototype.init = function (S_CMSNavigationVertex3D) {

    if (S_CMSNavigationVertex3D == null) {
        this.index = -1;
        this.point = null;
    }
    else {
        this.index = S_CMSNavigationVertex3D.index;
        this.point = new S_EOPointJS(S_CMSNavigationVertex3D.point);
    }
}

function S_CMSNavigationVertex3DJS(S_CMSNavigationVertex3D) {
    this.init(S_CMSNavigationVertex3D)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSNavigationEdge3DJS.prototype.init = function (S_CMSNavigationEdge3D) {

    if (S_CMSNavigationEdge3D == null) {
        this.index_1 = -1;
        this.index_2 = -1;
    }
    else {
        this.index_1 = S_CMSNavigationEdge3D.index_1;
        this.index_2 = S_CMSNavigationEdge3D.index_2;
    }
}

function S_CMSNavigationEdge3DJS(S_CMSNavigationEdge3D) {
    this.init(S_CMSNavigationEdge3D)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
/*ListSurface3DPointJS.prototype.init = function (ListSurface3DPoint) {

    this.items = [];
    if (typeof ListSurface3DPoint != "undefined")
        for (var i in ListSurface3DPoint.items)
            this.items.push(new S_Surface3DPointJS(ListSurface3DPoint.items[i]));
}

function ListSurface3DPointJS(ListSurface3DPoint) {
    this.init(ListSurface3DPoint);
}*/
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSSurface3DPointJS.prototype.init = function (S_CMSSurface3DPoint) {

    if (S_CMSSurface3DPoint == null) {
        this.point = null;
        this.colour = [0, 150, 150, 150];
        this.u = -1;
        this.v = -1;
        this.normal = null;
        this.luminance_factor = 0;
    }
    else {
        this.point = new S_EOPointJS(S_CMSSurface3DPoint.point);
        this.colour = GetColorBytes(S_CMSSurface3DPoint.colour);
        this.u = S_CMSSurface3DPoint.u;
        this.v = S_CMSSurface3DPoint.v;
        this.normal = new S_EOPointJS(S_CMSSurface3DPoint.normal);
        this.luminance_factor = S_CMSSurface3DPoint.luminance_factor;
    }
}

function S_CMSSurface3DPointJS(S_CMSSurface3DPoint) {
    this.init(S_CMSSurface3DPoint)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
IntArrayJS.prototype.init = function (IntArray) {

    if (IntArray == null) {
        this.array = [];
    }
    else {
        this.array = IntArray.array;
    }
}

function IntArrayJS(IntArray) {
    this.init(IntArray);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
/*ListIntArrayJS.prototype.init = function (ListIntArray) {

    this.items = [];
    if (typeof ListIntArray != "undefined")
        for (var i in ListIntArray.items)
            this.items.push(new IntArrayJS(ListIntArray.items[i]));
}

function ListIntArrayJS(ListIntArray) {
    this.init(ListIntArray);
} */
//------------------------------------------------------------------------------------------------------------------------------------------------------------
/* ListSurface3DJS.prototype.init = function (ListSurface3D) {

    this.items = [];
    if (typeof ListSurface3D != "undefined")
        for (var i in ListSurface3D.items)
            this.items.push(new S_Surface3DJS(ListSurface3D.items[i]));
}

function ListSurface3DJS(ListSurface3D) {
    this.init(ListSurface3D);
} */
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSSurface3DJS.prototype.init = function (S_CMSSurface3D) {

    if (S_CMSSurface3D == null) {
        this.modifier_index = -1;
        this.visualisation_order = 0;
        this.distance_from = 1;
        this.distance_to = 10000;
        this.distance_fadeout = 10000;
        this.event3D = null;
        this.points = [];
        this.triangles = [];
        this.texture = "";
        this.texture_name = "";
    }
    else {
        this.modifier_index = S_CMSSurface3D.modifier_index;
        this.visualisation_order = S_CMSSurface3D.visualisation_order;
        this.distance_from = S_CMSSurface3D.distance_from;
        this.distance_to = S_CMSSurface3D.distance_to;
        this.distance_fadeout = S_CMSSurface3D.distance_fadeout;
        this.event3D = S_CMSSurface3D.event3D == null ? {} : new S_Event3DJS(S_CMSSurface3D.event3D);

        this.points = [];
        if (typeof S_CMSSurface3D.points != "undefined")
            for (var i in S_CMSSurface3D.points)
                this.points.push(new S_CMSSurface3DPointJS(S_CMSSurface3D.points[i]));

        this.triangles = [];
        if (typeof S_CMSSurface3D.triangles != "undefined")
            for (var i in S_CMSSurface3D.triangles)
                this.triangles.push(new IntArrayJS(S_CMSSurface3D.triangles[i]));

        this.texture = S_CMSSurface3D.texture;
        this.texture_name = S_CMSSurface3D.texture_name;
    }
}

function S_CMSSurface3DJS(S_CMSSurface3D) {
    this.init(S_CMSSurface3D);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSModel3DJS.prototype.init = function (S_CMSModel3D) {

    this.surfaces = [];
    if (typeof S_CMSModel3D != "undefined")
        for (var i in S_CMSModel3D.surfaces)
            this.surfaces.push(new S_CMSSurface3DJS(S_CMSModel3D.surfaces[i]));
}

function S_CMSModel3DJS(S_CMSModel3D) {
    this.init(S_CMSModel3D);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSTransformation3DJS.prototype.init = function (S_CMSTransformation3D) {

    if (S_CMSTransformation3D == null) {
        this.projectTransformation = null;
    }
    else {
        this.projectTransformation = S_CMSTransformation3D.projectTransformation == null ? null : new S_LayerTransformation3DJS(S_CMSTransformation3D.projectTransformation);
    }
}

function S_CMSTransformation3DJS(S_CMSTransformation3D) {
    this.init(S_CMSTransformation3D);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_LayerTransformation3DJS.prototype.init = function (S_LayerTransformation3D) {

    if (S_LayerTransformation3D == null) {
        this.layer_scene_index = -1;
        this.layerTransformation = null;
        this.childernLayers = [];
    }
    else {
        this.layer_scene_index = S_LayerTransformation3D.layer_scene_index;
        this.layerTransformation = S_LayerTransformation3D.layerTransformation == null ? null : new S_LayerTransformationJS(S_LayerTransformation3D.layerTransformation);
        this.childernLayers = [];
        if (typeof S_LayerTransformation3D.childernLayers != "undefined")
            for (var i in S_LayerTransformation3D.childernLayers)
                this.childernLayers.push(new S_LayerTransformation3DJS(S_LayerTransformation3D.childernLayers[i]));
    }
}

function S_LayerTransformation3DJS(S_LayerTransformation3D) {
    this.init(S_LayerTransformation3D);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSTextureRepository3DJS.prototype.init = function (S_CMSTextureRepository3D) {

    if (S_CMSTextureRepository3D == null) {
        this.textures = {};
    }
    else {
        this.textures = {};

        for (var i in S_CMSTextureRepository3D.textures) {
            this.textures[S_CMSTextureRepository3D.textures[i].Key] = S_CMSTextureRepository3D.textures[i].Value;
        }
    }
}
function S_CMSTextureRepository3DJS(S_CMSTextureRepository3D) {
    this.init(S_CMSTextureRepository3D)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSModelRepository3DJS.prototype.init = function (S_CMSModelRepository3D) {

    if (S_CMSModelRepository3D == null) {
        this.models = {};
    }
    else {
        this.models = {};

        for (var i in S_CMSModelRepository3D.models) {
            this.models[S_CMSModelRepository3D.models[i].Key] =
                S_CMSModelRepository3D.models[i].Value == null ? null : new S_Model3DJS(S_CMSModelRepository3D.models[i].Value);
        }
    }
}

function S_CMSModelRepository3DJS(S_CMSModelRepository3D) {
    this.init(S_CMSModelRepository3D)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
