var mongoose = require('mongoose');
var ObjectId = mongoose.Schema.Types.ObjectId;

var temp = mongoose.Schema({
	name: {type: String , required: true}
	, data: { type: Date, default: Date.now  }
	, pin:   String
	, value:  Number
});


module.exports  = mongoose.model('Temp', temp);
