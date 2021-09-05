#include "Matrix.h"
#include <iostream>
#include <cstdio>

using namespace std;

Matrix::Matrix()
{
	m_nCols = 0;
	m_nRows = 0;
	m_pfMat = 0; // 널 포인터, 그 어떤 값도 보유 중이지 않음
}

Matrix::~Matrix() // 소멸자
{
	if (m_pfMat) // 행렬 값(포인터)가 남아있으면 
	{
		delete(m_pfMat); // 삭제하고
		m_pfMat = 0; // 널포인터로 만들어줌
	}
}

Matrix::Matrix(const Matrix& mxInput)
{
	{
		m_nCols = mxInput.m_nCols;
		m_nRows = mxInput.m_nRows;

		m_pfMat = new double[m_nRows * m_nCols]; // 전체 원소의 개수만큼 1차원 배열 동적할당
		for (int i = 0; i < m_nRows * m_nCols; i++) // 이차원 배열이 아닌, 그냥 1차원 배열로 데이터 담아둠
			m_pfMat[i] = mxInput.m_pfMat[i];
	}
}

void Matrix::show()
{
	for (int i = 0; i < m_nRows; i++)
	{
		for (int j = 0; j < m_nCols; j++)
		{
			printf("%3.7f ", m_pfMat[i * m_nCols + j]);
		}
		cout << "\n";
	}
}

void Matrix::SetValue(int* pnValue, int nRows, int nCols) // 행렬 원소가 정수들인 경우
{
	m_nRows = nRows;
	m_nCols = nCols;

	if (m_pfMat) delete(m_pfMat); // 기존에 있던 값(포인터) 삭제

	m_pfMat = new double[nRows * nCols];

	for (int i = 0; i < nRows * nCols; i++) m_pfMat[i] = pnValue[i];
}

void Matrix::SetValue(double* pfValue, int nRows, int nCols) // 행렬 원소가 소수들인 경우
{
	m_nRows = nRows;
	m_nCols = nCols;

	if (m_pfMat) delete(m_pfMat); // 기존에 있던 값(포인터) 삭제

	m_pfMat = new double[nRows * nCols];

	for (int i = 0; i < nRows * nCols; i++) m_pfMat[i] = pfValue[i];
}

double Matrix::GetValue(int nRows, int nCols)
{
	return m_pfMat[(nRows - 1) * m_nCols + nCols - 1]; // nRows, nCols는 인덱스 + 1 임에 주목
}

Matrix Matrix::operator *(const int nInput) const // 스칼라 곱
{
	Matrix mxTemp; // 임시 매트릭스
	double* pfTmp = new double[m_nRows * m_nCols]; // 동일한 사이즈의 더미 행렬 값 하나 만듦

	for (int i = 0; i < m_nRows * m_nCols; i++) pfTmp[i] = m_pfMat[i] * nInput; // 기존 원소에 스칼라 곱 적용

	mxTemp.m_nCols = m_nCols;
	mxTemp.m_nRows = m_nRows;
	mxTemp.SetValue(pfTmp, m_nRows, m_nCols);

	delete(pfTmp); // 더미 행렬 메모리 해제

	return mxTemp;
}

Matrix Matrix::operator *(const float nInput) const // 고정소수점 곱
{
	Matrix mxTemp; // 임시 매트릭스
	double* pfTmp = new double[m_nRows * m_nCols]; // 동일한 사이즈의 더미 행렬 값 하나 만듦

	for (int i = 0; i < m_nRows * m_nCols; i++) pfTmp[i] = m_pfMat[i] * nInput; // 기존 원소에 스칼라 곱 적용

	mxTemp.m_nCols = m_nCols;
	mxTemp.m_nRows = m_nRows;
	mxTemp.SetValue(pfTmp, m_nRows, m_nCols);

	delete(pfTmp); // 더미 행렬 메모리 해제

	return mxTemp;
}

Matrix Matrix::operator *(const double nInput) const // 부동소수점 곱
{
	Matrix mxTemp; // 임시 매트릭스
	double* pfTemp = new double[m_nRows * m_nCols]; // 동일한 사이즈의 더미 행렬 값 하나 만듦

	for (int i = 0; i < m_nRows * m_nCols; i++) pfTemp[i] = m_pfMat[i] * nInput; // 기존 원소에 스칼라 곱 적용

	mxTemp.m_nCols = m_nCols;
	mxTemp.m_nRows = m_nRows;
	mxTemp.SetValue(pfTemp, m_nRows, m_nCols);

	delete(pfTemp); // 더미 행렬 메모리 해제

	return mxTemp;
}

Matrix Matrix::operator *(const Matrix& mxInput) const // 행렬 곱
{
	if (m_nCols != mxInput.m_nRows)
	{
		cout << "Matrix dimension error" << endl;
		exit(-1);
	}

	Matrix mxTemp;
	double* pfTemp = new double[m_nRows * mxInput.m_nCols];

	for (int i = 0; i < m_nRows * mxInput.m_nCols; i++) pfTemp[i] = 0.0;

	int nIndex = 0;
	for (int i = 0; i < m_nRows; i++)
	{
		for (int j = 0; j < mxInput.m_nCols; j++)
		{
			for (int k = 0; k < m_nCols; k++)
				pfTemp[i * mxInput.m_nCols + j] += m_pfMat[i * m_nCols + k] * mxInput.m_pfMat[j + k * mxInput.m_nCols];
		}
	}

	mxTemp.SetValue(pfTemp, m_nRows, mxInput.m_nCols);

	delete(pfTemp);

	return mxTemp;
}

Matrix Matrix::Trans()
{
	Matrix mxTrans;
	double* pfTemp = new double[m_nRows * m_nCols];

	for (int i = 0; i < m_nRows; i++)
	{
		for (int j = 0; j < m_nCols; j++)
			pfTemp[i + m_nRows * j] = m_pfMat[i + m_nCols + j];
	}

	mxTrans.SetValue(pfTemp, m_nCols, m_nRows);

	delete(pfTemp);
	return mxTrans;
}