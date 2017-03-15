<?php
	//open connection to mysql DEWI_database
	$connection = mysqli_connect("localhost", "DEWI", "dewiPass", "DEWI_database")
				or die("Error ".mysqli_error($connection));

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

//erase  datatable of nodes's links 
	mysqli_query($connection, "truncate Dewi_linkTab")
		or die ("Error in Selecting 4".mysqli_error($connection));
	
	for($x =0; $x<$lenght; $x++){			//update  datatable of nodes's links 
		if($db_array[$x]["id"]!=null&&$db_array[$x]["target"]!=null){
			mysqli_query($connection, "INSERT INTO Dewi_linkTab (source, target)values(".$db_array[$x]["id"].", ".$db_array[$x]["target"].")") 
				or die ("Error in Selecting 4".mysqli_error($connection));
		}
	}
	//close the db connection 
	mysqli_close($connection);	
	
?>