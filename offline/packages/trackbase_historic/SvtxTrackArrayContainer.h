#ifndef SVTXTRACKARRAYCONTAINER_H
#define SVTXTRACKARRAYCONTAINER_H

#include "SvtxTrackArray.h"
//#include "SvtxTrackInfo_v1.h"

#include <phool/PHObject.h>

#include <climits>
#include <map>

class SvtxTrackArrayContainer : public PHObject
{
 public:

  SvtxTrackArrayContainer() = default;
  ~SvtxTrackArrayContainer() override = default;
  void identify(std::ostream& os = std::cout) const override{
    os << "SvtxTrackArrayContainer base class" << std::endl;
  }

  virtual void Reset() override {}
  virtual SvtxTrackArray* get_trackarray(int /*index*/) { return nullptr; }
  virtual void add_trackarray(int, SvtxTrackArray) {}
  virtual void add_trackarray(int, SvtxTrackArray*) {}
  //virtual TrackInfo* get_tower_at_key(int /*key*/) { return nullptr; }
  virtual size_t size() const { return 0; }


 private:
  ClassDefOverride(SvtxTrackArrayContainer, 1);
};

#endif