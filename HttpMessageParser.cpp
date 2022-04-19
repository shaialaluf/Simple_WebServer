#include "HttpMessageParser.h"


int GetRequestType(string httpRequest)
{
	int spaceIndex = httpRequest.find(" ");
	string requestType = httpRequest.substr(0, spaceIndex);
	return convertRequestTypeToInt(requestType);
}

string createFilePath(string fileName, string langParamValue)
{
	string filePath;
	if (langParamValue == "en" || langParamValue == "he" || langParamValue == "fr")
	{
		filePath = PATH + langParamValue + "_" + fileName;
	}

	else if (langParamValue == "none")
	{
		filePath = PATH + fileName;
	}
	else if (langParamValue == "error - lang")
	{
		filePath = PATH + string("404page.html");
	}

	return filePath;
}

string createDateHeader()
{
	time_t timer;
	time(&timer);
	struct tm* info;
	info = localtime(&timer);
	char time[200];
	strftime(time, 200, "%a, %d %b %Y %H:%M:%S GMT", info);
	return string(time);
}

string createLastModifiedHeader(string filePath)
{
	struct stat attrib;
	stat(filePath.c_str(), &attrib);
	struct tm* info;
	info = localtime(&attrib.st_mtime);
	char time[200];
	strftime(time, 200, "%a, %d %b %Y %H:%M:%S GMT", info);
	return string(time);
}

string createUnimplementedResponse(string& responseStatus)
{
	responseStatus = NOT_IMPLEMENTED;
	string content = "ERROR: this method is not implemented in our server!";
	return content;
}

string createDeleteResponseIfFileExist(string filePath, string& responseStatus)
{
	string resourceContent = "";
	ifstream infile(filePath.c_str());
	if (!infile) // file not exist
	{
		infile.close();
		responseStatus = NOT_FOUND;
		resourceContent = "ERROR: server failed to find the file";
	}
	else
	{
		infile.close();
		responseStatus = OK;
		resourceContent ="file in path: " + filePath  + " deleted successfully.";
		if (remove(filePath.c_str()) != 0)
		{
			cout << "Error deleting file" << endl;
			responseStatus = SERVER_ERROR;
			resourceContent = "ERROR: server failed to delete file";
		}
	}
	
	return resourceContent;
}

string getRequestBody(string httpRequest)
{
	string bodyRequest = "";
	if (httpRequest.find("\r\n\r\n") != string::npos)
	{
		bodyRequest = httpRequest.substr(httpRequest.find("\r\n\r\n") + 4);
	}
	return bodyRequest;
}

string createPostResponseIfFileExist(string filePath, string& responseStatus)
{
	string resourceContent = "";
	ifstream infile(filePath.c_str());
	if (!infile) // file not exist
	{
		infile.close();
		responseStatus = NOT_FOUND;
		resourceContent = "ERROR: server failed to find the file";
	}
	else
	{
		infile.close();
		responseStatus = OK;
		resourceContent = "file in path: " + filePath + " found successfully.";
	}

	return resourceContent;
}


string createPutResponse(string filePath, string& responseStatus, string requestBody)
{
	string resourceContent = "";
	ifstream infile(filePath.c_str());

	if (!infile) // file not exist - we will create it
	{
		infile.close();
		resourceContent = "server created a new file in path: " + filePath + " with the requested content";
	}
	else // file exist - open it
	{
		infile.close();
		resourceContent = "server updated the file in path: " + filePath + " with the requested content";
	}

	ofstream outfile(filePath.c_str());
	if (!outfile) // server internal error 
	{
		outfile.close();
		responseStatus = SERVER_ERROR;
		resourceContent = "ERROR: server failed to create/open the file in path " + filePath;
	}
	else // ok - write to the file
	{
		responseStatus = OK;
		outfile << requestBody;
		outfile.close();	
	}
	
	return resourceContent;
}

