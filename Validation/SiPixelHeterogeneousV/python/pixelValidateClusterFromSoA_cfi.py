import FWCore.ParameterSet.Config as cms

from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer

sipixelclusterfromsoavalid = DQMEDAnalyzer("SiPixelValidateClusterFromSoA",
    mightGet = cms.optional.untracked.vstring,
    src = cms.InputTag("siPixelDigisClustersPreSplitting")
)
