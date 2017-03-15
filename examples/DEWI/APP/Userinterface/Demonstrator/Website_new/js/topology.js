/**
 * Created by R00118979 on 14/03/2017.
 */

$(document).ready(function() {

    var width = 775,  //Dimension(Size) of the window for the topology
        height = 550;

    var data = JSON.parse(retrievData("../services/getTopologyData.php"))
    var svg = d3.select("#graph").append("svg")
        .attr("width", width)
        .attr("height", height);

    var force = d3.layout.force()			//attraction force
        .gravity(0.03)
        .linkDistance(100)
        .charge(-500)
        .size([width, height]);


    force.nodes(data.nodes)						//make link between nodes
        .links(data.links)
        .start();


    var link = svg.selectAll(".link")
        .data(data.links)
        .enter().append("line")
        .attr("class", "link")
        .style("marker-start", "url(#suit)"); 	//arrows

    var node = svg.selectAll(".node")
        .data(data.nodes)
        .enter().append("g")
        .attr("class", "node")
        .call(force.drag)
        .on('dblclick', connectedNodes); //double click on a node for highlighting

    node.append("image")
        .attr("xlink:href", "../../img/images_converted.ico")		//image for each node
        .attr("x", -10)
        .attr("y", -10)
        .attr("width", 23)
        .attr("height", 23);

    node.append("title")									//show when mouse is on a node
        .text(function (d) {
            return 'Group: ' + d.NGrp;
        });

    node.append("text")										//label for each node
        .attr("dx", 15)
        .attr("dy", ".35em")
        .style("font-size", '12px')
        .text(function (d) {
            if(d.colour != null)
                return d.name + ': ' + d.addr + "; Colour: " + d.colour;
            else
                return d.name + ': ' + d.addr;
        });


    force.on("tick", function () {
        link.attr("x1", function (d) {
            return d.source.x;
        }) 		//position of each node
            .attr("y1", function (d) {
                return d.source.y;
            })
            .attr("x2", function (d) {
                return d.target.x;
            })
            .attr("y2", function (d) {
                return d.target.y;
            });

        node.attr("transform", function (d) {
            return "translate(" + d.x + "," + d.y + ")";
        });
    });

//Toggle stores whether the highlighting is on
    var toggle = 0;
//Create an array logging what is connected to what
    var linkedByIndex = {};
    for (i = 0; i < data.nodes.length; i++) {
        linkedByIndex[i + "," + i] = 1;
    }
    ;
    data.links.forEach(function (d) {
        linkedByIndex[d.source.index + "," + d.target.index] = 1;
    });
//This function looks up whether a pair are neighbours
    function neighboring(a, b) {
        return linkedByIndex[a.index + "," + b.index];
    }

    function connectedNodes() {
        if (toggle == 0) {
            //Reduce the opacity of all but the neighbouring nodes
            d = d3.select(this).node().__data__;
            node.style("opacity", function (o) {
                return neighboring(d, o) | neighboring(o, d) ? 1 : 0.1;
            });
            link.style("opacity", function (o) {
                return d.index == o.source.index | d.index == o.target.index ? 1 : 0.1;
            });
            //Reduce the op
            toggle = 1;
        } else {
            //Put them back to opacity=1
            node.style("opacity", 1);
            link.style("opacity", 1);
            toggle = 0;
        }
    }

//end code for highlighting

//arrow
    svg.append("defs").selectAll("marker")
        .data(["suit", "licensing", "resolved"])
        .enter().append("marker")
        .attr("id", function (d) {
            return d;
        })
        .attr("viewBox", "0 -5 10 10")
        .attr("refX", -16)
        .attr("refY", 0)
        .attr("markerWidth", 10)
        .attr("markerHeight", 10)
        .attr("orient", "auto")
        .append("path")
        .attr("d", "M10,-5L0,0L10,5")
        .style("stroke", 'black');
//end arrow


// send a topology refresh message to the network and reload the page after 10 seconds

});
function sendTopologyRefresh() {
    socket.send("topologyrefresh\n");
    setTimeout(function () {
        window.location.reload(true);
    }, 10000);
}