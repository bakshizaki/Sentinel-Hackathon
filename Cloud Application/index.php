<?php 
include_once"config.php";
session_start();

$sql="select auth from auth where id='1'";

$query=$pdo->prepare($sql);

$query->execute();
$result=$query->fetch(PDO::FETCH_ASSOC);

if(isset($_POST)){
    if($_POST['pass']==$result['auth'])
    {
	$_SESSION['id']="chitale";
	header("location:hackerearth.php");
	
    }
    else
	header("location:index.html");
}
else
header("location:index.html");

?>
