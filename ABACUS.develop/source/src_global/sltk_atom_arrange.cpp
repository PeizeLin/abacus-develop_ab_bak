#include "sltk_atom_arrange.h"
#include "sltk_atom_input.h"
#include "sltk_grid.h"
#include "sltk_grid_driver.h"

// update the followig two includes in near future 
//#include "../src_pw/global.h"
//#include "../src_lcao/global_fp.h" // mohan add 2021-01-30
#include "../src_pw/unitcell.h"

atom_arrange::atom_arrange()
{
}

atom_arrange::~atom_arrange()
{
}

double atom_arrange::set_sr_NL(const double &rcutmax_Phi, const double &rcutmax_Beta, const bool gamma_only_local)
{
	TITLE("atom_arrange","set_sr_NL");

	if(OUT_LEVEL != "m") //xiaohui add 'OUT_LEVEL', 2015-09-16
	{
		ofs_running << "\n\n\n\n";
		ofs_running << " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
		ofs_running << " |                                                                    |" << endl;
		ofs_running << " | Search adjacent atoms:                                             |" << endl;
		ofs_running << " | Set the adjacent atoms for each atom and set the periodic boundary |" << endl;
		ofs_running << " | condition for the atoms on real space FFT grid. For k-dependent    |" << endl;  
		ofs_running << " | algorithm, we also need to set the sparse H and S matrix element   |" << endl;
		ofs_running << " | for each atom.                                                     |" << endl; 
		ofs_running << " |                                                                    |" << endl;
		ofs_running << " <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
		ofs_running << "\n\n\n\n";
	}

	
	//xiaohui add 'OUT_LEVEL' line, 2015-09-16
	if(OUT_LEVEL != "m") ofs_running << "\n SETUP SEARCHING RADIUS FOR PROGRAM TO SEARCH ADJACENT ATOMS" << endl;
	if(OUT_LEVEL != "m") ofs_running << setprecision(3);
	if(OUT_LEVEL != "m") OUT(ofs_running,"longest orb rcut (Bohr)",rcutmax_Phi);

//	cout << " LONGEST NL PROJ RCUT : " << longest_nl_proj_rcut << endl;
	if(OUT_LEVEL != "m") OUT(ofs_running,"longest nonlocal projector rcut (Bohr)", rcutmax_Beta);

	// check in use_overlap_matrix, 
	double sr = 0.0;
	if(gamma_only_local)
	{
		sr = 2 * rcutmax_Phi + 0.01;
	}
	else
	{
		sr = 2 * (rcutmax_Phi +rcutmax_Beta) + 0.01; // 0.01 is added to make safe.
		//sr = 2 * longest_orb_rcut + 0.01;
	}

	// if use build_Nonlocal_mu (not GAMMA_ONLY_LOCAL) use 2*longest_orb_rcut
	// if use build_Nonlocal_beta ( K-point used ) use 2 * (longest_orb_rcut + longest_nl_proj_rcut) 
	return sr;			
//	cout << " SEARCH RADIUS (BOHR) : " << SEARCH_RADIUS << endl;
//	OUT(ofs_running,"search radius (Bohr)", SEARCH_RADIUS);
}
/*
// mohan update 2011-03-10
void atom_arrange::set_sr_OV(void)
{
	TITLE("atom_arrange","set_sr_OV");
	double longest_orb_rcut = 0.0;
	for(int it=0; it<ucell.ntype; it++)
	{
		longest_orb_rcut = std::max(longest_orb_rcut, ORB.Phi[it].getRcut() );	
	}
//	cout << " LONGEST ORB RCUT     : " << longest_orb_rcut << endl;
	OUT(ofs_running,"longest orb rcut (Bohr)",longest_orb_rcut);
	double sr = 2 * longest_orb_rcut + 0.01;
	// if use build_Nonlocal_mu (not GAMMA_ONLY_LOCAL) use 2*longest_orb_rcut
	// if use build_Nonlocal_beta ( K-point used ) use 2 * (longest_orb_rcut + longest_nl_proj_rcut) 
	SEARCH_RADIUS = sr;			
//	cout << " SEARCH RADIUS (BOHR) : " << SEARCH_RADIUS << endl;
	return;
}
*/
void atom_arrange::search(Grid_Driver &grid_d, const UnitCell &ucell, const double &search_radius_bohr, const int &test_atom_in)
{
	TITLE("atom_arrange", "search");
	timer::tick("atom_arrange","search");

	assert( search_radius_bohr > 0.0 );

//	OUT(ofs_running,"Atom coordinates reading from",global_atom_card);
//	OUT(ofs_running,"The coordinate type",ucell.Coordinate);
//	OUT(ofs_running,"Use cartesian(unit:lat0) coordinate","TRUE");
	if(OUT_LEVEL != "m") OUT(ofs_running,"searching radius is (Bohr))", search_radius_bohr);
	if(OUT_LEVEL != "m") OUT(ofs_running,"searching radius unit is (Bohr))",ucell.lat0);

	assert(ucell.nat > 0);
	//=============================
	// Initial Atom information
	//=============================

	const double radius_lat0unit = search_radius_bohr / ucell.lat0;

	Atom_input at(ucell, ucell.nat, ucell.ntype, SEARCH_PBC, radius_lat0unit, test_atom_in);
	//===========================================
	// Print important information in Atom_input
	//===========================================
//	at.print(cout);
//	at.print_xyz_format("1.xyz");
	//=========================================
	// Construct Grid , Cells , Adjacent atoms
	//=========================================
	grid_d.init(ucell, at);

	// test the adjacent atoms and the box.
	//ofs_running << " " << setw(5) << "Type" << setw(5) << "Atom" << setw(8) << "AdjNum" << endl;
	for (int it = 0;it < ucell.ntype;it++)
	{
		for (int ia = 0;ia < ucell.atoms[it].na;ia++)
		{
	//		grid_d.Find_atom(ucell.atoms[it].tau[ia]);
			
	//		ofs_running << " " << setw(5) << it << setw(5) << ia << setw(8) << grid_d.getAdjacentNum()+1 << endl;
			/*
			for(int ad=0; ad < grid_d.getAdjacentNum()+1; ad++)
			{
				Vector3<double> tau = grid_d.getAdjacentTau(ad);
				Vector3<int> box = grid_d.getBox(ad);
				cout << setw(8) << tau.x << setw(8) << tau.y << setw(8) << tau.z 
				<< setw(8) << box.x << setw(8) << box.y << setw(8) << box.z << endl;
			}
			*/
		}
	}
	
	timer::tick("atom_arrange","search");
	return;
}


//2015-05-07
void atom_arrange::delete_vector(Grid_Driver &grid_d, const UnitCell &ucell, const double &search_radius_bohr, const int &test_atom_in)
{
	const double radius_lat0unit2 = search_radius_bohr / ucell.lat0;

	Atom_input at2(ucell, ucell.nat, ucell.ntype, SEARCH_PBC, radius_lat0unit2, test_atom_in);

	grid_d.delete_vector(at2);

	if (grid_d.init_cell_flag)
	{
		for (int i = 0;i < grid_d.dx;i++)
		{
			for (int j = 0;j < grid_d.dy;j++)
			{
				delete[] grid_d.Cell[i][j];
			}
		}

		for (int i = 0;i < grid_d.dx;i++)
		{
			delete[] grid_d.Cell[i];
		}

		delete[] grid_d.Cell;
	}
}
