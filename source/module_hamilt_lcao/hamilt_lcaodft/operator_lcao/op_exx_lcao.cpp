#ifdef __EXX
#include "op_exx_lcao.h"
#include "module_base/blacs_connector.h"

namespace hamilt
{

template<>
void OperatorEXX<OperatorLCAO<double, double>>::add_loaded_Hexx(const int ik)
{
    BlasConnector::axpy(this->hR->get_paraV()->get_local_size(), 1.0, this->Hexxd_k_load[ik].data(), 1, this->hsk->get_hk(), 1);
}
template<>
void OperatorEXX<OperatorLCAO<std::complex<double>, double>>::add_loaded_Hexx(const int ik)
{
    BlasConnector::axpy(this->hR->get_paraV()->get_local_size(), 1.0, this->Hexxc_k_load[ik].data(), 1, this->hsk->get_hk(), 1);
}
template<>
void OperatorEXX<OperatorLCAO<std::complex<double>, std::complex<double>>>::add_loaded_Hexx(const int ik)
{
    BlasConnector::axpy(this->hR->get_paraV()->get_local_size(), 1.0, this->Hexxc_k_load[ik].data(), 1, this->hsk->get_hk(), 1);
}

} // namespace hamilt
#endif