// -*- C++ -*-
//
// Package:    IPAnalyzer
// Class:      IPAnalyzer
// 
/**\class IPAnalyzer IPAnalyzer.cc RecoBTag/IPAnalyzer/src/IPAnalyzer.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Andrea Rizzi
//         Created:  Wed Apr 12 11:12:49 CEST 2006
// $Id: IPAnalyzer.cc,v 1.11 2012/01/25 15:34:28 innocent Exp $
//
//



// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/JetTracksAssociation.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/BTauReco/interface/TrackIPTagInfo.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/IPTools/interface/IPTools.h"

#include "DataFormats/Math/interface/Vector3D.h"

// Math
#include "Math/GenVector/VectorUtil.h"
#include "Math/GenVector/PxPyPzE4D.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TH1D.h"
#include "TH2D.h"

// system include files
#include <string>
#include <iostream>

using namespace std;
using namespace reco;

//
// class decleration
//

class IPAnalyzer : public edm::EDAnalyzer {
   public:
      explicit IPAnalyzer(const edm::ParameterSet&);
      ~IPAnalyzer() {}

      virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);

   private:
     edm::InputTag m_ipassoc;
     
     edm::Service<TFileService> fs;
     
     TH2D *h2_DTJA_Helix_vs_Linearized;
};

//
// constructors and destructor
//
IPAnalyzer::IPAnalyzer(const edm::ParameterSet& iConfig)
{
  m_ipassoc = iConfig.getParameter<edm::InputTag>("ipassociation");
  
  h2_DTJA_Helix_vs_Linearized = fs->make<TH2D>("h2_DTJA_Helix_vs_Linearized",";DTJA (helix) [cm];DTJA (linearized) [cm]", 100, 0, 2, 100, 0, 2);
}

void
IPAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace reco;

  Handle<TrackIPTagInfoCollection> ipHandle;
  iEvent.getByLabel(m_ipassoc, ipHandle);
  const TrackIPTagInfoCollection & ip = *(ipHandle.product());
  //cout << "Found " << ip.size() << " TagInfo" << endl;

  // get TransientTrackBuilder
  ESHandle<TransientTrackBuilder> builder;
  iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder", builder);

  //cout << boolalpha;
  //cout << fixed;



  TrackIPTagInfoCollection::const_iterator it = ip.begin();
  for(; it != ip.end(); it++)
  {
    //cout << "Jet pt: " << it->jet()->pt() << endl;
    //cout << "Tot tracks: " << it->tracks().size() << endl;    
    TrackRefVector selTracks=it->selectedTracks();
    int n=it->selectedTracks().size();
    //cout << "Sel tracks: " << n << endl; 

    GlobalVector direction(it->jet()->px(), it->jet()->py(), it->jet()->pz());

    for(int j=0; j< n;j++)
    {
      reco::TransientTrack transientTrack = builder->build(*(it->selectedTracks()[j]));

      double distanceToJetAxis = IPTools::jetTrackDistance(transientTrack, direction, *(it->primaryVertex())).second.value();
      double distanceToJetAxisLinearized = IPTools::jetTrackDistanceLinearized(transientTrack, direction, *(it->primaryVertex())).second.value();

      h2_DTJA_Helix_vs_Linearized->Fill(fabs(distanceToJetAxis),fabs(distanceToJetAxisLinearized));
    }
  }

}

//define this as a plug-in
DEFINE_FWK_MODULE(IPAnalyzer);
