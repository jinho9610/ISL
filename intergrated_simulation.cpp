#define MAX_STRETCH_PERCENTAGE 20
#include "Matrix.h"

#include <opencv2/opencv.hpp>
#include<iostream>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>
using namespace std;
using namespace cv;

//======================���� ����==========================

int B = 0; //Block Size
int BrightWidth = 0;
int BlackWidth = 0;

Mat src = imread("LGD.png"); // ����



string video_name;

// Pirmary : RsGsBs -> XYZ // inv(Primary) : XYZ -> RsGsBs // EOTF : RGB -> RsGsBs
double Primary[9];
//= { 262.775,123.2375,98.4775,120.6349,344.1349,31.7649,0.1934,21.3641,514.1541 };
double InvPrimary[9];
//= { 0.00452944, -0.00157421,-0.00077028,-0.00159373,0.00347093,0.00009081,0.00006452,-0.00014363,0.00194146 };
Matrix PC_Mx, IPC_Mx;
pair<int, double> Rs[128] = {}; // EOTF_Value TXT���Ͽ��� ������ Rs ���� ������ �迭
pair<int, double> Gs[128] = {}; // EOTF_Value TXT���Ͽ��� ������ Gs ���� ������ �迭
pair<int, double> Bs[128] = {}; // EOTF_Value TXT���Ͽ��� ������ Bs ���� ������ �迭
//======================�Լ� ����==========================
void decideBlock();

void makingBlock(int, Mat, Mat&); // ���ȭ �Լ� 
void stretchableForming(int, Mat, Mat&); // ��Ʈ��ó�� ����ȭ �Լ� (�� ���� �߰�)

void stretch_cols_general(int, double, Mat, Mat&); // ���� ����
void stretch_rows_general(int, double, Mat, Mat&); // ���� ����
void stretch_diag_general(int, double, Mat, Mat&); // �밢�� ����

void makeVideo_cols(int, int, int, Mat&); // ���� ���� - ����
void makeVideo_rows(int, int, int, Mat&); // ���� ���� - ����
void makeVideo_diag(int, int, int, Mat&); // ���� ���� - �밢��

void makeVideo_cols_bothways(int, int, int, Mat&);
void makeVideo_rows_bothways(int, int, int, Mat&);
void makeVideo_diag_bothways(int, int, int, Mat&);

void showFrames_cols(int, Mat&); // �� �����Ӿ� ���� - ����
void showFrames_rows(int, Mat&); // �� �����Ӿ� ���� - ����
void showFrames_diag(int, Mat&); // �� �����Ӿ� ���� - �밢��

int readvideo(); // �ùķ��̼� �ݺ� ȸ�� �� �ٽ� ��� ���� ����
void playVideo(VideoCapture videoCapture, int, string); // ����� ������ ���

//======================Į�� ���� �Լ�==========================
void setMatrix();
bool cmp_XYZ(pair<int, double>, pair<int, double>); // ���� �ȼ����� XYZ�� ��� ���� �������� ������ �� ���� compare
bool cmp_RGBs(pair<int, double>, pair<int, double>); // Rs, Bs, Gs �迭�� �����ϱ� ���� compare
void getMatrixfromTXT();
void getRsGsBsfromTXT(pair<int, double>*, pair<int, double>*, pair<int, double>*);
pair<double, pair<double, double> > calXYZofAdj_Pixel(int, int, int, pair<int, double>*, pair<int, double>*, pair<int, double>*);
// RsGsBs(EOTF Value ǥ) �ؽ�Ʈ���Ͽ��� Rs, Gs, Bs��  Rs, Gs, Bs �迭�� �������� �Լ�
pair<int, pair<int, int> > calNewRGB(pair<int, double>*, pair<int, double>*, pair<int, double>*, double, double, double); // '��ȣ��' �ȼ��� �־��� RGB ��
Matrix calRsGsBs(pair<double, pair<double, double> >, double*, double);
Matrix calRsGsBs2(pair<double, pair<double, double> >, pair<double, pair<double, double> >, double*, double, double);
void printRsGsBsArr(pair<int, double>*, pair<int, double>*, pair<int, double>*);

int main() {
    int mode, corr, sorr, oneorboth, howlong; //�ùķ��̼� ���� ���� ����
    int frames, fps;
    getMatrixfromTXT();
    setMatrix();
    PC_Mx.show();
    cout << "\n";
    IPC_Mx.show();
    getRsGsBsfromTXT(Rs, Gs, Bs); // TXT ���� �Ľ�
    //printRsGsBsArr(Rs, Gs, Bs);
    decideBlock();

    Mat blockedimg(src.rows * B, src.cols * B, CV_8UC3, Vec3b(0, 0, 0)); //���ȭ�� �̹��� ����
    Mat img(blockedimg.rows, blockedimg.cols, CV_8UC3, Scalar(255, 255, 255)); // ��Ʈ��ó�� ���� �ϼ� �̹��� ����

    makingBlock(B, src, blockedimg);
    //makingBlock(B, src, img);

    stretchableForming(B, blockedimg, img);

    //imshow("StretchFormedIMG", img);
    //waitKey(0);

    cout << "��带 �������ּ���" << endl;
    cout << "1. 20������ 1�ʾ� �������� ����      2. ������ �����ϱ�" << endl;
    cout << "MODE : ";
    cin >> mode;
    if (mode == 1)
    {
        cout << "1. cols�� ���� 2. rows�� ���� 3. �밢�� ����" << endl;
        cin >> corr;
        if (corr == 1) {
            showFrames_cols(B, img);
        }

        else if (corr == 2) {
            showFrames_rows(B, img);
        }
        else if (corr == 3) {
            showFrames_diag(B, img);
        }

    }
    else if (mode == 2)
    {
        int howlong = 0;

        cout << "���� ���̸� �Է����ּ��� (���� : ��)" << endl;
        cout << "���� ���� : ";
        cin >> howlong;

        int fps = 60;
        int frames = howlong * fps;

        cout << "1. cols�� ���� 2. rows�� ���� 3. �밢�� ����" << endl;
        cin >> corr;

        int oneorboth = 0;
        cout << "1. �ܹ��� ���� 2. ����� ����" << endl;
        cin >> oneorboth;

        if (corr == 1) {
            if (oneorboth == 1)
                makeVideo_cols(B, frames, fps, img);
            else if (oneorboth == 2)
                makeVideo_cols_bothways(B, frames, fps, img);
        }

        else if (corr == 2) {
            if (oneorboth == 1)
                makeVideo_rows(B, frames, fps, img);
            else if (oneorboth == 2)
                makeVideo_rows_bothways(B, frames, fps, img);
        }

        else if (corr == 3) {
            if (oneorboth == 1)
                makeVideo_diag(B, frames, fps, img);
            else if (oneorboth == 2)
                makeVideo_diag_bothways(B, frames, fps, img);
        }
        readvideo();
    }

    waitKey();
    return 0;
}

