<?php
include(__DIR__.'/phpgraphlib/phpgraphlib.php');

$addr = mysql_escape_string($_GET['address']);

//open connection to mysql DEWI_database
$connection = mysqli_connect("localhost", "DEWI", "dewiPass", "DEWI_database")
			or die("Error ".mysqli_error($connection));

$dataarray = array();

// fetch nodes table rows from mysql db
$sql = "select packets, latency from Dewi_statsTab where addr='$addr' order by latency";
$result = mysqli_query($connection, $sql) or die ("Error in Selecting ".mysqli_error($connection));

if(mysqli_num_rows($result)>0){
$i=0;
$prev_lat = -1;
	while($row = mysqli_fetch_assoc($result))
	{
		// extract the latencies and packet counts		
		// the following looks a bit ugly
		// it is a workaround for the lack of x-axis configurability in phpgraphlib
		$axislabel = $row['latency'];
		if ($prev_lat == -1)
		{
			$prev_lat = $axislabel;
		}
		else
		{
			while($axislabel - $prev_lat > 10)
			{
				$prev_lat += 10;
				$label = "t=".$prev_lat."ms";
				$dataarray[$label] = 0;
				$i++;
			}
		}
		$prev_lat = $axislabel;
		$packets = $row['packets'];
		$label = "t=".$axislabel."ms";
		$dataarray[$label] = $packets;
		$i++;
	}
}
	
//close the db connection 
mysqli_close($connection);

// create the plot
$graph = new PHPGraphLib(650,450);
$graph->addData($dataarray);
$graph->setBarColor('255, 255, 204');
$graph->setupXAxis(15);
$graph->setGrid(false);
$graph->createGraph();
?>
