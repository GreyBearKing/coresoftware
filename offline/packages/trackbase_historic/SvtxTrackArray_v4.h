#ifndef TRACKBASEHISTORIC_SVTXTRACKARRAYV4_H
#define TRACKBASEHISTORIC_SVTXTRACKARRAYV4_H

#include "SvtxTrack.h"
#include "SvtxTrackArray.h"
#include "TrackSeed.h"
#include "cluster_with_sector_side.h"
//#include "cluster_with_sector_side_residuals.h"
#include "cluster_with_sector_side_residuals_minimum.h"

#include <cstddef>        // for size_t
#include <iostream>        // for cout, ostream
#include <vector> 

class PHObject;

class SvtxTrackArray_v4 : public SvtxTrackArray
{
 public:
  SvtxTrackArray_v4();
  SvtxTrackArray_v4(const SvtxTrackArray& trackarray);
  SvtxTrackArray_v4(const SvtxTrackArray_v4& trackarray);
  SvtxTrackArray_v4& operator=(const SvtxTrackArray_v4& trackarray);
  ~SvtxTrackArray_v4() override;

  void identify(std::ostream& os = std::cout) const override;
  void Reset() override { *this = SvtxTrackArray_v4();}
  int isValid() const override;
  PHObject* CloneMe() const override { return new SvtxTrackArray_v4(*this); }


  void CopyFrom( const SvtxTrackArray& ) override;
  void CopyFrom( SvtxTrackArray* source ) override
  { CopyFrom( *source ); }
  //bool empty() const override{ return _map.empty(); }
  //size_t size() const override { return _map.size(); }
  //size_t count(unsigned int idkey) const override{ return _map.count(idkey); }
  //void clear() override { Reset(); }

  //const SvtxTrack* get(unsigned int idkey) const override;
  //SvtxTrack* get(unsigned int idkey) override;
  //SvtxTrack* insert(const SvtxTrack* track) override;
  //size_t erase(unsigned int idkey) override;

/*
  ConstIter begin() const override { return _map.begin(); }
  ConstIter find(unsigned int idkey) const override { return _map.find(idkey); }
  ConstIter end() const override { return _map.end(); }

  Iter begin() override { return _map.begin(); }
  Iter find(unsigned int idkey) override { return _map.find(idkey); }
  Iter end() override { return _map.end(); }
*/

  //basic track operators
  /*
  unsigned int get_id() const override { return _track_id; }
  void set_id(unsigned int id) override { _track_id = id; }

  short int get_crossing() const override { return _track_crossing; }
  void set_crossing(short int cross) override { _track_crossing = cross; }

  unsigned int get_vertex_id() const override { return _vertex_id; }
  void set_vertex_id(unsigned int id) override { _vertex_id = id; }

  bool get_positive_charge() const override { return _is_positive_charge; }
  void set_positive_charge(bool ispos) override { _is_positive_charge = ispos; }

  int get_charge() const override { return (get_positive_charge()) ? 1 : -1; }
  void set_charge(int charge) override { (charge > 0) ? set_positive_charge(true) : set_positive_charge(false); }

  float get_chisq() const override { return _chisq; }
  void set_chisq(float chisq) override { _chisq = chisq; }

  unsigned int get_ndf() const override { return _ndf; }
  void set_ndf(int ndf) override { _ndf = ndf; }

  //set position and momentum for track
  float get_x() const override { return _x; }
  void set_x(float x) override { _x = x; }

  float get_y() const override { return _y; }
  void set_y(float y) override { _y = y; }

  float get_z() const override { return _z; }
  void set_z(float z) override { _z = z; }

  float get_px() const override { return _px; }
  void set_px(float px) override { _px = px; }

  float get_py() const override { return _py; }
  void set_py(float py) override { _py = py; }

  float get_pz() const override { return _pz; }
  void set_pz(float pz) override { _pz = pz; }
*/


  //Functions for TPC Seed
  bool get_does_tpc_seed_exist() const override { return does_tpc_seed_exist; }
  float tpc_seed_get_qOverR() const  override{ return tpc_seed_m_qOverR; }
  float tpc_seed_get_X0() const  override{ return tpc_seed_m_X0; }
  float tpc_seed_get_Y0() const  override{ return tpc_seed_m_Y0; }
  float tpc_seed_get_slope() const  override{ return tpc_seed_m_slope; }
  float tpc_seed_get_Z0() const  override{ return tpc_seed_m_Z0; }
  //short int tpc_seed_get_crossing() const  override{ return tpc_seed_m_crossing; }
  bool get_is_tpc_helix_intersection_positive() const override { return tpc_helix_intersection_positive; }
  //float tpc_seed_get_R_param() const override {return tpc_seed_m_R_param;}