string createResponseBodyIfFileExist(string filePath, string& responseStatus)
{
	ifstream infile(filePath.c_str());
	string resourceContent = "";
	string errorFilePath;
	if (!infile) // file not exist
	{
		infile.close();
		errorFilePath = PATH + string("404page.html");
		infile.open(errorFilePath.c_str());
		if (!infile)
		{
			cout << "ERROR: page 404page.html must be under c:/temp directory" << endl;
			responseStatus = SERVER_ERROR;
		}
		else {
			responseStatus = NOT_FOUND;
		}

	}
	else
	{
		responseStatus = OK;
	}
	if (responseStatus != SERVER_ERROR)
	{
		getline(infile, resourceContent, '\0');
	}
	return resourceContent;
}

string createHttpResponse(string responseStatus, string responseBody, int contentLength, string  dateHeader, string lastModifiedHeader)
{
	string httpResponse = "";
	httpResponse += "HTTP/1.1 " + responseStatus + "\r\n" +
		"Connection: keep-alive" + "\r\n" +
		"Date: " + dateHeader + "\r\n" +
		"Server: S_Y_WebServer" + "\r\n" +
		"Last-Modified: " + lastModifiedHeader + "\r\n" +
		"Content-Length: " + to_string(contentLength) + "\r\n" +
		"Content-Type: text/html" + "\r\n\r\n" +
		responseBody;
	return httpResponse;
}

string createTraceHttpResponse(string responseStatus, string responseBody, int contentLength, string  dateHeader, string lastModifiedHeader)
{
	string httpResponse = "";
	httpResponse += "HTTP/1.1 " + responseStatus + "\r\n" +
		"Connection: keep-alive" + "\r\n" +
		"Date: " + dateHeader + "\r\n" +
		"Server: S_Y_WebServer" + "\r\n" +
		"Last-Modified: " + lastModifiedHeader + "\r\n" +
		"Content-Length: " + to_string(contentLength) + "\r\n" +
		"Content-Type: message/http" + "\r\n\r\n" +
		responseBody;
	return httpResponse;
}


string createOptionsHttpResponse(string responseStatus, string responseBody, int contentLength, string  dateHeader, string lastModifiedHeader)
{
	string httpResponse = "";
	httpResponse += "HTTP/1.1 " + responseStatus + "\r\n" +
		"Allow: GET,POST,HEAD,DELETE,PUT,TRACE,OPTIONS" + "\r\n" +
		"Connection: keep-alive" + "\r\n" +
		"Date: " + dateHeader + "\r\n" +
		"Server: S_Y_WebServer" + "\r\n" +
		"Last-Modified: " + lastModifiedHeader + "\r\n" +
		"Content-Length: " + to_string(contentLength) + "\r\n" +
		"Content-Type: text/html" + "\r\n\r\n" +
		responseBody;
	return httpResponse;
}

string processResponse(string filePath, int requestType, string httpRequest) // third parameter is optional
{
	string responseStatus;
	string httpResponse;
	string responseBody;
	string lastModifiedHeader = createLastModifiedHeader(filePath);
	string requestBody = getRequestBody(httpRequest);
	switch (requestType)
	{
	case GET:
		responseBody = createResponseBodyIfFileExist(filePath, responseStatus);
		break;
		
	case POST:
		responseBody = createPostResponseIfFileExist(filePath, responseStatus);
		break;

	case HEAD:
		responseBody = createResponseBodyIfFileExist(filePath, responseStatus);
		break;

	case PUT:
		responseBody = createPutResponse(filePath, responseStatus, requestBody);
		break;

	case TRACE:
		responseBody = httpRequest;
		responseStatus = OK;
		break;

	case DELETE:
		responseBody = createDeleteResponseIfFileExist(filePath, responseStatus);
		break;

	case OPTIONS:
		responseBody = " The server's options are: GET,POST,HEAD,DELETE,PUT,TRACE,OPTIONS";
		responseStatus = OK;
		break;

	case ERROR: // unimplemented or illegal request type - return a reponse with error message.
		responseBody = createUnimplementedResponse(responseStatus);
		break;
	}

	int contentLength = responseBody.size();
	if (requestType == HEAD) 
	{
		responseBody = "";
	}
	string dateHeader = createDateHeader();
	if (requestType == TRACE)
	{
		httpResponse=createTraceHttpResponse(responseStatus, responseBody, contentLength, dateHeader, lastModifiedHeader);
	}

	else if (requestType == OPTIONS)
	{
		httpResponse= createOptionsHttpResponse(responseStatus, responseBody, contentLength, dateHeader, lastModifiedHeader);
	}

	else
	{
		httpResponse = createHttpResponse(responseStatus, responseBody, contentLength, dateHeader, lastModifiedHeader);
	}

	return httpResponse;
}

