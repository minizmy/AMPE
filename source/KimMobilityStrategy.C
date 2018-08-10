#include "KimMobilityStrategy.h"

#include "CALPHADFreeEnergyFunctionsBinary.h"
#include "CALPHADFreeEnergyFunctionsTernary.h"
#include "QuatModel.h"

using namespace std;

KimMobilityStrategy::KimMobilityStrategy(
   QuatModel* quat_model,
   const int conc_l_id,
   const int conc_s_id,
   const int temp_id,
   const double epsilon,
   const double phase_well_scale,
   const string& energy_interp_func_type,
   const string& conc_interp_func_type,
   boost::shared_ptr<tbox::Database> calphad_db,
   boost::shared_ptr<tbox::Database> newton_db,
   const unsigned ncompositions,
   const double DL, const double Q0,
   const double mv):
      SimpleQuatMobilityStrategy(quat_model),
      d_conc_l_id(conc_l_id),
      d_conc_s_id(conc_s_id),
      d_temp_id(temp_id),
      d_epsilon(epsilon),
      d_phase_well_scale(phase_well_scale),
      d_ncompositions(ncompositions),
      d_DL(DL),
      d_Q0(Q0),
      d_mv(mv)
{
   assert( d_conc_l_id>=0 );
   assert( d_conc_s_id>=0 );
   assert( d_temp_id>=0 );
   assert( d_epsilon>0. );
   assert( d_phase_well_scale>=0. );
   assert( d_ncompositions>0 );

   if( ncompositions==1 ){
      d_calphad_fenergy = new
         CALPHADFreeEnergyFunctionsBinary(calphad_db,newton_db,
                                 energy_interp_func_type,
                                 conc_interp_func_type,
                                 false, // no 3rd phase
                                 phase_well_scale,0.,
                                 "double_well","");
   }else{
      d_calphad_fenergy = new
         CALPHADFreeEnergyFunctionsTernary(calphad_db,newton_db,
                                 energy_interp_func_type,
                                 conc_interp_func_type,
                                 phase_well_scale,
                                 "double_well");
   }

}

void KimMobilityStrategy::computePhaseMobility(
      const boost::shared_ptr<hier::PatchHierarchy > hierarchy,
      int& phase_id,
      int& mobility_id,
      const double time,
      const CACHE_TYPE cache )
{
   (void)phase_id;
   (void)time;
   (void)cache;

   const int maxl = hierarchy->getNumberOfLevels();

   for ( int amr_level = 0; amr_level < maxl; amr_level++ ) {
      boost::shared_ptr<hier::PatchLevel > level =
         hierarchy->getPatchLevel( amr_level );

      for( hier::PatchLevel::Iterator p(level->begin()); p!=level->end(); p++ ){
         boost::shared_ptr<hier::Patch > patch = *p;

         boost::shared_ptr< pdat::CellData<double> > temperature(
            BOOST_CAST< pdat::CellData<double>, hier::PatchData>(
               patch->getPatchData( d_temp_id) ) );
         assert( temperature );

         boost::shared_ptr< pdat::CellData<double> > concl(
            BOOST_CAST< pdat::CellData<double>, hier::PatchData>(
               patch->getPatchData( d_conc_l_id) ) );
         assert( concl );

         boost::shared_ptr< pdat::CellData<double> > concs(
            BOOST_CAST< pdat::CellData<double>, hier::PatchData>(
               patch->getPatchData( d_conc_s_id) ) );
         assert( concs );

         boost::shared_ptr< pdat::CellData<double> > mobility(
            BOOST_CAST< pdat::CellData<double>, hier::PatchData>(
               patch->getPatchData( mobility_id ) ) );
         assert( mobility );

         update(temperature, concl, concs, mobility, patch);
      }
   }
}

