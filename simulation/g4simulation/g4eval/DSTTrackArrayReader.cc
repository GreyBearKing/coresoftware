/*!
 * \file DSTTrackArrayReader.cc
 * \author Alex Patton <aopatton@mit.edu>
 */

#include "DSTTrackArrayReader.h"
#include <fun4all/Fun4AllReturnCodes.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <trackbase/InttDefs.h>
#include <intt/InttClusterizer.h>
#include <micromegas/MicromegasDefs.h>
#include <trackbase/MvtxDefs.h>
#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHNodeIterator.h>
#include <trackbase/TpcDefs.h>
#include <trackbase/TrkrDefs.h>
//#include <trackbase/TrkrCluster.h>
//#include <trackbase/TrkrClusterv3.h>
#include <trackbase/TrkrClusterv4.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrClusterContainerv4.h>
#include <trackbase/TrkrClusterHitAssoc.h>
#include <trackbase/TrkrHit.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/TrkrHitTruthAssoc.h>
#include <trackbase_historic/TrackSeedContainer_v1.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxTrackMap_v2.h>
#include <trackbase_historic/SvtxTrack_v4.h>
#include <trackbase_historic/SvtxTrackState_v1.h>
#include <trackbase_historic/SvtxTrackArray_v1.h>

#include <trackbase/ActsGeometry.h>
#include <trackbase_historic/ActsTransformations.h>
//#include <trackbase_historic/TrackSeed_v1.h>

#include <algorithm>
#include <bitset>
#include <cassert>
#include <iostream>
#include <numeric>
#include <TVector3.h>
//#define __STDC_LIMIT_MACROS

//_____________________________________________________________________

//_____________________________________________________________________
DSTTrackArrayReader::DSTTrackArrayReader(const std::string& name):
  SubsysReco( name)
{}

//_____________________________________________________________________
int DSTTrackArrayReader::Init(PHCompositeNode* topNode )
{

  PHNodeIterator iter(topNode);
  auto dstNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode)
  {
    std::cout << "DSTTrackArrayReader::Init - DST Node missing" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  // get EVAL node
  iter = PHNodeIterator(dstNode);
  auto evalNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "EVAL"));
  if( !evalNode )
  {
    // create
    std::cout << "DSTTrackArrayReader::Init - EVAL node missing - creating" << std::endl;
    evalNode = new PHCompositeNode( "EVAL" );
    dstNode->addNode(evalNode);
  }


  auto svtxNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "SVTX"));
  if( !svtxNode )
  {
    // create
    std::cout << "DSTTrackArrayReader::Init - SVTX node missing - creating" << std::endl;
    svtxNode = new PHCompositeNode( "SVTX" );
    dstNode->addNode(svtxNode);
  }


  auto trkrNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "TRKR"));
  if( !trkrNode )
  {
    // create
    std::cout << "DSTTrackArrayReader::Init - TRKR node missing - creating" << std::endl;
    trkrNode = new PHCompositeNode( "TRKR" );
    dstNode->addNode(trkrNode);
  }

  auto trackNode = findNode::getClass<SvtxTrackMap>(svtxNode, "SvtxTrackMap");
  if (!trackNode)
  {
      auto newTrackNode = new PHIODataNode<PHObject>(new SvtxTrackMap_v2, "SvtxTrackMap", "PHObject");
    svtxNode->addNode(newTrackNode);
  }

  auto clsNode = findNode::getClass<TrkrClusterContainer>(trkrNode, "TRKR_CLUSTER");
  if (!clsNode)
  {
    auto newClsNode = new PHIODataNode<PHObject>(new TrkrClusterContainerv4, "TRKR_CLUSTER", "PHObject");
    trkrNode->addNode(newClsNode);
  }


  auto tpcNode = findNode::getClass<TrackSeedContainer>(svtxNode, "TpcTrackSeedContainer");
  if (!tpcNode)
  {
    auto newTpcNode = new PHIODataNode<PHObject>(new TrackSeedContainer_v1, "TpcTrackSeedContainer", "PHObject");
    svtxNode->addNode(newTpcNode);
  }

  auto siliconNode = findNode::getClass<TrackSeedContainer>(svtxNode, "SiliconTrackSeedContainer");
  if (!siliconNode)
  {
    auto newSiliconNode = new PHIODataNode<PHObject>(new TrackSeedContainer_v1, "SiliconTrackSeedContainer", "PHObject");
    svtxNode->addNode(newSiliconNode);
  }


  //TrackArray container
  m_track_array_container = findNode::getClass<SvtxTrackArrayContainer_v1>(topNode, "SvtxTrackArrayContainer");

  //m_container = findNode::getClass<DSTContainer>(topNode, "DSTContainer");

  // svtxtrackmap constructer is protected
  // auto svtxNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "SVTX"));
  // auto newNode = new PHIODataNode<PHObject>( new SvtxTrackMap, "SvtxTrackMap_2","PHObject");
  // svtxNode->addNode(newNode);




  return Fun4AllReturnCodes::EVENT_OK;
}