void decideBlock() {
    cout << "��ü ��ϻ���� �������ּ��� (�߱���+��)" << endl;
    cin >> B;
    cout << "�߱����� �� ������ ũ�⸦ �Է����ּ���" << endl;
    cout << "�߱��� : ";
    cin >> BrightWidth;
    BlackWidth = B - BrightWidth;
    cout << "�� : " << BlackWidth << endl;
}

void stretchableForming(int B, Mat src1, Mat& dst) {
    int halfB = B / 2;
    for (int y = 0; y < dst.rows; y++)
    {
        if ((y % B) >= BrightWidth) {
            for (int x = 0; x < dst.cols; x++) {
                dst.at<Vec3b>(y, x) = (0);
            }

        }
        else {
            for (int x = 0; x < dst.cols; x++)
            {
                if ((x % B) >= BrightWidth) {
                    dst.at<Vec3b>(y, x) = (0);
                }
                else
                {
                    dst.at<Vec3b>(y, x) = src1.at<Vec3b>(y, x);
                }
            }
        }
    }
    imwrite("StretchableFormed.bmp", dst);
    /*imshow("ddd", dst);
    waitKey(0);*/
}

void makingBlock(int BlockSize, Mat src1, Mat& dst) {
    Mat tmp(src1.rows * BlockSize, src1.cols * BlockSize, CV_8UC3, Vec3b(0, 0, 0));
    for (int y = 0; y < tmp.rows; y++)
    {
        for (int x = 0; x < tmp.cols; x++)
        {
            tmp.at<Vec3b>(y, x) = src1.at<Vec3b>(y / BlockSize, x / BlockSize);
        }
    }
    dst = tmp;
}

void stretch_diag_general(int B, double S, Mat src, Mat& dst) {
    stretch_cols_general(B, S, src, dst);
    stretch_rows_general(B, S, dst, dst);
}

