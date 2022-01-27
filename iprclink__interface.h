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
 *  * Initial revision                 vijay gayee
 *  *
 *
 *
 *   `
 *            
-****************************************************************************/

#ifndef IPRC_LINK__INTERFACE_H
#define IPRC_LINK__INTERFACE_H

#include "iprclink__types.h"

/* In case we have diffrent domain for each systems 
 * 
*  most likeely ther will be one domain. lets start with one 
*/

#define DDSAPI_NO_ERROR  0
#define DDSAPI_ERROR    -1 

#define MAX_DOMAINID_SZ 7
#define MAX_TOPIC_SZ    10

#define DEFAULT_DOMAIN_ID 33
#define PTCG_DOMAIN_ID    10
#define CAD_DOMAIN_ID     20
#define BOS_DOMAIN_ID     30
#define LOCO_DOMAIN_ID    40

#define NUM_PROFILE_FILES 1
#define TRUE  1
#define FALSE 0

/* publisher or subscriber type */
typedef enum {
        LINK_UNKNOWN,
        LINK_READER,
        LINK_WRITER,
} iprc_link_type; 


/* types of IPRC_COMM_LINK Messages */
// MIP-MSG

typedef enum {
         LINK_MSG_K_UNKNOWN,
         LINK_MSG_K_DATA,
         LINK_MSG_K_SATUS,
         LINK_MSG_K_SATUS_ACK,
         LINK_MSG_K_RESET,
         CHMOD_K,
         RESTART_K,
         STOP_K,
         EVENT_K,
         ALARM_K,
         RTN_K,
         INTERNAL_K,
}COMM_LINK_DataTypes;


/* if there is more than one doamin for the process
 * we can populate following array. 
 * for more than one topic.
 * if need to send data to more topics, process need those topic writers
 * 
 */

/* To keep application links */
static int NumberofDoamins = 0;
static int NumberofTopic   = 0;

typedef struct iprc_comm_topic {
    Topic             *topic;
    char        *topic_name;
    TypeCode          *type_code;
//    Publisher         *publisher;
    DataWriter        *writer;
    LINKDataWriter    *iprc_link_writer;
//    Subscriber        *subscriber;
    DataReader        *reader;
    LINKDataReader    *iprc_link_reader;
} iprc_comm_topic; 


typedef struct iprc_comm_domain {
// default domainid = 0 is invalid in application.
    int                domainId;
    DomainParticipant *participant;
    const char        *type_name;
// The Subscriber manages the activities of several DataReader entities.
    Subscriber        *subscriber;
// The Publisher manages the activities of several DataWriters.
    Publisher         *publisher;
//  topics to which, need to send data.
    iprc_comm_topic    topic_list[MAX_TOPIC_SZ]; 
} iprc_comm_domain;


// domain list
iprc_comm_domain domain_list[MAX_DOMAINID_SZ];

/*
    DDS_Short   h_Domain ;
    DDS_Char   h_srcsys [32];
    DDS_Char   h_destsys [32];
    DDS_Long   h_Time_Date ;
    DDS_Long   h_RefNumber ;
    DDS_Short   h_Type ;
    DDS_Short   h_SubType ;
    DDS_Char   h_srcTopic [32];
    DDS_Char   h_destTopic [32];
    DDS_Long   h_DataLen ;
    DDS_Char   Data [4096];
IPRC_COMM_LINK ;
*/

/* The MSG has header fields and Data. in Data one can put the complete application message*/
/* IPRC_COMM_LINK is model name to create user topic from */
 

typedef struct IPRC_COMM_LINK LinkData;

/* api list */

void iprclink_api_set_my_domain(short id);
short iprclink_api_get_my_domain();
void iprclink_api_set_my_topic(const char* topic);
char* iprclink_api_get_my_topic();

void iprclink_api_set_cad_topic(const char* topic);
char* iprclink_api_get_cad_topic();
void iprclink_api_set_bos_topic(const char* topic);
char* iprclink_api_get_bos_topic();

int iprclink_api_create_subscriber( int domainId, const char* topic_name, Listener_callBack func);
int iprclink_api_create_publisher( int domainId, const char* topic_name, Listener_callBack func);
int iprclink_api_create_link( int domainId, const char* topic_name, Listener_callBack func, int flag );

ReturnCode_t iprclink_api_delete_data(IPRC_COMM_LINK *data); 
ReturnCode_t iprclink_api_send(LinkData *data); 
/* creates data type of IPRC_COMM_LINK */ 
IPRC_COMM_LINK* iprclink_writer_api_create_data(); 
int set_custom_QoS();
int IsEmptyString(const char *string);

/****************************************************/
#endif /* IPRC_LINK__INTERFACE_H */
