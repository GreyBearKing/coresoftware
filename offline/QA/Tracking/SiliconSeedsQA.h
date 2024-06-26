// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef SILICONSEEDSQA_H
#define SILICONSEEDSQA_H

#include <fun4all/SubsysReco.h>
#include <trackbase/ActsGeometry.h>
#include <trackbase/TrkrDefs.h>

#include <string>
#include <vector>

class SvtxTrack;
class PHCompositeNode;

class SiliconSeedsQA : public SubsysReco
{
 public:
  SiliconSeedsQA(const std::string &name = "SiliconSeedsQA");

  ~SiliconSeedsQA() override = default;

  int InitRun(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int EndRun(const int runnumber) override;
  int End(PHCompositeNode *topNode) override;
  void setTrackMapName(const std::string &name) { m_trackMapName = name; }
  void setVertexMapName(const std::string &name) { m_vertexMapName = name; }
  
 private:
  std::vector<TrkrDefs::cluskey> get_cluster_keys(SvtxTrack *track);
  void createHistos();
  std::string getHistoPrefix() const;

  std::string m_clusterContainerName = "TRKR_CLUSTER";
  std::string m_actsgeometryName = "ActsGeometry";
  std::string m_trackMapName = "SvtxTrackMap";
  std::string m_vertexMapName = "SvtxVertexMap";
};

#endif  // SILICONSEEDSQA_H
