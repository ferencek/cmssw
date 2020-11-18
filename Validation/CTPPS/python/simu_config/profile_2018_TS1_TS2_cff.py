import FWCore.ParameterSet.Config as cms

from Validation.CTPPS.simu_config.year_2018_cff import *

profile_2018_TS1_TS2 = profile.clone()

# LHCInfo
profile_2018_TS1_TS2.ctppsLHCInfo.xangleBetaStarHistogramObject = "2018_TS1_TS2/h2_betaStar_vs_xangle"

# alignment
alignmentFile = "Validation/CTPPS/alignment/2018_TS1_TS2.xml"
profile_2018_TS1_TS2.ctppsRPAlignmentCorrectionsDataXML.MisalignedFiles = [alignmentFile]
profile_2018_TS1_TS2.ctppsRPAlignmentCorrectionsDataXML.RealFiles = [alignmentFile]

# timing resolution
profile_2018_TS1_TS2.ctppsDirectSimuData.timeResolutionDiamonds45 = "2*((x<16)*(-0.171784+0.175856*x-0.0322344*x^2+0.00231489*x^3-5.7575E-05*x^4)+(x>=16)*0.105)"
profile_2018_TS1_TS2.ctppsDirectSimuData.timeResolutionDiamonds56 = "2*((x<16)*(-0.014943+0.102806*x-0.0209404*x^2+0.00158264*x^3-4.08241E-05*x^4)+(x>=16)*0.089)"
