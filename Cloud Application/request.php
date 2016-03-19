<?php 

include_once"config.php";
if(isset($_GET)){

    if(isset($_GET['i'])){
        $id=$_GET['i'];
        $v=1;
    }
    else{
        $id=3;
        $v=2;
    }

    $sql1="select * from m$id";
    $query1=$pdo->prepare($sql1);
    $query1->execute();
    $result1=end($query1->fetchAll());

    $count=($_GET['c'])?$_GET['c']:'NULL';
    $hum=($_GET['h'])?$_GET['h']:'NULL';
    $tmp=($_GET['t'])?$_GET['t']:'NULL';
    $time=time();
    $oa=($_GET['oa']);//?$_GET['oa']:'NULL';
    $ra=($_GET['ra']);//?$_GET['ra']:'NULL';
    $ga=($_GET['ga']);//?$_GET['ga']:'NULL';
    $oac=($_GET['oac']);//?$_GET['oa']:'NULL';
    $rac=($_GET['rac']);//?$_GET['ra']:'NULL';
    $gac=($_GET['gac']);//?$_GET['ga']:'NULL';
    $eid=($_GET['eid']);//?$_GET['ga']:'NULL';
    $mid='m'.$id;

    if($hum=='null' || $hum==0)
    {



        $hum=$result1->hum;

    }

    if($tmp=='null' || $tmp==0)
    {



        echo "1";

        $tmp=$result1->temp;

    }
    if($eid=="NULL" || $eid==0)
    {



        echo "1";

        $eid=$result1->eid;

    }

    echo "1";



    $sql="insert into $mid(count,hum,temp,time,ra,oa,ga,rac,oac,gac,eid) values('$count',:hum,:tmp,:time,:ra,:oa,:ga,:rac,:oac,:gac,:eid)";


    $query=$pdo->prepare($sql);

    $query->bindValue(':ra',$ra,PDO::PARAM_NULL);

    $query->bindValue(':oa',$oa,PDO::PARAM_NULL);

    $query->bindValue(':ga',$ga,PDO::PARAM_NULL);

    $query->bindValue(':rac',$rac,PDO::PARAM_NULL);

    $query->bindValue(':oac',$oac,PDO::PARAM_NULL);

    $query->bindValue(':gac',$gac,PDO::PARAM_NULL);

    $query->bindValue(':hum',$hum,PDO::PARAM_NULL);

    $query->bindValue(':tmp',$tmp,PDO::PARAM_NULL);

    $query->bindValue(':time',$time,PDO::PARAM_NULL);
    $query->bindValue(':eid',$eid,PDO::PARAM_NULL);

    $result=$query->execute();

    if($result){

        echo $v;

    }
    else{


        echo"0";

    }

}
?>
