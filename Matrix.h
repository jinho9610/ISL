#pragma once

class Matrix
{
private:
	int m_nRows;
	int m_nCols;
	double* m_pfMat;
public:
	Matrix(); // 디폴트 생성자
	Matrix(const Matrix& mxInput);
	~Matrix(); // 소멸자

	void show(); // 값 확인, 일종의 printf 함수
	void SetValue(int* jpnValue, int nRows, int nCols);
	void SetValue(double* pfValue, int nRows, int nCols);
	// void SetValue(double* pfValue, int nRows, int nCols);
	double GetValue(int nRwos, int nClos);

	Matrix operator*(const int nInput) const; // 스칼라 곱
	Matrix operator*(const float fInput) const; // 고정 소수점 곱
	Matrix operator*(const double dInput) const; // 부동 소수점 곱
	Matrix operator*(const Matrix& mxInput) const; // 행렬 곱

	Matrix Trans(); // 전치행렬
	// Matrix inv(); // 역행렬
};