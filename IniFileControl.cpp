/****************************************************************************** 
	INI File Control Class 

	File name: IniFileControl.cpp
	Version:	2.00 (2012/04/27)

	<機能> .ini ファイルに対してデータ読み込み＆書き込みを行う
	<関連ファイル>
	EventControl.h: クラス本体のコード
	<更新履歴>
	2008/06/03 Start-Ver.1.0完成
	2008/06/16 データの削除追加-Ver.1.1
	2012/04/27 UNICODE対応.コメント一新 -Ver 2.0
	<特記事項>
	カレントディレクトリにIniフォルダを自動生成し，.iniファイルを生成する．
	⇒自動生成でいいのか？呼び出し側でファイルパスを送る場合はどうなる？
	⇒呼び出し側のアプリケーションで特にiniファイルの存在を意識せずとも良い
	場合は，このままでOK．
	＜著作権表示＞
	Copyright © 2012 Keisuke SHIMA All rights reserved.
******************************************************************************/
#include "IniFileControl.h"
#include <stdlib.h>		//文字列操作
#include <direct.h>		//ディレクトリ操作用

//-----------------------------------------------------------------------------
//	コンストラクタ
//	<機能>　カレントディレクトリ下にIniディレクトリを生成
//	<特記事項> なし
//-----------------------------------------------------------------------------
IniFileControl::IniFileControl( void )
{
	TCHAR strTMP[DEF_INI_BUFFSIZE];

	MaxDtSize=DEF_INI_BUFFSIZE;					//文字列の最大サイズ設定
	GetCurrentDirectory(MAX_PATH,CrtDIRName);	//カレントディレクトリ名の取得
	//カレントディレクトリ下にIniディレクトリを生成
	wsprintf(strTMP,TEXT("%s\\Ini"),CrtDIRName);
	MAKEDIR(strTMP);
	//デフォルトパラメータの設定
	SetApplicationName(DEF_INI_DEFAULTAPPNAME);	//アプリケーション名
	SetDefaultMessage(DEF_INI_DEFAULTMSG);		//エラーメッセージ
	SetFileName(DEF_INI_DEFAULTFNAME);			//ファイル名
}

//-----------------------------------------------------------------------------
//	デストラクタ
//	<機能>　なし
//	<特記事項> なし
//-----------------------------------------------------------------------------
IniFileControl::~IniFileControl( void )
{
}
//-----------------------------------------------------------------------------
//	<機能>　保存・読み込みファイル名の設定.
//	<入力> 
//	TCHAR *strName: 設定するファイル名
//	<出力> なし
//	<戻り値>
//	int : 常に1
//	<特記事項> 
//	カレントディレクトリ上のファイル名を設定する．
//-----------------------------------------------------------------------------
int IniFileControl::SetFileName( TCHAR *strName )
{
	wsprintf(FileName, TEXT("%s\\Ini\\%s"), CrtDIRName, strName);	//ファイル名の設定
	if( STRSTR(strName,".ini")==NULL ){					//拡張子検索
		wsprintf(FileName, TEXT("%s.ini"), FileName);	//拡張子の追加
	}
	return 1;
}

//-----------------------------------------------------------------------------
//	<機能>　文字列の書き込み
//	<入力> 
//	TCHAR *strKey: キーの名称
//	TCHAR *strData:	書き込む文字列データ
//	<出力> なし
//	<戻り値>
//	BOOL : 文字列の格納に成功= 0以外の値
//	<特記事項> 
//	設定済みiniファイルに文字列データを書き込む．
//-----------------------------------------------------------------------------
BOOL IniFileControl::WriteString( TCHAR *strKey, TCHAR *strData )
{
	return WritePrivateProfileString(AppName,strKey,strData,FileName);
}

//-----------------------------------------------------------------------------
//	<機能>　文字列の読み込み
//	<入力> 
//	TCHAR *strKey: キーの名称
//	TCHAR *strBuff:	読み出した文字列データが格納される配列バッファへのポインタ
//	<出力> なし
//	<戻り値>
//	DWORD : バッファに格納された文字数
//	<特記事項> 
//	設定済みiniファイルから文字列データを読み出す．
//-----------------------------------------------------------------------------
DWORD IniFileControl::ReadString( TCHAR *strKey, TCHAR *strBuff )
{
	return GetPrivateProfileString(AppName,strKey,DefMsg,strBuff,MaxDtSize,FileName);
}

//-----------------------------------------------------------------------------
//	<機能>　アプリケーション名の設定
//	<入力> 
//	TCHAR *strName: アプリケーション名
//	<出力> なし
//	<戻り値>
//	int : 常に1
//	<特記事項> 
//	設定済みiniファイルにアプリケーション名を設定する．
//-----------------------------------------------------------------------------
int IniFileControl::SetApplicationName( TCHAR *strName )
{
	lstrcpy(AppName,strName);	//名称の設定
	return 1;
}

