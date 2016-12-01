<?php
	//open connection to mysql DEWI_database
	$connection = mysqli_connect("localhost", "DEWI", "dewiPass", "DEWI_database")
				or die("Error ".mysqli_error($connection));

	$grpNumber = 0;
	$sql = "select * from Dewi_tab order by insert_order";
	//open connection to mysql DEWI_database
	$connection = mysqli_connect("localhost", "DEWI", "dewiPass", "DEWI_database")
				or die("Error ".mysqli_error($connection));
	//update $result 
	$db_array = array();
	$result = mysqli_query($connection, $sql) or die ("Error in Selecting "
		.mysqli_error($connection));
	
	$lenght = mysqli_num_rows($result);
	if($lenght>0){
		while($row = mysqli_fetch_assoc($result))
		{
			$db_array[] = $row;
		}
	}


	//update target master node name and nodes group in Dewi_tab
	$db_array2 = array();
	$db_array2 = $db_array;
	$prec_1 = $db_array[0]["addr"];
	for($x = 0; $x < $lenght; $x++){
		for($y = 0; $y < $lenght; $y++){
			if(strcmp($db_array[$x]["addr"], $db_array2[$y]["a_target"])==0	&& $db_array[$x]["id"]!=null){
				if(strcmp($db_array[$x]["addr"], $prec_1)!=0){
					$grpNumber = $grpNumber +1;
					$prec_1 = $db_array[$x]["addr"];
				}
				
				mysqli_query($connection, "UPDATE Dewi_tab SET status = false, target =".$db_array[$x]["id"].", NGrp =".$grpNumber." WHERE id = ".$db_array2[$y]["id"])
						or die ("Error in Selecting 2bis".mysqli_error($connection));
				mysqli_query($connection, "UPDATE Dewi_tab SET status = true, NGrp =".$grpNumber.", name = 'Master".$grpNumber."' WHERE addr = '".$db_array[$x]["addr"]."'")
						or die ("Error in Selecting 2".mysqli_error($connection));	
			}
		}
	}

	//close the db connection 
	mysqli_close($connection);	
	
?>