#include <TH1F.h>
#include <TROOT.h>
#include <TFile.h>
#include <TSystem.h>

#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

#include "DataFormats/FWLite/interface/InputSource.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

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
  fwlite::InputSource inputHandler_(process);


  // now get each parameter
  const edm::ParameterSet& ana = process.getParameter<edm::ParameterSet>("fastjetAnalyzer");
  edm::InputTag candidates_( ana.getParameter<edm::InputTag>("candidates") );
  double rParam_( ana.getParameter<double>("rParam") );
  std::string jetAlgorithm_( ana.getParameter<std::string>("jetAlgorithm") );

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
  for(unsigned int iFile=0; iFile<inputHandler_.files().size(); ++iFile){
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
	
	// Handle to the candidates collection
	edm::Handle<std::vector<pat::PackedCandidate> > candidates;
	event.getByLabel(candidates_, candidates);
	
	std::vector<fastjet::PseudoJet> input_particles;
	
	// loop candidates collection
	for(std::vector<pat::PackedCandidate>::const_iterator it=candidates->begin(); it!=candidates->end(); ++it){
	  input_particles.push_back(fastjet::PseudoJet(it->px(),it->py(),it->pz(),it->energy()));
	  //printf("%15.20e\t %15.20e\t %15.20e\t %15.20e\n",it->px(),it->py(),it->pz(),it->energy());
	}
	
	std::cout << "Total number of input particles:  " << input_particles.size() << std::endl;
	
	fastjet::ClusterSequence clust_seq(input_particles, *fjJetDefinition_);
	
	double ptmin = 0.;
	std::vector<fastjet::PseudoJet> inclusive_jets = fastjet::sorted_by_pt(clust_seq.inclusive_jets(ptmin));
	
	std::vector<fastjet::PseudoJet> input_reclustering;
	
	std::cout << "Ran " << fjJetDefinition_->description() << std::endl;

	// label the columns
	printf("%5s %15s %15s %15s\n","jet #", "rapidity", "phi", "pt");

	// print out the details for each jet
	for (unsigned int i = 0; i < inclusive_jets.size(); i++) {
	  printf("%5u %15.8f %15.8f %15.8f\n",
		 i, inclusive_jets[i].rap(), inclusive_jets[i].phi(),
		 inclusive_jets[i].perp());

	  std::vector<fastjet::PseudoJet> constituents = inclusive_jets[i].constituents();
	  input_reclustering.insert(input_reclustering.end(),constituents.begin(),constituents.end());
	}
	
	std::cout << "Total number of input particles for reclustering:  " << input_reclustering.size() << std::endl;

	fastjet::ClusterSequence reclust_seq(input_reclustering, *fjJetDefinition_);
	
	std::vector<fastjet::PseudoJet> inclusive_jets_reclust = fastjet::sorted_by_pt(reclust_seq.inclusive_jets(ptmin));

	// label the columns
	printf("%5s %15s %15s %15s\n","jet #", "rapidity", "phi", "pt");

	// print out the details for each jet
	for (unsigned int i = 0; i < inclusive_jets_reclust.size(); i++) {
	  printf("%5u %15.8f %15.8f %15.8f\n",
		 i, inclusive_jets_reclust[i].rap(), inclusive_jets_reclust[i].phi(),
		 inclusive_jets_reclust[i].perp());
	}
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
