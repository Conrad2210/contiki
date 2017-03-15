<?php
	//open connection to mysql DEWI_database
	$connection = mysqli_connect("localhost", "DEWI", "dewiPass", "DEWI_database")
				or die("Error ".mysqli_error($connection));

	$db_array = array();
	// fetch nodes table rows from mysql db
	$sql = "select * from Dewi_tab order by insert_order";
	$result = mysqli_query($connection, $sql) or die ("Error in Selecting "
		.mysqli_error($connection));
	
	$lenght = mysqli_num_rows($result);

	if($lenght>0){
		while($row = mysqli_fetch_assoc($result))
		{
			$db_array[] = $row;
		}
	}

		//erase  datatable of nodes
	mysqli_query($connection, "truncate Dewi_tab")
		or die ("Error in Selecting 4".mysqli_error($connection));
	
	
	//close the db connection 
	mysqli_close($connection);	
	
?>