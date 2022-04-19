#include "HttpFunctions.h"
#include <ctime>

string doGet(string HttpRequest)
{
	string fileName=ExtractResourceName(HttpRequest);
	string langParamValue = checkLangParameter(fileName);
	fileName = fileName.substr(0,fileName.find("?"));
	string filePath= createFilePath(fileName, langParamValue);
	string httpResponse = processResponse(filePath,GET);
	return httpResponse;
}
string doPost(string HttpRequest)
{
	string fileName = ExtractResourceName(HttpRequest);
	fileName = fileName.substr(0, fileName.find("?"));
	string filePath = createFilePath(fileName, string("none"));

	string requestBody = getRequestBody(HttpRequest);
	cout<<endl<<"The request body is: "<<(requestBody.empty()?"Empty":requestBody)<<endl<<endl;
	string httpResponse = processResponse(filePath, POST);
	return httpResponse;
}

string doHead(string HttpRequest)
{
	string fileName = ExtractResourceName(HttpRequest);
	string langParamValue = checkLangParameter(fileName);
	fileName = fileName.substr(0, fileName.find("?"));
	string filePath = createFilePath(fileName, langParamValue);
	string httpResponse = processResponse(filePath, HEAD);
	return httpResponse;
}

string doPut(string HttpRequest)
{
	string fileName = ExtractResourceName(HttpRequest);
	fileName = fileName.substr(0, fileName.find("?"));
	string filePath = createFilePath(fileName, string("none"));
	string httpResponse = processResponse(filePath, PUT, HttpRequest);
	return httpResponse;
}

string doTrace(string HttpRequest)
{
	string fileName = ExtractResourceName(HttpRequest);
	fileName = fileName.substr(0, fileName.find("?"));
	string filePath = createFilePath(fileName, string("none"));
	string httpResponse = processResponse(filePath, TRACE, HttpRequest);
	return httpResponse;
}

string doDelete(string HttpRequest)
{
	string fileName = ExtractResourceName(HttpRequest);
	fileName = fileName.substr(0, fileName.find("?"));
	string filePath = createFilePath(fileName, string("none"));
	string httpResponse = processResponse(filePath,DELETE);
	return httpResponse;
}

string doOptions(string HttpRequest)
{
	string fileName = ExtractResourceName(HttpRequest);
	fileName = fileName.substr(0, fileName.find("?"));
	string filePath = createFilePath(fileName, string("none"));
	string httpResponse = processResponse(filePath, OPTIONS);
	return httpResponse;
}

string handleIllegalRequest(string HttpRequest)
{
	string fileName = ExtractResourceName(HttpRequest);
	fileName = fileName.substr(0, fileName.find("?"));
	string filePath = createFilePath(fileName, string("none"));

	string illegalMethod = HttpRequest.substr(HttpRequest.find(" "));
	cout <<endl<<"ERROR: "<< illegalMethod <<" method is not implemented in our server!" << endl << endl;
	string httpResponse = processResponse(filePath,ERROR);
	return httpResponse;
}