import FWCore.ParameterSet.Config as cms

from Validation.SiPixelPhase1TrackClustersV.SiPixelPhase1TrackClustersV_cfi import SiPixelPhase1TrackClustersAnalyzerV

siPixelClustersLegacyFromSoAValid = SiPixelPhase1TrackClustersAnalyzerV.clone(
    clusters = "siPixelDigisClustersPreSplitting"
)
for h in siPixelClustersLegacyFromSoAValid.histograms:
    h.topFolderName = "SiPixelHeterogeneousV/PixelClustersLegacyFromSoA"
