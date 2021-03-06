var net = require('net');
var h = require('./support/helper.js');

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

  var sendCmd = function(cmd) {
    client.write(cmd);
    return new Promise(function(resolve, reject) {
      client.once('data', function(data) {
        resolve(JSON.parse(data));
      }); 
    });     
  }
  
  describe("sends the command over TCP", function() {

    it("Link query spec - logging into admin", function(done) {
      client.write(h.loginToAdminQuery);
      client.once('data', function(data) {
        done();
        });
    });

    //tests are run sequentially

   //test insert
   var fredId;
    it("setting entity:1's forename to Fred", function(done) {
      sendCmd("INSERT DATA { $a <forename> \"Fred\" } WHERE { NEW($a,\"person\") }")
      .then(function(data) {
        fredId = data.result.data.a;
        done();
      });    
    });

    var smithId;
    //test insert
    it("setting entity:2's surname to Smith", function(done) {
      sendCmd("INSERT DATA { $a <surname> \"Smith\" } WHERE { NEW($a,\"person\") }")
      .then(function(data) {
        smithId = data.result.data.a;
        done();
      });       
    });    

    //test retrieve
    it("getting the forename of entity:1", function(done) {
      sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }")
      .then(function(data) {
        expect(data).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: fredId})]})}));
        done();
      });          
    });

    //test retrieve
    it("getting the surname of entity:2", function(done) {  
      sendCmd("SELECT $a WHERE { $a <surname> \"Smith\" }")
      .then(function(data) {
        expect(data).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: smithId})]})}));
        done();
      });
    });

    //test link
    it("link entity 1 and entity 2", function(done) {
      sendCmd("LINK entity:" + fredId + " entity:" + smithId)
      .then(function(data) {
        expect(data).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'text', data: 'Entities ' + fredId + ' and ' + smithId + ' linked successfully.'})}));
        done();
      });     
    });

    //test retrieve
    it("getting the forename of entity:1 after link", function(done) {
      sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }")
      .then(function(data) {
        expect(data).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: fredId})]})}));
        done();
      });      
    });

    //test retrieve
    it("getting the surname of entity:2 after link", function(done) {
      sendCmd("SELECT $a WHERE { $a <surname> \"Smith\" }")
      .then(function(data) {
        expect(data).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: fredId})]})}));
        done();
      });      
    });

    //test link
    it("unlink entity 1 and entity 2", function(done) {
      sendCmd("UNLINK entity:" + fredId + " entity:" + smithId)
      .then(function(data) {
        expect(data).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'text', data: 'Entities ' + fredId + ' and ' + smithId + ' unlinked successfully.'})}));
        done();
      });     
    });

    //test retrieve
    it("getting the forename of entity:1 after unlink", function(done) {
      sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }")
      .then(function(data) {
        expect(data).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: fredId})]})}));
        done();
      });          
    });

    //test retrieve
    it("getting the surname of entity:2 after unlink", function(done) {  
      sendCmd("SELECT $a WHERE { $a <surname> \"Smith\" }")
      .then(function(data) {
        expect(data).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: smithId})]})}));
        done();
      });
    });

    it("LinkQuerySpec - logging out from admin", function(done) {
        client.write('USER LOGOUT');
        client.once('data', function(data) {
            done();
        });
    });

  });
});
