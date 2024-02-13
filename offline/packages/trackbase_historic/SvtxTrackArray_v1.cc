#include "SvtxTrackArray_v1.h"
#include "SvtxTrackState.h"
#include "SvtxTrackState_v1.h"

#include <trackbase/TrkrDefs.h>  // for cluskey

#include <phool/PHObject.h>      // for PHObject

#include <algorithm>
#include <vector>

/*
namespace 
{
  SvtxTrackArray::SvtxTrackArray;
}
*/
/*
void SvtxTrackArray_v1::Reset()
{
    TrackArray->Clear();
    TrackArrayKeys.clear(); 
}
*/
SvtxTrackArray_v1::SvtxTrackArray_v1(){
// always include the pca point
  //_states.insert( std::make_pair(0, new SvtxTrackState_v1(0)));
}

SvtxTrackArray_v1::SvtxTrackArray_v1(const SvtxTrackArray& source)
{ SvtxTrackArray_v1::CopyFrom( source ); }

// have to suppress uninitMenberVar from cppcheck since it triggers many false positive
// cppcheck-suppress uninitMemberVar
SvtxTrackArray_v1::SvtxTrackArray_v1(const SvtxTrackArray_v1& source)
{ SvtxTrackArray_v1::CopyFrom( source ); }

SvtxTrackArray_v1& SvtxTrackArray_v1::operator=(const SvtxTrackArray_v1& source)
{ CopyFrom( source ); return *this; }

//IMPROVE
void SvtxTrackArray_v1::CopyFrom( const SvtxTrackArray& source )
{
  // do nothing if copying onto oneself
  if( this == &source ) return;
  
  // parent class method
  //SvtxTrackArray::CopyFrom( source );
  
  //_tpc_seed = source.get_tpc_seed();
  //_silicon_seed = source.get_silicon_seed();
  set_id(source.get_id());
  set_vertex_id(source.get_vertex_id());
  set_positive_charge(source.get_positive_charge());
  set_chisq(source.get_chisq());
  set_ndf(source.get_ndf());
  set_crossing(source.get_crossing());

  set_x(source.get_z());
  set_y(source.get_z());
  set_z(source.get_z());

  set_px(source.get_pz());
  set_py(source.get_pz());
  set_pz(source.get_pz());
  
  //copy over trackseeds

  set_does_silicon_seed_exist(source.get_does_silicon_seed_exist());
  silicon_seed_set_crossing(source.silicon_seed_get_crossing());
  silicon_seed_set_qOverR(source.silicon_seed_get_qOverR());
  silicon_seed_set_X0(source.silicon_seed_get_X0());
  silicon_seed_set_Y0(source.silicon_seed_get_Y0());
  silicon_seed_set_Z0(source.silicon_seed_get_Z0());
  silicon_seed_set_slope(source.silicon_seed_get_slope());

  set_does_tpc_seed_exist(source.get_does_tpc_seed_exist());
  tpc_seed_set_crossing(source.tpc_seed_get_crossing());
  tpc_seed_set_qOverR(source.tpc_seed_get_qOverR());
  tpc_seed_set_X0(source.tpc_seed_get_X0());
  tpc_seed_set_Y0(source.tpc_seed_get_Y0());
  tpc_seed_set_Z0(source.tpc_seed_get_Z0());
  tpc_seed_set_slope(source.tpc_seed_get_slope());

    //copy clusters
  for(int i = 0; i<59; i++){
    //check if source has valid cluster
    if(source.getValid(i)){
        //setClusKey(i, source.getClusKey(i));
        setValid(i, source.getValid(i));
        setLocalX(i, source.getLocalX(i));
        setLocalY(i, source.getLocalY(i));
        setSide(i, source.getSide(i));
        setSectorId(i, source.getSectorId(i));
        setAdc(i, source.getAdc(i));
        setMaxAdc(i, source.getMaxAdc(i));
        setPhiError(i, source.getRPhiError(i));
        setZError(i, source.getZError(i));
        setPhiSize(i, source.getPhiSize(i));
        setZSize(i, source.getZSize(i));
        setOverlap(i, source.getOverlap(i));
        setEdge(i, source.getEdge(i));
        setSubSurfKey(i, source.getSubSurfKey(i));
    }
  }

}

//SvtxTrackArray_v1::~SvtxTrackArray_v1() {clear_states();}
SvtxTrackArray_v1::~SvtxTrackArray_v1() {}

void SvtxTrackArray_v1::identify(std::ostream& os) const
{
    os << "SvtxTrackArray_v1 Object " << std::endl;
    //Can print more information here
}
/*
void SvtxTrackArray_v1::clear_states()
{
  for( const auto& pair:_states )
  { delete pair.second; }
  
  _states.clear();
}
*/
int SvtxTrackArray_v1::isValid() const
{
  return 1;
}

/*
const SvtxTrackState* SvtxTrackArray_v1::get_state(float pathlength) const
{
  const auto iter = _states.find(pathlength);
  return (iter == _states.end()) ? nullptr:iter->second;
}

SvtxTrackState* SvtxTrackArray_v1::get_state(float pathlength)
{
  const auto iter = _states.find(pathlength);
  return (iter == _states.end()) ? nullptr:iter->second;
}

SvtxTrackState* SvtxTrackArray_v1::insert_state(const SvtxTrackState* state)
{
  // find closest iterator
  const auto pathlength = state->get_pathlength();
  auto iterator = _states.lower_bound( pathlength );
  if( iterator == _states.end() || pathlength < iterator->first )
  {
    // pathlength not found. Make a copy and insert
    const auto copy =  static_cast<SvtxTrackState*> (state->CloneMe());
    iterator = _states.insert(iterator, std::make_pair( pathlength, copy ));
  }

  // return matching state
  return iterator->second;
}

size_t SvtxTrackArray_v1::erase_state(float pathlength)
{
  StateIter iter = _states.find(pathlength);
  if (iter == _states.end()) return _states.size();

  delete iter->second;
  _states.erase(iter);
  return _states.size();
}
*/


/*
void SvtxTrackArray_v1::addTrack(SvtxTrack_v4 &track, unsigned int key, int index){
    //SvtxTrack_v4* tmpTrack = TrackArray->ConstructedAt(index);
    TrackArray[index]= new SvtxTrack_v4(track);
    if(TrackArrayKeys.size()<index){
        TrackArrayKeys.resize(index);
    }
    TrackArrayKeys[index]=(key);
}

SvtxTrack_v4* SvtxTrackArray_v1::findTrack(unsigned int key){
    for(int i = 0; i < TrackArrayKeys.size(); i++){
        if(TrackArrayKeys[i]==key){
            return &TrackArray[i]; //search for TrackArray
        }
    }
    return nullptr;
}
*/