var net = require('net');
var h = require('./support/helper.js');


describe("Fuzzy Database", function() {
  var client;

  //connects to the database
  beforeAll(function(done) { 
    client = new net.Socket();
    client.connect(1407, '127.0.0.1', function() {
        client.write(h.loginToAdminQuery);
        client.once('data', function(data) {
            var resultJSON = JSON.parse(data);
            expect(resultJSON.result.data).toEqual('Logged in successfully.');
            client.write("FLUSH");
            client.once('data', function(data) {
                var resultJSON = JSON.parse(data);
                client.write('USER LOGOUT');
                client.once('data', function(data) {
                    var resultJSON = JSON.parse(data);
                    h.setupClient(done());
                });
            });   
        });
      });
    });
  
  describe("sends the command over TCP", function() {

    //tests are run sequentially
    it("Sample query - logging into admin", function(done) {
      client.write(h.loginToAdminQuery);
      client.once('data', function(data) {
            var resultJSON = JSON.parse(data);
            expect(resultJSON.result.data).toEqual('Logged in successfully.');
            done();
        });
    });

    //test pong
    it("'PONG'", function(done) {
      client.write("PING");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON.result).toEqual({type: 'text', data: 'PONG'});
        done();
      });      
    });
	
	
    //test insert
    it("setting entity:2's forname to Fred", function(done) {
      client.write("INSERT DATA { $a <forename> \"Fred\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });

    //test retrieve
    it("getting the forename of entity:2", function(done) {
      client.write("SELECT $a WHERE { entity:2 <forename> $a }");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: 'Fred'})]})}));
        done();
      });      
    });

    it("getting entities with forename 'Fred'", function(done) {
      client.write("SELECT $a WHERE { $a <forename> \"Fred\" }");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: '2'})]})}));
        done();
      });      
    });

	//test insert with 2 properties
    it("setting entity:3's forename to 'Ned' and surname to 'Flanders'", function(done) {
      client.write("INSERT DATA { $a <forename> \"Ned\"; <surname> \"Flanders\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });
	
	//test retrieve
    it("getting the surname of entity:3", function(done) {
      client.write("SELECT $a WHERE { entity:3 <surname> $a }");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: 'Flanders'})]})}));
        done();
      });      
    });

	//discard current data
	it("flush everything", function(done) {
      client.write("FLUSH");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'text', data: 'Database cleared.'})}));
        done();
      });      
    });
	
	//test insert with 4 properties
	it("setting entity:2's forename to 'Marco' and surname to 'Reus', which is aged '28' and drinks 'Water'", function(done) {
      client.write("INSERT DATA { $a <forename> \"Marco\"; <surname> \"Reus\"; <age> \"28\"; <drinks> \"Water\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });
	
	//test insert with 5 properties
    it("setting entity:3's forename to 'Moe' and surname to 'Szyslak', which is aged '34', drinks 'Beer' and has as a profession 'Bartender'", function(done) {
      client.write("INSERT DATA { $a <forename> \"Moe\"; <surname> \"Szyslak\"; <age> \"34\"; <drinks> \"Beer\"; <profession> \"Bartender\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });

	//test insert with 4 properties
	it("setting entity:4's forename to 'Marco' and surname to 'Polo', which is aged '34', drinks 'Wine'", function(done) {
      client.write("INSERT DATA { $a <forename> \"Marco\"; <surname> \"Polo\"; <age> \"34\"; <drinks> \"Wine\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });
	
	//test select - Option 1  $a <prop> value
    it("getting entities with forename Marco", function(done) {
      client.write("SELECT $a WHERE { $a <forename> \"Marco\"}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: '2'}), ({a: '4'})]})}));
        done();
      });      
    });
	
	//test select - Option 1  $a <prop> value
    it("getting entities with forename Marco and surname Reus", function(done) {
      client.write("SELECT $a WHERE { $a <forename> \"Marco\" . $a <surname> \"Reus\"}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: '2'})]})}));
        done();
      });      
    });
	
	//test select - Option 1  $a <prop> value
    it("getting entities with age 34", function(done) {
       client.write("SELECT $a WHERE { $a <age> \"34\"}");
       client.once('data', function(data) {
	     var resultJSON = JSON.parse(data);
         expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: '3'}), ({a: '4'})]})}));
         done();
       });      
    });
	
	//test select - Option 1 $a <prop> value
	it("getting entities with surname Polo and age 34 - no entities", function(done) {
       client.write("SELECT $a WHERE { $a <age> \"34\" . $a <surname> \"Polo\"}");
       client.once('data', function(data) {
	     var resultJSON = JSON.parse(data);
         expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: '4'})]})}));
         done();
       });      
    });
	
	//test select - Option 1 $a <prop> value
	it("getting entities with surname Reus and age 34", function(done) {
       client.write("SELECT $a WHERE { $a <age> \"34\" . $a <surname> \"Reus\"}");
       client.once('data', function(data) {
	     var resultJSON = JSON.parse(data);
         expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[]})}));
         done();
       });      
    });
	
	//test select - Option 2 $a <prop> $b
    it("In this case, select option 2, should retrieve all three entities, as all three have the drinks property set", function(done) {
      client.write("SELECT $a WHERE { $a <drinks> $b}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: '2'}), ({a: '3'}), ({a: '4'})]})}));
        done();
      });      
    });
	
	//test select - Option 2 $a <prop> $b
    it("In this case, select option 2, should retrieve entity 3, as it is the only one which has both the drinks and profession properties set", function(done) {
      client.write("SELECT $a WHERE { $a <drinks> $b . $a <profession> $c}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: '3'})]})}));
        done();
      });      
    });
	
	//test select - Option 2 $a <prop> $b
    it("In this case, select option 2, should retrieve nothing, but it returns entity 3", function(done) {
      client.write("SELECT $a WHERE { $a <drinks> $b . $a <profession> $b}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[]})}));
        done();
      });   
    });


	//test select - Option 2 $a <prop> $b
    it("In this case, select option 2, should retrieve the drinks value and the profession value", function(done) {
      client.write("SELECT $b $c WHERE { $a <drinks> $b . $a <profession> $c}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({b: 'Beer', c: 'Bartender'})]})}));
        done();
      });      
    });
	
	//test select - Option 2 $a <prop> $b
	it("In this case, select option 2, should retrieve entity 3, as it is the only one which has both the drinks and profession properties set", function(done) {
      client.write("SELECT $a WHERE { $a <drinks> $b . $a <profession> $c}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: '3'})]})}));
        done();
      });      
    });
	
	//test select - Option 2 $a <prop> $b
    it("Testing select option 2, should retrieve the profession of entity 3, which is a Bartender", function(done) {
      client.write("SELECT $b WHERE { $a <profession> $b}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({b: 'Bartender'})]})}));
        done();
      });      
    });
	
	//test select - Option 2 $a <prop> $b
    it("Testing select option 2, should retrieve only entity 3, which has a profession property set", function(done) {
      client.write("SELECT $a WHERE { $a <profession> $b}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: '3'})]})}));
        done();
      });      
    });
	
	//test select - Option 3 $a $b value 
    it("Select entities which contain Marco ", function(done) {
      client.write("SELECT $a WHERE { $a $b \"Marco\"}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: '2'}), ({a: '4'})]})}));
        done();
      });      
    });

	//test select - Option 3 $a $b value 
    it("Retrieve property names that have Marco as a value ", function(done) {
      client.write("SELECT $b WHERE { $a $b \"Marco\"}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({b: 'forename'}), ({b: 'forename'})]})}));
        done();
      });      
    });
	//
	//test select - Option 3 $a $b value 
    it("Select entities which have a property which is equal to both Marco and 34", function(done) {
      client.write("SELECT $a WHERE { $a $b \"Marco\" . $a $b \"34\"}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual({status: true, errorCode: 0, info:'', result: {type: 'fsparql', data:[]}});
        done();
      });      
    });
	
    
	//test select - Option 3 $a $b value 
    it("Select properties which are equal to both Marco and 34 for the same entity", function(done) {
      client.write("SELECT $b $c WHERE { $a $b \"Marco\" . $a $c \"34\"}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual({status: true, errorCode: 0, info:'', result: {type: 'fsparql', data:[({c: 'age'}), ({b: 'forename'})]}});
        done();
      });      
    });
	
	//test select - Option 3 $a $b value 
    it("Select properties which are equal to both Marco and 34 for the same entity", function(done) {
      client.write("SELECT $a WHERE { $a $b \"Marco\" . $a $c \"34\"}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual({status: true, errorCode: 0, info:'', result: {type: 'fsparql', data:[({a: '4'}), ({a: '4'})]}});
        done();
      });      
    });

	//test select - Option 4 $a $b $c

     // h.testCase("Retrieving all triples", "SELECT $a $c $b WHERE { $a $b $c}", function(data, done) {
		   //  var resultJSON = JSON.parse(data);
     //    expect(resultJSON).toEqual({status: true, errorCode: 0, info:'', result: {type: 'fsparql', data:[
     //      {"a": "1", "c": "Unknown Source", "b": "name"},
     //      {"a": "1", "c": "source", "b": "type"},

     //      {"a": "2", "c": "person", "b": "type"}, 
     //      {"a": "2", "c": "Marco", "b": "forename"}, 
     //      {"a": "2", "c": "Reus", "b": "surname"}, 
     //      {"a": "2", "c": "28", "b": "age"}, 
     //      {"a": "2", "c": "Water", "b": "drinks"},

     //      {"a": "3", "c": "person", "b": "type"}, 
     //      {"a": "3", "c": "Moe", "b": "forename"}, 
     //      {"a": "3", "c": "Szyslak", "b": "surname"},
     //      {"a": "3", "c": "34", "b": "age"}, 
     //      {"a": "3", "c": "Beer", "b": "drinks"},
     //      {"a": "3", "c": "Bartender", "b": "profession"},

     //      {"a": "4", "c": "person", "b": "type"},
     //      {"a": "4", "c": "Marco", "b": "forename"},
     //      {"a": "4", "c": "Polo", "b": "surname"},
     //      {"a": "4", "c": "34", "b": "age"},
     //      {"a": "4", "c": "Wine", "b": "drinks"}
     //    ]}});
     //    done();
     //  }, 8000);      
	
	// //test select - Option 4 + Option 8 
 //    it("retrieving property-value pairs for entity:2", function(done) {
 //      client.write("SELECT $c $b WHERE { $a $b $c . entity:2 $b $c}");
 //      client.once('data', function(data) {
	// 	var resultJSON = JSON.parse(data);
 //        expect(resultJSON).toEqual({status: true, errorCode: 0, info:'', result: {type: 'fsparql', data:[
 //          {"c": "Marco", "b": "forename"}, 
 //          {"c": "Marco", "b": "forename"}, 
 //          {"c": "Reus", "b": "surname"}, 
 //          {"c": "28", "b": "age"}, 
 //          {"c": "Water", "b": "drinks"}]
 //        }});
 //        done();
 //      });      
 //    });
	
	//test select - Option 6 entity <property> $c
    it("Retrieving the surname of entity:2 ", function(done) {
      client.write("SELECT $a WHERE { entity:2 <surname> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Reus'})]})}));
        done();
      });      
    });
	
	//test select - Option 6 entity <property> $c
    it("Retrieving the surname of entity 2 and forename of entity 3 ", function(done) {
      client.write("SELECT $a $b WHERE { entity:2 <surname> $a . entity:3 <forename> $b}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Reus'}), ({b: 'Moe'})]})}));
        done();
      });      
    });
	
	//test select - Option 6 entity <property> $c
    it("Retrieving the surname of entity 2, the forename of entity 3 and the age of entity 4 ", function(done) {
      client.write("SELECT $a $b $c WHERE { entity:2 <surname> $a . entity:3 <forename> $b . entity:4 <age> $c}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Reus'}), ({b: 'Moe'}), ({c: '34'})]})}));
        done();
      });      
    });
	
	//test select - Option 6 entity <prop> $c
	it("Retrieving the surnames of entities 2 and 3", function(done) {
      client.write("SELECT $a $b WHERE { entity:2 <surname> $a . entity:3 <surname> $b}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Reus'}), ({b: 'Szyslak'})]})}));
        done();
      });      
    });
	
	//test select - Option 6 entity <prop> $c
	it("Retrieve the common surname of entity:2 and entity:3", function(done) {
      client.write("SELECT $a WHERE { entity:2 <surname> $a . entity:3 <surname> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: {type: 'fsparql', data:[]}}));
        done();
      });      
    }); 
	
	//test select - Option 6 entity <prop> $c
	it("Retrieve the common forename of entity:2 and entity:4", function(done) {
      client.write("SELECT $a WHERE { entity:2 <forename> $a . entity:4 <forename> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: {type: 'fsparql', data:[({a: "Marco"})]}}));
        done();
      });      
    }); 
	
	//test select - Option 6 entity <prop> $c
    it("Retrieving drinks property of entity 2", function(done) {
      client.write("SELECT $a WHERE { entity:2 <drinks> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Water'})]})}));
        done();
      });      
    });
	
	//test select - Option 6 entity <prop> $c
    it("Retrieving the surname of entity 3", function(done) {
      client.write("SELECT $a WHERE { entity:3 <surname> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Szyslak'})]})}));
        done();
      });      
    });
	
	//test select - Option 6 entity <prop> $c
    it("Retrieving the profession property of entity 3", function(done) {
      client.write("SELECT $a WHERE { entity:3 <profession> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Bartender'})]})}));
        done();
      });      
    });
	
	//test select - Option 6 entity <prop> $c
    it("Checking if empty result is returned if the property does not exist for the entity", function(done) {
      client.write("SELECT $a WHERE { entity:2 <profession> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[]})}));
        done();
      });      
    });

	//test select - Option 7 entity $b value 
    it("Checking if option 7 returns the property that has Marco as a value ", function(done) {
      client.write("SELECT $a WHERE { entity:2 $a \"Marco\"}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'forename'})]})}));
        done();
      });      
    });
	
	//test select - Option 7 entity $b value 
    it("Checking if option 7 returns the property that has Marco as a value and the property that has 28 as a value for entity 2 ", function(done) {
      client.write("SELECT $a $b WHERE { entity:2 $a \"Marco\" . entity:2 $b \"28\"}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'forename'}), ({b: 'age'})]})}));
        done();
      });      
    });
	
	//test select - Option 7 entity $b value
	it("Checking if option 7 returns for entity 2, the property that has Marco as a value and that it returns for entity 3 the property that has 34 as a value", function(done) {
      client.write("SELECT $a $b WHERE { entity:2 $a \"Marco\" . entity:3 $b \"34\"}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'forename'}), ({b: 'age'})]})}));
        done();
      });      
    });
	
	
	//test select - Option 8 entity $b $c 
    it("Retrieving the property names of entity 2", function(done) {
      client.write("SELECT $b WHERE { entity:2 $b $c}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual({status: true, errorCode: 0, info:'', result: {type: 'fsparql', data:[
          {b: 'type'}, 
          {b: 'forename'}, 
          {b: 'surname'},
          {b: 'age'},
          {b: 'drinks'}
        ]}});
        done();
      });      
    });
	
	//test select - Option 8 entity $b $c 
    it("Retrieving the property values of entity 2", function(done) {
      client.write("SELECT $c WHERE { entity:2 $b $c}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual({status: true, errorCode: 0, info:'', result: {type: 'fsparql', data:[
          {c: 'person'}, 
          {c: 'Marco'}, 
          {c: 'Reus'}, 
          {c: '28'}, 
          {c: 'Water'}]
        }});
        done();
      });      
    });
	
	//test select - Option 8 entity $b $c
    it("Retrieving the property-value pairs for entity 2", function(done) {
      client.write("SELECT $b $c WHERE { entity:2 $b $c}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual({status: true, errorCode: 0, info:'', result: {type: 'fsparql', data:[
          {"b": "type", "c": "person"}, 
          {"b": "forename", "c": "Marco"}, 
          {"b": "surname", "c": "Reus"},
          {"b": "age", "c": "28"}, 
          {"b": "drinks", "c": "Water"}
        ]}});
        done();
      });      
    });
	
	//discard current data
	it("flush everything", function(done) {
      client.write("FLUSH");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'text', data: 'Database cleared.'})}));
        done();
      });      
    });

	var assignedIds = {};

    //Multiple insert working
	it("having multiple inserts'", function(done) {
      client.write("INSERT DATA { $per1 <forename> \"Homer\", [60] \"Max\"; <surname> \"Simpson\", [60] \"Power\"; <age> 38 ; <wife> $per2; <drinks> \"Beer\" . $per2 <forename> \"Marge\"; <surname> \"Simpson\", [40] \"Bouvier\"; <age> 34 . $per3 <forename> \"Ned\"; <surname> \"Flanders\" . $per4 <forename> \"Moe\"; <surname> \"Szyslak\"; <occupation> \"Bartender\" }  WHERE { NEW($per1,\"person\") .  NEW($per2,\"person\") . NEW($per3,\"person\") . NEW($per4,\"person\") }");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        assignedIds = resultJSON.result.data;
        done();
      });      
    });
  
	//Multiple insert working
	it("having multiple inserts'", function(done) {
      client.write("INSERT DATA { $per1 <forename> \"Phil\", [60] \"Max\"; <surname> \"Travis\", [60] \"Power\"; <age> 38 ; <wife> $per2; <drinks> \"Beer\" . $per2 <forename> \"Marge\"; <surname> \"Sinclair\", [40] \"Dentist\"; <age> 37 . $per3 <forename> \"Barney\"; <surname> \"Stinson\"; <profession> \"magician\" . $per4 <forename> \"Moe\"; <surname> \"Szyslak\"; <occupation> \"Bartender\" } WHERE { NEW($per1,\"person\") . NEW($per2,\"person\") . NEW($per3,\"person\") . NEW($per4,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });
  
	//testing if entity is returned as output
    it("Retrieving the wife of entity:2 which is entity:3", function(done) {
      client.write("SELECT $a WHERE { entity:" + assignedIds["per1"] + " <wife> $a}");
      client.once('data', function(data) {
    var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: assignedIds["per2"]})]})}));
        done();
      });      
    });
	
	//discard current data
	it("flush everything", function(done) {
      client.write("FLUSH");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'text', data: 'Database cleared.'})}));
        done();
      });      
    });
	

	//test insert with 4 properties
	it("setting entity:2's forename to 'Marco' and surname to 'Reus', which is aged '28' and drinks 'Water'", function(done) {
      client.write("INSERT DATA { $a <forename> \"Marco\"; <surname> \"Reus\"; <age> \"28\"; <drinks> \"Water\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });
	
	//test select - Option 5 entity <property> $c
    it("Checking if option 5 works - if the entity exists ", function(done) {
      client.write("SELECT $a WHERE { entity:2 <surname> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Reus'})]})}));
        done();
      });      
    });
	
	
//need to flush everything at the start of the session to get rid of any previously stored data
	it("NANA", function(done) {
      client.write("FLUSH");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'text', data: 'Database cleared.'})}));
        done();
      });      
    });

    // Jonathan: Testing hierarchies.
    it("Creating a simple hierarchy", function(done) {
      client.write("INSERT DATA { $eng <name> \"England\" . $lon <name> \"London\"; <subsetOf> $eng } WHERE { NEW($eng, \"Location\") . NEW($lon, \"Location\") }");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({"status":true,"errorCode":0,"info":"Inserted 3 triples.","result":{"type":"fsparql","data":{"eng":"2","lon":"3"}}}));
        done();
      });      
    });

    it("Querying the hierarchy for superset link", function(done) {
      client.write("SELECT $a WHERE { $a <supersetOf> $b }");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({"status":true,"errorCode":0,"info":"","result":{"type":"fsparql","data":[{"a":"2"}]}}));
        done();
      });      
    });

    it("Querying the hierarchy for subset link", function(done) {
      client.write("SELECT $a WHERE { $a <subsetOf> $b }");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({"status":true,"errorCode":0,"info":"","result":{"type":"fsparql","data":[{"a":"3"}]}}));
        done();
      });      
    });

    it("Sample query - logging out from admin", function(done) {
        client.write('USER LOGOUT');
        client.once('data', function(data) {
            var resultJSON = JSON.parse(data);

        done();
        });
    });
 });   
}); 
	
