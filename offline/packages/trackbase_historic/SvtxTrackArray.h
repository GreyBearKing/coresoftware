#ifndef TRACKBASEHISTORIC_SVTXTRACKARRAY_H
#define TRACKBASEHISTORIC_SVTXTRACKARRAY_H

#include "SvtxTrack.h"
#include "SvtxTrackState.h"
#include "TrackSeed.h"

#include <trackbase/TrkrDefs.h>
#include <phool/PHObject.h>

#include <limits.h>
#include <iostream>
#include <map>
#include <cstddef> 
//#include <TClonesArray.h>

class SvtxTrack;

class SvtxTrackArray : public PHObject
{
 public:
  //typedef std::map<float, SvtxTrackState*> StateMap;
  //typedef StateMap::const_iterator ConstStateIter;
  //typedef StateMap::iterator StateIter;
  //typedef std::map<unsigned int, SvtxTrack*> TrackMap;
  //typedef std::map<unsigned int, SvtxTrack*>::const_iterator ConstIter;
  //typedef std::map<unsigned int, SvtxTrack*>::iterator Iter;

  


  ~SvtxTrackArray() override = default;


  void identify(std::ostream& os = std::cout) const override
  {
    os << "SvtxTrackArray base class" << std::endl;
  }
  int isValid() const override { return 0; }
  PHObject* CloneMe() const override { return nullptr; }

  //! import PHObject CopyFrom, in order to avoid clang warning
  using PHObject::CopyFrom;
  
  //! copy content from base class
  virtual void CopyFrom( const SvtxTrackArray& ) 
  {}

  //! copy content from base class
  virtual void CopyFrom( SvtxTrackArray* ) 
  {}
  
  //virtual bool empty() const { return true; }
  //virtual size_t size() const { return 0; }
  //virtual size_t count(unsigned int /*idkey*/) const { return 0; }
  //virtual void clear() {}

  //virtual const SvtxTrack* get(unsigned int /*idkey*/) const { return nullptr; }
  //virtual SvtxTrack* get(unsigned int /*idkey*/) { return nullptr; }
  //virtual SvtxTrack* insert(const SvtxTrack* /*cluster*/) { return nullptr; }
  //virtual SvtxTrack* insertWithKey(const SvtxTrack*, unsigned int) { return nullptr; }
  //virtual size_t erase(unsigned int /*idkey*/) { return 0; }
  
    

  /*
  virtual ConstIter begin() const;
  virtual ConstIter find(unsigned int idkey) const;
  virtual ConstIter end() const;

  virtual Iter begin();
  virtual Iter find(unsigned int idkey);
  virtual Iter end();
*/

//add all functions as virtual
  virtual unsigned int get_id() const  { return 0; }
  virtual void set_id(unsigned int)  {}

  virtual short int get_crossing() const  { return 0; }
  virtual void set_crossing(short int)  {}

  virtual unsigned int get_vertex_id() const  { return 0; }
  virtual void set_vertex_id(unsigned int)  {}

  virtual bool get_positive_charge() const  { return false; }
  virtual void set_positive_charge(bool)  {}

  virtual int get_charge() const  { return 0; }
  virtual void set_charge(int )  {}

  virtual float get_chisq() const  { return NAN; }
  virtual void set_chisq(float)  {}

  virtual unsigned int get_ndf() const  { return 0; }
  virtual void set_ndf(int)  {}

  //set position and momentum for track state

  virtual float get_x() const { return 0;}
  virtual void set_x(float) {}

  virtual float get_y() const { return 0;}
  virtual void set_y(float) {}

  virtual float get_z() const { return 0;}
  virtual void set_z(float) {}

  virtual float get_px() const { return 0;}
  virtual void set_px(float) {}

  virtual float get_py() const { return 0;}
  virtual void set_py(float) {}

  virtual float get_pz() const { return 0;}
  virtual void set_pz(float) {}
  //Functions for TPC Seed

