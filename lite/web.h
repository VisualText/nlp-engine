/** web.h
* 1.Declares a class to encode strings converting a String
* into a MIME format called "x-www-form-urlencoded" format.
*	To convert a String, each character is examined in turn:
*		1) The ASCII characters 'a' through 'z', 'A' through 'Z', and '0' through '9' remain the same.
* 	2) The space character ' ' is converted into a plus sign '+'.
*		3) All other characters are converted into the 3-character string "%xy", where xy is the two-digit hexadecimal representation of the lower 8-bits of the character.
* 2.Declares a class to decode such strings
* 3. Declares the WebForm class to wrap win32 HTTP calls
* Author: Vijay Mathew Pandyalakal
* Date: 18/10/03
**/

#ifndef WEB_H_
#define WEB_H_

namespace openutils {

	class URLEncoder {
	public:
		static string encode(string str);
	private:
		static bool isOrdinaryChar(char c);
	};

	class URLDecoder {
	public:
		static string decode(string str);
	private:
		static int convertToDec(const char* hex);
		static void getAsDec(char* hex);
	};

	class WebForm {
	private:
		vector<string> m_vctVars;
		vector<string> m_vctVals;
		string m_strHostName;
		string m_strScriptFile;
		HINTERNET m_hSession;
		HINTERNET m_hRequest;
		unsigned long m_lBytesRead;

	public:
		WebForm();
		void putVariable(const char* var_name, const char* value);
		string getVariable(const char* var_name) const;
		void setHost(const char* host);

		void setScriptFile(const char* sf);
		string getHost() const;
		string getScriptFile() const;
		string sendRequest();
		bool getFile(CHAR *szFileName);
		bool getResponse(char* buff, int sz);
		unsigned long getBytesRead() { return m_lBytesRead; }
		~WebForm();
	private:
		bool isDuplicateVar(const char* var_name);
	};

	class WebFormException {
	private:
		string err;
	public:
		WebFormException(const char* str) {
			err = str;
		}
		string getMessage() {
			return err;
		}
	};
} // openutils

#endif