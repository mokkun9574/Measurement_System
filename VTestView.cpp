
// VTestView.cpp : CVTestView クラスの実装
//

#include "stdafx.h"
#ifndef SHARED_HANDLERS
#include "VTest.h"
#endif

#include "VTestDoc.h"
#include "VTestView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CPen p1(PS_DOT, 1, RGB(200, 200, 200));
CPen p3(PS_SOLID, 20, RGB(255, 255, 255));							
CPen p4(PS_SOLID, 20, RGB(0, 0, 0));
CPen p5(PS_SOLID, 500, RGB(255, 255, 255));
CPen p6(PS_SOLID, 3, RGB(255, 0, 0));
CPen p7(PS_SOLID, 1, RGB(0, 0, 0));
CPen p9(PS_SOLID, 3, RGB(0, 0, 0));
CPen p13(PS_SOLID, 3, RGB(255, 255, 255));
CPen *oldp1,*oldp3,*oldp4,*oldp5,*oldp6,*oldp7,*oldp9,*oldp11,*oldp13;


// CVTestView

IMPLEMENT_DYNCREATE(CVTestView, CView)

BEGIN_MESSAGE_MAP(CVTestView, CView)
	// 標準印刷コマンド
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CVTestView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CVTestView コンストラクション/デストラクション


CVTestView::CVTestView()
{
	connection();	//ソケット通信開始
	MessageBox(TEXT("ソケット作成完了\n接続を待機中."));
	dstSocket=accept(srcSocket, (struct sockaddr *) &dstAddr, &dstAddrSize);
}

BOOL CVTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}



void CVTestView::OnDraw(CDC* pDC)
{
	ready_data(pDC);
	ready_plot(pDC);
	//ready_file(pDC);
	while(1){
		Recv();								//生体信号データの受信
		bandpass();							//バンドパスフィルタ[2 10]
		data_stk(0);						//信号データの格納
		count_ar++;							//パラメータ開始条件をカウント
		if(count_ar>=send_SF){				//ARモデルパラメータ推定開始		
			ar_yule();						//ARモデル（Yule Walker法）
			//plot_psd(pDC);				//PSD分布のプロット
			plot_position(pDC);				//推定位置のプロット
			plot_speed(pDC);				//遊泳速度のプロット
			//direction(pDC);
			plot_freq(pDC);
			//file_update();
			data_stk(1);					//send_SFデータ分だけydataをずらす
			count_ar=0;						//パラメータ開始条件のリセット
		}
	}
	fin();
}





/*************以下は位置・方向推定プログラムの関数*************/

//各変数対する初期化プログラム
void CVTestView::ready_data(CDC* pDC){
	count_ar=0;
	count_file=0;
	dF=0.1;
	for( i=0 ; i<Chnumber ; i++){
		Filter_b[i] = new DGFilter;
		Filter_b[i] ->SetButterBPF(NumberOfFilter, 2, 10, smpf);
	}
	Filter_c   = new DGFilter;
	Filter_l   = new DGFilter;
	Filter_spd = new DGFilter;
	Filter_c   ->SetButterLPF(NumberOfFilter, 0.5 , smpf_dis);
	Filter_l   ->SetButterLPF(NumberOfFilter, 0.5 , smpf_dis);
	Filter_spd ->SetButterLPF(NumberOfFilter, 0.5 , smpf_dis);

	sct_column_num = 13;
	sct_line_num = 8;
	h_column = new double [sct_column_num];		h_line = new double [sct_line_num];
	b_column = new double [sct_column_num];		b_line = new double [sct_line_num];
	c_column = new double [sct_column_num];		c_line = new double [sct_line_num];
	g_column = new double [sct_column_num];		g_line = new double [sct_line_num];
	u_column = new double [sct_column_num];		u_line = new double [sct_line_num];
	r_column = new double [sct_column_num+1];	r_line = new double [sct_line_num+1];
	x_column = new double [sct_column_num+1];	x_line = new double [sct_line_num+1];
	y_column = new double [sct_column_num+1];	y_line = new double [sct_line_num+1];
}

