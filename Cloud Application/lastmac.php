<?php 
require_once"config.php";
$i='m'.$_GET['i'];
$sql="select * from $i";
$query=$pdo->prepare($sql);
$query->execute();
$result=$query->fetchAll();
$last=end($result);

echo json_encode($last);


?>
