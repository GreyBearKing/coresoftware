#ifndef G4EVAL_DSTTRACKARRAYWRITER_H
#define G4EVAL_DSTTRACKARRAYWRITER_H

/*!
 * \file DSTTrackArrayWriter.h
 * \author Alex Patton <aopatton@mit.edu>
 * 
 */

#include <fun4all/SubsysReco.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase_historic/SvtxTrack_v4.h>
#include <trackbase_historic/SvtxTrackArray_v1.h>
#include <trackbase_historic/SvtxTrackArrayContainer_v1.h>
//#include <trackbase_historic/TrkrClusterContainer.h>

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
class TrkrClusterContainer;
class TrkrClusterHitAssoc;
class TrkrHitSetContainer;
class TrkrHitTruthAssoc;
class TrackSeedContainer;

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

  private:

  //! load nodes
  int load_nodes( PHCompositeNode* );

  //! evaluate track info
  void evaluate_track_and_clusters();

  SvtxTrackArrayContainer_v1* m_track_array_container = nullptr;

  //! flags
  int m_flags = WriteEvent | WriteClusters | WriteTracks;

  //! tracks
  SvtxTrackMap* m_track_map = nullptr;

  TrkrClusterContainer* m_cluster_map = nullptr;


};

#endif  // G4EVAL_DSTTRACKARRAYWRITER_H