//プロット準備
void CVTestView::ready_plot(CDC* pDC){
	px=0, py=0,drct_x0=0,drct_y0=0,drct_a=0;
	pastMs=0,pastMs0=50,end_time=0;
	pastMsf=0,pastMsf0=50,end_timef=0;
	duration_spd = 0.033;	// smpf/send_SF
    GetClientRect(screen);
    ox = screen.right/2;
    oy = screen.bottom/2;
	S_ox = ox; S_oy = oy;
	S_ratio      = (210*140)/(S_ox*S_oy);
	Speed0=1.875*oy;
	freq0=1.875*oy;
	oldp7=pDC->SelectObject(&p7);
	pDC      ->MoveTo(50,1.875*oy); pDC->LineTo(50,1.875*oy-30*10);
	pDC      ->MoveTo(50,1.875*oy);  pDC->LineTo(ox-50, 1.875*oy);
	for( deg_spd=1; deg_spd<=10; deg_spd++){
		pDC->MoveTo(50,1.875*oy-30*deg_spd); pDC->LineTo(50+5,1.875*oy-30*deg_spd);
	}
	pDC->MoveTo(ox+50,1.875*oy); pDC->LineTo(ox+50,1.875*oy-50*6);
	pDC->MoveTo(ox+50,1.875*oy);  pDC->LineTo(2*ox-50, 1.875*oy);
	for( deg_spd=1; deg_spd<=6; deg_spd++){
		pDC->MoveTo(ox+50,1.875*oy-50*deg_spd); pDC->LineTo(ox+50+5,1.875*oy-50*deg_spd);
	}
	pDC->TextOutW(ox/2-50,1.875*oy-30*10-5,_T("Frequency [Hz]"));
	pDC->TextOutW(30,1.875*oy-5,_T(" 0"));
	pDC->TextOutW(30,1.875*oy-30*10-5,_T("10"));
	pDC->TextOutW(ox*1.5-50,1.875*oy-30*10-5,_T("Speed [mm/s]"));
	pDC->TextOutW(ox+30,1.875*oy-5,_T(" 0"));
	pDC->TextOutW(ox+30,1.875*oy-30*10-5,_T("60"));
	pDC->SelectObject(oldp7);
}

//ファイル準備
void CVTestView::ready_file(CDC* pDC){
	start_fp=clock();
	/*str.Format(_T("data_%d.txt"), count_file);
	if(ofp1.Open(str,CFile::modeCreate | CFile::modeWrite)==NULL){
		TRACE("error\n");
	}*/
	str.Format(_T("F_%d.csv"), count_file);
	if(ofp2.Open(str,CFile::modeCreate | CFile::modeWrite)==NULL){
		TRACE("error\n");
	}
	str.Format(_T("speed_%d.csv"), count_file);
	if(ofp3.Open(str,CFile::modeCreate | CFile::modeWrite)==NULL){
		TRACE("error\n");
	}
}

//ソケット通信接続プログラム
void CVTestView::connection(void){
	dstAddrSize=sizeof(dstAddr);
	WSADATA data;
	WSAStartup(MAKEWORD(2,0),&data);
	memset(&srcAddr,0,sizeof(srcAddr));
	srcAddr.sin_port=htons(PORT);
	srcAddr.sin_family=AF_INET;
	srcAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	srcSocket=socket(AF_INET,SOCK_STREAM,0);
	bind(srcSocket,(struct sockaddr *) &srcAddr,sizeof(srcAddr));
	listen(srcSocket,1);
}

//データ受信プログラム
void CVTestView::Recv(void){
	numrcv=recv(dstSocket,(char*)buffer,sizeof(buffer),0);	//生体電気信号データを受信
	/*for( num=0 ; num<Chnumber ; num++){
		str.Format(_T("    %.2f"), buffer[num]);
		ofp1.WriteString(str);
	}	
	ofp1.WriteString(_T("    0.00    0.00\n"));*/
}

//データ格納プログラム
void CVTestView::data_stk(int i){
	if(i==0){
		for( i=0 ; i<Chnumber ; i++){
			ydata[data_num-send_SF+count_ar][i]=buffer[i];
		}
	}
	if(i==1){
		for( i=0 ; i<Chnumber ; i++){
			for( j=0; j<data_num-send_SF; j++ ){
				ydata[j][i]=ydata[j+send_SF][i];}
		}
	}
}

//バンドパスフィルタ(2-10Hz,100Hz)
void CVTestView::bandpass(void){
	for( i=0 ; i<Chnumber ; i++){
		buffer[i] = Filter_b[i]->filter(buffer[i]);}
}

//ARモデルための初期化プログラム
void CVTestView::format_ar(void){
	for( num=0 ; num<Chnumber ; num++){
		for( y=0;y<M;y++){
			for( x=0;x<M;x++){
				m1_inv[y][x][num] = 0;}
		}
		for( i=0;i<M;i++){
			w[i][num] = 0;}
		sigma_1[num]   = 0;
	}
}

