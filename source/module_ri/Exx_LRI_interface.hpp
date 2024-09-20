#ifndef EXX_LRI_INTERFACE_HPP
#define EXX_LRI_INTERFACE_HPP
#include "module_parameter/parameter.h"

#include "Exx_LRI_interface.h"
#include "module_ri/exx_abfs-jle.h"
#include "module_ri/exx_opt_orb.h"
#include "module_hamilt_lcao/hamilt_lcaodft/hamilt_lcao.h"
#include "module_hamilt_lcao/hamilt_lcaodft/operator_lcao/op_exx_lcao.h"
#include "module_base/parallel_common.h"
#include "module_base/formatter.h"

#include <sys/time.h>
#include "module_io/csr_reader.h"
#include "module_io/write_HS_sparse.h"

template<typename T, typename Tdata>
void Exx_LRI_Interface<T, Tdata>::write_Hexxs_cereal(const std::string& file_name) const
{
    ModuleBase::TITLE("Exx_LRI", "write_Hexxs_cereal");
    ModuleBase::timer::tick("Exx_LRI", "write_Hexxs_cereal");
    std::ofstream ofs(file_name + "_" + std::to_string(GlobalV::MY_RANK), std::ofstream::binary);
	cereal::BinaryOutputArchive oar(ofs);
    oar(this->exx_ptr->Hexxs);
    ModuleBase::timer::tick("Exx_LRI", "write_Hexxs_cereal");
}

template<typename T, typename Tdata>
void Exx_LRI_Interface<T, Tdata>::read_Hexxs_cereal(const std::string& file_name)
{
    ModuleBase::TITLE("Exx_LRI", "read_Hexxs_cereal");
    ModuleBase::timer::tick("Exx_LRI", "read_Hexxs_cereal");
    std::ifstream ifs(file_name + "_" + std::to_string(GlobalV::MY_RANK), std::ofstream::binary);
	cereal::BinaryInputArchive iar(ifs);
	iar(this->exx_ptr->Hexxs);
    ModuleBase::timer::tick("Exx_LRI", "read_Hexxs_cereal");
}

template<typename T, typename Tdata>
void Exx_LRI_Interface<T, Tdata>::exx_beforescf(const K_Vectors& kv, const Charge_Mixing& chgmix, const UnitCell& ucell, const Parallel_2D& pv, const LCAO_Orbitals& orb)
{
#ifdef __MPI
    if (GlobalC::exx_info.info_global.cal_exx)
    {
        if (GlobalC::restart.info_load.load_H_finish && !GlobalC::restart.info_load.restart_exx) { XC_Functional::set_xc_type(GlobalC::ucell.atoms[0].ncpp.xc_func);
        } else
        {
            if (ucell.atoms[0].ncpp.xc_func == "HF" || ucell.atoms[0].ncpp.xc_func == "PBE0" || ucell.atoms[0].ncpp.xc_func == "HSE")
            {
                XC_Functional::set_xc_type("pbe");
            }
            else if (ucell.atoms[0].ncpp.xc_func == "SCAN0")
            {
                XC_Functional::set_xc_type("scan");
            }
        }
        // initialize the rotation matrix in AO representation
        this->exx_spacegroup_symmetry = (GlobalV::NSPIN < 4 && ModuleSymmetry::Symmetry::symm_flag == 1);
        if (this->exx_spacegroup_symmetry)
        {
            const std::array<int, 3>& period = RI_Util::get_Born_vonKarmen_period(kv);
            this->symrot_.find_irreducible_sector(ucell.symm, ucell.atoms, ucell.st,
                RI_Util::get_Born_von_Karmen_cells(period), period, ucell.lat);
            // this->symrot_.set_Cs_rotation(this->exx_ptr->get_abfs_nchis());
            this->symrot_.cal_Ms(kv, ucell, pv);
        }

        this->exx_ptr->cal_exx_ions();
    }

		if (Exx_Abfs::Jle::generate_matrix)
		{
			//program should be stopped after this judgement
			Exx_Opt_Orb exx_opt_orb;
			exx_opt_orb.generate_matrix(kv, orb);
			ModuleBase::timer::tick("ESolver_KS_LCAO", "beforescf");
			return;
		}
		
		// set initial parameter for mix_DMk_2D
		if(GlobalC::exx_info.info_global.cal_exx)
        {
            if (this->exx_spacegroup_symmetry)
                {this->mix_DMk_2D.set_nks(kv.get_nkstot_full() * (GlobalV::NSPIN == 2 ? 2 : 1), PARAM.globalv.gamma_only_local);}
            else
                {this->mix_DMk_2D.set_nks(kv.get_nks(), PARAM.globalv.gamma_only_local);}
			if(GlobalC::exx_info.info_global.separate_loop) {
                this->mix_DMk_2D.set_mixing(nullptr);
			} else {
				this->mix_DMk_2D.set_mixing(chgmix.get_mixing());
            }
            // for exx two_level scf
            this->two_level_step = 0;
        }
#endif // __MPI
}

