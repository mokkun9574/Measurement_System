/****************************************************************************** 
	ディジタルフィルタクラス
	
	File name：DGFilter.cpp
	Version:	0.00 (xxxx/xx/xx)

	<機能> 
	各種ディジタルフィルタ処理を行うクラス
	<関連ファイル>
	xxxx.h	: XXXに用いるヘッダ
	xxxx.lib:　XXX関連ライブラリ
	<更新履歴>
	2012/05/22  Start-
	xxxx/xx/xx  xxxxxxxx  (by XXXX XXX)
	xxxx/xx/xx  xxxxxxxx Ver.1.02完成 (by XXXX XXXX)
	<特記事項>
	筋電組で使っているFilterClassをベースに作成開始．
	後々構造をすべて変更する予定だが，暫定的にそのまま使う．
	＜著作権表示＞
	Copyright © 2012 BSYS, Lab. All rights reserved.
****************************************************************************/
#include "DGFilter.h"
#include <stdio.h>
#include <math.h>

//------------------------------------------------
//             フィルタクラス初期化
//------------------------------------------------
DGFilter::DGFilter(void)
{
	int i;
	order = 0;
	for(i=0;i<DEF_DGFILTER_MAX_ORDER;i++)
	{
		input[i] = 0.0;
		output[i] = 0.0;
		a[i] = 0.0;
	}
	for(i=0;i<DEF_DGFILTER_MAX_ORDER+1;i++)
		b[i] = 0.0;
}

//------------------------------------------------
//            デジタルフィルタ
//    入力：現在の値
//    出力：フィルタ後の値
//------------------------------------------------
double DGFilter::filter(double now_value)
{	
	input[0] = now_value;

	switch(order)
	{
	case 1:	/**** １次 ****/
		output[0] = a[0]*output[1] + b[0]*input[0] + b[1]*input[1];
		input[1] = input[0];
		output[1] = output[0];
		break;
	case 2:	/**** 2次 ****/
		output[0] =a[0]*output[1] + a[1]*output[2]
					+ b[0]*input[0] + b[1]*input[1] + b[2]*input[2];
		input[2] = input[1];
		output[2] = output[1];
		input[1] = input[0];
		output[1] = output[0];
		break;
	case 3:
		output[0] = a[0] * output[1] + a[1] * output[2] + a[2] * output[3]
					+ b[0] * input[0] + b[1] * input[1] + b[2] * input[2] + b[3] * input[3];

		input[3] = input[2];
		output[3] = output[2];
		input[2] = input[1];
		output[2] = output[1];
		input[1] = input[0];
		output[1] = output[0];

		break;
	case 4:
		output[0] = a[0] * output[1] + a[1] * output[2] + a[2] * output[3] + a[3] * output[4]
					+ b[0] * input[0] + b[1] * input[1] + b[2] * input[2] + b[3] * input[3] + b[4] * input[4];

		input[4] = input[3];
		output[4] = output[3];
		input[3] = input[2];
		output[3] = output[2];
		input[2] = input[1];
		output[2] = output[1];
		input[1] = input[0];
		output[1] = output[0];

		break;
	case 5:
		output[0] = a[0] * output[1] + a[1] * output[2] + a[2] * output[3] + a[3] * output[4] + a[4] * output[5]
					+ b[0] * input[0] + b[1] * input[1] + b[2] * input[2] + b[3] * input[3] + b[4] * input[4] + b[5] * input[5];

		input[5] = input[4];
		output[5] = output[4];
		input[4] = input[3];
		output[4] = output[3];
		input[3] = input[2];
		output[3] = output[2];
		input[2] = input[1];
		output[2] = output[1];
		input[1] = input[0];
		output[1] = output[0];

		break;
	case 6:
		output[0] = a[0] * output[1] + a[1] * output[2] + a[2] * output[3] + a[3] * output[4] + a[4] * output[5] + a[5] * output[6]
					+ b[0] * input[0] + b[1] * input[1] + b[2] * input[2] + b[3] * input[3] + b[4] * input[4] + b[5] * input[5] + b[6] * input[6];

		input[6] = input[5];
		output[6] = output[5];
		input[5] = input[4];
		output[5] = output[4];
		input[4] = input[3];
		output[4] = output[3];
		input[3] = input[2];
		output[3] = output[2];
		input[2] = input[1];
		output[2] = output[1];
		input[1] = input[0];
		output[1] = output[0];

		break;

	default:
		printf("フィルタの次数がおかしい(DGFilter::filter();)\n");
		output[0]=0.0;
	}

	return output[0];
}
//------------------------------------------------
// order:次数(１,２のみ)
// fc:遮断周波数（Hz）
// fs:標本化周波数（Hz）
// m:バターワース(m=1)かチェビシェフ(m=2)を選択
//------------------------------------------------
bool DGFilter::SetButterLPF(int order_num,double cut_freq,double f_sampling)
{
	if(order_num>=DEF_DGFILTER_MAX_ORDER)return false;
	order=order_num;
	rpl=0;
	fc=cut_freq/1000.0;
	fs=f_sampling/1000.0;
	m=1;//バターワースを選択
	Coef_l();
	matrix_x_l();
	matrix_y_l();

	return true;
}

