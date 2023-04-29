<?php
$servername = "unclephysics.com.ng";

// REPLACE with your Database name
$dbname = "unclephy_sim7000share";
// REPLACE with Database user
$username = "unclephy_sim7000";
// REPLACE with Database user password
$password = "BobJohnSon2841";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 
?>