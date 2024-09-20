//==========================================================
// AUTHOR : mohan
// DATE : 2021-01-04
//==========================================================
#ifndef NUMERICAL_DESCRIPTOR_H
#define NUMERICAL_DESCRIPTOR_H
#include "../src_pw/tools.h"
#include "bessel_basis.h"
//==========================================================
// CLASS :
// NAME :  Numerical_Descriptor 
//==========================================================
class Numerical_Descriptor
{
	public:
	Numerical_Descriptor();
	~Numerical_Descriptor();

	void output_descriptor( const ComplexMatrix *psi, const int &lmax_in); // mohan added 2021-01-03

	private:

	bool init_label;

	int lmax; // lmax for descriptor
	int nmax; // nmax for descriptor
	int nlocal; // total number of atomic orbitals

	Bessel_Basis bessel_basis;

	IntArray *mu_index;
	void init_mu_index(void);//mohan added 2021-01-03

	void jlq3d_overlap(realArray &overlap_Q1, realArray &overlap_Q2,
		const int &ik_ibz, const int &ik, const int &np, const ComplexMatrix &psi);

	void generate_descriptor(realArray &overlap_Q1, realArray &overlap_Q2, 
		const int &it, const int &ia, double *d, const int &nd);

};

#endif