void KimMobilityStrategy::update(
   boost::shared_ptr< pdat::CellData<double> > cd_te,
   boost::shared_ptr< pdat::CellData<double> > cd_cl,
   boost::shared_ptr< pdat::CellData<double> > cd_cs,
   boost::shared_ptr< pdat::CellData<double> > cd_mob,
   boost::shared_ptr<hier::Patch > patch )
{
   assert( cd_te );
   assert( cd_cl );
   assert( cd_cs );
   assert( cd_mob );

   const hier::Box& pbox ( patch->getBox() );
   int imin[3] = {pbox.lower(0),pbox.lower(1),0};
   int imax[3] = {pbox.upper(0),pbox.upper(1),0};
#if (NDIM == 3)
   imin[2] = pbox.lower(2);
   imax[2] = pbox.upper(2);
#endif

   const hier::Box& temp_gbox = cd_te->getGhostBox();
   int min_te[3] = {temp_gbox.lower(0),temp_gbox.lower(1),0};
   int inc_j_te = temp_gbox.numberCells(0);
#if (NDIM == 3)
   min_te[2] = temp_gbox.lower(2);
   int inc_k_te = inc_j_te * temp_gbox.numberCells(1);
#else
   int inc_k_te = 0;
#endif

   //assume cs and cl have the same number of ghost values
   const hier::Box& ci_gbox = cd_cl->getGhostBox();
   int min_ci[3] = {ci_gbox.lower(0),ci_gbox.lower(1),0};
   int inc_j_ci = ci_gbox.numberCells(0);
#if (NDIM == 3)
   min_ci[2] = ci_gbox.lower(2);
   int inc_k_ci = inc_j_ci * ci_gbox.numberCells(1);
#else
   int inc_k_ci = 0;
#endif

   const hier::Box& mo_gbox = cd_mob->getGhostBox();
   int min_mo[3] = {mo_gbox.lower(0),mo_gbox.lower(1),0};
   int inc_j_mo = mo_gbox.numberCells(0);
#if (NDIM == 3)
   min_mo[2] = mo_gbox.lower(2);
   int inc_k_mo = inc_j_mo * mo_gbox.numberCells(1);
#else
   int inc_k_mo = 0;
#endif

   int idx_te = (imin[0]-min_te[0])
              + (imin[1]-min_te[1])*inc_j_te
              + (imin[2]-min_te[2])*inc_k_te;
   int idx_ci = (imin[0]-min_ci[0])
              + (imin[1]-min_ci[1])*inc_j_ci
              + (imin[2]-min_ci[2])*inc_k_ci;
   int idx_mo = (imin[0]-min_mo[0])
              + (imin[1]-min_mo[1])*inc_j_mo
              + (imin[2]-min_mo[2])*inc_k_mo;

   const double xi = d_epsilon/sqrt(32.*d_phase_well_scale);
   const double a2 = 47./60.;
   const PHASE_INDEX pi0=phaseL;
   std::vector<double> d2fdc2(d_ncompositions*d_ncompositions);

   std::vector<double> phaseconc(2*d_ncompositions); // 2 for two phases
   double* cl=&phaseconc[0];
   double* cs=&phaseconc[d_ncompositions];

   for ( int kk = imin[2]; kk <= imax[2]; kk++ ) {
      for ( int jj = imin[1]; jj <= imax[1]; jj++ ) {
         for ( int ii = imin[0]; ii <= imax[0]; ii++ ) {

            const double temp = cd_te->getPointer()[idx_te];
            for(unsigned ic=0;ic<d_ncompositions;ic++)
               cl[ic] = cd_cl->getPointer(ic)[idx_ci];
            for(unsigned ic=0;ic<d_ncompositions;ic++)
               cs[ic] = cd_cs->getPointer(ic)[idx_ci];

            d_calphad_fenergy->computeSecondDerivativeFreeEnergy(
               temp,&phaseconc[0],pi0,d2fdc2);

            double zeta=0.;
            for(unsigned i=0;i<d_ncompositions;i++)
            for(unsigned j=0;j<d_ncompositions;j++)
               zeta+=(cl[i]-cs[i])*d2fdc2[2*i+j]*(cl[j]-cs[j]);
            const double DL=d_DL*exp(-d_Q0/(gas_constant_R_JpKpmol*temp));
            zeta/=DL;
            zeta*=(1.e-6/d_mv); // convert from J/mol to pJ/um^3

            cd_mob->getPointer()[idx_mo]=1./(3.*(2.*xi*xi)*a2*zeta);

            idx_te++;
            idx_ci++;
            idx_mo++;
         }
         idx_te+=2*cd_te->getGhostCellWidth()[0];
         idx_ci+=2*cd_cl->getGhostCellWidth()[0];
         idx_mo+=2*cd_mob->getGhostCellWidth()[0];
      }
      idx_te+=2*inc_j_te*cd_te->getGhostCellWidth()[1];
      idx_ci+=2*inc_j_ci*cd_cl->getGhostCellWidth()[1];
      idx_mo+=2*inc_j_mo*cd_mob->getGhostCellWidth()[1];
   }
}
