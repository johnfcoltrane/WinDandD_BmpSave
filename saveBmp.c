/*
   ＤＩＢをビットマップファイルに保存する

 　　     2000/ 1/28  宍戸　輝光
*/
//#undef UNICODE

#include <windows.h>
#include "libWstr.h"
//wchar_t* convStr2WStr(void* ap, const char* p);
//char* convWstr2Str(void* ap, const wchar_t* q);
#ifdef UNICODE
#pragma message("** UNICODE **")
#else
#pragma message("** NOT UNICODE **")
#endif

unsigned char buf1[100];
unsigned char buf2[100];
unsigned char buf3[100];
unsigned char buf4[100];
unsigned char buf5[100];

BYTE* p;
LPWSTR p2; //typedef WCHAR* LPWSTR;
#if 0
LPWSTR Char2-LPWSTR(BYTE* x) {
	int i = 0;
	int n = (int)strlen(x);
	LPWSTR p = malloc(n * sizeof(WCHAR)); //new WCHAR[n];
	for (i = 0; i < 1024; i++) {
		*p = (WCHAR)*x;
		x++;
		p++;
	}
	*p = L'\0';
	return(p);
};
#else
#if 1
LPWSTR Char2LPWSTR(void* buf, BYTE* x) { 
	wchar_t* result = convStr2WStr(buf, (const char*)x);
	return((LPWSTR)result);
	//return NULL;
}
#else
#define Char2LPWSTR(buf1, lpszSave) (lpszSave)
#endif
#endif
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
BOOL readBMP(LPVOID);
void greenScale(void);
void saveBMP(LPBYTE *, LPBITMAPINFO,LPBYTE);

HINSTANCE hInst;
HWND hwnd;
DWORD dwLength,dwWidth,dwHeight;
LPBITMAPINFO lpInfo;
LPBYTE lpPixel;
LPVOID lpBuf;
BOOL loaded;
 
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow){

	MSG  msg;
	WNDCLASSEX  wndclass ;

	hInst=hInstance; /* プロセスのハンドルを保存 */

	wndclass.cbSize        = sizeof(wndclass);
	wndclass.style         = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc   = WndProc;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = 0;
	wndclass.hInstance     = hInstance;
	wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = L"CWindow";
	wndclass.hIconSm       = LoadIcon (NULL, IDI_APPLICATION);

	RegisterClassEx (&wndclass);

	hwnd = CreateWindow (L"CWindow", L"ビットマップの保存",
	  WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,560,480,
      NULL,NULL,hInstance,NULL);

	ShowWindow (hwnd,iCmdShow); /* ウインドウを表示 */
	UpdateWindow (hwnd);        /* 再描画 */

	while (GetMessage (&msg,NULL,0,0)) { /* メッセージループ */

		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}

	return (int)msg.wParam ;

}

LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	char* p = NULL;
	HDC hdc;
	HDROP hDrop;
	PAINTSTRUCT ps;
	static HWND hwDoB,hwSaveB;
	static BYTE lpszFn[MAX_PATH + 1];
	
	static BYTE lpszSave[MAX_PATH + 1];
	static wchar_t lpszSaveW[MAX_PATH + 1];

	static wchar_t lpszFnW[MAX_PATH+1];

	static OPENFILENAME ofn;

	ofn.lStructSize=sizeof(OPENFILENAME);
	ofn.hwndOwner=hwnd;
	ofn.lpstrFilter = L"ビットマップ(*.bmp)\0*.bmp\0";
	ofn.nFilterIndex=1;

	//ofn.lpstrFile = Char2LPWSTR(buf1, lpszSave);
	ofn.lpstrFile = lpszSaveW;

	ofn.nMaxFile=MAX_PATH+1;
	ofn.lpstrDefExt = L"bmp";
	ofn.lpstrTitle = L"保存するファイル名を指定してください";
	ofn.Flags = OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY;

	switch (iMsg) {

		case WM_CREATE:

			hwDoB=CreateWindow(L"Button",L"緑色化", /* ボタン作成 */
				WS_CHILD | WS_VISIBLE,16,8,96,32,hwnd,
				(HMENU)0,hInst,NULL);

			hwSaveB=CreateWindow(L"Button",L"保存", /* ボタン作成 */
				WS_CHILD | WS_VISIBLE,128,8,96,32,hwnd,
				(HMENU)1,hInst,NULL);

			EnableWindow(hwDoB,FALSE);
			EnableWindow(hwSaveB,FALSE);

			DragAcceptFiles(hwnd,TRUE); /* ドラッグ＆ドロップ受入 */

			break;

		case WM_DROPFILES: /* ファイルがドロップされた時の処理 */

			hDrop=(HDROP)wParam; /* HDROPを取得 */
			//
			//DragQueryFile(hDrop,0, Char2LPWSTR(buf2, lpszFn),MAX_PATH+1); /* ファイル名を取得 */
			DragQueryFile(hDrop, 0, lpszFnW, MAX_PATH + 1); /* ファイル名を取得 */
			// convert: lpszFnW -> lpszFn
			p = convWstr2Str(buf2, lpszFnW);
			strcpy_s(lpszFn, 100, p);
			if (readBMP(lpszFn)) {

				EnableWindow(hwDoB,TRUE);
				EnableWindow(hwSaveB,TRUE);

			} else {

				EnableWindow(hwDoB,FALSE);
				EnableWindow(hwSaveB,FALSE);

			}

			DragFinish(hDrop); /* 終了処理 */

			InvalidateRgn(hwnd,NULL,FALSE);
			UpdateWindow (hwnd); /* 再描画 */

			break;

		case WM_COMMAND:

			switch (LOWORD(wParam)) {

				case 0: /* 緑色化 */

					greenScale();

					break;

				case 1:

					/* 保存するファイル名指定 */
					if (GetSaveFileName(&ofn))
						// convert: lpszSaveW -> lpszSave
//#pragma message("todo:  convert: lpszSaveW -> lpszSave")
						p = convWstr2Str(buf1, lpszSaveW);
						strcpy_s(lpszSave, 100, p);
						saveBMP((LPBYTE *)lpszSave,lpInfo,lpPixel);
						//saveBMP(lpszSaveW, lpInfo, lpPixel);

				break;

			}

			break;

		case WM_PAINT:

			hdc=BeginPaint(hwnd,&ps);

			if (loaded) /* ビットマップが読み込まれていれば */
				StretchDIBits(hdc,4,48,dwWidth,dwHeight,
				  0,0,dwWidth,dwHeight,lpPixel,lpInfo,
				  DIB_RGB_COLORS,SRCCOPY); /* DIBを画面に描画 */

			EndPaint(hwnd,&ps);

			break;

		case WM_DESTROY :

			if (loaded)
				GlobalFree(lpBuf);

			PostQuitMessage(0);
			break;

	}

	return DefWindowProc (hwnd, iMsg, wParam, lParam) ;

}

