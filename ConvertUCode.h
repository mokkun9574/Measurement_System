/****************************************************************************** 
	UNICODE対応マクロ定義ファイル
	
	File name: ConvertUCode.h
	Version:	0.01 (2012/04/27)
	Compiler:	Visual Studio 2010 Proffesional

	<機能> UNICODEに対応させるためのマクロ定義群
	<関連ファイル> なし
	<更新履歴>
	2012/04/27 Start -Ver0.01
	<特記事項>
	＜著作権表示＞
	Copyright © 2012 Keisuke SHIMA All rights reserved.
******************************************************************************/
#ifndef __INC_CONVERTUCODE_H

#ifdef _UNICODE

#define MAKEDIR(dirName) _wmkdir(dirName)
#define ITOA(value,str,radix) _itow_s(value,str,4*sizeof(wchar_t),radix)
#define GCVT(buffer,size,value,digits) _gcvt_s((char*)buffer,size,value,digits);
#define STRSTR(str,strSearch)	wcsstr(str,TEXT(strSearch))
#define ATOF(str) _wtof(str)	
#define ATOI(str) _wtoi(str)	

#else 

#define MAKEDIR(dirName) _mkdir(dirName)
#define ITOA(value,str,radix) _itoa(value,(char*)str,radix)	
#define GCVT(buffer,size,value,digits) _gcvt(value,digits,buffer);
#define STRSTR(str,strSearch)	strstr(str,strSearch)
#define ATOF(str) atof(str)	
#define ATOI(str) atoi(str)	

#endif

#endif