  virtual bool get_does_tpc_seed_exist() const {return true;}
  virtual float tpc_seed_get_qOverR() const  {return NAN;}
  virtual float tpc_seed_get_X0() const  { return NAN; }
  virtual float tpc_seed_get_Y0() const  { return NAN; }
  virtual float tpc_seed_get_slope() const  { return NAN; }
  virtual float tpc_seed_get_Z0() const  { return NAN; }
  virtual short int tpc_seed_get_crossing() const  { return 0; }
  virtual bool get_is_tpc_helix_intersection_positive() const {return true;}
  //virtual float tpc_seed_get_R_param() const {return NAN;}

  virtual void set_does_tpc_seed_exist(const bool) {}
  virtual void tpc_seed_set_crossing(const short int)  {}
  virtual void tpc_seed_set_qOverR(const float)  {}
  virtual void tpc_seed_set_X0(const float)  {}
  virtual void tpc_seed_set_Y0(const float)  {}
  virtual void tpc_seed_set_slope(const float)  {}
  virtual void tpc_seed_set_Z0(const float)  {}
  virtual void set_is_tpc_helix_intersection_positive(const bool) {}
  //virtual void tpc_seed_set_R_param(const float) {}

  //Functions for silicon seed

  virtual bool get_does_silicon_seed_exist() const {return true;}
  virtual float silicon_seed_get_qOverR() const  {return NAN;}
  virtual float silicon_seed_get_X0() const  { return NAN; }
  virtual float silicon_seed_get_Y0() const  { return NAN; }
  virtual float silicon_seed_get_slope() const  { return NAN; }
  virtual float silicon_seed_get_Z0() const  { return NAN; }
  virtual short int silicon_seed_get_crossing() const  { return 0; }

  virtual void set_does_silicon_seed_exist(const bool) {}
  virtual void silicon_seed_set_crossing(const short int)  {}
  virtual void silicon_seed_set_qOverR(const float)  {}
  virtual void silicon_seed_set_X0(const float)  {}
  virtual void silicon_seed_set_Y0(const float)  {}
  virtual void silicon_seed_set_slope(const float)  {}
  virtual void silicon_seed_set_Z0(const float)  {}

  //make struct for cluster information

 
  //functions to set cluster structs in the array
  virtual float getPosition(int, int) const  { return NAN; }
  virtual void setPosition(int, int , float )  {}
  virtual float getLocalX(int) const  { return NAN; }
  virtual void setLocalX(int, float )  {}
  virtual float getLocalY(int) const  { return NAN; }
  virtual void setLocalY(int, float)  {}


  virtual TrkrDefs::subsurfkey getSubSurfKey(int) const {return 0;}
  virtual void setSubSurfKey(int, TrkrDefs::subsurfkey)  {}

  virtual uint8_t getSide(int) const {return 0;}
  virtual void setSide(int, uint8_t) {}
  
  virtual uint8_t getSectorId(int) const {return 0;}
  virtual void setSectorId(int, uint8_t) {}

  virtual bool getValid(int) const {return 0;}
  virtual void setValid(int, bool) {}

  //TrkrDefs::hitsetkey getHitSetKey(int struct_index) const override { return cluster_array[struct_index].m_hitsetkey; }
  //void setHitSetKey(int struct_index, TrkrDefs::hitsetkey id) override { cluster_array[struct_index].m_hitsetkey = id; }

  virtual TrkrDefs::cluskey getClusKey(int) const {return 0;}
  virtual void setClusKey(int, TrkrDefs::cluskey)  {}

  //cluster info
  virtual unsigned int getAdc(int) const  { return 0;}

  virtual void setAdc(int, unsigned int)  {}

  virtual unsigned int getMaxAdc(int) const { return 0;  }

  virtual void setMaxAdc(int, uint16_t) {}

  //
  // convenience interface
  //
  virtual float getRPhiError(int) const {return NAN;}
  virtual float getZError(int) const {return NAN;}

  virtual void setPhiError(int, float) {}
  virtual void setZError(int, float){}

  //
  virtual uint8_t getPhiSize(int) const  { return 0; }
  virtual void setPhiSize(int, uint8_t) {}

  virtual uint8_t getZSize(int) const  { return 0; }
  virtual void setZSize(int, uint8_t ) { }
 
  virtual uint8_t getOverlap(int) const{ return 0; }
  virtual void setOverlap(int, uint8_t) {}
 