int convertRequestTypeToInt(string requestType)
{
	int typeInt;
	if (requestType.compare("GET") == 0)
	{
		typeInt = GET;
	}
	else if (requestType.compare("POST") == 0)
	{
		typeInt = POST;
	}
	else if (requestType.compare("HEAD") == 0)
	{
		typeInt = HEAD;
	}
	else if (requestType.compare("PUT") == 0)
	{
		typeInt = PUT;
	}
	else if (requestType.compare("TRACE") == 0)
	{
		typeInt = TRACE;
	}
	else if (requestType.compare("DELETE") == 0)
	{
		typeInt = DELETE;
	}
	else if (requestType.compare("OPTIONS") == 0)
	{
		typeInt = OPTIONS;
	}
	else
	{
		typeInt = ERROR;
	}

	return typeInt;
}

string ExtractResourceName(string httpRequest)
{
	int startIndex = httpRequest.find("/") + 1;
	int endIndex = httpRequest.find(" HTTP/") - 1;
	string fileName = httpRequest.substr(startIndex, endIndex - startIndex + 1);
	return fileName;
}

string checkLangParameter(string file)
{
	int langParamIndex = file.find("lang");
	if (file.find("lang") != string::npos)
	{
		if (file.substr(langParamIndex).find("en") != string::npos)
		{
			return "en";
		}

		else if (file.substr(langParamIndex).find("fr") != string::npos)
		{
			return "fr";
		}

		else if (file.substr(langParamIndex).find("he") != string::npos)
		{
			return "he";
		}
		else
		{
			return "error-lang";
		}
	}

	else
	{
		return "none";
	}

}

string HandleCurrentRequest(int index, SocketState* sockets)
{
	string bufferStr = string(sockets[index].buffer);
	int spaceIndex = bufferStr.find(" ");
	string bufferRequestType = bufferStr.substr(spaceIndex);

	int firstRequestLastIndex;
	if (bufferRequestType.find("GET") != string::npos)
	{
		firstRequestLastIndex = bufferRequestType.find("GET") + spaceIndex;

	}
	else if (bufferRequestType.find("POST") != string::npos)
	{
		firstRequestLastIndex = bufferRequestType.find("POST") + spaceIndex;
	}
	else if (bufferRequestType.find("PUT") != string::npos)
	{
		firstRequestLastIndex = bufferRequestType.find("PUT") + spaceIndex;
	}
	else if (bufferRequestType.find("HEAD") != string::npos)
	{
		firstRequestLastIndex = bufferRequestType.find("HEAD") + spaceIndex;
	}
	else if (bufferRequestType.find("DELETE") != string::npos)
	{
		firstRequestLastIndex = bufferRequestType.find("DELETE") + spaceIndex;
	}
	else if (bufferRequestType.find("TRACE") != string::npos)
	{
		firstRequestLastIndex = bufferRequestType.find("TRACE") + spaceIndex;
	}
	else if (bufferRequestType.find("OPTIONS") != string::npos)
	{
		firstRequestLastIndex = bufferRequestType.find("OPTIONS") + spaceIndex;
	}
	else
	{
		firstRequestLastIndex = bufferRequestType.size() + spaceIndex - 1;
	}

	string firstRequest = bufferStr.substr(0, firstRequestLastIndex + 1);

	string remainedStrBuffer = bufferStr.substr(firstRequestLastIndex + 1);
	strcpy(sockets[index].buffer, remainedStrBuffer.c_str());
	sockets[index].len = remainedStrBuffer.size();
	return firstRequest;
}