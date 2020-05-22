import FWCore.ParameterSet.Config as cms

from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer

sipixelclustervalid = DQMEDAnalyzer("SiPixelValidateCluster",
    mightGet = cms.optional.untracked.vstring,
    src = cms.InputTag("siPixelClustersPreSplitting@cpu")
)
