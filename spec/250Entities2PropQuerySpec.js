/*var net = require('net');
var gen = require('./support/generator.js');

var largeData = gen([{
  count: 250,
  type: "person",
  template: `<forename> "Alexis#"; <surname> "Sanchez#"`
}]);

describe("Fuzzy Database", function() {
  var client;

  //connects to the database
  beforeAll(function(done) { 
    client = new net.Socket();
    client.connect(1407, '127.0.0.1', function() {
      client.write("FLUSH");
      client.once('data', function(data) {
        done();
      });   
    });
  });
  
  describe("sends the command over TCP", function() {
	it("NANA", function(done) {
      client.write("FLUSH");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'text', data: 'Database cleared.'})}));
        done();
      });      
    }); 
	
	//test insert with 2 properties
	it("Adding 250 entities", function(done) {
   client.write(largeData);
		client.on('data', function(data) {
        done();
      });      
    }); 
	
	it("NANA", function(done) {
      client.write("FLUSH");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'text', data: 'Database cleared.'})}));
        done();
      });      
    });   
	  
	});
}); */