//自己相関関数を計算
void CVTestView::autocorrelation(void){
	for( num=0 ; num<Chnumber ; num++ ){
		for( j=0; j<data_num; j++ ){
			for( i=0; i<data_num-j; i++ ){
				C[j][num] += ydata[i][num]*ydata[i+j][num];}
			C[j][num] = C[j][num] / data_num;
		}
	}
}

//ARモデルの重みを計算するための行列をSETする
void CVTestView::set_determinant(void){
	for( num=0 ; num<Chnumber ; num++){
		k=0,m=0;
		for( y=0; y<M; y++){
			for( x=0; x<M-k; x++){
				m1[y][x+k][num] = C[m][num];
				m++;}
			m = 0;
			k++;}
		k = M-1;
		for( y=1; y<M; y++){
			for( x=0; x<M-k; x++){
				m1[y][x][num] = C[y-m][num];
				m++;}
			m=0;
			k--;}
		k=1;
		for( y=0; y<M; y++){
			m2[y][num]=C[k][num];
			k++;}
	}
}

//m1の逆行列を計算
void CVTestView::inverse_m1(void){
	for( num=0 ; num<Chnumber ; num++){
		n=M;	
		/*	単位行列の作成	*/
		for( i=0;i<n;i++ ){
			m1_inv[i][i][num]=1.0;}

		/*****	逆行列の計算	*****/
		for( k=0;k<n;k++){
			for( j=n-1;j>=0;j--){m1_inv[k][j][num] /= m1[k][k][num];}
			for( j=n-1;j>=0;j--){m1[k][j][num] /= m1[k][k][num];}
			for( i=0;i<n;i++){
				if( i!=k ){
					for( j=n-1;j>=0;j--){
						m1_inv[i][j][num] -= m1[i][k][num]*m1_inv[k][j][num];}
					for( j=n-1;j>=0;j--){
						m1[i][j][num] -= m1[i][k][num]*m1[k][j][num];}}}}
	}
}

//ARモデルの重みパラメータを計算
void CVTestView::ar_a(void){
	for( num=0 ; num<Chnumber ; num++){
		for( y=0; y<M; y++){
			for( x=0; x<M; x++){
				w[y][num] += m1_inv[y][x][num] * m2[x][num];}
		}
	}
}

//ARモデルのσ^2パラメータを計算
void CVTestView::ar_sigma(void){
	for( num=0 ; num<Chnumber ; num++){
		for( j=0;j<M;j++){
			sigma_1[num] += w[j][num] * C[j+1][num];
		}
		sigma[num] = C[0][num] - sigma_1[num];
	}
}

//ARモデルの各パラメータからPSDを計算し、最大PSDを各電極に格納
void CVTestView::psd_ar(void){
	freq=0; tes_P=0;
	for( num=0 ; num<Chnumber ; num++){
		max_psd[num]=0; max_F[num]=0,F=0;
		while( F < 10 ){
			psd_1[num] = 1; psd_2[num] = 0;
			F_t=F/smpf;
			for( psd_M=0; psd_M<M; psd_M++){
				j=psd_M;
				psd_data=2*PI*(psd_M+1)*F_t;
				psd_1[num] -= w[j][num] * cos(psd_data);
				psd_2[num] += w[j][num] * sin(psd_data);
			}
			psd[num] = psd_1[num]*psd_1[num] + psd_2[num]*psd_2[num];
			psd[num] = sigma[num]/psd[num];
			if(psd[num] > max_psd[num]){
				max_psd[num] = psd[num];
				max_F[num]	 = F;}
			F=F+dF;
		}
		if(max_psd[num] > tes_P){
			tes_P=max_psd[num];
			freq=max_F[num];
		}
	}
}

