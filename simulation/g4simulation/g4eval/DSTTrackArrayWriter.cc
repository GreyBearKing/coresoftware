/*!
 * \file DSTTrackArrayWriter.cc
 * \author Alex Patton <aopatton@mit.edu>
 */

#include "DSTTrackArrayWriter.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <trackbase/InttDefs.h>
#include <micromegas/MicromegasDefs.h>
#include <trackbase/MvtxDefs.h>
#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHNodeIterator.h>
#include <trackbase/TpcDefs.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterv4.h>
//include new cluster
#include <trackbase/TrkrClusterContainer.h>
// #include <trackbase/TrkrClusterContainerv5.h>
#include <trackbase/TrkrClusterHitAssoc.h>
#include <trackbase/TrkrHit.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/TrkrHitTruthAssoc.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/TrackSeed_v1.h>
//include SvtxTrackMap_v1 to write data to it
#include <trackbase_historic/SvtxTrackMap_v1.h>
#include <trackbase_historic/SvtxTrack_v4.h>
#include <trackbase_historic/SvtxTrackArray_v1.h>
#include <trackbase_historic/SvtxTrackArrayContainer_v1.h>
#include <trackermillepedealignment/HelicalFitter.h>

#include <algorithm>
#include <bitset>
#include <cassert>
#include <iostream>
#include <numeric>
#include <utility> //for pair

#include <TClonesArray.h>
#include <TFile.h>
#include <TTree.h>
#include <TLine.h>
//_____________________________________________________________________

//_____________________________________________________________________
DSTTrackArrayWriter::DSTTrackArrayWriter( const std::string& name ):
  SubsysReco( name)
{}

//_____________________________________________________________________
int DSTTrackArrayWriter::Init(PHCompositeNode* topNode )
{
  std::cout << "Writer Init start" << std::endl;
  // find DST node
  PHNodeIterator iter(topNode);
  auto dstNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode)
  {
    std::cout << "DSTTrackArrayWriter::Init - DST Node missing" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  // get EVAL node
  iter = PHNodeIterator(dstNode);
  auto evalNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "EVAL"));
  if( !evalNode )
  {
    // create
    std::cout << "DSTTrackArrayWriter::Init - EVAL node missing - creating" << std::endl;
    evalNode = new PHCompositeNode( "EVAL" );
    dstNode->addNode(evalNode);
  }

  // // TClonesArary
  // m_container->arrClsDST = new TClonesArray("DSTContainerv3::ClusterStruct");
  // m_container->trkrClsDST = new TClonesArray("TrkrClusterv4");


  auto newArrayNode = new PHIODataNode<PHObject>( new SvtxTrackArrayContainer_v1, "SvtxTrackArrayContainer","PHObject");
  evalNode->addNode(newArrayNode);

  //auto newTrackNode = new PHIODataNode<PHObject>(new SvtxTrackArray_v1, "TRACK_ARRAYV1", "PHObject");
  //evalNode->addNode(newTrackNode);

  // TClonesArary
  // fcl = new TFile("dstcl.root", "recreate");
  // tcl = new TTree("tcl", "dst tree");
  // arrEvt = new TClonesArray("DSTContainerv3::EventStruct");
  // arrTrk = new TClonesArray("DSTContainerv3::TrackStruct");
  // arrCls = new TClonesArray("DSTContainerv3::ClusterStruct");
  // tcl->Branch("evt", &arrEvt);
  // tcl->Branch("trk", &arrTrk);
  // tcl->Branch("cls", &arrCls);
  std::cout << "Writer Init end" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//_____________________________________________________________________
int DSTTrackArrayWriter::InitRun(PHCompositeNode*)
{ return Fun4AllReturnCodes::EVENT_OK; }

