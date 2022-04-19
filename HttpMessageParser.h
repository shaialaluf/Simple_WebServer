#pragma once
#pragma warning(disable : 4996)
#include <iostream>
#include <fstream> 
#include "HttpServerUtillity.h"
#define PATH "C:\\temp\\"
#define NOT_FOUND "404 Not Found"
#define SERVER_ERROR "500 Internal Server Error"
#define OK "200 OK"
#define NOT_IMPLEMENTED "501 Not Implemented"
#include <string>
using namespace std;

int GetRequestType(string httpRequest);
string HandleCurrentRequest(int index, SocketState* sockets);
int convertRequestTypeToInt(string requestType);
string ExtractResourceName(string httpRequest);
string checkLangParameter(string file);
string createFilePath(string fileName, string langParamValue);
string createResponseBodyIfFileExist(string filePath, string& responseStatus);
string createDeleteResponseIfFileExist(string filePath, string& responseStatus);
string createPutResponse(string filePath, string& responseStatus, string requestBody);
string createDateHeader();
string createLastModifiedHeader(string filePath);
string createHttpResponse(string responseStatus, string responseBody, int contentLength, string  dateHeader, string lastModifiedHeader);
string processResponse(string filePath, int requestType, string httpRequest = "null");
string getRequestBody(string httpRequest);
string createTraceHttpResponse(string responseStatus, string responseBody, int contentLength, string  dateHeader, string lastModifiedHeader);
string createUnimplementedResponse(string& responseStatus);