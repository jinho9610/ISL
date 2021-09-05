#include "Matrix.h"
#include <iostream>
#include <cstdio>

using namespace std;

Matrix::Matrix()
{
	m_nCols = 0;
	m_nRows = 0;
	m_pfMat = 0; // �� ������, �� � ���� ���� ������ ����
}

Matrix::~Matrix() // �Ҹ���
{
	if (m_pfMat) // ��� ��(������)�� ���������� 
	{
		delete(m_pfMat); // �����ϰ�
		m_pfMat = 0; // �������ͷ� �������
	}
}

Matrix::Matrix(const Matrix& mxInput)
{
	{
		m_nCols = mxInput.m_nCols;
		m_nRows = mxInput.m_nRows;

		m_pfMat = new double[m_nRows * m_nCols]; // ��ü ������ ������ŭ 1���� �迭 �����Ҵ�
		for (int i = 0; i < m_nRows * m_nCols; i++) // ������ �迭�� �ƴ�, �׳� 1���� �迭�� ������ ��Ƶ�
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

void Matrix::SetValue(int* pnValue, int nRows, int nCols) // ��� ���Ұ� �������� ���
{
	m_nRows = nRows;
	m_nCols = nCols;

	if (m_pfMat) delete(m_pfMat); // ������ �ִ� ��(������) ����

	m_pfMat = new double[nRows * nCols];

	for (int i = 0; i < nRows * nCols; i++) m_pfMat[i] = pnValue[i];
}

void Matrix::SetValue(double* pfValue, int nRows, int nCols) // ��� ���Ұ� �Ҽ����� ���
{
	m_nRows = nRows;
	m_nCols = nCols;

	if (m_pfMat) delete(m_pfMat); // ������ �ִ� ��(������) ����

	m_pfMat = new double[nRows * nCols];

	for (int i = 0; i < nRows * nCols; i++) m_pfMat[i] = pfValue[i];
}

double Matrix::GetValue(int nRows, int nCols)
{
	return m_pfMat[(nRows - 1) * m_nCols + nCols - 1]; // nRows, nCols�� �ε��� + 1 �ӿ� �ָ�
}

Matrix Matrix::operator *(const int nInput) const // ��Į�� ��
{
	Matrix mxTemp; // �ӽ� ��Ʈ����
	double* pfTmp = new double[m_nRows * m_nCols]; // ������ �������� ���� ��� �� �ϳ� ����

	for (int i = 0; i < m_nRows * m_nCols; i++) pfTmp[i] = m_pfMat[i] * nInput; // ���� ���ҿ� ��Į�� �� ����

	mxTemp.m_nCols = m_nCols;
	mxTemp.m_nRows = m_nRows;
	mxTemp.SetValue(pfTmp, m_nRows, m_nCols);

	delete(pfTmp); // ���� ��� �޸� ����

	return mxTemp;
}

Matrix Matrix::operator *(const float nInput) const // �����Ҽ��� ��
{
	Matrix mxTemp; // �ӽ� ��Ʈ����
	double* pfTmp = new double[m_nRows * m_nCols]; // ������ �������� ���� ��� �� �ϳ� ����

	for (int i = 0; i < m_nRows * m_nCols; i++) pfTmp[i] = m_pfMat[i] * nInput; // ���� ���ҿ� ��Į�� �� ����

	mxTemp.m_nCols = m_nCols;
	mxTemp.m_nRows = m_nRows;
	mxTemp.SetValue(pfTmp, m_nRows, m_nCols);

	delete(pfTmp); // ���� ��� �޸� ����

	return mxTemp;
}

Matrix Matrix::operator *(const double nInput) const // �ε��Ҽ��� ��
{
	Matrix mxTemp; // �ӽ� ��Ʈ����
	double* pfTemp = new double[m_nRows * m_nCols]; // ������ �������� ���� ��� �� �ϳ� ����

	for (int i = 0; i < m_nRows * m_nCols; i++) pfTemp[i] = m_pfMat[i] * nInput; // ���� ���ҿ� ��Į�� �� ����

	mxTemp.m_nCols = m_nCols;
	mxTemp.m_nRows = m_nRows;
	mxTemp.SetValue(pfTemp, m_nRows, m_nCols);

	delete(pfTemp); // ���� ��� �޸� ����

	return mxTemp;
}

Matrix Matrix::operator *(const Matrix& mxInput) const // ��� ��
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