//_____________________________________________________________________
int DSTTrackArrayReader::InitRun(PHCompositeNode*)
{ return Fun4AllReturnCodes::EVENT_OK; }

//_____________________________________________________________________
int DSTTrackArrayReader::process_event(PHCompositeNode* topNode)
{

  //Init(topNode);
  // load nodes
  auto res =  load_nodes(topNode);
  //std::cout <<"This is before I fill cluster map" << std::endl;
  //m_cluster_map->identify();
  if( res != Fun4AllReturnCodes::EVENT_OK ) 
  {
    return res;
  }
 //evaluate_track_and_clusters();

  evaluate_track_and_cluster_residuals();

  m_track_array_container->Reset();
 
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//_____________________________________________________________________
int DSTTrackArrayReader::End(PHCompositeNode*)
{
  std::cout << "DST reader finishes" << "\n";
  //delete TPCSeed;
  //delete SiliconSeed;
  //delete m_cluster;
  //delete trackContainer;

  return Fun4AllReturnCodes::EVENT_OK; }

//_____________________________________________________________________
int DSTTrackArrayReader::load_nodes( PHCompositeNode* topNode)
{
  tgeometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");

    if (!tgeometry)
    {
      std::cout << PHWHERE << "No Acts geometry on node tree. Can't  continue."
                << std::endl;
    }
  // get necessary nodes
  //m_track_map = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  
  // get necessary nodes
  m_track_map = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");

  // cluster map
  m_cluster_map = findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");
  m_tpc_seed_container = findNode::getClass<TrackSeedContainer>(topNode, "TpcTrackSeedContainer");
  m_silicon_seed_container = findNode::getClass<TrackSeedContainer>(topNode, "SiliconTrackSeedContainer");
  m_track_array_container = findNode::getClass<SvtxTrackArrayContainer_v1>(topNode, "SvtxTrackArrayContainer");

  tpcGeom = findNode::getClass<PHG4TpcCylinderGeomContainer>(topNode, "CYLINDERCELLGEOM_SVTX");


  return Fun4AllReturnCodes::EVENT_OK;

}
 


void DSTTrackArrayReader::evaluate_track_and_clusters()
{

      if(m_track_map){
        std::cout << "m_track_map exists"
                << "\n";
      }
      if(m_cluster_map){
        std::cout << "m_cluster_map exists"
                << "\n";
      }

      if (!(m_track_map&&m_cluster_map&&m_track_array_container)) {
        return;
      }



        //there should only be as many trackContainers as layers
        //std:: cout << "ArrayKeysSize: " << m_container->getArrayKeysSize() << std::endl;
        for(int iTrk = 0; iTrk < ((int)m_track_array_container->size()); iTrk++){
          //auto track = (SvtxTrack_v4*)m_container->arrTrkArr->ConstructedAt(iTrk);
          //unsigned int key = m_container->getArrayKeys(iTrk);
         
          //m_track_map->insert(track);
          std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__ << std::endl;
          trackContainer = (SvtxTrackArray_v1*)m_track_array_container->get_trackarray(iTrk);
          
          //now put this information back into original container

          //make a track and store information into it
          SvtxTrack_v4* track = new SvtxTrack_v4();
          std::cout << "track id: " << trackContainer->get_id() << std::endl;


          std::cout << "track chisq: " << trackContainer->get_chisq() << std::endl;

          track->set_id(trackContainer->get_id());
          track->set_vertex_id(trackContainer->get_vertex_id());
          track->set_positive_charge(trackContainer->get_positive_charge());
          track->set_chisq(trackContainer->get_chisq());
          track->set_ndf(trackContainer->get_ndf());
          track->set_crossing(trackContainer->get_crossing());

          //insert PCA point, should be included in constructor
          //track->insert_state(new SvtxTrackState_v1(0));

          track->set_x(trackContainer->get_x());
          track->set_y(trackContainer->get_y());
          track->set_z(trackContainer->get_z());
          track->set_px(trackContainer->get_px());
          track->set_py(trackContainer->get_py());
          track->set_pz(trackContainer->get_pz());

          /*
          for( auto iter = trackContainer->begin_states(); iter != trackContainer->end_states(); ++iter )
          { track->insert_state(static_cast<SvtxTrackState*>(iter->second->CloneMe() ) ) ; }  
          */
          //Make TPCSeed and SiliconSeed
          //TrackSeed_v1* TPCSeed = new TrackSeed_v1();
          //TrackSeed_v1* SiliconSeed = new TrackSeed_v1();
          //TrackSeed_v1 TPCSeedObject;
          TPCSeed = new TrackSeed_v1();
          //TPCSeed = &TPCSeedObject;
          SiliconSeed = new TrackSeed_v1();

          std::cout << "Before TPC seeds" << std::endl;
          //set TPC Seed
          if(trackContainer->get_does_tpc_seed_exist()){
            TPCSeed->set_qOverR(trackContainer->tpc_seed_get_qOverR());
            TPCSeed->set_X0(trackContainer->tpc_seed_get_X0());
            TPCSeed->set_Y0(trackContainer->tpc_seed_get_Y0());
            TPCSeed->set_slope(trackContainer->tpc_seed_get_slope());
            TPCSeed->set_Z0(trackContainer->tpc_seed_get_Z0());
            TPCSeed->set_crossing(trackContainer->tpc_seed_get_crossing());

          
            /*
            for(int TPClayer = 7; TPClayer < 59; TPClayer++){
                if(trackContainer->getClusKey(TPClayer)==0){
              //std::cout << "No cluster at layer: " << layer << std::endl;
              continue;
            }
                
                TPCSeed->insert_cluster_key(trackContainer->getClusKey(TPClayer));
            }
            //set TPCSeed
            track->set_tpc_seed(TPCSeed);
            m_tpc_seed_container->insert(TPCSeed);
            */
          }
          std::cout << "Before silicon seeds" << std::endl;
          //set Silicon Seed
          if(trackContainer->get_does_silicon_seed_exist()){
            std::cout << "Inside silicon seeds" << std::endl;
            SiliconSeed->set_qOverR(trackContainer->silicon_seed_get_qOverR());
            SiliconSeed->set_X0(trackContainer->silicon_seed_get_X0());
            SiliconSeed->set_Y0(trackContainer->silicon_seed_get_Y0());
            SiliconSeed->set_slope(trackContainer->silicon_seed_get_slope());
            SiliconSeed->set_Z0(trackContainer->silicon_seed_get_Z0());
            SiliconSeed->set_crossing(trackContainer->silicon_seed_get_crossing());

            
            /*
            std::cout << "Before silicon seeds' clusters" << std::endl;
            for(int Siliconlayer = 0; Siliconlayer < 7; Siliconlayer++){
               
               if(trackContainer->getClusKey(Siliconlayer)==0){
              //std::cout << "No cluster at layer: " << layer << std::endl;
              continue;
            }
               SiliconSeed->insert_cluster_key(trackContainer->getClusKey(Siliconlayer));
            }
            track->set_silicon_seed(SiliconSeed);
            m_silicon_seed_container->insert(SiliconSeed);
            */
          }
          
          //add the seeds to track object
          //std::cout << "Before setting the seeds \n";
          
          
          //std::cout << "After setting the seeds \n";
          //make a cluster key set to load into this

          //delete pointers
          //delete TPCSeed;
          //delete SiliconSeed;
          //generate new clusterkey WIP

          //load it back into m_track_map
          
          //m_track_map->insert(track);


          for(int layer = 0; layer < 59; layer++){
            //check if clusterkey is default and if so then skip
            // this is 9999999999999999 in hexidecimal
            //11068046444225730969
            if(!trackContainer->getValid(layer)){
              std::cout << "No cluster at layer: " << layer << std::endl;
              continue;
            }
            std::cout << "Valid cluster at layer " << layer << ": " << trackContainer->getValid(layer) << std::endl;
            

            //std::cout << "layer "<< layer << " clusterKey: " << clusterkey << std::endl;
            std::cout << "layer "<< layer << std::endl;
            //trackContainer->getClusKey(layer) 
            //TrkrDefs::cluskey clusterkey = trackContainer->getClusKey(layer);
            //set clusterkey
            uint8_t TrkrId = 0;
            if(layer < 3){
              TrkrId = 0;  //mvtx
            }else if(layer >= 3 && layer <7){
              TrkrId = 1; //intt
            }else if(layer >= 7 && layer <55){
              TrkrId = 2; //tpc
            }else{
              TrkrId = 3; //higher layer
            }
            uint8_t layerbyte = (uint8_t) layer;

            std::cout << "Trkr Id: " << unsigned(TrkrId) << std::endl;
            std::cout << "Layer byte: " << unsigned(layerbyte) << std::endl;
            std::cout << "Sector Id: " << unsigned(trackContainer->getSectorId(layer)) << std::endl;
            std::cout << "Side: " << unsigned(trackContainer->getSide(layer)) << std::endl;
            

            std::cout << "Generated hitsetkey: " << ((uint32_t) TrkrId << 24) + ((uint32_t) layerbyte << 16 ) + ((uint32_t) trackContainer->getSectorId(layer) << 8) + trackContainer->getSide(layer) << std::endl;
            //(TrkrId <<24)+(layerbyte << 16 )+subsurfkey;
            TrkrDefs::hitsetkey hitsetkey = ((uint32_t)TrkrId << 24)+((uint32_t)layerbyte << 16 )+ + ((uint32_t) trackContainer->getSectorId(layer) << 8) + trackContainer->getSide(layer);
            if(hitsetkey != TrkrDefs::getHitSetKeyFromClusKey(trackContainer->getClusKey(layer))){
              std::cout << "Hitsetkey does not match original" << std::endl;
              std::cout << "Original: " << TrkrDefs::getHitSetKeyFromClusKey(trackContainer->getClusKey(layer)) << std::endl;
              std::cout << "Generated: " << hitsetkey << std::endl;

              std::cout << "Sector from generated: " << TrkrDefs::getPhiElement(hitsetkey) << std::endl;
              std::cout << "Side from generated: " << TrkrDefs::getZElement(hitsetkey) << std::endl;
            }
            
            
            //TrkrDefs::hitsetkey hitsetkey = ((uint32_t) TrkrId << 24) + ((uint32_t) layerbyte << 16 ) + ((uint32_t) trackContainer->getSectorId(layer) << 8) + trackContainer->getSide(layer);
          
          
          //make cluster object
          //TrkrClusterv5* m_cluster = new TrkrClusterv5();
          m_cluster = new TrkrClusterv5();

          //layer = TrkrDefs::getLayer(cluster_key);
          m_cluster->setLocalX(trackContainer->getLocalX(layer));
          m_cluster->setLocalY(trackContainer->getLocalY(layer));
          m_cluster->setSubSurfKey(trackContainer->getSubSurfKey(layer));
          m_cluster->setPhiError(trackContainer->getRPhiError(layer));
          m_cluster->setZError(trackContainer->getZError(layer));
          m_cluster->setAdc(trackContainer->getAdc(layer));
          m_cluster->setMaxAdc(trackContainer->getMaxAdc(layer));
          m_cluster->setPhiSize(trackContainer->getPhiSize(layer));
          m_cluster->setZSize(trackContainer->getZSize(layer));
          m_cluster->setOverlap(trackContainer->getOverlap(layer));
          m_cluster->setEdge(trackContainer->getEdge(layer));

          //TrkrDefs::hitsetkey = trackContainer->setSubSurfKey(layer);
          //load clusters back into m_cluster_map

          //key should be cluskey

          //comment out generating key
          


          //clus key is hitsetkey for upper 32 bits
          // and cluster id for lower 32 bits
          bool foundCluster = false;
          //trackseed lists cluster keys
          for(uint32_t index = 0; index < 1000 && !foundCluster; index++){
            TrkrDefs::cluskey clusterkey = ((uint64_t)hitsetkey << 32) + (uint64_t)index;
            
            if(!m_cluster_map->findCluster(clusterkey)){
              std::cout << "Index: " << index << std::endl;
              std::cout << "generated clusterkey: " << clusterkey << std::endl;
              std::cout << "Clusterkey layer: " << unsigned(TrkrDefs::getLayer(clusterkey)) << std::endl;
              std::cout << "Clusterkey sector Id: " << unsigned(TrkrDefs::getPhiElement(clusterkey)) << std::endl;
              std::cout << "Clusterkey side: " << unsigned(TrkrDefs::getZElement(clusterkey)) << std::endl;
              std::cout << "m_cluster zSize: " << m_cluster->getZSize() << std::endl;
              std::cout << "m_cluster zError: " << m_cluster->getZError() << std::endl;
              std::cout << "m_cluster local x: " << m_cluster->getLocalX() << std::endl;
              std::cout << "m_cluster local y: " << m_cluster->getLocalY() << std::endl;

              std::cout << "identify cluster" << std::endl;
              m_cluster->identify();
              m_cluster_map->addClusterSpecifyKey(clusterkey, m_cluster);
              foundCluster = true;



              //fill TPC and silicon seeds
              
              if(layer < 7){
                SiliconSeed->insert_cluster_key(clusterkey);
              }else{
                TPCSeed->insert_cluster_key(clusterkey);
              }
                
                
            }
            

          }
          //delete m_cluster;
          //m_cluster_map->identify();
          }
          if(trackContainer->get_does_silicon_seed_exist()){
            track->set_silicon_seed(SiliconSeed);
          
            m_silicon_seed_container->insert(SiliconSeed);
          }else{
            track->set_silicon_seed(nullptr);
          }
          if(trackContainer->get_does_tpc_seed_exist()){
            track->set_tpc_seed(TPCSeed);
            m_tpc_seed_container->insert(TPCSeed);
          }else{
            track->set_tpc_seed(nullptr);
          }
          m_tpc_seed_container->identify();
          //m_track_map->insertWithKey(track,key);
          //insert without key
          m_track_map->insert(track);
          //delete TPCSeed;
          //delete SiliconSeed;
          }
          //delete TPCSeed;
          //delete SiliconSeed;
          m_cluster_map->identify();

      /*
          std::vector<Acts::Vector3> clusterPositions;
          std::vector<TrkrDefs::cluskey> clusterKeys;
          clusterKeys.insert(clusterKeys.end(), tpcseed->begin_cluster_keys(),
			    tpcseed->end_cluster_keys());
          if(silseed)
	          {
	           clusterKeys.insert(clusterKeys.end(), silseed->begin_cluster_keys(),
			         silseed->end_cluster_keys());
	          }
          TrackFitUtils::getTrackletClusters(tgeometry, m_clustermap, 
					 clusterPositions, clusterKeys);
          std::vector<float> fitparams = TrackFitUtils::fitClusters(clusterPositions, clusterKeys);

*/          
/*
      std::vector<Acts::Vector3> clusterPositions;
      std::vector<TrkrDefs::cluskey> clusterKeys;
      clusterKeys.insert(clusterKeys.end(), tpcseed->begin_cluster_keys(),
			 tpcseed->end_cluster_keys());
      if(silseed)
	{
	  clusterKeys.insert(clusterKeys.end(), silseed->begin_cluster_keys(),
			     silseed->end_cluster_keys());
	}
      TrackFitUtils::getTrackletClusters(tgeometry, m_cluster_map, 
					 clusterPositions, clusterKeys);

        for (const auto& hitsetkey : m_cluster_map->getHitSetKeys())
      {
        //int hitsetlayer = TrkrDefs::getLayer(hitsetkey);
        auto range = m_cluster_map->getClusters(hitsetkey);
        for (auto iter = range.first; iter != range.second; ++iter)
        {
          TrkrDefs::cluskey cluster_key = iter->first;
          TrkrCluster* cluster = m_cluster_map->findCluster(cluster_key);
	       // SvtxTrack* track = nullptr;//best_track_from(cluster_key);
          //float niter = 0;
         
          //float hitID = (float) cluster_key;
          cluster->identify();
          std::cout << "Hitsetkey going into tgeometry: " << hitsetkey << std::endl;
          std::cout << "Subsurfkey going into tgeometry: " << cluster->getSubSurfKey() << std::endl;
          Acts::Vector3 cglob;
          cglob = tgeometry->getGlobalPosition(cluster_key, cluster);

          //fill fitpars and surface
          //std::vector<float> fitparams = TrackFitUtils::fitClusters(clusterPositions, clusterKeys);

          //now that we have global position, calculate helical fitter based off surface
          //Acts::Vector3 intersection = HelicalFitter::get_helix_surface_intersection(surface, fitpars, cglob);
      }

        }
*/

}
 
void DSTTrackArrayReader::evaluate_track_and_cluster_residuals(){
  if(m_track_map){
        std::cout << "m_track_map exists"
                << "\n";
      }
      if(m_cluster_map){
        std::cout << "m_cluster_map exists"
                << "\n";
      }

      if (!(m_track_map&&m_cluster_map&&m_track_array_container)) {
        return;
      }



        //there should only be as many trackContainers as layers
        //std:: cout << "ArrayKeysSize: " << m_container->getArrayKeysSize() << std::endl;
        for(int iTrk = 0; iTrk < ((int)m_track_array_container->size()); iTrk++){
          //auto track = (SvtxTrack_v4*)m_container->arrTrkArr->ConstructedAt(iTrk);
          //unsigned int key = m_container->getArrayKeys(iTrk);
         
          //m_track_map->insert(track);
          std::cout << __FILE__ << "::" << __func__ << "::" << __LINE__ << std::endl;
          trackContainer = (SvtxTrackArray_v1*)m_track_array_container->get_trackarray(iTrk);
          
          //now put this information back into original container

          //make a track and store information into it
          SvtxTrack_v4* track = new SvtxTrack_v4();
          std::cout << "track id: " << trackContainer->get_id() << std::endl;


          std::cout << "track chisq: " << trackContainer->get_chisq() << std::endl;

          track->set_id(trackContainer->get_id());
          track->set_vertex_id(trackContainer->get_vertex_id());
          track->set_positive_charge(trackContainer->get_positive_charge());
          track->set_chisq(trackContainer->get_chisq());
          track->set_ndf(trackContainer->get_ndf());
          track->set_crossing(trackContainer->get_crossing());

          //insert PCA point, should be included in constructor
          //track->insert_state(new SvtxTrackState_v1(0));

          track->set_x(trackContainer->get_x());
          track->set_y(trackContainer->get_y());
          track->set_z(trackContainer->get_z());
          track->set_px(trackContainer->get_px());
          track->set_py(trackContainer->get_py());
          track->set_pz(trackContainer->get_pz());

          /*
          for( auto iter = trackContainer->begin_states(); iter != trackContainer->end_states(); ++iter )
          { track->insert_state(static_cast<SvtxTrackState*>(iter->second->CloneMe() ) ) ; }  
          */
          //Make TPCSeed and SiliconSeed
          //TrackSeed_v1* TPCSeed = new TrackSeed_v1();
          //TrackSeed_v1* SiliconSeed = new TrackSeed_v1();
          //TrackSeed_v1 TPCSeedObject;
          TPCSeed = new TrackSeed_v1();
          //TPCSeed = &TPCSeedObject;
          SiliconSeed = new TrackSeed_v1();

          std::cout << "Before TPC seeds" << std::endl;
          //set TPC Seed
          if(trackContainer->get_does_tpc_seed_exist()){
            TPCSeed->set_qOverR(trackContainer->tpc_seed_get_qOverR());
            TPCSeed->set_X0(trackContainer->tpc_seed_get_X0());
            TPCSeed->set_Y0(trackContainer->tpc_seed_get_Y0());
            TPCSeed->set_slope(trackContainer->tpc_seed_get_slope());
            TPCSeed->set_Z0(trackContainer->tpc_seed_get_Z0());
            TPCSeed->set_crossing(trackContainer->tpc_seed_get_crossing());

          
            /*
            for(int TPClayer = 7; TPClayer < 59; TPClayer++){
                if(trackContainer->getClusKey(TPClayer)==0){
              //std::cout << "No cluster at layer: " << layer << std::endl;
              continue;
            }
                
                TPCSeed->insert_cluster_key(trackContainer->getClusKey(TPClayer));
            }
            //set TPCSeed
            track->set_tpc_seed(TPCSeed);
            m_tpc_seed_container->insert(TPCSeed);
            */
          }
          std::cout << "Before silicon seeds" << std::endl;
          //set Silicon Seed
          if(trackContainer->get_does_silicon_seed_exist()){
            std::cout << "Inside silicon seeds" << std::endl;
            SiliconSeed->set_qOverR(trackContainer->silicon_seed_get_qOverR());
            SiliconSeed->set_X0(trackContainer->silicon_seed_get_X0());
            SiliconSeed->set_Y0(trackContainer->silicon_seed_get_Y0());
            SiliconSeed->set_slope(trackContainer->silicon_seed_get_slope());
            SiliconSeed->set_Z0(trackContainer->silicon_seed_get_Z0());
            SiliconSeed->set_crossing(trackContainer->silicon_seed_get_crossing());

            
            /*
            std::cout << "Before silicon seeds' clusters" << std::endl;
            for(int Siliconlayer = 0; Siliconlayer < 7; Siliconlayer++){
               
               if(trackContainer->getClusKey(Siliconlayer)==0){
              //std::cout << "No cluster at layer: " << layer << std::endl;
              continue;
            }
               SiliconSeed->insert_cluster_key(trackContainer->getClusKey(Siliconlayer));
            }
            track->set_silicon_seed(SiliconSeed);
            m_silicon_seed_container->insert(SiliconSeed);
            */
          }
          
          //add the seeds to track object
          //std::cout << "Before setting the seeds \n";
          
          
          //std::cout << "After setting the seeds \n";
          //make a cluster key set to load into this

          //delete pointers
          //delete TPCSeed;
          //delete SiliconSeed;
          //generate new clusterkey WIP

          //load it back into m_track_map
          
          //m_track_map->insert(track);


          for(int layer = 0; layer < 59; layer++){
            //check if clusterkey is default and if so then skip
            // this is 9999999999999999 in hexidecimal
            //11068046444225730969
            if(!trackContainer->getValid(layer)){
              std::cout << "No cluster at layer: " << layer << std::endl;
              continue;
            }
            std::cout << "Valid cluster at layer " << layer << ": " << trackContainer->getValid(layer) << std::endl;
            

            //std::cout << "layer "<< layer << " clusterKey: " << clusterkey << std::endl;
            std::cout << "layer "<< layer << std::endl;
            //trackContainer->getClusKey(layer) 
            //TrkrDefs::cluskey clusterkey = trackContainer->getClusKey(layer);
            //set clusterkey
            uint8_t TrkrId = 0;
            if(layer < 3){
              TrkrId = 0;  //mvtx
            }else if(layer >= 3 && layer <7){
              TrkrId = 1; //intt
            }else if(layer >= 7 && layer <55){
              TrkrId = 2; //tpc
            }else{
              TrkrId = 3; //higher layer
            }
            uint8_t layerbyte = (uint8_t) layer;

            std::cout << "Trkr Id: " << unsigned(TrkrId) << std::endl;
            std::cout << "Layer byte: " << unsigned(layerbyte) << std::endl;
            std::cout << "Sector Id: " << unsigned(trackContainer->getSectorId(layer)) << std::endl;
            std::cout << "Side: " << unsigned(trackContainer->getSide(layer)) << std::endl;
            

            std::cout << "Generated hitsetkey: " << ((uint32_t) TrkrId << 24) + ((uint32_t) layerbyte << 16 ) + ((uint32_t) trackContainer->getSectorId(layer) << 8) + trackContainer->getSide(layer) << std::endl;
            //(TrkrId <<24)+(layerbyte << 16 )+subsurfkey;
            TrkrDefs::hitsetkey hitsetkey = ((uint32_t)TrkrId << 24)+((uint32_t)layerbyte << 16 )+ + ((uint32_t) trackContainer->getSectorId(layer) << 8) + trackContainer->getSide(layer);
            if(hitsetkey != TrkrDefs::getHitSetKeyFromClusKey(trackContainer->getClusKey(layer))){
              std::cout << "Hitsetkey does not match original" << std::endl;
              std::cout << "Original: " << TrkrDefs::getHitSetKeyFromClusKey(trackContainer->getClusKey(layer)) << std::endl;
              std::cout << "Generated: " << hitsetkey << std::endl;

              std::cout << "Sector from generated: " << TrkrDefs::getPhiElement(hitsetkey) << std::endl;
              std::cout << "Side from generated: " << TrkrDefs::getZElement(hitsetkey) << std::endl;
            }
            
            
            //TrkrDefs::hitsetkey hitsetkey = ((uint32_t) TrkrId << 24) + ((uint32_t) layerbyte << 16 ) + ((uint32_t) trackContainer->getSectorId(layer) << 8) + trackContainer->getSide(layer);
          
          
          //make cluster object
          //TrkrClusterv5* m_cluster = new TrkrClusterv5();
          m_cluster = new TrkrClusterv5();

          //layer = TrkrDefs::getLayer(cluster_key);
          m_cluster->setLocalX(trackContainer->getLocalX(layer));
          m_cluster->setLocalY(trackContainer->getLocalY(layer));
          m_cluster->setSubSurfKey(trackContainer->getSubSurfKey(layer));
          m_cluster->setPhiError(trackContainer->getRPhiError(layer));
          m_cluster->setZError(trackContainer->getZError(layer));
          m_cluster->setAdc(trackContainer->getAdc(layer));
          m_cluster->setMaxAdc(trackContainer->getMaxAdc(layer));
          m_cluster->setPhiSize(trackContainer->getPhiSize(layer));
          m_cluster->setZSize(trackContainer->getZSize(layer));
          m_cluster->setOverlap(trackContainer->getOverlap(layer));
          m_cluster->setEdge(trackContainer->getEdge(layer));

          //TrkrDefs::hitsetkey = trackContainer->setSubSurfKey(layer);
          //load clusters back into m_cluster_map

          //key should be cluskey

          //comment out generating key
          


          //clus key is hitsetkey for upper 32 bits
          // and cluster id for lower 32 bits
          bool foundCluster = false;
          //trackseed lists cluster keys
          for(uint32_t index = 0; index < 1000 && !foundCluster; index++){
            TrkrDefs::cluskey clusterkey = ((uint64_t)hitsetkey << 32) + (uint64_t)index;
            
            if(!m_cluster_map->findCluster(clusterkey)){
              std::cout << "Index: " << index << std::endl;
              std::cout << "generated clusterkey: " << clusterkey << std::endl;
              std::cout << "Clusterkey layer: " << unsigned(TrkrDefs::getLayer(clusterkey)) << std::endl;
              std::cout << "Clusterkey sector Id: " << unsigned(TrkrDefs::getPhiElement(clusterkey)) << std::endl;
              std::cout << "Clusterkey side: " << unsigned(TrkrDefs::getZElement(clusterkey)) << std::endl;
              std::cout << "m_cluster zSize: " << m_cluster->getZSize() << std::endl;
              std::cout << "m_cluster zError: " << m_cluster->getZError() << std::endl;
              std::cout << "m_cluster residual x: " << m_cluster->getLocalX() << std::endl;
              std::cout << "m_cluster reisudal y: " << m_cluster->getLocalY() << std::endl;

              std::cout << "identify cluster" << std::endl;
              
              //fill TPC and silicon seeds
              
              if(layer < 7){
                SiliconSeed->insert_cluster_key(clusterkey);
                
                

              }else{
                TPCSeed->insert_cluster_key(clusterkey);
                //add in extra calculations about the residuals

                //calculate helix surface
                auto surface = tgeometry->maps().getSurface(clusterkey, m_cluster);
                auto geoLayer = tpcGeom->GetLayerCellGeom(layer);
                auto radius = geoLayer->get_radius();
                std::cout << "radius: " << radius << std::endl;
                std::cout << "tpc_R: " << 1.0/abs(trackContainer->tpc_seed_get_qOverR()) << std::endl;
                std::cout << "tpc_X0: " << trackContainer->tpc_seed_get_X0() << std::endl;
                std::cout << "tpc_Y0: " << trackContainer->tpc_seed_get_Y0() << std::endl;

                auto result = TrackFitUtils::circle_circle_intersection(radius, 1.0/abs(trackContainer->tpc_seed_get_qOverR()), trackContainer->tpc_seed_get_X0(), trackContainer->tpc_seed_get_Y0());
                //std::cout << "result(0): " << std::get<0>(result) << std::endl;
                //std::cout << "result(1): " << std::get<1>(result) << std::endl;
                //std::cout << "result(2): " << std::get<2>(result) << std::endl;
                //std::cout << "result(3): " << std::get<3>(result) << std::endl;
                //std::cout << "radius: " << radius << std::endl;
                //std::cout << "z = zlope*radius + z0: " << radius * trackContainer->tpc_seed_get_slope() + trackContainer->tpc_seed_get_Z0() << std::endl;
                //std::cout << "z = zlope*radius + z0: " << radius * fitparams[3] + fitparams[4] << std::endl;
                //std::cout << "z = zlope*(x^2+y^2) + z0: " << radius * ((std::get<0>(result) * std::get<0>(result)) + std::get<1>(result)*std::get<1>(result))+ fitparams[4] << std::endl;


                Acts::Vector3 globalIntersection;
                if(trackContainer->get_is_tpc_helix_intersection_positive()){
                  //use xplus and yplus
                   globalIntersection[0] = std::get<0>(result);
                   globalIntersection[1] = std::get<1>(result); 
                }else{
                  //use xminus and yminus
                  globalIntersection[0] = std::get<2>(result);
                  globalIntersection[1] = std::get<3>(result);
                }
                globalIntersection[2] = radius * trackContainer->tpc_seed_get_slope() + trackContainer->tpc_seed_get_Z0();

                Acts::Vector3 localFromGlobalIntersectionNoTolerance = (surface->transform(tgeometry->geometry().getGeoContext())).inverse() * (globalIntersection * Acts::UnitConstants::cm);
                localFromGlobalIntersectionNoTolerance /=  Acts::UnitConstants::cm;


                //std::cout << "localFromGlobalIntersectionNoTolerance(0): " << localFromGlobalIntersectionNoTolerance(0) << std::endl;
                //std::cout << "localFromGlobalIntersectionNoTolerance(1): " << localFromGlobalIntersectionNoTolerance(1) << std::endl;
                //std::cout << "localFromGlobalIntersectionNoTolerance(2): " << localFromGlobalIntersectionNoTolerance(2) << std::endl;
                //std::cout << "localFromGlobalIntersectionNoTolerance.y(): " << localFromGlobalIntersectionNoTolerance.y() << std::endl;


                double surfaceZCenter2 = 52.89; //this is where G4 thinks the surface center is in cm
                double drift_velocity2 = 8.0e-3;  // cm/ns

                unsigned int side2 = TpcDefs::getSide(clusterkey);
                float yintersection;
                if(side2 == 0){
                  std::cout << "yFromIntersection: " << (localFromGlobalIntersectionNoTolerance.y() + surfaceZCenter2)/drift_velocity2 << std::endl;
                  yintersection = (localFromGlobalIntersectionNoTolerance.y() + surfaceZCenter2)/drift_velocity2;
                }
                else{
                  std::cout << "yFromIntersection: " << (surfaceZCenter2 - localFromGlobalIntersectionNoTolerance.y())/drift_velocity2 << std::endl;
                  yintersection = (surfaceZCenter2 - localFromGlobalIntersectionNoTolerance.y())/drift_velocity2;
                }
                float localX;
                float localY;
                //residual is cluster - helix fit
                //cluster is residual + helix fit
                localX = m_cluster->getLocalX() + localFromGlobalIntersectionNoTolerance(0);
                localY = m_cluster->getLocalY() + yintersection;

                std::cout << "localX: " << localX << std::endl;
                std::cout << "localY: " << localY << std::endl;

                m_cluster->setLocalX(localX);
                m_cluster->setLocalY(localY);
              }
              m_cluster->identify();
              m_cluster_map->addClusterSpecifyKey(clusterkey, m_cluster);
              foundCluster = true;
                
            }
            

          }
          //delete m_cluster;
          //m_cluster_map->identify();
          }
          if(trackContainer->get_does_silicon_seed_exist()){
            track->set_silicon_seed(SiliconSeed);
          
            m_silicon_seed_container->insert(SiliconSeed);
          }else{
            track->set_silicon_seed(nullptr);
          }
          if(trackContainer->get_does_tpc_seed_exist()){
            track->set_tpc_seed(TPCSeed);
            m_tpc_seed_container->insert(TPCSeed);
          }else{
            track->set_tpc_seed(nullptr);
          }
          m_tpc_seed_container->identify();
          //m_track_map->insertWithKey(track,key);
          //insert without key
          m_track_map->insert(track);
          //delete TPCSeed;
          //delete SiliconSeed;
          }
          //delete TPCSeed;
          //delete SiliconSeed;
          m_cluster_map->identify();


}