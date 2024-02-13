#ifndef G4EVAL_DSTTRACKARRAYREADER_H
#define G4EVAL_DSTTRACKARRAYREADER_H

/*!
 * \file DSTTrackArrayReader.h
 * \author Alex Patton <aopatton@mit.edu>
 */

// #include "DSTContainerv3.h"

#include <fun4all/SubsysReco.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase/TrkrClusterContainerv4.h>
#include <trackbase/TrkrClusterv5.h>
#include <trackbase_historic/TrackSeed_v1.h>
#include <trackbase_historic/TrackSeedContainer_v1.h>
#include <trackbase_historic/SvtxTrackArrayContainer_v1.h>
#include <trackbase/ActsGeometry.h>

#include <map>
#include <set>
#include <string>
#include <vector>

class PHG4Hit;
class PHG4HitContainer;
class PHG4Particle;
class PHG4TruthInfoContainer;
class SvtxTrack;
class SvtxTrackMap;
//class DSTContainer;
class TrkrCluster;
class TrkrClusterContainer;
class TrkrClusterHitAssoc;
class TrkrHitSetContainer;
class TrkrHitTruthAssoc;

class DSTTrackArrayReader : public SubsysReco
{
  public:

  //! constructor
  DSTTrackArrayReader(const std::string& = "DSTTrackArrayReader");

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

  private:

  //! load nodes
  int load_nodes( PHCompositeNode* );
  
  void evaluate_track_and_clusters();

  // SvtxTrack recover_track(DSTContainerv3::TrackStruct);

  //! tracks
  SvtxTrackMap* m_track_map = nullptr;

  //! clusters
  TrkrClusterContainer* m_cluster_map = nullptr;

  //! trackseedcontainers
  TrackSeedContainer* m_tpc_seed_container = nullptr;
  TrackSeedContainer* m_silicon_seed_container = nullptr;

  SvtxTrackArrayContainer_v1* m_track_array_container = nullptr;
  //DSTContainer* m_container = nullptr;
  SvtxTrackArray_v1* trackContainer = nullptr;


  TrackSeed_v1* TPCSeed = nullptr;
  TrackSeed_v1* SiliconSeed = nullptr;
  TrkrClusterv5* m_cluster = nullptr;

  //! flags
  int m_flags = WriteEvent | WriteClusters | WriteTracks;
  
  ActsGeometry* tgeometry = nullptr;

  //@}

  // debugging helpers
  //bool dryrun = false;
  //bool generateKey = false;

};

#endif  // G4EVAL_DSTTRACKARRAYREADER_H
