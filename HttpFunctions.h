#pragma once
#include <iostream>
#include "HttpMessageParser.h"
using namespace std;

string doGet(string HttpRequest);
string doPost(string HttpRequest);
string doHead(string HttpRequest);
string doPut(string HttpRequest);
string doTrace(string HttpRequest);
string doDelete(string HttpRequest);
string doOptions(string HttpRequest);
string handleIllegalRequest(string currentRequest);