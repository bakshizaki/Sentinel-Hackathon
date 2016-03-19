<?php
include_once"config.php";

    if(isset($_GET['date'])){
        $date=$_GET['date'];
        //        echo $date.$m;
   }
    else{
        $date=date("Y-m-d");
    }

    $sql="SELECT * FROM m3 WHERE (from_unixtime(time,'%Y-%m-%d'))='$date'";
    $query=$pdo->prepare($sql);

    $query->execute();
    $result=$query->fetchAll();
    echo json_encode($result);





?>