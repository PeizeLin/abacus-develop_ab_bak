/**********************************************************************
  Mulliken_Charge.cpp:

     Mulliken_Charge.cpp is a subrutine to calculate Mulliken charge.
 
  Log of Mulliken_Charge.cpp:

     12/Oct/2018  Released by Feng Qi

***********************************************************************/

//#include "../src_pw/global.h"
#include "Mulliken_Charge.h"
#include "../src_lcao/local_orbital_charge.h"
//#include "../src_lcao/sltk_grid_driver.h"
#include "use_overlap_matrix.h"
#include "../src_pw/global.h"
#include "../src_pw/wavefunc.h"
#include "../src_lcao/lcao_matrix.h"
#include "global_fp.h"
//#include "../src_develop/src_wannier/manipulation.h"
//#include "lcao_nnr.h"
#include "lcao_matrix.h"
#include "../src_lcao/global_fp.h"
#include "../src_lcao/wfc_dm_2d.h"
#include "../src_global/lapack_connector.h"
#include "../src_global/scalapack_connector.h"
#include "../src_global/matrix.h"
#include "../src_global/complexmatrix.h"
#include <vector>
#include <mpi.h>
#include "../src_lcao/sltk_atom_arrange.h"
#include "../src_lcao/lcao_nnr.h"




Mulliken_Charge::Mulliken_Charge()
{
                        


                Mullik.resize(NSPIN);
                                Mulliken.resize(kv.nks);
                DecMulP = new double* [NSPIN];
		for(int is=0; is<NSPIN; ++is)
		{
			DecMulP[is] = new double[NLOCAL];
			ZEROS(DecMulP[is], NLOCAL);
		}
                                MecMulP = new double* [NSPIN];
		for(int is=0; is<NSPIN; ++is)
		{
			MecMulP[is] = new double[NLOCAL];
			ZEROS(MecMulP[is], NLOCAL);
		}

	
	ADecMulP = new double**[NSPIN];
		for(int is=0; is<NSPIN; is++)
		{
			ADecMulP[is] = new double*[ucell.nat];

			for (int i=0; i<ucell.nat; i++)
			{
				ADecMulP[is][i] = new double[(2*ucell.lmax+1)*(2*ucell.lmax+1)*ucell.nmax];
				ZEROS(ADecMulP[is][i], (2*ucell.lmax+1)*(2*ucell.lmax+1)*ucell.nmax);
			}
        }
}
	
