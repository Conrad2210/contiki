<?php
	//open connection to mysql DEWI_database
	$connection = mysqli_connect("localhost", "DEWI", "dewiPass", "DEWI_database")
				or die("Error ".mysqli_error($connection));

	$slave_number = 1;
	$db_array = array();
	$sql = "select * from Dewi_tab order by insert_order";

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

	//update slave name in dewi_tab
	for($y = 0; $y < $lenght; $y++){
		if($db_array[$y]["status"]==0){
			mysqli_query($connection, "UPDATE Dewi_tab SET name = 'Slave".$slave_number."' WHERE addr = '".$db_array[$y]["addr"]."'")
				or die ("Error in Selecting 3".mysqli_error($connection));
			$slave_number =$slave_number +1;
		}
	}

	//close the db connection 
	mysqli_close($connection);	
	
?>