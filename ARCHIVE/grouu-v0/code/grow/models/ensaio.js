var mongoose = require('mongoose');
var ObjectId = mongoose.Schema.Types.ObjectId;

var ensaio = mongoose.Schema({
	name: {type: String , required: true}
	, data: { type: Date, default: Date.now  }
	, probe:{ type: String  , required: true}
	, probee: ObjectId 
	//, user : {type: String  , required: true}
	, activo : { type: Boolean, default: true}
});


module.exports  = mongoose.model('Ensaio', ensaio);
