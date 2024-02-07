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

#include <algorithm>
#include <bitset>
#include <cassert>
#include <iostream>
#include <numeric>

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
    evaluate_track_and_clusters();




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

  // get necessary nodes
  m_track_map = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  
  m_cluster_map = findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");

  m_track_array_container = findNode::getClass<SvtxTrackArrayContainer_v1>(topNode, "SvtxTrackArrayContainer");

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

      std::cout << "TPC Side: " << unsigned(TrkrDefs::getZElement(cluster_key)) << std::endl;
      std::cout << "TPC Sector Id: " << unsigned(TrkrDefs::getPhiElement(cluster_key)) << std::endl;

      trackContainer->setValid(layer, true);

      if(trackContainer->getValid(layer)){
        std::cout << "Valid is true" << std::endl;
      }

      trackContainer->setSide(layer, TrkrDefs::getZElement(cluster_key));
      trackContainer->setSectorId(layer, TrkrDefs::getPhiElement(cluster_key));
      //trackContainer->setClusKey(layer, cluster_key);

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
