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
        for (var i in S_CMSPolyArea.images) this.images.push(new S_CMSEOImageJS( S_CMSPolyArea.images[i]));
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

