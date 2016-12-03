// this file should be autogenereted together with SType.proto files

//------------------------------------------------------------------------------------------------------------------------------------------------------------
// TradeFairs attributtes ###################################
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSImageJS.prototype.init = function (S_CMSImage) {

    if (S_CMSImage == null) {
        this.name = "";
        this.image_str = "";
        this.size = S_CMSImageSizeJS_Enum[2];
    }
    else {
        this.name = S_CMSImage.name;
        this.image_str = S_CMSImage.image_str;
        this.size = S_CMSImageSizeJS_Enum[S_CMSImage.size];
    }
}
S_CMSImageJS.prototype.Convert = function () {

    var result = new S_CMSImage();
    result.name = this.name;
    result.image_str = this.image_str;
    result.size = this.size;
    return result;
}
function S_CMSImageJS(S_CMSImage) {
    this.init(S_CMSImage)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------

S_CMSDataUpdateBroadcastJS.prototype.init = function (S_CMSDataUpdateBroadcast) {

    if (S_CMSDataUpdateBroadcast == null) {
        this.project_id = "";
        this.object_type = "";
        this.object_dbids = [];
    }
    else {
        this.project_id = S_CMSDataUpdateBroadcast.project_id;
        this.object_type = S_CMSDataUpdateBroadcast.object_type;
        this.object_dbids = [];
        for (var i in S_CMSDataUpdateBroadcast.object_dbids) {
            this.object_dbids.push(S_CMSDataUpdateBroadcast.object_dbids[i]);
        }
    }
}
S_CMSDataUpdateBroadcastJS.prototype.Convert = function () {

    var result = new S_CMSDataUpdateBroadcast();
    result.project_id = this.project_id;
    result.object_type = this.object_type;
    result.object_dbids = this.object_dbids;
    return result;
}
function S_CMSDataUpdateBroadcastJS(S_CMSDataUpdateBroadcast) {
    this.init(S_CMSDataUpdateBroadcast)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------

S_CMSListItemJS.prototype.init = function (S_CMSListItem) {

    if (S_CMSListItem == null) {
        this.id = "";
        this.original_type = "";
        this.title = "";
        this.line1 = "";
        this.line2 = "";
        this.line3 = "";
    }
    else {
        this.id = S_CMSListItem.id;
        this.original_type = S_CMSListItem.original_type;
        this.title = S_CMSListItem.title;
        this.line1 = S_CMSListItem.line1;
        this.line2 = S_CMSListItem.line2;
        this.line3 = S_CMSListItem.line3;
    }
}
S_CMSListItemJS.prototype.Convert = function () {

    var result = new S_CMSListItem();
    result.id = this.id;
    result.original_type = this.original_type;
    result.title = this.title;
    result.line1 = this.line1;
    result.line2 = this.line2;
    result.line3 = this.line3;
    return result;
}
function S_CMSListItemJS(S_CMSListItem) {
    this.init(S_CMSListItem)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------

var S_CMSImageSizeJS_Enum;
(function (S_CMSImageSizeJS_Enum) {
    S_CMSImageSizeJS_Enum[S_CMSImageSizeJS_Enum["S_CMSImageSize_tiny"] = 0] = "S_CMSImageSize_tiny";
    S_CMSImageSizeJS_Enum[S_CMSImageSizeJS_Enum["S_CMSImageSize_small"] = 1] = "S_CMSImageSize_small";
    S_CMSImageSizeJS_Enum[S_CMSImageSizeJS_Enum["S_CMSImageSize_full"] = 2] = "S_CMSImageSize_full";
})(S_CMSImageSizeJS_Enum || (S_CMSImageSizeJS_Enum = {}));
S_CMSImageSizeJS_Array = [S_CMSImageSizeJS_Enum[0], S_CMSImageSizeJS_Enum[1], S_CMSImageSizeJS_Enum[2]];
//------------------------------------------------------------------------------------------------------------------------------------------------------------

var S_CMSAdminTypeJS_Enum;
(function (S_CMSAdminTypeJS_Enum) {
    S_CMSAdminTypeJS_Enum[S_CMSAdminTypeJS_Enum["S_CMSAdminType_notuser"] = 0] = "S_CMSAdminType_notuser";
    S_CMSAdminTypeJS_Enum[S_CMSAdminTypeJS_Enum["S_CMSAdminType_profileuser"] = 1] = "S_CMSAdminType_profileuser";
    S_CMSAdminTypeJS_Enum[S_CMSAdminTypeJS_Enum["S_CMSAdminType_basic"] = 2] = "S_CMSAdminType_basic";
    S_CMSAdminTypeJS_Enum[S_CMSAdminTypeJS_Enum["S_CMSAdminType_projectadmin"] = 3] = "S_CMSAdminType_projectadmin";
    S_CMSAdminTypeJS_Enum[S_CMSAdminTypeJS_Enum["S_CMSAdminType_superuser"] = 4] = "S_CMSAdminType_superuser";    
})(S_CMSAdminTypeJS_Enum || (S_CMSAdminTypeJS_Enum = {}));
var S_CMSAdminTypeJS_Array = [S_CMSAdminTypeJS_Enum[0], S_CMSAdminTypeJS_Enum[1], S_CMSAdminTypeJS_Enum[2], S_CMSAdminTypeJS_Enum[3], S_CMSAdminTypeJS_Enum[4]];

function adminTypeIndex(type) {
    for (var key in S_CMSAdminTypeJS_Enum)
        if (S_CMSAdminTypeJS_Enum[key].indexOf(type) > -1)
            return key;
    return -1;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------

var S_CMSProfileUserStatusJS_Enum;
(function (S_CMSProfileUserStatusJS_Enum) {
    S_CMSProfileUserStatusJS_Enum[S_CMSProfileUserStatusJS_Enum["S_CMSProfileUserStatus_valid"] = 0] = "S_CMSProfileUserStatus_valid";
    S_CMSProfileUserStatusJS_Enum[S_CMSProfileUserStatusJS_Enum["S_CMSProfileUserStatus_deleted"] = 1] = "S_CMSProfileUserStatus_deleted";
    S_CMSProfileUserStatusJS_Enum[S_CMSProfileUserStatusJS_Enum["S_CMSProfileUserStatus_unknown"] = 2] = "S_CMSProfileUserStatus_unknown";
    S_CMSProfileUserStatusJS_Enum[S_CMSProfileUserStatusJS_Enum["S_CMSProfileUserStatus_inactive"] = 3] = "S_CMSProfileUserStatus_inactive";
})(S_CMSProfileUserStatusJS_Enum || (S_CMSProfileUserStatusJS_Enum = {}));
S_CMSProfileUserStatusJS_Array = [S_CMSProfileUserStatusJS_Enum[0], S_CMSProfileUserStatusJS_Enum[1], S_CMSProfileUserStatusJS_Enum[2], S_CMSProfileUserStatusJS_Enum[3]];

//------------------------------------------------------------------------------------------------------------------------------------------------------------

ListCMSAccountProjectJS.prototype.init = function (ListCMSAccountProject) {

    this.items = [];
    if (typeof ListCMSAccountProject != "undefined")
        for (var i in ListCMSAccountProject.items)
            this.items.push(new S_CMSAccountProjectJS(ListCMSAccountProject.items[i]));
}

function ListCMSAccountProjectJS(ListCMSAccountProject) {
    this.init(ListCMSAccountProject);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSAccountProjectJS.prototype.init = function (S_CMSAccountProject) {

    if (S_CMSAccountProject == null) {
        this.project_id = "";
        this.user_project_key = "";
        this.expire = 0;
        this.last_used = 0;
        this.status = S_CMSProfileUserStatusJS_Enum[2];
    }
    else {
        this.project_id = S_CMSAccountProject.project_id;
        this.user_project_key = S_CMSAccountProject.user_project_key;
        this.expire = S_CMSAccountProject.expire;
        this.last_used = S_CMSAccountProject.last_used;
        this.status = S_CMSProfileUserStatusJS_Enum[S_CMSAccountProject.status];
    }
}
S_CMSAccountProjectJS.prototype.Convert = function () {

    var result = new S_CMSAccountProject();
    result.project_id = this.project_id;
    result.user_project_key = this.user_project_key;
    result.expire = this.expire;
    result.last_used = this.last_used;
    result.status = this.status;
    return result;
}
function S_CMSAccountProjectJS(S_CMSAccountProject) {
    this.init(S_CMSAccountProject)
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
ListCMSAccountJS.prototype.init = function (ListCMSAccount) {

    this.items = [];

    if (typeof ListCMSAccount != "undefined")
        for (var i in ListCMSAccount.items)
            this.items.push(new S_CMSAccountJS(ListCMSAccount.items[i]));
}

function ListCMSAccountJS(ListCMSAccount) {
    this.init(ListCMSAccount);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSAccountJS.prototype.init = function (S_CMSAccount) {

    if (S_CMSAccount == null) {
        this.list_item = null;
        this.username = "";
        this.firstname = "";
        this.lastname = "";
        this.admintype = S_CMSAdminTypeJS_Enum[0];
        this.image_url = "";
        this.image_string = "";
        this.projects = [];
        this.status = S_CMSProfileUserStatusJS_Enum[2];
        this.last_login_project = "";
        this.account_id = -1;
        this.profileuser_id = -1;
        this.profile_id = -1;
    }
    else {
        this.list_item = S_CMSAccount.list_item == null ? null : new S_CMSListItemJS(S_CMSAccount.list_item);
        this.username = S_CMSAccount.username;
        this.firstname = S_CMSAccount.firstname;
        this.lastname = S_CMSAccount.lastname;
        this.admintype = S_CMSAdminTypeJS_Enum[S_CMSAccount.admintype];
        this.image_url = S_CMSAccount.image_url;
        this.image_string = S_CMSAccount.image_string;
        this.projects = [];
        for (var i in S_CMSAccount.projects) {
            if (S_CMSAccount.projects[i] != null)
                this.projects.push( new S_CMSAccountProjectJS(S_CMSAccount.projects[i]));
        }
        this.status = S_CMSProfileUserStatusJS_Enum[S_CMSAccount.status];
        this.last_login_project = S_CMSAccount.last_login_project;
        this.account_id = S_CMSAccount.account_id;
        this.profileuser_id = S_CMSAccount.profileuser_id;
        this.profile_id = S_CMSAccount.profile_id;
    }
}
S_CMSAccountJS.prototype.Convert = function () {

    var result = new S_CMSAccount();
    result.list_item = this.list_item.Convert();
    result.username = this.username;
    result.firstname = this.firstname;
    result.lastname = this.lastname;
    result.admintype = this.admintype;
    result.image_url = this.image_url;
    result.image_string = this.image_string;
    result.projects = this.projects;
    result.isActive = this.isActive;
    result.profileuser_userkey = this.profileuser_userkey;
    result.profile_id = this.profile_id;
    return result;
}
S_CMSAccountJS.prototype.UpdateMenuItem = function () {
    this.list_item.title = this.username;
    this.list_item.line1 = this.firstname;
    this.list_item.line2 = this.lastname;
    this.list_item.line3 = this.admintype;
}
function S_CMSAccountJS(S_CMSAccount) {
    this.init(S_CMSAccount)
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
ListCMSStoreJS.prototype.init = function (ListCMSStore) {

    this.items = [];

    if (typeof ListCMSStore != "undefined") {
        for (var i in ListCMSStore.items) {
            this.items.push(new S_CMSStoreJS(ListCMSStore.items[i]));
        }
    }
}

function ListCMSStoreJS(ListCMSStore) {
    this.init(ListCMSStore);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSStoreJS.prototype.init = function (S_CMSStore) {

    if (S_CMSStore == null) {
        this.list_item = null;
        this.id = -1;
        this.name1 = "";
        this.name2 = "";
        this.addressline1 = "";
        this.addressline2 = "";
        this.description = "";
        this.image_url = "";
        this.image_string = "";
        this.booths = [];
        this.linked_profile_ids = [];
        this.billboard_text = "";
        this.isActive = false;
        this.isVisible = false;     //Added to enable Conrad.d to use attr when loading images - Possibly will be removed
        this.service_level = 0;
        this.bg_image_url = "";
        this.bg_image_string = "";
        this.addressline3 = "";
        this.addressline4 = "";
        this.zipcode = "";
        this.country = "";
        this.phone = "";
        this.mobile = "";
        this.email = "";
        this.fax = "";
        this.web = "";
        this.product_categories = [];
        this.sectors = [];
    }
    else {
        this.list_item = S_CMSStore.list_item == null ? null : new S_CMSListItemJS(S_CMSStore.list_item);
        this.id = S_CMSStore.id;
        this.name1 = S_CMSStore.name1;
        this.name2 = S_CMSStore.name2;
        this.addressline1 = S_CMSStore.addressline1;
        this.addressline2 = S_CMSStore.addressline2;
        this.description = S_CMSStore.description;
        this.image_url = S_CMSStore.image_url;
        this.image_string = S_CMSStore.image_string;
        this.booths = [];
        for (var i in S_CMSStore.booths) {
            if (S_CMSStore.booths[i] != null)
                this.booths.push(new S_CMSBoothJS(S_CMSStore.booths[i]));
        }
        this.linked_profile_ids = [];
        for (var i in S_CMSStore.linked_profile_ids) {
            this.linked_profile_ids.push(S_CMSStore.linked_profile_ids[i]);
        }
        this.billboard_text = S_CMSStore.billboard_text;
        this.isActive = S_CMSStore.isActive;
        this.service_level = S_CMSStore.service_level;
        this.bg_image_url = S_CMSStore.bg_image_url;
        this.bg_image_string = S_CMSStore.bg_image_string;
        this.addressline3 = S_CMSStore.addressline3;
        this.addressline4 = S_CMSStore.addressline4;
        this.zipcode = S_CMSStore.zipcode;
        this.country = S_CMSStore.country;
        this.phone = S_CMSStore.phone;
        this.mobile = S_CMSStore.mobile;
        this.email = S_CMSStore.email;
        this.fax = S_CMSStore.fax;
        this.web = S_CMSStore.web;
        this.product_categories = [];
        for (var i in S_CMSStore.product_categories) {
            if (S_CMSStore.product_categories[i] != null)
                this.product_categories.push(new S_CMSProfileCategoryJS(S_CMSStore.product_categories[i]));
        }
        this.sectors = [];
        for (var i in S_CMSStore.sectors) {
            if (S_CMSStore.sectors[i] != null)
                this.sectors.push(new S_CMSProfileCategoryJS(S_CMSStore.sectors[i]));
        }
    }
}
S_CMSStoreJS.prototype.Convert = function () {

    var result = new S_CMSStore();
    result.list_item = this.list_item.Convert();
    result.id = this.id;
    result.name1 = this.name1;
    result.name2 = this.name2;
    result.addressline1 = this.addressline1;
    result.addressline2 = this.addressline2;
    result.description = this.description;
    result.image_url = this.image_url;
    result.image_string = this.image_string;
    result.booths = [];
    for (var i in this.booths) {
        if (typeof this.booths[i].Convert() != "undefined") 
            result.booths.push(this.booths[i].Convert());
    }
    result.linked_profile_ids = this.linked_profile_ids;
    result.billboard_text = this.billboard_text;
    result.isActive = this.isActive;
    result.service_level = this.service_level;
    result.bg_image_url = this.bg_image_url;
    result.bg_image_string = this.bg_image_string;
    result.addressline3 = this.addressline3;
    result.addressline4 = this.addressline4;
    result.zipcode = this.zipcode;
    result.country = this.country;
    result.phone = this.phone;
    result.mobile = this.mobile;
    result.email = this.email;
    result.fax = this.fax;
    result.web = this.web;
    result.product_categories = this.product_categories;
    result.sectors = this.sectors;
    return result;
}
S_CMSStoreJS.prototype.UpdateMenuItem = function () {
    this.list_item.title = this.firstname;
    this.list_item.line1 = this.lastname;
    this.list_item.line2 = this.addressline1;
    this.list_item.line3 = this.addressline2;
}
function S_CMSStoreJS(S_CMSStore) {
    this.init(S_CMSStore)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
ListCMSBoothJS.prototype.init = function (ListCMSBooth) {

    this.items = [];

    if (typeof ListCMSBooth != "undefined") {
        for (var i in ListCMSBooth.items) {
            this.items.push(new S_CMSBoothJS(ListCMSBooth.items[i]));
        }
    }
}

function ListCMSBoothJS(ListCMSBooth) {
    this.init(ListCMSBooth);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSBoothJS.prototype.init = function (S_CMSBooth) {

    if (S_CMSBooth == null) {
        this.list_item = null;
        this.id = -1;
        this.booth = "";
        this.hall = "";
        this.destination_dbid = -1;
    }
    else {
        this.list_item = S_CMSBooth.list_item == null ? null : new S_CMSListItemJS(S_CMSBooth.list_item);
        this.id = S_CMSBooth.id;
        this.booth = S_CMSBooth.booth;
        this.hall = S_CMSBooth.hall;
        this.destination_dbid = S_CMSBooth.destination_dbid;
    }
}

S_CMSBoothJS.prototype.Convert = function () {

    var result = new S_CMSBooth();
    result.list_item = this.list_item.Convert();
    result.id = this.id;
    result.booth = this.booth;
    result.hall = this.hall;
    result.destination_dbid = this.destination_dbid;

    return result;
}
S_CMSBoothJS.prototype.UpdateMenuItem = function () {
    this.list_item.title = this.booth;
    this.list_item.line1 = this.hall;
    this.list_item.line2 = "";
    this.list_item.line3 = "";
}
function S_CMSBoothJS(S_CMSBooth) {
    this.init(S_CMSBooth)
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

var S_CMSProfileCategoryTypeJS_Enum;
(function (S_CMSProfileCategoryTypeJS_Enum) {
    S_CMSProfileCategoryTypeJS_Enum[S_CMSProfileCategoryTypeJS_Enum["S_CMSProfileCategoryType_unknown"] = 0] = "S_CMSProfileCategoryType_unknown";
    S_CMSProfileCategoryTypeJS_Enum[S_CMSProfileCategoryTypeJS_Enum["S_CMSProfileCategoryType_product"] = 1] = "S_CMSProfileCategoryType_product";
    S_CMSProfileCategoryTypeJS_Enum[S_CMSProfileCategoryTypeJS_Enum["S_CMSProfileCategoryType_profile"] = 2] = "S_CMSProfileCategoryType_profile";
    S_CMSProfileCategoryTypeJS_Enum[S_CMSProfileCategoryTypeJS_Enum["S_CMSProfileCategoryType_sector"] = 3] = "S_CMSProfileCategoryType_sector";
})(S_CMSProfileCategoryTypeJS_Enum || (S_CMSProfileCategoryTypeJS_Enum = {}));
S_CMSProfileCategoryTypeJS_Array = [S_CMSProfileCategoryTypeJS_Enum[0], S_CMSProfileCategoryTypeJS_Enum[1], S_CMSProfileCategoryTypeJS_Enum[2], S_CMSProfileCategoryTypeJS_Enum[3]];

//------------------------------------------------------------------------------------------------------------------------------------------------------------
ListCMSProfileCategoryJS.prototype.init = function (ListCMSProfileCategory) {

    this.items = [];

    if (typeof ListCMSProfileCategory != "undefined") {
        for (var i in ListCMSProfileCategory.items) {
            this.items.push(new S_CMSProfileCategoryJS(ListCMSProfileCategory.items[i]));
        }
    }
}
function ListCMSProfileCategoryJS(ListCMSProfileCategory) {
    this.init(ListCMSProfileCategory);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSProfileCategoryJS.prototype.init = function (S_CMSProfileCategory) {

    if (S_CMSProfileCategory == null) {
        this.list_item = null;
        this.id = -1;
        this.type = S_CMSProfileCategoryTypeJS_Enum.S_CMSProfileCategoryType_unknown;
        this.category = "";
        this.code = [];
    }
    else {
        this.list_item = S_CMSProfileCategory.list_item == null ? null : new S_CMSListItemJS(S_CMSProfileCategory.list_item);
        this.id = S_CMSProfileCategory.id;
        this.type = S_CMSProfileCategoryTypeJS_Enum[S_CMSProfileCategory.type];
        this.category = S_CMSProfileCategory.category;
        this.code = S_CMSProfileCategory.code;
    }
}

S_CMSProfileCategoryJS.prototype.Convert = function () {

    var result = new S_CMSProfileCategory();
    result.list_item = this.list_item.Convert();
    result.id = this.id;
    result.type = this.type;
    result.category = this.category;
    result.code = this.code;

    return result;
}
S_CMSProfileCategoryJS.prototype.UpdateMenuItem = function () {
    this.list_item.title = this.category;
    var catalogcode = "";
    for (var i = 0; i < this.code.length; i += 1) {
        if (i > 0) catalogcode += '.';
        catalogcode += this.code[i].toString();
    }
    this.list_item.line1 = catalogcode;
    this.list_item.line2 = "";
    this.list_item.line3 = "";
}
function S_CMSProfileCategoryJS(S_CMSProfileCategory) {
    this.init(S_CMSProfileCategory)
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
ListCMSBeaconJS.prototype.init = function (ListCMSBeacon) {

    this.items = [];

    if (typeof ListCMSBeacon != "undefined") {
        for (var i in ListCMSBeacon.items) {
            this.items.push(new S_CMSBeaconJS(ListCMSBeacon.items[i]));
        }
    }
}

function ListCMSBeaconJS(ListCMSBeacon) {
    this.init(ListCMSBeacon);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSBeaconJS.prototype.init = function (S_CMSBeacon) {

    if (S_CMSBeacon == null) {
        this.id = -1;
        this.list_item = null;
        this.name = "";
        this.store_id = -1;
        this.booth_id = -1;
        this.project_id = "";
        this.minor = 0;
        this.major = 0;
        this.subsystem_name = "";
        this.subsystem_uuid = "";
        this.isActive = false;
        this.description = "";
        this.display_duration = 3000;
        this.display_period = 600000;
    }
    else {
        this.id = S_CMSBeacon.id;
        this.list_item = S_CMSBeacon.list_item == null ? null : new S_CMSListItemJS(S_CMSBeacon.list_item);
        this.name = S_CMSBeacon.name;
        this.store_id = S_CMSBeacon.store_id;
        this.booth_id = S_CMSBeacon.booth_id;
        this.project_id = S_CMSBeacon.project_id;
        this.minor = S_CMSBeacon.minor;
        this.major = S_CMSBeacon.major;
        this.subsystem_name = S_CMSBeacon.subsystem_name;
        this.subsystem_uuid = S_CMSBeacon.subsystem_uuid;
        this.isActive = S_CMSBeacon.isActive;
        this.description = S_CMSBeacon.description;
        this.display_duration = S_CMSBeacon.display_duration;
        this.display_period = S_CMSBeacon.display_period;
    }
}

S_CMSBeaconJS.prototype.Convert = function () {

    var result = new S_CMSBeacon();
    result.id = this.id;
    result.list_item = this.list_item.Convert();
    result.name = this.name;
    result.store_id = this.store_id;
    result.booth_id = this.booth_id;
    result.project_id = this.project_id;
    result.minor = this.minor;
    result.major = this.major;
    result.subsystem_name = this.subsystem_name;
    result.subsystem_uuid = this.subsystem_uuid;
    result.isActive = this.isActive;
    result.description = this.description;
    result.display_duration = this.display_duration;
    result.display_period = this.display_period;

    return result;
}
S_CMSBeaconJS.prototype.UpdateMenuItem = function () {
    this.list_item.title = this.name;
    this.list_item.line1 = this.subsystem_name;
    this.list_item.line2 = this.minor.toString();
    this.list_item.line3 = this.major.toString();
}
function S_CMSBeaconJS(S_CMSBeacon) {
    this.init(S_CMSBeacon)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
ListCMSNewsFeedJS.prototype.init = function (ListCMSNewsFeed) {

    this.items = [];

    if (typeof ListCMSNewsFeed != "undefined") {
        for (var i in ListCMSNewsFeed.items) {
            this.items.push(new S_CMSNewsFeedJS(ListCMSNewsFeed.items[i]));
        }
    }
}

function ListCMSNewsFeedJS(ListCMSNewsFeed) {
    this.init(ListCMSNewsFeed);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSNewsFeedJS.prototype.init = function (S_CMSNewsFeed) {

    if (S_CMSNewsFeed == null) {
        this.id = -1;
        this.list_item = null;
        this.title = "";
        this.timestamp = 0;
        this.valid_from = 0;
        this.valid_to = 0;
        this.store_id = -1;
        this.booth_id = -1;
        this.project_id = "";
        this.action_type = S_CMSNewsFeedActionTypeJS_Enum.S_CMSNewsFeedActionType_message;
        this.message = "";
        this.url_link = "";
        this.image_url = "";
        this.image_string = "";
        this.from_user = "";
        this.display_duration = 3000;
        this.display_period = 600000;
        this.recurrence_period = S_CMSRecurrencePeriodJS_Enum.S_CMSRecurrencePeriod_none;
        this.recurrence_from = 0;
        this.recurrence_to = 0;
    }
    else {
        this.id = S_CMSNewsFeed.id;
        this.list_item = S_CMSNewsFeed.list_item == null ? null : new S_CMSListItemJS(S_CMSNewsFeed.list_item);
        this.title = S_CMSNewsFeed.title;
        this.timestamp = S_CMSNewsFeed.timestamp;
        this.valid_from = S_CMSNewsFeed.valid_from;
        this.valid_to = S_CMSNewsFeed.valid_to;
        this.store_id = S_CMSNewsFeed.store_id;
        this.booth_id = S_CMSNewsFeed.booth_id;
        this.project_id = S_CMSNewsFeed.project_id;
        this.action_type = S_CMSNewsFeedActionTypeJS_Enum[S_CMSNewsFeed.action_type];
        this.message = S_CMSNewsFeed.message;
        this.url_link = S_CMSNewsFeed.url_link;
        this.image_url = S_CMSNewsFeed.image_url;
        this.image_string = S_CMSNewsFeed.image_string;
        this.from_user = S_CMSNewsFeed.from_user;
        this.display_duration = S_CMSNewsFeed.display_duration;
        this.display_period = S_CMSNewsFeed.display_period;
        this.recurrence_period = S_CMSRecurrencePeriodJS_Enum[S_CMSNewsFeed.recurrence_period];
        this.recurrence_from = S_CMSNewsFeed.recurrence_from;
        this.recurrence_to = S_CMSNewsFeed.recurrence_to;
    }
}

S_CMSNewsFeedJS.prototype.Convert = function () {

    var result = new S_CMSNewsFeed();
    result.id = this.id;
    result.list_item = this.list_item.Convert();
    result.title = this.title;
    result.timestamp = this.timestamp;
    result.valid_from = this.valid_from;
    result.valid_to = this.valid_to;
    result.store_id = this.store_id;
    result.booth_id = this.booth_id;
    result.project_id = this.project_id;
    result.action_type = this.action_type;
    result.message = this.message;
    result.url_link = this.url_link;
    result.image_url = this.image_url;
    result.image_string = this.image_string;
    result.from_user = this.from_user;
    result.display_duration = this.display_duration;
    result.display_period = this.display_period;
    result.recurrence_period = this.recurrence_period;
    result.recurrence_from = this.recurrence_from;
    result.recurrence_to = this.recurrence_to;

    return result;
}
S_CMSNewsFeedJS.prototype.UpdateMenuItem = function () {
    this.list_item.title = this.title;
    this.list_item.line1 = this.message;
    this.list_item.line2 = this.valid_from.toString();
    this.list_item.line3 = this.valid_to.toString();
}
function S_CMSNewsFeedJS(S_CMSNewsFeed) {
    this.init(S_CMSNewsFeed)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------

var S_CMSNewsFeedActionTypeJS_Enum;
(function (S_CMSNewsFeedActionTypeJS_Enum) {
    S_CMSNewsFeedActionTypeJS_Enum[S_CMSNewsFeedActionTypeJS_Enum["S_CMSNewsFeedActionType_message"] = 0] = "S_CMSNewsFeedActionType_message";
    S_CMSNewsFeedActionTypeJS_Enum[S_CMSNewsFeedActionTypeJS_Enum["S_CMSNewsFeedActionType_url_link"] = 1] = "S_CMSNewsFeedActionType_url_link";
    S_CMSNewsFeedActionTypeJS_Enum[S_CMSNewsFeedActionTypeJS_Enum["S_CMSNewsFeedActionType_custom_1"] = 2] = "S_CMSNewsFeedActionType_custom_1";
})(S_CMSNewsFeedActionTypeJS_Enum || (S_CMSNewsFeedActionTypeJS_Enum = {}));
S_CMSNewsFeedActionTypeJS_Array = [S_CMSNewsFeedActionTypeJS_Enum[0], S_CMSNewsFeedActionTypeJS_Enum[1], S_CMSNewsFeedActionTypeJS_Enum[2]];
//------------------------------------------------------------------------------------------------------------------------------------------------------------

var S_CMSRecurrencePeriodJS_Enum;
(function (S_CMSRecurrencePeriodJS_Enum) {
    S_CMSRecurrencePeriodJS_Enum[S_CMSRecurrencePeriodJS_Enum["S_CMSRecurrencePeriod_none"] = 0] = "S_CMSRecurrencePeriod_none";
    S_CMSRecurrencePeriodJS_Enum[S_CMSRecurrencePeriodJS_Enum["S_CMSRecurrencePeriod_hourly"] = 1] = "S_CMSRecurrencePeriod_hourly";
    S_CMSRecurrencePeriodJS_Enum[S_CMSRecurrencePeriodJS_Enum["S_CMSRecurrencePeriod_daily"] = 2] = "S_CMSRecurrencePeriod_daily";
    S_CMSRecurrencePeriodJS_Enum[S_CMSRecurrencePeriodJS_Enum["S_CMSRecurrencePeriod_weekly"] = 3] = "S_CMSRecurrencePeriod_weekly";
    S_CMSRecurrencePeriodJS_Enum[S_CMSRecurrencePeriodJS_Enum["S_CMSRecurrencePeriod_monthly"] = 4] = "S_CMSRecurrencePeriod_monthly";
    S_CMSRecurrencePeriodJS_Enum[S_CMSRecurrencePeriodJS_Enum["S_CMSRecurrencePeriod_yearly"] = 5] = "S_CMSRecurrencePeriod_yearly";
})(S_CMSRecurrencePeriodJS_Enum || (S_CMSRecurrencePeriodJS_Enum = {}));
S_CMSRecurrencePeriodJS_Array = [S_CMSRecurrencePeriodJS_Enum[0], S_CMSRecurrencePeriodJS_Enum[1], S_CMSRecurrencePeriodJS_Enum[2], S_CMSRecurrencePeriodJS_Enum[3], S_CMSRecurrencePeriodJS_Enum[4], S_CMSRecurrencePeriodJS_Enum[5]];
//------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSProjectSettingJS.prototype.init = function (S_CMSProjectSetting) {

    if (S_CMSProjectSetting == null) {
        this.id = -1;
        this.title = "";
        this.subtitle = "";
        this.description = "";
        this.image_url = "";
        this.image_string = "";
        this.color1 = [0xFF, 0xFF, 0xFF, 0xFF];
        this.color2 = [0xBB, 0xBB, 0xBB, 0xFF];
        this.color3 = [0x88, 0x88, 0x88, 0xFF];
        this.color4 = [0x00, 0x00, 0x00, 0xFF];
    }
    else {
        this.id = S_CMSProjectSetting.id;
        this.title = S_CMSProjectSetting.title;
        this.subtitle = S_CMSProjectSetting.subtitle;
        this.description = S_CMSProjectSetting.description;
        this.image_url = S_CMSProjectSetting.image_url;
        this.image_string = S_CMSProjectSetting.image_string;
        this.color1 = GetColorBytes(S_CMSProjectSetting.color1);
        this.color2 = GetColorBytes(S_CMSProjectSetting.color2);
        this.color3 = GetColorBytes(S_CMSProjectSetting.color3);
        this.color4 = GetColorBytes(S_CMSProjectSetting.color4);
      /*  if (typeof S_CMSProjectSetting.color1 == 'object')
            if (S_CMSProjectSetting.color1.length > 0) {
                var element0 = S_CMSProjectSetting.color1[0];
                this.color1 = [0xFF, 0xFF, 0xFF, 0xFF];
                for (var i = element0.offset; i < element0.limit; i++) {
                    this.color1[i - element0.offset] = element0.view[i];
                }
            } */
    }
}

function S_CMSProjectSettingJS(S_CMSProjectSetting) {
    this.init(S_CMSProjectSetting)
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

var S_CMSMenuItemStatusJS_Enum;
(function (S_CMSMenuItemStatusJS_Enum) {
    S_CMSMenuItemStatusJS_Enum[S_CMSMenuItemStatusJS_Enum["S_CMSMenuItemStatus_active"] = 0] = "S_CMSMenuItemStatus_active";
    S_CMSMenuItemStatusJS_Enum[S_CMSMenuItemStatusJS_Enum["S_CMSMenuItemStatus_inactive"] = 1] = "S_CMSMenuItemStatus_inactive";
})(S_CMSMenuItemStatusJS_Enum || (S_CMSMenuItemStatusJS_Enum = {}));
S_CMSMenuItemStatusJS_Array = [S_CMSMenuItemStatusJS_Enum[0], S_CMSMenuItemStatusJS_Enum[1]];
//------------------------------------------------------------------------------------------------------------------------------------------------------------

var S_CMSMenuItemTypeJS_Enum;
(function (S_CMSMenuItemTypeJS_Enum) {
    S_CMSMenuItemTypeJS_Enum[S_CMSMenuItemTypeJS_Enum["S_CMSMenuItemType_text"] = 0] = "S_CMSMenuItemType_text";
    S_CMSMenuItemTypeJS_Enum[S_CMSMenuItemTypeJS_Enum["S_CMSMenuItemType_html"] = 1] = "S_CMSMenuItemType_html";
    S_CMSMenuItemTypeJS_Enum[S_CMSMenuItemTypeJS_Enum["S_CMSMenuItemType_htmlfile"] = 2] = "S_CMSMenuItemType_htmlfile";
    S_CMSMenuItemTypeJS_Enum[S_CMSMenuItemTypeJS_Enum["S_CMSMenuItemType_urllink"] = 3] = "S_CMSMenuItemType_urllink";
    S_CMSMenuItemTypeJS_Enum[S_CMSMenuItemTypeJS_Enum["S_CMSMenuItemType_email"] = 4] = "S_CMSMenuItemType_email";
    S_CMSMenuItemTypeJS_Enum[S_CMSMenuItemTypeJS_Enum["S_CMSMenuItemType_zippedhtmldirectory"] = 5] = "S_CMSMenuItemType_zippedhtmldirectory";
    S_CMSMenuItemTypeJS_Enum[S_CMSMenuItemTypeJS_Enum["S_CMSMenuItemType_image"] = 6] = "S_CMSMenuItemType_image";
    S_CMSMenuItemTypeJS_Enum[S_CMSMenuItemTypeJS_Enum["S_CMSMenuItemType_folder"] = 7] = "S_CMSMenuItemType_folder";
})(S_CMSMenuItemTypeJS_Enum || (S_CMSMenuItemTypeJS_Enum = {}));
S_CMSMenuItemTypeJS_EnumJS_Array = [S_CMSMenuItemTypeJS_Enum[0], S_CMSMenuItemTypeJS_Enum[1], S_CMSMenuItemTypeJS_Enum[2], S_CMSMenuItemTypeJS_Enum[3],
    S_CMSMenuItemTypeJS_Enum[4], S_CMSMenuItemTypeJS_Enum[5], S_CMSMenuItemTypeJS_Enum[6], S_CMSMenuItemTypeJS_Enum[7]];

//------------------------------------------------------------------------------------------------------------------------------------------------------------
ListCMSMenuItemJS.prototype.init = function (ListCMSMenuItem) {

    this.items = [];

    if (typeof ListCMSMenuItem != "undefined") {
        for (var i in ListCMSMenuItem.items) {
            this.items.push(new S_CMSMenuItemJS(ListCMSMenuItem.items[i]));
        }
    }
}

function ListCMSMenuItemJS(ListCMSMenuItem) {
    this.init(ListCMSMenuItem);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------

S_CMSMenuItemJS.prototype.init = function (S_CMSMenuItem) {

    if (S_CMSMenuItem == null) {
        this.id = -1;
        this.list_item = null;
        this.title = "";
        this.order = -1;
        this.type = S_CMSMenuItemTypeJS_Enum.S_CMSMenuItemType_text;
        this.content = "";
        this.filename = "";
        this.iconstring = "";
        this.iconfilename = "";
        this.parent_dbid = -1;
        this.destination_dbid = -1;
        this.status = S_CMSMenuItemStatusJS_Enum.S_CMSMenuItemStatus_active;
    }
    else {
        this.id = S_CMSMenuItem.id;
        this.list_item = S_CMSMenuItem.list_item == null ? null : new S_CMSListItemJS(S_CMSMenuItem.list_item);
        this.title = S_CMSMenuItem.title;
        this.order = S_CMSMenuItem.order;
        this.type = S_CMSMenuItemTypeJS_Enum[S_CMSMenuItem.type];
        this.content = S_CMSMenuItem.content;
        this.filename = S_CMSMenuItem.filename;
        this.iconstring = S_CMSMenuItem.iconstring;
        this.iconfilename = S_CMSMenuItem.iconfilename;
        this.parent_dbid = S_CMSMenuItem.parent_dbid;
        this.destination_dbid = S_CMSMenuItem.destination_dbid;
        this.status = S_CMSMenuItemStatusJS_Enum[S_CMSMenuItem.status];
    }
}

S_CMSMenuItemJS.prototype.Convert = function () {

    var result = new S_CMSMenuItem();
    result.id = this.id;
    result.list_item = this.list_item.Convert();
    result.title = this.title;
    result.order = this.order;
    result.type = this.type;
    result.content = this.content;
    result.filename = this.filename;
    result.iconstring = this.iconstring;
    result.iconfilename = this.iconfilename;
    result.parent_dbid = this.parent_dbid;
    result.destination_dbid = this.destination_dbid;
    result.status = this.status;

    return result;
}
S_CMSMenuItemJS.prototype.UpdateMenuItem = function () {
    this.list_item.title = this.title;
    this.list_item.line1 = this.status;
    this.list_item.line2 = "";
    this.list_item.line3 = "";
}
function S_CMSMenuItemJS(S_CMSMenuItem) {
    this.init(S_CMSMenuItem)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------

ListCMSRaceJS.prototype.init = function (ListCMSRace) {

    this.items = [];

    if (typeof ListCMSRace != "undefined") {
        for (var i in ListCMSRace.items) {
            this.items.push(new S_CMSRaceJS(ListCMSRace.items[i]));
        }
    }
}

function ListCMSRaceJS(ListCMSRace) {
    this.init(ListCMSRace);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

ListCMSRaceCategoryJS.prototype.init = function (ListCMSRaceCategory) {

    this.items = [];

    if (typeof ListCMSRaceCategory != "undefined") {
        for (var i in ListCMSRaceCategory.items) {
            this.items.push(new S_CMSRaceCategoryJS(ListCMSRaceCategory.items[i]));
        }
    }
}

function ListCMSRaceCategoryJS(ListCMSRaceCategory) {
    this.init(ListCMSRaceCategory);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

ListCMSRaceResultJS.prototype.init = function (ListCMSRaceResult) {

    this.items = [];

    if (typeof ListCMSRaceResult != "undefined") {
        for (var i in ListCMSRaceResult.items) {
            this.items.push(new S_CMSRaceResultJS(ListCMSRaceResult.items[i]));
        }
    }
}

function ListCMSRaceResultJS(ListCMSRaceResult) {
    this.init(ListCMSRaceResult);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

var S_CMSRaceStatusJS_Enum;
(function (S_CMSRaceStatusJS_Enum) {
    S_CMSRaceStatusJS_Enum[S_CMSRaceStatusJS_Enum["S_CMSRaceStatus_active"] = 0] = "S_CMSRaceStatus_active";
    S_CMSRaceStatusJS_Enum[S_CMSRaceStatusJS_Enum["S_CMSRaceStatus_inactive"] = 1] = "S_CMSRaceStatus_inactive";
})(S_CMSRaceStatusJS_Enum || (S_CMSRaceStatusJS_Enum = {}));
S_CMSRaceStatusJS_Array = [S_CMSRaceStatusJS_Enum[0], S_CMSRaceStatusJS_Enum[1]];

//------------------------------------------------------------------------------------------------------------------------------------------------------------

var S_CMSRaceResultStatusJS_Enum;
(function (S_CMSRaceResultStatusJS_Enum) {
    S_CMSRaceResultStatusJS_Enum[S_CMSRaceResultStatusJS_Enum["S_CMSRaceResultStatus_active"] = 0] = "S_CMSRaceResultStatus_active";
    S_CMSRaceResultStatusJS_Enum[S_CMSRaceResultStatusJS_Enum["S_CMSRaceResultStatus_inactive"] = 1] = "S_CMSRaceResultStatus_inactive";
})(S_CMSRaceResultStatusJS_Enum || (S_CMSRaceResultStatusJS_Enum = {}));
S_CMSRaceResultStatusJS_Array = [S_CMSRaceResultStatusJS_Enum[0], S_CMSRaceResultStatusJS_Enum[1]];

//------------------------------------------------------------------------------------------------------------------------------------------------------------

var S_CMSSexJS_Enum;
(function (S_CMSSexJS_Enum) {
    S_CMSSexJS_Enum[S_CMSSexJS_Enum["S_CMSSex_unknown"] = 0] = "S_CMSSex_unknown";
    S_CMSSexJS_Enum[S_CMSSexJS_Enum["S_CMSSex_male"] = 1] = "S_CMSSex_male";
    S_CMSSexJS_Enum[S_CMSSexJS_Enum["S_CMSSex_female"] = 2] = "S_CMSSex_female";
})(S_CMSSexJS_Enum || (S_CMSSexJS_Enum = {}));
S_CMSSexJS_Array = [S_CMSSexJS_Enum[0], S_CMSSexJS_Enum[1], S_CMSSexJS_Enum[2]];

//------------------------------------------------------------------------------------------------------------------------------------------------------------

S_CMSRaceJS.prototype.init = function (S_CMSRace) {

    if (S_CMSRace == null) {
        this.list_item = null;
        this.id = -1;
        this.name = "";
        this.start = 0;
        this.status = S_CMSRaceStatusJS_Enum.S_CMSRaceStatus_inactive;
        this.last_modified = 0;
        this.categories = [];
    }
    else {
        this.list_item = S_CMSRace.list_item == null ? null : new S_CMSListItemJS(S_CMSRace.list_item);
        this.id = S_CMSRace.id;
        this.name = S_CMSRace.name;
        this.start = S_CMSRace.start;
        this.status = S_CMSRaceStatusJS_Enum[S_CMSRace.status];
        this.last_modified = S_CMSRace.last_modified;
        this.categories = [];
        for (var i in S_CMSRace.categories) {
            if (S_CMSRace.categories[i] != null)
                this.categories.push(new S_CMSRaceCategoryJS(S_CMSRace.categories[i]));
        }
    }
}

S_CMSRaceJS.prototype.Convert = function () {

    var result = new S_CMSRace();
    result.list_item = this.list_item.Convert();
    result.id = this.id;
    result.name = this.name;
    result.start = this.start;
    result.status = this.status;
    result.last_modified = this.last_modified;
    result.categories = [];
    for (var i in this.categories) {
        if (typeof this.categories[i].Convert() != "undefined") 
            result.categories.push(this.categories[i].Convert());
    }
    return result;
}
S_CMSRaceJS.prototype.UpdateMenuItem = function () {
    this.list_item.title = this.name;
    this.list_item.line1 = this.start.toString();
    this.list_item.line2 = this.status;
    this.list_item.line3 = "";
}
function S_CMSRaceJS(S_CMSRace) {
    this.init(S_CMSRace)
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

S_CMSRaceCategoryJS.prototype.init = function (S_CMSRaceCategory) {

    if (S_CMSRaceCategory == null) {
        this.list_item = null;
        this.id = -1;
        this.name = "";
        this.class_id = -1;
        this.class_name = "";
    }
    else {
        this.list_item = S_CMSRaceCategory.list_item == null ? null : new S_CMSListItemJS(S_CMSRaceCategory.list_item);
        this.id = S_CMSRaceCategory.id;
        this.name = S_CMSRaceCategory.name;
        this.class_id = S_CMSRaceCategory.class_id;
        this.class_name = S_CMSRaceCategory.class_name;
    }
}

S_CMSRaceCategoryJS.prototype.Convert = function () {

    var result = new S_CMSRaceCategory();
    result.list_item = this.list_item.Convert();
    result.id = this.id;
    result.name = this.name;
    result.class_id = this.class_id;
    result.class_name = this.class_name;

    return result;
}
S_CMSRaceCategoryJS.prototype.UpdateMenuItem = function () {
    this.list_item.title = this.dh_cat;
    this.list_item.line1 = this.class_id;
    this.list_item.line2 = this.class_name;
    this.list_item.line3 = "";
}
function S_CMSRaceCategoryJS(S_CMSRaceCategory) {
    this.init(S_CMSRaceCategory)
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

S_CMSRaceResultJS.prototype.init = function (S_CMSRaceResult) {

    if (S_CMSRaceResult == null) {
        this.list_item = null;
        this.id = -1;
        this.status = S_CMSRaceResultStatusJS_Enum.S_CMSRaceResultStatus_inactive;
        this.rank = -1;
        this.bib = -1;
        this.last_name = "";
        this.first_name = "";
        this.sex = S_CMSSexJS_Enum.S_CMSSex_unknown;
        this.age = -1;
        this.address = "";
        this.city = "";
        this.state = "";
        this.nation = "";
        this.sponsor = "";
        this.license = "";
        this.finish = -1;
    }
    else {
        this.list_item = S_CMSRaceResult.list_item == null ? null : new S_CMSListItemJS(S_CMSRaceResult.list_item);
        this.id = S_CMSRaceResult.id;
        this.status = S_CMSRaceResultStatusJS_Enum[S_CMSRaceResult.status];
        this.rank = S_CMSRaceResult.rank;
        this.bib = S_CMSRaceResult.bib;
        this.last_name = S_CMSRaceResult.last_name;
        this.first_name = S_CMSRaceResult.first_name;
        this.sex = S_CMSSexJS_Enum[S_CMSRaceResult.sex];
        this.age = S_CMSRaceResult.age;
        this.address = S_CMSRaceResult.address;
        this.city = S_CMSRaceResult.city;
        this.state = S_CMSRaceResult.state;
        this.nation = S_CMSRaceResult.nation;
        this.sponsor = S_CMSRaceResult.sponsor;
        this.license = S_CMSRaceResult.license;
        this.finish = S_CMSRaceResult.finish;
    }
}

S_CMSRaceResultJS.prototype.Convert = function () {

    var result = new S_CMSRaceResult();
    result.list_item = this.list_item.Convert();
    result.id = this.id;
    result.status = this.status;
    result.rank = this.rank;
    result.bib = this.bib;
    result.last_name = this.last_name;
    result.first_name = this.first_name;
    result.sex = this.sex;
    result.age = this.age;
    result.address = this.address;
    result.city = this.city;
    result.state = this.state;
    result.nation = this.nation;
    result.sponsor = this.sponsor;
    result.license = this.license;
    result.finish = this.finish;

    return result;
}
S_CMSRaceResultJS.prototype.UpdateMenuItem = function () {
    this.list_item.title = this.rank.toString();
    this.list_item.line1 = this.finish.toString();
    this.list_item.line2 = this.first_name + " " + this.last_name;
    this.list_item.line3 = this.license;
}
function S_CMSRaceResultJS(S_CMSRaceResult) {
    this.init(S_CMSRaceResult)
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
ListCMSSpeakerJS.prototype.init = function (ListCMSSpeaker) {

    this.items = [];

    if (typeof ListCMSSpeaker != "undefined") {
        for (var i in ListCMSSpeaker.items) {
            this.items.push(new S_CMSSpeakerJS(ListCMSSpeaker.items[i]));
        }
    }
}

function ListCMSSpeakerJS(ListCMSSpeaker) {
    this.init(ListCMSSpeaker);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSSpeakerJS.prototype.init = function (S_CMSSpeaker) {

    if (S_CMSSpeaker == null) {
        this.list_item = null;
        this.id = -1;
        this.title = "";
        this.first_name = "";
        this.last_name = "";
        this.company_name = "";
        this.logo_url = "";
        this.logo_str = "";
        this.profile = "";
        this.last_modified = 0;
    }
    else {
        this.list_item = S_CMSSpeaker.list_item == null ? null : new S_CMSListItemJS(S_CMSSpeaker.list_item);
        this.id = S_CMSSpeaker.id;
        this.title = S_CMSSpeaker.title;
        this.first_name = S_CMSSpeaker.first_name;
        this.last_name = S_CMSSpeaker.last_name;
        this.company_name = S_CMSSpeaker.company_name;
        this.logo_url = S_CMSSpeaker.logo_url;
        this.logo_str = S_CMSSpeaker.logo_str;
        this.profile = S_CMSSpeaker.profile;
        this.last_modified = S_CMSSpeaker.last_modified;
    }
}

S_CMSSpeakerJS.prototype.Convert = function () {

    var result = new S_CMSSpeaker();
    result.list_item = this.list_item.Convert();
    result.id = this.id;
    result.title = this.title;
    result.first_name = this.first_name;
    result.last_name = this.last_name;
    result.company_name = this.company_name;
    result.logo_url = this.logo_url;
    result.logo_str = this.logo_str;
    result.profile = this.profile;
    result.last_modified = this.last_modified;

    return result;
}
S_CMSSpeakerJS.prototype.UpdateMenuItem = function () {
    this.list_item.title = this.title;
    this.list_item.line1 = this.first_name;
    this.list_item.line2 = this.last_name;
    this.list_item.line3 = this.company_name;
}
function S_CMSSpeakerJS(S_CMSSpeaker) {
    this.init(S_CMSSpeaker)
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
ListCMSEventLocationJS.prototype.init = function (ListCMSEventLocation) {

    this.items = [];

    if (typeof ListCMSEventLocation != "undefined") {
        for (var i in ListCMSEventLocation.items) {
            this.items.push(new S_CMSEventLocationJS(ListCMSEventLocation.items[i]));
        }
    }
}

function ListCMSEventLocationJS(ListCMSEventLocation) {
    this.init(ListCMSEventLocation);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSEventLocationJS.prototype.init = function (S_CMSEventLocation) {

    if (S_CMSEventLocation == null) {
        this.list_item = null;
        this.location = "";
        this.destination_id = "";
    }
    else {
        this.list_item = S_CMSEventLocation.list_item == null ? null : new S_CMSListItemJS(S_CMSEventLocation.list_item);
        this.location = S_CMSEventLocation.location;
        this.destination_id = S_CMSEventLocation.destination_id;
    }
}

S_CMSEventLocationJS.prototype.Convert = function () {

    var result = new S_CMSEventLocation();
    result.list_item = this.list_item.Convert();
    result.location = this.location;
    result.destination_id = this.destination_id;

    return result;
}
S_CMSEventLocationJS.prototype.UpdateMenuItem = function () {
    this.list_item.title = this.location;
    this.list_item.line1 = this.destination_id.toString();
    this.list_item.line2 = "";
    this.list_item.line3 = "";
}
function S_CMSEventLocationJS(S_CMSEventLocation) {
    this.init(S_CMSEventLocation)
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
ListCMSEventGroupJS.prototype.init = function (ListCMSEventGroup) {

    this.items = [];

    if (typeof ListCMSEventGroup != "undefined") {
        for (var i in ListCMSEventGroup.items) {
            this.items.push(new S_CMSEventGroupJS(ListCMSEventGroup.items[i]));
        }
    }
}

function ListCMSEventGroupJS(ListCMSEventGroup) {
    this.init(ListCMSEventGroup);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSEventGroupJS.prototype.init = function (S_CMSEventGroup) {

    if (S_CMSEventGroup == null) {
        this.list_item = null;
        this.id = -1;
        this.group = "";
        this.subgroup = "";
        this.colour = [0xBB, 0xBB, 0xBB, 0xFF];
    }
    else {
        this.list_item = S_CMSEventGroup.list_item == null ? null : new S_CMSListItemJS(S_CMSEventGroup.list_item);
        this.id = S_CMSEventGroup.id;
        this.group = S_CMSEventGroup.group;
        this.subgroup = S_CMSEventGroup.subgroup;
        this.colour = GetColorBytes(S_CMSEventGroup.colour);
    }
}

S_CMSEventGroupJS.prototype.Convert = function () {

    var result = new S_CMSEventGroup();
    result.list_item = this.list_item.Convert();
    result.id = this.id;
    result.group = this.group;
    result.subgroup = this.subgroup;
    result.colour = this.colour;

    return result;
}
S_CMSEventGroupJS.prototype.UpdateMenuItem = function () {
    this.list_item.title = this.group;
    this.list_item.line1 = this.subgroup;
    this.list_item.line2 = "";
    this.list_item.line3 = "";
}
function S_CMSEventGroupJS(S_CMSEventGroup) {
    this.init(S_CMSEventGroup)
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

var S_CMSSeminarSpeakerLinkTypeJS_Enum;
(function (S_CMSSeminarSpeakerLinkTypeJS_Enum) {
    S_CMSSeminarSpeakerLinkTypeJS_Enum[S_CMSSeminarSpeakerLinkTypeJS_Enum["S_CMSSeminarSpeakerLinkType_unknown"] = 0] = "S_CMSSeminarSpeakerLinkType_unknown";
    S_CMSSeminarSpeakerLinkTypeJS_Enum[S_CMSSeminarSpeakerLinkTypeJS_Enum["S_CMSSeminarSpeakerLinkType_speaker"] = 1] = "S_CMSSeminarSpeakerLinkType_speaker";
    S_CMSSeminarSpeakerLinkTypeJS_Enum[S_CMSSeminarSpeakerLinkTypeJS_Enum["S_CMSSeminarSpeakerLinkType_chair"] = 2] = "S_CMSSeminarSpeakerLinkType_chair";
})(S_CMSSeminarSpeakerLinkTypeJS_Enum || (S_CMSSeminarSpeakerLinkTypeJS_Enum = {}));
S_CMSSeminarSpeakerLinkTypeJS_Array = [S_CMSSeminarSpeakerLinkTypeJS_Enum[0], S_CMSSeminarSpeakerLinkTypeJS_Enum[1], S_CMSSeminarSpeakerLinkTypeJS_Enum[2]];

//------------------------------------------------------------------------------------------------------------------------------------------------------------

var S_CMSSeminarAdmissionTypeJS_Enum;
(function (S_CMSSeminarAdmissionTypeJS_Enum) {
    S_CMSSeminarAdmissionTypeJS_Enum[S_CMSSeminarAdmissionTypeJS_Enum["S_CMSSeminarAdmissionType_unknown"] = 0] = "S_CMSSeminarAdmissionType_unknown";
    S_CMSSeminarAdmissionTypeJS_Enum[S_CMSSeminarAdmissionTypeJS_Enum["S_CMSSeminarAdmissionType_free"] = 1] = "S_CMSSeminarAdmissionType_free";
    S_CMSSeminarAdmissionTypeJS_Enum[S_CMSSeminarAdmissionTypeJS_Enum["S_CMSSeminarAdmissionType_paid"] = 2] = "S_CMSSeminarAdmissionType_paid";
})(S_CMSSeminarAdmissionTypeJS_Enum || (S_CMSSeminarAdmissionTypeJS_Enum = {}));
S_CMSSeminarAdmissionTypeJS_Array = [S_CMSSeminarAdmissionTypeJS_Enum[0], S_CMSSeminarAdmissionTypeJS_Enum[1], S_CMSSeminarAdmissionTypeJS_Enum[2]];

//------------------------------------------------------------------------------------------------------------------------------------------------------------

var S_CMSSeminarTypeJS_Enum;
(function (S_CMSSeminarTypeJS_Enum) {
    S_CMSSeminarTypeJS_Enum[S_CMSSeminarTypeJS_Enum["S_CMSSeminarType_unknown"] = 0] = "S_CMSSeminarType_unknown";
    S_CMSSeminarTypeJS_Enum[S_CMSSeminarTypeJS_Enum["S_CMSSeminarType_session"] = 1] = "S_CMSSeminarType_session";
    S_CMSSeminarTypeJS_Enum[S_CMSSeminarTypeJS_Enum["S_CMSSeminarType_meeting"] = 2] = "S_CMSSeminarType_meeting";
    S_CMSSeminarTypeJS_Enum[S_CMSSeminarTypeJS_Enum["S_CMSSeminarType_seminar"] = 3] = "S_CMSSeminarType_seminar";
    S_CMSSeminarTypeJS_Enum[S_CMSSeminarTypeJS_Enum["S_CMSSeminarType_talk"] = 4] = "S_CMSSeminarType_talk";
})(S_CMSSeminarTypeJS_Enum || (S_CMSSeminarTypeJS_Enum = {}));
S_CMSSeminarTypeJS_Array = [S_CMSSeminarTypeJS_Enum[0], S_CMSSeminarTypeJS_Enum[1], S_CMSSeminarTypeJS_Enum[2], S_CMSSeminarTypeJS_Enum[3], S_CMSSeminarTypeJS_Enum[4]];

//------------------------------------------------------------------------------------------------------------------------------------------------------------

var S_CMSSeminarStatusJS_Enum;
(function (S_CMSSeminarStatusJS_Enum) {
    S_CMSSeminarStatusJS_Enum[S_CMSSeminarStatusJS_Enum["S_CMSSeminarStatus_active"] = 0] = "S_CMSSeminarStatus_active";
    S_CMSSeminarStatusJS_Enum[S_CMSSeminarStatusJS_Enum["S_CMSSeminarStatus_inactive"] = 1] = "S_CMSSeminarStatus_inactive";
})(S_CMSSeminarStatusJS_Enum || (S_CMSSeminarStatusJS_Enum = {}));
S_CMSSeminarStatusJS_Array = [S_CMSSeminarStatusJS_Enum[0], S_CMSSeminarStatusJS_Enum[1]];

//------------------------------------------------------------------------------------------------------------------------------------------------------------

ListCMSSeminarJS.prototype.init = function (ListCMSSeminar) {

    this.items = [];

    if (typeof ListCMSSeminar != "undefined") {
        for (var i in ListCMSSeminar.items) {
            this.items.push(new S_CMSSeminarJS(ListCMSSeminar.items[i]));
        }
    }
}

function ListCMSSeminarJS(ListCMSSeminar) {
    this.init(ListCMSSeminar);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSSeminarJS.prototype.init = function (S_CMSSeminar) {

    if (S_CMSSeminar == null) {
        this.list_item = null;
        this.id = -1;
        this.title = "";
        this.start = 0;
        this.end = 0;
        this.description = "";
        this.eventlocation = null;
        this.eventgroup = null;
        this.admission_type = S_CMSSeminarAdmissionTypeJS_Enum.S_CMSSeminarAdmissionType_unknown;
        this.last_modified = 0;
        this.type = S_CMSSeminarTypeJS_Enum.S_CMSSeminarType_unknown;
        this.status = S_CMSSeminarStatusJS_Enum.S_CMSSeminarStatus_inactive;
        this.seats = -1;
        this.profile_dbid = -1;
        this.logo_url = "";
        this.logo_str = "";
        this.chair = null;
        this.speakers = [];
    }
    else {
        this.list_item = S_CMSSeminar.list_item == null ? null : new S_CMSListItemJS(S_CMSSeminar.list_item);
        this.id = S_CMSSeminar.id;
        this.title = S_CMSSeminar.title;
        this.start = S_CMSSeminar.start;
        this.end = S_CMSSeminar.end;
        this.description = S_CMSSeminar.description;
        this.eventlocation = S_CMSSeminar.eventlocation == null ? null : new S_CMSEventLocationJS(S_CMSSeminar.eventlocation);
        this.eventgroup = S_CMSSeminar.eventgroup == null ? null : new S_CMSEventGroupJS(S_CMSSeminar.eventgroup);
        this.admission_type = S_CMSSeminarAdmissionTypeJS_Enum[S_CMSSeminar.admission_type];
        this.last_modified = S_CMSSeminar.last_modified;
        this.type = S_CMSSeminarTypeJS_Enum[S_CMSSeminar.type];
        this.status = S_CMSSeminarStatusJS_Enum[S_CMSSeminar.status];
        this.seats = S_CMSSeminar.seats;
        this.profile_dbid = S_CMSSeminar.profile_dbid;
        this.logo_url = S_CMSSeminar.logo_url;
        this.logo_str = S_CMSSeminar.logo_str;
        this.chair = S_CMSSeminar.chair == null ? null : new S_CMSSpeakerJS(S_CMSSeminar.chair);
        this.speakers = [];
        for (var i in S_CMSSeminar.speakers) {
            if (S_CMSSeminar.speakers[i] != null)
                this.speakers.push(new S_CMSSpeakerJS(S_CMSSeminar.speakers[i]));
        }
    }
}

S_CMSSeminarJS.prototype.Convert = function () {

    var result = new S_CMSSeminar();
    result.list_item = this.list_item.Convert();
    result.id = this.id;
    result.title = this.title;
    result.start = this.start;
    result.end = this.end;
    result.description = this.description;
    result.eventlocation = this.eventlocation.Convert();
    result.eventgroup = this.eventgroup.Convert();
    result.admission_type = this.admission_type;
    result.last_modified = this.last_modified;
    result.type = this.type;
    result.status = this.status;
    result.seats = this.seats;
    result.profile_dbid = this.profile_dbid;
    result.logo_url = this.logo_url;
    result.logo_str = this.logo_str;
    result.chair = this.chair.Convert();

    result.speakers = [];
    for (var i in this.speakers) {
        if (typeof this.speakers[i].Convert() != "undefined") 
            result.speakers.push(this.speakers[i].Convert());
    }

    return result;
}
S_CMSSeminarJS.prototype.UpdateMenuItem = function () {
    this.list_item.title = this.title;
    this.list_item.line1 = this.eventgroup == null ? "" : this.eventgroup.group;
    this.list_item.line2 = this.eventgroup == null ? "" : this.eventgroup.subgroup;
    this.list_item.line3 = "";
}
function S_CMSSeminarJS(S_CMSSeminar) {
    this.init(S_CMSSeminar)
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

function GetColorBytes( color)
{
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
//------------------------------------------------------------------------------------------------------------------------------------------------------------
ListCMSGraphJS.prototype.init = function (ListCMSGraph) {

    this.items = [];
    if (typeof ListCMSGraph != "undefined")
        for (var i in ListCMSGraph.items)
            this.items.push(new S_CMSGraphJS(ListCMSGraph.items[i]));
}

function ListCMSGraphJS(ListCMSGraph) {
    this.init(ListCMSGraph);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
S_CMSGraphJS.prototype.init = function (S_CMSGraph) {

    if (S_CMSGraph == null) {
        this.title = "";
        this.type = "Line";
        this.x_label = "";
        this.y_label = "";
        this.x_names = []
        this.bar_names = []
        this.values = [];
    }
    else {
        this.title = S_CMSGraph.title;
        this.type = S_CMSGraph.type;
        this.x_label = S_CMSGraph.x_label;
        this.y_label = S_CMSGraph.y_label;
        this.x_names = []
        for (var i in S_CMSGraph.x_names) {
            this.x_names.push(S_CMSGraph.x_names[i]);
        }
        this.bar_names = []
        for (var i in S_CMSGraph.bar_names) {
            this.bar_names.push(S_CMSGraph.bar_names[i]);
        }
        this.values = [];
        for (var i in S_CMSGraph.values) {
            this.values.push(S_CMSGraph.values[i]);
        }
    }
}
function S_CMSGraphJS(S_CMSGraph) {
    this.init(S_CMSGraph)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------