void stretch_cols_general(int B, double S, Mat src, Mat& dst) {
    double P = B * S; //���� �� ������
    double a = 0;
    double m = 0;
    int index = -1;
    int i_max = 0;
    double stretchper = 1 + S;

    Mat imgg(src.rows, src.cols * stretchper, CV_8UC3, Scalar(255, 255, 255));
    vector<Vec3b> v;
    int count = 0;

    for (int y = 0; y < src.rows; y++) {

        a = 0;
        m = 0;
        index = -1;
        i_max = 0;
        //cout << "y : " << y << endl;
        if (y % B == 0)
        {
            v.clear();
            for (int x = 0; x < src.cols; x++) {
                v.push_back(src.at<Vec3b>(y, x));
            }

            while (i_max < imgg.cols) {
                pair<double, pair<double, double> > adjacentXYZ1;
                pair<double, pair<double, double> > adjacentXYZ2;
                
                double XYZ[3];
                if (0 <= (a - (int)a) * (a - (int)a) && (a - (int)a) * (a - (int)a) < 0.00000000001) {
                    //cout << "a����" << "index : "<<index<< endl;
                    m = 0;

                    index = index + B;
                    i_max = i_max + B;

                    if (index >= imgg.cols - 1) {
                        if (imgg.cols > v.size()) {
                            int plus = imgg.cols - v.size();
                            Vec3b plusval = v[v.size() - 1];
                            v.insert(v.begin() + v.size() - 1, plus, plusval);
                        }

                        i_max = imgg.cols;
                        break;
                    }

                    if (((P - (int)P) * (P - (int)P) < 0.00000000001)) {
                        //v.insert(v.begin() + index, (int)P, Vec3b(0, 0, 0));
                        Vec3b equalvalue = v.at(index);
                        v.insert(v.begin() + index, (int)P, equalvalue);
                        index = index + (int)P;
                        i_max = i_max + (int)P;
                    }
                    else {
                        //v.insert(v.begin() + index, ceil(P), Vec3b(0, 0, 0));
                        Vec3b equalvalue = v.at(index);
                        v.insert(v.begin() + index, ceil(P), equalvalue);
                        index = index + ceil(P);
                        i_max = i_max + ceil(P);
                    }

                    if (index >= imgg.cols - 1) {
                        if (imgg.cols > v.size()) {
                            int plus = imgg.cols - v.size();
                            Vec3b plusval = v[v.size() - 1];
                            v.insert(v.begin() + v.size() - 1, plus, plusval);
                        }
                        i_max = imgg.cols;
                        break;
                    }

                    if (P > 1) {
                        double percon = 1 - (P - (int)P);
                        adjacentXYZ1 = calXYZofAdj_Pixel(v[index + 1][2], v[index + 1][1], v[index + 1][0], Rs, Gs, Bs); // ���� �ȼ��� XYZ �� ���
                        adjacentXYZ2 = calXYZofAdj_Pixel(v[index - 1][2], v[index - 1][1], v[index - 1][0], Rs, Gs, Bs); // ���� �ȼ��� XYZ �� ���
                        double percentage1 = 1 - (P - (int)P);
                        double percentage2 = 1 - percentage1;

                        if ((percon - 1) * (percon - 1) > 0.000000001 || (percon - 1) * (percon - 1) < -0.0000000001) {
                            //Matrix RsGsBs_Mx = calRsGsBs(adjacentXYZ, XYZ, 1 - (P - (int)P));
                            Matrix RsGsBs_Mx = calRsGsBs2(adjacentXYZ1, adjacentXYZ2, XYZ, percentage1, percentage2);
                            pair<int, pair<int, int> > newRGB = calNewRGB(Rs, Gs, Bs, RsGsBs_Mx.GetValue(1, 1), RsGsBs_Mx.GetValue(2, 1), RsGsBs_Mx.GetValue(3, 1));
                            v[index][2] = newRGB.first;
                            v[index][1] = newRGB.second.first;
                            v[index][0] = newRGB.second.second;
                            
                            /*cout << (int)(v[index][2]) << "   " << (int)v[index][1] << "   " << (int)v[index][0] << endl;
                            if (v[index][2] == 0 && v[index][1] == 0 && v[index][0] == 0) cout << "������!!!!!!!!!" << endl;*/
                        }
                        m = P - (int)P;
                    }
                    else if (P < 1) {
                        adjacentXYZ1 = calXYZofAdj_Pixel(v[index + 1][2], v[index + 1][1], v[index + 1][0], Rs, Gs, Bs); // ���� �ȼ��� XYZ �� ���
                        adjacentXYZ2 = calXYZofAdj_Pixel(v[index - 1][2], v[index - 1][1], v[index - 1][0], Rs, Gs, Bs); // ���� �ȼ��� XYZ �� ���
                        double percentage1 = 1 - P;
                        double percentage2 = 1 - percentage1;
                        Matrix RsGsBs_Mx = calRsGsBs2(adjacentXYZ1, adjacentXYZ2, XYZ, percentage1, percentage2);
                        pair<int, pair<int, int> > newRGB = calNewRGB(Rs, Gs, Bs, RsGsBs_Mx.GetValue(1, 1), RsGsBs_Mx.GetValue(2, 1), RsGsBs_Mx.GetValue(3, 1));
                        v[index][2] = newRGB.first;
                        v[index][1] = newRGB.second.first;
                        v[index][0] = newRGB.second.second;
                        
                        /*cout << (int)(v[index][2]) << "   " << (int)v[index][1] << "   " << (int)v[index][0] << endl;
                        if (v[index][2] == 0 && v[index][1] == 0 && v[index][0] == 0) cout << "������!!!!!!!!!" << endl;*/
                        m = P;
                    }
                    a = m;
                }
                else {
                    //cout << "a�����ƴ�" << "index : " << index << endl;
                    index = index + BrightWidth;
                    i_max = i_max + BrightWidth;

                    if (index >= imgg.cols - 1) {
                        if (imgg.cols > v.size()) {
                            int plus = imgg.cols - v.size();
                            Vec3b plusval = v[v.size() - 1];
                            v.insert(v.begin() + v.size() - 1, plus, plusval);
                        }
                        i_max = imgg.cols;
                        break;
                    }

                    adjacentXYZ1 = calXYZofAdj_Pixel(v[index][2], v[index][1], v[index][0], Rs, Gs, Bs);
                    adjacentXYZ2 = calXYZofAdj_Pixel(v[index + 1][2], v[index + 1][1], v[index + 1][0], Rs, Gs, Bs); // ���� �ȼ��� XYZ �� ���
                    double percentage1 = m;
                    double percentage2 = 1 - percentage1;
                    Matrix RsGsBs_Mx = calRsGsBs2(adjacentXYZ1, adjacentXYZ2, XYZ, percentage1, percentage2);
                    pair<int, pair<int, int> > newRGB = calNewRGB(Rs, Gs, Bs, RsGsBs_Mx.GetValue(1, 1), RsGsBs_Mx.GetValue(2, 1), RsGsBs_Mx.GetValue(3, 1));
                    v[index][2] = newRGB.first;
                    v[index][1] = newRGB.second.first;
                    v[index][0] = newRGB.second.second;
                    
                    /*cout << (int)(v[index][2]) << "   " << (int)v[index][1] << "   " << (int)v[index][0] << endl;
                    if (v[index][2] == 0 && v[index][1] == 0 && v[index][0] == 0) cout << "������!!!!!!!!!" << endl;*/

                    index = index + BlackWidth;
                    i_max = i_max + BlackWidth;

                    if (index >= imgg.cols - 1) {
                        if (imgg.cols > v.size()) {
                            int plus = imgg.cols - v.size();
                            Vec3b plusval = v[v.size() - 1];
                            v.insert(v.begin() + v.size() - 1, plus, plusval);
                        }
                        i_max = imgg.cols;
                        break;
                    }

                    a = P - (1 - m);

                    if (((a - (int)a) * (a - (int)a)) < 0.0000000001) {
                        if (a > 1)
                            a = (int)a;
                        else if (a < 1)
                            a = 0;

                        //v.insert(v.begin() + index, a, Vec3b(0, 0, 0));
                        Vec3b equalvalue = v.at(index);
                        v.insert(v.begin() + index, a, equalvalue);
                        index = index + a;
                        i_max = i_max + a;
                    }

                    else {
                        //v.insert(v.begin() + index, ceil(a), Vec3b(0, 0, 0));
                        Vec3b equalvalue = v.at(index);
                        v.insert(v.begin() + index, ceil(a), equalvalue);
                        index = index + ceil(a);
                        i_max = i_max + ceil(a);
                    }

                    if (index >= imgg.cols - 1) {
                        if (imgg.cols > v.size()) {
                            int plus = imgg.cols - v.size();
                            Vec3b plusval = v[v.size() - 1];
                            v.insert(v.begin() + v.size() - 1, plus, plusval);
                        }
                        i_max = imgg.cols;
                        break;
                    }

                    if ((a - (int)a) * (a - (int)a) < 0.000000001) continue;



                    else {
                        if (a > 1) {
                            adjacentXYZ1 = calXYZofAdj_Pixel(v[index + 1][2], v[index + 1][1], v[index + 1][0], Rs, Gs, Bs);
                            adjacentXYZ2 = calXYZofAdj_Pixel(v[index - 1][2], v[index - 1][1], v[index - 1][0], Rs, Gs, Bs); // ���� �ȼ��� XYZ �� ���
                            double percentage1 = 1 - (a - (int)a);
                            double percentage2 = 1 - percentage1;
                            Matrix RsGsBs_Mx = calRsGsBs2(adjacentXYZ1, adjacentXYZ2, XYZ, percentage1, percentage2);
                            pair<int, pair<int, int> > newRGB = calNewRGB(Rs, Gs, Bs, RsGsBs_Mx.GetValue(1, 1), RsGsBs_Mx.GetValue(2, 1), RsGsBs_Mx.GetValue(3, 1));
                            v[index][2] = newRGB.first;
                            v[index][1] = newRGB.second.first;
                            v[index][0] = newRGB.second.second;
                          
                            /*cout << (int)(v[index][2]) << "   " << (int)v[index][1] << "   " << (int)v[index][0] << endl;
                            if (v[index][2] == 0 && v[index][1] == 0 && v[index][0] == 0) cout << "������!!!!!!!!!" << endl;*/
                            m = a - (int)a;
                        }

                        else if (0 < a && a < 1) {

                            adjacentXYZ1 = calXYZofAdj_Pixel(v[index + 1][2], v[index + 1][1], v[index + 1][0], Rs, Gs, Bs);
                            adjacentXYZ2 = calXYZofAdj_Pixel(v[index - 1][2], v[index - 1][1], v[index - 1][0], Rs, Gs, Bs); // ���� �ȼ��� XYZ �� ���
                            double percentage1 = 1 - a;
                            double percentage2 = 1 - percentage1;
                            Matrix RsGsBs_Mx = calRsGsBs2(adjacentXYZ1, adjacentXYZ2, XYZ, percentage1, percentage2);
                            pair<int, pair<int, int> > newRGB = calNewRGB(Rs, Gs, Bs, RsGsBs_Mx.GetValue(1, 1), RsGsBs_Mx.GetValue(2, 1), RsGsBs_Mx.GetValue(3, 1));
                            v[index][2] = newRGB.first;
                            v[index][1] = newRGB.second.first;
                            v[index][0] = newRGB.second.second;
                           
                            /*cout << (int)(v[index][2]) << "   " << (int)v[index][1] << "   " << (int)v[index][0] << endl;
                            if (v[index][2] == 0 && v[index][1] == 0 && v[index][0] == 0) cout << "������!!!!!!!!!" << endl;*/
                            m = a;
                        }

                        else if (a < 0) {
                            adjacentXYZ1 = calXYZofAdj_Pixel(v[index + 1][2], v[index + 1][1], v[index + 1][0], Rs, Gs, Bs);
                            adjacentXYZ2 = calXYZofAdj_Pixel(v[index - 1][2], v[index - 1][1], v[index - 1][0], Rs, Gs, Bs); // ���� �ȼ��� XYZ �� ���
                            double percentage1 = -a;
                            double percentage2 = 1 - percentage1;
                            Matrix RsGsBs_Mx = calRsGsBs2(adjacentXYZ1, adjacentXYZ2, XYZ, percentage1, percentage2);
                            pair<int, pair<int, int> > newRGB = calNewRGB(Rs, Gs, Bs, RsGsBs_Mx.GetValue(1, 1), RsGsBs_Mx.GetValue(2, 1), RsGsBs_Mx.GetValue(3, 1));
                            v[index][2] = newRGB.first;
                            v[index][1] = newRGB.second.first;
                            v[index][0] = newRGB.second.second;
                           
                            /*cout << (int)(v[index][2]) << "   " << (int)v[index][1] << "   " << (int)v[index][0] << endl;
                            if (v[index][2] == 0 && v[index][1] == 0 && v[index][0] == 0) cout << "������!!!!!!!!!" << endl;*/
                            m = a + 1;
                        }
                    }
                }
            }
        }

        else if (y % B >= B - BlackWidth) {
            v.clear();

            for (int x = 0; x < imgg.cols; x++)
                v.push_back(Vec3b(0, 0, 0));
        }

        for (int x = 0; x < imgg.cols; x++) {
            imgg.at<Vec3b>(y, x) = v[x];
        }
    }
    dst = imgg;
}

