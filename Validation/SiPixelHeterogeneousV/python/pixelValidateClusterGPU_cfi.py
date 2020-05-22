import FWCore.ParameterSet.Config as cms

from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer

sipixelclustergpuvalid = DQMEDAnalyzer("SiPixelValidateClusterGPU",
    mightGet = cms.optional.untracked.vstring,
    src = cms.InputTag("siPixelClustersCUDAPreSplitting")
)