//_____________________________________________________________________
int DSTTrackArrayWriter::process_event(PHCompositeNode* topNode)
{
  //make topNode run in Init
  //Init(topNode);
  // load nodes
  std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__ << std::endl;
  std::cout << "DSTTrackArrayWriter::process_event" << std::endl;
  auto res =  load_nodes(topNode);
  if( res != Fun4AllReturnCodes::EVENT_OK ) {
    return res;
  }
  std::cout << "Return codes  start" << Fun4AllReturnCodes::EVENT_OK << std::endl;
  // cleanup output container
  if( m_track_array_container) {
    m_track_array_container->Reset();
  }
    evaluate_track_and_cluster_residuals();




  std::cout << "Return codes end" << Fun4AllReturnCodes::EVENT_OK << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//_____________________________________________________________________
int DSTTrackArrayWriter::End(PHCompositeNode*)
{
  std::cout << "In the end" << std::endl;
  // tcl->Print();
  // tcl->Write();
  // fcl->Write();
  // fcl->Close();
  return Fun4AllReturnCodes::EVENT_OK;
}

//_____________________________________________________________________
int DSTTrackArrayWriter::load_nodes( PHCompositeNode* topNode )
{

  tgeometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");

    if (!tgeometry)
    {
      std::cout << PHWHERE << "No Acts geometry on node tree. Can't  continue."
                << std::endl;
    }

  // get necessary nodes
  m_track_map = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  
  m_cluster_map = findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");

  m_track_array_container = findNode::getClass<SvtxTrackArrayContainer_v1>(topNode, "SvtxTrackArrayContainer");

  tpcGeom = findNode::getClass<PHG4TpcCylinderGeomContainer>(topNode, "CYLINDERCELLGEOM_SVTX");

  return Fun4AllReturnCodes::EVENT_OK;

}


//_____________________________________________________________________
void DSTTrackArrayWriter::evaluate_track_and_clusters(){
//use this to create object that looks through both tracks and clusters and saves into new object
//make sure clusters exist
//std::cout << "start of check" << "\n";
//if(!(m_cluster_map&&m_hitsetcontainer&&m_container)) return;
//make sure tracks exist
if( !( m_track_map && m_cluster_map && m_track_array_container ) ) {
    return;
}
//std::cout << "after check" << "\n";
//m_track_array_container->Reset();



//TClonesArray& TrkContainer = *m_container->arrTrkContainer;
//TrkContainer.Clear();


Int_t iTrk = 0;
  //long unsigned int iKey = 0;

  //std::cout << "Before loop" << "\n";
  for( const auto& trackpair:*m_track_map )
  {
    std::cout << "start of loop" << "\n";
    //new(trkrDST[iCluster]) TrkrClusterv4();
    // TrkrClusterv4* clsArr = (TrkrClusterv4*) trkrDST.ConstructedAt(iCluster);
    

    //unsigned int key = trackpair.first;
    const auto track = trackpair.second;
    //auto track_struct = create_track( track );
    /*
    // truth information
    const auto [id,contributors] = get_max_contributor( track );
    track_struct.contributors = contributors;
    
    // get particle
    auto particle = m_g4truthinfo->GetParticle(id);
    track_struct.embed = get_embed(particle);
    add_truth_information(track_struct, particle);
    */
    

    //Store information from Track into new container that will also hold cluster information
    //new(TrkContainer[iTrk]) SvtxTrackArray_v1;
    //SvtxTrackArray_v1* trackContainer = (SvtxTrackArray_v1*) TrkContainer.ConstructedAt(iTrk);

    SvtxTrackArray_v1* trackContainer = new SvtxTrackArray_v1();

    /*
    if(m_container->getArrayKeysSize()<(iKey+1)){
        m_container->resizeArrayKeys(iKey+1);
    }
    m_container->setArrayKeys(iKey,key);
    */

    //Save all information from a Track into the new container
    std::cout << "track get id:" << track->get_id() <<"\n"; 
    trackContainer->set_id(track->get_id());
    trackContainer->set_vertex_id(track->get_vertex_id());
    trackContainer->set_positive_charge(track->get_positive_charge());
    trackContainer->set_chisq(track->get_chisq());
    trackContainer->set_ndf(track->get_ndf());
    trackContainer->set_crossing(track->get_crossing());

    trackContainer->set_x(track->get_x());
    trackContainer->set_y(track->get_y());
    trackContainer->set_z(track->get_z());
    trackContainer->set_px(track->get_px());
    trackContainer->set_py(track->get_py());
    trackContainer->set_pz(track->get_pz());

    TrackSeed* TPCSeed = track->get_tpc_seed();
    TrackSeed* SiliconSeed = track->get_silicon_seed();

    if(!TPCSeed){
      std::cout << "TPCSeed does not exist \n";
      trackContainer->set_does_tpc_seed_exist(false);
    }else{
    //store information from tpc seed
    trackContainer->set_does_tpc_seed_exist(true);
    std::cout << "tpcseed:" << TPCSeed->get_qOverR() <<"\n"; 
    trackContainer->tpc_seed_set_qOverR(TPCSeed->get_qOverR());
    trackContainer->tpc_seed_set_X0(TPCSeed->get_X0());
    trackContainer->tpc_seed_set_Y0(TPCSeed->get_Y0());
    trackContainer->tpc_seed_set_slope(TPCSeed->get_slope());
    trackContainer->tpc_seed_set_Z0(TPCSeed->get_Z0());
    trackContainer->tpc_seed_set_crossing(TPCSeed->get_crossing());
    }

    if(!SiliconSeed){
      std::cout << "SiliconSeed does not exist \n";
      trackContainer->set_does_silicon_seed_exist(false);
    }else{
    trackContainer->set_does_silicon_seed_exist(true);
    //store information from silicon seed
    std::cout << "silconseed:" << SiliconSeed->get_qOverR() <<"\n"; 
    trackContainer->silicon_seed_set_qOverR(SiliconSeed->get_qOverR());
    trackContainer->silicon_seed_set_X0(SiliconSeed->get_X0());
    trackContainer->silicon_seed_set_Y0(SiliconSeed->get_Y0());
    trackContainer->silicon_seed_set_slope(SiliconSeed->get_slope());
    trackContainer->silicon_seed_set_Z0(SiliconSeed->get_Z0());
    trackContainer->silicon_seed_set_crossing(SiliconSeed->get_crossing());
    }

    //trackContainer->copy_states(track);
    //store clusterkeyset from each seed
  
  //set all clusterkeys = 0
  for(int layer = 0; layer < 59; layer++){
      //trackContainer->setClusKey(layer, 0);
  }

  

  if(trackContainer->get_does_tpc_seed_exist()){
    std::cout << "We are about to loop over cluster keys in TPC Seed" << std::endl;
  TPCSeed->identify();
    for( auto key_iter = TPCSeed->begin_cluster_keys(); key_iter != TPCSeed->end_cluster_keys(); ++key_iter )
    {
      const auto& cluster_key = *key_iter;
      auto cluster = m_cluster_map->findCluster( cluster_key );
      if( !cluster )
      {
        std::cout << "DSTTrackArrayWriter::evaluate_tracks - unable to find cluster for key " << cluster_key << std::endl;
        continue;
      }
      //store information in track array
      std::cout << "TPC clusterkey: " << cluster_key <<"\n";
      std::cout << "TPC subsurfkey: " << cluster->getSubSurfKey() << std::endl;
      uint8_t layer = TrkrDefs::getLayer(cluster_key);

      std::cout << "TPC layer: " << unsigned(TrkrDefs::getLayer(cluster_key)) << std::endl;
      //std::cout << "cluster position 0: " << cluster->getPosition(0) <<"\n";
      trackContainer->setLocalX(layer, cluster->getLocalX());
      trackContainer->setLocalY(layer, cluster->getLocalY());
      trackContainer->setSubSurfKey(layer, cluster->getSubSurfKey());
      trackContainer->setPhiError(layer, cluster->getRPhiError());
      trackContainer->setZError(layer, cluster->getZError());
      trackContainer->setAdc(layer, cluster->getAdc());
      trackContainer->setMaxAdc(layer, cluster->getMaxAdc());
      trackContainer->setPhiSize(layer, cluster->getPhiSize());
      trackContainer->setZSize(layer, cluster->getZSize());
      trackContainer->setOverlap(layer, cluster->getOverlap());
      trackContainer->setEdge(layer, cluster->getEdge());

      std::cout << "TPC Side: " << unsigned(TrkrDefs::getZElement(cluster_key)) << std::endl;
      std::cout << "TPC Sector Id: " << unsigned(TrkrDefs::getPhiElement(cluster_key)) << std::endl;

      trackContainer->setValid(layer, true);

      if(trackContainer->getValid(layer)){
        std::cout << "Valid is true" << std::endl;
      }

      trackContainer->setSide(layer, TrkrDefs::getZElement(cluster_key));
      trackContainer->setSectorId(layer, TrkrDefs::getPhiElement(cluster_key));
      trackContainer->setClusKey(layer, cluster_key);

      //TrkrDefs::
      
    }
  }

  
  if(trackContainer->get_does_silicon_seed_exist()){
    std::cout << "We are about to loop over cluster keys in Silicon Seed" << std::endl;
  SiliconSeed->identify();
    for( auto key_iter = SiliconSeed->begin_cluster_keys(); key_iter != SiliconSeed->end_cluster_keys(); ++key_iter )
    {
      const auto& cluster_key = *key_iter;
      auto cluster = m_cluster_map->findCluster( cluster_key );
      if( !cluster )
      {
        std::cout << "DSTTrackArrayWriter::evaluate_tracks - unable to find cluster for key " << cluster_key << std::endl;
        continue;
      }
      //store cluster key
      //silicon_cluster_keys->insert_cluster_key(cluster_key);

      //store information in track array
      std::cout << "Silicon clusterkey: " << cluster_key <<"\n";

      std::cout << "Silicon subsurfkey: " << cluster->getSubSurfKey() << std::endl;
      uint8_t layer = TrkrDefs::getLayer(cluster_key);

      std::cout << "Silicon layer: " << unsigned(TrkrDefs::getLayer(cluster_key)) << std::endl;
      //std::cout << "cluster position 0: " << cluster->getPosition(0) <<"\n";
      trackContainer->setLocalX(layer, cluster->getPosition(0));
      trackContainer->setLocalY(layer, cluster->getPosition(1));
      trackContainer->setSubSurfKey(layer, cluster->getSubSurfKey());
      trackContainer->setPhiError(layer, cluster->getRPhiError());
      trackContainer->setZError(layer, cluster->getZError());
      trackContainer->setAdc(layer, cluster->getAdc());
      trackContainer->setMaxAdc(layer, cluster->getMaxAdc());
      trackContainer->setPhiSize(layer, cluster->getPhiSize());
      trackContainer->setZSize(layer, cluster->getZSize());
      trackContainer->setOverlap(layer, cluster->getOverlap());
      trackContainer->setEdge(layer, cluster->getEdge());
      //trackContainer->setClusKey(layer, cluster_key);

      std::cout << "Silicon Side: " << unsigned(TrkrDefs::getZElement(cluster_key)) << std::endl;
      std::cout << "Silicon Sector Id: " << unsigned(TrkrDefs::getPhiElement(cluster_key)) << std::endl;

      trackContainer->setSide(layer, TrkrDefs::getZElement(cluster_key));
      trackContainer->setSectorId(layer, TrkrDefs::getPhiElement(cluster_key));

      //trackContainer->setClusKey(layer, cluster_key);

      
    }
  }
    //Start a loop over clusters to store them in container associated with each track
    //ConstClusterKeyIter find_cluster_key(TrkrDefs::cluskey clusterid) const override { return m_cluster_keys.find(clusterid); }
    /*
    
    MAKE A LOOP OVER CLUSTERS HERE
    Maybe go into evaluate_clusters
    */
    
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

    TrkrDefs::hitsetkey m_hitsetkey;
    };
    */
    

    

    /*
    trackContainer->clusterArray[layer].m_local[0] = cluster->getPosition(0);
    trackContainer->clusterArray[layer].m_local[1] = cluster->getPosition(1);
    trackContainer->clusterArray[layer].m_subsurfkey = cluster->getSubSurfKey();
    trackContainer->clusterArray[layer].m_phierr = cluster->getRPhiError();
    trackContainer->clusterArray[layer].m_zerr = cluster->getZError();
    trackContainer->clusterArray[layer].m_adc = cluster->getAdc();
    trackContainer->clusterArray[layer].m_maxadc = cluster->getMaxAdc();
    trackContainer->clusterArray[layer].m_phisize = cluster->getPhiSize();
    trackContainer->clusterArray[layer].m_zsize = cluster->getZSize();
    trackContainer->clusterArray[layer].m_overlap = cluster->getOverlap();
    trackContainer->clusterArray[layer].m_edge = cluster->getEdge();
    */
    


    // running iterator over track states, used to match a given cluster to a track state
    



  /*
    // loop over clusters
    for( auto key_iter = track->begin_cluster_keys(); key_iter != track->end_cluster_keys(); ++key_iter )
    {

      const auto& cluster_key = *key_iter;
      auto cluster = m_cluster_map->findCluster( cluster_key );
      if( !cluster )
      {
        std::cout << "DSTWriter::evaluate_tracks - unable to find cluster for key " << cluster_key << std::endl;
        continue;
      }

      //store information in track array
      layer = TrkrDefs::getLayer(cluster_key);
      trackContainer->setLocalX(layer, cluster->getPosition(0));
      trackContainer->setLocalY(layer, cluster->getPosition(1));
      trackContainer->setSubSurfKey(layer, cluster->getSubSurfKey());
      trackContainer->setPhiError(layer, cluster->getRPhiError());
      trackContainer->setZError(layer, cluster->getZError());
      trackContainer->setAdc(layer, cluster->getAdc());
      trackContainer->setMaxAdc(layer, cluster->getMaxAdc());
      trackContainer->setPhiSize(layer, cluster->getPhiSize());
      trackContainer->setZSize(layer, cluster->getZSize());
      trackContainer->setOverlap(layer, cluster->getOverlap());
      trackContainer->setEdge(layer, cluster->getEdge());
*/
       
      /*
      // find track state that is the closest to cluster
      // this assumes that both clusters and states are sorted along r
      const auto radius( cluster_struct.r );
      float dr_min = -1;
      for( auto iter = state_iter; iter != track->end_states(); ++iter )
      {
        const auto dr = std::abs( radius - get_r( iter->second->get_x(), iter->second->get_y() ) );
        if( dr_min < 0 || dr < dr_min )
        {
          state_iter = iter;
          dr_min = dr;
        } else break;
      }

      // store track state in cluster struct
      add_trk_information( cluster_struct, state_iter->second );

      // add to track
      track_struct.clusters.push_back( cluster_struct );
      */
      m_track_array_container->add_trackarray(iTrk, trackContainer);
      std::cout<< "track chisq: " << trackContainer->get_chisq() << std::endl;
      std::cout<< "container chisq: " << (m_track_array_container->get_trackarray(iTrk))->get_chisq() << std::endl;



      std::cout<< "cluster 1 local x: " << trackContainer->getLocalX(1) << std::endl;
      std::cout<< "container 1 local x: " << (m_track_array_container->get_trackarray(iTrk))->getLocalX(1) << std::endl;

      std::cout<< "cluster 7 Subsurf: " << trackContainer->getSubSurfKey(7) << std::endl;
      std::cout<< "container 7 Subsurf: " << (m_track_array_container->get_trackarray(iTrk))->getSubSurfKey(7) << std::endl;

      std::cout<< "cluster 7 is valid: " << trackContainer->getValid(7) << std::endl;

      delete trackContainer;
      ++iTrk;
      //++iKey;
      std::cout << "end of loop" << "\n";
    }
    

     //m_track_map->clear();
     m_track_map->Reset();
     m_cluster_map->Reset();
     
     //m_cluster_map->Clear();


    //cluster->getSubSurfKey();
    //get a cluskey from a subsurfkey
    //TPCSeed->find_cluster_key(cluskey);

    



}