template<typename T, typename Tdata>
void Exx_LRI_Interface<T, Tdata>::exx_eachiterinit(const elecstate::DensityMatrix<T, double>& dm, const K_Vectors& kv, const int& iter)
{
    if (GlobalC::exx_info.info_global.cal_exx)
    {
        if (!GlobalC::exx_info.info_global.separate_loop && this->two_level_step)
        {
            const bool flag_restart = (iter == 1) ? true : false;
            if (this->exx_spacegroup_symmetry) { this->mix_DMk_2D.mix(symrot_.restore_dm(kv, dm.get_DMK_vector(), *dm.get_paraV_pointer()), flag_restart); }
            else { this->mix_DMk_2D.mix(dm.get_DMK_vector(), flag_restart); }
			const std::vector<std::map<int,std::map<std::pair<int, std::array<int, 3>>,RI::Tensor<Tdata>>>>
				Ds = PARAM.globalv.gamma_only_local
                ? RI_2D_Comm::split_m2D_ktoR<Tdata>(*this->exx_ptr->p_kv, this->mix_DMk_2D.get_DMk_gamma_out(), *dm.get_paraV_pointer(), GlobalV::NSPIN)
                : RI_2D_Comm::split_m2D_ktoR<Tdata>(*this->exx_ptr->p_kv, this->mix_DMk_2D.get_DMk_k_out(), *dm.get_paraV_pointer(), GlobalV::NSPIN, this->exx_spacegroup_symmetry);
            if (this->exx_spacegroup_symmetry && GlobalC::exx_info.info_global.exx_symmetry_realspace) { this->exx_ptr->cal_exx_elec(Ds, *dm.get_paraV_pointer(), &this->symrot_); }
            else { this->exx_ptr->cal_exx_elec(Ds, *dm.get_paraV_pointer()); }
        }
    }
}

template<typename T, typename Tdata>
void Exx_LRI_Interface<T, Tdata>::exx_hamilt2density(elecstate::ElecState& elec, const Parallel_Orbitals& pv, const  int iter)
{
    // Peize Lin add 2020.04.04
    if (XC_Functional::get_func_type() == 4 || XC_Functional::get_func_type() == 5)
    {
        // add exx
        // Peize Lin add 2016-12-03
        if (GlobalC::restart.info_load.load_H_finish && !GlobalC::restart.info_load.restart_exx
            && this->two_level_step == 0 && iter == 1)
        {
            if (GlobalV::MY_RANK == 0) {GlobalC::restart.load_disk("Eexx", 0, 1, &this->exx_ptr->Eexx);
}
            Parallel_Common::bcast_double(this->exx_ptr->Eexx);
            this->exx_ptr->Eexx /= GlobalC::exx_info.info_global.hybrid_alpha;
        }
        elec.set_exx(this->get_Eexx());
    }
    else
    {
        elec.f_en.exx = 0.;
    }
}

