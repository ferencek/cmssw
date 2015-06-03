//----------------------------------------------------------------------
/// example program:
///
/// run it with    : ./FastjetReclustering < single-event.dat
///
/// Source code: FastjetReclustering.cc
//----------------------------------------------------------------------

#include "fastjet/ClusterSequence.hh"
#include <iostream> // needed for io
#include <cstdio>   // needed for io

using namespace std;

/// an example program showing how to use fastjet
int main(){
  
  // read in input particles
  //----------------------------------------------------------
  vector<fastjet::PseudoJet> input_particles;
  
  double px, py , pz, E;
  while (cin >> px >> py >> pz >> E) {
    // create a fastjet::PseudoJet with these components and put it onto
    // back of the input_particles vector
    input_particles.push_back(fastjet::PseudoJet(px,py,pz,E)); 
  }
  
  cout << "Total number of input particles:  " << input_particles.size() << endl;

  // create a jet definition: 
  // a jet algorithm with a given radius parameter
  //----------------------------------------------------------
  double R = 1.5;
  fastjet::JetDefinition jet_def(fastjet::cambridge_algorithm, R);


  // run the jet clustering with the above jet definition
  //----------------------------------------------------------
  fastjet::ClusterSequence clust_seq(input_particles, jet_def);


  // get the resulting jets ordered in pt
  //----------------------------------------------------------
  double ptmin = 0.0;
  vector<fastjet::PseudoJet> inclusive_jets = sorted_by_pt(clust_seq.inclusive_jets(ptmin));


  // tell the user what was done
  //  - the description of the algorithm used
  //  - extract the inclusive jets with pt > 5 GeV
  //    show the output as 
  //      {index, rap, phi, pt}
  //----------------------------------------------------------
  cout << "Ran " << jet_def.description() << endl;

  vector<fastjet::PseudoJet> input_reclustering;

  // label the columns
  printf("%5s %15s %15s %15s\n","jet #", "rapidity", "phi", "pt");
 
  // print out the details for each jet
  for (unsigned int i = 0; i < inclusive_jets.size(); i++) {
    printf("%5u %15.8f %15.8f %15.8f\n",
	   i, inclusive_jets[i].rap(), inclusive_jets[i].phi(),
	   inclusive_jets[i].perp());
    
    vector<fastjet::PseudoJet> constituents = inclusive_jets[i].constituents();
    input_reclustering.insert(input_reclustering.end(),constituents.begin(),constituents.end());
  }

  cout << "Total number of input particles for reclustering:  " << input_reclustering.size() << endl;

  fastjet::ClusterSequence reclust_seq(input_reclustering, jet_def);

  vector<fastjet::PseudoJet> inclusive_jets_reclust = sorted_by_pt(reclust_seq.inclusive_jets(ptmin));

  // label the columns
  printf("%5s %15s %15s %15s\n","jet #", "rapidity", "phi", "pt");

  // print out the details for each jet
  for (unsigned int i = 0; i < inclusive_jets_reclust.size(); i++) {
    printf("%5u %15.8f %15.8f %15.8f\n",
	   i, inclusive_jets_reclust[i].rap(), inclusive_jets_reclust[i].phi(),
	   inclusive_jets_reclust[i].perp());
  }
	
  return 0;
}
