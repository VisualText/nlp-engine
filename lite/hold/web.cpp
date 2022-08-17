/** web.cpp
* Implements classes declared in web.h
* Author: Vijay Mathew Pandyalakal
* Date: 18/10/03
**/

#include "StdAfx.h"	// 05/20/14 AM.

#include <string>
#include <vector>

#include <windows.h>
#include <wininet.h>
#include <tchar.h>
#include <StrSafe.h>
#include <sstream>

#include "web.h"
using namespace openutils;

string URLEncoder::encode(string str) {
	int len = str.length();
	char* buff = new char[len + 1];
	strcpy(buff, str.c_str());
	string ret = "";
	for (int i = 0; i<len; i++) {
		if (isOrdinaryChar(buff[i])) {
			ret = ret + buff[i];
		}
		else if (buff[i] == ' ') {
			ret = ret + "+";
		}
		else {
			char tmp[6];
			sprintf(tmp, "%%%x", buff[i]);
			ret = ret + tmp;
		}
	}
	delete[] buff;
	return ret;
}

bool URLEncoder::isOrdinaryChar(char c) {
	char ch = tolower(c);
	if (ch == 'a' || ch == 'b' || ch == 'c' || ch == 'd' || ch == 'e'
		|| ch == 'f' || ch == 'g' || ch == 'h' || ch == 'i' || ch == 'j'
		|| ch == 'k' || ch == 'l' || ch == 'm' || ch == 'n' || ch == 'o'
		|| ch == 'p' || ch == 'q' || ch == 'r' || ch == 's' || ch == 't'
		|| ch == 'u' || ch == 'v' || ch == 'w' || ch == 'x' || ch == 'y'
		|| ch == 'z' || ch == '0' || ch == '1' || ch == '2' || ch == '3'
		|| ch == '4' || ch == '5' || ch == '6' || ch == '7' || ch == '8'
		|| ch == '9') {
		return true;
	}
	return false;
}

string URLDecoder::decode(string str) {
	int len = str.length();
	char* buff = new char[len + 1];
	strcpy(buff, str.c_str());
	string ret = "";
	for (int i = 0; i<len; i++) {
		if (buff[i] == '+') {
			ret = ret + " ";
		}
		else if (buff[i] == '%') {
			char tmp[4];
			char hex[4];
			hex[0] = buff[++i];
			hex[1] = buff[++i];
			hex[2] = '\0';
			//int hex_i = atoi(hex);
			sprintf(tmp, "%c", convertToDec(hex));
			ret = ret + tmp;
		}
		else {
			ret = ret + buff[i];
		}
	}
	delete[] buff;
	return ret;
}

int URLDecoder::convertToDec(const char* hex) {
	char buff[12];
	sprintf(buff, "%s", hex);
	int ret = 0;
	int len = strlen(buff);
	for (int i = 0; i<len; i++) {
		char tmp[4];
		tmp[0] = buff[i];
		tmp[1] = '\0';
		getAsDec(tmp);
		int tmp_i = atoi(tmp);
		int rs = 1;
		for (int j = i; j<(len - 1); j++) {
			rs *= 16;
		}
		ret += (rs * tmp_i);
	}
	return ret;
}

void URLDecoder::getAsDec(char* hex) {
	char tmp = tolower(hex[0]);
	if (tmp == 'a') {
		strcpy(hex, "10");
	}
	else if (tmp == 'b') {
		strcpy(hex, "11");
	}
	else if (tmp == 'c') {
		strcpy(hex, "12");
	}
	else if (tmp == 'd') {
		strcpy(hex, "13");
	}
	else if (tmp == 'e') {
		strcpy(hex, "14");
	}
	else if (tmp == 'f') {
		strcpy(hex, "15");
	}
	else if (tmp == 'g') {
		strcpy(hex, "16");
	}
}

WebForm::WebForm() {
	string m_strContentType = "Content-Type: application/x-www-form-urlencoded";
	string m_strHostName = "localhost:8080";
	string m_strScriptFile = "";
	HINTERNET m_hSession = NULL;
	HINTERNET m_hConnect = NULL;
	HINTERNET m_hRequest = NULL;
	HINTERNET m_hResponse = NULL;
}
void WebForm::putVariable(const char* var_name, const char* value) {
	if (isDuplicateVar(var_name)) {
		string str = "Duplicate variable name - ";
		str = str + var_name;
		throw WebFormException(str.c_str());
	}
	else {
		m_vctVars.push_back(var_name);
		m_vctVals.push_back(value);
	}
}

string WebForm::getVariable(const char* var_name) const {
	for (unsigned int i = 0; i<m_vctVars.size(); i++) {
		if (strcmpi(var_name, m_vctVars[i].c_str()) == 0) {
			return m_vctVals[i];
		}
	}
	string str = "Variable not found - ";
	str = str + var_name;
	throw WebFormException(str.c_str());
}

void WebForm::setHost(const char* host) {
	m_strHostName = host;
}

void WebForm::setScriptFile(const char* sf) {
	m_strScriptFile = sf;
}

string WebForm::getHost() const {
	return m_strHostName;
}

string WebForm::getScriptFile() const {
	return m_strScriptFile;
}