template<typename T, typename Tdata>
bool Exx_LRI_Interface<T, Tdata>::exx_after_converge(
    hamilt::Hamilt<T>& hamilt,
    const elecstate::DensityMatrix<T, double>& dm,
    const K_Vectors& kv,
    const int& nspin,
    int& iter,
    const double& etot,
    const double& scf_ene_thr)
{   // only called if (GlobalC::exx_info.info_global.cal_exx)
    auto restart_reset = [this]()
        { // avoid calling restart related procedure in the subsequent ion steps
            GlobalC::restart.info_load.restart_exx = true;
            this->exx_ptr->Eexx = 0;
        };

        // no separate_loop case
        if (!GlobalC::exx_info.info_global.separate_loop)
        {
            GlobalC::exx_info.info_global.hybrid_step = 1;

            // in no_separate_loop case, scf loop only did twice
            // in first scf loop, exx updated once in beginning,
            // in second scf loop, exx updated every iter

            if (this->two_level_step)
            {
                restart_reset();
                return true;
            }
            else
            {
                // update exx and redo scf
                XC_Functional::set_xc_type(GlobalC::ucell.atoms[0].ncpp.xc_func);
                iter = 0;
                std::cout << " Entering 2nd SCF, where EXX is updated" << std::endl;
                this->two_level_step++;
                return false;
            }
        }
        else
        { // has separate_loop case
            const double ediff = std::abs(etot - etot_last_outer_loop) * ModuleBase::Ry_to_eV;
            if (two_level_step) { std::cout << FmtCore::format("EDIFF/eV (outer loop): %.8e \n", ediff); }
            // exx converged or get max exx steps
            if (this->two_level_step == GlobalC::exx_info.info_global.hybrid_step
                || (iter == 1 && this->two_level_step != 0) // density convergence of outer loop
                || (ediff < scf_ene_thr && this->two_level_step != 0))   //energy convergence of outer loop
            {
                restart_reset();
                return true;
            }
            else
            {
                this->etot_last_outer_loop = etot;
                // update exx and redo scf
                if (this->two_level_step == 0)
                {
                    XC_Functional::set_xc_type(GlobalC::ucell.atoms[0].ncpp.xc_func);
                }

                std::cout << " Updating EXX " << std::flush;
                timeval t_start;       gettimeofday(&t_start, nullptr);

                const bool flag_restart = (this->two_level_step == 0) ? true : false;

                if (this->exx_spacegroup_symmetry)
                    {this->mix_DMk_2D.mix(symrot_.restore_dm(kv, dm.get_DMK_vector(), *dm.get_paraV_pointer()), flag_restart);}
                else
                    {this->mix_DMk_2D.mix(dm.get_DMK_vector(), flag_restart);}

                // GlobalC::exx_lcao.cal_exx_elec(p_esolver->LOC, p_esolver->LOWF.wfc_k_grid);
                const std::vector<std::map<int, std::map<std::pair<int, std::array<int, 3>>, RI::Tensor<Tdata>>>>
                    Ds = std::is_same<T, double>::value //gamma_only_local
                    ? RI_2D_Comm::split_m2D_ktoR<Tdata>(*this->exx_ptr->p_kv, this->mix_DMk_2D.get_DMk_gamma_out(), *dm.get_paraV_pointer(), nspin)
                    : RI_2D_Comm::split_m2D_ktoR<Tdata>(*this->exx_ptr->p_kv, this->mix_DMk_2D.get_DMk_k_out(), *dm.get_paraV_pointer(), nspin, this->exx_spacegroup_symmetry);

                // check the rotation of Ds
                // this->symrot_.test_HR_rotation(GlobalC::ucell.symm, GlobalC::ucell.atoms, GlobalC::ucell.st, 'D', Ds[0]);

                // check the rotation of H(R) before adding exx
                // this->symrot_.find_irreducible_sector(GlobalC::ucell.symm, GlobalC::ucell.atoms, GlobalC::ucell.st, this->symrot_.get_Rs_from_adjacent_list(GlobalC::ucell, GlobalC::GridD, *lm.ParaV));
                // this->symrot_.test_HR_rotation(GlobalC::ucell.symm, GlobalC::ucell.atoms, GlobalC::ucell.st, 'H', *(dynamic_cast<hamilt::HamiltLCAO<T, double>*>(&hamilt)->getHR()));
                // exit(0);

            if (this->exx_spacegroup_symmetry && GlobalC::exx_info.info_global.exx_symmetry_realspace)
            {
                this->exx_ptr->cal_exx_elec(Ds, *dm.get_paraV_pointer(), &this->symrot_);
                // this->symrot_.print_HR(this->exx_ptr->Hexxs[0], "Hexxs_irreducible");   // test
                // this->symrot_.print_HR(this->exx_ptr->Hexxs[0], "Hexxs_restored", 1e-10);   // test
            }
            else
            {
                this->exx_ptr->cal_exx_elec(Ds, *dm.get_paraV_pointer());    // restore DM but not Hexx
                // this->symrot_.print_HR(this->exx_ptr->Hexxs[0], "Hexxs_restore-DM-only");   // test
                // this->symrot_.print_HR(this->exx_ptr->Hexxs[0], "Hexxs_ref");   // test
            }
                // ========================  test   ========================
                // if (this->two_level_step)exit(0);
                // check the rotation of S(R)
                // this->symrot_.find_irreducible_sector(GlobalC::ucell.symm, GlobalC::ucell.atoms, GlobalC::ucell.st, this->symrot_.get_Rs_from_adjacent_list(GlobalC::ucell, GlobalC::GridD, *lm.ParaV));
                // this->symrot_.test_HR_rotation(GlobalC::ucell.symm, GlobalC::ucell.atoms, GlobalC::ucell.st, 'H', *(dynamic_cast<hamilt::HamiltLCAO<T, double>*>(&hamilt)->getSR()));

                // check the rotation of D(R): no atom pair?
                // symrot_.find_irreducible_sector(GlobalC::ucell.symm, GlobalC::ucell.atoms, GlobalC::ucell.st, symrot_.get_Rs_from_adjacent_list(GlobalC::ucell, GlobalC::GridD, *this->DM->get_paraV_pointer()));
                // symrot_.test_HR_rotation(GlobalC::ucell.symm, GlobalC::ucell.atoms, GlobalC::ucell.st, 'D', *(this->DM->get_DMR_pointer(0)));

                // check the rotation of Hexx
                // this->symrot_.test_HR_rotation(GlobalC::ucell.symm, GlobalC::ucell.atoms, GlobalC::ucell.st, 'H', this->exx_ptr->Hexxs[0]);
                // exit(0);// break after test
                // ========================  test   ========================
                iter = 0;
                this->two_level_step++;

                timeval t_end;       gettimeofday(&t_end, nullptr);
                std::cout << "and rerun SCF\t"
                    << std::setprecision(3) << std::setiosflags(std::ios::scientific)
                    << (double)(t_end.tv_sec-t_start.tv_sec) + (double)(t_end.tv_usec-t_start.tv_usec)/1000000.0 
                    << std::defaultfloat << " (s)" << std::endl;
            return false;
            }
        }
    restart_reset();
    return true;
}
#endif