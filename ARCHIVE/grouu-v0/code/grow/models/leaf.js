var mongoose = require('mongoose');
var ObjectId = mongoose.Schema.Types.ObjectId;

var leaf = mongoose.Schema({
	name: {type: String , required: true}
	, data: { type: Date, default: Date.now  }
	, pin:   String
	, value:  Number
});


module.exports  = mongoose.model('leaf', leaf);
