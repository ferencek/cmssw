import FWCore.ParameterSet.Config as cms

from DQM.SiPixelPhase1Common.SiPixelPhase1Clusters_cfi import SiPixelPhase1ClustersAnalyzer

siPixelClustersLegacyFromSoAMechanicalViewValid = SiPixelPhase1ClustersAnalyzer.clone(
    pixelSrc = "siPixelDigisClustersPreSplitting"
)
for h in siPixelClustersLegacyFromSoAMechanicalViewValid.histograms:
    if h.topFolderName == "PixelPhase1/Phase1_MechanicalView":
        h.topFolderName = "SiPixelHeterogeneousV/PixelClustersLegacyFromSoA_MechanicalView"