//------------------------------------------------
// order:次数(１,２のみ)
// fc:遮断周波数（Hz）
// fs:標本化周波数（Hz）
// rpl:リップル率（現在未使用）
// m:バターワース(m=1)かチェビシェフ(m=2)を選択
//------------------------------------------------
bool DGFilter::SetButterHPF(int order_num,double cut_freq,double f_sampling,double rpl_rate)
{
	order = order_num;
	rpl = rpl_rate;
	fc = cut_freq/1000.0;
	fs = f_sampling/1000.0;

	if(rpl == 0.0){		//リップルが0.0だと強制的にバターワースフィルタになる
		m = 1;
	}
	else{
		m = 2;
	}
	Coef_h();
	matrix_x_h();
	matrix_y_h();

	return true;
}

//------------------------------------------------
// order:次数(１,２のみ)
// fc:遮断周波数（Hz）
// fs:標本化周波数（Hz）
// rpl:リップル率（現在未使用）
// m:バターワース(m=1)かチェビシェフ(m=2)を選択
//------------------------------------------------
bool DGFilter::SetChebyshavLPF(int order_num,double cut_freq,double f_sampling,double rpl_rate)
{
	if(order_num>=DEF_DGFILTER_MAX_ORDER)return false;
	order=order_num;
	rpl=rpl_rate;
	fc=cut_freq/1000.0;
	fs=f_sampling/1000.0;
	m=2;//チェビシェフを選択
	Coef_l();
	matrix_x_l();
	matrix_y_l();

	return true;
}

//------------------------------------------------
// order:次数(１,２のみ)
// fc:遮断周波数-立ち上がり（KHz）
// h_fc:遮断周波数-立ち下がり（KHz）
// fs:標本化周波数（KHz）
//------------------------------------------------
bool DGFilter::SetButterBPF(int order_num, double low_cut_freq, double high_cut_freq, double f_sampling)
{
	if(order_num>=DEF_DGFILTER_MAX_ORDER)return false;
	order = order_num;
	fc = low_cut_freq / 1000.0;
	h_fc = high_cut_freq / 1000.0;
	fs = f_sampling / 1000.0;

	m = 1;

	Coef_b();
	matrix_x_b();
	matrix_y_b();
	order *= 2;

	return true;
}