//-----------------------------------------------------------------------------
//	<機能>　デフォルトメッセージの設定
//	<入力> 
//	TCHAR *strMsg: デフォルトメッセージ
//	<出力> なし
//	<戻り値>
//	int : 常に1
//	<特記事項> なし
//-----------------------------------------------------------------------------
int IniFileControl::SetDefaultMessage( TCHAR *strMsg )
{
	lstrcpy(DefMsg,strMsg);	//名称の設定
	return 1;
}

//-----------------------------------------------------------------------------
//	<機能>　データ書き込み(文字列型)
//	<入力> 
//	TCHAR *strKey: キーの名称
//	TCHAR *strData: 格納する文字列へのポインタ
//	<出力> なし
//	<戻り値>
//	BOOL : 文字列の格納に成功= 0以外の値
//	<特記事項> なし
//-----------------------------------------------------------------------------
BOOL IniFileControl::WriteData( TCHAR *strKey, TCHAR *strData )
{
	return WriteString(strKey,strData);
}

//-----------------------------------------------------------------------------
//	<機能>　データ書き込み(int型)
//	<入力> 
//	TCHAR *strKey: キーの名称
//	int data: 格納データ
//	<出力> なし
//	<戻り値>
//	BOOL : データの格納に成功= 0以外の値
//	<特記事項> なし
//-----------------------------------------------------------------------------
BOOL IniFileControl::WriteData( TCHAR *strKey, int data )
{
	TCHAR strTMP[DEF_INI_BUFFSIZE];
	ITOA(data,strTMP,10);
	return WriteString(strKey,strTMP);
}

//-----------------------------------------------------------------------------
//	<機能>　データ書き込み(double型)
//	<入力> 
//	TCHAR *strKey: キーの名称
//	double data: 格納データ
//	<出力> なし
//	<戻り値>
//	BOOL : データの格納に成功= 0以外の値
//	<特記事項> なし
//-----------------------------------------------------------------------------
BOOL IniFileControl::WriteData( TCHAR *strKey, double data )
{
	char strTMP[DEF_INI_BUFFSIZE];
	TCHAR wstrTMP[DEF_INI_BUFFSIZE];

	GCVT(strTMP,DEF_INI_BUFFSIZE,data,30);
	
	//マルチバイト文字列(char*)をワイド文字列(WCHAR*)に変換
	size_t wLen=0;
	errno_t err=0;

	err = mbstowcs_s(&wLen, wstrTMP, DEF_INI_BUFFSIZE, strTMP, DEF_INI_BUFFSIZE);
	return WriteString(strKey,wstrTMP);
}

//-----------------------------------------------------------------------------
//	<機能>　データ読み込み(文字列型)
//	<入力> 
//	TCHAR *strKey: キーの名称
//	TCHAR *strBuff: 読み出した文字列が格納される配列バッファへのポインタ
//	<出力> なし
//	<戻り値>
//	DWORD : バッファに格納された文字数
//	<特記事項> なし
//-----------------------------------------------------------------------------
DWORD IniFileControl::ReadData( TCHAR *strKey, TCHAR *strBuff )
{
	return ReadString(strKey,strBuff);
}

//-----------------------------------------------------------------------------
//	<機能>　データ読み込み(int型)
//	<入力> 
//	TCHAR *strKey: キーの名称
//	int *data: データが格納される領域へのポインタ
//	<出力> なし
//	<戻り値>
//	DWORD : バッファに格納されたデータの桁数
//	<特記事項> なし
//-----------------------------------------------------------------------------
DWORD IniFileControl::ReadData( TCHAR *strKey, int *data )
{
	DWORD dtsize;
	TCHAR strTMP[DEF_INI_BUFFSIZE];

	//データを文字列として読み込み
	if( (dtsize=ReadString(strKey,strTMP))!=0 ){
		if(STRSTR(strTMP,".")!=NULL){	//double型かどうかチェック
			*data=(int)ATOF(strTMP);	//double型の場合はintにキャストして格納
		}
		else *data=ATOI(strTMP);
	}
	return dtsize;
}
//-----------------------------------------------------------------------------
//	<機能>　データ読み込み(double型)
//	<入力> 
//	TCHAR *strKey: キーの名称
//	double *data: データが格納される領域へのポインタ
//	<出力> なし
//	<戻り値>
//	DWORD : バッファに格納されたデータの桁数
//	<特記事項> なし
//-----------------------------------------------------------------------------
DWORD IniFileControl::ReadData( TCHAR *strKey, double *data )
{
	TCHAR strTMP[DEF_INI_BUFFSIZE];
	DWORD dtsize;

	if( (dtsize=ReadString(strKey,strTMP))!=0 ){
		*data=ATOF(strTMP);
	}

	return dtsize;
}

//-----------------------------------------------------------------------------
//	<機能>　指定キーのデータを削除
//	<入力> 
//	TCHAR *AppName: 対象アプリケーション名
//	TCHAR *strKey: キーの名称(NULL指定で全要素削除)
//	<出力> なし
//	<戻り値>
//	BOOL : 成功時=非負値
//	<特記事項> なし
//-----------------------------------------------------------------------------
BOOL IniFileControl::DeleteData( TCHAR *AppName, TCHAR *strKey )
{
	return WritePrivateProfileString(AppName,strKey,NULL,FileName);
}

