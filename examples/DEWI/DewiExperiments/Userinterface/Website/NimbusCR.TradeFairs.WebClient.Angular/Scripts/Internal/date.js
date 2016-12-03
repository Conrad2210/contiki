/********************************************************************
* Author: John Swanton
* Created Date: 09/03/2014
* Last Modified: 09/03/2014
* 
* date.js
* Description: 
* date functions
* ******************************************************************/

// extend date function
Date.prototype.format = function (format) {
    var returnStr = '';
    var replace = Date.replaceChars;
    for (var i = 0; i < format.length; i++) {
        var curChar = format.charAt(i); if (i - 1 >= 0 && format.charAt(i - 1) == "\\") {
            returnStr += curChar;
        }
        else if (replace[curChar]) {
            returnStr += replace[curChar].call(this);
        } else if (curChar != "\\") {
            returnStr += curChar;
        }
    }
    return returnStr;
};

Date.replaceChars = {
    shortMonths: ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'],
    longMonths: ['January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December'],
    shortDays: ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat'],
    longDays: ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'],

    // Day
    d: function () { return (this.getDate() < 10 ? '0' : '') + this.getDate(); },
    D: function () { return Date.replaceChars.shortDays[this.getDay()]; },
    j: function () { return this.getDate(); },
    l: function () { return Date.replaceChars.longDays[this.getDay()]; },
    N: function () { return this.getDay() + 1; },
    S: function () { return (this.getDate() % 10 == 1 && this.getDate() != 11 ? 'st' : (this.getDate() % 10 == 2 && this.getDate() != 12 ? 'nd' : (this.getDate() % 10 == 3 && this.getDate() != 13 ? 'rd' : 'th'))); },
    w: function () { return this.getDay(); },
    z: function () { var d = new Date(this.getFullYear(), 0, 1); return Math.ceil((this - d) / 86400000); }, // Fixed now
    // Week
    W: function () { var d = new Date(this.getFullYear(), 0, 1); return Math.ceil((((this - d) / 86400000) + d.getDay() + 1) / 7); }, // Fixed now
    // Month
    F: function () { return Date.replaceChars.longMonths[this.getMonth()]; },
    m: function () { return (this.getMonth() < 9 ? '0' : '') + (this.getMonth() + 1); },
    M: function () { return Date.replaceChars.shortMonths[this.getMonth()]; },
    n: function () { return this.getMonth() + 1; },
    t: function () { var d = new Date(); return new Date(d.getFullYear(), d.getMonth(), 0).getDate() }, // Fixed now, gets #days of date
    // Year
    L: function () { var year = this.getFullYear(); return (year % 400 == 0 || (year % 100 != 0 && year % 4 == 0)); },   // Fixed now
    o: function () { var d = new Date(this.valueOf()); d.setDate(d.getDate() - ((this.getDay() + 6) % 7) + 3); return d.getFullYear(); }, //Fixed now
    Y: function () { return this.getFullYear(); },
    y: function () { return ('' + this.getFullYear()).substr(2); },
    // Time
    a: function () { return this.getHours() < 12 ? 'am' : 'pm'; },
    A: function () { return this.getHours() < 12 ? 'AM' : 'PM'; },
    B: function () { return Math.floor((((this.getUTCHours() + 1) % 24) + this.getUTCMinutes() / 60 + this.getUTCSeconds() / 3600) * 1000 / 24); }, // Fixed now
    g: function () { return this.getHours() % 12 || 12; },
    G: function () { return this.getHours(); },
    h: function () { return ((this.getHours() % 12 || 12) < 10 ? '0' : '') + (this.getHours() % 12 || 12); },
    H: function () { return (this.getHours() < 10 ? '0' : '') + this.getHours(); },
    i: function () { return (this.getMinutes() < 10 ? '0' : '') + this.getMinutes(); },
    s: function () { return (this.getSeconds() < 10 ? '0' : '') + this.getSeconds(); },
    u: function () {
        var m = this.getMilliseconds(); return (m < 10 ? '00' : (m < 100 ?
    '0' : '')) + m;
    },
    // Timezone
    e: function () { return "Not Yet Supported"; },
    I: function () {
        var DST = null;
        for (var i = 0; i < 12; ++i) {
            var d = new Date(this.getFullYear(), i, 1);
            var offset = d.getTimezoneOffset();

            if (DST === null) DST = offset;
            else if (offset < DST) { DST = offset; break; } else if (offset > DST) break;
        }
        return (this.getTimezoneOffset() == DST) | 0;
    },
    O: function () { return (-this.getTimezoneOffset() < 0 ? '-' : '+') + (Math.abs(this.getTimezoneOffset() / 60) < 10 ? '0' : '') + (Math.abs(this.getTimezoneOffset() / 60)) + '00'; },
    P: function () { return (-this.getTimezoneOffset() < 0 ? '-' : '+') + (Math.abs(this.getTimezoneOffset() / 60) < 10 ? '0' : '') + (Math.abs(this.getTimezoneOffset() / 60)) + ':00'; }, // Fixed now
    T: function () { var m = this.getMonth(); this.setMonth(0); var result = this.toTimeString().replace(/^.+ \(?([^\)]+)\)?$/, '$1'); this.setMonth(m); return result; },
    Z: function () { return -this.getTimezoneOffset() * 60; },
    // Full Date/Time
    c: function () { return this.format("Y-m-d\\TH:i:sP"); }, // Fixed now
    r: function () { return this.toString(); },
    U: function () { return this.getTime() / 1000; }
};
/*
EXAMPLES
*/
/*var now = new Date();//date now
now.setSeconds(now.getSeconds() - 10);//take 10 seconds from time
console.log(formatDate(now.getTime(), 'Y-m-d H:i:s'));
console.log(timeSince(now.getTime()));
console.log(getTimeAgo(now.getTime()));

console.log(formatDate('1425655417382', 'Y-m-d H:i:s'));
console.log(timeSince('1425655417382'));
console.log(getTimeAgo('1425655417382'));

console.log(formatDate('1425895650', 'Y-m-d H:i:s'));
console.log(timeSince('1425895650'));
console.log(getTimeAgo('1425895650'));

var now = new Date();

var minute_now = now.getMinutes();
var minutes_over = minute_now % 15;

var end_date = new Date();
var start_date = new Date();

end_date.setMinutes(minute_now - minutes_over);
end_date.setSeconds(0);
end_date.setMilliseconds(0);
console.log('End date ' + formatDate(end_date.getTime(), 'Y-m-d H:i:s'));

start_date.setMinutes(minute_now - minutes_over);
start_date.setSeconds(0);
start_date.setMilliseconds(0);
start_date.setDate(end_date.getDate()-1);

console.log('bstart date ' + formatDate(start_date.getTime(), 'Y-m-d H:i:s'));
var difMiliseconds = end_date - start_date;
var diffSteps = ((difMiliseconds / 3600000) * 60) / 15;

for (i = 0; i < diffSteps+1; i++) {
    console.log(i + ' Date Now:' + formatDate(start_date.getTime(), 'Y-m-d H:i:s'));
    start_date.setMinutes(start_date.getMinutes() + 15);
}*/

