var moisture = require("../models/moisture.js");
var com = require("serialport");
var aaa=0;
var gerador= com.SerialPort;
//var serialPort = new gerador("/dev/ttyUSB0", {
var serialPort = new gerador("/dev/tty.usbserial-A900F4PE", {
	
	baudrate: 9600
	,    parser: com.parsers.readline('\r\n')
},true);



serialPort.open(function (err) 
{
	console.log('Port OPEN');


});





module.exports = function (socket) {


	serialPort.on('data', function(data) 
	{
		var pecas=data.split(':'); 
		if ((pecas[0]=="A0") && ( Number(pecas[1])!=NaN ))
		{
			var leitura=new moisture;
			leitura.name="coiso";
			leitura.pin=pecas[0];
			leitura.value=pecas[1];
			/*leitura.save( function(err, docs)
			{ 
				if(err)
				{
					console.log('ERRO AO GRAVAR VALOR ->'+docs);
				}
				else
				{
					aaa++;
					console.log(aaa+" < > "+pecas[1]);

					var enviar={ "pin":pecas[0], "val_x":pecas[1] };
					//socket.emit('receber',enviar);
				}
			});*/
			//console.log("VALOR -> "+pecas[1]);
			var enviar={ "pin":pecas[0], "val_x":pecas[1] };
			socket.emit('receber',enviar);
		}
		else
			console.log("ERRO "+pecas[1]);
	});











  // clean up when a user leaves, and broadcast it to other users
  socket.on('disconnect', function () {
  	socket.broadcast.emit('user:left', {
  		name: "jose"
  	});

  });
};