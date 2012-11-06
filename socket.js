var net = require('net');

var status = {eFirst : 0, eLogin : 1, eChat : 2, eLogout : 3}

var sockets = [];

var server = net.Server(function(socket)
{
	console.log('User Connected');
	socket.status = status.eFirst;
	//socket.write('hello\n');
	//socket.end('world\n');

	if (socket.status == status.eFirst)
	{
		socket.write('Please insert your name. \r\n');
		socket.status = status.eLogin;
		//socket.setEncoding('utf8');
		sockets.push(socket); 
	};

	socket.on('data', function(data)
	{
		console.log(data);
		
		if (socket.status == status.eLogin) 
		{
			var s = data.toString();
			//console.log(data);
			socket.username = s.trim();
			//socket.name = s.trim();
			socket.status = status.eChat;
			console.log('User :' + socket.username + ' is login');
			return;
		};

		// if (data.toString().trim() == '')
		// {
		// 	return;
		// };

		for (var i = 0; i < sockets.length; ++i)
		{
			if (sockets[i] == socket) 
			{
				continue;
			};
			var s = socket.username + ' : ' + data;
			console.log(s.toString());
			
			sockets[i].write(s);
		}
	});

	socket.on('end', function()
	{
		var i  = sockets.indexOf(socket);
		var sName = sockets[i].username;
		sockets.splice(i, 1);
		console.log('User ' + sName +' is logout');

		for (var i = 0; i < sockets.length; ++i)
		{
			sockets[i].write('User ' + sName +' is logout \r\n');
		}
	});
});

server.listen(3001);