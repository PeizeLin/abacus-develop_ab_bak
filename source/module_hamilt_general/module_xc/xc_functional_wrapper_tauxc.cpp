// This file contains wrapper for the mGGA functionals
// it includes 1 subroutine:
// 1. tau_xc

#include "xc_functional.h"
#include "module_hamilt_pw/hamilt_pwdft/global.h"

//tau_xc and tau_xc_spin: interface for calling xc_mgga_exc_vxc from LIBXC
//XC_POLARIZED, XC_UNPOLARIZED: internal flags used in LIBXC, denote the polarized(nspin=1) or unpolarized(nspin=2) calculations, definition can be found in xc.h from LIBXC
#ifdef USE_LIBXC
void XC_Functional::tau_xc(const double &rho, const double &grho, const double &atau, double &sxc,
          double &v1xc, double &v2xc, double &v3xc)
{
    double s, v1, v2, v3;
	double lapl_rho, vlapl_rho;
    lapl_rho = grho;
    std::vector<xc_func_type> funcs = init_func(XC_UNPOLARIZED);
    
    sxc = 0.0; v1xc = 0.0; v2xc = 0.0; v3xc = 0.0;

    for(xc_func_type &func : funcs)
    {
        xc_mgga_exc_vxc(&func,1,&rho,&grho,&lapl_rho,&atau,&s,&v1,&v2,&vlapl_rho,&v3);
#ifdef __EXX
        if (func.info->number == XC_MGGA_X_SCAN && get_func_type() == 5)
        {
            s *= (1.0 - GlobalC::exx_info.info_global.hybrid_alpha);
            v1 *= (1.0 - GlobalC::exx_info.info_global.hybrid_alpha);
            v2 *= (1.0 - GlobalC::exx_info.info_global.hybrid_alpha);
            v3 *= (1.0 - GlobalC::exx_info.info_global.hybrid_alpha);
        }
#endif
        sxc += s * rho;
        v2xc += v2 * 2.0;
        v1xc += v1;
        v3xc += v3;
    }
    finish_func(funcs);

	return;
}

#endif