//------------------------------------------------
//　LPF独自のパラメータを決定
//------------------------------------------------
void DGFilter::Coef_l(void)
{
	double w0,wa,u,zt;
	int j;
	int n;

	if(m==2)
	u = (1.0 / order) * ArcSinh(1.0 / sqrt(pow(10.0,0.1 * rpl) - 1));

	wa = tan(PI * fc / fs);
	
	if((order % 2)==0)	n = 1;
	else			n = 2;

	for(j=1;j<=(int)(order / 2);j++)
	{
	switch(m){
		case 1:	zt = cos(n * PI / (2 * order));
			 	w0 = 1.0;
				break;
		case 2:  w0 = sqrt(
			 	( (sinh(u) * cos(n * PI / (2 * order)))*
			 	(sinh(u) * cos(n * PI / (2 * order)))) +
			 	((cosh(u) * sin(n * PI / (2 * order)))*
			 	(cosh(u) * sin(n * PI / (2 * order))))
			 	);
			 	zt = sinh(u) * cos(n * PI / (2 * order)) / w0;
			 	break;
              }

	matrix_a[0][j]=1.0 + wa*w0*2.0*zt + (wa*w0)*(wa*w0);
	matrix_a[1][j]=2.0 * ((wa*w0)*(wa*w0) - 1.0) / matrix_a[0][j];
	matrix_a[2][j]=(1.0 - wa*w0*2.0*zt + (wa*w0)*(wa*w0)) / matrix_a[0][j];
	matrix_b[0][j]=(wa*w0)*(wa*w0) / matrix_a[0][j];
	matrix_b[1][j]=2.0 * matrix_b[0][j];

	n = n + 2;
	
	}

	if(m==1)
	    w0 = 1.0;
	if(m==2)
	    w0 = sinh(u);

	if((order % 2)==1)
	{
		j = (int)(order / 2) + 1;
		matrix_a[0][j] = 1.0 + wa*w0;
		matrix_a[1][j] = (wa*w0 - 1.0) / matrix_a[0][j];
		matrix_b[0][j] = wa*w0 / matrix_a[0][j];
		matrix_b[1][j] = matrix_b[0][j];
	}
}

//------------------------------------------------
//　HPF独自のパラメータを決定
//------------------------------------------------
void DGFilter::Coef_h()
{
	double wa,zt;
	int j;
	int n;

	wa = tan(PI * fc / fs);
	
	if((order % 2)==0)
		n = 1;
	else
		n = 2;

	for(j=1;j<=(int)(order / 2);j++)
	{
		zt = cos(n * PI / (2 * order));

		matrix_a[0][j] = 1.0 + wa*2.0*zt + wa*wa;
		matrix_a[1][j] = -2.0 * (1.0 - wa*wa) / matrix_a[0][j];
		matrix_a[2][j] = (1.0 - wa*2.0*zt + wa*wa) / matrix_a[0][j];
	
		matrix_b[0][j] = 1.0/ matrix_a[0][j];
		matrix_b[1][j] = -2.0 * matrix_b[0][j];
		matrix_b[2][j] = matrix_b[0][j];
	
		n = n + 2;
	}

	if((order % 2)==1)
	{
		j = (int)(order / 2) + 1;
		
		matrix_a[0][j] = 1.0 + wa;
		matrix_a[1][j] = (wa - 1.0) / matrix_a[0][j];
		matrix_b[0][j] = 1.0/ matrix_a[0][j];
		matrix_b[1][j] = -matrix_b[0][j];
	}

}

//------------------------------------------------
//　BPF独自のパラメータを決定
//------------------------------------------------
void DGFilter::Coef_b()
{
	int n, j;
	double w1,w2,zt,para_p,para_a;

	w1 = tan(PI * fc / fs);
	w2 = tan(PI * h_fc / fs);
	
	para_p = w2 - w1;
	para_a = w1 * w2;

	if((order % 2)==0)
		n = 1;
	else			
		n = 2;

	for(j = 1; j <= (int)(order / 2); j++)
	{
		zt = cos(n * PI / (2 * order));

		matrix_a[0][j] = 1.0 + para_p * 2.0 * zt + (2.0 * para_a + para_p * para_p) + para_a * para_p * 2.0 * zt + para_a * para_a;
		matrix_a[1][j] = 2.0 * (-2.0 - para_p * 2.0 * zt + para_a * para_p * 2.0 * zt + 2.0 * para_a * para_a) / matrix_a[0][j];
		matrix_a[2][j] = 2.0 * (3.0 - (2.0 * para_a + para_p * para_p) + 3.0 * para_a * para_a) / matrix_a[0][j];
		matrix_a[3][j] = 2.0 * (-2.0 + para_p * 2.0 * zt - para_a * para_p * 2.0 * zt + 2.0 * para_a * para_a) / matrix_a[0][j];
		matrix_a[4][j] = (1.0 - para_p * 2.0 * zt + (2.0 * para_a + para_p * para_p) - para_a * para_p * 2.0 * zt + para_a * para_a) / matrix_a[0][j];
	
		matrix_b[0][j]= para_p * para_p / matrix_a[0][j];
		matrix_b[1][j]= 0.0;
		matrix_b[2][j]= -2.0 * matrix_b[0][j];
		matrix_b[3][j]= 0.0;
		matrix_b[4][j]= para_p * para_p / matrix_a[0][j];

		n = n + 2;
	}

	if((order % 2) == 1)
	{
		j = (int)(order / 2) + 1;
		matrix_a[0][j] = 1.0 + para_a + para_p;
		matrix_a[1][j] = 2.0 * (para_a - 1.0) / matrix_a[0][j];
		matrix_a[2][j] = (1.0 - para_p + para_a) / matrix_a[0][j];

		matrix_b[0][j] = para_p / matrix_a[0][j];
		matrix_b[1][j] = 0.0;
		matrix_b[2][j] = -matrix_b[0][j];
	}
}

