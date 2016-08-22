// -*- C++ -*-
//
// Package:    RecoLocalTracker/SiPixelClusterizer
// Class:      SiPixelClusterMerger
//
/**\class SiPixelClusterMerger SiPixelClusterMerger.cc RecoLocalTracker/SiPixelClusterizer/plugins/SiPixelClusterMerger.cc
  *
  * Description: EDProducer that merges pairs of nearby clusters
  *
  * Implementation:
  *    Pairs of pixel cluster that are closer to each other than a configurable maximum distance are merged
  *    together. The clostest pairs are merged first and each cluster is allowed to be merged in only one pair.
  *    Original unmerged clusters are also kept in the output collection.
  */
//
// Original Author:  Dinko Ferencek
//         Created:  Fri, 12 Aug 2016 17:36:47 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "PixelClusterizerBase.h"

#include <map>

//
// class declaration
//

class SiPixelClusterMerger : public edm::stream::EDProducer<> {
   public:
      explicit SiPixelClusterMerger(const edm::ParameterSet&);
      ~SiPixelClusterMerger();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginStream(edm::StreamID) override;
      virtual void produce(edm::Event&, const edm::EventSetup&) override;
      virtual void endStream() override;

      double distance(const SiPixelCluster & cluster1, const SiPixelCluster & cluster2);
      void merge(const edmNew::DetSetVector<SiPixelCluster> & input, edmNew::DetSetVector<SiPixelCluster> & output);

      // ----------member data ---------------------------
      const edm::EDGetTokenT<edmNew::DetSetVector<SiPixelCluster> > src_;
      const double maxDistance_;
      const bool keepMerged_;
};

//
// constants, enums and typedefs
//


//
// static data member definitions
//

//
// constructors and destructor
//
SiPixelClusterMerger::SiPixelClusterMerger(const edm::ParameterSet& iConfig) :
  src_( consumes<edmNew::DetSetVector<SiPixelCluster> >(iConfig.getParameter<edm::InputTag>("src")) ),
  maxDistance_( iConfig.getParameter<double>("maxDistance") ),
  keepMerged_( iConfig.getParameter<bool>("keepMerged") )
{
    produces<edmNew::DetSetVector<SiPixelCluster> >();
}


SiPixelClusterMerger::~SiPixelClusterMerger()
{

   // do anything here that needs to be done at destruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
SiPixelClusterMerger::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   // get input pixel clusters
   edm::Handle<edmNew::DetSetVector<SiPixelCluster> > input;
   iEvent.getByToken(src_, input);

   // create the final output collection
   std::auto_ptr<edmNew::DetSetVector<SiPixelCluster> > output( new edmNew::DetSetVector<SiPixelCluster>() );

   // perform pixel cluster merging
   merge(*input, *output);

   // put the output in the event
   output->shrink_to_fit();
   iEvent.put( output );
}

// ------------ method that computes distance betweeen a pair of pixel clusters  ------------
double
SiPixelClusterMerger::distance(const SiPixelCluster & cluster1, const SiPixelCluster & cluster2)
{
   // TO BE IMPLEMENTED
   double distance = 0;

   return distance;
}

// ------------ method that implements the cluster merging algorithm  ------------
void
SiPixelClusterMerger::merge(const edmNew::DetSetVector<SiPixelCluster> & input, edmNew::DetSetVector<SiPixelCluster> & output)
{
   // loop over DetUnits
   edmNew::DetSetVector<SiPixelCluster>::const_iterator DSViter = input.begin();
   for( ; DSViter != input.end(); ++DSViter)
   {
     // map to store distances for all cluster pairs
     std::multimap<double, std::pair<size_t, size_t> > distances;

     // fast filler for clusters
     edmNew::DetSetVector<SiPixelCluster>::FastFiller spc(output, DSViter->detId());

     edmNew::DetSet<SiPixelCluster>::const_iterator iter1, iter2;
     const edmNew::DetSet<SiPixelCluster>::const_iterator beginIter = DSViter->begin();
     const edmNew::DetSet<SiPixelCluster>::const_iterator endIter = DSViter->end();
     // loop over clusters and compute distances for all cluster pairs
     for(iter1 = beginIter; iter1 != endIter; ++iter1)
     {
       for(iter2 = (iter1 + 1); iter2 != endIter; ++iter2)
       {
         // insert the computed distance and cluster indices
         distances.insert( std::make_pair(distance(*iter1, *iter2), std::make_pair(iter1 - beginIter, iter2 - beginIter)) );
       }
     }

     // flags to indicate whether a cluster was already merged
     std::vector<bool> isMerged(DSViter->size(),false);

     // merge cluster pairs that are closer that the maximum distance
     for(std::multimap<double, std::pair<size_t,size_t> >::const_iterator iter =
         distances.begin(); iter != distances.end(); ++iter)
     {
       // with the map keys in ascending order, break the loop once the maximum distance is reached
       if( iter->first > maxDistance_ )
         break;

       // fetch the the clusters from the pair
       const size_t clusterIdx1 = iter->second.first;
       const size_t clusterIdx2 = iter->second.second;
       const SiPixelCluster & cluster1 = *(beginIter + clusterIdx1);
       const SiPixelCluster & cluster2 = *(beginIter + clusterIdx2);

       // check if the two clusters are still available for merging
       if( isMerged.at(clusterIdx1) || isMerged.at(clusterIdx2) ) continue;

       // mark the two clusters as no longer available for clustering
       isMerged.at(clusterIdx1) = true;
       isMerged.at(clusterIdx2) = true;

       PixelClusterizerBase::AccretionCluster aCluster;

       // collect individual pixels from the 1st cluster
       for(int i = 0; i < cluster1.size(); ++i)
       {
         const SiPixelCluster::Pixel pixel = cluster1.pixel(i);

         aCluster.add(SiPixelCluster::PixelPos(pixel.x,pixel.y), pixel.adc);
       }
       // collect individual pixels from the 2nd cluster
       for(int i = 0; i < cluster2.size(); ++i)
       {
         const SiPixelCluster::Pixel pixel = cluster2.pixel(i);

         aCluster.add(SiPixelCluster::PixelPos(pixel.x,pixel.y), pixel.adc);
       }

       // construct a merged cluster
       SiPixelCluster cluster(aCluster.isize, aCluster.adc, aCluster.x, aCluster.y, aCluster.xmin, aCluster.ymin);

       // add the newly created merged cluster to the output collection
       spc.push_back( cluster );
     }

     // also want to keep the original unmerged clusters
     for(edmNew::DetSet<SiPixelCluster>::const_iterator iter = beginIter; iter != endIter; ++iter)
     {
       // check if the original clusters that got merged should also be kept
       if( keepMerged_ )
         spc.push_back( *iter );
       else
       {
         if( !isMerged.at(iter - beginIter) )
           spc.push_back( *iter );
       }
     }
   }
}

// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
SiPixelClusterMerger::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
SiPixelClusterMerger::endStream() {
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
SiPixelClusterMerger::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(SiPixelClusterMerger);
