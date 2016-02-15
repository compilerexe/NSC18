<!DOCTYPE html>
<html>
<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>NSC 18</title>
	<!-- Fonts -->
    <link href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.4.0/css/font-awesome.min.css" rel='stylesheet' type='text/css'>
    <link href="https://fonts.googleapis.com/css?family=Lato:100,300,400,700" rel='stylesheet' type='text/css'>
	<!-- Latest compiled and minified CSS -->
	<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap.min.css" integrity="sha384-1q8mTJOASx8j1Au+a5WDVnPi2lkFfwwEAa8hDDdjZlpLegxhjVME1fgjWPGmkzs7" crossorigin="anonymous">
	<!-- Optional theme -->
	<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap-theme.min.css" integrity="sha384-fLW2N01lMqjakBkx3l/M9EahuwpSfeNvV63J5ezn3uZzapT0u7EYsXMjQV+0En5r" crossorigin="anonymous">

	<script src="https://ajax.googleapis.com/ajax/libs/jquery/1.12.0/jquery.min.js"></script>

	<style type="text/css">
		.data-sensor {
			background-image: url('bg.png');
		}
	</style>

</head>
<body background="bg.png">
	<nav class="navbar navbar-inverse navbar-static-top">
		<div class="container-fluid">
			<div class="navbar-header">
				<button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#bs-example-navbar-collapse-1" aria-expanded="false">
				<span class="sr-only">Toggle navigation</span>
				<span class="icon-bar"></span>
				<span class="icon-bar"></span>
				<span class="icon-bar"></span>
				</button>
				<a href="/" class="navbar-brand">NSC18</a>
			</div>
			<div class="collapse navbar-collapse" id="bs-example-navbar-collapse-1">
				<ul class="nav navbar-nav">
					<!-- <li class="active"><a href="#">Home <span class="sr-only">(current)</span></a></li> -->
				</ul>
				
				<ul class="nav navbar-nav navbar-right">
				<li><a href="#">Login</a></li>
				<li><a href="{{ url('/register') }}">Register</a></li>
				</ul>
			</div><!-- /.navbar-collapse -->
		</div><!-- /.container-fluid -->
	</nav>
	<div class="container">
		<div class="row">
		<div class="test">
			
		</div>
			
			
			<div class="col-md-12">
				<div class="panel panel-info">
					<div class="panel-heading">
						<b id="time_now">DATE</b>
					</div>
					<div class="panel-body data-sensor">
						<div id="data">
							
						</div>
						<div id="chart">
							<div class="col-md-12">
								<b id=""></b>
							</div>
							<div class="col-md-6">
								<iframe width="450" height="260" style="border: 1px solid #cccccc;" src="http://api.thingspeak.com/channels/78787/charts/1?width=450&height=260&timescale=30&results=1024&dynamic=true&type=spline" ></iframe>
							</div>
							<div class="col-md-6">
								<iframe width="450" height="260" style="border: 1px solid #cccccc;" src="http://api.thingspeak.com/channels/78787/charts/2?width=450&height=260&timescale=30&results=1024&dynamic=true&type=spline" ></iframe>
							</div>
							<div class="col-md-6">
								<iframe width="450" height="260" style="border: 1px solid #cccccc;" src="http://api.thingspeak.com/channels/78787/charts/3?width=450&height=260&results=60&dynamic=true" ></iframe>
							</div>
							<div class="col-md-4" style="margin-top: 15px">
								<div class="panel panel-warning">
								<div class="panel-heading">
									<b id="label_mode" >Mode Intiligent</b>
								</div>
								<div class="panel-body">
									<div class="col-md-6">
										<div class="form-group"><button type="button" id="Intiligent_on" class="btn btn-success" style="width:100%; height:50px;"><span class="glyphicon glyphicon-leaf"></span> <b>On</b></button></div>
									</div>
									<div class="col-md-6">
										<div class="form-group"><button type="button" id="Intiligent_off" class="btn btn-danger" style="width:100%; height:50px;"><span class="glyphicon glyphicon-off"></span> <b>Off</b></button></div>
									</div>
									
								</div>
							</div>
							</div>

							<div class="col-md-2" style="margin-top: 15px">
								<span style="display: block !important; width: 180px; text-align: center; font-family: sans-serif; font-size: 12px;"><a href="http://www.wunderground.com/cgi-bin/findweather/getForecast?query=zmw:00000.1.48327&bannertypeclick=wu_bluestripes" title="Chiang Mai, Thailand Weather Forecast" target="_blank"><img src="http://weathersticker.wunderground.com/weathersticker/cgi-bin/banner/ban/wxBanner?bannertype=wu_bluestripes_metric&airportcode=VTCC&ForcedCity=Chiang Mai&ForcedState=Thailand&wmo=48327&language=EN" alt="Find more about Weather in Chiang Mai, TH" width="160" /></a><br><a href="http://www.wunderground.com/cgi-bin/findweather/getForecast?query=zmw:00000.1.48327&bannertypeclick=wu_bluestripes" title="Get latest Weather Forecast updates" style="font-family: sans-serif; font-size: 12px" target="_blank">Click for weather forecast</a></span>
							</div>


						</div>
					</div>
				</div>
			</div>
			
		</div>
	</div>

	<!-- jQuery (necessary for Bootstrap's JavaScript plugins) -->
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js"></script>
    <!-- Latest compiled and minified JavaScript -->
	<script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/js/bootstrap.min.js" integrity="sha384-0mSbJDEHialfmuBBQP6A4Qrprq5OVfW37PRR3j5ELqxss1yVqOtnepnHVP9aJ7xS" crossorigin="anonymous"></script>
	<script src="http://code.highcharts.com/highcharts.js"></script>
	<script src="microgear.js"></script>

	<script>

		$(function () {
			var data_sensor = 0;

			const APPKEY = 'jWnMSWIG9IbEq9y';
			const APPSECRET = 'mPY8ZMkliWoFp1jlDHm7M4euZSucwb';
			const APPID = 'NSC18';

			var microgear = Microgear.create({
				gearkey: APPKEY,
				gearsecret: APPSECRET
			});

			microgear.on('message',function(topic,msg) {
				// data_sensor = parseInt(msg);
				// document.getElementById("data").innerHTML = msg;

				if (msg == "10") {
					document.getElementById("label_mode").innerHTML = "Mode Intiligent : Off";
				} else if (msg == "01") {
					document.getElementById("label_mode").innerHTML = "Mode Intiligent : On";
				}

				console.log(msg);
				
			});

			microgear.on('connected', function() {
				microgear.setname('wwwNSC18');
				
				$('#Intiligent_on').click(function(){
					microgear.chat("NODEMCU","1"); // 01 is intiligent on
				});

				$('#Intiligent_off').click(function(){
					microgear.chat("NODEMCU","0"); // 01 is intiligent off
				});

			});

			microgear.on('present', function(event) {
				console.log(event);
			});

			microgear.on('absent', function(event) {
				console.log(event);
			});

			microgear.resettoken(function(err) {
				microgear.connect(APPID);
			});

			// End NETPIE

			setInterval(function(){
				var currentdate = new Date();
				document.getElementById("time_now").innerHTML = 'DATE ' + currentdate.getDate() + '/' + (currentdate.getMonth()+1) + '/' + currentdate.getFullYear() + ' - ' + currentdate.getHours() + ":"  
                + currentdate.getMinutes();
			}, 1000);

		});
			// if (data_sensor == "") {
			// 	console.log("n");
			// } else {
			// 	console.log(data_sensor);
			// }

		    

	</script>


</body>
</html>