
/**
 * This is a Herwig6-specific parton selector that selects all partons that don't have other partons as daughters, i.e., partons
 * from the end of the parton showering sequence. An explanation of the particle status codes returned by Herwig6 can be found in
 * Section 8.3.1 of Herwig6 manual (http://arxiv.org/abs/hep-ph/0011363)
 */

#include "PhysicsTools/JetMCAlgos/interface/Herwig6PartonSelector.h"
#include "PhysicsTools/JetMCUtils/interface/CandMCTag.h"


Herwig6PartonSelector::Herwig6PartonSelector()
{
}

Herwig6PartonSelector::~Herwig6PartonSelector()
{
}

void
Herwig6PartonSelector::run(const edm::Handle<reco::GenParticleCollection> & particles,
                          std::auto_ptr<reco::GenParticleRefVector> & partons)
{
   // loop over particles and select partons
   for(reco::GenParticleCollection::const_iterator it = particles->begin(); it != particles->end(); ++it)
   {
     if( it->status()==1 ) continue;                   // skip stable particles
     if( !CandMCTagUtils::isParton( *it ) ) continue;  // skip particle if not a parton

     // check if the parton has other partons as daughters
     int nparton_daughters = 0;
     for(unsigned i=0; i<it->numberOfDaughters(); ++i)
     {
       if( CandMCTagUtils::isParton( *(it->daughter(i)) ) )
         ++nparton_daughters;
     }

     if( nparton_daughters==0 )
       partons->push_back( reco::GenParticleRef( particles, it - particles->begin() ) );
   }

   return;
}