function epochToDate(timestamp) {
    console.log(timestamp);
    if (timestamp.length == 10) {
        timestamp = timestamp * 1000;
    }

    if (timestamp.length == 13) {
        var dateVal = "/Date(" + timestamp + ")/";
        var date = new Date(parseFloat(dateVal.substr(6)));
        timestamp = date;
    }
    var myDate = new Date(timestamp);
    console.log(myDate);
    return myDate;

}


function formatDate(timestamp, format) {
    var myDate = epochToDate(timestamp);
    return myDate.format(format);
}

function getTimeAgo(timestamp) {
    var date = epochToDate(timestamp);
    var units = [
       { name: "sec", limit: 60, in_seconds: 1 },
      { name: "min", limit: 3600, in_seconds: 60 },
      { name: "hr", limit: 86400, in_seconds: 3600 },
      { name: "day", limit: 604800, in_seconds: 86400 },
      { name: "wk", limit: 2629743, in_seconds: 604800 },
      { name: "mth", limit: 31556926, in_seconds: 2629743 },
      { name: "yr", limit: null, in_seconds: 31556926 }
    ];
    var diff = (new Date() - date) / 1000;
    if (diff < 5) return "now";

    var i = 0;
    while (unit = units[i++]) {
        if (diff < unit.limit || !unit.limit) {
            var diff = Math.floor(diff / unit.in_seconds);
            return diff + " " + unit.name + (diff > 1 ? "s" : "");
        }
    };
}


function timeSince(timestamp) {
    var now = new Date();
    var date = epochToDate(timestamp);

    secondsPast = (now.getTime() - date.getTime()) / 1000;
    console.log('seconds past' + parseInt(secondsPast));
    if (secondsPast < 60) {
        return parseInt(secondsPast) + ' sec';
    }
    if (secondsPast < 3600) {
        return parseInt(secondsPast / 60) + ' min';
    }
    if (secondsPast <= 86400) {
        return parseInt(secondsPast / 3600) + ' hr';
    }
    if (secondsPast > 86400) {
        day = date.getDate();
        month = date.toDateString().match(/ [a-zA-Z]*/)[0].replace(" ", "");
        year = date.getFullYear() == now.getFullYear() ? "" : " " + date.getFullYear();
        return day + " " + month + year;
    }
}


