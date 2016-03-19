<?php 
ini_set('error_reporting',E_ALL);
define('DB_TYPE', 'mysql');
define('DB_HOST', 'localhost');
define('DB_NAME', 'hackerearth');
define('DB_USER', 'root');
define('DB_PASS', 'root3');
define('DB_CHARSET', 'utf8');


$options = array(PDO::ATTR_DEFAULT_FETCH_MODE => PDO::FETCH_OBJ, PDO::ATTR_ERRMODE => PDO::ERRMODE_WARNING,PDO::ATTR_ORACLE_NULLS=>NULL_NATURAL);
$pdo = new PDO(DB_TYPE . ':host=' . DB_HOST . ';dbname=' . DB_NAME . ';charset=' . DB_CHARSET, DB_USER, DB_PASS, $options);
?>
