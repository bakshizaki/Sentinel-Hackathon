<?php
include_once "config.php";
if(isset($_GET)){
$id=$_GET['id'];
$sql="truncate $id";
$query=$pdo->prepare($sql);
$result=$query->execute();
echo $result;
}

?>