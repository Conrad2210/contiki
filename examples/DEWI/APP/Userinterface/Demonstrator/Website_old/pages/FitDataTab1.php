<?php
	//open connection to mysql DEWI_database
	$connection = mysqli_connect("localhost", "DEWI", "dewiPass", "DEWI_database")
				or die("Error ".mysqli_error($connection));
	$id = 0;
	$db_array = array();
	

	// fetch nodes table rows from mysql db
	$sql = "select * from Dewi_tab order by insert_order"; //Sort out in the order of insertion
	$result = mysqli_query($connection, $sql) or die ("Error in Selecting "
		.mysqli_error($connection));
	
	$lenght = mysqli_num_rows($result);

	if($lenght>0){
		while($row = mysqli_fetch_assoc($result))
		{
			$db_array[] = $row;
		}
	}//else {
	//	echo '<h2 class="page-header">no connected node</h2>';
	//}

	//udapte all nodes id in the datatable
	for($y = 0; $y < $lenght; $y++){
		mysqli_query($connection, "UPDATE Dewi_tab SET id = ".$id." WHERE addr = '".$db_array[$y]["addr"]."'")  // and add id number
			or die ("Error in Selecting1 ".mysqli_error($connection));;
		$id=$id+1;
	}

	//close the db connection 
	mysqli_close($connection);	

?>
