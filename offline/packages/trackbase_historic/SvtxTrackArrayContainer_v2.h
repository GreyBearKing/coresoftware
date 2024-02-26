#ifndef SVTXTRACKARRAYCONTAINERV2_H
#define SVTXTRACKARRAYCONTAINERV2_H

#include "SvtxTrackArrayContainer.h"
#include "SvtxTrackArray.h"
#include "SvtxTrackArray_v2.h"
#include <phool/PHObject.h>

#include <climits>
#include <map>
#include <TClonesArray.h>
#include <cstdint>

class SvtxTrackArrayContainer_v2 : public SvtxTrackArrayContainer
{
    public:
 
    SvtxTrackArrayContainer_v2();
    ~SvtxTrackArrayContainer_v2() override;
    void identify(std::ostream& os = std::cout) const override;

      void Reset() override;

    size_t size() const override { return _clones->GetEntries(); }

    SvtxTrackArray* get_trackarray(int pos) override{
    return (SvtxTrackArray*) _clones->At(pos);
    }

    //    void TrackInfoContainer_v1::add_trackinfo(int, SvtxTrackInfo_v1 ) {}
    void add_trackarray(int pos, SvtxTrackArray trackinfo) override{
      new((*_clones)[pos]) SvtxTrackArray;
      trackinfo.identify();
      std::cout << "Right before ConstructedAt1" << std::endl;
      SvtxTrackArray *info = (SvtxTrackArray *)_clones->ConstructedAt(pos);
      std::cout << "Right before copyFrom1" << std::endl;
      info->CopyFrom(trackinfo);

    }

    void add_trackarray(int pos, SvtxTrackArray* trackinfo) override{
      new((*_clones)[pos]) SvtxTrackArray_v2;
      trackinfo->identify();
      std::cout << "Right before ConstructedAt2" << std::endl;
      SvtxTrackArray_v2 *info = (SvtxTrackArray_v2*)_clones->ConstructedAt(pos);
      std::cout << "Right before copyFrom2" << std::endl;
      info->CopyFrom(trackinfo);
      info->identify();
        info->set_chisq(trackinfo->get_chisq());
        std::cout << "info chisq: " << info->get_chisq() << std::endl;
    }


    protected:
    TClonesArray *_clones = nullptr;

    private:
    ClassDefOverride(SvtxTrackArrayContainer_v2, 1);
};

#endif