//------------------------------------------------
// 入力部分のパラメータを算出
//------------------------------------------------
void DGFilter::matrix_x_l(void)
{
	int n,nn;
	int i,j, l;
	double x[DEF_DGFILTER_N];
	double y[DEF_DGFILTER_N];

	for(i=0;i<DEF_DGFILTER_N;i++){
		x[i] = 0.0;
		y[i] = 0.0;
	}

	/**** 入力の2次後れは現在の入力と同じ *****/
	for(i=1;i<=(int)(order / 2);i++)  
		matrix_b[2][i] = matrix_b[0][i];

	/**** 2次のシステムのデータを入力 ****/
	for(i=0;i<3;i++)
		x[i] = matrix_b[i][1];

	n = 2;
	for(j=2;j<=(int)(order / 2);j++){
		for(l=0;l<3;l++){
			for(i=0;i<=n;i++){
				nn = n + 2-l;
				y[nn-i] = matrix_b[l][j] * x[i] + y[nn-i];
			}
		}
		for(i=0;i<DEF_DGFILTER_N;i++)
			x[i] = y[i];
		for(i=0;i<DEF_DGFILTER_N;i++)
			y[i] = 0.0;
		n = n + 2;
	}

	if((order%2) == 1){
		j=(int)(order / 2) + 1;
		for(l=0;l<2;l++){
			for(i=0;i<=n;i++){
				nn = n + 1-l;
				y[nn-i] = matrix_b[l][j] * x[n-i] + y[nn-i];
			}
		}
		for(i=0;i<DEF_DGFILTER_N;i++)
			x[i] = y[i];	
		n = n + 1;
	}

	if(order==1){
		b[0] = matrix_b[0][1];
		b[1] = matrix_b[0][1];
	}
	else{
		for(i=0;i<=n;i++){
			if((n-i)==0){
				b[n-i]=x[i];
				break;
			}
			b[n-i]=x[i];
		}
	}
}

//------------------------------------------------
// 出力部分のパラメータを算出
//------------------------------------------------
void DGFilter::matrix_y_l(void)
{
	int n,nn;
	int i,l,j;
	double x[DEF_DGFILTER_N];
	double y[DEF_DGFILTER_N];

	for(i=0;i<DEF_DGFILTER_N;i++){
		x[i] = 0.0;
		y[i] = 0.0;
	}

	/**** 2次のシステムのデータを入力 ****/
	for(j=1;j<=(int)(order / 2);j++)
		matrix_a[0][j] = 1.0;
	if((order % 2)==1)
		matrix_a[0][(int)(order / 2)+1] = 1.0;
	for(i=0;i<3;i++)
		y[i] = matrix_a[2-i][1];

	n = 2;
	for(j=2;j<=(int)(order / 2);j++){
		for(l=0;l<3;l++){
			for(i=0;i<=n;i++){
				nn = n + 2-l;
				x[nn-i] = matrix_a[l][j] * y[n-i] + x[nn-i];
			}
		}
		for(i=0;i<DEF_DGFILTER_N;i++)
			y[i] = x[i];	
		for(i=0;i<DEF_DGFILTER_N;i++)
			x[i] = 0.0;
		n = n + 2;
	}

	if((order%2) == 1){
		j=(int)(order / 2) + 1;
		for(l=0;l<2;l++){
			for(i=0;i<=n;i++){
				nn = n + 1-l;
				x[nn-i] = matrix_a[l][j] * y[n-i] + x[nn-i];
			}
		}
		for(i=0;i<DEF_DGFILTER_N;i++)
			y[i] = x[i];	
		n = n + 1;
	}

	if(order==1){
		a[0]= -1*matrix_a[1][1];
	}
	else{
		for(i=0;i<n;i++){
			a[n-i-1]=-1*y[i];
		}
	}

}