  void set_does_tpc_seed_exist(const bool exist) override { does_tpc_seed_exist = exist; }
  //void tpc_seed_set_crossing(const short int crossing) override { tpc_seed_m_crossing = crossing; }
  void tpc_seed_set_qOverR(const float qOverR) override { tpc_seed_m_qOverR = qOverR; }
  void tpc_seed_set_X0(const float X0) override { tpc_seed_m_X0 = X0; }
  void tpc_seed_set_Y0(const float Y0) override { tpc_seed_m_Y0 = Y0; }
  void tpc_seed_set_slope(const float slope) override { tpc_seed_m_slope = slope; }
  void tpc_seed_set_Z0(const float Z0) override { tpc_seed_m_Z0 = Z0; }
  void set_is_tpc_helix_intersection_positive(const bool positive) override { tpc_helix_intersection_positive = positive; }
  //void tpc_seed_set_R_param(const float R) override {tpc_seed_m_R_param = R;}

  //Functions for silicon seed
/*
  bool get_does_silicon_seed_exist() const override { return does_silicon_seed_exist; }
  float silicon_seed_get_qOverR() const override { return silicon_seed_m_qOverR; }
  float silicon_seed_get_X0() const override { return silicon_seed_m_X0; }
  float silicon_seed_get_Y0() const override { return silicon_seed_m_Y0; }
  float silicon_seed_get_slope() const override { return silicon_seed_m_slope; }
  float silicon_seed_get_Z0() const override { return silicon_seed_m_Z0; }
  short int silicon_seed_get_crossing() const override { return silicon_seed_m_crossing; }

  void set_does_silicon_seed_exist(const bool exist) override { does_silicon_seed_exist = exist; }
  void silicon_seed_set_crossing(const short int crossing) override { silicon_seed_m_crossing = crossing; }
  void silicon_seed_set_qOverR(const float qOverR) override { silicon_seed_m_qOverR = qOverR; }
  void silicon_seed_set_X0(const float X0) override { silicon_seed_m_X0 = X0; }
  void silicon_seed_set_Y0(const float Y0) override { silicon_seed_m_Y0 = Y0; }
  void silicon_seed_set_slope(const float slope) override { silicon_seed_m_slope = slope; }
  void silicon_seed_set_Z0(const float Z0) override { silicon_seed_m_Z0 = Z0; }
*/

  //make struct for cluster information
  /*
  struct cluster_with_key{
    //below is in trkclusterv5.h
    float m_local[2];          //< 2D local position [cm] 2 * 32 64bit  - cumul 1*64  
    TrkrDefs::subsurfkey m_subsurfkey; //< unique identifier for hitsetkey-surface maps 16 bit
    float m_phierr;
    float m_zerr;
    unsigned short int m_adc;           //< cluster sum adc 16
    unsigned short int m_maxadc;           //< cluster sum adc 16
    char m_phisize; // 8bit
    char m_zsize;   // 8bit
    char m_overlap; // 8bit 
    char m_edge;    // 8bit - cumul 2*64

    //TrkrDefs::hitsetkey m_hitsetkey;
    TrkrDefs::cluskey m_cluskey;
    };
    */

  //functions to set cluster structs in the array
  /*
  float getPosition(int struct_index, int coor) const override { return cluster_array[struct_index].m_local[coor]; }
  void setPosition(int struct_index, int coor, float xi) override { cluster_array[struct_index].m_local[coor] = xi; }
  float getLocalX(int struct_index) const override { return cluster_array[struct_index].m_local[0]; }
  void setLocalX(int struct_index, float loc0) override { cluster_array[struct_index].m_local[0] = loc0; }
  float getLocalY(int struct_index) const override { return cluster_array[struct_index].m_local[1]; }
  void setLocalY(int struct_index, float loc1) override { cluster_array[struct_index].m_local[1] = loc1; }
*/

  //TrkrDefs::subsurfkey getSubSurfKey(int struct_index) const override { return cluster_array[struct_index].m_subsurfkey; }
  //void setSubSurfKey(int struct_index, TrkrDefs::subsurfkey id) override { cluster_array[struct_index].m_subsurfkey = id; }

  //TrkrDefs::hitsetkey getHitSetKey(int struct_index) const override { return cluster_array[struct_index].m_hitsetkey; }
  //void setHitSetKey(int struct_index, TrkrDefs::hitsetkey id) override { cluster_array[struct_index].m_hitsetkey = id; }

  //TrkrDefs::cluskey getClusKey(int struct_index) const override { return cluster_array[struct_index].m_cluskey; }
  //void setClusKey(int struct_index, TrkrDefs::cluskey id) override { cluster_array[struct_index].m_cluskey = id; }

