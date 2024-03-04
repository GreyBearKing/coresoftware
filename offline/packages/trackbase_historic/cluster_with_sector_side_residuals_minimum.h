#ifndef CLUSTER_WITH_SECTOR_SIDE_RESIDUALS_MINIMUM_H
#define CLUSTER_WITH_SECTOR_SIDE_RESIDUALS_MINIMUM_H

#include "SvtxTrack.h"
#include "SvtxTrackArray.h"
#include "TrackSeed.h"

#include <cstddef>        // for size_t
 

struct cluster_with_sector_side_residuals_minimum{
    //below is in trkclusterv5.h
    //float m_local[2];          //< 2D local position [cm] 2 * 32 64bit  - cumul 1*64  
    //UShort_t m_residual[2];
    //use only 8 bits for residual compression
    uint8_t m_residual[2];
    //TrkrDefs::subsurfkey m_subsurfkey; //< unique identifier for hitsetkey-surface maps 16 bit
    //float m_phierr;
    //float m_zerr;
    //unsigned short int m_adc;           //< cluster sum adc 16
    //unsigned short int m_maxadc;           //< cluster sum adc 16
    uint8_t m_adc_information;
    uint8_t m_flags;
    //uint8_t m_phisize;
    //uint8_t m_zsize;
    //uint8_t m_overlap;
    //uint8_t m_edge;

    //remove subsurfkey and errors
    //put m_adc and m_maxadc both into 8 bit that contains 4 bits 
    //triple i compression of each of them
    //put phisize, zsize, overlap, and edge all into 1 bit flags
    //get rid of valid as well as side and sector



    //char m_phisize; // 8bit
    //char m_zsize;   // 8bit
    //char m_overlap; // 8bit 
    //char m_edge;    // 8bit - cumul 2*64

    //TrkrDefs::hitsetkey m_hitsetkey;
    //setting default to zero to check if it exists
    //TrkrDefs::cluskey m_cluskey = 0;
    //uint8_t m_side;
    //uint8_t m_sector_id;
    //making clusterkey for testing
    //TrkrDefs::cluskey m_cluskey = 0;

    //make bool to see if cluster is filled or not
    //bool m_valid = false;
    };

    #endif