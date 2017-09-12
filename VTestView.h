
// VTestView.h : CVTestView クラスのインターフェイス
//


/****************インクルードファイル****************/
#pragma once
#include "StdAfx.h"
#include <stdio.h>
#include <tchar.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <math.h>
#include <complex>
#include <omp.h>
#include <stdlib.h>
#include <windows.h>
#include "DGFilter.h"
#include <time.h>
/****************************************************/


/*********************定数の定義*********************/	
#define PORT 9876				//2台のPC間の通信を識別するPORT番号を指定
#define NumberOfFilter 2		//フィルタの次数
#define Chnumber 126			//脳波計のCh数
#define data_num 80
#define M 10			        //ARモデルの次数	
#define send_SF 3				//脳波計から受信したデータ数
#define smpf  100
#define smpf_dis 33
#define sp 0.2
#define line_sp 41				//8*(1/sp)+1
#define column_sp 66			//13*(1/sp)+1
#define file_time 15
/****************************************************/



class CVTestView : public CView
{
protected: // シリアル化からのみ作成します。
	CVTestView();
	DECLARE_DYNCREATE(CVTestView)

// オーバーライド
public:
	virtual void OnDraw(CDC* pDC);  // このビューを描画するためにオーバーライドされます。
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:

// 生成された、メッセージ割り当て関数
protected:
	DECLARE_MESSAGE_MAP()
public:
	/*********************関数の定義*********************/
	void ready_data(CDC* pDC);
	void ready_plot(CDC* pDC);
	void ready_file(CDC* pDC);
	void connection(void);				//ソケット通信接続プログラム
	void Recv(void);
	void data_stk(int i);
	void bandpass(void);
	void format_ar(void);				//ARモデルための初期化プログラム
	void autocorrelation(void);			//自己相関関数を計算
	void set_determinant(void);			//ARモデルの重みを計算するための行列をSETする
	void inverse_m1(void);				//m1の逆行列を計算
	void ar_a(void);					//ARモデルの重みパラメータを計算
	void ar_sigma(void);				//ARモデルのσ^2パラメータを計算
	void psd_ar(void);					//ARモデルの各パラメータからPSDを計算
	void ar_yule(void);
	double spline(int sct_num, double *x, double *y, double x1,
              double *h, double *b, double *d, double *g, double *u, double *r);
	void spline_psd(void);
	void MaxPSD(void);
	void format_plot(CDC* pDC);
	void lowpass(void);
	void direction(CDC* pDC);
	void plot_position(CDC* pDC);
	void plot_speed(CDC* pDC);
	void plot_psd(CDC* pDC);
	void plot_freq(CDC* pDC);
	void file_update(void);
	void fin(void);
	/****************************************************/


	/*********************変数の定義*********************/
	/*ソケット通信*/
	int srcSocket;
	int dstSocket;
	struct sockaddr_in srcAddr;
	struct sockaddr_in dstAddr;
	int dstAddrSize;
	int numrcv;
	int status;

	/*ソケット通信のデータ関連*/
	double buffer[Chnumber];						//受信データ
	double sig_data[Chnumber];
	double ydata[data_num][Chnumber];					//ARモデルに使用するデータ

	/*カウントする変数*/
	double *p;
	int i,i1,j,num,x,y,k,s,n,m;
	double y1, qi, si, xx;
	double psd_M;
	double F,dF,F_t;
	int deg_spd;
	int count_sp;
	int count_ar;									//ARモデルを始めるためのカウント変数
	int count_check;
	int count_file;
	int count_anl;

	/*ARモデルのパラメータを導出するための変数*/
	DGFilter *Filter_b[Chnumber];					//フィルタプログラムの定義
	DGFilter *Filter_c;
	DGFilter *Filter_l;
	DGFilter *Filter_dx;
	DGFilter *Filter_dy;
	DGFilter *Filter_spd;
	double C[data_num][Chnumber];						//ARモデルで扱う自己相関関数
	double m1[M][M][Chnumber];				//ARモデルの重みパラメータを算出するM×M行列
	double m1_inv[M][M][Chnumber];			//行列m1の逆行列
	double m2[M][Chnumber];						//ARモデルの重みパラメータを算出するM×1行列
	double w[M][Chnumber];							//ARモデルの重みパラメータ
	double sigma_1[Chnumber];
	double sigma[Chnumber];							//ARモデルのσ^2パラメータ
	double psd_1[Chnumber];
	double psd_2[Chnumber];
	double psd_data;
	double psd[Chnumber];							//ARモデルにより算出されるパワースペクトル密度
	double max_psd[Chnumber];						//各電極における最大パワースペクトル密度
	double max_F[Chnumber];							//各電極における最大周波数
	double tes_P;
	clock_t start_fp,end_fp;
	clock_t start_rcv,end_rcv;
	clock_t start_anl,end_anl;
	double duration_spd;
	double duration_rcv;							//データ抽出の間隔時間
	double duration_anl;							//データ解析の間隔時間
	double duration_file;

	/*スプライン補間を行うための変数*/
	double *h_column, *b_column, *c_column, *g_column, *u_column, *r_column, *x_column, *y_column;
	double *h_line, *b_line, *c_line, *g_line, *u_line, *r_line, *x_line, *y_line;
	double x1, y_out_column, y_out_line, y_column_sp[9][column_sp],y_line_sp[line_sp][column_sp];
	int sct_column_num,sct_line_num,column,line;

	/*プロットするための変数*/
	double max_max_psd;								//全電極の中における最大パワースペクトル密度
	double max_psd_line,max_psd_line0;				//全電極の中における最大PSDを示した行
	double max_psd_column,max_psd_column0;			//全電極の中における最大PSDを示した列										
	int d;											//電極を特定する変数
	CString str;									//電極番号の表示変数
	int ox, oy;										//画面の中心座標
	double S_ox,S_oy;
	CRect screen;									//画面の大きさを取得する変数
	int px, py,px0, py0;							//一時刻前の位置と現在の位置を示す変数
	double drct_px,drct_py,drct_px0,drct_py0;
	double drct_x,drct_y,drct_a,drct_b,drct_m;
	int drct_x0,drct_y0;
	int clr[line_sp][column_sp];
	double px_psd,py_psd;
	double distance;								//魚が進んだ距離を示す変数
	double Speed;									//魚の速度を示す変数
	int Speed0,int_Speed;
	CStdioFile ofp1,ofp2,ofp3;	
	double S_ratio;									//実際の速度に変換するための変数
	double freq,freq0;
	int int_freq;
	int pastMs,pastMs0,x_time,end_time;
	int pastMsf,pastMsf0,x_timef,end_timef;
	/****************************************************/
};

//#ifndef _DEBUG  // VTestView.cpp のデバッグ バージョン
//inline CVTestDoc* CVTestView::GetDocument() const
//   { return reinterpret_cast<CVTestDoc*>(m_pDocument); }
//#endif

