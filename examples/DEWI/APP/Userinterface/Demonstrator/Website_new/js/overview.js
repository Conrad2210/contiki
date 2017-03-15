/**
 * Created by R00118979 on 14/03/2017.
 */
$(document).ready(function () {

    console.log(window);
    var data = JSON.parse(retrievData("../services/getTopologyData.php"))
    $('#empTable').dataTable({
        "data": data.data,
        "columnDefs": [
            {className: "hover order-column dt-body-center dt-header-center", "targets": [0, 1, 2, 3, 4, 5, 6, 7, 8]}
        ],
        "pageLength": 50,
        "searching": false,
        "columns": [
            {"data": "id"},
            {"data": "name"},
            {"data": "addr"},
            {"data": "NGrp"},
            {"data": "a_target"},
            {"data": "colour"},
            {"data": "battery"},
            {"data": "pkt_sum"},
            {
                "data": "avg_lat"
            }
        ]
    });



})


function sendTopologyRefresh() {
    socket.send("topologyrefresh\n");
    setTimeout(function () {
        window.location.reload(true);
    }, 15000);
}

// refresh the table by reloading
function collectStatistics() {
    window.location.reload(true);
}

// send a statistics reset message to the network and reload the page after 10 seconds
function resetStatistics() {
    socket.send("resetstatistics\n");
    setTimeout(function () {
        window.location.reload(true);
    }, 10000);
}