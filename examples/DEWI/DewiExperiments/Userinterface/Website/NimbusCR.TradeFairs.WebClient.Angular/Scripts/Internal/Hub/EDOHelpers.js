function GetTransformableObjectProfile(S_ProjectProfileJS, building_id, floorplan_id)
{
    if (S_ProjectProfileJS == null) return null;

    if (myProjectProfileJS.transformation.identif.building_id == building_id && myProjectProfileJS.transformation.identif.floorplan_id == floorplan_id)
        return myProjectProfileJS.transformation;

    S_TransformableObjectProfileJS_test.longname = "33";
}

function ConvertByteArraytoHexString(arr) {
    var result = "";
    var z;

    for (var i = 0; i < arr.length; i++) {
        var str = arr[i].toString(16);

        z = 2 - str.length + 1;
        str = Array(z).join("0") + str;

        result += str;
    }

    return result;
}

function TryConvertByteArraytoHexString_RGB(arr) {

    if (arr.length < 3) return null;

    var result = "#";
    var z;
    for (var i = 0; i < 3; i++) {
        var str = arr[i].toString(16);

        z = 2 - str.length + 1;
        str = Array(z).join("0") + str;

        result += str;
    }

    return result;
}

function TryConvertByteArraytoHexString_RGB_fakeA(arr, A) {

    if (arr.length < 3) return null;

    var result = "#";
    var z;
    for (var i = 0; i < 3; i++) {
        var ligther = Math.floor(arr[i] + (255 - arr[i]) * (255 - A) / 255);
        var str = ligther.toString(16);

        z = 2 - str.length + 1;
        str = Array(z).join("0") + str;

        result += str;
    }

    return result;
}

function TryConvertByteArraytoOpacity(arr) {

    if (arr.length < 4) return 1;

    return arr[3] / 255.0;
}

function TryConvertByteArraytoHexString_RGBA_Fill(arr) {

    if (arr.length < 4) return TryConvertByteArraytoHexString_RGB(arr);

    var result = "#";
    var z;
    for (var i = 0; i < 4; i++) {

        var str = arr[i].toString(16);

        if (i == 3)
          str = (arr[i] * 100 / 255).toString(16);

        z = 2 - str.length + 1;
        str = Array(z).join("0") + str;

        result += str;
    }

    return result;
}