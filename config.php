<?php
$servername = "your.server.name.here.com";

// REPLACE with your Database name
$dbname = "database name here";
// REPLACE with Database user
$username = "database username here";
// REPLACE with Database user password
$password = "database password here";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 
?>
