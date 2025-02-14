#include "MvtxEventInfov1.h"

#include <phool/phool.h>

PHObject* MvtxEventInfov1::CloneMe() const
{
  std::cout << PHWHERE << "::" << __func__ << " is not implemented in daughter class" << std::endl;
  return nullptr;
}

void MvtxEventInfov1::Reset()
{
  m_strobe_BCO_L1_BCO.clear();
  m_StringEventProperties.clear();
  m_IntEventProperties.clear();
  m_Int64EventProperties.clear();
  m_UintEventProperties.clear();
  m_Uint64EventProperties.clear();
  m_FloatEventProperties.clear();

  return;
}


void MvtxEventInfov1::identify(std::ostream &out) const
{
  out << "MvtxEventInfov1 information" << std::endl;

  for (auto iters = m_StringEventProperties.begin(); iters != m_StringEventProperties.end(); ++iters)
  {
    out << iters->first << ": " << iters->second << std::endl;
  }
    
  out << "Number of L1 triggers in this event" << std::endl;
  out << m_number_L1_name << ": " << get_number_L1s() << std::endl;
    
  out << "Number of heart beats in this event" << std::endl;
  out << m_number_HB_name << ": " << get_number_HB() << std::endl;

  if (get_number_L1s() > 0)
  {
    out << "List of strobe BCOs and L1 BCOs in this event" << std::endl;

    std::set<uint64_t> strobeList = get_strobe_BCOs();
    for (auto iterStrobe = strobeList.begin(); iterStrobe != strobeList.end(); ++iterStrobe)
    { 
      std::set<uint64_t> l1List = get_L1_BCO_from_strobe_BCO(*iterStrobe);
      out << "Strobe BCO: " << *iterStrobe << std::endl; 
      for (auto iterL1 = l1List.begin(); iterL1 != l1List.end(); ++iterL1)
      {
        out << "L1 BCO: " << *iterL1 << std::endl; 
      }
      out << std::endl;
    }
  }

  for (auto iteri = m_IntEventProperties.begin(); iteri != m_IntEventProperties.end(); ++iteri)
  {
    out << iteri->first << ": " << iteri->second << std::endl;
  }

  for (auto iteri64 = m_Int64EventProperties.begin(); iteri64 != m_Int64EventProperties.end(); ++iteri64)
  {
    out << iteri64->first << ": " << iteri64->second << std::endl;
  }

  for (auto iteru = m_UintEventProperties.begin(); iteru != m_UintEventProperties.end(); ++iteru)
  {
    out << iteru->first << ": " << iteru->second << std::endl;
  }

  for (auto iteru64 = m_Uint64EventProperties.begin(); iteru64 != m_Uint64EventProperties.end(); ++iteru64)
  {
    out << iteru64->first << ": " << iteru64->second << std::endl;
  }

  for (auto iterf = m_FloatEventProperties.begin(); iterf != m_FloatEventProperties.end(); ++iterf)
  {
    out << iterf->first << ": " << iterf->second << std::endl;
  }
  return;
}

int MvtxEventInfov1::isValid() const
{
  std::cout << PHWHERE << " isValid not implemented by daughter class" << std::endl;
  return 0;
}

void MvtxEventInfov1::set_number_HB(const int ival)
{
  m_number_HB = ival;
}

int MvtxEventInfov1::get_number_HB() const
{
  return m_number_HB;
}

void MvtxEventInfov1::set_strobe_BCO_L1_BCO(const uint64_t strobe_BCO, const uint64_t L1_BCO)
{
  strobe_L1_pair strobe_L1_BCO_pair(strobe_BCO, L1_BCO);
  m_strobe_BCO_L1_BCO.insert(strobe_L1_BCO_pair);
}

unsigned int MvtxEventInfov1::get_number_strobes() const
{
  std::set<uint64_t> mySet;
  std::set<strobe_L1_pair>::const_iterator iter;
  for (iter = m_strobe_BCO_L1_BCO.begin(); iter != m_strobe_BCO_L1_BCO.end(); ++iter)
  {
    strobe_L1_pair myPair = *iter;
    mySet.insert(myPair.first);
  }
  
  return mySet.size();
}

unsigned int MvtxEventInfov1::get_number_L1s() const
{
  std::set<uint64_t> mySet;
  std::set<strobe_L1_pair>::const_iterator iter;
  for (iter = m_strobe_BCO_L1_BCO.begin(); iter != m_strobe_BCO_L1_BCO.end(); ++iter)
  {
    strobe_L1_pair myPair = *iter;
    mySet.insert(myPair.second);
  }
  
  return mySet.size();
}

std::set<uint64_t> MvtxEventInfov1::get_strobe_BCOs() const
{
  std::set<uint64_t> mySet;
  std::set<strobe_L1_pair>::const_iterator iter;
  for (iter = m_strobe_BCO_L1_BCO.begin(); iter != m_strobe_BCO_L1_BCO.end(); ++iter)
  { 
    strobe_L1_pair myPair = *iter;
    mySet.insert(myPair.first);
  }

  return mySet;
}

std::set<uint64_t> MvtxEventInfov1::get_L1_BCOs() const
{
  std::set<uint64_t> mySet;
  std::set<strobe_L1_pair>::const_iterator iter;
  for (iter = m_strobe_BCO_L1_BCO.begin(); iter != m_strobe_BCO_L1_BCO.end(); ++iter)
  {
    strobe_L1_pair myPair = *iter;
    mySet.insert(myPair.second);
  }

  return mySet;
}

std::set<uint64_t> MvtxEventInfov1::get_strobe_BCO_from_L1_BCO(const uint64_t ival) const
{
  std::set<uint64_t> mySet;
  std::set<strobe_L1_pair>::const_iterator iter;
  for (iter = m_strobe_BCO_L1_BCO.begin(); iter != m_strobe_BCO_L1_BCO.end(); ++iter)
  { 
    strobe_L1_pair myPair = *iter;
    if (ival == myPair.second)
    {
      mySet.insert(myPair.first);
    }
  }

  return mySet;
}

std::set<uint64_t> MvtxEventInfov1::get_L1_BCO_from_strobe_BCO(const uint64_t ival) const
{
  std::set<uint64_t> mySet;
  std::set<strobe_L1_pair>::const_iterator iter;
  for (iter = m_strobe_BCO_L1_BCO.begin(); iter != m_strobe_BCO_L1_BCO.end(); ++iter)
  {
    strobe_L1_pair myPair = *iter;
    if (ival == myPair.first)
    {
      mySet.insert(myPair.second);
    }
  }

  return mySet;
}
