<?php
include_once"config.php";
$sql="select distinct from_unixtime(time,'%Y-%m-%d') as dates from m3";
$query=$pdo->prepare($sql);
$query->execute();
$result=$query->fetchAll(PDO::FETCH_ASSOC);
$res=array();
foreach($result as $r)
{
foreach($r as $a){
if($a!="dates"){
array_push($res,$a);
}
}
}echo json_encode($res);

?>