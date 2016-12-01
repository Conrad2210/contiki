<?php
	//open connection to mysql DEWI_database
	$connection = mysqli_connect("localhost", "DEWI", "dewiPass", "DEWI_database")
				or die("Error ".mysqli_error($connection));

	// fetch nodes table rows from mysql db
	$sql = "select avg(plr) as aplr, avg(latency) as alat from Dewi_statsTab";
	$result = mysqli_query($connection, $sql) or die ("Error in Selecting "
		.myqli_error($connection));

	if(mysqli_num_rows($result)>0){
		while($row = mysqli_fetch_assoc($result))
		{
			echo "Average PLR: ".$row['aplr']." , Average latency: ".$row['alat'];
		}
	}
	
	
	//close the db connection 
	mysqli_close($connection);
?>