//_____________________________________________________________________



//_____________________________________________________________________
void DSTTrackArrayWriter::evaluate_track_and_cluster_residuals(){
//use this to create object that looks through both tracks and clusters and saves into new object
//make sure clusters exist
//std::cout << "start of check" << "\n";
//if(!(m_cluster_map&&m_hitsetcontainer&&m_container)) return;
//make sure tracks exist
if( !( m_track_map && m_cluster_map && m_track_array_container ) ) {
    return;
}
//std::cout << "after check" << "\n";
//m_track_array_container->Reset();



//TClonesArray& TrkContainer = *m_container->arrTrkContainer;
//TrkContainer.Clear();


Int_t iTrk = 0;
  //long unsigned int iKey = 0;

  //std::cout << "Before loop" << "\n";
  for( const auto& trackpair:*m_track_map )
  {
    std::cout << "start of loop" << "\n";
    //new(trkrDST[iCluster]) TrkrClusterv4();
    // TrkrClusterv4* clsArr = (TrkrClusterv4*) trkrDST.ConstructedAt(iCluster);
    

    //unsigned int key = trackpair.first;
    const auto track = trackpair.second;
    //auto track_struct = create_track( track );
    /*
    // truth information
    const auto [id,contributors] = get_max_contributor( track );
    track_struct.contributors = contributors;
    
    // get particle
    auto particle = m_g4truthinfo->GetParticle(id);
    track_struct.embed = get_embed(particle);
    add_truth_information(track_struct, particle);
    */
    

    //Store information from Track into new container that will also hold cluster information
    //new(TrkContainer[iTrk]) SvtxTrackArray_v1;
    //SvtxTrackArray_v1* trackContainer = (SvtxTrackArray_v1*) TrkContainer.ConstructedAt(iTrk);

    SvtxTrackArray_v1* trackContainer = new SvtxTrackArray_v1();

    /*
    if(m_container->getArrayKeysSize()<(iKey+1)){
        m_container->resizeArrayKeys(iKey+1);
    }
    m_container->setArrayKeys(iKey,key);
    */

    //Save all information from a Track into the new container
    std::cout << "track get id:" << track->get_id() <<"\n"; 
    trackContainer->set_id(track->get_id());
    trackContainer->set_vertex_id(track->get_vertex_id());
    trackContainer->set_positive_charge(track->get_positive_charge());
    trackContainer->set_chisq(track->get_chisq());
    trackContainer->set_ndf(track->get_ndf());
    trackContainer->set_crossing(track->get_crossing());

    trackContainer->set_x(track->get_x());
    trackContainer->set_y(track->get_y());
    trackContainer->set_z(track->get_z());
    trackContainer->set_px(track->get_px());
    trackContainer->set_py(track->get_py());
    trackContainer->set_pz(track->get_pz());

    TrackSeed* TPCSeed = track->get_tpc_seed();
    TrackSeed* SiliconSeed = track->get_silicon_seed();

    if(!TPCSeed){
      std::cout << "TPCSeed does not exist \n";
      trackContainer->set_does_tpc_seed_exist(false);
    }else{
    //store information from tpc seed
    trackContainer->set_does_tpc_seed_exist(true);
    std::cout << "tpcseed:" << TPCSeed->get_qOverR() <<"\n"; 
    trackContainer->tpc_seed_set_qOverR(TPCSeed->get_qOverR());
    trackContainer->tpc_seed_set_X0(TPCSeed->get_X0());
    trackContainer->tpc_seed_set_Y0(TPCSeed->get_Y0());
    trackContainer->tpc_seed_set_slope(TPCSeed->get_slope());
    trackContainer->tpc_seed_set_Z0(TPCSeed->get_Z0());
    trackContainer->tpc_seed_set_crossing(TPCSeed->get_crossing());
    }

    if(!SiliconSeed){
      std::cout << "SiliconSeed does not exist \n";
      trackContainer->set_does_silicon_seed_exist(false);
    }else{
    trackContainer->set_does_silicon_seed_exist(true);
    //store information from silicon seed
    std::cout << "silconseed:" << SiliconSeed->get_qOverR() <<"\n"; 
    trackContainer->silicon_seed_set_qOverR(SiliconSeed->get_qOverR());
    trackContainer->silicon_seed_set_X0(SiliconSeed->get_X0());
    trackContainer->silicon_seed_set_Y0(SiliconSeed->get_Y0());
    trackContainer->silicon_seed_set_slope(SiliconSeed->get_slope());
    trackContainer->silicon_seed_set_Z0(SiliconSeed->get_Z0());
    trackContainer->silicon_seed_set_crossing(SiliconSeed->get_crossing());
    }

    //trackContainer->copy_states(track);
    //store clusterkeyset from each seed




    //calculate global position and residuals
    /*
      std::vector<Acts::Vector3> clusterPositions;
      std::vector<TrkrDefs::cluskey> clusterKeys;
      clusterKeys.insert(clusterKeys.end(), TPCSeed->begin_cluster_keys(),
			 TPCSeed->end_cluster_keys());
      if(SiliconSeed)
	    {
	      clusterKeys.insert(clusterKeys.end(), SiliconSeed->begin_cluster_keys(),
			     SiliconSeed->end_cluster_keys());
	    }
      TrackFitUtils::getTrackletClusters(tgeometry, m_cluster_map, 
					 clusterPositions, clusterKeys);
      std::vector<float> fitparams = TrackFitUtils::fitClusters(clusterPositions, clusterKeys);
*/  

      //make a loop over clusterKeys, find which position matches our cluster

  /*
  //set all clusterkeys = 0
  for(int layer = 0; layer < 59; layer++){
      //trackContainer->setClusKey(layer, 0);
  }
  */
  

  if(trackContainer->get_does_tpc_seed_exist()){
    std::cout << "We are about to loop over cluster keys in TPC Seed" << std::endl;
  TPCSeed->identify();
    for( auto key_iter = TPCSeed->begin_cluster_keys(); key_iter != TPCSeed->end_cluster_keys(); ++key_iter )
    {
      const auto& cluster_key = *key_iter;
      auto cluster = m_cluster_map->findCluster( cluster_key );
      if( !cluster )
      {
        std::cout << "DSTTrackArrayWriter::evaluate_tracks - unable to find cluster for key " << cluster_key << std::endl;
        continue;
      }
      //store information in track array
      std::cout << "TPC clusterkey: " << cluster_key <<"\n";
      std::cout << "TPC subsurfkey: " << cluster->getSubSurfKey() << std::endl;
      uint8_t layer = TrkrDefs::getLayer(cluster_key);

      std::cout << "TPC layer: " << unsigned(TrkrDefs::getLayer(cluster_key)) << std::endl;
      //std::cout << "cluster position 0: " << cluster->getPosition(0) <<"\n";
      trackContainer->setLocalX(layer, cluster->getLocalX());
      trackContainer->setLocalY(layer, cluster->getLocalY());
      trackContainer->setSubSurfKey(layer, cluster->getSubSurfKey());
      trackContainer->setPhiError(layer, cluster->getRPhiError());
      trackContainer->setZError(layer, cluster->getZError());
      trackContainer->setAdc(layer, cluster->getAdc());
      trackContainer->setMaxAdc(layer, cluster->getMaxAdc());
      trackContainer->setPhiSize(layer, cluster->getPhiSize());
      trackContainer->setZSize(layer, cluster->getZSize());
      trackContainer->setOverlap(layer, cluster->getOverlap());
      trackContainer->setEdge(layer, cluster->getEdge());

      std::cout << "TPC Side: " << unsigned(TrkrDefs::getZElement(cluster_key)) << std::endl;
      std::cout << "TPC Sector Id: " << unsigned(TrkrDefs::getPhiElement(cluster_key)) << std::endl;

      trackContainer->setValid(layer, true);

      if(trackContainer->getValid(layer)){
        std::cout << "Valid is true" << std::endl;
      }

      trackContainer->setSide(layer, TrkrDefs::getZElement(cluster_key));
      trackContainer->setSectorId(layer, TrkrDefs::getPhiElement(cluster_key));
      trackContainer->setClusKey(layer, cluster_key);

      //TrkrDefs::

      //loop over clusterKeys to match

      std::vector<Acts::Vector3> clusterPositions;
      std::vector<TrkrDefs::cluskey> clusterKeys;
      //clusterKeys.push_back(cluster_key);
      
      clusterKeys.insert(clusterKeys.end(), TPCSeed->begin_cluster_keys(),
			 TPCSeed->end_cluster_keys());
      
      TrackFitUtils::getTrackletClusters(tgeometry, m_cluster_map, 
					 clusterPositions, clusterKeys);
      std::cout << "clusterPositions.size(): " << clusterPositions.size() << std::endl;
      std::vector<float> fitparams = TrackFitUtils::fitClusters(clusterPositions, clusterKeys);
      std::cout << "clusterKeys[0]" << clusterKeys.at(0) << std::endl;
      std::cout << "fitParams[0]" << fitparams.at(0) << std::endl;
      std::cout << "fitParams[1]" << fitparams.at(1) << std::endl;
      std::cout << "fitParams[2]" << fitparams.at(2) << std::endl;
      std::cout << "fitParams[3]" << fitparams.at(3) << std::endl;
      std::cout << "fitParams[4]" << fitparams.at(4) << std::endl;
      //for(size_t i = 0; i < clusterPositions.size(); i++){
        //if(clusterKeys[i] == cluster_key){
          //Acts::Vector3 position = clusterPositions[i];
          Acts::Vector3 cglob;
          std::cout << "Before cglob" << std::endl;
          cglob = tgeometry->getGlobalPosition(cluster_key, cluster);
          std::cout << "Before surface" << std::endl;
          auto surface = tgeometry->maps().getSurface(cluster_key, cluster);
          //HelicalFitter fitter;
          std::cout << "Before intersection" << std::endl;
          //Acts::Vector3 intersection = TrackFitUtils::get_helix_pca(fitparams, cglob);
          Acts::Vector3 intersection = get_helix_surface_intersection(surface, fitparams, cglob);
          std::cout << "global[0]: " << cglob(0) << std::endl;
          std::cout << "global[1]: " << cglob(1) << std::endl;
          std::cout << "global[2]: " << cglob(2) << std::endl;
          std::cout << "After intersection" << std::endl;
          std::cout << "Intersection[0]: " << intersection(0) << std::endl;
          std::cout << "Intersection[1]: " << intersection(1) << std::endl;
          std::cout << "Intersection[2]: " << intersection(2) << std::endl;
          float pca_phi = atan2(intersection(1), intersection(0));
          float cluster_phi = atan2(cglob(1), cglob(0));
          float dphi = cluster_phi - pca_phi;
          if (dphi > M_PI)
          {
             dphi = 2 * M_PI - dphi;
          }
          if (dphi < -M_PI)
          {
           dphi = 2 * M_PI + dphi;
          }
          float dz = cglob(2) - intersection(2);
          std::cout << std::fixed;
          std::cout << std::setprecision(6);
          std::cout << "cluster phi: " << cluster_phi << std::endl;
          std::cout << "dPhi: " << dphi << std::endl;
          std::cout << "dZ: " << dz << std::endl;




          //calculate circle_circle_intersection between helix and tpc layer
          
          if(layer > 7 && layer < 55){
            auto geoLayer = tpcGeom->GetLayerCellGeom(layer);
            auto radius = geoLayer->get_radius();
            auto result = TrackFitUtils::circle_circle_intersection(radius, fitparams[0], fitparams[1], fitparams[2]);
            std::cout << "result(0): " << std::get<0>(result) << std::endl;
            std::cout << "result(1): " << std::get<1>(result) << std::endl;
            std::cout << "result(2): " << std::get<2>(result) << std::endl;
            std::cout << "result(3): " << std::get<3>(result) << std::endl;
            std::cout << "radius: " << radius << std::endl;
            std::cout << "z = zlope*radius + z0: " << radius * fitparams[3] + fitparams[4] << std::endl;
            std::cout << "z = zlope*radius + z0: " << radius * fitparams[3] + fitparams[4] << std::endl;
           // std::cout << "z = zlope*(x^2+y^2) + z0: " << radius * ((std::get<0>(result) * std::get<0>(result)) + std::get<1>(result)*std::get<1>(result))+ fitparams[4] << std::endl;
          }

          std::cout << "Cluster local X: " << cluster->getLocalX() << std::endl;
          std::cout << "Cluster local Y: " << cluster->getLocalY() << std::endl;




          //drift velocity
          double surfaceZCenter = 52.89; //this is where G4 thinks the surface center is in cm
          double drift_velocity = 8.0e-3;  // cm/ns
          double zdriftlength = cluster->getLocalY() * drift_velocity; //cm
          double zloc = surfaceZCenter - zdriftlength; //local z relative to surface center
          unsigned int side = TpcDefs::getSide(cluster_key);
          if(side == 0) zloc = -zloc;
          Acts::Vector3 glob;
          Acts::Vector2 local(cluster->getLocalX(), zloc);
          glob = surface->localToGlobal(tgeometry->geometry().getGeoContext(),
				  local * Acts::UnitConstants::cm,
				  Acts::Vector3(1,1,1));
          std::cout << "Zloc: " << zloc << std::endl;
          std::cout << "glob(0) before constants: " << glob(0) << std::endl;
          std::cout << "glob(1) before constants: " << glob(1) << std::endl;
          std::cout << "glob(2) before constants: " << glob(2) << std::endl;
          glob /= Acts::UnitConstants::cm;
          std::cout << "glob(0) after constants: " << glob(0) << std::endl;
          std::cout << "glob(1) after constants: " << glob(1) << std::endl;
          std::cout << "glob(2) after constants: " << glob(2) << std::endl;


          //surface->transform(tGeometry->geometry().getGeoContext()).inverse() * global;
          // localFromGlobalResult;
          
          double surfaceTolerance = 1e-4;
          Acts::Result<Acts::Vector2> localFromGlobalResult = surface->globalToLocal(tgeometry->geometry().getGeoContext(), glob * Acts::UnitConstants::cm, Acts::Vector3(1,1,1), surfaceTolerance);
          Acts::Vector2 localFromGlobal = *localFromGlobalResult;
          std::cout << "localFromGlobal(0): " << localFromGlobal(0) << std::endl;
          std::cout << "localFromGlobal(1): " << localFromGlobal(1) << std::endl;
        
        
          Acts::Vector3 localFromGlobalNoTolerance = (surface->transform(tgeometry->geometry().getGeoContext())).inverse() * glob;
          std::cout << "localFromGlobalNoTolerance(0): " << localFromGlobalNoTolerance(0) << std::endl;
          std::cout << "localFromGlobalNoTolerance(1): " << localFromGlobalNoTolerance(1) << std::endl;
          std::cout << "localFromGlobalNoTolerance(2): " << localFromGlobalNoTolerance(2) << std::endl;

          std::cout << "localFromGlobalNoTolerance(1) corrected: " << surfaceZCenter + localFromGlobalNoTolerance(1)/drift_velocity << std::endl;
          
      //}
    }
 // }
  }

  
  if(trackContainer->get_does_silicon_seed_exist()){
    std::cout << "We are about to loop over cluster keys in Silicon Seed" << std::endl;
  SiliconSeed->identify();
    for( auto key_iter = SiliconSeed->begin_cluster_keys(); key_iter != SiliconSeed->end_cluster_keys(); ++key_iter )
    {
      const auto& cluster_key = *key_iter;
      auto cluster = m_cluster_map->findCluster( cluster_key );
      if( !cluster )
      {
        std::cout << "DSTTrackArrayWriter::evaluate_tracks - unable to find cluster for key " << cluster_key << std::endl;
        continue;
      }
      //store cluster key
      //silicon_cluster_keys->insert_cluster_key(cluster_key);

      //store information in track array
      std::cout << "Silicon clusterkey: " << cluster_key <<"\n";

      std::cout << "Silicon subsurfkey: " << cluster->getSubSurfKey() << std::endl;
      uint8_t layer = TrkrDefs::getLayer(cluster_key);

      std::cout << "Silicon layer: " << unsigned(TrkrDefs::getLayer(cluster_key)) << std::endl;
      //std::cout << "cluster position 0: " << cluster->getPosition(0) <<"\n";
      trackContainer->setLocalX(layer, cluster->getPosition(0));
      trackContainer->setLocalY(layer, cluster->getPosition(1));
      trackContainer->setSubSurfKey(layer, cluster->getSubSurfKey());
      trackContainer->setPhiError(layer, cluster->getRPhiError());
      trackContainer->setZError(layer, cluster->getZError());
      trackContainer->setAdc(layer, cluster->getAdc());
      trackContainer->setMaxAdc(layer, cluster->getMaxAdc());
      trackContainer->setPhiSize(layer, cluster->getPhiSize());
      trackContainer->setZSize(layer, cluster->getZSize());
      trackContainer->setOverlap(layer, cluster->getOverlap());
      trackContainer->setEdge(layer, cluster->getEdge());
      //trackContainer->setClusKey(layer, cluster_key);

      std::cout << "Silicon Side: " << unsigned(TrkrDefs::getZElement(cluster_key)) << std::endl;
      std::cout << "Silicon Sector Id: " << unsigned(TrkrDefs::getPhiElement(cluster_key)) << std::endl;

      trackContainer->setSide(layer, TrkrDefs::getZElement(cluster_key));
      trackContainer->setSectorId(layer, TrkrDefs::getPhiElement(cluster_key));

      //trackContainer->setClusKey(layer, cluster_key);

      
    }
  }
    //Start a loop over clusters to store them in container associated with each track
    //ConstClusterKeyIter find_cluster_key(TrkrDefs::cluskey clusterid) const override { return m_cluster_keys.find(clusterid); }
    /*
    
    MAKE A LOOP OVER CLUSTERS HERE
    Maybe go into evaluate_clusters
    */
    
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

    TrkrDefs::hitsetkey m_hitsetkey;
    };
    */
    

    

    /*
    trackContainer->clusterArray[layer].m_local[0] = cluster->getPosition(0);
    trackContainer->clusterArray[layer].m_local[1] = cluster->getPosition(1);
    trackContainer->clusterArray[layer].m_subsurfkey = cluster->getSubSurfKey();
    trackContainer->clusterArray[layer].m_phierr = cluster->getRPhiError();
    trackContainer->clusterArray[layer].m_zerr = cluster->getZError();
    trackContainer->clusterArray[layer].m_adc = cluster->getAdc();
    trackContainer->clusterArray[layer].m_maxadc = cluster->getMaxAdc();
    trackContainer->clusterArray[layer].m_phisize = cluster->getPhiSize();
    trackContainer->clusterArray[layer].m_zsize = cluster->getZSize();
    trackContainer->clusterArray[layer].m_overlap = cluster->getOverlap();
    trackContainer->clusterArray[layer].m_edge = cluster->getEdge();
    */
    


    // running iterator over track states, used to match a given cluster to a track state
    



  /*
    // loop over clusters
    for( auto key_iter = track->begin_cluster_keys(); key_iter != track->end_cluster_keys(); ++key_iter )
    {

      const auto& cluster_key = *key_iter;
      auto cluster = m_cluster_map->findCluster( cluster_key );
      if( !cluster )
      {
        std::cout << "DSTWriter::evaluate_tracks - unable to find cluster for key " << cluster_key << std::endl;
        continue;
      }

      //store information in track array
      layer = TrkrDefs::getLayer(cluster_key);
      trackContainer->setLocalX(layer, cluster->getPosition(0));
      trackContainer->setLocalY(layer, cluster->getPosition(1));
      trackContainer->setSubSurfKey(layer, cluster->getSubSurfKey());
      trackContainer->setPhiError(layer, cluster->getRPhiError());
      trackContainer->setZError(layer, cluster->getZError());
      trackContainer->setAdc(layer, cluster->getAdc());
      trackContainer->setMaxAdc(layer, cluster->getMaxAdc());
      trackContainer->setPhiSize(layer, cluster->getPhiSize());
      trackContainer->setZSize(layer, cluster->getZSize());
      trackContainer->setOverlap(layer, cluster->getOverlap());
      trackContainer->setEdge(layer, cluster->getEdge());
*/
       
      /*
      // find track state that is the closest to cluster
      // this assumes that both clusters and states are sorted along r
      const auto radius( cluster_struct.r );
      float dr_min = -1;
      for( auto iter = state_iter; iter != track->end_states(); ++iter )
      {
        const auto dr = std::abs( radius - get_r( iter->second->get_x(), iter->second->get_y() ) );
        if( dr_min < 0 || dr < dr_min )
        {
          state_iter = iter;
          dr_min = dr;
        } else break;
      }

      // store track state in cluster struct
      add_trk_information( cluster_struct, state_iter->second );

      // add to track
      track_struct.clusters.push_back( cluster_struct );
      */
      m_track_array_container->add_trackarray(iTrk, trackContainer);
      std::cout<< "track chisq: " << trackContainer->get_chisq() << std::endl;
      std::cout<< "container chisq: " << (m_track_array_container->get_trackarray(iTrk))->get_chisq() << std::endl;


  /*
      std::cout<< "cluster 1 local x: " << trackContainer->getLocalX(1) << std::endl;
      std::cout<< "container 1 local x: " << (m_track_array_container->get_trackarray(iTrk))->getLocalX(1) << std::endl;

      std::cout<< "cluster 7 Subsurf: " << trackContainer->getSubSurfKey(7) << std::endl;
      std::cout<< "container 7 Subsurf: " << (m_track_array_container->get_trackarray(iTrk))->getSubSurfKey(7) << std::endl;

      std::cout<< "cluster 7 is valid: " << trackContainer->getValid(7) << std::endl;
  */
      delete trackContainer;
      ++iTrk;
      //++iKey;
      std::cout << "end of loop" << "\n";
    }
    

     //m_track_map->clear();
     m_track_map->Reset();
     m_cluster_map->Reset();
     
     //m_cluster_map->Clear();


    //cluster->getSubSurfKey();
    //get a cluskey from a subsurfkey
    //TPCSeed->find_cluster_key(cluskey);

    



}


Acts::Vector3 DSTTrackArrayWriter::get_helix_surface_intersection(Surface surf, std::vector<float>& fitpars, Acts::Vector3 global)
{
  // we want the point where the helix intersects the plane of the surface

  // get the plane of the surface
  Acts::Vector3 sensorCenter      = surf->center(tgeometry->geometry().getGeoContext()) * 0.1;  // convert to cm
  Acts::Vector3 sensorNormal    = -surf->normal(tgeometry->geometry().getGeoContext());
  sensorNormal /= sensorNormal.norm();

  // there are analytic solutions for a line-plane intersection.
  // to use this, need to get the vector tangent to the helix near the measurement and a point on it.
  std::pair<Acts::Vector3, Acts::Vector3> line = TrackFitUtils::get_helix_tangent(fitpars, global);
  Acts::Vector3 pca = line.first;
  Acts::Vector3 tangent = line.second;
  float d = (sensorCenter - pca).dot(sensorNormal) / tangent.dot(sensorNormal);
  Acts::Vector3 intersection = pca + d * tangent;

  return intersection;
}
