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
#include "HeterogeneousCore/CUDACore/interface/ScopedContext.h"
#include "CUDADataFormats/SiPixelCluster/interface/SiPixelClustersCUDA.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"

class SiPixelValidateClusterGPU : public DQMEDAnalyzer {

public:
  explicit SiPixelValidateClusterGPU(const edm::ParameterSet& iConfig);
  ~SiPixelValidateClusterGPU() override = default;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void analyze(const edm::Event& iEvent, edm::EventSetup const& iSetup) override;
  void bookHistograms(DQMStore::IBooker&, edm::Run const&, edm::EventSetup const&) override;
  //Token for the type of collection you want to read
  edm::EDGetTokenT<cms::cuda::Product<SiPixelClustersCUDA>> token_;

  uint32_t m_nClusters;

  std::string topFolderName_;
  MonitorElement* nClusters;
};

SiPixelValidateClusterGPU::SiPixelValidateClusterGPU(const edm::ParameterSet& iConfig)
    : token_(consumes<cms::cuda::Product<SiPixelClustersCUDA>>(iConfig.getParameter<edm::InputTag>("src")))
{
  //define the top folder name according to the type cms::cuda::Product<SiPixelClustersCUDA>
  topFolderName_ = "SiPixelHeterogeneous/PixelClusterGPU";
}

void SiPixelValidateClusterGPU::bookHistograms(DQMStore::IBooker& ibooker, edm::Run const& run , edm::EventSetup const& es) {
  ibooker.cd();
  ibooker.setCurrentFolder(topFolderName_);
  nClusters = ibooker.book1D("nClustersFromGPU", "Number of clusters from GPU", 100, 0, 20000);

}

void SiPixelValidateClusterGPU::analyze(const edm::Event& iEvent, edm::EventSetup const& es) {
  //get the collection
  cms::cuda::Product<SiPixelClustersCUDA> const& inputDataWrapped = iEvent.get(token_);
  cms::cuda::ScopedContextAnalyze ctx{inputDataWrapped};
  auto const& inputData = ctx.get(inputDataWrapped);

  //fill the histos
  m_nClusters = inputData.nClusters();
  if (m_nClusters == 0)
    return;

  nClusters->Fill(m_nClusters);
}

void SiPixelValidateClusterGPU::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("src", edm::InputTag("siPixelClustersCUDAPreSplitting"));
  descriptions.add("SiPixelValidateClusterGPU", desc);
}
DEFINE_FWK_MODULE(SiPixelValidateClusterGPU);