  //uint8_t getSide(int struct_index) const override { return cluster_array[struct_index].m_side; }
  //void setSide(int struct_index, uint8_t side) override { cluster_array[struct_index].m_side = side; }

  //uint8_t getSectorId(int struct_index) const override { return cluster_array[struct_index].m_sector_id; }
  //void setSectorId(int struct_index, uint8_t sector_id) override { cluster_array[struct_index].m_sector_id = sector_id; }

  //bool getValid(int struct_index) const override { return cluster_array[struct_index].m_valid; }
  //void setValid(int struct_index, bool valid) override { cluster_array[struct_index].m_valid = valid; }

  //cluster info
  /*
  unsigned int getAdc(int struct_index) const override { 
    uint8_t tmp = cluster_array[struct_index].m_adc;
    return tmp ; 

  }

  void setAdc(int struct_index, unsigned int adc) override {
    cluster_array[struct_index].m_adc = adc; 
  }

  unsigned int getMaxAdc(int struct_index) const override { 
    return cluster_array[struct_index].m_maxadc; 
  }

  void setMaxAdc(int struct_index, uint16_t maxadc) override {
    cluster_array[struct_index].m_maxadc = maxadc;
  }

  //
  // convenience interface
  //
  float getRPhiError(int struct_index) const override
  { return cluster_array[struct_index].m_phierr;}
  float getZError(int struct_index) const override
  { return cluster_array[struct_index].m_zerr;}

  void setPhiError(int struct_index, float phierror) override
  { cluster_array[struct_index].m_phierr = phierror;}
  void setZError(int struct_index, float zerror) override
  { cluster_array[struct_index].m_zerr = zerror;}

  //
  uint8_t getPhiSize(int struct_index) const override { return (float) cluster_array[struct_index].m_phisize; }
  void setPhiSize(int struct_index, uint8_t phisize) override { cluster_array[struct_index].m_phisize = phisize; }

  uint8_t getZSize(int struct_index) const override { return (float) cluster_array[struct_index].m_zsize; }
  void setZSize(int struct_index, uint8_t zsize) override { cluster_array[struct_index].m_zsize = zsize; }
 
  uint8_t getOverlap(int struct_index) const override { return cluster_array[struct_index].m_overlap; }
  void setOverlap(int struct_index, uint8_t overlap) override { cluster_array[struct_index].m_overlap = overlap; }
 
  uint8_t getEdge(int struct_index) const override { return cluster_array[struct_index].m_edge; }
  void setEdge(int struct_index, uint8_t edge) override { cluster_array[struct_index].m_edge = edge; }
*/
  
  //Now set functions for cluster residual member
  //functions to set cluster structs in the array
  /*
  UShort_t getPositionKeyResiduals(int struct_index, int coor) const override { return cluster_residuals_array[struct_index].m_residual[coor]; }
  void setPositionKeyResiduals(int struct_index, int coor, UShort_t xi) override { cluster_residuals_array[struct_index].m_residual[coor] = xi; }
  UShort_t getLocalXKeyResiduals(int struct_index) const override { return cluster_residuals_array[struct_index].m_residual[0]; }
  void setLocalXKeyResiduals(int struct_index, UShort_t loc0) override { cluster_residuals_array[struct_index].m_residual[0] = loc0; }
  UShort_t getLocalYKeyResiduals(int struct_index) const override { return cluster_residuals_array[struct_index].m_residual[1]; }
  void setLocalYKeyResiduals(int struct_index, UShort_t loc1) override { cluster_residuals_array[struct_index].m_residual[1] = loc1; }
*/


  uint8_t getMinimumPositionKeyResiduals(int struct_index, int coor) const override { return cluster_residuals_array[struct_index].m_residual[coor]; }
  void setMinimumPositionKeyResiduals(int struct_index, int coor, uint8_t xi) override { cluster_residuals_array[struct_index].m_residual[coor] = xi; }
  uint8_t getMinimumLocalXKeyResiduals(int struct_index) const override { return cluster_residuals_array[struct_index].m_residual[0]; }
  void setMinimumLocalXKeyResiduals(int struct_index, uint8_t loc0) override { cluster_residuals_array[struct_index].m_residual[0] = loc0; }
  uint8_t getMinimumLocalYKeyResiduals(int struct_index) const override { return cluster_residuals_array[struct_index].m_residual[1]; }
  void setMinimumLocalYKeyResiduals(int struct_index, uint8_t loc1) override { cluster_residuals_array[struct_index].m_residual[1] = loc1; }


  //new adc info
  uint8_t getAdcInfoResiduals(int struct_index) const override { 
    return cluster_residuals_array[struct_index].m_adc_information; 
  }

