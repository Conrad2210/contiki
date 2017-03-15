/**
 * Created by R00118979 on 14/03/2017.
 */
function retrievData(url)
{
    var result = null;
    $.ajax(
        {
            type: 'GET',
            url: url,
            data: {
            },
            dataType: 'json',
            async: false,
            //waits to return value
            cache: false,
            success: function(data)
            {

                result = data;
            }
        });
    return result;
}

var idleInterval
var idleTime = 0;
$(document).ready(function () {
    //Increment the idle time counter every minute.
    idleInterval = setInterval(timerIncrement, 60000); // 1 minute

    //Zero the idle timer on mouse movement.
    $(this).mousemove(function (e) {
        idleTime = 0;
    });
    $(this).keypress(function (e) {
        idleTime = 0;
    });
});

function timerIncrement() {
    idleTime = idleTime + 1;
    idleInterval = setInterval(timerIncrement, 10000)
    if (idleTime > 4) { // 5 minutes
        window.location = "../../index.html"
    }
}