  virtual uint8_t getEdge(int) const{ return 0; }
  virtual void setEdge(int, uint8_t) {}

  virtual bool empty_states() const  { return false; }
  virtual size_t size_states() const  { return 0; }
  virtual size_t count_states(float) const  { return 0; }
  virtual void clear_states() {}

  
  //virtual const SvtxTrackState* get_state(float /*pathlength*/) const { return nullptr; }
  //virtual SvtxTrackState* get_state(float /*pathlength*/) { return nullptr; }
  //virtual SvtxTrackState* insert_state(const SvtxTrackState*) { return nullptr; }
  //virtual size_t erase_state(float /*pathlength*/) { return 0; }

  //virtual ConstStateIter begin_states() const = 0;
  //virtual ConstStateIter find_state(float) const = 0;
  //virtual ConstStateIter end_states() const = 0;

  //virtual StateIter begin_states() = 0;
  //virtual StateIter find_state(float) = 0;
  //virtual StateIter end_states() = 0;

  //virtual void copy_states(SvtxTrack*){}

  //TrackMap _map;

  //save seeds and parts of cluster
  //track seeds are currently members of SvtxTrack_v4
  //TClonesArray TPCTrackSeed = new TClonesArray("TrackSeed_v1");

  //store specific information from track seeds
  //ClusterKeySet m_cluster_keys;


  //make struct for cluster information
 

  //functions to set cluster structs in the array
 
  //functions for residual cluster struct

  //functions to set cluster structs in the array
  virtual UShort_t getPositionKeyResiduals(int, int) const  { return 0; }
  virtual void setPositionKeyResiduals(int, int , UShort_t )  {}
  virtual UShort_t getLocalXKeyResiduals(int) const  { return 0; }
  virtual void setLocalXKeyResiduals(int, UShort_t )  {}
  virtual UShort_t getLocalYKeyResiduals(int) const  { return 0; }
  virtual void setLocalYKeyResiduals(int, UShort_t)  {}


  virtual TrkrDefs::subsurfkey getSubSurfKeyResiduals(int) const {return 0;}
  virtual void setSubSurfKeyResiduals(int, TrkrDefs::subsurfkey)  {}

  virtual uint8_t getSideResiduals(int) const {return 0;}
  virtual void setSideResiduals(int, uint8_t) {}
  
  virtual uint8_t getSectorIdResiduals(int) const {return 0;}
  virtual void setSectorIdResiduals(int, uint8_t) {}

  virtual bool getValidResiduals(int) const {return 0;}
  virtual void setValidResiduals(int, bool) {}

  //TrkrDefs::hitsetkey getHitSetKey(int struct_index) const override { return cluster_array[struct_index].m_hitsetkey; }
  //void setHitSetKey(int struct_index, TrkrDefs::hitsetkey id) override { cluster_array[struct_index].m_hitsetkey = id; }

  //cluster info
  virtual unsigned int getAdcResiduals(int) const  { return 0;}

  virtual void setAdcResiduals(int, unsigned int)  {}

  virtual unsigned int getMaxAdcResiduals(int) const { return 0;  }

  virtual void setMaxAdcResiduals(int, uint16_t) {}

  //
  // convenience interface
  //
  virtual float getRPhiErrorResiduals(int) const {return NAN;}
  virtual float getZErrorResiduals(int) const {return NAN;}

  virtual void setPhiErrorResiduals(int, float) {}
  virtual void setZErrorResiduals(int, float){}

  //
  virtual uint8_t getPhiSizeResiduals(int) const  { return 0; }
  virtual void setPhiSizeResiduals(int, uint8_t) {}

  virtual uint8_t getZSizeResiduals(int) const  { return 0; }
  virtual void setZSizeResiduals(int, uint8_t ) { }
 
  virtual uint8_t getOverlapResiduals(int) const{ return 0; }
  virtual void setOverlapResiduals(int, uint8_t) {}
 
  virtual uint8_t getEdgeResiduals(int) const{ return 0; }
  virtual void setEdgeResiduals(int, uint8_t) {}



 private:
  //SvtxTrackArray() {}

  ClassDefOverride(SvtxTrackArray, 1);
  
};

#endif
