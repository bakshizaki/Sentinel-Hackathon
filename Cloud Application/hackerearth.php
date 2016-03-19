<?php

session_start();

if(!isset($_SESSION['id'])){
    header("location:index.html");

    
}
?>
<!DOCTYPE HTML>
    <head>
	<script src="../jquery-2.1.4.min.js" rel="text/javascript"></script>

	<script src="jquery.tabletoCSV.js" rel="text/javascript"></script>
	<script src="d3.min.js" rel="text/javascript"></script>
	<script src="monitor.js" rel="text/javascript"></script>
  <link rel="stylesheet" href="jquery-ui.min.css">
   <script src="jquery-ui.min.js"></script>

  <script>


$(function() {
 $.get("dates.php",function(data){

     window.dates=data;
 });

 $("#datepicker").datepicker();
          $("#datepicker").datepicker( "option", "dateFormat", "yy-mm-dd" );



  });

</script>
	<meta charset="UTF-8">
	<link href="../bootstrap-3.3.6-dist/css/bootstrap.min.css" rel="stylesheet"/>
	<link href="../bootstrap-3.3.6-dist/css/bootstrap-theme.min.css" rel="stylesheet"/>

	<link href="dis.css" rel="stylesheet"/>
	
    </head>
    <body>
    <div id="hdr">
    
    <p>Sensor Monitor</p>
    </div>
    <div class="container-fluid" id="top">
	<div class="col-md-2">

    <input type="text" id="datepicker" placeholder=/>

    </div>
    <div class="col-md-1">
	<button id="export" data-export="export" class="btn btn-primary btn-lg">Export Data</button>
    </div>
    <div class="col-md-1">
        <h2 id="emp"></h2>
    </div>
    <div class="col-md-1">
	<div id="count">
	    <p>Total <br>Cuts</p>
	    <div id="cnt"></div>
	</div>
    </div>
    <!-- <div class="col-md-1">
	 <div id="ralert">
	 <p>Red Alerts </p>
	 <div id="ralr"></div>

	 </div>
	 </div> -->
    <div class="col-md-1">
	<div id="rc">
	    <p>Red Count</p>
	    <div id="rcc"></div>
	</div>
    </div>
    <div class="col-md-1">
	<div id="oalert">
	    <p>Orange Count</p>
	    <div id="oalr"></div>
	</div>
    </div>
    <div class="col-md-1">
	<div id="galert">
	    <p>Green Count</p>
	    <div id="galr"></div>
	</div>
    </div>
    <div class="col-md-1">
	<div id="rj">
	    <p>Rejection Percent</p>
	    <div id="rjp"></div>
	</div>
    </div>
    <div class="col-md-2">
	<div id="tot">
	    <p>Estimated Total Production</p>
	    <div id="t"></div>
	</div>
    </div>
    </div>
    
    <div class="container-fluid">
    <div class="row">

<div class="col-md-6" id="pie">
    <h1>Zone Distribution</h1>
    </div>
    <div class="col-md-5" id="ta">
    <table id="data" class="table table-bordered">
 

   <thead><tr><th>Time</th><th>Temperature</th><th>Humidity</th><th>Employee ID</th></tr></thead>
    <tbody>

    </tbody>
    </table>
    </div>
    </div>
    <div id="graph"></div>

    </body>
    </html>
