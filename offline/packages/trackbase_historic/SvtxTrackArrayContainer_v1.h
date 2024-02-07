#ifndef SVTXTRACKARRAYCONTAINERV1_H
#define SVTXTRACKARRAYCONTAINERV1_H

#include "SvtxTrackArrayContainer.h"
#include "SvtxTrackArray.h"
#include "SvtxTrackArray_v1.h"
#include <phool/PHObject.h>

#include <climits>
#include <map>
#include <TClonesArray.h>
#include <cstdint>

class SvtxTrackArrayContainer_v1 : public SvtxTrackArrayContainer
{
    public:
 
    SvtxTrackArrayContainer_v1();
    ~SvtxTrackArrayContainer_v1() override;
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
      new((*_clones)[pos]) SvtxTrackArray_v1;
      trackinfo->identify();
      std::cout << "Right before ConstructedAt2" << std::endl;
      SvtxTrackArray_v1 *info = (SvtxTrackArray_v1*)_clones->ConstructedAt(pos);
      std::cout << "Right before copyFrom2" << std::endl;
      info->CopyFrom(trackinfo);
      info->identify();
        info->set_chisq(trackinfo->get_chisq());
        std::cout << "info chisq: " << info->get_chisq() << std::endl;
    }


    protected:
    TClonesArray *_clones = nullptr;

    private:
    ClassDefOverride(SvtxTrackArrayContainer_v1, 1);
};

#endif