void stretch_rows_general(int B, double S, Mat src, Mat& dst) {
    double P = B * S; //���� �� ������
    double a = 0;
    double m = 0;
    int index = -1;
    int i_max = 0;
    double stretchper = 1 + S;


    Mat imgg(src.rows * stretchper, src.cols, CV_8UC3, Scalar(255, 255, 255));

    vector<Vec3b> v;
    int count = 0;

    for (int x = 0; x < src.cols; x++) {
        a = 0;
        m = 0;
        index = -1;
        i_max = 0;

        if (x % B == 0)
        {
            v.clear();
            for (int y = 0; y < src.rows; y++) {
                v.push_back(src.at<Vec3b>(y, x));
            }

            while (i_max < imgg.rows) {
                pair<double, pair<double, double> > adjacentXYZ;
                double XYZ[3];
                if (0 <= (a - (int)a) * (a - (int)a) && (a - (int)a) * (a - (int)a) < 0.00000000001) {
                    m = 0;

                    index = index + B;
                    i_max = i_max + B;

                    if (index >= imgg.rows - 1) {
                        if (imgg.rows > v.size()) {
                            int plus = imgg.rows - v.size();
                            Vec3b plusval = v[v.size() - 1];
                            v.insert(v.begin() + v.size() - 1, plus, plusval);
                        }

                        i_max = imgg.rows;
                        break;
                    }

                    if (((P - (int)P) * (P - (int)P) < 0.00000000001)) {
                        v.insert(v.begin() + index, (int)P, Vec3b(0, 0, 0));
                        index = index + (int)P;
                        i_max = i_max + (int)P;
                    }
                    else {
                        v.insert(v.begin() + index, ceil(P), Vec3b(0, 0, 0));
                        index = index + ceil(P);
                        i_max = i_max + ceil(P);
                    }

                    if (index >= imgg.rows - 1) {
                        if (imgg.rows > v.size()) {
                            int plus = imgg.rows - v.size();
                            Vec3b plusval = v[v.size() - 1];
                            v.insert(v.begin() + v.size() - 1, plus, plusval);
                        }
                        i_max = imgg.rows;
                        break;
                    }

                    if (P > 1) {
                        double percon = 1 - (P - (int)P);
                        adjacentXYZ = calXYZofAdj_Pixel(v[index + 1][0], v[index + 1][1], v[index + 1][2], Rs, Gs, Bs); // ���� �ȼ��� XYZ �� ���
                        if ((percon - 1) * (percon - 1) > 0.000000001 || (percon - 1) * (percon - 1) < -0.0000000001) {
                            Matrix RsGsBs_Mx = calRsGsBs(adjacentXYZ, XYZ, 1 - (P - (int)P));
                            pair<int, pair<int, int> > newRGB = calNewRGB(Rs, Gs, Bs, RsGsBs_Mx.GetValue(1, 1), RsGsBs_Mx.GetValue(2, 1), RsGsBs_Mx.GetValue(3, 1));
                            v[index][0] = newRGB.first;
                            v[index][1] = newRGB.second.first;
                            v[index][2] = newRGB.second.second;
                        }
                        m = P - (int)P;
                    }
                    else if (P < 1) {
                        adjacentXYZ = calXYZofAdj_Pixel(v[index + 1][0], v[index + 1][1], v[index + 1][2], Rs, Gs, Bs); // ���� �ȼ��� XYZ �� ���
                        Matrix RsGsBs_Mx = calRsGsBs(adjacentXYZ, XYZ, 1 - P);
                        pair<int, pair<int, int> > newRGB = calNewRGB(Rs, Gs, Bs, RsGsBs_Mx.GetValue(1, 1), RsGsBs_Mx.GetValue(2, 1), RsGsBs_Mx.GetValue(3, 1));
                        v[index][0] = newRGB.first;
                        v[index][1] = newRGB.second.first;
                        v[index][2] = newRGB.second.second;
                        m = P;
                    }
                    a = m;
                }
                else {
                    index = index + BrightWidth;
                    i_max = i_max + BrightWidth;

                    if (index >= imgg.rows - 1) {
                        if (imgg.rows > v.size()) {
                            int plus = imgg.rows - v.size();
                            Vec3b plusval = v[v.size() - 1];
                            v.insert(v.begin() + v.size() - 1, plus, plusval);
                        }
                        i_max = imgg.rows;
                        break;
                    }

                    adjacentXYZ = calXYZofAdj_Pixel(v[index][0], v[index][1], v[index][2], Rs, Gs, Bs);
                    Matrix RsGsBs_Mx = calRsGsBs(adjacentXYZ, XYZ, m);
                    pair<int, pair<int, int> > newRGB = calNewRGB(Rs, Gs, Bs, RsGsBs_Mx.GetValue(1, 1), RsGsBs_Mx.GetValue(2, 1), RsGsBs_Mx.GetValue(3, 1));
                    v[index][0] = newRGB.first;
                    v[index][1] = newRGB.second.first;
                    v[index][2] = newRGB.second.second;

                    index = index + BlackWidth;
                    i_max = i_max + BlackWidth;

                    if (index >= imgg.rows - 1) {
                        if (imgg.rows > v.size()) {
                            int plus = imgg.rows - v.size();
                            Vec3b plusval = v[v.size() - 1];
                            v.insert(v.begin() + v.size() - 1, plus, plusval);
                        }
                        i_max = imgg.rows;
                        break;
                    }

                    a = P - (1 - m);

                    if (((a - (int)a) * (a - (int)a)) < 0.00000000001) {
                        if (a > 1)
                            a = (int)a;
                        else if (a < 1)
                            a = 0;

                        v.insert(v.begin() + index, a, Vec3b(0, 0, 0));
                        index = index + a;
                        i_max = i_max + a;
                    }

                    else {
                        v.insert(v.begin() + index, ceil(a), Vec3b(0, 0, 0));
                        index = index + ceil(a);
                        i_max = i_max + ceil(a);
                    }

                    if (index >= imgg.rows - 1) {
                        if (imgg.rows > v.size()) {
                            int plus = imgg.rows - v.size();
                            Vec3b plusval = v[v.size() - 1];
                            v.insert(v.begin() + v.size() - 1, plus, plusval);
                        }
                        i_max = imgg.rows;
                        break;
                    }
                    if ((a - (int)a) * (a - (int)a) < 0.00000000001) continue;


                    else {
                        if (a > 1) {
                            adjacentXYZ = calXYZofAdj_Pixel(v[index + 1][0], v[index + 1][1], v[index + 1][2], Rs, Gs, Bs);
                            Matrix RsGsBs_Mx = calRsGsBs(adjacentXYZ, XYZ, 1 - (a - (int)a));
                            pair<int, pair<int, int> > newRGB = calNewRGB(Rs, Gs, Bs, RsGsBs_Mx.GetValue(1, 1), RsGsBs_Mx.GetValue(2, 1), RsGsBs_Mx.GetValue(3, 1));
                            v[index][0] = newRGB.first;
                            v[index][1] = newRGB.second.first;
                            v[index][2] = newRGB.second.second;
                            m = a - (int)a;
                        }

                        else if (0 < a && a < 1) {

                            adjacentXYZ = calXYZofAdj_Pixel(v[index + 1][0], v[index + 1][1], v[index + 1][2], Rs, Gs, Bs);
                            Matrix RsGsBs_Mx = calRsGsBs(adjacentXYZ, XYZ, 1 - a);
                            pair<int, pair<int, int> > newRGB = calNewRGB(Rs, Gs, Bs, RsGsBs_Mx.GetValue(1, 1), RsGsBs_Mx.GetValue(2, 1), RsGsBs_Mx.GetValue(3, 1));
                            v[index][0] = newRGB.first;
                            v[index][1] = newRGB.second.first;
                            v[index][2] = newRGB.second.second;
                            m = a;
                        }

                        else if (a < 0) {
                            adjacentXYZ = calXYZofAdj_Pixel(v[index + 1][0], v[index + 1][1], v[index + 1][2], Rs, Gs, Bs);
                            Matrix RsGsBs_Mx = calRsGsBs(adjacentXYZ, XYZ, -a);
                            pair<int, pair<int, int> > newRGB = calNewRGB(Rs, Gs, Bs, RsGsBs_Mx.GetValue(1, 1), RsGsBs_Mx.GetValue(2, 1), RsGsBs_Mx.GetValue(3, 1));
                            v[index][0] = newRGB.first;
                            v[index][1] = newRGB.second.first;
                            v[index][2] = newRGB.second.second;
                            m = a + 1;
                        }
                    }
                }
            }
        }
        else if (x % B >= BlackWidth)
        {
            v.clear();
            for (int y = 0; y < imgg.rows; y++)
            {
                v.push_back(Vec3b(0, 0, 0));
            }
        }

        for (int y = 0; y < imgg.rows; y++) {
            imgg.at<Vec3b>(y, x) = v[y];
        }
    }
    dst = imgg;
}


