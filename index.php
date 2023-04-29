<!--
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

-->

<?php 

//log-in to DBserver
require_once "config.php";

$sql = "SELECT id, temperature, humidity, pressure, sendtime FROM sim7000Share order by sendtime desc limit 24";

$result = $conn->query($sql);

while ($data = $result->fetch_assoc()){
    $sensor_data[] = $data;
}

$readings_time = array_column($sensor_data, 'sendtime');

// ******* Uncomment to convert readings time array to your timezone ********
$i = 0;
foreach ($readings_time as $reading){
    // Uncomment to set timezone to - 1 hour (you can change 1 to any number)
    //$readings_time[$i] = date("Y-m-d H:i:s", strtotime("$reading - 1 hours"));
    // Uncomment to set timezone to + 4 hours (you can change 4 to any number)
    $readings_time[$i] = date("Y-m-d H:i:s", strtotime("$reading + 1 hours"));
    $i += 1;
}

$temperature = json_encode(array_reverse(array_column($sensor_data, 'temperature')), JSON_NUMERIC_CHECK);
$relative_humidity = json_encode(array_reverse(array_column($sensor_data, 'humidity')), JSON_NUMERIC_CHECK);
$pressure = json_encode(array_reverse(array_column($sensor_data, 'pressure')), JSON_NUMERIC_CHECK);
$reading_time = json_encode(array_reverse($sendtime), JSON_NUMERIC_CHECK);

/*echo $temperature;
echo $relative_humidity;
echo $pressure;
echo $reading_time;*/

$result->free();
$conn->close(); 

?>

<!DOCTYPE html>
<html>
<meta name="viewport" content="width=device-width, initial-scale=1">
  <script src="https://code.highcharts.com/highcharts.js"></script>
  <link rel="stylesheet" href="extract_style.css">
  <style>
    body {
      min-width: 310px;
    	max-width: 1280px;
    	height: 500px;
      margin: 0 auto;
    }
    h2 {
      font-family: Arial;
      font-size: 2.5rem;
      text-align: center;
    }
  </style>
  <body>
  <?php
  //connect with DBserver
  require_once "config.php";

$sql = "SELECT id, temperature, humidity, pressure, sendtime FROM sim7000Share order by sendtime desc limit 24";

if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $row_reading_time = $row["sendtime"];
        $row_temperature = $row["temperature"];
        $row_relative_humidity = $row["humidity"]; 
        $row_pressure = $row["pressure"]; 
        
        // Uncomment to set timezone to - 1 hour (you can change 1 to any number)
        //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time - 1 hours"));
      
        // Uncomment to set timezone to + 4 hours (you can change 4 to any number)
        $row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time + 1 hours"));
      
        echo '<tr> 
                <td>' . $row_reading_time . '</td> 
                <td style="text-align: center">' . $row_temperature . '</td> 
                <td style="text-align: center">' . $row_relative_humidity . '</td>
                <td style="text-align: center">' . $row_pressure . '</td> 
              </tr>';
    }
    $result->free();
}

$conn->close();

echo '<h2>Weather Station</h2>';

echo '<center><b>Location Name: </b> LAFIA, Nigeria <br><b>Latitude:</b> 8.234 °N<br><b>Longitude: </b> 8.534 °E</center>';
?>

<h2><center>Data Extraction</center></h2>
<div>
	<table border="1" class="excenter">
		<thead>
			<th>Sendtime</th>
			<th style="text-align: center">Temperature<br> (<sup>o</sup>C)</th>
			<th style="text-align: center">Relative Humidity<br> (%)</th>
			<th style="text-align: center">DewPoint<br> (<sup>o</sup>C)</th>
			<th style="text-align: center">Pressure<br> (hPa)</th>
		</thead>
		<tbody>
<?php
			include('config.php');
			//MySQLi Object-oriented
			$query=$conn->query("SELECT * FROM sim7000Share ORDER BY sendtime DESC LIMIT 12");
			while($row = $query->fetch_array()) { 
				?>
				<tr>
					<td><?php echo $row['sendtime']; ?></td>
					<td style="text-align: center"><?php echo $row['temperature']; ?></td>
					<td style="text-align: center"><?php echo $row['humidity']; ?></td>
					<td style="text-align: center"><?php echo $row['dewpoint']; ?></td>
					<td style="text-align: center"><?php echo $row['pressure']; ?></td>
				</tr>
				<?php 
			} 
		?>
		</tbody>
	</table>
</div>
<h2>Export Data Between Selected Dates</h2>
   <p> <center>Using the form below, data can be downloaded between two selected dates, in comma separated file  (.csv) format. <br>
    The data is available from 21st of March, 2023 till present!</center></p>
<div class="form-center">
    <form method="POST" action="download.php">
		<label>From: </label><input type="date" name="from_date">
		<label>To: </label><input type="date" name="to_date">
		<input type="submit" value="Export Data" name="submit">
	</form>
</div>

<br>
    <div id="chart-temperature" class="container"></div><br><br>
    <div id="chart-humidity" class="container"></div><br><br>
    <div id="chart-pressure" class="container"></div>
<script>

var temperature = <?php echo $temperature; ?>;
var relative_humidity = <?php echo $relative_humidity; ?>;
var pressure = <?php echo $pressure; ?>;
var reading_time = <?php echo $reading_time; ?>;

var chartT = new Highcharts.Chart({
  chart:{ renderTo : 'chart-temperature' },
  title: { text: ' Surface Ambient Temperature (°C)' },
  series: [{
    showInLegend: false,
    data: temperature
  }],
  plotOptions: {
    line: { animation: false,
      dataLabels: { enabled: false }
    },
    series: { color: '#059e8a' }
  },
  xAxis: { 
    type: 'datetime',
    categories: reading_time
  },
  yAxis: {
    title: { text: 'Temperature (°C)' }
    //title: { text: 'Temperature (Fahrenheit)' }
  },
  credits: { enabled: false }
});

var chartH = new Highcharts.Chart({
  chart:{ renderTo:'chart-humidity' },
  title: { text: 'Surface Relative Humidity (%)' },
  series: [{
    showInLegend: false,
    data: relative_humidity
  }],
  plotOptions: {
    line: { animation: false,
      dataLabels: { enabled: false }
    }
  },
  xAxis: {
    type: 'datetime',
    dateTimeLabelFormats: {hour:'%H:%M'},
    categories: reading_time
  },
  yAxis: {
    title: { text: 'Relative Humidity (%)' }
  },
  credits: { enabled: false }
});


var chartP = new Highcharts.Chart({
  chart:{ renderTo:'chart-pressure' },
  title: { text: 'Surface Atmospheric Pressure (hPa)' },
  series: [{
    showInLegend: false,
    data: pressure
  }],
  plotOptions: {
    line: { animation: false,
      dataLabels: { enabled: false }
    },
    series: { color: '#18009c' }
  },
  xAxis: {
    type: 'datetime',
    categories: reading_time
  },
  yAxis: {
    title: { text: 'Pressure (hPa)' }
  },
  credits: { enabled: false }
});

</script>
</body>
</html>

