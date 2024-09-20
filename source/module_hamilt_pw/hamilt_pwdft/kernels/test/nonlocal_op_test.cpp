#include <vector>
#include <complex>
#include <iostream>
#include <gtest/gtest.h>
#include "module_psi/kernels/memory_op.h"
#include "module_hamilt_pw/hamilt_pwdft/kernels/nonlocal_op.h"

class TestModuleHamiltNonlocal : public ::testing::Test
{
  protected:
    // xx = tf.random.uniform([100], minval=-4, maxval=4, dtype = tf.float64)
    const int l1 = 2;
    const int l2 = 8;
    const int l2_spin = 56;
    const int l3 = 4;

    int sum = 0, expected_sum = 8;
    int iat = 0, expected_iat = 2;

    const int spin = 0;
    const int nkb = 8;
    const int deeq_x = 2;
    const int deeq_y = 4;
    const int deeq_z = 4;

    std::vector<double> deeq = {
      1.52389, 0, 0, 0, 0, 3.6833, 0, 0, 0, 0, 3.6833, 0, 0, 0, 0, 3.6833, 1.52389, 0, 0, 0, 0, 3.6833, 0, 0, 0, 0, 3.6833, 0, 0, 0, 0, 3.6833
    };

    std::vector<std::complex<double>> deeq_spin = {
      {1.52389, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {3.6833, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {3.6833, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {3.6833, 0}, {1.52389, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {3.6833, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {3.6833, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {3.6833, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {1.52389, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {3.6833, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {3.6833, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {3.6833, 0}, {1.52389, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {3.6833, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {3.6833, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {3.6833, 0}
    };

    std::vector<std::complex<double>> expected_ps = {
      {1.08811,0}, {0,2.11482e-17}, {0,4.22963e-17}, {0,-6.34445e-17}, {0.473301,2.11482e-17}, {-3.80667e-16,-5.28704e-18}, {-2.11482e-17,-2.11482e-17}, {4.22963e-17,2.64352e-17}, {1.50743e-33,1.75712e-17}, {0.93285,-5.71206e-17}, {0,-2.83751e-33}, {-3.83371e-17,1.41876e-33}, 
      {6.38951e-18,2.79541e-18}, {-0.0996506,1.91685e-17}, {1.2779e-17,5.11161e-17}, {2.87528e-17,3.83371e-17}, {-2.48283e-33,7.02847e-17}, {1.2779e-17,-2.83751e-33}, {0.93285,-5.71206e-17}, {-1.02232e-16,3.54689e-34}, {0,6.38951e-18}, {9.58427e-18,5.11161e-17}, {-0.0996506,-6.38951e-18}, 
      {-7.83714e-18,0}, {1.41876e-33,1.24596e-16}, {-5.11161e-17,1.41876e-33}, {-1.91685e-17,3.54689e-34}, {0.93285,-5.71206e-17}, {0,5.09164e-18}, {2.87528e-17,2.55581e-17}, {-7.78722e-18,0}, {-0.0996506,6.38951e-18}, {0.473301,-2.11482e-17}, {3.80667e-16,-5.28704e-18}, 
      {2.11482e-17,-2.11482e-17}, {-4.22963e-17,2.64352e-17}, {1.08811,-6.92094e-34}, {-2.95657e-33,-2.11482e-17}, {-1.31476e-33,4.22963e-17}, {3.35836e-34,1.26889e-16}, {-6.38951e-18,-3.99345e-18}, {-0.0996506,6.38951e-18}, {1.2779e-17,-4.47266e-17}, {2.87528e-17,-4.47266e-17}, 
      {-5.32034e-34,-6.38951e-18}, {0.93285,-5.71206e-17}, {-1.2779e-17,1.41876e-33}, {2.55581e-17,-5.67503e-33}, {0,-6.38951e-18}, {1.2779e-17,-4.47266e-17}, {-0.0996506,1.91685e-17}, {9.98362e-19,0}, {1.41876e-33,-1.91685e-17}, {0,-1.41876e-33}, {0.93285,-5.71206e-17}, 
      {1.02232e-16,7.80316e-33}, {-6.38951e-18,-3.5941e-18}, {2.87528e-17,-1.2779e-17}, {-5.34123e-18,0}, {-0.0996506,0}, {0,-3.51423e-17}, {1.2779e-17,-4.96565e-33}, {8.30637e-17,-2.12814e-33}, {0.93285,-5.71206e-17}
    };

    std::vector<std::complex<double>> expected_ps_spin = {
      {0.898349, 0.898349}, {0, 0}, {5.5008e-17, 5.5008e-17}, {0.898349, 0.898349}, {-1.32417e-17, -2.0787e-17}, {0, 0}, {-4.47914e-17, -1.64852e-16}, {0, 0}, {-8.89752e-17, -1.23726e-16}, {0, 0}, {7.61637e-34, 3.27494e-34}, {-2.0787e-17, -1.32417e-17}, {-5.0317e-34, -4.81996e-33}, {-1.64852e-16, -4.47914e-17}, {-3.77607e-33, -2.16796e-33}, {-1.23726e-16, -8.89752e-17}, {0.333942, 0.333942}, {0, 0}, {2.04481e-17, 2.04481e-17}, {0.333942, 0.333942}, {3.56019e-17, 2.80084e-17}, {0, 0}, {-2.01942e-17, -1.04257e-17}, {0, 0}, {-4.20997e-17, 5.08557e-17}, {0, 0}, {3.512e-34, -2.09989e-33}, {2.14478e-17, 1.38544e-17}, {-1.69364e-34, 1.698e-33}, {-1.8653e-17, 6.97665e-18}, {3.70341e-34, 3.231e-33}, {-1.70411e-17, 5.44357e-17}, {-0.230744, 0.302671}, {0.622165, -0.255728}, {-0.21374, -0.0616951}, {-0.316678, -0.0312102}, {-0.237357, 0.643759}, {-0.0531013, 0.468}, {0.415893, 0.246569}, {-0.401426, -0.0965346}, {-0.44529, 0.114999}, {0.250482, 0.102919}, {0.440536, 0.449415}, {0.0284661, -0.109981}, {0.523275, -0.247933}, {-0.184478, 0.126704}, {-0.216067, -0.143467}, {0.353863, 0.348662}, {-0.328859, 0.308482}, {-0.0164594, -0.196908}, {-0.340233, 0.108988}, {-0.0564905, -0.243882}, {0.599485, 0.263851}, {0.250563, -0.1169}, {-0.114713, 0.914133}, {-0.0257248, 0.332045}, {1.34207e-18, -1.62305e-18}, {0, 0}, {7.6548e-35, 8.51989e-35}, {1.62305e-18, -1.34207e-18}, {-0.649674, 0.649674}, {0, 0}, {2.26919e-18, -1.70722e-18}, {0, 0}, {-6.09832e-18, 5.95782e-18}, {0, 0}, {-3.97811e-17, 3.97811e-17}, {-0.649674, 0.649674}, {8.56914e-34, -8.25718e-34}, {1.70722e-18, -2.26919e-18}, {-1.28019e-33, 5.55216e-34}, {-5.95782e-18, 6.09832e-18}, {1.28047e-17, -1.64288e-17}, {0, 0}, {4.02499e-34, 1.25146e-33}, {9.32896e-18, -2.23376e-17}, {0.0703638, -0.0703638}, {0, 0}, {-8.67574e-18, -1.45033e-17}, {0, 0}, {-2.25898e-17, -2.96005e-17}, {0, 0}, {4.30854e-18, -4.30854e-18}, {0.0703638, -0.0703638}, {-2.6325e-33, -1.18255e-33}, {-1.45033e-17, 4.10329e-18}, {-1.8853e-33, 2.16153e-34}, {-9.95127e-18, -2.28708e-17}, {-0.0573837, -0.0643268}, {-0.0371039, 0.0609584}, {0.00401883, -0.0107605}, {-0.202513, 0.151009}, {-0.0564302, 0.148095}, {-0.101392, -0.10415}, {-0.0218668, -0.0620697}, {0.0947394, -0.0684151}, {-0.0768246, -0.231909}, {-0.0514549, -0.0248385}, {-0.133934, -0.124877}, {0.129989, -0.0287355}, {0.0176577, -0.105658}, {0.124485, -0.107696}, {0.00311226, -0.179625}, {-0.0593093, -0.139317}, {0.0948213, 0.187728}, {0.0670226, -0.0727921}, {-0.0541868, -0.073592}, {-0.272545, -0.155377}, {0.0626124, -0.239634}, {-0.0776201, 0.0483137}, {0.29566, 0.075091}, {-0.0679552, -0.0295722}, {-4.75938e-18, 1.66955e-17}, {0, 0}, {-4.57566e-34, 5.60841e-35}, {-1.66955e-17, 4.75938e-18}, {-3.97984e-18, 4.40131e-18}, {0, 0}, {-0.649674, 0.649674}, {0, 0}, {-1.07074e-17, 4.8623e-17}, {0, 0}, {9.19305e-34, -1.2272e-33}, {-4.40131e-18, 3.97984e-18}, {-3.97811e-17, 3.97811e-17}, {-0.649674, 0.649674}, {-1.66811e-33, 1.62131e-33}, {-4.8623e-17, 1.07074e-17}, {-7.11118e-18, 1.63014e-18}, {0, 0}, {1.21261e-34, -2.80222e-34}, {-1.56462e-18, 1.20574e-17}, {-8.81623e-18, -7.55181e-18}, {0, 0}, {0.0703638, -0.0703638}, {0, 0}, {-5.07231e-18, 5.6436e-18}, {0, 0}, {-2.58571e-33, 1.00798e-33}, {-2.00524e-18, -2.07523e-17}, {4.30854e-18, -4.30854e-18}, {0.0703638, -0.0703638}, {-2.52107e-35, 7.26047e-34}, {-5.49379e-18, 1.24546e-17}, {0.00740483, -0.0735311}, {0.138605, -0.0772226}, {0.0747133, -0.0525776}, {0.00746346, -0.125709}, {0.0797641, 0.128052}, {0.0910954, -0.0610522}, {-0.108056, -0.128453}, {-0.0602172, 0.148445}, {0.103672, 0.0627013}, {0.118206, 0.0211509}, {0.097993, -0.125868}, {-0.0183113, -0.0938135}, {-0.11488, -0.0769668}, {-0.040742, 0.0389805}, {0.0618463, 0.170832}, {-0.186072, -0.146774}, {-0.0222281, 0.128044}, {-0.0523003, 0.215975}, {0.196863, -0.101591}, {-0.237618, 0.0389213}, {0.0748492, -0.188815}, {0.023035, 0.131075}, {-0.0338894, 0.192402}, {-0.132448, 0.28203}, {-1.06768e-17, -8.21079e-18}, {0, 0}, {5.32702e-34, 1.49293e-35}, {8.21079e-18, 1.06768e-17}, {-4.6991e-18, 4.55861e-18}, {0, 0}, {-8.94329e-17, 6.33129e-17}, {0, 0}, {-0.649674, 0.649674}, {0, 0}, {-5.57565e-34, -1.67412e-34}, {-4.55861e-18, 4.6991e-18}, {-1.67681e-33, 2.35499e-33}, {-6.33129e-17, 8.94329e-17}, {-3.97811e-17, 3.97811e-17}, {-0.649674, 0.649674}, {-7.763e-18, -4.88627e-18}, {0, 0}, {6.94449e-34, 3.77526e-34}, {-1.0925e-18, -5.36693e-18}, {1.39517e-17, 7.51237e-19}, {0, 0}, {2.17961e-18, 1.3598e-17}, {0, 0}, {0.0703638, -0.0703638}, {0, 0}, {-1.91639e-33, -1.69623e-34}, {1.03222e-18, 8.91728e-19}, {-3.59863e-34, 9.66394e-35}, {1.61764e-18, 7.77043e-18}, {4.30854e-18, -4.30854e-18}, {0.0703638, -0.0703638}, {0.0074578, -0.166046}, {0.148099, 0.142035}, {-0.178783, -0.0819175}, {0.201433, 0.154842}, {0.0254153, -0.10943}, {0.182133, 0.0993425}, {-0.0828282, -0.0432532}, {0.11018, 0.00196916}, {0.197299, -0.0709579}, {-0.222306, -0.00800548}, {-0.0608988, 0.0232307}, {-0.00826633, 0.035842}, {0.0199024, -0.331465}, {-0.170684, -0.00740501}, {-0.0712079, 0.228907}, {-0.167823, -0.0376773}, {-0.181003, 0.103005}, {0.118772, 0.031469}, {-0.00292941, -0.0360347}, {-0.0694756, 0.0819652}, {-0.0408032, -0.0608558}, {-0.0759721, -0.178142}, {-0.271596, 0.0341193}, {0.243051, 0.0773969}, {0.333942, 0.333942}, {0, 0}, {2.04481e-17, 2.04481e-17}, {0.333942, 0.333942}, {3.94022e-17, -2.70229e-17}, {0, 0}, {-7.47752e-19, -7.88598e-18}, {0, 0}, {-5.35976e-18, -8.10023e-18}, {0, 0}, {2.04996e-35, 9.85524e-34}, {4.29822e-17, -4.45911e-17}, {9.72757e-34, -1.77581e-33}, {1.09154e-17, -1.48346e-18}, {-2.1946e-34, -7.3193e-34}, {-3.64781e-18, -1.69721e-17}, {0.898349, 0.898349}, {0, 0}, {5.5008e-17, 5.5008e-17}, {0.898349, 0.898349}, {-1.385e-17, 3.23029e-17}, {0, 0}, {-5.16196e-17, -1.12652e-16}, {0, 0}, {-5.67823e-17, -9.26867e-17}, {0, 0}, {1.44467e-34, 1.41502e-33}, {-2.13952e-17, 2.39871e-17}, {-1.96765e-33, -4.17694e-33}, {-5.93175e-17, -6.26583e-17}, {-4.32457e-33, -4.10933e-33}, {-5.98063e-17, -1.42203e-16}, {-0.304815, 0.337978}, {0.333879, -0.0911541}, {-0.28295, 0.27365}, {-0.699364, 0.37403}, {0.0661855, 0.566246}, {-0.0685443, 0.173105}, {0.492138, -0.419708}, {-0.341971, 0.157495}, {-0.707468, -0.346845}, {0.395835, -0.196645}, {0.492577, -0.069174}, {-0.178373, 0.0119797}, {0.0796147, -0.592672}, {0.0211272, 0.0413917}, {0.428018, -0.047478}, {0.446912, 0.0309616}, {-0.192429, 0.164746}, {-0.076418, -0.259322}, {-0.400357, 0.047729}, {-0.277265, 0.125464}, {0.215864, -0.323063}, {-0.0713515, -0.0583378}, {-0.153737, 0.739445}, {-0.314828, 0.208236}, {2.89386e-17, 1.8813e-17}, {0, 0}, {4.85439e-34, -1.5119e-33}, {2.00347e-17, 2.85157e-17}, {0.0703638, -0.0703638}, {0, 0}, {1.72459e-17, 1.13446e-17}, {0, 0}, {5.38803e-18, 1.3677e-17}, {0, 0}, {4.30854e-18, -4.30854e-18}, {0.0703638, -0.0703638}, {9.27364e-34, 3.63145e-33}, {-7.89612e-18, 3.84426e-17}, {9.65652e-34, 2.85493e-33}, {-6.27252e-18, 2.72935e-17}, {1.40095e-17, 3.27952e-18}, {0, 0}, {2.57815e-34, 3.96825e-34}, {4.50658e-18, 6.35592e-18}, {-0.649674, 0.649674}, {0, 0}, {2.80935e-18, -4.24648e-18}, {0, 0}, {-1.30899e-17, 5.22984e-18}, {0, 0}, {-3.97811e-17, 3.97811e-17}, {-0.649674, 0.649674}, {-6.2223e-34, -2.93226e-34}, {3.22538e-18, -5.42784e-18}, {-6.38403e-35, 4.79563e-34}, {-1.21711e-17, 6.34832e-18}, {-0.0651089, 0.252252}, {-0.0310165, -0.0529542}, {-0.0548229, -0.0228264}, {-0.190507, -0.0450867}, {-0.0369112, 0.17003}, {-0.0976763, -0.12958}, {0.00941216, -0.102069}, {0.0289235, 0.103138}, {-0.134219, -0.0520054}, {0.104241, 0.0056989}, {0.0203017, 0.0600165}, {-0.0797669, 0.163464}, {0.211788, -0.0248343}, {-0.0154356, -0.0570675}, {-0.156286, -0.0379479}, {-0.151619, -0.187369}, {0.0860071, -0.0932265}, {-0.0149229, -0.0309086}, {-0.18319, 0.0817577}, {0.0376425, -0.084288}, {-0.19554, -0.0753742}, {-0.0792463, -0.00476987}, {0.0900378, -0.0243934}, {0.0487987, -0.0351105}, {4.94414e-18, 1.50637e-17}, {0, 0}, {-6.13583e-34, 3.3753e-34}, {-3.94757e-18, 1.08756e-17}, {3.69939e-18, -1.70776e-18}, {0, 0}, {0.0703638, -0.0703638}, {0, 0}, {4.07128e-18, 5.91969e-18}, {0, 0}, {4.65674e-34, 2.74254e-33}, {-2.06828e-17, 2.58692e-17}, {4.30854e-18, -4.30854e-18}, {0.0703638, -0.0703638}, {8.12949e-34, -6.61445e-34}, {3.20013e-19, 1.32547e-18}, {-2.35086e-17, -3.34351e-18}, {0, 0}, {-7.71643e-34, -9.57183e-36}, {-2.25153e-17, -8.7397e-18}, {-3.50471e-18, 2.15065e-18}, {0, 0}, {-0.649674, 0.649674}, {0, 0}, {2.87238e-18, -1.66997e-17}, {0, 0}, {2.66677e-34, -6.34502e-34}, {-2.32881e-18, 1.72918e-18}, {-3.97811e-17, 3.97811e-17}, {-0.649674, 0.649674}, {-1.05771e-33, 2.04402e-33}, {1.66923e-17, -7.67194e-18}, {0.00700547, 0.0796791}, {-0.111909, -0.11059}, {-0.0496604, -0.00280436}, {-0.0206468, -0.243382}, {0.188916, -0.0207421}, {-0.0601878, -0.102819}, {-0.123982, 0.0339939}, {0.132023, -0.169255}, {0.211826, 0.136737}, {-0.136536, -0.125222}, {0.0591417, 0.053948}, {0.131346, -0.0700752}, {0.169414, 0.00539238}, {-0.285759, -0.126328}, {0.180106, -0.0235219}, {0.0155764, -0.137379}, {0.0274904, -0.0186002}, {0.0302117, 0.0196978}, {-0.0955218, 0.0893471}, {0.0694461, -0.175027}, {0.195098, 0.15783}, {0.133154, -0.0101063}, {0.0597049, -0.121845}, {0.103684, 0.0637283}, {-8.81818e-18, 9.28744e-18}, {0, 0}, {-4.87482e-34, 6.61217e-34}, {-6.30375e-18, 5.97432e-18}, {-4.4846e-18, 2.56065e-18}, {0, 0}, {-6.664e-18, 1.44867e-18}, {0, 0}, {0.0703638, -0.0703638}, {0, 0}, {2.27154e-34, 2.41419e-33}, {-1.71231e-17, 1.51992e-17}, {8.3133e-34, -2.92137e-34}, {-1.02748e-17, -3.28604e-18}, {4.30854e-18, -4.30854e-18}, {0.0703638, -0.0703638}, {-1.64723e-17, -1.23978e-17}, {0, 0}, {-7.5577e-34, -2.20308e-34}, {-1.01641e-17, -1.58735e-17}, {-1.07795e-17, 3.69775e-18}, {0, 0}, {-1.3001e-17, 1.26552e-17}, {0, 0}, {-0.649674, 0.649674}, {0, 0}, {-2.88142e-34, 1.71831e-34}, {-1.0639e-17, 3.83824e-18}, {-3.64383e-33, 3.0053e-33}, {9.59339e-19, -1.33189e-17}, {-3.97811e-17, 3.97811e-17}, {-0.649674, 0.649674}, {-0.108101, 0.0759951}, {-0.0132256, -0.103687}, {0.0775753, 0.197664}, {-0.0268197, -0.217684}, {-0.0973845, 0.0133805}, {0.166886, -0.176922}, {-0.174082, 0.0368077}, {-0.146363, 0.0633013}, {0.101839, 0.00578492}, {0.28734, -0.0850284}, {0.0864538, 0.0798966}, {-0.134927, 0.294084}, {-0.306375, 0.0444565}, {0.177774, -0.112248}, {-0.0713049, 0.0285475}, {-0.0659292, 0.143305}, {0.127778, -0.00169064}, {-0.0844571, -0.0840331}, {0.0196931, -0.192129}, {-0.116802, 0.133512}, {-0.0707155, 0.00573431}, {0.0151778, 0.00127919}, {0.26952, -0.0982653}, {-0.108576, -0.168438}
    };

    std::vector<std::complex<double> > becp = {
      {0.714037,0}, {4.0926e-34,4.77049e-18}, {-6.74075e-34,1.9082e-17}, {3.85186e-34,3.38271e-17}, {0.310588,-1.38778e-17}, {-1.73472e-18,-1.0842e-18}, {0,-1.73472e-18}, {-1.73472e-18,-9.75782e-19}, {0,1.38778e-17}, {0.253264,-1.5508e-17}, {3.46945e-18,-7.70372e-34}, {-1.38778e-17,3.85186e-34},
      {2.498e-16,-3.46945e-18}, {-0.0270547,1.73472e-18}, {3.46945e-18,-1.21431e-17}, {7.80626e-18,-3.46945e-18}, {0,2.77556e-17}, {0,-7.70372e-34}, {0.253264,-1.5508e-17}, {-5.20417e-18,9.62965e-35}, {1.38778e-17,-1.38778e-17}, {3.46945e-18,-1.21431e-17}, {-0.0270547,5.20417e-18},
      {-1.45012e-18,0}, {0,-4.16334e-17}, {-1.04083e-17,3.85186e-34}, {-2.77556e-17,9.62965e-35}, {0.253264,-1.5508e-17}, {-2.77556e-17,1.73472e-17}, {7.80626e-18,-1.21431e-17}, {2.71051e-19,0}, {-0.0270547,0}, {0.310588,1.38778e-17}, {1.73472e-18,7.58942e-19}, {0,1.73472e-18},
      {0,1.38236e-18}, {0.714037,-4.54164e-34}, {-1.44445e-34,-1.73472e-18}, {3.85186e-34,-5.20417e-18}, {0,-9.54098e-18}, {-2.498e-16,-3.46945e-18}, {-0.0270547,5.20417e-18}, {2.60209e-18,1.38778e-17}, {7.80626e-18,6.93889e-18}, {-1.94015e-33,-1.38778e-17}, {0.253264,-1.5508e-17},
      {0,-3.85186e-34}, {3.46945e-18,-1.34815e-33}, {-1.38778e-17,-1.38778e-17}, {3.46945e-18,1.38778e-17}, {-0.0270547,-1.73472e-18}, {-2.11419e-18,0}, {-8.62772e-34,2.77556e-17}, {-3.46945e-18,3.85186e-34}, {0.253264,-1.5508e-17}, {2.25514e-17,-5.77779e-34}, {2.77556e-17,1.73472e-17},
      {7.80626e-18,1.04083e-17}, {-2.12775e-18,0}, {-0.0270547,1.73472e-18}, {2.20382e-34,8.32667e-17}, {6.93889e-18,-1.54074e-33}, {2.77556e-17,2.11852e-33}, {0.253264,-1.5508e-17} 
    };

    std::vector<std::complex<double> > becp_spin = {
      {0.589512, 0.589512}, {3.64365e-19, -4.40651e-19}, {-1.29215e-18, 4.53274e-18}, {-2.89869e-18, -2.22919e-18}, {0.219139, 0.219139}, {7.8567e-18, 5.10765e-18}, {1.34231e-18, 4.08972e-18}, {-2.39409e-18, 2.5215e-18}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {3.60972e-17, 3.60972e-17}, {2.07824e-35, 2.31311e-35}, {-1.24227e-34, 1.52266e-35}, {1.44626e-34, 4.05323e-36}, {1.34184e-17, 1.34184e-17}, {1.31795e-34, -4.10474e-34}, {-1.66585e-34, 9.16378e-35}, {-1.32349e-34, 1.79517e-34}, {0.589512, 0.589512}, {4.40651e-19, -3.64365e-19}, {-4.53274e-18, 1.29215e-18}, {2.22919e-18, 2.89869e-18}, {0.219139, 0.219139}, {5.43932e-18, 7.74187e-18}, {-1.07175e-18, 2.95268e-18}, {-1.71144e-18, 1.622e-18}, {-8.68944e-18, -1.36408e-17}, {-0.176383, 0.176383}, {-1.08051e-18, 1.19494e-18}, {-1.27578e-18, 1.23764e-18}, {2.58564e-17, -1.77329e-17}, {0.0191034, -0.0191034}, {1.00437e-18, -4.63648e-19}, {-1.21755e-18, 6.95205e-19}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {-2.93929e-17, -1.08179e-16}, {6.16074e-19, -4.63503e-19}, {-0.176383, 0.176383}, {-2.42806e-17, 1.71892e-17}, {-4.90688e-19, -5.17492e-18}, {4.68219e-18, 3.08002e-18}, {0.0191034, -0.0191034}, {-1.80924e-18, 3.93306e-19}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {-5.83871e-17, -8.11913e-17}, {-1.65566e-18, 1.61752e-18}, {-2.90701e-18, 1.32009e-17}, {-0.176383, 0.176383}, {-3.51717e-18, -5.31551e-18}, {1.46283e-18, 3.71325e-18}, {1.10533e-18, 1.60717e-18}, {0.0191034, -0.0191034}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {4.998e-34, 2.14907e-34}, {-1.08004e-17, 1.08004e-17}, {2.49587e-34, -3.33179e-34}, {-1.51376e-34, -4.54515e-35}, {1.34522e-35, 6.46718e-34}, {1.16975e-18, -1.16975e-18}, {1.26428e-34, 7.44588e-34}, {6.16712e-35, 6.55441e-34}, {-1.36408e-17, -8.68944e-18}, {-0.176383, 0.176383}, {-1.19494e-18, 1.08051e-18}, {-1.23764e-18, 1.27578e-18}, {2.82057e-17, -2.92614e-17}, {0.0191034, -0.0191034}, {-5.61528e-18, 7.02336e-18}, {-4.64885e-18, 4.12651e-18}, {-3.30189e-34, -3.16294e-33}, {2.32648e-34, -2.24179e-34}, {-1.08004e-17, 1.08004e-17}, {-4.55245e-34, 6.39369e-34}, {6.3834e-34, -1.16532e-33}, {2.51775e-34, 9.85922e-34}, {1.16975e-18, -1.16975e-18}, {2.25702e-34, -7.93138e-35}, {-1.08179e-16, -2.93929e-17}, {4.63503e-19, -6.16074e-19}, {-0.176383, 0.176383}, {-1.71892e-17, 2.42806e-17}, {7.1629e-18, -9.73473e-19}, {-2.14376e-18, 1.0437e-17}, {0.0191034, -0.0191034}, {-2.78955e-18, -8.92146e-19}, {-2.47792e-33, -1.42266e-33}, {-3.47566e-34, 1.50739e-34}, {-4.52883e-34, 4.40179e-34}, {-1.08004e-17, 1.08004e-17}, {-1.44014e-34, -4.80305e-34}, {2.6217e-34, 7.751e-34}, {2.20712e-34, -1.79579e-34}, {1.16975e-18, -1.16975e-18}, {-8.11913e-17, -5.83871e-17}, {-1.61752e-18, 1.65566e-18}, {-1.32009e-17, 2.90701e-18}, {-0.176383, 0.176383}, {-2.39376e-18, -1.11374e-17}, {-1.70296e-18, 7.41007e-18}, {8.68822e-20, 3.59858e-19}, {0.0191034, -0.0191034}, {0.219139, 0.219139}, {3.47642e-18, -4.46033e-18}, {-1.93065e-18, 4.42575e-19}, {-2.10762e-18, -1.3266e-18}, {0.589512, 0.589512}, {3.80353e-18, 8.90376e-19}, {-6.38248e-18, -9.07748e-19}, {-4.47215e-18, -3.36594e-18}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {1.34184e-17, 1.34184e-17}, {1.09277e-34, 3.39765e-34}, {3.29217e-35, -7.60789e-35}, {1.8854e-34, 1.02497e-34}, {3.60972e-17, 3.60972e-17}, {6.99955e-35, 1.07736e-34}, {-2.09497e-34, -2.59871e-36}, {-2.05188e-34, -5.98125e-35}, {0.219139, 0.219139}, {2.53277e-18, -6.06456e-18}, {-4.24787e-19, 3.27352e-18}, {-2.96609e-19, -1.4571e-18}, {0.589512, 0.589512}, {1.22352e-18, 1.7256e-18}, {-6.1128e-18, -2.37279e-18}, {-2.7595e-18, -4.30959e-18}, {2.33626e-17, 1.83796e-17}, {0.0191034, -0.0191034}, {-2.39357e-18, -2.05028e-18}, {3.78783e-18, 2.03957e-19}, {-9.08858e-18, 2.11978e-17}, {-0.176383, 0.176383}, {-9.51513e-19, 5.83892e-19}, {-2.9266e-18, 1.00392e-18}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {-1.32518e-17, -6.84152e-18}, {-2.35542e-18, -3.93758e-18}, {0.0191034, -0.0191034}, {5.91753e-19, 3.69179e-18}, {-3.38737e-17, -7.39245e-17}, {7.62727e-19, -1.1529e-18}, {-0.176383, 0.176383}, {-3.52972e-18, 3.43583e-18}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {-2.76266e-17, 3.33724e-17}, {-6.13303e-18, -8.03639e-18}, {-1.37711e-18, 1.53221e-18}, {0.0191034, -0.0191034}, {-3.72616e-17, -6.08227e-17}, {-3.55386e-18, 1.41988e-18}, {7.79838e-19, -4.53389e-18}, {-0.176383, 0.176383}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {2.30464e-34, -1.37798e-33}, {1.16975e-18, -1.16975e-18}, {-7.02008e-34, 2.73661e-34}, {-5.2029e-34, -4.60519e-35}, {9.4802e-35, 9.28562e-34}, {-1.08004e-17, 1.08004e-17}, {7.24015e-35, -1.72264e-34}, {-7.82293e-35, 4.66514e-35}, {1.40744e-17, 9.09148e-18}, {0.0191034, -0.0191034}, {-5.44414e-19, -5.63416e-18}, {2.80243e-19, 2.421e-19}, {-1.40399e-17, 1.57407e-17}, {-0.176383, 0.176383}, {-6.32261e-19, 4.69464e-19}, {-2.88845e-18, 1.04206e-18}, {-1.1114e-34, 1.11426e-33}, {-7.14712e-34, -3.21057e-34}, {1.16975e-18, -1.16975e-18}, {-9.77011e-35, 2.62371e-35}, {-1.2912e-33, -2.74098e-33}, {-1.68933e-34, -7.96096e-35}, {-1.08004e-17, 1.08004e-17}, {-9.89283e-34, 8.15924e-34}, {-1.22404e-17, 4.5782e-18}, {-3.93758e-18, 1.11402e-18}, {0.0191034, -0.0191034}, {4.39182e-19, 2.10964e-18}, {-3.89252e-17, -4.11175e-17}, {8.75675e-19, -1.47363e-18}, {-0.176383, 0.176383}, {2.60456e-19, -3.61601e-18}, {2.43024e-34, 2.12024e-33}, {-5.1185e-34, 5.86845e-35}, {-6.84459e-36, 1.97118e-34}, {1.16975e-18, -1.16975e-18}, {-2.83786e-33, -2.69662e-33}, {-1.73324e-35, 1.30199e-34}, {-2.87165e-34, 5.54942e-34}, {-1.08004e-17, 1.08004e-17}, {-1.11827e-17, 3.57217e-17}, {-2.70172e-18, -6.20931e-18}, {-1.49154e-18, 3.38136e-18}, {0.0191034, -0.0191034}, {-3.92459e-17, -9.33159e-17}, {-3.30441e-18, 1.72354e-18}, {4.53187e-18, -2.0829e-18}, {-0.176383, 0.176383}, {-0.151418, 0.198618}, {-0.0155794, -0.0174644}, {0.00201038, -0.0199633}, {0.00202476, -0.0450809}, {-0.200025, 0.221787}, {-0.0176768, 0.0684854}, {0.00190195, 0.0216325}, {-0.029349, 0.0206323}, {0.408276, -0.167813}, {-0.0100735, 0.0165499}, {0.0376307, -0.0209656}, {0.0402082, 0.0385619}, {0.219097, -0.0598169}, {-0.00842084, -0.0143768}, {-0.0303829, -0.0300247}, {-0.00359068, -0.0281506}, {-0.14026, -0.0404854}, {0.00109109, -0.00292143}, {0.0202843, -0.0142746}, {-0.0485386, -0.0222402}, {-0.185677, 0.179574}, {-0.0148842, -0.00619727}, {-0.0134826, -0.000761371}, {0.0210613, 0.0536649}, {-0.207809, -0.0204807}, {-0.0549812, 0.0409982}, {0.00202629, -0.0341294}, {0.0546881, 0.042039}, {-0.458935, 0.245445}, {-0.0517218, -0.0122408}, {-0.00560551, -0.0660771}, {-0.00728142, -0.0591003}, {-0.155758, 0.422446}, {-0.0153205, 0.040207}, {0.0216556, 0.0347654}, {0.00690013, -0.0297097}, {0.0434321, 0.371581}, {-0.0100212, 0.0461625}, {0.0512898, -0.00563137}, {-0.0264394, 0.00363274}, {-0.034846, 0.30711}, {-0.0275275, -0.0282763}, {0.024732, -0.0165754}, {0.0494484, 0.026971}, {-0.04498, 0.113595}, {-0.0265187, -0.0351804}, {-0.0163407, -0.0279149}, {0.0453088, -0.0480335}, {0.272916, 0.161803}, {-0.00593673, -0.0168516}, {-0.0293366, -0.0348744}, {-0.0224875, -0.011743}, {0.322949, -0.27542}, {0.00255536, -0.0277111}, {-0.0336606, 0.00922919}, {-0.0472624, 0.00999311}, {-0.263423, -0.0633477}, {0.0257213, -0.0185744}, {-0.0163487, 0.0403021}, {0.0299132, 0.000534618}, {-0.224408, 0.103351}, {0.00785259, 0.0280015}, {0.0358437, -0.0459518}, {-0.039737, 0.017186}, {-0.292207, 0.0754645}, {-0.0208575, -0.0629621}, {0.0281465, 0.0170231}, {0.0535657, -0.0192648}, {-0.464253, -0.227606}, {-0.0364398, -0.0141192}, {0.0575099, 0.0371234}, {0.0276488, 0.00157058}, {0.164371, 0.0675376}, {-0.0139698, -0.00674355}, {0.0320923, 0.00574237}, {-0.0603552, -0.00217345}, {0.259754, -0.129042}, {0.028301, 0.00154723}, {-0.0370689, -0.0339973}, {0.0780115, -0.0230848}, {0.289087, 0.294914}, {-0.0363624, -0.0339035}, {0.0266047, -0.0341726}, {-0.0165338, 0.00630703}, {0.323237, -0.0453932}, {0.00551181, 0.0162942}, {0.0160567, 0.0146466}, {0.0234718, 0.0216916}, {0.01868, -0.0721715}, {0.0352915, -0.00780156}, {-0.00497142, -0.0254699}, {-0.00224427, 0.00973094}, {-0.117051, 0.00786132}, {-0.0216563, 0.0443798}, {0.0356598, -0.0190251}, {-0.0366322, 0.0798423}, {0.343382, -0.162698}, {0.00479399, -0.0286857}, {-0.0311893, -0.0208961}, {0.0054034, -0.0899912}, {0.0522446, -0.388922}, {0.0574995, -0.00674239}, {0.0459952, 0.00146401}, {-0.0831795, 0.0120697}, {-0.121058, 0.0831456}, {0.0337971, -0.029239}, {-0.0110613, 0.010583}, {-0.0463398, -0.00201043}, {0.0138641, 0.027162}, {-0.0041907, -0.0154936}, {-0.0775822, -0.0342975}, {0.0482649, -0.0304748}, {-0.141787, -0.0941454}, {0.000844965, -0.0487674}, {0.016791, 0.0463802}, {-0.0193326, 0.0621471}, {0.280873, -0.0311559}, {-0.0424309, -0.0103027}, {0.0488979, -0.0063861}, {-0.0193589, 0.00775051}, {0.232211, 0.228798}, {-0.0161022, -0.037824}, {-0.0505177, -0.0398486}, {-0.045563, -0.0102292}, {0.293272, 0.0203175}, {-0.0411639, -0.0508697}, {0.00422893, -0.0372978}, {-0.0178995, 0.0389067}, {-0.215803, 0.202432}, {0.0257435, 0.0509673}, {-0.00603483, 0.0347632}, {-0.0491414, 0.0279653}, {-0.126275, 0.108109}, {0.0233505, -0.0253105}, {0.00746352, -0.00504986}, {0.0346911, -0.000459002}, {-0.010801, -0.129215}, {0.0181963, -0.0197627}, {-0.0141993, 0.0586362}, {0.0322462, 0.0085437}, {-0.0501468, -0.170172}, {-0.00405151, -0.00839154}, {0.00820234, 0.00534787}, {-0.0229297, -0.0228146}, {-0.223267, 0.0715199}, {-0.0147115, -0.0199799}, {0.0534475, -0.0275814}, {-0.000795322, -0.00978326}, {-0.262721, 0.0313206}, {-0.0497351, 0.0221968}, {-0.0259337, 0.0242573}, {0.00534659, -0.052162}, {-0.03707, -0.16004}, {-0.0739947, -0.042184}, {-0.0645123, 0.010567}, {-0.0188623, 0.0222532}, {-0.181946, 0.0823316}, {0.0102198, -0.0228838}, {0.0188543, -0.0475191}, {-0.0317113, 0.0362478}, {0.393393, 0.173144}, {0.016999, -0.0650595}, {0.0203212, -0.0512624}, {-0.0110779, -0.0165221}, {0.141654, -0.212}, {-0.0530882, -0.0204637}, {0.0529682, 0.0428501}, {-0.0191989, 0.00155684}, {0.164424, -0.0767119}, {-0.0210735, 0.013117}, {0.00625389, 0.0355862}, {-0.0206261, -0.0483646}, {-0.0468221, -0.0382823}, {-0.021515, -0.001295}, {0.0361506, -0.0027438}, {0.00412072, 0.000347295}, {-0.0752766, 0.59987}, {0.0802704, 0.0203869}, {-0.0092008, 0.0522364}, {-0.0737371, 0.00926323}, {-0.100885, 0.485237}, {0.0244448, -0.00662269}, {0.0162096, -0.0330803}, {0.0731735, -0.0266786}, {-0.0168811, 0.217894}, {-0.0184495, -0.00802871}, {-0.0359591, 0.0765698}, {0.0659872, 0.0210129}, {-0.206596, 0.136648}, {0.0132486, -0.00953235}, {0.0281496, 0.0173019}, {-0.0294779, -0.0457301}
    };
    const psi::DEVICE_CPU * cpu_ctx = {};
    const psi::DEVICE_GPU * gpu_ctx = {};

    void SetUp() override {
    }
    void TearDown() override {
    }

    using nonlocal_cpu_op = hamilt::nonlocal_pw_op<double, psi::DEVICE_CPU>;
    using nonlocal_gpu_op = hamilt::nonlocal_pw_op<double, psi::DEVICE_GPU>;
    using set_memory_complex_double_op = psi::memory::set_memory_op<std::complex<double>, psi::DEVICE_GPU>;
    using delete_memory_double_op = psi::memory::delete_memory_op<double, psi::DEVICE_GPU>;
    using delete_memory_complex_double_op = psi::memory::delete_memory_op<std::complex<double>, psi::DEVICE_GPU>;
    using resize_memory_double_op = psi::memory::resize_memory_op<double, psi::DEVICE_GPU>;
    using resize_memory_complex_double_op = psi::memory::resize_memory_op<std::complex<double>, psi::DEVICE_GPU>;
    using syncmem_d_h2d_op = psi::memory::synchronize_memory_op<double, psi::DEVICE_GPU, psi::DEVICE_CPU>;
    using syncmem_cd_h2d_op = psi::memory::synchronize_memory_op<std::complex<double>, psi::DEVICE_GPU, psi::DEVICE_CPU>;
    using syncmem_cd_d2h_op = psi::memory::synchronize_memory_op<std::complex<double>, psi::DEVICE_CPU, psi::DEVICE_GPU>;
};

TEST_F(TestModuleHamiltNonlocal, nonlocal_pw_op_cpu)
{
  sum = 0; iat = 0;
  std::vector<std::complex<double>> ps(expected_ps.size(), std::complex<double>(0.0, 0.0));
  nonlocal_cpu_op()(
      cpu_ctx, 
      l1, l2, l3, 
      sum, iat, spin, nkb, 
      deeq_x, deeq_y, deeq_z,
      deeq.data(),
      ps.data(), becp.data());
  for (int ii = 0; ii < ps.size(); ii++) {
    EXPECT_LT(std::abs(ps[ii] - expected_ps[ii]), 5 * 1e-6);
  }
  EXPECT_EQ(sum, expected_sum);
  EXPECT_EQ(iat, expected_iat);
}

TEST_F(TestModuleHamiltNonlocal, nonlocal_pw_spin_op_cpu)
{
  sum = 0; iat = 0;
  std::vector<std::complex<double>> ps(expected_ps_spin.size(), std::complex<double>(0.0, 0.0));
  nonlocal_cpu_op()(
      cpu_ctx, 
      l1, l2_spin, l3, 
      sum, iat, nkb, 
      deeq_x, deeq_y, deeq_z,
      deeq_spin.data(),
      ps.data(), becp_spin.data());
  for (int ii = 0; ii < ps.size(); ii++) {
    EXPECT_LT(std::abs(ps[ii] - expected_ps_spin[ii]), 5 * 1e-6);
  }
  EXPECT_EQ(sum, expected_sum);
  EXPECT_EQ(iat, expected_iat);
}

#if __CUDA || __UT_USE_CUDA || __ROCM || __UT_USE_ROCM
TEST_F(TestModuleHamiltNonlocal, nonlocal_pw_op_gpu)
{
  sum = 0; iat = 0;
  double* deeq_dev = NULL;
  std::complex<double>* ps_dev = NULL, * becp_dev = NULL;
  std::vector<std::complex<double>> ps(expected_ps.size(), std::complex<double>(0.0, 0.0));
  resize_memory_double_op()(gpu_ctx, deeq_dev, deeq.size());
  resize_memory_complex_double_op()(gpu_ctx, ps_dev, ps.size());
  resize_memory_complex_double_op()(gpu_ctx, becp_dev, becp.size());
  syncmem_d_h2d_op()(gpu_ctx, cpu_ctx, deeq_dev, deeq.data(), deeq.size());
  syncmem_cd_h2d_op()(gpu_ctx, cpu_ctx, ps_dev, ps.data(), ps.size());
  syncmem_cd_h2d_op()(gpu_ctx, cpu_ctx, becp_dev, becp.data(), becp.size());
  nonlocal_gpu_op()(
      gpu_ctx, 
      l1, l2, l3, 
      sum, iat, spin, nkb, 
      deeq_x, deeq_y, deeq_z,
      deeq_dev,
      ps_dev, becp_dev);

  syncmem_cd_d2h_op()(cpu_ctx, gpu_ctx, ps.data(), ps_dev, ps.size());
  for (int ii = 0; ii < ps.size(); ii++) {
    EXPECT_LT(fabs(ps[ii] - expected_ps[ii]), 5 * 1e-6);
  }
  EXPECT_EQ(sum, expected_sum);
  EXPECT_EQ(iat, expected_iat);
  delete_memory_double_op()(gpu_ctx, deeq_dev);
  delete_memory_complex_double_op()(gpu_ctx, ps_dev);
  delete_memory_complex_double_op()(gpu_ctx, becp_dev);
}

TEST_F(TestModuleHamiltNonlocal, nonlocal_pw_spin_op_gpu)
{
  sum = 0; iat = 0;
  std::complex<double>* ps_dev = NULL, * becp_dev = NULL, * deeq_dev = NULL;
  std::vector<std::complex<double>> ps(expected_ps.size(), std::complex<double>(0.0, 0.0));
  resize_memory_complex_double_op()(gpu_ctx, deeq_dev, deeq_spin.size());
  resize_memory_complex_double_op()(gpu_ctx, ps_dev, ps.size());
  resize_memory_complex_double_op()(gpu_ctx, becp_dev, becp_spin.size());
  syncmem_cd_h2d_op()(gpu_ctx, cpu_ctx, deeq_dev, deeq_spin.data(), deeq_spin.size());
  syncmem_cd_h2d_op()(gpu_ctx, cpu_ctx, ps_dev, ps.data(), ps.size());
  syncmem_cd_h2d_op()(gpu_ctx, cpu_ctx, becp_dev, becp_spin.data(), becp_spin.size());
  nonlocal_gpu_op()(
      gpu_ctx, 
      l1, l2_spin, l3, 
      sum, iat, nkb, 
      deeq_x, deeq_y, deeq_z,
      deeq_dev,
      ps_dev, becp_dev);

  syncmem_cd_d2h_op()(cpu_ctx, gpu_ctx, ps.data(), ps_dev, ps.size());
  for (int ii = 0; ii < ps.size(); ii++) {
    EXPECT_LT(fabs(ps[ii] - expected_ps_spin[ii]), 5 * 1e-6);
  }
  EXPECT_EQ(sum, expected_sum);
  EXPECT_EQ(iat, expected_iat);
  delete_memory_complex_double_op()(gpu_ctx, deeq_dev);
  delete_memory_complex_double_op()(gpu_ctx, ps_dev);
  delete_memory_complex_double_op()(gpu_ctx, becp_dev);
}
#endif // __CUDA || __UT_USE_CUDA || __ROCM || __UT_USE_ROCM