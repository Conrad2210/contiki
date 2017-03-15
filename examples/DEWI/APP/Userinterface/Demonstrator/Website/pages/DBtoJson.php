<?php
	//open connection to mysql DEWI_database
	$connection = mysqli_connect("localhost", "DEWI", "dewiPass", "DEWI_database")
				or die("Error ".mysqli_error($connection));

	// fetch nodes table rows from mysql db
//	$sql = "select * from Dewi_tab order by id";
	$sql = "select  a.*, sum(b.packets) as pkt_sum, (sum(b.packets*b.latency)/sum(b.packets)) as avg_lat from Dewi_tab a left outer join Dewi_statsTab b on a.addr=b.addr group by a.addr order by a.id";
	$result = mysqli_query($connection, $sql) or die ("Error in Selecting "
		.myqli_error($connection));

	//create an array
	$db_array = array();
	if(mysqli_num_rows($result)>0){
		while($row = mysqli_fetch_assoc($result))
		{
			$db_array[] = $row;
		}
	}
	
	
	//close the db connection 
	mysqli_close($connection);
	
	
	//echo json_encode($db_array);
	
	//delete old file
	unlink('ntwkJson.json');
	//write to json file 
	$fp = fopen('ntwkJson.json', 'w');
	fwrite($fp, "{\"data\" :\r\n");
	$stringy = json_encode($db_array, JSON_NUMERIC_CHECK);
	fwrite($fp, $stringy);
	fwrite($fp, ",\r\n\"nodes\" :\r\n");
	fwrite($fp, $stringy);
	fwrite($fp, ",\r\n\"links\":\r\n");
	

	//open connection to mysql DEWI_database
	$connection = mysqli_connect("localhost", "DEWI", "dewiPass", "DEWI_database")
				or die("Error ".mysqli_error($connection));

	// fetch links table rows from mysql db
	$sql = "select * from Dewi_linkTab";
	$result = mysqli_query($connection, $sql) or die ("Error in Selecting "
		.myqli_error($connection));

	//create an array
	$db_array = array();
	if(mysqli_num_rows($result)>0){
		while($row = mysqli_fetch_assoc($result))
		{
			$db_array[] = $row;
		}
	}

	//close the db connection 
	mysqli_close($connection);

	//echo json_encode("\r\n");
	//echo json_encode($db_array);
	$stringy = json_encode($db_array, JSON_NUMERIC_CHECK);
	fwrite($fp, $stringy);
	fwrite($fp, "\r\n}");
	fclose($fp);
?>

