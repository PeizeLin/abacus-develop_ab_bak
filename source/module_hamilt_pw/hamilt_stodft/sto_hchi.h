#ifndef STO_HCHI_H
#define STO_HCHI_H
#include "module_basis/module_pw/pw_basis_k.h"
#include "module_cell/klist.h"

//-----------------------------------------------------
// h * chi
// chi: stochastic wave functions
//
// h: the normalized Hamiltonian matrix, which equals to (H - E\bar) / \DeltaE
// where H is decomposed into the electron kinetic, effective potential V(r),
// and the non-local pseudopotentials.
// The effective potential = Local pseudopotential +
// Hartree potential + Exchange-correlation potential
//------------------------------------------------------
class Stochastic_hchi
{

  public:
    // constructor and deconstructor
    Stochastic_hchi();
    ~Stochastic_hchi();

    void init(ModulePW::PW_Basis_K* wfc_basis, K_Vectors* pkv, double* Emin_in, double* Emax_in);

    double* Emin = nullptr;
    double* Emax = nullptr;

    void orthogonal_to_psi_reciprocal(std::complex<double>* wfin,
                                      std::complex<double>* wfout,
                                      const int& ikk); // wfin & wfout are wavefunctions in reciprocal space
    void hchi(std::complex<double>* wfin,
              std::complex<double>* wfout,
              const int m = 1); // wfin & wfout are wavefunctions in reciprocal space

    void hchi_norm(std::complex<double>* wfin,
                   std::complex<double>* wfout,
                   const int m = 1); // wfin & wfout are wavefunctions in reciprocal space

  public:
    int current_ik = 0;
    ModulePW::PW_Basis_K* wfcpw = nullptr;
    K_Vectors* pkv = nullptr;

    // chi should be orthogonal to psi (generated by diaganolization methods,
    // such as CG)
};

#endif // Eelectrons_hchi
