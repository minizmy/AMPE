// Copyright (c) 2018, Lawrence Livermore National Security, LLC and
// UT-Battelle, LLC.
// Produced at the Lawrence Livermore National Laboratory and
// the Oak Ridge National Laboratory
// Written by M.R. Dorr, J.-L. Fattebert and M.E. Wickett
// LLNL-CODE-747500
// All rights reserved.
// This file is part of AMPE.
// For details, see https://github.com/LLNL/AMPE
// Please also read AMPE/LICENSE.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// - Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the disclaimer below.
// - Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the disclaimer (as noted below) in the
//   documentation and/or other materials provided with the distribution.
// - Neither the name of the LLNS/LLNL nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL SECURITY,
// LLC, UT BATTELLE, LLC,
// THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
// IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
#include "KimMobilityStrategyInfMob.h"

#include <iomanip>

KimMobilityStrategyInfMob::KimMobilityStrategyInfMob(
    QuatModel* quat_model, const int conc_l_id, const int conc_s_id,
    const int temp_id, const double epsilon, const double phase_well_scale,
    const EnergyInterpolationType energy_interp_func_type,
    const ConcInterpolationType conc_interp_func_type,
    std::shared_ptr<tbox::Database> conc_db, const unsigned ncompositions,
    const double DL, const double Q0, const double mv)
    : KimMobilityStrategy(quat_model, conc_l_id, conc_s_id, temp_id,
                          energy_interp_func_type, conc_interp_func_type,
                          conc_db, ncompositions),
      d_DL(DL),
      d_Q0(Q0)
{
   assert(epsilon > 0.);
   assert(phase_well_scale >= 0.);
   assert(mv > 0.);

   double a2 = 0.;
   switch (energy_interp_func_type) {
      case EnergyInterpolationType::PBG: a2 = 47. / 60.; break;
      case EnergyInterpolationType::HARMONIC: a2 = 0.5; break;
      default:
         TBOX_ERROR(
             "Invalid interpolation function in KimMobilityStrategyInfMob");
   }

   const double xi = epsilon / sqrt(32. * phase_well_scale);

   d_factor = 3. * (2. * xi * xi) * a2;
   d_factor *= (1.e-6 / mv);  // convert from J/mol to pJ/um^3

   d_d2fdc2.resize(d_ncompositions * d_ncompositions);
}

double KimMobilityStrategyInfMob::evaluateMobility(
    const double temp, const std::vector<double>& phaseconc)
{
   const PhaseIndex pi0 = PhaseIndex::phaseL;

   d_fenergy->computeSecondDerivativeFreeEnergy(temp, &phaseconc[0], pi0,
                                                d_d2fdc2);
   // std::cout<<std::setprecision(15);
   // std::cout<<"c="<<phaseconc[0]<<", d2fdc2="<<d_d2fdc2[0]<<std::endl;
   const double* const cl = &phaseconc[0];
   const double* const cs = &phaseconc[d_ncompositions];

   double zeta = 0.;
   for (unsigned i = 0; i < d_ncompositions; i++)
      for (unsigned j = 0; j < d_ncompositions; j++)
         zeta += (cl[i] - cs[i]) * d_d2fdc2[2 * i + j] * (cl[j] - cs[j]);
   const double DL = d_DL * exp(-d_Q0 / (gas_constant_R_JpKpmol * temp));

   const double mob = DL / (d_factor * zeta);
   // std::cout<<"DL="<<DL<<", zeta="<<zeta<<std::endl;
   assert(mob == mob);
   return mob;
}