void makeVideo_cols(int B, int frames, int fps, Mat& img)
{
    Size size = Size(img.cols * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200, img.rows);
    VideoWriter writer;
    video_name = "./result_cols.avi";
    writer.open(video_name, VideoWriter::fourcc('H', 'F', 'Y', 'U'), fps, size, true);
    Mat* frame;
    frame = new Mat[frames];

    for (int frameNum = 0; frameNum < frames; frameNum++)
    {

        if (frameNum == 0) {
            copyMakeBorder(img, frame[frameNum], 0, 0, 0, img.cols * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - img.cols, BORDER_CONSTANT, 0); // ������ ���θ� 800�ȼ��� ���־�����, ���ŵ� �̹����� ä���� ���� �κ��� ���������� ǥ��
            writer.write(frame[frameNum]);
        }
        else
        {
            Mat temp(img.rows, img.cols * (1 + (double)MAX_STRETCH_PERCENTAGE / (frames - 1) * frameNum * 0.01), CV_8UC3, Vec3b(0, 0, 0));
            stretch_cols_general(B, (double)MAX_STRETCH_PERCENTAGE / (frames - 1) * frameNum * 0.01, img, temp);
            cout << (double)MAX_STRETCH_PERCENTAGE / (frames - 1) * frameNum << "%" << endl;
            copyMakeBorder(temp, frame[frameNum], 0, 0, 0, img.cols * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - temp.cols, BORDER_CONSTANT, 0); // ������ ���θ� 800�ȼ��� ���־�����, ���ŵ� �̹����� ä���� ���� �κ��� ���������� ǥ��
            writer.write(frame[frameNum]);
        }
    }
    for (int frameNum = frames - 1; frameNum >= 0; frameNum--)
        writer.write(frame[frameNum]);
    cout << "done..." << endl;
    delete[] frame;
}



