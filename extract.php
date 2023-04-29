<!DOCTYPE html>
<html>
<head>
<title> Weather Data</title>
<link rel="stylesheet" href="extract_style.css">
</head>
<body>
<h2><center>Data Extraction</center></h2>
<div>
	<table border="1" class="excenter">
		<thead>
			<th>Sendtime</th>
			<th>Temperature<br> (<sup>o</sup>C)</th>
			<th>Relative Humidity<br> (%)</th>
			<th>DewPoint<br> (<sup>o</sup>C)</th>
			<th>Pressure<br> (hPa)</th>
		</thead>
		<tbody>
<?php
			include('config.php');
			//MySQLi Object-oriented
			$query=$conn->query("SELECT * FROM tableName ORDER BY sendtime DESC LIMIT 12");
			while($row = $query->fetch_array()) { 
				?>
				<tr>
					<td><?php echo $row['sendtime']; ?></td>
					<td><?php echo $row['temperature']; ?></td>
					<td><?php echo $row['humidity']; ?></td>
					<td><?php echo $row['dewpoint']; ?></td>
					<td><?php echo $row['pressure']; ?></td>
				</tr>
				<?php 
			} 
		?>
		</tbody>
	</table>
</div><br>
<div class="textCenter">
    <h2>Export Data Between Selected Dates</h2><br>
   <p> Using the form below, data can be downloaded between two selected dates, in comma separated file  (.csv) format. <br>
    The data is available from 21st of March, 2023 till present!</p>
</div>
<div class="form-center">
    <form method="POST" action="download.php">
		<label>From: </label><input type="date" name="from_date">
		<label>To: </label><input type="date" name="to_date">
		<input type="submit" value="Export Data" name="submit">
	</form>
</div>

</body>
</html>
