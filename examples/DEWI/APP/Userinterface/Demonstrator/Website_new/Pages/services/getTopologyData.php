<?php

include "getDatabaseAccess.php";


$myDB = new database($host, $user, $password, $database);

$myDB->connectDB();

$id = 0;
$db_array = array();


$sql = "select * from Dewi_tab order by insert_order"; //Sort out in the order of insertion

$result = $myDB->query($sql);

$lengths = $result->num_rows;

if ($lengths > 0) {
    while ($row = $result->fetch_assoc()) {
        $db_array[] = $row;
    }

}

//udapte all nodes id in the datatable
for ($y = 0; $y < $lengths; $y++) {
    $sql = "UPDATE Dewi_tab SET id = " . $id . " WHERE addr = '" . $db_array[$y]["addr"] . "'"; //Sort out in the order of insertion
    $result = $myDB->query($sql);

    $id = $id + 1;
}


$grpNumber = 0;
$db_array = array();
$sql = "select * from Dewi_tab order by insert_order";
$result = $myDB->query($sql);
$lengths = $result->num_rows;

if ($lengths > 0) {
    while ($row = $result->fetch_assoc()) {
        $db_array[] = $row;
    }

}
$db_array2 = array();
$db_array2 = $db_array;
$prec_1 = $db_array[0]["addr"];
for ($x = 0; $x < $lengths; $x++) {
    if($db_array2[$x]["target"] !== null AND $db_array2[$x]["status"] === '0') {

        $sql = "UPDATE Dewi_tab as t1 INNER JOIN (SELECT colour FROM Dewi_tab WHERE id=".$db_array[$x]["target"] . ") AS t2 SET t1.colour = t2.colour WHERE t1.status='0' AND t1.addr='".$db_array[$x]["addr"]."'" ;

        $result = $myDB->query($sql);

    }
    for ($y = 0; $y < $lengths; $y++) {
        if (strcmp($db_array[$x]["addr"], $db_array2[$y]["a_target"]) == 0 && $db_array[$x]["id"] != null) {

            if (strcmp($db_array[$x]["addr"], $prec_1) != 0) {
                $grpNumber = $grpNumber + 1;
                $prec_1 = $db_array[$x]["addr"];
            }


            $sql = "UPDATE Dewi_tab SET status = false, target =" . $db_array[$x]["id"] . ", NGrp =" . $grpNumber . " WHERE id = " . $db_array2[$y]["id"];
            $result = $myDB->query($sql);


            $sql = "UPDATE Dewi_tab SET status = true, NGrp =" . $grpNumber . ", name = 'Master" . $grpNumber . "' WHERE addr = '" . $db_array[$x]["addr"] . "'";
            $result = $myDB->query($sql);

        }
    }
}

$slave_number = 1;
$db_array = array();
$sql = "select * from Dewi_tab order by insert_order";
$result = $myDB->query($sql);
$lengths = $result->num_rows;

if ($lengths > 0) {
    while ($row = $result->fetch_assoc()) {
        $db_array[] = $row;
    }

}

//update slave name in dewi_tab
for ($y = 0; $y < $lengths; $y++) {
    if ($db_array[$y]["status"] == 0) {
        $sql = "UPDATE Dewi_tab SET name = 'Slave" . $slave_number . "' WHERE addr = '" . $db_array[$y]["addr"] . "'";
        $myDB->query($sql);
        $slave_number = $slave_number + 1;
    }
}


$db_array = array();
$sql = "select * from Dewi_tab order by insert_order";
$result = $myDB->query($sql);
$lengths = $result->num_rows;

if ($lengths > 0) {
    while ($row = $result->fetch_assoc()) {
        $db_array[] = $row;
    }

}

//erase  datatable of nodes's links
$sql = "truncate Dewi_linkTab";
$myDB->query($sql);
for ($x = 0; $x < $lengths; $x++) {            //update  datatable of nodes's links

    if ($db_array[$x]["id"] != null && $db_array[$x]["target"] != null) {
        $sql = "INSERT INTO Dewi_linkTab (source, target)values(" . $db_array[$x]["id"] . ", " . $db_array[$x]["target"] . ")";
        $myDB->query($sql);
    }
}



$sql = "select  a.*, sum(b.packets) as pkt_sum, (sum(b.packets*b.latency)/sum(b.packets)) as avg_lat from Dewi_tab a left outer join Dewi_statsTab b on a.addr=b.addr group by a.addr order by a.id";
$db_array = array();

$result = $myDB->query($sql);
if ($lengths > 0) {
    while ($row = $result->fetch_assoc()) {
        $db_array[] = $row;
    }

}

$finalresult = "{\"data\" :\r\n". json_encode($db_array, JSON_NUMERIC_CHECK).",\r\n\"nodes\" :\r\n". json_encode($db_array, JSON_NUMERIC_CHECK);





$sql = "select * from Dewi_linkTab";
$db_array = array();

$result = $myDB->query($sql);
if ($lengths > 0) {
    while ($row = $result->fetch_assoc()) {
        $db_array[] = $row;
    }

}
$finalresult = $finalresult .",\r\n\"links\":\r\n". json_encode($db_array, JSON_NUMERIC_CHECK)."\r\n}";
echo json_encode($finalresult);


//
////echo json_encode("\r\n");
////echo json_encode($db_array);
//$stringy = json_encode($db_array, JSON_NUMERIC_CHECK);


?>