void makeVideo_rows(int B, int frames, int fps, Mat& img)
{
    Size size = Size(img.cols, img.rows * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200);
    VideoWriter writer;
    video_name = "./result_rows.avi";
    writer.open(video_name, VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, size, true);
    Mat* frame;
    frame = new Mat[frames];
    for (int frameNum = 0; frameNum < frames; frameNum++)
    {
        if (frameNum == 0) {
            copyMakeBorder(img, frame[frameNum], 0, img.rows * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - img.rows, 0, 0, BORDER_CONSTANT, 0);
            writer.write(frame[frameNum]);
        }
        else
        {
            Mat temp(img.rows * (1 + (double)MAX_STRETCH_PERCENTAGE / (frames - 1) * frameNum * 0.01), img.cols, CV_8UC3, Vec3b(0, 0, 0));
            stretch_rows_general(B, (double)MAX_STRETCH_PERCENTAGE / (frames - 1) * frameNum * 0.01, img, temp);
            cout << (double)MAX_STRETCH_PERCENTAGE / (frames - 1) * frameNum << "%" << endl;
            copyMakeBorder(temp, frame[frameNum], 0, img.rows * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - temp.rows, 0, 0, BORDER_CONSTANT, 0);
            writer.write(frame[frameNum]);
        }
    }
    for (int frameNum = frames - 1; frameNum >= 0; frameNum--)
        writer.write(frame[frameNum]);
    cout << "done..." << endl;

    delete[] frame;
}

void makeVideo_diag(int B, int frames, int fps, Mat& img)
{
    Size size = Size(img.cols * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200, img.rows * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200);
    VideoWriter writer;
    video_name = "./result_diag.avi";
    writer.open(video_name, VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, size, true);
    Mat* frame;
    frame = new Mat[frames];
    for (int frameNum = 0; frameNum < frames; frameNum++)
    {
        if (frameNum == 0) {
            copyMakeBorder(img, frame[frameNum], 0, img.rows * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - img.rows, 0, img.cols * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - img.cols, BORDER_CONSTANT, 0);
            writer.write(frame[frameNum]);
        }
        else
        {
            Mat temp(img.rows * (1 + (double)MAX_STRETCH_PERCENTAGE / (frames - 1) * frameNum * 0.01), img.cols * (1 + (double)MAX_STRETCH_PERCENTAGE / (frames - 1) * frameNum * 0.01), CV_8UC3, Vec3b(0, 0, 0));
            stretch_diag_general(B, (double)MAX_STRETCH_PERCENTAGE / (frames - 1) * frameNum * 0.01, img, temp);
            cout << (double)MAX_STRETCH_PERCENTAGE / (frames - 1) * frameNum << "%" << endl;
            copyMakeBorder(temp, frame[frameNum], 0, img.rows * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - temp.rows, 0, img.cols * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - temp.cols, BORDER_CONSTANT, 0);
            writer.write(frame[frameNum]);
        }
    }
    for (int frameNum = frames - 1; frameNum >= 0; frameNum--)
        writer.write(frame[frameNum]);
    cout << "done..." << endl;

    delete[] frame;
}

void makeVideo_cols_bothways(int B, int frames, int fps, Mat& img)
{
    Size size = Size(img.cols * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200, img.rows);
    VideoWriter writer;
    video_name = "./result_cols_bothways.avi";
    writer.open(video_name, VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, size, true);
    Mat* frame;
    frame = new Mat[frames];

    for (int frameNum = 0; frameNum < frames; frameNum++)
    {

        if (frameNum == 0) {
            copyMakeBorder(img, frame[frameNum], 0, 0, (img.cols * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - img.cols) / 2, (img.cols * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - img.cols) / 2, BORDER_CONSTANT, 0); // ������ ���θ� 800�ȼ��� ���־�����, ���ŵ� �̹����� ä���� ���� �κ��� ���������� ǥ��
            writer.write(frame[frameNum]);
        }
        else
        {
            Mat temp(img.rows, img.cols * (1 + (double)MAX_STRETCH_PERCENTAGE / (frames - 1) * frameNum * 0.01), CV_8UC3, Vec3b(0, 0, 0));
            stretch_cols_general(B, (double)MAX_STRETCH_PERCENTAGE / (frames - 1) * frameNum * 0.01, img, temp);
            cout << (double)MAX_STRETCH_PERCENTAGE / (frames - 1) * frameNum << "%" << endl;
            copyMakeBorder(temp, frame[frameNum], 0, 0, (img.cols * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - temp.cols) / 2, (img.cols * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - temp.cols) / 2, BORDER_CONSTANT, 0); // ������ ���θ� 800�ȼ��� ���־�����, ���ŵ� �̹����� ä���� ���� �κ��� ���������� ǥ��
            writer.write(frame[frameNum]);
        }
    }
    for (int frameNum = frames - 1; frameNum >= 0; frameNum--)
        writer.write(frame[frameNum]);
    cout << "done..." << endl;
    delete[] frame;
}

void makeVideo_rows_bothways(int B, int frames, int fps, Mat& img)
{
    Size size = Size(img.cols, img.rows * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200);
    VideoWriter writer;
    video_name = "./result_rows_bothways.avi";
    writer.open(video_name, VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, size, true);
    Mat* frame;
    frame = new Mat[frames];
    for (int frameNum = 0; frameNum < frames; frameNum++)
    {
        if (frameNum == 0) {
            copyMakeBorder(img, frame[frameNum], (img.rows * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - img.rows) / 2, (img.rows * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - img.rows) / 2, 0, 0, BORDER_CONSTANT, 0);
            writer.write(frame[frameNum]);
        }
        else
        {
            Mat temp(img.rows * (1 + (double)MAX_STRETCH_PERCENTAGE / (frames - 1) * frameNum * 0.01), img.cols, CV_8UC3, Vec3b(0, 0, 0));
            stretch_rows_general(B, (double)MAX_STRETCH_PERCENTAGE / (frames - 1) * frameNum * 0.01, img, temp);
            cout << (double)MAX_STRETCH_PERCENTAGE / (frames - 1) * frameNum << "%" << endl;
            copyMakeBorder(temp, frame[frameNum], (img.rows * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - temp.rows) / 2, (img.rows * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - temp.rows) / 2, 0, 0, BORDER_CONSTANT, 0);
            writer.write(frame[frameNum]);
        }
    }
    for (int frameNum = frames - 1; frameNum >= 0; frameNum--)
        writer.write(frame[frameNum]);
    cout << "done..." << endl;

    delete[] frame;
}