BOOL readBMP(LPBYTE fn) { /* ビットマップ読み込み */

	DWORD offset,dummy;

	HANDLE fh;

	fh=CreateFile(Char2LPWSTR(buf3,fn) ,GENERIC_READ,0,NULL,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,NULL); /* ファイルオープン */

	if (fh==INVALID_HANDLE_VALUE)
		return FALSE;

	lpBuf=(LPBYTE)GlobalAlloc
	    (GPTR,GetFileSize(fh,NULL)); /* バッファ確保 */

	ReadFile(fh,lpBuf,GetFileSize(fh,NULL),&dummy,NULL);
	CloseHandle(fh);

	lpInfo=(LPBITMAPINFO)((LPBYTE)lpBuf+sizeof(BITMAPFILEHEADER));

	offset=*(LPDWORD)((LPBYTE)lpBuf+10);
	lpPixel=(LPBYTE)lpBuf+offset; /* ビットマップバッファの先頭アドレス */

	/* ２４ビットフルカラービットマップでなければ無効 */
	if (lpInfo->bmiHeader.biBitCount!=24) {

		GlobalFree(lpBuf);
		loaded=FALSE;

		return FALSE;

	}

	/* ビットマップの大きさ保存 */
	dwWidth=lpInfo->bmiHeader.biWidth;
	dwHeight=lpInfo->bmiHeader.biHeight;

	if ((dwWidth*3) % 4==0) /* バッファの１ラインの長さを計算 */
		dwLength=dwWidth*3;
	else
		dwLength=dwWidth*3+(4-(dwWidth*3) % 4);

	loaded=TRUE; /* 読み込みフラグセット */

	InvalidateRgn(hwnd,NULL,FALSE);
	UpdateWindow (hwnd); /* 再描画 */

	return TRUE;

}

void greenScale(void) { /* 緑色化 */

	DWORD i,j;
	BYTE r,g,b,l;

	for (i=0;i<dwHeight;i++)
		for (j=0;j<dwWidth;j++) {

			b=lpPixel[j*3+i*dwLength];
			g=lpPixel[j*3+i*dwLength+1];
			r=lpPixel[j*3+i*dwLength+2];

			l=(BYTE)(r*0.3+g*0.59+b*0.11); /* 明るさ */

			lpPixel[j*3+i*dwLength]=0;
			lpPixel[j*3+i*dwLength+1]=l;
			lpPixel[j*3+i*dwLength+2]=0;

		}

	InvalidateRgn(hwnd,NULL,FALSE);
	UpdateWindow (hwnd); /* 再描画 */

}

void saveBMP(LPBYTE* lpszFn,LPBITMAPINFO lpInfo,LPBYTE lpPixel) {

	LPVOID lpBufl;
	LPBITMAPFILEHEADER lpHead;
	LPBITMAPINFOHEADER lpInfol;
	LPBYTE lpPixell;
	DWORD dwWidth,dwHeight,dwLength,dwSize;
	HANDLE fh;

	/* ビットマップの大きさ設定 */
	dwWidth=lpInfo->bmiHeader.biWidth;
	dwHeight=lpInfo->bmiHeader.biHeight;

	if ((dwWidth*3) % 4==0) /* バッファの１ラインの長さを計算 */
		dwLength=dwWidth*3;
	else
		dwLength=dwWidth*3+(4-(dwWidth*3) % 4);

	lpBufl=GlobalAlloc(GPTR, /* メモリ確保 */
	  sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwLength*dwHeight);

	/* メモリイメージ用ポインタ設定 */
	lpHead=(LPBITMAPFILEHEADER)lpBufl;
	lpInfol=(LPBITMAPINFOHEADER)((LPBYTE)lpBufl+sizeof(BITMAPFILEHEADER));
	lpPixell=(LPBYTE)lpBufl+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

	/* ビットマップのヘッダ作成 */
	lpHead->bfType='M'*256+'B';
	lpHead->bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwLength*dwHeight;
	lpHead->bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	lpInfol->biSize=sizeof(BITMAPINFOHEADER);
	lpInfol->biWidth=dwWidth;
	lpInfol->biHeight=dwHeight;
	lpInfol->biPlanes=1;
	lpInfol->biBitCount=24;

	/* ピクセル列をコピー */
	CopyMemory(lpPixell,lpPixel,dwLength*dwHeight);

	fh=CreateFile(Char2LPWSTR(buf4, (char *)lpszFn),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	WriteFile(fh,lpBufl,sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFO)+dwLength*dwHeight,&dwSize,NULL);

	CloseHandle(fh);

	if (dwSize<sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFO)+dwLength*dwHeight)
		MessageBox(hwnd,L"ファイル作成に失敗しました。空き容量を確認してください",
		  L"書き込みエラー",MB_OK);

	return;

}