Mulliken_Charge::~Mulliken_Charge()
{  


       for(int is=0; is<NSPIN; ++is)
		{
			delete[] DecMulP[is];
		}	
		delete[] DecMulP;
                   for(int is=0; is<NSPIN; ++is)
		{
			delete[] MecMulP[is];
		}	
		delete[] MecMulP;
                    
              for (int is=0; is<NSPIN; is++)
	  {
		for (int i=0; i<ucell.nat; i++)
		{
			delete[] ADecMulP[is][i]; 
		}
		delete[] ADecMulP[is];
	  }
	  delete[] ADecMulP;

}

  
void Mulliken_Charge::cal_mulliken(void)
{
	TITLE("Mulliken_Charge","cal_mulliken");
	 // if(GAMMA_ONLY_LOCAL)
         // {    
              for(int is=0; is!=NSPIN; ++is)
             {
                                if(GAMMA_ONLY_LOCAL)
	                {
                Mullik[is].create(ParaO.ncol,ParaO.nrow);
                     const double one_float=1.0, zero_float=0.0;
		     const int one_int=1;
		     const char N_char='N', T_char='T';    
                                     pdgemm_(
				&N_char, &T_char,
				&NLOCAL, &NLOCAL, &NLOCAL,
				&one_float,
				LOC.wfc_dm_2d.dm_gamma[is].c, &one_int, &one_int, ParaO.desc,
				LM.Sloc, &one_int, &one_int, ParaO.desc,
				&zero_float,
				Mullik[is].c, &one_int, &one_int, ParaO.desc);
                                                               // DecMulP[is](NLOCAL,0.0);
                              for (int i=0; i<NLOCAL; ++i)
                             {
                                   if ( ParaO.in_this_processor(i,i) )
                                  {
                                         const int ir = ParaO.trace_loc_row[i];
                                         const int ic = ParaO.trace_loc_col[i];
                                         MecMulP[is][i] = Mullik[is] (ic,ir ); 
                                 }
	                     }
                      
                                             // MPI_Reduce(MecMulP[is], DecMulP[is] , NLOCAL , MPI_DOUBLE , MPI_SUM, 0, MPI_COMM_WORLD);
                                            }//if
                                                         else
                                                        {
                                                                                                                                           hm.hon.set_orb_tables(); 
                                                                      atom_arrange::set_sr_NL();
		                                     atom_arrange::search( SEARCH_RADIUS );
                                                                      LM.allocate_HS_R(LNNR.nnr);
		                                       LM.zeros_HSR('S', LNNR.nnr);
                                                                      UHM.UOM.calculate_S_no();
		                                       UHM.UOM.build_ST_new('S', false);

                                                                 std::vector<ComplexMatrix> dmk;
                                                                 dmk.resize(kv.nks);
                                                               //   std::vector<ComplexMatrix> PDOS_k;
                                                                //  PDOS_k.resize(kv.nks);
                                       for(int ik=0;ik<kv.nks;ik++)
		      {
			if(is == kv.isk[ik])
			{
				LM.allocate_HS_k(ParaO.nloc);
				LM.zeros_HSk('S');
				LNNR.folding_fixedH(ik);
 

                                                                std::vector<double> wg_local(ParaO.ncol,0.0);
			for(int ib_global=0; ib_global!=NLOCAL; ++ib_global)
			{
				const int ib_local = ParaO.trace_loc_col[ib_global];
				if(ib_local>=0)
					wg_local[ib_local] = wf.wg(ik,ib_global);
			}
			
			// wg_wfc(ib,iw) = wg[ib] * wfc(ib,iw);
			//matrix wg_wfc(wfc_gamma[is]);
                                                ComplexMatrix wg_wfck = LOC.wfc_dm_2d.wfc_k[ik];

			for(int ir=0; ir!=wg_wfck.nr; ++ir)
				LapackConnector::scal( wg_wfck.nc, wg_local[ir], wg_wfck.c+ir*wg_wfck.nc, 1 );
			
			// C++: dm(iw1,iw2) = wfc(ib,iw1) * wg_wfc(ib,iw2)
			const double one_float=1.0, zero_float=0.0;
			const int one_int=1;
			const char N_char='N',C_char='C';
			dmk[ik].create(LOC.wfc_dm_2d.wfc_k[ik].nr,LOC.wfc_dm_2d.wfc_k[ik].nc);
                                                Mulliken[ik].create(ParaO.ncol,ParaO.nrow);
			pzgemm_(
				&N_char, &C_char,
				&NLOCAL, &NLOCAL, &NLOCAL,
				&one_float,
				wg_wfck.c, &one_int, &one_int, ParaO.desc,
				LOC.wfc_dm_2d.wfc_k[ik].c, &one_int, &one_int, ParaO.desc,
				&zero_float,
				dmk[ik].c, &one_int, &one_int, ParaO.desc); 
                                               pzgemm_(
				&N_char, &N_char,
				&NLOCAL, &NLOCAL, &NLOCAL,
				&one_float,
				dmk[ik].c, &one_int, &one_int, ParaO.desc,
				LM.Sloc2, &one_int, &one_int, ParaO.desc,
				&zero_float,
				 Mulliken[ik].c, &one_int, &one_int, ParaO.desc);  
                                            for (int j=0; j<NLOCAL; ++j)
                                         {
                                                    if ( ParaO.in_this_processor(j,j) )
                                                  {
                                                                complex<double> h  = 0.0;
                                                                const int ir = ParaO.trace_loc_row[j];
                                                                const int ic = ParaO.trace_loc_col[j];
                                                                h =  Mulliken[ik] (ic,ir ); 
                                                             // MecMulP[is][j] += kv.wk[ik]*h.real();
                                                              MecMulP[is][j] += h.real();
                                                  }
	                         }
                                    }//if
                                              }//k
                                             #ifdef __MPI
	                       atom_arrange::delete_vector( SEARCH_RADIUS );
                                            #endif
	                      hm.hon.clear_after_ions();

                                      }//else
                                      
                                        MPI_Reduce(MecMulP[is], DecMulP[is] , NLOCAL , MPI_DOUBLE , MPI_SUM, 0, MPI_COMM_WORLD);
                                      
         if(MY_RANK == 0)
        {
               for (int i=0; i<ucell.nat; i++)
	  {   
                         int a = ucell.iat2ia[i];
		int t = ucell.iat2it[i];
		Atom* atom1 = &ucell.atoms[t];
	    for(int j=0; j<atom1->nw; ++j)
	    {
		    int k = ucell.itiaiw2iwt(t,a,j);
			ADecMulP[is][i][j] = DecMulP[is][k];
                                               
	    }
	  }
                   
       }
				
     }//is
    
                									
}				   

