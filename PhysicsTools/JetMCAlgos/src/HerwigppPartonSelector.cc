
/**
 * This is a Herwig++-specific parton selector that selects all partons that don't have other partons as daughters, i.e., partons
 * from the end of the parton showering sequence
 */

#include "PhysicsTools/JetMCAlgos/interface/HerwigppPartonSelector.h"
#include "PhysicsTools/JetMCUtils/interface/CandMCTag.h"


HerwigppPartonSelector::HerwigppPartonSelector()
{
}

HerwigppPartonSelector::~HerwigppPartonSelector()
{
}

void
HerwigppPartonSelector::run(const edm::Handle<reco::GenParticleCollection> & particles,
                          std::auto_ptr<reco::GenParticleRefVector> & partons)
{
   // loop over particles and select partons
   for(reco::GenParticleCollection::const_iterator it = particles->begin(); it != particles->end(); ++it)
   {
     if( it->status()==1 ) continue;                   // skip stable particles
     if( !CandMCTagUtils::isParton( *it ) ) continue;  // skip particle if not a parton

     // check if the parton has other partons as daughters
     unsigned nparton_daughters = 0;
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
