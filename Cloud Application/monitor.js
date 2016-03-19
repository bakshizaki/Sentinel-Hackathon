window.onload=function(){
    var margin = 30,
	width = 650,
	height = 450;
    var lid=0;
    window.dat = new Date().toJSON().slice(0,10);
    $("#datepicker").val(dat);

    $('#datepicker').change(function(e){
	$("#data").find("tr:gt(0)").remove();

	window.dat =$("#datepicker").val();
	// dat=dat.replace("/","-");
	// 	dat=dat.replace("/","-");
	$("#cnt").html("<p>"+"</p>");
	$("#ralr").html("<p>"+"</p>");
	$("#rcc").html("<p>"+"</p>");
	$("#oalr").html("<p>"+"</p>");
	$("#galr").html("<p>"+"</p>");
	$("#t").html("<p>"+"</p>");
	$("#rjp").html("<p>"+"</p>");	

	$.ajax({
	    url: 'stats.php',
	    type: "GET",
	    dataType: "json",
	    data: {'date':dat
		  }
	    ,
	    success: function(data, textStatus, jqXHR) {

		if($.isEmptyObject(data)){
		    alert("No data recorded for this day. Choose other date");
		}
		d3.select("svg").remove();
		drawTable(data);

	    }
	});
	function drawpie(co,cr,cg){
	    var w = 400;
	    var h = 400;
	    var r = h/2;

	    var color = d3.scale.ordinal()
		.range(['#fa0','#884444','#448844']);

	    var to=co+cr+cg;

	    var data = [{"label":(co*100/to).toFixed(1), "value":co},
			{"label":(cr*100/to).toFixed(0), "value":cr},
			{"label":(cg*100/to).toFixed(1), "value":cg}];


	    var vis = d3.select('#pie').append("svg:svg").data([data]).attr("width", w).attr("height", h).append("svg:g").attr("transform", "translate(" + r + "," + r + ")");
	    var pie = d3.layout.pie().value(function(d){return d.value;});

	    // declare an arc generator function
	    var arc = d3.svg.arc().outerRadius(r);

	    // select paths, use arc generator to draw
	    var arcs = vis.selectAll("g.slice").data(pie).enter().append("svg:g").attr("class", "slice");
	    arcs.append("svg:path")
		.attr("fill", function(d, i){
		    return color(i);
		})
		.attr("d", function (d) {
		    // log the result of the arc generator to show how cool it is :)
		    console.log(arc(d));
		    return arc(d);
		});

	    // add the text
	    arcs.append("svg:text").attr("transform", function(d){
		d.innerRadius = 0;
		d.outerRadius = r;
		return "translate(" + arc.centroid(d) + ")";}).attr("text-anchor", "middle").text( function(d, i) {
		    return data[i].label;}
												 );
	};
	function drawTable(data) {


	    //	    start=data.id[0];
var	    start_val=data[0].id;


	    function relate(element,index,array){

		data[index].id-=start_val;

	    };
	    
	    data.forEach(relate); 
	    var id_extent=d3.extent(data, function(d){return +(d.id);});
    	    var y_extent=d3.extent(data, function(d){return +d.hum;});
	    var z_extent=d3.extent(data, function(d){return +d.temp;});
	    var id_scale=d3.scale.linear()
		.range([margin,width-margin])
		.domain(id_extent);
	    var y_scale=d3.scale.linear()
		.range([height-25,30])
		.domain([25,45]);
	    var z_scale=d3.scale.linear()
		.range([height-25,30])
		.domain([25,45]);
	    
	    var x_axis= d3.svg.axis().scale(id_scale);
	    var y_axis= d3.svg.axis().scale(y_scale).orient("left");
	    d3.select("#graph")
		.append("svg")
		.attr("width",width)
		.attr("height",height);
	    
	    d3.select("svg")
		.selectAll("circle.hum")
		.data(data)
		.enter()
		.append("circle")
		.attr("class","hum");

    	    d3.select("svg")
		.selectAll("circle.temp")
		.data(data)
		.enter()
		.append("circle")
		.attr("class","temp");
	    
	    d3.selectAll("circle.hum")
		.attr("cx",function(d){return id_scale(d.id)})
    		.attr("cy",function(d){return y_scale(d.hum)})
		.attr("r",1);
	    
	    d3.selectAll("circle.temp")
		.attr("cx",function(d){return id_scale(d.id)})
    		.attr("cy",function(d){return z_scale(d.temp)})
		.attr("r",1);

	    d3.select("svg")
		.append("g")
		.attr("class", "x axis")
		.attr("transform", "translate(0," + (height-25) + ")")
		.call(x_axis);
	    d3.select("svg")
		.append("g")
		.attr("class", "y axis")
		.attr("transform", "translate(" + margin + ", 0 )")
		.call(y_axis);
	    
	    
	    var lineh = d3.svg.line()
		.x(function(d){return id_scale(d.id)})
		.y(function(d){return y_scale(d.hum)});
	    var linet = d3.svg.line()
		.x(function(d){return id_scale(d.id)})
		.y(function(d){return z_scale(d.temp)});

	    d3.select("svg")
		.append("path")
		.attr("d",lineh(data))
		.attr("class", "humidity");
	    d3.select("svg")
		.append("path")
		.attr("d",linet(data))
		.attr("class", "temperature");
	    
	    var cra=0,coa=0,cga=0,cd=cr=cg=co=0;	
	    for (var i = 0; i <= data.length; i++) {
		if(data[i]==undefined)
		    break;
		drawRow(data[i]);
		window.cr+=+(data[i].ra||0);
		window.cg+=+data[i].ga||0;
		window.co+=+data[i].oa||0;

		coa+=+(data[i].oac||0);
		cra+=+(data[i].rac||0);
		cga+=+(data[i].gac||0);

		cd=cg+co+cr;
		var tot=cd-cr;
		var rjp=(cr/cd*100)||0;
		rjp=rjp.toFixed(2);
		$("#cnt").html("<p><strong>"+cd+"</strong></p>");
		$("#ralr").html("<p><strong>"+cra+"</strong></p>");
		$("#rcc").html("<p><strong>"+cr+"</strong></p>");
		$("#oalr").html("<p><strong>"+co+"</strong></p>");
		$("#galr").html("<p><strong>"+cg+"</strong></p>");
		$("#t").html("<p><strong>"+tot+"</strong></p>");
		$("#rjp").html("<p><strong>"+rjp+"</strong></p>");

		
	    }

	    function drawRow(rowData) {
		var row = $("<tr />");
		var date=new Date(rowData.time*1000);
		var hr=date.getHours();
		var mn=date.getMinutes();
		var sc=date.getSeconds();
              window.lastid=rowData.id+1;
		formtime=(hr<10?"0"+hr:hr)+":"+(mn<10?"0"+mn:mn)+":"+(sc<10?"0"+sc:sc);
		$("#data").prepend(row);
		row.append($("<td>" +formtime + "</td>"));
		row.append($("<td>" + rowData.temp + "</td>"));
	      row.append($("<td>" + rowData.hum + "</td>"));
              		row.append($("<td>" + rowData.eid + "</td>"));

	    }
	    		drawpie(co,cr,cg);
	}


    }).change();

      function drawRow(rowData) {
		var row = $("<tr />");
		var date=new Date(rowData.time*1000);
		var hr=date.getHours();
		var mn=date.getMinutes();
		var sc=date.getSeconds();
              window.lastid=rowData.id+1;
		formtime=(hr<10?"0"+hr:hr)+":"+(mn<10?"0"+mn:mn)+":"+(sc<10?"0"+sc:sc);
		$("#data").prepend(row);
		row.append($("<td>" +formtime + "</td>"));
		row.append($("<td>" + rowData.temp + "</td>"));
	      row.append($("<td>" + rowData.hum + "</td>"));
              		row.append($("<td>" + rowData.eid + "</td>"));
        $("#emp").html(rowData.eid);
	    }
  setInterval(function(){
    $.ajax({url:"lastmac.php",dataType:"json",data:{i:3},success:function(response){
      console.log(response.id);
      if(response.id > lastid){
  drawRow(response);
      }
    }
});
  },2000);

    $("#export").click(function(){

	$("table").tableToCSV();
    });
    $("#del").click(function(){
	$.ajax({
	    url: 'delete.php',
	    type: "GET",
	    dataType: "json",
	    data: {'id':par
		  }
	    ,
	    success: function(data, textStatus, jqXHR) {
		$("#data").find("tr:gt(0)").remove();
		d3.select("svg").remove();

	    }});

    });
  
};
