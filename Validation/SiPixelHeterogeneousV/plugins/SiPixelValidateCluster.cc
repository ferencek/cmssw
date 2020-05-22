#include <cuda_runtime.h>
#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"

class SiPixelValidateCluster : public DQMEDAnalyzer {

public:
  explicit SiPixelValidateCluster(const edm::ParameterSet& iConfig);
  ~SiPixelValidateCluster() override = default;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void analyze(const edm::Event& iEvent, edm::EventSetup const& iSetup) override;
  void bookHistograms(DQMStore::IBooker&, edm::Run const&, edm::EventSetup const&) override;
  //Token for the type of collection you want to read
  edm::EDGetTokenT<SiPixelClusterCollectionNew> token_;

  uint32_t m_nClusters;

  std::string topFolderName_;
  MonitorElement* nClusters;
};

SiPixelValidateCluster::SiPixelValidateCluster(const edm::ParameterSet& iConfig)
    : token_(consumes<SiPixelClusterCollectionNew>(iConfig.getParameter<edm::InputTag>("src")))
{
  //define the top folder name according to the type SiPixelClusterCollectionNew
  topFolderName_ = "SiPixelHeterogeneous/PixelCluster";
}

void SiPixelValidateCluster::bookHistograms(DQMStore::IBooker& ibooker, edm::Run const& run , edm::EventSetup const& es) {
  ibooker.cd();
  ibooker.setCurrentFolder(topFolderName_);
  nClusters = ibooker.book1D("nClusters", "Number of clusters", 1000, 0, 1e6);

}

void SiPixelValidateCluster::analyze(const edm::Event& iEvent, edm::EventSetup const& es) {
  //get the collection
  const auto& clusters = iEvent.get(token_);

  // loop over all DetSets and count the number of clusters
  for(auto const& c: clusters) {
    m_nClusters += c.size();
  }
//   m_nClusters = clusters.size();
  if (m_nClusters == 0)
    return;

  //fill the histos
  nClusters->Fill(m_nClusters);
}

void SiPixelValidateCluster::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("src", edm::InputTag("siPixelClustersPreSplitting"));
  descriptions.add("SiPixelValidateCluster", desc);
}
DEFINE_FWK_MODULE(SiPixelValidateCluster);
