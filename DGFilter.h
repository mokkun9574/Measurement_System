/****************************************************************************** 
	ディジタルフィルタクラス
	
	File name：DGFilter.h
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
#ifndef __INC_DGFILTER_H
#define __INC_DGFILTER_H

#ifndef PI
#define PI	3.14159265358979323846264338327950288419716939937510
#endif

#define FILTER_VER 4.0	//バージョン管理用
#define DEF_DGFILTER_MAX_ORDER 6		//次数の最大値（一応６次まで対応する予定）
#define DEF_DGFILTER_N 20			//配列次数の数

class DGFilter{
public:
	DGFilter(void);						//コンストラクタ
	virtual double filter(double now_value);		//現在の値→フィルタ後の値
	virtual bool SetButterLPF(int oreder_num,double cut_freq,double f_sampling);//次数，遮断周波数（Hz），標本化周波数（Hz）
	virtual bool SetChebyshavLPF(int oreder_num,double cut_freq,double f_sampling,double rpl_rate);//次数，遮断周波数（Hz），標本化周波数（Hz），rpl:リップル率（現在未使用）
	virtual bool SetButterBPF(int order_num, double low_cut_freq, double high_cut_freq, double f_sampling);//次数、遮断周波数-立ち上がり（Hz），遮断周波数-立ち下がり（Hz），標本化周波数（Hz）
	virtual bool SetButterHPF(int oreder_num,double cut_freq,double f_sampling,double rpl_rate);//次数，遮断周波数（Hz），標本化周波数（Hz），rpl:リップル率（現在未使用）
private:
	double a[DEF_DGFILTER_MAX_ORDER],b[DEF_DGFILTER_MAX_ORDER+1];		//フィルタのパラメータ，aは入力ゲイン，bは出力ゲイン
	double matrix_a[5][DEF_DGFILTER_MAX_ORDER], matrix_b[5][DEF_DGFILTER_MAX_ORDER];
	double input[DEF_DGFILTER_MAX_ORDER+1];		//入力値
	double output[DEF_DGFILTER_MAX_ORDER+1];		//出力値
	int order;						//次数
	int m;							// バターワース(m=1)かチェビシェフ(m=2)を選択
	double fc, h_fc, fs, rpl;
	double ArcSinh(double x);
	virtual void matrix_y_l(void);
	virtual void matrix_x_l(void);
	virtual void matrix_y_b(void);
	virtual void matrix_x_b(void);
	virtual void matrix_y_h(void);
	virtual void matrix_x_h(void);
	virtual void Coef_l(void);
	virtual void Coef_b(void);
	virtual void Coef_h(void);
};

#endif
