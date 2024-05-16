#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

static std::string ConvertWstringToUTF8(const std::wstring& src)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t> > converter;
	return converter.to_bytes(src);
}

static std::wstring ConvertUTF8ToWstring(const std::string& src)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(src);
}

extern "C"
size_t str2wstr(wchar_t* q, int n, const char* p) 
{
	std::string src(p);
	std::wstring wstr = ConvertUTF8ToWstring(src);
	const wchar_t* qq = wstr.c_str();
	wcscpy_s(q, n, qq);
	size_t m = wcslen(q);
	return(m * sizeof(*q));
}

extern "C"
size_t wstr2str(char* p, int n, const wchar_t* q) 
{
	std::wstring src(q);
	std::string str = ConvertWstringToUTF8(src);
	const char* qq = str.c_str();
	strcpy_s(p, n, qq);
	size_t  m = strlen(p);
	return(m * sizeof(*p));
}

/*
	wstr -> str
	void a[]を使用
	void* ap = &a;
	return: 次のポインタ位置
	void* convWstr2Str(char* p, wchar_t* q, void* ap);
*/

extern "C"
wchar_t * convStr2WStr(void* ap, const char* p)
{
	std::string src(p);
	std::wstring wstr = ConvertUTF8ToWstring(src);
	const wchar_t* qq = wstr.c_str();
	size_t n = wcslen(qq);
	wcscpy_s((wchar_t *)ap, n+1, qq);
	//size_t m = wcslen(q);
	return((wchar_t *)ap);
}

extern "C"
char* convWstr2Str(void* ap, const wchar_t* q)
{
	std::wstring src(q);
	std::string str = ConvertWstringToUTF8(src);
	const char* qq = str.c_str();
	size_t n = strlen(qq);
	strcpy_s((char*)ap, n+1, qq);
	//size_t  m = strlen(p);
	return((char *)ap);
}
