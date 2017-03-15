/**
 * Created by R00118979 on 13/03/2017.
 */
$(document).ready(function() {

    $("#brightnessSlider")
        .slider({
            max: 100,
            min: 0,
            value: 50,
            step: 1,
            range: "min",
            orientation: "vertical",
            change: function (event, ui) {
                var selection = $("#brightnessSlider").slider("option", "value");
                selection = parseInt(selection / 10)
                socket.send("BRIGHTNESS " + selection + "\n");
            },
            slide: function (event, ui) {
                var selection = $("#brightnessSlider").slider("option", "value");
                selection = parseInt(selection / 10)
                socket.send("BRIGHTNESS " + selection + "\n");
            }
        });

    $("#brightnessSlider").slider().slider("pips");

    var standalone = window.navigator.standalone,  				//
        IOS_userAgent = window.navigator.userAgent.toLowerCase(),	// to make this webpage works on IOS navigator
        safari = /safari/.test(IOS_userAgent),					//
        ios = /iphone|ipod|ipad/.test(IOS_userAgent);				//


    var canvas = document.getElementById('colour_picker');
    var ctx = canvas.getContext('2d');

    var img = new Image();
    img.src = '../../img/colours.jpg';

    $(img).load(function () {
        ctx.drawImage(img, 0, 0);
    });

    var lasttimestamp = 0;
    if ((navigator.userAgent.indexOf("Chrome") || navigator.userAgent.indexOf("CriOS")) != -1) {
        $('#colour_picker').on('touchstart touchmove', function (event) {
            event.preventDefault();
            //console.log(event.changedTouches[0]);
            var x = event.originalEvent.targetTouches[0].pageX - this.offsetLeft;
            var y = event.originalEvent.targetTouches[0].pageY - this.offsetTop;
            var R, G, B, rgb, img_data;
            var hex;
            //console.log(this.offsetTop);
            img_data = ctx.getImageData(x, y, canvas.width, canvas.height);
            if (event.timeStamp - lasttimestamp > 100) {
                lasttimestamp = event.timeStamp;
                R = img_data[0];
                G = img_data[1];
                B = img_data[2];
                rgb = R + ',' + G + ',' + B;
                hex = rgbToHex(R, G, B);
                socket.send(hex + "\n");

            }
            event.preventDefault();
        });
        $('#colour_picker').on('mousemove', function (event) {
            event.preventDefault();
            //console.log(event.changedTouches[0]);
            var x = event.pageX - this.offsetLeft;
            var y = event.pageY - this.offsetTop;
            var R, G, B, rgb, img_data;
            var hex;
            //console.log(this.offsetTop);
            img_data = ctx.getImageData(x, y, canvas.width, canvas.height).data;
            if (event.timeStamp - lasttimestamp > 100) {
                lasttimestamp = event.timeStamp;
                R = img_data[0];
                G = img_data[1];
                B = img_data[2];
                rgb = R + ', ' + G + ', ' + B;
                hex = rgbToHex(R, G, B);
                socket.send(hex + "\n");
            }

            event.preventDefault();
        });
    }
    else if (navigator.userAgent.indexOf("Firefox") != -1) { //colour choice event for firefox navigator
        $('#colour_picker').on('mousemove', function (event) {
            event.preventDefault();
            //console.log(event.changedTouches[0]);
            var x = event.pageX - this.offsetLeft;
            var y = event.pageY - this.offsetTop;
            var R, G, B, rgb, img_data;
            var hex;

            //console.log(this.offsetTop);
            img_data = ctx.getImageData(x, y, canvas.width, canvas.height).data;
            if (event.timeStamp - lasttimestamp > 100) {
                console.log("dosomething")
                lasttimestamp = event.timeStamp;
                R = img_data[0];
                G = img_data[1];
                B = img_data[2];
                rgb = R + ', ' + G + ', ' + B;
                hex = rgbToHex(R, G, B);
                socket.send(hex + "\n");
            }
            event.preventDefault();
        });
        $('#colour_picker').on('touchstart touchmove', function (event) {
            event.preventDefault();
            //console.log(event.changedTouches[0]);
            var x = event.originalEvent.targetTouches[0].pageX - this.offsetLeft;
            var y = event.originalEvent.targetTouches[0].pageY - this.offsetTop;
            var R, G, B, rgb, img_data;
            var hex;
            //console.log(this.offsetTop);
            img_data = ctx.getImageData(x, y, canvas.width, canvas.height).data;
            if (event.timeStamp - lasttimestamp > 100) {
                lasttimestamp = event.timeStamp;
                R = img_data[0];
                G = img_data[1];
                B = img_data[2];
                rgb = R + ',' + G + ',' + B;
                hex = rgbToHex(R, G, B);
                socket.send(hex + "\n");
            }
            event.preventDefault();
        });
    } else if (ios) {  //colour choice event for IOS navigator
        if (!standalone && safari) {
            //browser
            $('#colour_picker').on('mousemove', function (event) {
                event.preventDefault();
                //console.log(event.changedTouches[0]);
                var x = event.pageX - this.offsetLeft;
                var y = event.pageY - this.offsetTop;
                var R, G, B, rgb, img_data;
                var hex;

                img_data = ctx.getImageData(x, y, canvas.width, canvas.height).data;
                if (event.timeStamp - lasttimestamp > 100) {
                    lasttimestamp = event.timeStamp;
                    R = img_data[0];
                    G = img_data[1];
                    B = img_data[2];
                    rgb = R + ', ' + G + ', ' + B;
                    hex = rgbToHex(R, G, B);
                    socket.send(hex + "\n");
                }
                event.preventDefault();
            });
            $('#colour_picker').on('touchstart touchmove', function (event) {
                event.preventDefault();
                //console.log(event.changedTouches[0]);
                var x = event.originalEvent.targetTouches[0].pageX - this.offsetLeft;
                var y = event.originalEvent.targetTouches[0].pageY - this.offsetTop;
                var R, G, B, rgb, img_data;
                var hex;
                //console.log(this.offsetTop);
                img_data = ctx.getImageData(x, y, canvas.width, canvas.height).data;
                if (event.timeStamp - lasttimestamp > 100) {
                    lasttimestamp = event.timeStamp;
                    R = img_data[0];
                    G = img_data[1];
                    B = img_data[2];
                    rgb = R + ',' + G + ',' + B;
                    hex = rgbToHex(R, G, B);
                    socket.send(hex + "\n");
                }
                event.preventDefault();
            });
        }
        ;

    } else {
        console.log("ERROR: Browse in FF or Chrome or IOS_Safari jejej");
    }

//function rgbToHex(R,G,B) {return toHex(R)+toHex(G)+toHex(B)}			//use image color.png  the dark colours chosen on the webpage are not the same showed by the nodes
    function rgbToHex(R, G, B) {
        return toHex(R / 3) + toHex(G / 3) + toHex(B / 3)
    }  //use image color1.png   because with a same coordinate, the colour chosen on the webpage is more dark than the nodes display
//so we need to reduce the coordinate value, in order that the light colour of the nodes is more similar to the webpage colour

    function rgbToHexfordisplay(R, G, B) {
        return toHex(R & 0b11111000) + toHex(G & 0b11111000) + toHex(B & 0b11111000)
    }


    function toHex(n) {
        n = parseInt(n, 10);
        if (isNaN(n)) return "00";
        n = Math.max(0, Math.min(n, 255));
        return "0123456789ABCDEF".charAt((n - n % 16) / 16) + "0123456789ABCDEF".charAt(n % 16);
    }


    /*WEBSOCKET SERVER*/

    function setup() {
        socket.onopen = openSocket;
    }

// Change text to push up message or similar
    function openSocket() {
        socket.send("Hello server");
        console.log("Connected to wsServer");
        // alert("Connection Established");
    }

})