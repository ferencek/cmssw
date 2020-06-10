import FWCore.ParameterSet.Config as cms

from Validation.SiPixelPhase1TrackClustersV.SiPixelPhase1TrackClustersV_cfi import SiPixelPhase1TrackClustersAnalyzerV

siPixelClustersLegacyValid = SiPixelPhase1TrackClustersAnalyzerV.clone(
    clusters = "siPixelClustersPreSplitting@cpu"
)
for h in siPixelClustersLegacyValid.histograms:
    h.topFolderName = "SiPixelHeterogeneousV/PixelClustersLegacy"