void Mulliken_Charge::stdout_mulliken(void)
{                    this->cal_mulliken();
    if(MY_RANK == 0)
    {

           TITLE("Dos","calculate_Mulliken");
	 ofstream fout;
	 const char * fn= "mulliken.txt";
     fout.open(fn);
                                                // ofstream fout;
                                       // string wordqf="mulliken.txt";
                                       // wordqf += char(MY_RANK + 48);
                                    //  wordqf += ".txt";
                                    //   fout.open(wordqf.c_str(),ios::app);     
	 int num,l,m,mul;
                  double Tcharge;
                  double*   sum_l = new double[2];
                   double*   sum_mul = new double[2];
            

	 string Name_Angular[5][11];
	 /* decomposed Mulliken charge */

      Name_Angular[0][0] = "s          ";
      Name_Angular[1][0] = "px         ";
      Name_Angular[1][1] = "py         ";
      Name_Angular[1][2] = "pz         ";
      Name_Angular[2][0] = "d3z^2-r^2  ";
      Name_Angular[2][1] = "dx^2-y^2   ";
      Name_Angular[2][2] = "dxy        ";
      Name_Angular[2][3] = "dxz        ";
      Name_Angular[2][4] = "dyz        ";
      Name_Angular[3][0] = "f5z^2-3r^2 ";
      Name_Angular[3][1] = "f5xz^2-xr^2";
      Name_Angular[3][2] = "f5yz^2-yr^2";
      Name_Angular[3][3] = "fzx^2-zy^2 ";
      Name_Angular[3][4] = "fxyz       ";
      Name_Angular[3][5] = "fx^3-3*xy^2";
      Name_Angular[3][6] = "f3yx^2-y^3 ";
      Name_Angular[4][0] = "g1         ";
      Name_Angular[4][1] = "g2         ";
      Name_Angular[4][2] = "g3         ";
      Name_Angular[4][3] = "g4         ";
      Name_Angular[4][4] = "g5         ";
      Name_Angular[4][5] = "g6         ";
      Name_Angular[4][6] = "g7         ";
      Name_Angular[4][7] = "g8         ";
      Name_Angular[4][8] = "g9         ";
		fout << "\n CALCULATE THE MULLIkEN ANALYSIS FOR EACH ATOM" << endl;
		// calculate the total charge of the system.
			double sch = 0.0;
			fout << setprecision(8);
			for(int is=0; is<NSPIN; ++is)
			{
				double sss = 0.0;
				for(int iw=0; iw<NLOCAL; ++iw)
				{
					sch += DecMulP[is][iw];
					sss += DecMulP[is][iw];
                                                                            
				              
				}
				fout << sss << " (Total charge all spin " << is+1 << ")" << endl;
			}
			fout << sch << " (Total charge of the system)" << endl;
                                               	fout << "  Decomposed Mulliken populations" << endl;
                               
		/*for (int i=0; i<ucell.nat; i++)
		{
			int   t = ucell.iat2it[i];
			Atom* atom1 = &ucell.atoms[t];  
			fout << i << "         " << ucell.atoms[t].label << "			" << "Up spin" << "                    " << "Down spin" << endl;
			fout << "            " << "multiple" << endl;
			//num = 0;
		  for(int j=0; j<atom1->nw; ++j)
	      {
			const int L1 = atom1->iw2l[j];
			const int N1 = atom1->iw2n[j];
			const int m1 = atom1->iw2m[j];
			fout << Name_Angular[L1][m1] << "      " << N1 << "          " << ADecMulP[0][i][j] << "               " << ADecMulP[0][i][j] << endl;
		  }
		}*/
                                      for (int i=0; i<ucell.nat; i++)
                                      {
                                                 Tcharge = 0.0;
                                                 int t = ucell.iat2it[i];
	                          if (NSPIN==1 || NSPIN==2){
                                           fout << i << setw(25) << ucell.atoms[t].label << setw(30) << "Up spin" << setw(30) << "Down spin" << setw(30) << "Sum"<< setw(30) << "Diff"<< endl;
                                                                                          }
	  // else{
	//   fout << i << "         " << ucell.atoms[t].label << "       " << "Up spin" << "                    " << "Down spin" << "                    " << "Sum"<< "                    " << "Diff"<< "                    " << "Angles(Deg)"<<endl;
	   fout << setw(29) << "multiple" <<endl;
	 
                                                       num = 0;

	                                      for (l=0; l<=ucell.lmax; l++)
                                                      {             
                                                               if (NSPIN==1){
	                                              sum_l[0] = 0.0;
	                                                                       }
	                                                else if (NSPIN==2){
	                                                sum_l[0] = 0.0;
	                                                sum_l[1] = 0.0;
	                                                                                 }
	                                              /* else if (NSPIN==3){
	                                                  sum_l[0] = 0.0;
	                                                  sum_l[1] = 0.0;
	                                                 }*/
	                       	            for (mul=0; mul<ucell.atoms[t].l_nchi[l]; mul++)
                                                           {                 
                                                                            if (NSPIN==1){
	                                                          sum_mul[0] = 0.0;
	                                                                                    }
	                                                          else if (NSPIN==2){
	                                                           sum_mul[0] = 0.0;
	                                                           sum_mul[1] = 0.0;
	                                                                                           }
	                                                           /* else if (SpinP_switch==3){
	                                                               sum_mul[0] = 0.0;
	                                                                sum_mul[1] = 0.0;
	                                                                  }*/
	                                	          for (m=0; m<(2*l+1); m++)
                                                                         {     
	                                                                if (NSPIN==1){
                                                                                                                    ADecMulP[0][i][num] = 0.5*ADecMulP[0][i][num]; 
                                                          fout << Name_Angular[l][m] << setw(14) << mul << setw(32)<< ADecMulP[0][i][num] << setw(30)<< ADecMulP[0][i][num] << setw(30)<< ADecMulP[0][i][num]+ ADecMulP[0][i][num]<< setw(28) << ADecMulP[0][i][num]-ADecMulP[0][i][num]<< endl;
		                        sum_mul[0] += ADecMulP[0][i][num];
                                                                                                        }
                                                                           
		                         else if (NSPIN==2){
                                                        

		                       fout << Name_Angular[l][m] << setw(14) << mul << setw(32) << ADecMulP[0][i][num] << setw(30) << ADecMulP[1][i][num] << setw(30) << ADecMulP[0][i][num]+ ADecMulP[1][i][num]<< setw(28) << ADecMulP[0][i][num]-ADecMulP[1][i][num]<< endl;
		                       sum_mul[0] += ADecMulP[0][i][num];
		                      sum_mul[1] += ADecMulP[1][i][num];
		                                                            }
                                                                                 num++;

		                                               

	                                                       }
                                                           
		                           if (NSPIN==1){
		                          fout <<"  sum over m "<< setw(43) <<sum_mul[0]<< setw(30) <<sum_mul[0]<< setw(35) <<sum_mul[0]+sum_mul[0]<< setw(25) <<sum_mul[0]-sum_mul[0]<<endl;
		                          sum_l[0] += sum_mul[0];
	                                                                   }
		                           else if (NSPIN==2){
		                            fout <<"  sum over m "<< setw(43) <<sum_mul[0]<< setw(30) <<sum_mul[0]<< setw(35) <<sum_mul[0]+sum_mul[1]<< setw(25) <<sum_mul[0]-sum_mul[1]<<endl;
		                            sum_l[0] += sum_mul[0];
	                                            sum_l[1] += sum_mul[1];
	                                                                           }
                                                    
                                            }
                                                     
		if (ucell.atoms[t].l_nchi[l]!=0){
		if (NSPIN==1){
		fout <<"  sum over m+mul "<< setw(36) <<sum_l[0]<< setw(30) <<sum_l[0]<< setw(33) <<sum_l[0]+sum_l[0]<< setw(29) <<sum_l[0]-sum_l[0]<<endl;
                                                       Tcharge =  Tcharge+ sum_l[0]+sum_l[0];
		                         }
		else if (NSPIN==2){
		fout <<"  sum over m+mul "<< setw(36) <<sum_l[0]<< setw(30) <<sum_l[1]<< setw(33) <<sum_l[0]+sum_l[1]<< setw(29) <<sum_l[0]-sum_l[1]<<endl;
                                                       Tcharge =  Tcharge+sum_l[0]+sum_l[1];
		                                }
		                                                  }
                                          
                                          
                             
	      }
                             fout <<"Total Charge on atom  "<< ucell.atoms[t].label <<  setw(20) << Tcharge <<endl<<endl<<endl;
                }
                      fout.close();
                    delete[] sum_l;
                 delete[] sum_mul;
	  }
}
			 


