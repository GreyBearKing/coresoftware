#ifndef G4EVAL_DSTTRACKARRAYWRITER_H
#define G4EVAL_DSTTRACKARRAYWRITER_H

/*!
 * \file DSTTrackArrayWriter.h
 * \author Alex Patton <aopatton@mit.edu>
 * 
 */

//#include "DSTCompressor.h"
#include <fun4all/SubsysReco.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase_historic/SvtxTrack_v4.h>
#include <trackbase_historic/SvtxTrackArray_v1.h>
#include <trackbase_historic/SvtxTrackArray_v2.h>
#include <trackbase_historic/SvtxTrackArray_v3.h>
#include <trackbase_historic/SvtxTrackArray_v4.h>
#include <trackbase_historic/SvtxTrackArray_v5.h>
#include <trackbase_historic/SvtxTrackArray_v6.h>
#include <trackbase_historic/SvtxTrackArrayContainer_v1.h>
#include <trackbase_historic/SvtxTrackArrayContainer_v2.h>
#include <trackbase_historic/SvtxTrackArrayContainer_v3.h>
#include <trackbase_historic/SvtxTrackArrayContainer_v4.h>
#include <trackbase_historic/SvtxTrackArrayContainer_v5.h>
#include <trackbase_historic/SvtxTrackArrayContainer_v6.h>
//#include <trackbase_historic/TrkrClusterContainer.h>
#include <trackbase/ActsGeometry.h>
#include <trackbase/TrackFitUtils.h>
#include <g4detectors/PHG4TpcCylinderGeom.h>
#include <g4detectors/PHG4TpcCylinderGeomContainer.h>
#include <g4detectors/PHG4CylinderGeom.h>
//#include <intt/CylinderGeomIntt.h>

#include <map>
#include <set>
#include <string>
#include <vector>

#include <TClonesArray.h>
#include <TFile.h>
#include <TTree.h>

class PHG4Hit;
class PHG4HitContainer;
class PHG4Particle;
class PHG4TruthInfoContainer;
class SvtxTrack;
class SvtxTrackMap;
//class DSTContainerv3;
//class DSTContainer;
class TrkrCluster;
class TrkrClusterv5;
class TrkrClusterContainer;
class TrkrClusterHitAssoc;
class TrkrHitSetContainer;
class TrkrHitTruthAssoc;
class TrackSeedContainer;
class TNtuple;
class DSTCompressor;

class DSTTrackArrayWriter : public SubsysReco
{
  public:

  //! constructor
  DSTTrackArrayWriter( const std::string& = "DSTTrackArrayWriter");

  //! global initialization
  int Init(PHCompositeNode*) override;

  //! run initialization
  int InitRun(PHCompositeNode*) override;

  //! event processing
  int process_event(PHCompositeNode*) override;

  //! end of processing
  int End(PHCompositeNode*) override;

  enum Flags
  {
    WriteEvent = 1<<0,
    WriteClusters = 1<<1,
    WriteTracks = 1<<2
  };

  //! set flags. Should be a bitwise or of Flags enum
  void set_flags( int flags )
  { m_flags = flags; }

  void set_write_ntuple_residual(bool flag) {m_write_ntp_residual = flag;}

  void set_write_ntuple_reduced_cluster(bool flag) {m_write_ntp_reduced_cluster = flag;}

  void set_ntuple_reduced_cluster_outfile(std::string name){ntp_reduced_cluster_name = name;}

  private:

  //! load nodes
  int load_nodes( PHCompositeNode* );

  //! evaluate track info
  void evaluate_track_and_clusters();

  void evaluate_track_and_cluster_residuals();

  void evaluate_track_and_cluster_residual_compression();

  void evaluate_track_and_cluster_minimum_residual_compression();
  void evaluate_only_cluster_minimum_residual_compression();
  void evaluate_limited_track_and_cluster_minimum_residual_compression();
  void no_silicon_evaluate_limited_track_and_cluster_minimum_residual_compression();
  void no_silicon_evaluate_limited_trackv5_and_cluster_minimum_residual_compression();
  void no_silicon_evaluate_limited_trackv6_and_cluster_minimum_residual_compression();

  Acts::Vector3 get_helix_surface_intersection(Surface, std::vector<float>&, Acts::Vector3);

  void fillNtpResidual(float /*xresidual*/, float /*yresidual*/, float /*radius*/, float /*local x*/, float /*local y*/, float /*globalz*/, float /*localx*/, float /*localy*/, float /*phi*/, float /*TrackSeedR*/);

  void fillNtpReducedCluster(float /*xKey*/, float /*yKey*/);

  SvtxTrackArrayContainer_v1* m_track_array_container = nullptr;

  SvtxTrackArrayContainer_v2* m_track_array_container_v2 = nullptr;

  SvtxTrackArrayContainer_v3* m_track_array_container_v3 = nullptr;

  SvtxTrackArrayContainer_v4* m_track_array_container_v4 = nullptr;

  SvtxTrackArrayContainer_v5* m_track_array_container_v5 = nullptr;
    
  SvtxTrackArrayContainer_v6* m_track_array_container_v6 = nullptr;

  //SvtxTrackArrayContainer_v5* m_track_array_container_v5 = new SvtxTrackArrayContainer_v5();

  //! flags
  int m_flags = WriteEvent | WriteClusters | WriteTracks;

  //! tracks
  SvtxTrackMap* m_track_map = nullptr;

  TrkrClusterContainer* m_cluster_map = nullptr;

  SvtxTrackMap* m_reduced_track_map = nullptr;

  TrkrClusterContainer* m_reduced_cluster_map = nullptr;

  TrkrClusterv5* m_cluster = nullptr;

  ActsGeometry* tgeometry = nullptr;

  PHG4TpcCylinderGeomContainer* tpcGeom = nullptr;

  //make an TNTuple to put residual information for analysis in
  TNtuple *ntp_residual{nullptr};

  TNtuple *ntp_reduced_cluster{nullptr};

  bool m_write_ntp_residual = false;

  bool m_write_ntp_reduced_cluster = false;

  std::string ntp_reduced_cluster_name = "ReducedClusterTuple";

  DSTCompressor* m_compressor{};

  DSTCompressor* m_compressor_minimum{};

  DSTCompressor* m_compressor_adc{};

  int nBits = 12;

  int nBitsAdc = 4;
  int nBitsMinimum = 8;

};

#endif  // G4EVAL_DSTTRACKARRAYWRITER_H