//電極間におけるスプライン補間を行う関数
double CVTestView::spline(int sct_num, double *x, double *y, double x1,
              double *h, double *b, double *c, double *g, double *u, double *r){
	i = -1, i1, k;
	for (i1 = 1; i1 < sct_num && i < 0; i1++) {
		if (x1 < x[i1])
			i = i1 - 1;
	}
	if (i < 0)
		i = sct_num - 1;
	for (i1 = 0; i1 < sct_num; i1++)
		h[i1] = x[i1+1] - x[i1];
	for (i1 = 1; i1 < sct_num; i1++) {
		b[i1] = 2.0 * (h[i1] + h[i1-1]);
		c[i1] = 3.0 * ((y[i1+1] - y[i1]) / h[i1] - (y[i1] - y[i1-1]) / h[i1-1]);
	}
	g[1] = h[1] / b[1];
	for (i1 = 2; i1 < sct_num-1; i1++)
		g[i1] = h[i1] / (b[i1] - h[i1-1] * g[i1-1]);
	u[1] = c[1] / b[1];
	for (i1 = 2; i1 < sct_num; i1++)
		u[i1] = (c[i1] - h[i1-1] * u[i1-1]) / (b[i1] - h[i1-1] * g[i1-1]);
	k      = (i > 1) ? i : 1;
	r[0]   = 0.0;
	r[sct_num]   = 0.0;
	r[sct_num-1] = u[sct_num-1];
	for (i1 = sct_num-2; i1 >= k; i1--)
		r[i1] = u[i1] - g[i1] * r[i1+1];
	xx = x1 - x[i];
	qi = (y[i+1] - y[i]) / h[i] - h[i] * (r[i+1] + 2.0 * r[i]) / 3.0;
	si = (r[i+1] - r[i]) / (3.0 * h[i]);
	y1 = y[i] + xx * (qi + xx * (r[i]  + si * xx));
	return y1;
}

//電極間において、横⇒縦の順でspline関数を呼び出し補間する
void CVTestView::spline_psd(void){
	for( count_sp=0; count_sp<9; count_sp++){
		for( i=sct_column_num*count_sp+count_sp+1; i<=sct_column_num*(count_sp+1)+count_sp+1; i++){
			d=i%14;
			if(d==0){d=14;}
			x_column[d-1] = d-1;
			y_column[d-1] = max_psd[i-1];
		}
		x1 = 0.0;
		column=0;
		while (x1 < sct_column_num) {
			y_out_column = spline(sct_column_num, x_column, y_column, x1, h_column, b_column, c_column, g_column, u_column, r_column);
			y_column_sp[count_sp][column] = y_out_column;
			x1 += sp;
			column++;
		}
	}

	for( count_sp=0; count_sp<column_sp; count_sp++){
		for( i=sct_line_num*count_sp+count_sp+1; i<=sct_line_num*(count_sp+1)+count_sp+1; i++){
			d=i%9;
			if(d==0){d=9;}
			x_line[d-1] = d-1;
			y_line[d-1] = y_column_sp[d-1][count_sp];
		}
		x1 = 0.0;
		line=0;
		while (x1 < sct_line_num) {
			y_out_line = spline(sct_line_num, x_line, y_line, x1, h_line, b_line, c_line, g_line, u_line, r_line);
			y_line_sp[line][count_sp]=y_out_line;
			x1 += sp;
			line++;
		}
	}
}

//スプライン補間を行った後の最大PSDを示した位置を指定する
void CVTestView::MaxPSD(void){
	max_psd_line   = 0;
	max_psd_column = 0;
	max_max_psd    = 0;
	for(i=0 ; i<line_sp ; i++){
		for( j=0 ; j<column_sp ; j++){
			if(y_line_sp[i][j]>max_max_psd){
				max_max_psd    = y_line_sp[i][j];
				max_psd_line   = i+1;
				max_psd_column = j+1;}
		}
	}
	max_psd_column = max_psd_column/column_sp;
	max_psd_line   = max_psd_line/line_sp;
}

//ARモデル（yule-walker法）
void CVTestView::ar_yule(void){
	format_ar();					//ARモデルための初期化プログラム
	autocorrelation();				//自己相関関数Cを計算
	set_determinant();				//ARモデルの重みを計算するための行列m1をSETする
	inverse_m1();					//m1の逆行列を計算
	ar_a();							//ARモデルの重みパラメータを計算
	ar_sigma();						//ARモデルのσ^2パラメータを計算
	psd_ar();						//ARモデルの各パラメータからPSDを計算
	spline_psd();					//電極間におけるスプライン補間
	MaxPSD();						//最大PSDを示す場所を特定
}

