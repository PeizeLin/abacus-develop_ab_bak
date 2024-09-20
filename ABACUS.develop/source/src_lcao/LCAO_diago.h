#ifndef DIAGO_LCAO_MATRIX_H
#define DIAGO_LCAO_MATRIX_H

#include "../src_pw/tools.h"

class Diago_LCAO_Matrix
{
	public:
	Diago_LCAO_Matrix();
	~Diago_LCAO_Matrix();

	void solve_double_matrix(const int &ik, double **wfc, matrix &wfc_2d)const;
	void solve_complex_matrix(const int &ik, complex<double> **wfc, ComplexMatrix &wfc_2d)const;

	private:

	void using_LAPACK(const int &ik, double **wfc)const;
	void using_LAPACK_complex(const int &ik, complex<double> **wfc)const;
	void using_CG(const int &ik, double **c)const;

#ifdef __MPI
	void using_HPSEPS_double(const int &ik, double **wfc, matrix &wfc_2d)const;
	void using_HPSEPS_complex(const int &ik, complex<double> **wfc, ComplexMatrix &wfc_2d)const;
#endif

};

#endif
