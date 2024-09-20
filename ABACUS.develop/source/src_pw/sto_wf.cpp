#include "sto_wf.h"
#include "global.h"


Stochastic_WF::Stochastic_WF()
{
    chiortho  = new ComplexMatrix[1];
    chi0  = new ComplexMatrix[1];
}

Stochastic_WF::~Stochastic_WF()
{ 
    delete[] chiortho;
    delete[] chi0;
}

void Stochastic_WF::init()
{
    //wait for init
    int nrxx = pw.nrxx;
    int nx = pw.nx,ny = pw.ny,nz = pw.nz;

    //distribute nchi for each process
    nchip = int(nchi/NPROC_IN_POOL);
    if(RANK_IN_POOL < nchi%NPROC_IN_POOL) ++nchip;

    complex<double> ui(0,1);
    
    delete[] chi0;
    //We temporarily init one group of orbitals for all k points.
    //This save memories.
    chi0 = new ComplexMatrix[1]; 
    chi0[0].create(nchip,nrxx,false);
    //init with random number
    
    srand((unsigned)time(NULL));
    for(int i=0; i<chi0[0].size; ++i)
    {
        chi0[0].c[i]=exp(2*PI*rand()/double(RAND_MAX)*ui) / sqrt(double(nchi));
    }
    

    delete[] chiortho;
    int nkk = 1; // We temporarily use gamma k point.
    chiortho = new ComplexMatrix[1];
    if(NBANDS > 0)
    {
        chiortho[0].create(nchip,nrxx,false);
    }
    return;
}


