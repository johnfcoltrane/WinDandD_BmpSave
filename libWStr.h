#pragma once
#ifdef CPPSRC
static std::string ConvertWstringToUTF8(const std::wstring& src);
static std::wstring ConvertUTF8ToWstring(const std::string& src);
#endif

#ifdef CPPSRC
extern "C" {
#endif
size_t str2wstr(wchar_t* q, int n, const char* p);
size_t wstr2str(char* p, int n, const wchar_t* q);
wchar_t* convStr2WStr(void* ap, const char* p);
char* convWstr2Str(void* ap, const wchar_t* q);
#ifdef CPPSRC
}
#endif