//プロットを行うための初期化プログラム
void CVTestView::format_plot(CDC* pDC){
	px0 = px;
	py0 = py;
	drct_px0 = drct_px;
	drct_py0 = drct_py;
	if( x_time >= ox - 100 ){
		end_time = pastMs;
		end_timef = pastMsf;
		pastMs0  = 50;
		pastMsf0 = 50;
		oldp5    = pDC->SelectObject(&p5);
		pDC      ->MoveTo(0,1.5*oy);
		pDC      ->LineTo(2*ox,1.5*oy);
		oldp7    = pDC->SelectObject(&p7);
		pDC      ->MoveTo(50,1.875*oy); pDC->LineTo(50,1.875*oy-30*10);
		pDC      ->MoveTo(50,1.875*oy);  pDC->LineTo(ox-50, 1.875*oy);
		pDC      ->MoveTo(ox+50,1.875*oy); pDC->LineTo(ox+50,1.875*oy-50*6);
		pDC      ->MoveTo(ox+50,1.875*oy);  pDC->LineTo(2*ox-50, 1.875*oy);
		for( deg_spd=1; deg_spd<=10; deg_spd++){
			pDC->MoveTo(50,1.875*oy-30*deg_spd); pDC->LineTo(50+5,1.875*oy-30*deg_spd);
		}
		for( deg_spd=1; deg_spd<=6; deg_spd++){
			pDC->MoveTo(ox+50,1.875*oy-50*deg_spd); pDC->LineTo(ox+50+5,1.875*oy-50*deg_spd);
		}
		pDC->TextOutW(ox/2-50,1.875*oy-30*10-5,_T("Frequency [Hz]"));
		pDC->TextOutW(30,1.875*oy-5,_T(" 0"));
		pDC->TextOutW(30,1.875*oy-30*10-5,_T("10"));
		pDC->TextOutW(ox*1.5-50,1.875*oy-30*10-5,_T("Speed [mm/s]"));
		pDC->TextOutW(ox+30,1.875*oy-5,_T(" 0"));
		pDC->TextOutW(ox+30,1.875*oy-30*10-5,_T("60"));
	}
}

//2時刻前までの位置座標にローパスフィルタをかける
void CVTestView::lowpass(void){
	max_psd_column = Filter_c->filter( max_psd_column );
	max_psd_line   = Filter_l->filter( max_psd_line );
}

//推定位置をプロットする
void CVTestView::plot_position(CDC* pDC){
	format_plot(pDC);
	oldp3=pDC->SelectObject(&p3);
	pDC->MoveTo(px,py);
	pDC->LineTo(px,py);
	lowpass();
	px = (13*ox/15)*(max_psd_column)+ox/15;
	py = (8*oy/10)*(max_psd_line)+oy/10;
	oldp4=pDC->SelectObject(&p4);
	pDC->MoveTo(px,py);
	pDC->LineTo(px,py);
	pDC->SelectObject(oldp4);
	oldp1=pDC->SelectObject(&p1);
	for( i=1;i<=9;i++){
		for( j=1;j<=14;j++){
			pDC->MoveTo(0, (oy*i)/10);   pDC->LineTo(ox, (oy*i)/10);
			pDC->MoveTo((ox*j)/15,0);  pDC->LineTo((ox*j)/15, oy);}}
	oldp7=pDC->SelectObject(&p7);
	pDC->MoveTo(0, (oy*10)/10);   pDC->LineTo(ox, (oy*10)/10);
	pDC->MoveTo((ox*15)/15,0);  pDC->LineTo((ox*15)/15, oy);
	/*pDC->MoveTo(0, (oy*1)/10);   pDC->LineTo(ox, (oy*1)/10);
	pDC->MoveTo(0, (oy*9)/10);   pDC->LineTo(ox, (oy*9)/10);
	pDC->MoveTo((ox*1)/15,0);  pDC->LineTo((ox*1)/15, oy);
	pDC->MoveTo((ox*14)/15,0);  pDC->LineTo((ox*14)/15, oy);*/
}

//遊泳速度をプロット
void CVTestView::plot_speed(CDC* pDC){
	distance     = (px-px0)*(px-px0) + (py-py0)*(py-py0);
	distance     = sqrt( distance );
	distance     = distance*S_ratio;					//単位[mm]
	Speed        = ( distance / duration_spd );			//単位[mm/s]
	Speed        = Filter_spd->filter( Speed );
	int_Speed    = Speed*5;
	if(int_Speed>300){
		int_Speed=300;
	}

	pastMs  +=1;
	x_time  = pastMs - end_time + 50;
	oldp6   = pDC->SelectObject(&p6);
	pDC     ->MoveTo(ox+pastMs0,Speed0);
	pDC     ->LineTo(ox+x_time,1.875*oy-int_Speed);
	pastMs0 = x_time;
	Speed0  = 1.875 * oy - int_Speed;
	pDC     ->SelectObject(oldp6);
}

