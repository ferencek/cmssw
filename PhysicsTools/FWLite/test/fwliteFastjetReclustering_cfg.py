import FWCore.ParameterSet.Config as cms

process = cms.PSet()

process.fwliteInput = cms.PSet(
    fileNames   = cms.vstring('root://cmsxrootd-site.fnal.gov//store/relval/CMSSW_7_4_1/RelValTTbar_13/MINIAODSIM/PU50ns_MCRUN2_74_V8_gensim71X-v1/00000/B0722DC0-8AEC-E411-8AB5-0025905B85D6.root'), ## mandatory
    maxEvents   = cms.int32(1),                             ## optional
    outputEvery = cms.uint32(1),                            ## optional
)

process.fastjetAnalyzer = cms.PSet(
    ## input specific for this analyzer
    candidates   = cms.InputTag('packedPFCandidates'),
    rParam       = cms.double(1.5),
    jetAlgorithm = cms.string('CambridgeAachen')
)