void DGFilter::matrix_x_h()
{
	int n,nn;
	int i, j, l;
	double x[DEF_DGFILTER_N];
	double y[DEF_DGFILTER_N];

	for(i = 0; i < DEF_DGFILTER_N; i++){
		x[i] = 0.0;
		y[i] = 0.0;
	}

	/**** 入力の2次後れは現在の入力と同じ *****/
	for(i = 1; i <= (int)(order / 2); i++)  
		matrix_b[2][i] = matrix_b[0][i];

	/**** 2次のシステムのデータを入力 ****/
	for(i = 0; i < 3; i++)
		x[i] = matrix_b[i][1];

	n = 2;

	for(j = 2; j <= (int)(order / 2); j++){
		for(l = 0; l < 3; l++){
			for(i = 0; i <= n; i++){
				nn = n + 2 - l;
				y[nn-i] = matrix_b[l][j] * x[i] + y[nn-i];
			}
		}
	
		for(i=0;i<DEF_DGFILTER_N;i++)
			x[i] = y[i];	
	
		for(i=0;i<DEF_DGFILTER_N;i++)
			y[i] = 0.0;
	
		n = n + 2;
	}

	if((order % 2) == 1){
		j = (int)(order / 2) + 1;
	
		for(l = 0; l < 2; l++){
			for(i = 0;i <= n; i++){
				nn = n + 1-l;
				y[nn-i] = matrix_b[l][j] * x[n-i] + y[nn-i];
			}
		}

		for(i = 0; i < DEF_DGFILTER_N; i++)
			x[i] = y[i];	
	
		n = n + 1;
	}

	if(order==1){
		b[0] = matrix_b[0][1];
		b[1] = matrix_b[0][1];
	}
	else{
		for(i=0;i<=n;i++){
			if((n-i)==0){
				b[n-i]=x[i];
				break;
			}
			b[n-i]=x[i];
		}
	}

}

void DGFilter::matrix_y_h()
{
	int n, nn;
	int i, j, l;
	double x[DEF_DGFILTER_N];
	double y[DEF_DGFILTER_N];

	for(i = 0; i < DEF_DGFILTER_N; i++){
		x[i] = 0.0;
		y[i] = 0.0;
	}

	/**** 2次のシステムのデータを入力 ****/
	for(j = 1; j <= (int)(order / 2); j++)
		matrix_a[0][j] = 1.0;

	if((order % 2) == 1)
		matrix_a[0][(int)(order / 2) + 1] = 1.0;
	
	for(i = 0; i < 5; i++)
		y[i] = matrix_a[i][1];

	n = 2 * 2;

	for(j = 2; j <= (int)(order / 2); j++){
		for(l = 0; l < 5; l++){
			for(i = 0; i <= n; i++){
				nn = n + 2 * 2-l;
				x[nn - i] = matrix_a[4 - l][j] * y[n - i] + x[nn - i];
			}
		}
	
		for(i = 0; i < DEF_DGFILTER_N; i++)
			y[i] = x[i];
	
		for(i = 0; i < DEF_DGFILTER_N; i++)
			x[i] = 0.0;
	
		n = n + 2 * 2;
	}

	if((order % 2) == 1){
		j = (int)(order / 2) + 1;
	
		for(l = 0; l < 3; l++){
			for(i = 0; i <= n; i++){
				nn = n + 1 * 2-l;
				x[nn - i] = matrix_a[2 - l][j] * y[n - i] + x[nn - i];
			}
		}
	
		for(i = 0; i < DEF_DGFILTER_N; i++)
			y[i] = x[i];	
		
		n = n + 1*2;
	}

	if(order==1){
		a[1] = -1 * matrix_a[1][1];
		a[0] = -1 * matrix_a[0][1];
	}else{
		for(i=0;i<n;i++)
			a[n-i-1] = -1 * y[n-i];
	}
}

