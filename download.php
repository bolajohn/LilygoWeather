<?php
include "config.php";
$filename = 'Filename_'.time().'.csv';

// POST values
$from_date = $_POST['from_date'];
$to_date = $_POST['to_date'];

// Select query
$query = "SELECT * FROM tableName ORDER BY id asc";

if(isset($_POST['from_date']) && isset($_POST['to_date'])){
	$query = "SELECT * FROM tableName where sendtime between '".$from_date."' and '".$to_date."' ORDER BY id asc";
}

$result = mysqli_query($conn,$query);
$weather_arr = array();

// file creation
$file = fopen($filename,"w");

// Header row - Remove this code if you don't want a header row in the export file.
$weather_arr = array("Date and Time","Temperature (oC)","Relative Humidity (%)","Dewpoint Temperature (oC)","Pressure (hPa)"); 
fputcsv($file,$weather_arr);   
while($row = mysqli_fetch_assoc($result)){
    $sendtime = $row['sendtime'];
    $temperature = $row['temperature'];
    $humidity = $row['humidity'];
    $dewpoint = $row['dewpoint'];
    $pressure = $row['pressure'];

    // Write to file 
    $weather_arr = array($sendtime,$temperature,$humidity,$dewpoint,$pressure);
    fputcsv($file,$weather_arr);   
}

fclose($file); 

// download
header("Content-Description: File Transfer");
header("Content-Disposition: attachment; filename=$filename");
header("Content-Type: application/csv; "); 

readfile($filename);

// deleting file
unlink($filename);
exit();
