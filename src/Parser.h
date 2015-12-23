#ifndef FUZZY_PARSER
#define FUZZY_PARSER

#include <iterator>
#include <vector>
#include <string>

using StringIterator = std::vector<std::string>::iterator;
using StringMap = std::map<std::string, std::string>;

std::vector<std::string> Tokenize(std::string str) {

	std::vector<std::string> results;
	std::string buffer;
	bool speechMarks = false;
	bool squareBrackets = false;
	bool triangleBrackets = false;
	bool filter = false;
	bool typing = false;

	for (auto iter = str.begin(); iter != str.end(); iter++) {	
		if ((*iter == '\n' || *iter == ' ' || *iter == '\t') && !speechMarks && !squareBrackets && !triangleBrackets && !filter) {
			if (buffer.length() > 0) {
				results.push_back(buffer);
				buffer.clear();
			}
			typing = false;
		}
		else {

			if ((*iter == '{' || *iter == '}' || *iter == ';' || *iter == ',' || *iter == '.' || (*iter == ':' && typing == false)) && !speechMarks && !squareBrackets && !filter) {
				if (buffer.length() > 0) {
					results.push_back(buffer);
					buffer.clear();
				}
				buffer.push_back(*iter);
				results.push_back(buffer);
				buffer.clear();
			} else {

				buffer += *iter;

				if (*iter == '^') {
					typing = true;
				}

				if (buffer == "FILTER(") {
					filter = true;
				}

				if (filter && *iter == ')') {
					filter = false;
				}

				if (*iter == '"') {
					speechMarks = !speechMarks;
				}

				if (*iter == '[' && !squareBrackets) {
					squareBrackets = true;
				}

				if (*iter == ']' && squareBrackets) {
					squareBrackets = false;
				}

				if (*iter == '<' && !triangleBrackets) {
					triangleBrackets = true;
				}

				if (*iter == '>' && triangleBrackets) {
					triangleBrackets = false;
				}

			}
			
		}
	}

	if (buffer.length() > 0) {
		results.push_back(buffer);
		buffer.clear();
	}

	return results;
}

class ParseException : public std::exception {

private:

	std::string _message;

public:
	
	ParseException(std::string msg) {
		_message = msg;
	}

	virtual const char* what() const throw()
	{
		return _message.c_str();
	}
};

struct Triple {
public: 
	std::string subject;
	std::string predicate;
	std::string object;

	Triple(std::string sub, std::string pred, std::string obj) {
		subject = sub;
		predicate = pred;
		object = obj;
	}
};

struct TriplesBlock {
public:
	std::vector<Triple> triples;
	std::string name;
	std::vector<std::string> filters;

	TriplesBlock(std::vector<Triple> trip, std::string n) {
		triples = trip;
		name = n;
	}

	TriplesBlock(std::vector<Triple> trip) {
		triples = trip;
	}

	TriplesBlock() {}
};

//ECHO is called DEBUGECHO to avoid a namespace collision on linux
enum class QueryType {
	SELECT,
	INSERT,
	DEL,
	PING,
	DEBUGECHO,
	USER
};

struct Query {
public:

	QueryType type;
	StringMap sources;
	TriplesBlock conditions;
	TriplesBlock whereClause;
	std::string data0;

	Query(QueryType t, StringMap s, TriplesBlock cond, TriplesBlock wh, std::string dat0) {
		type = t;
		sources = s;
		conditions = cond;
		whereClause = wh;
		data0 = dat0;
	}
};

std::vector<Triple> ParseTriples(StringIterator&& iter, StringIterator end) {
	std::vector<Triple> triples;
	std::string sub;
	std::string pred;
	int pos = 0;

	while(iter != end && *iter != "}") {

		if (*iter != ";" && *iter != "," && *iter != ".") {
			switch (pos) {
			case 0: 
				sub = *iter;
				pos = 1;
				break;
			case 1:
				pred = *iter;
				pos = 2;
				break;
			case 2:
				Triple trip(sub, pred, *iter);
				triples.push_back(trip);
				break;
			}
		}
		else {

			if (*iter == ".") {
				pos = 0;
			}

			if (*iter == ";") {
				pos = 1;
			}

			if (*iter == ",") {
				pos = 2;
			}
		}

		iter++;
	}

	return triples;
}

TriplesBlock ParseInsert(StringIterator&& iter, StringIterator end) {

	TriplesBlock output;

	if (*iter == "{") {
		iter++;
		output = TriplesBlock(ParseTriples(std::move(iter), end));

		if (*iter == "}") {
			iter++;
		}
	}

	return output;
}

StringMap ParseSources(StringIterator&& iter, StringIterator end) {
	
	StringMap sources;

	while (iter != end && *iter != "INSERT" &&  *iter != "SELECT" && *iter != "DELETE") {

		std::string name;

		name = *iter;
		iter++;

		if (iter != end) {
			if (*iter == ":") {
				iter++;
				if (iter != end) {
					sources[name] = *iter;
					iter++;
				}
			}
		}
	}

	return sources;
}

Query ParseAll(std::vector<std::string> tokens) {

	auto iter = tokens.begin();

	QueryType type;
	StringMap sources;
	TriplesBlock conditions;
	TriplesBlock whereClause;
	std::string data0;

	while(iter != tokens.end()) {
			
		if (*iter == "SOURCE") {
			*iter++;
			sources = ParseSources(std::move(iter), tokens.end());
			continue;
		}

		if (*iter == "INSERT") {
			*iter++;
			if (*iter == "DATA") {
				iter++;
				type = QueryType::INSERT;
				conditions = ParseInsert(std::move(iter), tokens.end());
			}
			continue;
		}

		if (*iter == "WHERE") {
			*iter++;
			whereClause = ParseInsert(std::move(iter), tokens.end());
			continue;
		}

		if (*iter == "PING") {
			*iter++;
			type = QueryType::PING;
			
			if (iter != tokens.end()) {
				throw ParseException("PING does not take any arguments");
			}

			break;
		}

		if (*iter == "ECHO") {
			*iter++;
			type = QueryType::DEBUGECHO;

			if (iter != tokens.end()) {
				data0 = *iter;
				*iter++;
				if (iter != tokens.end()) {
					throw ParseException("ECHO only takes one argument");
				}
			}

			break;
		}
		
		throw ParseException("Unknown symbol: " + *iter);
	}

	return Query(type, sources, conditions, whereClause, data0);
}

#endif