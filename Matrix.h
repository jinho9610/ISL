#pragma once

class Matrix
{
private:
	int m_nRows;
	int m_nCols;
	double* m_pfMat;
public:
	Matrix(); // ����Ʈ ������
	Matrix(const Matrix& mxInput);
	~Matrix(); // �Ҹ���

	void show(); // �� Ȯ��, ������ printf �Լ�
	void SetValue(int* jpnValue, int nRows, int nCols);
	void SetValue(double* pfValue, int nRows, int nCols);
	// void SetValue(double* pfValue, int nRows, int nCols);
	double GetValue(int nRwos, int nClos);

	Matrix operator*(const int nInput) const; // ��Į�� ��
	Matrix operator*(const float fInput) const; // ���� �Ҽ��� ��
	Matrix operator*(const double dInput) const; // �ε� �Ҽ��� ��
	Matrix operator*(const Matrix& mxInput) const; // ��� ��

	Matrix Trans(); // ��ġ���
	// Matrix inv(); // �����
};