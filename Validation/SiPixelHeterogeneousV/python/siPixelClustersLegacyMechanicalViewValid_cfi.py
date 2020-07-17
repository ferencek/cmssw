import FWCore.ParameterSet.Config as cms

from DQM.SiPixelPhase1Common.SiPixelPhase1Clusters_cfi import SiPixelPhase1ClustersAnalyzer

siPixelClustersLegacyMechanicalViewValid = SiPixelPhase1ClustersAnalyzer.clone(
    pixelSrc = "siPixelClustersPreSplitting@cpu"
)
for h in siPixelClustersLegacyMechanicalViewValid.histograms:
    if h.topFolderName == "PixelPhase1/Phase1_MechanicalView":
        h.topFolderName = "SiPixelHeterogeneousV/PixelClustersLegacy_MechanicalView"
