
// VTestView.h : CVTestView �N���X�̃C���^�[�t�F�C�X
//


/****************�C���N���[�h�t�@�C��****************/
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


/*********************�萔�̒�`*********************/	
#define PORT 9876				//2���PC�Ԃ̒ʐM�����ʂ���PORT�ԍ����w��
#define NumberOfFilter 2		//�t�B���^�̎���
#define Chnumber 126			//�]�g�v��Ch��
#define data_num 80
#define M 10			        //AR���f���̎���	
#define send_SF 3				//�]�g�v�����M�����f�[�^��
#define smpf  100
#define smpf_dis 33
#define sp 0.2
#define line_sp 41				//8*(1/sp)+1
#define column_sp 66			//13*(1/sp)+1
#define file_time 15
/****************************************************/



class CVTestView : public CView
{
protected: // �V���A��������̂ݍ쐬���܂��B
	CVTestView();
	DECLARE_DYNCREATE(CVTestView)

// �I�[�o�[���C�h
public:
	virtual void OnDraw(CDC* pDC);  // ���̃r���[��`�悷�邽�߂ɃI�[�o�[���C�h����܂��B
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:

// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
protected:
	DECLARE_MESSAGE_MAP()
public:
	/*********************�֐��̒�`*********************/
	void ready_data(CDC* pDC);
	void ready_plot(CDC* pDC);
	void ready_file(CDC* pDC);
	void connection(void);				//�\�P�b�g�ʐM�ڑ��v���O����
	void Recv(void);
	void data_stk(int i);
	void bandpass(void);
	void format_ar(void);				//AR���f�����߂̏������v���O����
	void autocorrelation(void);			//���ȑ��֊֐����v�Z
	void set_determinant(void);			//AR���f���̏d�݂��v�Z���邽�߂̍s���SET����
	void inverse_m1(void);				//m1�̋t�s����v�Z
	void ar_a(void);					//AR���f���̏d�݃p�����[�^���v�Z
	void ar_sigma(void);				//AR���f���̃�^2�p�����[�^���v�Z
	void psd_ar(void);					//AR���f���̊e�p�����[�^����PSD���v�Z
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


	/*********************�ϐ��̒�`*********************/
	/*�\�P�b�g�ʐM*/
	int srcSocket;
	int dstSocket;
	struct sockaddr_in srcAddr;
	struct sockaddr_in dstAddr;
	int dstAddrSize;
	int numrcv;
	int status;

	/*�\�P�b�g�ʐM�̃f�[�^�֘A*/
	double buffer[Chnumber];						//��M�f�[�^
	double sig_data[Chnumber];
	double ydata[data_num][Chnumber];					//AR���f���Ɏg�p����f�[�^

	/*�J�E���g����ϐ�*/
	double *p;
	int i,i1,j,num,x,y,k,s,n,m;
	double y1, qi, si, xx;
	double psd_M;
	double F,dF,F_t;
	int deg_spd;
	int count_sp;
	int count_ar;									//AR���f�����n�߂邽�߂̃J�E���g�ϐ�
	int count_check;
	int count_file;
	int count_anl;

	/*AR���f���̃p�����[�^�𓱏o���邽�߂̕ϐ�*/
	DGFilter *Filter_b[Chnumber];					//�t�B���^�v���O�����̒�`
	DGFilter *Filter_c;
	DGFilter *Filter_l;
	DGFilter *Filter_dx;
	DGFilter *Filter_dy;
	DGFilter *Filter_spd;
	double C[data_num][Chnumber];						//AR���f���ň������ȑ��֊֐�
	double m1[M][M][Chnumber];				//AR���f���̏d�݃p�����[�^���Z�o����M�~M�s��
	double m1_inv[M][M][Chnumber];			//�s��m1�̋t�s��
	double m2[M][Chnumber];						//AR���f���̏d�݃p�����[�^���Z�o����M�~1�s��
	double w[M][Chnumber];							//AR���f���̏d�݃p�����[�^
	double sigma_1[Chnumber];
	double sigma[Chnumber];							//AR���f���̃�^2�p�����[�^
	double psd_1[Chnumber];
	double psd_2[Chnumber];
	double psd_data;
	double psd[Chnumber];							//AR���f���ɂ��Z�o�����p���[�X�y�N�g�����x
	double max_psd[Chnumber];						//�e�d�ɂɂ�����ő�p���[�X�y�N�g�����x
	double max_F[Chnumber];							//�e�d�ɂɂ�����ő���g��
	double tes_P;
	clock_t start_fp,end_fp;
	clock_t start_rcv,end_rcv;
	clock_t start_anl,end_anl;
	double duration_spd;
	double duration_rcv;							//�f�[�^���o�̊Ԋu����
	double duration_anl;							//�f�[�^��͂̊Ԋu����
	double duration_file;

	/*�X�v���C����Ԃ��s�����߂̕ϐ�*/
	double *h_column, *b_column, *c_column, *g_column, *u_column, *r_column, *x_column, *y_column;
	double *h_line, *b_line, *c_line, *g_line, *u_line, *r_line, *x_line, *y_line;
	double x1, y_out_column, y_out_line, y_column_sp[9][column_sp],y_line_sp[line_sp][column_sp];
	int sct_column_num,sct_line_num,column,line;

	/*�v���b�g���邽�߂̕ϐ�*/
	double max_max_psd;								//�S�d�ɂ̒��ɂ�����ő�p���[�X�y�N�g�����x
	double max_psd_line,max_psd_line0;				//�S�d�ɂ̒��ɂ�����ő�PSD���������s
	double max_psd_column,max_psd_column0;			//�S�d�ɂ̒��ɂ�����ő�PSD����������										
	int d;											//�d�ɂ���肷��ϐ�
	CString str;									//�d�ɔԍ��̕\���ϐ�
	int ox, oy;										//��ʂ̒��S���W
	double S_ox,S_oy;
	CRect screen;									//��ʂ̑傫�����擾����ϐ�
	int px, py,px0, py0;							//�ꎞ���O�̈ʒu�ƌ��݂̈ʒu�������ϐ�
	double drct_px,drct_py,drct_px0,drct_py0;
	double drct_x,drct_y,drct_a,drct_b,drct_m;
	int drct_x0,drct_y0;
	int clr[line_sp][column_sp];
	double px_psd,py_psd;
	double distance;								//�����i�񂾋����������ϐ�
	double Speed;									//���̑��x�������ϐ�
	int Speed0,int_Speed;
	CStdioFile ofp1,ofp2,ofp3;	
	double S_ratio;									//���ۂ̑��x�ɕϊ����邽�߂̕ϐ�
	double freq,freq0;
	int int_freq;
	int pastMs,pastMs0,x_time,end_time;
	int pastMsf,pastMsf0,x_timef,end_timef;
	/****************************************************/
};

//#ifndef _DEBUG  // VTestView.cpp �̃f�o�b�O �o�[�W����
//inline CVTestDoc* CVTestView::GetDocument() const
//   { return reinterpret_cast<CVTestDoc*>(m_pDocument); }
//#endif

