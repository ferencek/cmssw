import FWCore.ParameterSet.Config as cms

process = cms.PSet()

process.fwliteInput = cms.PSet(
    fileNames   = cms.vstring('file:/uscms_data/d2/ferencek/BTV/CMSSW_7_4_4/src/MiniAOD_BeforeDegridification.root'), ## mandatory
    maxEvents   = cms.int32(-1),                              ## optional
    outputEvery = cms.uint32(100),                            ## optional
)

process.fwliteOutput = cms.PSet(
    #fileName  = cms.string('AK8_resclusteringTest_AfterFix.root'),   ## mandatory
    #fileName  = cms.string('CA8_resclusteringTest_AfterFix.root'),   ## mandatory
    #fileName  = cms.string('AK8_resclusteringTest_BeforeFix.root'),  ## mandatory
    fileName  = cms.string('CA8_resclusteringTest_BeforeFix.root'),  ## mandatory
)

process.fastjetAnalyzer = cms.PSet(
    ## input specific for this analyzer
    pfCandidates     = cms.InputTag('particleFlow'),
    packedCandidates = cms.InputTag('packedPFCandidates'),
    rParam           = cms.double(0.8),
    #jetAlgorithm     = cms.string('AntiKt')
    jetAlgorithm     = cms.string('CambridgeAachen')
)
