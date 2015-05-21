var app         = require('express')();
var server      = require('http').Server(app);
var io          = require('socket.io')(server);

server.listen(8000);

// ブラウザから/にアクセスがあったらindex.htmlを戻す
app.get('/', function(req, res) {
    res.sendFile(__dirname + '/index.html');
});

io.on('connection', function(socket) {

    socket.on('join', function(data) {
	// typeには'browser'か'native'が来るので、それぞれのroomに登録する
	socket.join(data.type);
	// nameをセッションに保存する
	socket.session = {
	    name: data.name
	}
	console.log('join:', data.name);
    });

    socket.on('run', function(data) {
	// 送り元の名前をdataに追加してブロードキャスト
	data.from = socket.session.name;
	socket.broadcast.emit('run', data);
	console.log('run(' + data.from + '):' + data.command);
    });

    socket.on('reply', function(data) {
	// browserルームに対してdataをブロードキャスト
	data.from = socket.session.name;
	io.to('browser').emit('reply', data);
	console.log('reply(' + data.from + '):' + data.output);
    });
});
