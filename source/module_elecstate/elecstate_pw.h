#ifndef ELECSTATEPW_H
#define ELECSTATEPW_H

#include <module_base/macros.h>

#include "elecstate.h"
#include "module_basis/module_pw/pw_basis_k.h"
#include "module_elecstate/kernels/elecstate_op.h"
#include "module_hamilt_pw/hamilt_pwdft/kernels/meta_op.h"
#include "module_hsolver/kernels/math_kernel_op.h"

namespace elecstate
{

template<typename T, typename Device = psi::DEVICE_CPU>
class ElecStatePW : public ElecState
{
  private:
    using Real = typename GetTypeReal<T>::type;
  public:
    ElecStatePW(ModulePW::PW_Basis_K* wfc_basis_in,
                Charge* chg_in,
                K_Vectors* pkv_in,
                UnitCell* ucell_in,
                pseudopot_cell_vnl* ppcell_in,
                ModulePW::PW_Basis* rhodpw_in,
                ModulePW::PW_Basis* rhopw_in,
                ModulePW::PW_Basis_Big* bigpw_in);
    // void init(Charge* chg_in):charge(chg_in){} override;

    ~ElecStatePW();
    // interface for HSolver to calculate rho from Psi
    virtual void psiToRho(const psi::Psi<T, Device>& psi);
    // return current electronic density rho, as a input for constructing Hamiltonian
    // const double* getRho(int spin) const override;

    // update charge density for next scf step
    // void getNewRho() override;

    Real* becsum = nullptr;

  protected:
    ModulePW::PW_Basis* rhopw_smooth = nullptr;
    ModulePW::PW_Basis_K* basis = nullptr;
    UnitCell* ucell = nullptr;
    const pseudopot_cell_vnl* ppcell = nullptr;

    // calculate electronic charge density on grid points or density matrix in real space
    // the consequence charge density rho saved into rho_out, preparing for charge mixing.
    void updateRhoK(const psi::Psi<T, Device>& psi); // override;
    // sum over all pools for rho and ebands
    void parallelK();
    // calcualte rho for each k
    void rhoBandK(const psi::Psi<T, Device>& psi);
    // add to the charge density in reciprocal space the part which is due to the US augmentation.
    void add_usrho(const psi::Psi<T, Device>& psi);
    // \sum_lm Q_lm(r) \sum_i <psi_i|beta_l><beta_m|psi_i> w_i
    void addusdens_g(const Real* becsum, T* rhog);

    void init_rho_data();

    Device * ctx = {};
    bool init_rho = false;
    mutable T* vkb = nullptr;
    Real ** rho = nullptr, ** kin_r = nullptr;
    Real * rho_data = nullptr, * kin_r_data = nullptr;
    T * wfcr = nullptr, * wfcr_another_spin = nullptr;

    using meta_op = hamilt::meta_pw_op<Real, Device>;
    using elecstate_pw_op = elecstate::elecstate_pw_op<Real, Device>;

    using setmem_var_op = psi::memory::set_memory_op<Real, Device>;
    using resmem_var_op = psi::memory::resize_memory_op<Real, Device>;
    using delmem_var_op = psi::memory::delete_memory_op<Real, Device>;
    using castmem_var_d2h_op = psi::memory::cast_memory_op<double, Real, psi::DEVICE_CPU, Device>;

    using setmem_complex_op = psi::memory::set_memory_op<T, Device>;
    using resmem_complex_op = psi::memory::resize_memory_op<T, Device>;
    using delmem_complex_op = psi::memory::delete_memory_op<T, Device>;

    using gemv_op = hsolver::gemv_op<T, Device>;
    using gemm_op = hsolver::gemm_op<T, Device>;
};

} // namespace elecstate

#endif