void DGFilter::matrix_x_b()
{
	int n,nn;
	int i, j, l;
	double x[DEF_DGFILTER_N];
	double y[DEF_DGFILTER_N];

	for(i = 0; i < DEF_DGFILTER_N; i++){
		x[i] = 0.0;
		y[i] = 0.0;
	}

	/**** 2次のシステムのデータを入力 ****/
	for(i = 0; i < 5; i++)
		x[i] = matrix_b[i][1];

	n = 2 * 2;

	for(j = 2; j <= (int)(order / 2); j++){
		for(l = 0; l < 5; l++){
			for(i = 0; i <= n; i++){
				nn = n + 2 * 2-l;
				y[nn-i] = matrix_b[l][j] * x[i] + y[nn-i];
			}
		}
	
		for(i = 0; i < DEF_DGFILTER_N; i++)
			x[i] = y[i];	
	
		for(i = 0; i < DEF_DGFILTER_N; i++)
			y[i] = 0.0;
	
		n = n + 2 * 2;
	}

	if((order % 2) == 1){
		j = (int)(order / 2) + 1;
	
		for(l = 0; l < 3; l++){
			for(i = 0; i <= n; i++){
				nn = n + 1 * 2-l;
				y[nn-i] = matrix_b[l][j] * x[n-i] + y[nn-i];
			}
		}
	
		for(i = 0;i < DEF_DGFILTER_N; i++)
			x[i] = y[i];	
			
		n = n + 1 * 2;
	}

	if(order == 1){
		b[0] = matrix_b[0][1];
		b[1] = matrix_b[1][1];
		b[2] = matrix_b[2][1];
	}else{
		for(i = 0; i <= n; i++){
			if((n - i) == 0){
				b[n - i] = x[i];

				break;
			}
			b[n - i] = x[i];
		}
	}
}

void DGFilter::matrix_y_b()
{
	int n, nn;
	int i, j, l;
	double x[DEF_DGFILTER_N];
	double y[DEF_DGFILTER_N];

	for(i = 0; i < DEF_DGFILTER_N; i++){
		x[i] = 0.0;
		y[i] = 0.0;
	}

	/**** 2次のシステムのデータを入力 ****/
	for(j = 1; j <= (int)(order / 2); j++)
		matrix_a[0][j] = 1.0;

	if((order % 2) == 1)
		matrix_a[0][(int)(order / 2) + 1] = 1.0;
	
	for(i = 0; i < 5; i++)
		y[i] = matrix_a[i][1];

	n = 2 * 2;

	for(j = 2; j <= (int)(order / 2); j++){
		for(l = 0; l < 5; l++){
			for(i = 0; i <= n; i++){
				nn = n + 2 * 2-l;
				x[nn - i] = matrix_a[4 - l][j] * y[n - i] + x[nn - i];
			}
		}
	
		for(i = 0; i < DEF_DGFILTER_N; i++)
			y[i] = x[i];
	
		for(i = 0; i < DEF_DGFILTER_N; i++)
			x[i] = 0.0;
	
		n = n + 2*2;
	}

	if((order % 2) == 1){
		j = (int)(order / 2) + 1;
	
		for(l = 0; l < 3; l++){
			for(i = 0; i <= n; i++){
				nn = n + 1 * 2-l;
				x[nn - i] = matrix_a[2 - l][j] * y[n - i] + x[nn - i];
			}
		}
	
		for(i = 0; i < DEF_DGFILTER_N; i++)
			y[i] = x[i];	
		
		n = n + 1*2;
	}

	if(order==1){
		a[1] = -1 * matrix_a[1][1];
		a[0] = -1 * matrix_a[0][1];
	}else{
		for(i=0;i<n;i++)
			a[n-i-1] = -1 * y[n-i];
	}
}
//------------------------------------------------
// ハイパボリックアークサイン
//------------------------------------------------
double DGFilter::ArcSinh(double x)
{
	double ArcSinh;

	ArcSinh = log(x + sqrt(x * x +1));
	return ArcSinh;
}