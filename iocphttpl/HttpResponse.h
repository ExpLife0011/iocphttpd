#pragma once
#include "HttpHeader.h"
#include "HttpTemplate.h"
#include <fstream>
#include "Shlwapi.h"
#include "json.h"

class IOCPHTTPL_API HttpResponse : public HttpTemplate
{
public:
	HttpResponse();
	~HttpResponse();

	const char* resp_ok = "HTTP/1.0 200 OK";
	const char* resp_ct = "text/html";
	const char* tmp_date = "Fri, 31 Dec 1999 23:59:59 GMT";

	void Write(const char* str);
	void WriteTemplate(char* code);
	void GetResponse(char* szResponse, vector<byte> *pvb, DWORD dwSize);
	void SetStaticFileName(string path);
	std::vector<byte> GetStaticContent(const char *path);
	void WriteStatic(const char *path);
	void SetContentTypeFromExtension();
	PTSTR  GetPathExtension(const char* pszPath);
	PTSTR  GetPathExtension(PTSTR pszPath);
	void SetContentType(const char* str);

private:
	HttpHeader httpHeader;
	char* m_resp_body[DATA_BUFSIZE * 4];
	char* m_content_type[DATA_BUFSIZE];
	char* m_content_lenght[DATA_BUFSIZE];
	char* m_date[DATA_BUFSIZE];
	std::string m_szResponse;
	std::string m_path;
	std::vector<byte> m_sbResponse;

	std::wstring pathExtension;
	std::wstring contenType;
	const char *szPath;

	HANDLE ghMutex;

};

