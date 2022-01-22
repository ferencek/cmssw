#ifndef SIPIXELROCSTATUS_h
#define SIPIXELROCSTATUS_h

// ----------------------------------------------------------------------
class SiPixelRocStatus {
public:
  SiPixelRocStatus();
  ~SiPixelRocStatus();

  void fillDIGI();
  void fillFEDerror25();

  void updateDIGI(unsigned int hits);
  void updateFEDerror25(bool fedError25);

  // occpancy
  const unsigned int digiOccROC() const;
  // FEDerror25 for stuckTBM
  const bool isFEDerror25() const;

private:
  unsigned int fDC_;
  bool isFEDerror25_;
};

#endif