void makeVideo_diag_bothways(int B, int frames, int fps, Mat& img)
{
    Size size = Size(img.cols * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200, img.rows * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200);
    VideoWriter writer;
    video_name = "./result_diag_bothways.avi";
    writer.open(video_name, VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, size, true);
    Mat* frame;
    frame = new Mat[frames];
    for (int frameNum = 0; frameNum < frames; frameNum++)
    {
        if (frameNum == 0) {
            copyMakeBorder(img, frame[frameNum], (img.rows * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - img.rows) / 2, (img.rows * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - img.rows) / 2, (img.cols * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - img.cols) / 2, (img.cols * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - img.cols) / 2, BORDER_CONSTANT, 0);
            writer.write(frame[frameNum]);
        }
        else
        {
            Mat temp(img.rows * (1 + (double)MAX_STRETCH_PERCENTAGE / (frames - 1) * frameNum * 0.01), img.cols * (1 + (double)MAX_STRETCH_PERCENTAGE / (frames - 1) * frameNum * 0.01), CV_8UC3, Vec3b(0, 0, 0));
            stretch_diag_general(B, (double)MAX_STRETCH_PERCENTAGE / (frames - 1) * frameNum * 0.01, img, temp);
            cout << (double)MAX_STRETCH_PERCENTAGE / (frames - 1) * frameNum << "%" << endl;
            copyMakeBorder(temp, frame[frameNum], (img.rows * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - temp.rows) / 2, (img.rows * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - temp.rows) / 2, (img.cols * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - temp.cols) / 2, (img.cols * (1 + (double)MAX_STRETCH_PERCENTAGE / 100) + 200 - temp.cols) / 2, BORDER_CONSTANT, 0);
            writer.write(frame[frameNum]);
        }
    }
    for (int frameNum = frames - 1; frameNum >= 0; frameNum--)
        writer.write(frame[frameNum]);
    cout << "done..." << endl;

    delete[] frame;
}

void showFrames_cols(int B, Mat& img)
{
    vector<Mat> v;
    for (double i = 0; i <= 0; i++)
    {
        Mat temp(src.rows, src.cols, CV_8UC3, Vec3b(0, 0, 0));
        stretch_cols_general(B, i * 0.01, img, temp);
        cout << i << "%" << endl;
        v.push_back(temp);
        string s = "img" + to_string(i) + ".bmp";
        imwrite(s, temp);
    }
    for (int i = 0; i < v.size(); i++)
    {
        imshow("extending...", v[i]);
        waitKey();
    }
    waitKey(0);
}

void showFrames_rows(int B, Mat& img)
{
    vector<Mat> v;
    for (double i = 1.25; i <= 1.25; i++)
    {
        Mat temp(src.rows, src.cols, CV_8UC3, Vec3b(0, 0, 0));
        stretch_rows_general(B, i * 0.01, img, temp);
        cout << i << "%" << endl;
        v.push_back(temp);
        string s = "img" + to_string(i) + ".png";
        imwrite(s, temp);
    }
    for (int i = 0; i < v.size(); i++)
    {
        imshow("extending...", v[i]);
        waitKey(1000);
    }
    waitKey(0);
}

void showFrames_diag(int B, Mat& img)
{
    vector<Mat> v;
    for (int i = 1; i <= 20; i++)
    {
        Mat temp(src.rows, src.cols, CV_8UC3, Vec3b(0, 0, 0));
        stretch_diag_general(B, i * 0.01, img, temp);
        cout << i << "%" << endl;
        v.push_back(temp);
        string s = "img" + to_string(i) + ".png";
        imwrite(s, temp);
    }
    for (int i = 0; i < v.size(); i++)
    {
        imshow("extending...", v[i]);
        waitKey(1000);
    }
    waitKey(0);
}

int readvideo() {
    cv::VideoCapture videoCapture(video_name);
    int count = 1;
    int playcount = 1;

    cout << "���ϴ� �ݺ� ȸ���� �Է����ּ���" << endl;
    cin >> playcount;

    playVideo(videoCapture, playcount, video_name);

    string wannastop;

    bool stop = 0;

    while (stop < 1) {
        cout << "�ٽ� ��� �Ͻðڽ��ϱ�? (Y/N)" << endl;
        cin >> wannastop;

        if (wannastop == "Y" || wannastop == "y") {
            cv::VideoCapture videoCapture(video_name);
            playVideo(videoCapture, playcount, video_name);
            count++;
        }

        else if (wannastop == "N" || wannastop == "n")
            stop = 1;
    }

    cout << "�ݺ� ȸ�� : " << count << endl;
    cout << "Finished" << endl;

    return 0;
}

void playVideo(VideoCapture videoCapture, int playcount, string video_name) {

    //������ �ҷ����� ���� ��
    if (!videoCapture.isOpened()) {
        std::cout << "Can't open video !!!" << std::endl;
        return;
    }

    videoCapture.set(CAP_PROP_FPS, 60);
    //OpenCV Mat class
    cv::Mat videoFrame;

    float videoFPS = videoCapture.get(cv::CAP_PROP_FPS);

    //�̹����� window�� �����Ͽ� �����ݴϴ�.
    cv::namedWindow(video_name);

    //video ��� ����
    for (int i = 0; i < playcount; i++) {
        videoCapture.open(video_name);

        while (true) {
            //VideoCapture�� ���� �������� �޾ƿ´�
            videoCapture >> videoFrame;

            //ĸ�� ȭ���� ���� ���� Video�� ���� ���
            if (videoFrame.empty()) {
                break;
            }

            cv::imshow(video_name, videoFrame);

            //'ESC'Ű�� ������ ����ȴ�.
            //FPS�� �̿��Ͽ� ���� ��� �ӵ��� �����Ͽ��ش�.
            if (cv::waitKey(1000 / videoFPS) == 27) {
                std::cout << "Stop Video" << std::endl;
                break;
            }
        }
    }

    destroyWindow(video_name);
}

