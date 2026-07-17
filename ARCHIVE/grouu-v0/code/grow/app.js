/*  MODULES */
var express = require('express')
  , routes = require('./routes')
  , user = require('./routes/user')
  , http = require('http')
  , path = require('path')

  , mongoose = require('mongoose').connect('mongodb://localhost/GROWDB')
  , db       = mongoose.connection
  , io       = require('socket.io')
  , socket   = require('./routes/socket.js')
  ;

var app = express();

/*  ALL ENVOIREMENTS */
app.set('port', process.env.PORT || 3000);
app.set('views', __dirname + '/views');
app.set('view engine', 'jade');
app.use(express.favicon());
app.use(express.logger('dev'));
app.use(express.bodyParser());
app.use(express.methodOverride());
app.use(app.router);
app.use(require('less-middleware')({ src: __dirname + '/public' }));
app.use(express.static(path.join(__dirname, 'public')));

/*  DEV ENVOIREMENT */
if ('development' == app.get('env')) {
  app.use(express.errorHandler());
}
/*  ROUTES */
app.get('/', routes.index);
app.get('/users', user.list);

	
app.get('/environment', routes.index);
app.get('/trial', routes.index);
app.get('/trial/:trialId', routes.index);
app.get('/probe/:probeId', routes.index);
app.get('/watering', routes.index);
app.get('/reports', routes.index);
app.get('/journal', routes.index);
app.get('/dashboard', routes.index);

app.get('/partials/:name', routes.partials);



/*  START SERVER */
var server =http.createServer(app).listen(app.get('port'), function(){
  io.listen(server,{ log: false }).sockets.on('connection', socket);
  console.log('GROW-LAB a correr na porta : ' + app.get('port'));
});


db.on('error', console.error.bind(console, 'connection error:'));
db.once('open', function callback () {
  console.log("CONECTADO A BASE DE DADOS");
});