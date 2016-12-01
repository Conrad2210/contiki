<?php
	//open connection to mysql DEWI_database
	$connection = mysqli_connect("localhost", "DEWI", "dewiPass", "DEWI_database")
				or die("Error ".mysqli_error($connection));

	// fetch nodes table rows from mysql db
	$sql = "select * from Dewi_statsTab order by id";
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
	unlink('statJson.json');
	//write to json file 
	$fp = fopen('statJson.json', 'w');
	fwrite($fp, "{\"data\" :\r\n");
	$stringy = json_encode($db_array, JSON_NUMERIC_CHECK);
	fwrite($fp, $stringy);
	fwrite($fp, "\r\n}");
	fclose($fp);
?>