string WebForm::sendRequest() {
	string host = m_strHostName;
	string form_action = host;
	if (m_strScriptFile.length() > 0)
		form_action = form_action + m_strScriptFile + "?";
	int sz = m_vctVars.size();
	for (int i = 0; i<sz; i++) {
		string var = m_vctVars[i];
		string enc_var = URLEncoder::encode(var);
		string val = m_vctVals[i];
		string enc_val = URLEncoder::encode(val);
		form_action = form_action + enc_var;
		form_action = form_action + "=";
		form_action = form_action + enc_val;
		if (i != (sz - 1)) {
			form_action = form_action + "&";
		}
	}

	LPSTR s = const_cast<char *>(host.c_str());

	m_hSession = InternetOpen("WebForm 1",
		PRE_CONFIG_INTERNET_ACCESS,
		NULL,
		INTERNET_INVALID_PORT_NUMBER,
		0);
	if (m_hSession == NULL) {
		//throw WebFormException("Error:- InternetOpen()");
		return 0;
	}

	DWORD dwOpenRequestFlags = INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |
		INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS |
		INTERNET_FLAG_KEEP_CONNECTION |
		INTERNET_FLAG_NO_AUTH |
		INTERNET_FLAG_NO_AUTO_REDIRECT |
		INTERNET_FLAG_NO_COOKIES |
		INTERNET_FLAG_NO_UI |
		INTERNET_FLAG_RELOAD;

	m_hRequest = InternetOpenUrl(m_hSession,
		form_action.c_str(),
		NULL,
		0,
		dwOpenRequestFlags, //INTERNET_FLAG_RELOAD,
		0);
	if (m_hRequest == NULL) {
		//throw WebFormException("Error:- InternetOpenUrl()");
		return 0;
	}

	LPVOID lpOutBuffer = NULL;
	DWORD dwSize = 0;

retry:

	// This call will fail on the first pass, because
	// no buffer is allocated.
	if (!HttpQueryInfo(m_hRequest, HTTP_QUERY_RAW_HEADERS_CRLF,
		(LPVOID)lpOutBuffer, &dwSize, NULL))
	{
		if (GetLastError() == ERROR_HTTP_HEADER_NOT_FOUND)
		{
			// Code to handle the case where the header isn't available.
			return "";
		}
		else
		{
			// Check for an insufficient buffer.
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				// Allocate the necessary buffer.
				lpOutBuffer = new char[dwSize];

				// Retry the call.
				goto retry;
			}
			else
			{
				// Error handling code.
				if (lpOutBuffer)
				{
					delete[] lpOutBuffer;
				}
				return "";
			}
		}
	}

	string str = (LPCTSTR)lpOutBuffer;
	std::istringstream ss(str);
	std::string line, location;
	std::size_t found;

	while (ss.good())
	{
		getline(ss, line, '\n');
		found = line.find("Location:");
		if (found != std::string::npos) {
			found = line.find(":");
			location = line.substr(found + 2, line.length() - found - 3);
			break;
		}
	}

	if (lpOutBuffer)
	{
		delete[] lpOutBuffer;
	}

	m_hRequest = InternetOpenUrl(m_hSession,
		form_action.c_str(),
		NULL,
		0,
		INTERNET_FLAG_RELOAD,
		0);
	if (m_hRequest == NULL) {
		//throw WebFormException("Error:- InternetOpenUrl()");
		return 0;
	}


	return location;
}

bool WebForm::getFile(
	CHAR *szFileName)   // Local file name
{
	if (m_hRequest == NULL) {
		//throw WebFormException("No request made to server !");
		return false;
	}
	DWORD dwSize;
	CHAR   szHead[] = "Accept: */*\r\n\r\n";
	VOID * szTemp[25];
	FILE * pFile;

	if (!(pFile = fopen(szFileName, "wb")))
	{
		//throw WebFormException("Error opening file");
		return false;
	}
	do
	{
		// Keep coping in 25 bytes chunks, while file has any data left.
		// Note: bigger buffer will greatly improve performance.
		if (!InternetReadFile(m_hRequest, szTemp, 50, &dwSize))
		{
			fclose(pFile);
			throw WebFormException("Could not close file");
			return false;
		}
		if (!dwSize)
			break;  // Condition of dwSize=0 indicate EOF. Stop.
		else
			fwrite(szTemp, sizeof (char), dwSize, pFile);
	}   // do
	while (TRUE);
	fflush(pFile);
	fclose(pFile);
	return TRUE;
}

bool WebForm::getResponse(char* buff, int sz) {
	if (m_hRequest == NULL) {
		throw WebFormException("No request made to server !");
		return false;
	}
	unsigned int totalBytesRead = 0;
	unsigned int origSize = (unsigned int) sz;
	std::string buffer;
	while (totalBytesRead < origSize) {
		if (InternetReadFile(m_hRequest, buff, sz, &m_lBytesRead)) {
			if (m_lBytesRead > (unsigned int) sz) {
				throw WebFormException("Buffer overflow !");
			}
			if (m_lBytesRead == 0)
				break;
			buff[m_lBytesRead] = 0;
			buffer += buff;
			strcpy(buff, "");
			totalBytesRead += m_lBytesRead;
			sz -= m_lBytesRead;
			if (sz <= 0)
				break;
		}
	}
	strcpy(buff, buffer.c_str());
	return true;
}

bool WebForm::isDuplicateVar(const char* var_name) {
	for (unsigned int i = 0; i<m_vctVars.size(); i++) {
		if (strcmpi(var_name, m_vctVars[i].c_str()) == 0) {
			return true;
		}
	}
	return false;
}

WebForm::~WebForm() {
	if (m_hSession != NULL) InternetCloseHandle(m_hSession);
	if (m_hRequest != NULL) InternetCloseHandle(m_hRequest);
};