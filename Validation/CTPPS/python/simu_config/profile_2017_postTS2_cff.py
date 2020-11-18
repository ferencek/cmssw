import FWCore.ParameterSet.Config as cms

from Validation.CTPPS.simu_config.year_2017_cff import profile

profile_2017_postTS2 = profile.clone()

# LHCInfo
profile_2017_postTS2.ctppsLHCInfo.xangleBetaStarHistogramObject = "2017_postTS2/h2_betaStar_vs_xangle"

# alignment
alignmentFile = "Validation/CTPPS/alignment/2017_postTS2.xml"
profile_2017_postTS2.ctppsRPAlignmentCorrectionsDataXML.MisalignedFiles = [alignmentFile]
profile_2017_postTS2.ctppsRPAlignmentCorrectionsDataXML.RealFiles = [alignmentFile]

# aperture cuts
profile_2017_postTS2.ctppsDirectSimuData.useEmpiricalApertures = True

profile_2017_postTS2.ctppsDirectSimuData.empiricalAperture45 = "-(8.92079E-07*[xangle]-0.000150214)+(([xi]<(0.000278622*[xangle]+0.0964383))*-(3.9541e-05*[xangle]-0.0115104)+([xi]>=(0.000278622*[xangle]+0.0964383))*-(0.000108249*[xangle]-0.0249303))*([xi]-(0.000278622*[xangle]+0.0964383))"
profile_2017_postTS2.ctppsDirectSimuData.empiricalAperture56 = "4.56961E-05+(([xi]<(0.00075625*[xangle]+0.0643361))*-(3.01107e-05*[xangle]-0.00985126)+([xi]>=(0.00075625*[xangle]+0.0643361))*-(8.95437e-05*[xangle]-0.0169474))*([xi]-(0.00075625*[xangle]+0.0643361))"

# timing resolution
profile_2017_postTS2.ctppsDirectSimuData.timeResolutionDiamonds45 = "2*(0.0152613+0.0498784*x-0.00824168*x^2+0.000599844*x^3-1.5923E-05*x^4)"
profile_2017_postTS2.ctppsDirectSimuData.timeResolutionDiamonds56 = "2*(-0.00458856+0.0522619*x-0.00806666*x^2+0.000558331*x^3-1.42165E-05*x^4)"
