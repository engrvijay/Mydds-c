/*********************************************************************************
 * NAME: iprclink__interface.h - DDS interfaces.
 * ----  --------------------------------------------------
 *
 *  PURPOSE:
 *  -------
 *
 *  This file defines the functions used for DDS communication operations 
 *
 *  REVISIONS:
 *  ---------
 *
 *  Revision       Date                 Author
 *  ------------------------------------------------------------------
 *									Vijay Gayee
 *
 *
 *
 *  SYNOPSYS:
 *   ---------
 *            
 ****************************************************************************/


#ifndef IPRC_LINK__TYPES_H
#define IPRC_LINK__TYPES_H

#include "IPRC_COMM_LINK.h"
#include "IPRC_COMM_LINKSupport.h"

// as per RTI dds documentation the domain id value should not excceed 233
// i.e. ID should be 1-232 
// in our application default domain id 0 is not allowed.

#define DOMAIN_ID_MAX  233
#define DOMAIN_ID_MIN  1


    /* reader */
typedef    DDS_DomainParticipant DomainParticipant;
typedef    DDS_Subscriber Subscriber;
typedef    DDS_Topic Topic;
typedef    struct DDS_DataReaderListener ReaderListener;
typedef    DDS_DataReader DataReader;
typedef    struct DDS_TypeCode TypeCode;
typedef    IPRC_COMM_LINKDataReader LINKDataReader;

    /* writer */
typedef    DDS_Publisher Publisher;
typedef    DDS_DataWriter DataWriter;
typedef    IPRC_COMM_LINKDataWriter LINKDataWriter ;
typedef    DDS_InstanceHandle_t InstanceHandle_t;
typedef    struct  DDS_DataWriterListener WriterListener;
typedef    struct DDS_DataWriterQos DataWriterQos;
typedef    DDS_DataReaderListener_DataAvailableCallback Listener_callBack;
typedef    DDS_ReturnCode_t ReturnCode_t;


/* others */

/* This defines long- seconds and unsigned long nanoseconds */
typedef struct DDS_Duration_t Duration_t;

#endif /* IPRCLINK_TYPES_H */