//=============================�÷� ��Ī�� �Լ�==========================
void setMatrix()
{
    PC_Mx.SetValue(Primary, 3, 3);
    IPC_Mx.SetValue(InvPrimary, 3, 3);
}

bool cmp_RGBs(pair<int, double> a, pair<int, double> b)
{
    return (double)a.second < (double)b.second;
}

bool cmp_XYZ(pair<int, double> a, pair<int, double> b)
{
    return a.first < b.first;
}

void getMatrixfromTXT()
{
    string in_line;
    ifstream in("Matrix_DP1.txt");

    int i = 0;
    while (getline(in, in_line))
    {
        if (i < 9) Primary[i] = stod(in_line);
        else InvPrimary[i - 9] = stod(in_line);
        i++;
    }
}

void getRsGsBsfromTXT(pair<int, double>* Rs, pair<int, double>* Gs, pair<int, double>* Bs)
{
    string in_line;
    //ifstream in("RsGsBs_table.txt");
    ifstream in("RsGsBs_Table_DP1.txt");

    int i = 0;
    while (getline(in, in_line))
    {
        int channel = 1;
        istringstream ss(in_line);
        string stringBuffer;
        while (getline(ss, stringBuffer, '\t'))
        {
            pair<int, double> tmp_pair = make_pair(254 - i * 2, stod(stringBuffer));
            switch (channel)
            {
            case 1:
                Rs[i] = tmp_pair;
                channel = 2;
                break;
            case 2:
                Gs[i] = tmp_pair;
                channel = 3;
                break;
            case 3:
                Bs[i] = tmp_pair;
                channel = 1;
                break;
            }
        }
        i++;
    }

    in.close();
}

pair<double, pair<double, double> > calXYZofAdj_Pixel(int adj_R, int adj_G, int adj_B, pair<int, double>* Rs, pair<int, double>* Gs, pair<int, double>* Bs)
{
    pair<int, double> tmp_Rs[128];
    pair<int, double> tmp_Gs[128];
    pair<int, double> tmp_Bs[128];
    copy(Rs, Rs + 128, tmp_Rs);
    copy(Rs, Rs + 128, tmp_Gs);
    copy(Rs, Rs + 128, tmp_Bs);
    for (int i = 0; i < 128; i++)
    {
        tmp_Rs[i].first = abs(tmp_Rs[i].first - adj_R);
        tmp_Gs[i].first = abs(tmp_Gs[i].first - adj_G);
        tmp_Bs[i].first = abs(tmp_Bs[i].first - adj_B);
    }
    sort(tmp_Rs, tmp_Rs + 128, cmp_XYZ);
    sort(tmp_Gs, tmp_Gs + 128, cmp_XYZ);
    sort(tmp_Bs, tmp_Bs + 128, cmp_XYZ);

    double tmp_RGBs[] = { tmp_Rs[0].second, tmp_Gs[0].second, tmp_Bs[0].second };
    Matrix tmp_RGBs_Mx;
    tmp_RGBs_Mx.SetValue(tmp_RGBs, 3, 1);
    Matrix ans = PC_Mx * tmp_RGBs_Mx;

    return pair<double, pair<double, double> >(ans.GetValue(1, 1), pair<double, double>(ans.GetValue(2, 1), ans.GetValue(3, 1)));
}

pair<int, pair<int, int> > calNewRGB(pair<int, double>* Rs, pair<int, double>* Gs, pair<int, double>* Bs, double cal_value_Rs, double cal_value_Gs, double cal_value_Bs)
{
    // Rs, Gs, Bs�� ���ؼ��� �ȵǴµ� �̷��� ���ϴ� ���� �߻� 
    // ���ο� �迭 �ʿ���
    pair<int, double> tmp_Rs[128];
    pair<int, double> tmp_Gs[128];
    pair<int, double> tmp_Bs[128];
    copy(Rs, Rs + 128, tmp_Rs);
    copy(Rs, Rs + 128, tmp_Gs);
    copy(Rs, Rs + 128, tmp_Bs);
    for (int i = 0; i < 128; i++)
    {
        tmp_Rs[i].second = abs(tmp_Rs[i].second - cal_value_Rs);
        tmp_Gs[i].second = abs(tmp_Gs[i].second - cal_value_Gs);
        tmp_Bs[i].second = abs(tmp_Bs[i].second - cal_value_Bs);
    }

    sort(tmp_Rs, tmp_Rs + 128, cmp_RGBs);
    sort(tmp_Gs, tmp_Gs + 128, cmp_RGBs);
    sort(tmp_Bs, tmp_Bs + 128, cmp_RGBs);

    return pair<int, pair<int, int> >(tmp_Rs[0].first, pair<int, int>(tmp_Gs[0].first, tmp_Bs[0].first));
}

Matrix calRsGsBs(pair<double, pair<double, double> > adjacentXYZ, double* XYZ, double percentage)
{
    XYZ[0] = percentage * adjacentXYZ.first;
    XYZ[1] = percentage * adjacentXYZ.second.first;
    XYZ[2] = percentage * adjacentXYZ.second.second;

    Matrix XYZ_Mx;
    XYZ_Mx.SetValue(XYZ, 3, 1);

    return IPC_Mx * XYZ_Mx;
}

Matrix calRsGsBs2(pair<double, pair<double, double> > adjacentXYZ1, pair<double, pair<double, double> > adjacentXYZ2, double* XYZ, double percentage1, double percentage2)
{
    XYZ[0] = adjacentXYZ1.first * percentage1 + adjacentXYZ2.first * percentage2;
    XYZ[1] = adjacentXYZ1.second.first * percentage1 + adjacentXYZ2.second.first * percentage2;
    XYZ[2] = adjacentXYZ1.second.second * percentage1 + adjacentXYZ2.second.second * percentage2;

    Matrix XYZ_Mx;
    XYZ_Mx.SetValue(XYZ, 3, 1);

    return IPC_Mx * XYZ_Mx;
}

void printRsGsBsArr(pair<int, double>* Rs, pair<int, double>* Gs, pair<int, double>* Bs)
{
    for (int i = 0; i < 128; i++)
    {
        cout << fixed;
        cout.precision(6);
        cout << Rs[i].first << "\t\t" << Rs[i].second << "\t\t" << Gs[i].first << ' ' << Gs[i].second << "\t\t" << Bs[i].first << ' ' << Bs[i].second << endl;
    }
}