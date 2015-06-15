#include <TH1F.h>
#include <TH2F.h>
#include <TROOT.h>
#include <TFile.h>
#include <TSystem.h>

#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

#include "DataFormats/FWLite/interface/InputSource.h"
#include "DataFormats/FWLite/interface/OutputFiles.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

#include "PhysicsTools/FWLite/interface/TFileService.h"

#include "fastjet/JetDefinition.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/Selector.hh"
#include "fastjet/PseudoJet.hh"

int main(int argc, char* argv[]) 
{
  // ----------------------------------------------------------------------
  // First Part: 
  //
  //  * enable the AutoLibraryLoader 
  //  * book the histograms of interest 
  //  * open the input file
  // ----------------------------------------------------------------------

  // load framework libraries
  gSystem->Load( "libFWCoreFWLite" );
  AutoLibraryLoader::enable();

  // parse arguments
  if ( argc < 2 ) {
    std::cout << "Usage : " << argv[0] << " [parameters.py]" << std::endl;
    return 0;
  }

  if( !edm::readPSetsFrom(argv[1])->existsAs<edm::ParameterSet>("process") ){
    std::cout << " ERROR: ParametersSet 'process' is missing in your configuration file" << std::endl; exit(0);
  }
  // get the python configuration
  const edm::ParameterSet& process = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");
  fwlite::InputSource inputHandler_(process); fwlite::OutputFiles outputHandler_(process);

  // now get each parameter
  const edm::ParameterSet& ana = process.getParameter<edm::ParameterSet>("fastjetAnalyzer");
  edm::InputTag pfCandidates_( ana.getParameter<edm::InputTag>("pfCandidates") );
  edm::InputTag packedCandidates_( ana.getParameter<edm::InputTag>("packedCandidates") );
  double rParam_( ana.getParameter<double>("rParam") );
  std::string jetAlgorithm_( ana.getParameter<std::string>("jetAlgorithm") );

  // book a set of histograms
  fwlite::TFileService fs = fwlite::TFileService(outputHandler_.file().c_str());
  TFileDirectory dir = fs.mkdir("output");
  TH2F* h2_nJetDiffAOD     = dir.make<TH2F>("h2_nJetDiffAOD"     , "AOD;Original nJet;(Reclustered - Original) nJet",      100, -0.5, 99.5,  10, -4.5,  5.5);
  TH2F* h2_nJetDiffMiniAOD = dir.make<TH2F>("h2_nJetDiffMiniAOD" , "MiniAOD;Original nJet;(Reclustered - Original) nJet",  100, -0.5, 99.5,  10, -4.5,  5.5);
  TH2F* h2_nJetDiff        = dir.make<TH2F>("h2_nJetDiff"        , ";AOD nJet;(MiniAOD - AOD) nJet",                       100, -0.5, 99.5,  20, -9.5, 10.5);

  TH2F* h2_JetPtRelDiffAOD     = dir.make<TH2F>("h2_JetPtRelDiffAOD"     , "AOD N^{th} Jet;Original p_{T};(Reclustered - Original)/Original p_{T}",      50, 0., 250.,  200, -1, 1);
  TH2F* h2_JetPtRelDiffMiniAOD = dir.make<TH2F>("h2_JetPtRelDiffMiniAOD" , "MiniAOD N^{th} Jet;Original p_{T};(Reclustered - Original)/Original p_{T}",  50, 0., 250.,  200, -1, 1);
  TH2F* h2_JetPtRelDiff        = dir.make<TH2F>("h2_JetPtRelDiff"        , "N^{th} Jet;AOD p_{T};(MiniAOD - AOD)/AOD p_{T}",                             50, 0., 250.,  200, -1, 1);


//   double phi1 = M_PI;
//   double phi2 = -M_PI;
//   printf("%10.20e\n",phi1);
//   printf("%10.20e\n",phi2);
//   printf("%10.20e\n",reco::deltaPhi(phi1,phi2));
//   int16_t packedPhi1 =  int16_t(std::round(phi1/3.2f*std::numeric_limits<int16_t>::max()));
//   int16_t packedPhi2 =  int16_t(std::round(phi2/3.2f*std::numeric_limits<int16_t>::max()));
//   std::cout << packedPhi1 << std::endl;
//   std::cout << packedPhi2 << std::endl;
//   double unpackedPhi1 = int16_t(packedPhi1)*3.2f/std::numeric_limits<int16_t>::max() + 0.1*3.2/std::numeric_limits<int16_t>::max();
//   double unpackedPhi2 = int16_t(packedPhi2)*3.2f/std::numeric_limits<int16_t>::max() - 0.1*3.2/std::numeric_limits<int16_t>::max();
//   printf("%10.20e\n",unpackedPhi1);
//   printf("%10.20e\n",unpackedPhi2);
//   printf("%10.20e\n",reco::deltaPhi(unpackedPhi1,unpackedPhi2));
//   packedPhi1 =  int16_t(std::round(unpackedPhi1/3.2f*std::numeric_limits<int16_t>::max()));
//   packedPhi2 =  int16_t(std::round(unpackedPhi2/3.2f*std::numeric_limits<int16_t>::max()));
//   std::cout << packedPhi1 << std::endl;
//   std::cout << packedPhi2 << std::endl;

  fastjet::JetDefinition *fjJetDefinition_;

  // set jet algorithm
  if (jetAlgorithm_=="Kt")
    fjJetDefinition_= new fastjet::JetDefinition(fastjet::kt_algorithm, rParam_);
  else if (jetAlgorithm_=="CambridgeAachen")
    fjJetDefinition_= new fastjet::JetDefinition(fastjet::cambridge_algorithm, rParam_);
  else if (jetAlgorithm_=="AntiKt")
    fjJetDefinition_= new fastjet::JetDefinition(fastjet::antikt_algorithm, rParam_);
  else {
    std::cout << "ERROR: Jet clustering algorithm is invalid: " << jetAlgorithm_ << ", use CambridgeAachen | Kt | AntiKt" << std::endl; exit(0);
  }

  // loop the events
  int ievt=0;
  int maxEvents_( inputHandler_.maxEvents() );
  for(size_t iFile=0; iFile<inputHandler_.files().size(); ++iFile){
    // open input file (can be located on castor)
    TFile* inFile = TFile::Open(inputHandler_.files()[iFile].c_str());
    if( inFile ){
      // ----------------------------------------------------------------------
      // Second Part: 
      //
      //  * loop the events in the input file 
      //  * receive the collections of interest via fwlite::Handle
      //  * fill the histograms
      //  * after the loop close the input file
      // ----------------------------------------------------------------------
      fwlite::Event ev(inFile);
      for(ev.toBegin(); !ev.atEnd(); ++ev, ++ievt){
	edm::EventBase const & event = ev;
	// break loop if maximal number of events is reached 
	if(maxEvents_>0 ? ievt+1>maxEvents_ : false) break;
	// simple event counter
	if(inputHandler_.reportAfter()!=0 ? (ievt>0 && ievt%inputHandler_.reportAfter()==0) : false) 
	  std::cout << "  processing event: " << ievt << std::endl;

	// Handle to the PF candidates collection
	edm::Handle<std::vector<reco::PFCandidate> > pfCandidates;
	event.getByLabel(pfCandidates_, pfCandidates);
	
	// Handle to the packed candidates collection
	edm::Handle<std::vector<pat::PackedCandidate> > packedCandidates;
	event.getByLabel(packedCandidates_, packedCandidates);
	
	double ptmin = 0.;
	//---------------------------------------------------------
	// AOD
	std::vector<fastjet::PseudoJet> input_particles_AOD;
	
	// loop over PF candidates collection
	for(std::vector<reco::PFCandidate>::const_iterator it=pfCandidates->begin(); it!=pfCandidates->end(); ++it)
	  input_particles_AOD.push_back(fastjet::PseudoJet(it->px(),it->py(),it->pz(),it->energy()));

	fastjet::ClusterSequence clust_seq_AOD(input_particles_AOD, *fjJetDefinition_);
	
	std::vector<fastjet::PseudoJet> inclusive_jets_AOD = fastjet::sorted_by_pt(clust_seq_AOD.inclusive_jets(ptmin));
	
	std::vector<fastjet::PseudoJet> input_reclustering_AOD;
	
	for (size_t i = 0; i < inclusive_jets_AOD.size(); i++) {
	  std::vector<fastjet::PseudoJet> constituents = inclusive_jets_AOD[i].constituents();
	  input_reclustering_AOD.insert(input_reclustering_AOD.end(),constituents.begin(),constituents.end());
	}

	fastjet::ClusterSequence reclust_seq_AOD(input_reclustering_AOD, *fjJetDefinition_);
	
	std::vector<fastjet::PseudoJet> inclusive_jets_reclust_AOD = fastjet::sorted_by_pt(reclust_seq_AOD.inclusive_jets(ptmin));
	
	h2_nJetDiffAOD->Fill(inclusive_jets_AOD.size(), int(inclusive_jets_reclust_AOD.size()) - int(inclusive_jets_AOD.size()));
	
	size_t nJetsMin_AOD = std::min(inclusive_jets_reclust_AOD.size(), inclusive_jets_AOD.size());
	for (size_t i = 0; i < nJetsMin_AOD; i++)
          h2_JetPtRelDiffAOD->Fill(inclusive_jets_AOD[i].pt(), (inclusive_jets_reclust_AOD[i].pt() - inclusive_jets_AOD[i].pt())/inclusive_jets_AOD[i].pt());
	//---------------------------------------------------------
	// MiniAOD
	std::vector<fastjet::PseudoJet> input_particles;

	// loop over packed candidates collection
	for(std::vector<pat::PackedCandidate>::const_iterator it=packedCandidates->begin(); it!=packedCandidates->end(); ++it)
	  input_particles.push_back(fastjet::PseudoJet(it->px(),it->py(),it->pz(),it->energy()));
	
	fastjet::ClusterSequence clust_seq(input_particles, *fjJetDefinition_);
	
	std::vector<fastjet::PseudoJet> inclusive_jets = fastjet::sorted_by_pt(clust_seq.inclusive_jets(ptmin));
	
	std::vector<fastjet::PseudoJet> input_reclustering;
	
	for (size_t i = 0; i < inclusive_jets.size(); i++) {
	  std::vector<fastjet::PseudoJet> constituents = inclusive_jets[i].constituents();
	  input_reclustering.insert(input_reclustering.end(),constituents.begin(),constituents.end());
	}

	fastjet::ClusterSequence reclust_seq(input_reclustering, *fjJetDefinition_);
	
	std::vector<fastjet::PseudoJet> inclusive_jets_reclust = fastjet::sorted_by_pt(reclust_seq.inclusive_jets(ptmin));
	
	h2_nJetDiffMiniAOD->Fill(inclusive_jets.size(), int(inclusive_jets_reclust.size()) - int(inclusive_jets.size()));
	
	size_t nJetsMin_MiniAOD = std::min(inclusive_jets_reclust.size(), inclusive_jets.size());
	for (size_t i = 0; i < nJetsMin_MiniAOD; i++)
          h2_JetPtRelDiffMiniAOD->Fill(inclusive_jets[i].pt(), (inclusive_jets_reclust[i].pt() - inclusive_jets[i].pt())/inclusive_jets[i].pt());
	//---------------------------------------------------------
	// MiniAOD vs AOD
	h2_nJetDiff->Fill(inclusive_jets_AOD.size(), int(inclusive_jets.size()) - int(inclusive_jets_AOD.size()));
	
	size_t nJetsMin = std::min(inclusive_jets.size(), inclusive_jets_AOD.size());
	for (size_t i = 0; i < nJetsMin; i++)
          h2_JetPtRelDiff->Fill(inclusive_jets_AOD[i].pt(), (inclusive_jets[i].pt() - inclusive_jets_AOD[i].pt())/inclusive_jets_AOD[i].pt());
      }
      // close input file
      inFile->Close();
    }
    // break loop if maximal number of events is reached:
    // this has to be done twice to stop the file loop as well
    if(maxEvents_>0 ? ievt+1>maxEvents_ : false) break;
  }
  return 0;
}