//頭部方向を推定
void CVTestView::direction(CDC* pDC){
	oldp13=pDC->SelectObject(&p13);
	pDC->MoveTo(px0,py0);
	pDC->LineTo(drct_x0,drct_y0);
	oldp4=pDC->SelectObject(&p4);
	pDC->MoveTo(px,py);
	pDC->LineTo(px,py);
	if(Speed>0.5){
		drct_px=max_psd_column;
		drct_py=max_psd_line;
		drct_x=drct_px-drct_px0;
		drct_y=drct_py-drct_py0;
		drct_a=drct_y/drct_x;
		drct_b=drct_a*drct_a+1;
		if(drct_x<0){drct_m=-0.1;}
		else{drct_m=0.1;}
		drct_x=drct_m/sqrt(drct_b);
		drct_y=drct_a*drct_x;
		drct_x=drct_px+drct_x;
		drct_y=drct_py+drct_y;
		drct_x0 = (13*ox/15)*(drct_x)+ox/15;
		drct_y0 = (8*oy/10)*(drct_y)+oy/10;
	}
	oldp9=pDC->SelectObject(&p9);
	pDC->MoveTo(px,py);
	pDC->LineTo(drct_x0,drct_y0);
	pDC->SelectObject(oldp9);
}

//PSDをプロット（　青:　0 ～ 1　:赤　）
void CVTestView::plot_psd(CDC* pDC){
	for( i=0 ; i<line_sp ; i++){
		for( j=0 ; j<column_sp ; j++){
			y_line_sp[i][j] = y_line_sp[i][j] / max_max_psd;
			clr[i][j] = y_line_sp[i][j] * 255;
			if( clr[i][j] < 0.8 ){
				clr[i][j] = 0;}}
	}
	for( i=0 ; i<line_sp ; i++){
		for( j=0 ; j<column_sp ; j++){
			CPen p11(PS_SOLID, 15, RGB(clr[i][j], 0, 255-clr[i][j]));
			oldp11 = pDC->SelectObject(&p11);
			px_psd = j+1;
			py_psd = i+1;
			px_psd = ox + (px_psd/column_sp)*ox;
			py_psd = (py_psd/line_sp)*oy;
			pDC->MoveTo(px_psd, py_psd);pDC->LineTo(px_psd, py_psd);
		}
	}
	pDC ->SelectObject(oldp11);
}

//推定位置における周波数をプロット
void CVTestView::plot_freq(CDC* pDC){
	int_freq=freq*30;
	pastMsf  +=1;
	x_timef  = pastMsf-end_timef+50;
	oldp6    = pDC->SelectObject(&p6);
	pDC      ->MoveTo(pastMsf0,freq0);
	pDC      ->LineTo(x_timef,1.875*oy-int_freq);
	pastMsf0 = x_timef;
	freq0    = 1.875*oy-int_freq;
	pDC      ->SelectObject(oldp6);
}

//ファイルを一時保存し、新たなファイルを作成
void CVTestView::file_update(void){
	str.Format(_T("%f\n"), Speed);
	ofp3.WriteString(str);
	str.Format(_T("%f\n"), freq);
	ofp2.WriteString(str);
	end_fp=clock();
	duration_file = double(end_fp - start_fp) / CLOCKS_PER_SEC;
	if( duration_file > file_time){
		//ofp1.Close();
		ofp2.Close();
		ofp3.Close();
		count_file++;
		/*str.Format(_T("data_%d.txt"), count_file);
		if(ofp1.Open(str,CFile::modeCreate | CFile::modeWrite)==NULL){
			TRACE("error\n");
		}*/
		str.Format(_T("F_%d.csv"), count_file);
		if(ofp2.Open(str,CFile::modeCreate | CFile::modeWrite)==NULL){
			TRACE("error\n");
		}
		str.Format(_T("speed_%d.csv"), count_file);
		if(ofp3.Open(str,CFile::modeCreate | CFile::modeWrite)==NULL){
			TRACE("error\n");
		}
		start_fp=clock();
	}
}

//終了プログラム
void CVTestView::fin(void){
	WSACleanup();
	for( i=0 ; i<Chnumber ; i++){
		delete Filter_b[i];
	}
	delete Filter_c;
	delete Filter_l;
	delete Filter_spd;
	delete [] h_column;	delete [] h_line;
	delete [] b_column;	delete [] b_line;
	delete [] c_column;	delete [] c_line;
	delete [] g_column;	delete [] g_line;
	delete [] u_column;	delete [] u_line;
	delete [] r_column;	delete [] r_line;
	delete [] x_column;	delete [] x_line;
	delete [] y_column;	delete [] y_line;
}