  void setAdcInfoResiduals(int struct_index, uint8_t adcinfo) override {
    cluster_residuals_array[struct_index].m_adc_information = adcinfo;
  }

  //flag information
  //new adc info
  uint8_t getFlagsResiduals(int struct_index) const override { 
    return cluster_residuals_array[struct_index].m_flags; 
  }

  void setFlagsResiduals(int struct_index, uint8_t flags) override {
    cluster_residuals_array[struct_index].m_flags = flags;
  }

  //
  // convenience interface
  //
  /*
  float getRPhiErrorResiduals(int struct_index) const override
  { return cluster_residuals_array[struct_index].m_phierr;}
  float getZErrorResiduals(int struct_index) const override
  { return cluster_residuals_array[struct_index].m_zerr;}

  void setPhiErrorResiduals(int struct_index, float phierror) override
  { cluster_residuals_array[struct_index].m_phierr = phierror;}
  void setZErrorResiduals(int struct_index, float zerror) override
  { cluster_residuals_array[struct_index].m_zerr = zerror;}
*/
  //
  /*
  uint8_t getPhiSizeResiduals(int struct_index) const override { return (float) cluster_residuals_array[struct_index].m_phisize; }
  void setPhiSizeResiduals(int struct_index, uint8_t phisize) override { cluster_residuals_array[struct_index].m_phisize = phisize; }

  uint8_t getZSizeResiduals(int struct_index) const override { return (float) cluster_residuals_array[struct_index].m_zsize; }
  void setZSizeResiduals(int struct_index, uint8_t zsize) override { cluster_residuals_array[struct_index].m_zsize = zsize; }
 
  uint8_t getOverlapResiduals(int struct_index) const override { return cluster_residuals_array[struct_index].m_overlap; }
  void setOverlapResiduals(int struct_index, uint8_t overlap) override { cluster_residuals_array[struct_index].m_overlap = overlap; }
 
  uint8_t getEdgeResiduals(int struct_index) const override { return cluster_residuals_array[struct_index].m_edge; }
  void setEdgeResiduals(int struct_index, uint8_t edge) override { cluster_residuals_array[struct_index].m_edge = edge; }
*/


 private:
  //TrackMap _map;

  //save seeds and parts of cluster
  //track seeds are currently members of SvtxTrack_v4
  //TClonesArray TPCTrackSeed = new TClonesArray("TrackSeed_v1");

  //store specific information from track seeds
  //ClusterKeySet m_cluster_keys;
  
  bool does_tpc_seed_exist = true;
  float tpc_seed_m_qOverR = NAN;
  float tpc_seed_m_X0 = NAN;
  float tpc_seed_m_Y0 = NAN;
  float tpc_seed_m_slope = NAN;
  float tpc_seed_m_Z0 = NAN;
  //short int tpc_seed_m_crossing = std::numeric_limits<short int>::max();

  //bool to say whether to use positive or negative values in circle_circle_intersection for helix fix
  bool tpc_helix_intersection_positive = true;
  //float tpc_seed_m_R_param = NAN;

/*
  bool does_silicon_seed_exist = true;
  float silicon_seed_m_qOverR = NAN;
  float silicon_seed_m_X0 = NAN;
  float silicon_seed_m_Y0 = NAN;
  float silicon_seed_m_slope = NAN;
  float silicon_seed_m_Z0 = NAN;
  short int silicon_seed_m_crossing = std::numeric_limits<short int>::max();
*/
  //ClusterKeySet silicon_cluster_keys;
  //information from SvtxTrack_v4
  // track information
  //don't include seeds as we will store thier direct information
  //TrackSeed* _tpc_seed = nullptr; 
  //TrackSeed* _silicon_seed = nullptr;

  /*
  unsigned int _track_id = UINT_MAX;
  unsigned int _vertex_id = UINT_MAX;
  bool _is_positive_charge = false;
  float _chisq = NAN;
  unsigned int _ndf = 0;
  short int _track_crossing = SHRT_MAX;

  float _x = NAN;
  float _y = NAN;
  float _z = NAN;
  float _px = NAN;
  float _py = NAN;
  float _pz = NAN;
  */
  
  // track state  information
  //find a way to not use this
  //StateMap _states;  //< path length => state object
  //to make hitset key, first 8 bits are z, next 8 bits are phi, next 8 bits are layer, last 8 bits are id
  
  //originally 59 elements, now split to 7 (silicon) and 52 (tpc)
  //include the 9 in cluster_array because the two TPOT seeds will be stored in it
  //struct cluster_with_sector_side cluster_array[59];
  //struct cluster_with_sector_side cluster_array[9];
  struct cluster_with_sector_side_residuals_minimum cluster_residuals_array[48];
  

  ClassDefOverride(SvtxTrackArray_v4, 1);
};





#endif
