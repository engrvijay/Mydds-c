/*********************************************************************************
 * NAME: iprclink_api .c - DDS  interfaces.
 * ----  --------------------------------------------------
 *
 *  PURPOSE:
 *  -------
 *
 *    This file defines the functions used for DDS communication operations 
 *
 *    REVISIONS:
 *    ---------
 *
 *   Revision       Date                 Author
 *   ------------------------------------------------------------------
 *  * 		                      vijay gayee
 *  *
 *
 *  SYNOPSYS:
 *  ---------
-****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "ndds/ndds_c.h"
#include "iprclink__interface.h"
#include "utils_printlog.h"

static char *myTopic_Name = NULL;
static char *cadTopic_Name = NULL;
static char *bosTopic_Name = NULL;
static int  myDomainID = 0;

const char* libraryName = "DDS_Message_Library";
const char* profileName = "DDS_Message_Profile";

//We are only going to add one XML file to the url_profile
//sequence, so we set a maximum length of 1
/*static const char *myUrlProfile[NUM_PROFILE_FILES] =
  {"file://./../user/config/USER_QOS_PROFILES.xml"};*/

char *myUrlProfile[NUM_PROFILE_FILES] = {};

int IsEmptyString(const char *string)
{
  return (strlen(string) == 0) ? TRUE : FALSE;
}

void iprclink_api_set_my_topic(const char* topic)
{
    char *name = malloc (strlen(topic) + 1);

    if( name != NULL)
    {
        strcpy(name, topic);
        myTopic_Name = name;
    }
    
}

void iprclink_api_set_cad_topic(const char* topic)
{
    char *name = malloc (strlen(topic) + 1);

    if( name != NULL)
    {
        strcpy(name, topic);
        cadTopic_Name = name;
    }
    
}

void iprclink_api_set_bos_topic(const char* topic)
{
    char *name = malloc (strlen(topic) + 1);

    if( name != NULL)
    {
        strcpy(name, topic);
        bosTopic_Name = name;
    }
    
}

char* iprclink_api_get_my_topic()
{
    return myTopic_Name;
}

char* iprclink_api_get_cad_topic()
{
    return cadTopic_Name;
}

char* iprclink_api_get_bos_topic()
{
    return bosTopic_Name;
}

void iprclink_api_set_my_domain(short id)
{
    if (  id >=  DOMAIN_ID_MIN && id <  DOMAIN_ID_MAX) 
        myDomainID = id;
    else sys_printlog(SYS_LOG_DEBUG, "Error- %d Invalid domain Id",id);
    
}

short iprclink_api_get_my_domain()
{
    return myDomainID;
}

/**************************************************************************
 */

int find_domain_topic(int domain_idx, const char *topic_name)
{
    int cmp;
    int i;
    int found = 0;
    for ( i = 0; i < MAX_TOPIC_SZ; i++)
    {
       if(domain_list[domain_idx].topic_list[i].topic_name== NULL) continue;       
      
       cmp = strcmp(domain_list[domain_idx].topic_list[i].topic_name, topic_name);

       if( cmp == 0)
       {   /*****
			fprintf(stderr, "%s %d: [%d] topic_name:\"%s\",\"%s\"\n",
						__FILE__, __LINE__, i,
						domain_list[domain_idx].topic_list[i].topic_name,
						topic_name);
			 *****/
           found = 1; break;  
       }          
    }
    return (found ? i: -1);
}


int find_domain_participant(int domain_id)
{
   int i;
   int found = 0;

   for (i = 0 ; i < MAX_DOMAINID_SZ; i++)
   {
       if( domain_list[i].domainId == domain_id)
       {
          found = 1; break;
       }
    }

    if( (found) && domain_list[i].participant != NULL)
        return i;
    
    return -1;
}

int is_domain_participant_created( int doamin_id)
{
   int i;
   int found = 0;
  
   for (i = 0 ; i < MAX_DOMAINID_SZ; i++)
   {
       if (domain_list[i].domainId == doamin_id)
       {
          found = 1; break;   
       }  
   }
   
   // find the place for next use.
   if (!found) 
   {
     for (i = 0; i < MAX_DOMAINID_SZ; i++)
          if (domain_list[i].domainId == 0)
          {
             sys_printlog(SYS_LOG_DEBUG, "found place for participant at index=%d", i); 
              return i;
          }
   }
 
  if ( i == MAX_DOMAINID_SZ) 
     sys_printlog(SYS_LOG_WARNING, "domain list reached max limit:%d", MAX_DOMAINID_SZ);

  return (found ? i: -1);
}


int is_domain_topic_created(int domain_idx, const char *topic_name)
{
    int cmp, i;
    int found = 0;
    for ( i = 0; i < MAX_TOPIC_SZ; i++)
    {
       //printf("index = %d, topic='%s', input='%s'",i, domain_list[domain_idx].topic_list[i].topic_name, topic_name);
       if(domain_list[domain_idx].topic_list[i].topic_name== NULL) continue;       
      
       cmp = strcmp(domain_list[domain_idx].topic_list[i].topic_name, topic_name);

       if( strcmp(domain_list[domain_idx].topic_list[i].topic_name, topic_name) == 0)
       {
            /*** fprintf(stderr, "%s %d: [%d] topic_name:\"%s\",\"%s\"",
					__FILE__, __LINE__,
					i, domain_list[domain_idx].topic_list[i].topic_name, topic_name);
			 ***/
           found = 1; break;  
       }          
    }

    //find the place for topic. 
    if (!found)
    {
         for (i = 0; i < MAX_TOPIC_SZ; i++)
          if(domain_list[domain_idx].topic_list[i].topic == NULL)
          {
           sys_printlog(SYS_LOG_DEBUG, "found place for topic at index=%d", i);
            return i;
          }
    }

    if ( domain_list[domain_idx].topic_list[i].topic == NULL)
       return -1;
     
    return (found ? i: -1);
}


/* ***********public api************** */

int iprclink_api_create_subscriber( int domainId, const char* topic_name, Listener_callBack func)
{
     return iprclink_api_create_link( domainId, topic_name, func, LINK_READER);
}

int iprclink_api_create_publisher( int domainId, const char* topic_name, Listener_callBack func)
{
     return  iprclink_api_create_link( domainId, topic_name, func, LINK_WRITER);
}


/* delete this data type of IPRC_COMM_LINK */

ReturnCode_t iprclink_api_delete_data(IPRC_COMM_LINK *data)
{
   return IPRC_COMM_LINKTypeSupport_delete_data_ex(data, DDS_BOOLEAN_TRUE);
}


ReturnCode_t iprclink_api_send(LinkData *send_data)
{
   DDS_InstanceHandle_t instance_handle = DDS_HANDLE_NIL;
   LINKDataWriter *data_writer = NULL;
   IPRC_COMM_LINK  *data = send_data;
   char *topic_name = send_data->h_destTopic;
   int domain_id = 0;
   int writer_idx, domain_idx;
   /* find the data writer */

   domain_id = send_data->h_Domain;
   if ( domain_id < DOMAIN_ID_MIN || domain_id > DOMAIN_ID_MAX  )
   {
        sys_printlog(SYS_LOG_INFO,"%s- invalid DomainId:%d",__func__, domain_id);
        return DDSAPI_ERROR; 
   }
    
   domain_idx = find_domain_participant(domain_id);
   if  (domain_idx < 0 )
   {
       sys_printlog(SYS_LOG_INFO,"%s-no initialization found for domain:%d",__func__, domain_id);
       return DDSAPI_ERROR;
   } 

    writer_idx = find_domain_topic(domain_idx,topic_name); 
   if ( writer_idx  < 0 )
   {
       sys_printlog(SYS_LOG_INFO,"%s-no data_writer found for link:%s",__func__, topic_name);
       return DDSAPI_ERROR;
   } 

   sys_printlog(SYS_LOG_INFO, "domain_idx:%d,writer_idx:%d for topic:%s", domain_idx,writer_idx,topic_name);
   data_writer = domain_list[domain_idx].topic_list[writer_idx].iprc_link_writer;
   if( data_writer == NULL) 
   {
      sys_printlog(SYS_LOG_INFO,"%s-no data_writer failed for link:%s",__func__, topic_name);
      return DDSAPI_ERROR;
   }

   return IPRC_COMM_LINKDataWriter_write(
		   data_writer, data, &instance_handle);
}


/* creates data type of IPRC_COMM_LINK */
IPRC_COMM_LINK* iprclink_writer_api_create_data()
{
    return IPRC_COMM_LINKTypeSupport_create_data_ex(DDS_BOOLEAN_TRUE);
}


/* Delete all entities */
////static int publisher_shutdown(
int writer_shutdown(
    DDS_DomainParticipant *participant)
{
    DDS_ReturnCode_t retcode;
    int status = 0;

    if (participant != NULL) {
        retcode = DDS_DomainParticipant_delete_contained_entities(participant);
        if (retcode != DDS_RETCODE_OK) {
            sys_printlog(SYS_LOG_INFO, "delete_contained_entities error %d", retcode);
            status = -1;
        }

        retcode = DDS_DomainParticipantFactory_delete_participant(
            DDS_TheParticipantFactory, participant);
        if (retcode != DDS_RETCODE_OK) {
            sys_printlog(SYS_LOG_INFO, "delete_participant error %d", retcode);
            status = -1;
        }
    }

    return status;
}

/* Delete all entities */
static int subscriber_shutdown(
    DDS_DomainParticipant *participant)
{
    DDS_ReturnCode_t retcode;
    int status = 0;

    if (participant != NULL) {
        retcode = DDS_DomainParticipant_delete_contained_entities(participant);
        if (retcode != DDS_RETCODE_OK) {
            sys_printlog(SYS_LOG_INFO, "delete_contained_entities error %d", retcode);
            status = -1;
        }

        retcode = DDS_DomainParticipantFactory_delete_participant(
            DDS_TheParticipantFactory, participant);
        if (retcode != DDS_RETCODE_OK) {
            sys_printlog(SYS_LOG_INFO, "delete_participant error %d", retcode);
            status = -1;
        }
    }

 /* RTI Data Distribution Service provides the finalize_instance() method on
 *     domain participant factory for users who want to release memory used
 *         by the participant factory. Uncomment the following block of code for
 *             clean destruction of the singleton. */
 /*
 *     retcode = DDS_DomainParticipantFactory_finalize_instance();
 *         if (retcode != DDS_RETCODE_OK) {
 *                 sys_printlog(SYS_LOG_INFO, "finalize_instance error %d", retcode);
 *                         status = -1;
 *                             }
 * */

    return status;
}

void IPRC_COMM_LINKListener_on_requested_deadline_missed(
    void* listener_data,
    DDS_DataReader* reader,
    const struct DDS_RequestedDeadlineMissedStatus *status)
{
}

void IPRC_COMM_LINKListener_on_requested_incompatible_qos(
    void* listener_data,
    DDS_DataReader* reader,
    const struct DDS_RequestedIncompatibleQosStatus *status)
{
}

void IPRC_COMM_LINKListener_on_sample_rejected(
    void* listener_data,
    DDS_DataReader* reader,
    const struct DDS_SampleRejectedStatus *status)
{
}

void IPRC_COMM_LINKListener_on_liveliness_changed(
    void* listener_data,
    DDS_DataReader* reader,
    const struct DDS_LivelinessChangedStatus *status)
{
}

void IPRC_COMM_LINKListener_on_sample_lost(
    void* listener_data,
    DDS_DataReader* reader,
    const struct DDS_SampleLostStatus *status)
{
}

void IPRC_COMM_LINKListener_on_subscription_matched(
    void* listener_data,
    DDS_DataReader* reader,
    const struct DDS_SubscriptionMatchedStatus *status)
{
}

/* TEST listener CAllBack function */

void IPRC_COMM_LINKListener_on_data_available(
    void* listener_data,
    DDS_DataReader* reader)
{
    LINKDataReader *data_reader = NULL;
    struct IPRC_COMM_LINKSeq data_seq = DDS_SEQUENCE_INITIALIZER;
    struct DDS_SampleInfoSeq info_seq = DDS_SEQUENCE_INITIALIZER;
    DDS_ReturnCode_t retcode;
    int i, domain_idx, reader_idx;
    char *topic_name = "IPRC_COMM_LINK";


  sys_printlog(SYS_LOG_DEBUG, "%s-Received data", __func__);
   domain_idx = is_domain_participant_created(myDomainID); 
   if ( domain_idx  < 0 )
   {
       sys_printlog(SYS_LOG_INFO,"%s-no initialization fornd for domain:%d",__func__, myDomainID);
       return;
   }

   reader_idx = is_domain_topic_created(domain_idx,topic_name);
   if ( reader_idx < 0 )
   {
       sys_printlog(SYS_LOG_INFO,"%s-no data_writer found for link:%s",__func__, topic_name);
       return;
   }

   sys_printlog(SYS_LOG_INFO, "%s-domain_idx:%d,writer_idx:%d for topic:%s",__func__,domain_idx,reader_idx,topic_name);
   data_reader = domain_list[domain_idx].topic_list[reader_idx].iprc_link_reader;
   if( data_reader == NULL)
   {
      sys_printlog(SYS_LOG_INFO,"%s-no data_reader failed for link:%s",__func__, topic_name);
      return;
   }


    retcode = IPRC_COMM_LINKDataReader_take(
        data_reader,
        &data_seq, &info_seq, DDS_LENGTH_UNLIMITED,
        DDS_ANY_SAMPLE_STATE, DDS_ANY_VIEW_STATE, DDS_ANY_INSTANCE_STATE);
    if (retcode == DDS_RETCODE_NO_DATA) {
        return;
    } else if (retcode != DDS_RETCODE_OK) {
        sys_printlog(SYS_LOG_INFO, "take error %d", retcode);
        return;
    }

    for (i = 0; i < IPRC_COMM_LINKSeq_get_length(&data_seq); ++i) {
        if (DDS_SampleInfoSeq_get_reference(&info_seq, i)->valid_data) {
           sys_printlog(SYS_LOG_DEBUG, "Received data");
            IPRC_COMM_LINKTypeSupport_print_data(
                IPRC_COMM_LINKSeq_get_reference(&data_seq, i));
        }
    }

    retcode = IPRC_COMM_LINKDataReader_return_loan(
        data_reader,
        &data_seq, &info_seq);
    if (retcode != DDS_RETCODE_OK) {
        sys_printlog(SYS_LOG_INFO, "return loan error %d", retcode);
    }
}

// DDS_PublisherQos struct
// DDS_SubscriberQos Struct 
// DDS_DataWriterQos Struct
// DDS_DataReaderQos Struct

int iprclink_api_create_link( int domainId, const char* topic_name, Listener_callBack func, int flag )
{
   
    int          idx = -1;
    int    topic_idx = -1;
    int domain_found = 0;
    int topic_found  = 0;
    DDS_InstanceHandle_t instance_handle = DDS_HANDLE_NIL;
    int count = 0;  
    int sample_count = 0;  
    Duration_t send_period = {4,0};
    ReaderListener     reader_listener = DDS_DataReaderListener_INITIALIZER; 
    ReturnCode_t   retcode; 

    if ( domainId < DOMAIN_ID_MIN || domainId > DOMAIN_ID_MAX )
    {
       sys_printlog(SYS_LOG_INFO, "(1-to-%d)invalid domain id = %d", DOMAIN_ID_MAX, domainId);
       return -1;
    }
    
    /* find participant */
    idx = is_domain_participant_created(domainId); 
    if( idx < 0 )
    {
        sys_printlog(SYS_LOG_INFO, "Domain list reached max size:%d", idx);
        return -1; 
    }

    if ( domain_list[idx].participant != NULL) 
    {
       domain_found = 1;
       sys_printlog(SYS_LOG_INFO, "This domian:%d, participant already created", domainId);  
    }
   
    set_custom_QoS();
    // participant/domain init if not done.
    if ( domain_list[idx].participant == NULL)
    {
        /* To customize participant QoS, use 
        the configuration file USER_QOS_PROFILES.xml */
        if(myUrlProfile[0] != NULL && !(IsEmptyString(myUrlProfile[0]))) {
          domain_list[idx].participant = DDS_DomainParticipantFactory_create_participant_with_profile(
                     DDS_TheParticipantFactory, domainId, libraryName,profileName,
                     NULL /* listener */, DDS_STATUS_MASK_NONE);
        }else{
          domain_list[idx].participant = DDS_DomainParticipantFactory_create_participant(
                     DDS_TheParticipantFactory, domainId, &DDS_PARTICIPANT_QOS_DEFAULT,
                     NULL /* listener */, DDS_STATUS_MASK_NONE);
        }
        if (domain_list[idx].participant == NULL) {
            sys_printlog(SYS_LOG_INFO, "create_participant error");
            writer_shutdown(domain_list[idx].participant);
            return -1;
        }
    }

   // set domain ID.
   domain_list[idx].domainId = domainId;
   sys_printlog(SYS_LOG_DEBUG, "create_participant successfully");

    /* To customize publisher QoS, use 
    the configuration file USER_QOS_PROFILES.xml */
    if (domain_list[idx].publisher == NULL && flag == LINK_WRITER)
    {
        if(myUrlProfile[0] != NULL && !(IsEmptyString(myUrlProfile[0]))) {
          domain_list[idx].publisher = DDS_DomainParticipant_create_publisher_with_profile(
            domain_list[idx].participant, libraryName,profileName, NULL /* listener */,
            DDS_STATUS_MASK_NONE);
        }else{
          domain_list[idx].publisher = DDS_DomainParticipant_create_publisher(
            domain_list[idx].participant, &DDS_PUBLISHER_QOS_DEFAULT, NULL /* listener */,
            DDS_STATUS_MASK_NONE);
        }
        if (domain_list[idx].publisher == NULL) {
            sys_printlog(SYS_LOG_INFO, "create_publisher error");
            writer_shutdown(domain_list[idx].participant);
            return -1;
        }
    sys_printlog(SYS_LOG_DEBUG, "create_publisher successfully");
    }
  
    /* To customize sbscriber  QoS, use 
    the configuration file USER_QOS_PROFILES.xml */
    if (domain_list[idx].subscriber == NULL && flag == LINK_READER)
    {
        if(myUrlProfile[0] != NULL && !(IsEmptyString(myUrlProfile[0]))) {
          domain_list[idx].subscriber = DDS_DomainParticipant_create_subscriber_with_profile(
             domain_list[idx].participant,libraryName,profileName, NULL /* listener */,
             DDS_STATUS_MASK_NONE);
        }else{
          domain_list[idx].subscriber = DDS_DomainParticipant_create_subscriber(
             domain_list[idx].participant, &DDS_SUBSCRIBER_QOS_DEFAULT, NULL /* listener */,
             DDS_STATUS_MASK_NONE);
        } 
        if (domain_list[idx].subscriber == NULL) {
           sys_printlog	(SYS_LOG_INFO, "create_subscriber error");
           writer_shutdown(domain_list[idx].participant);
           return -1;
       }
    	sys_printlog(SYS_LOG_DEBUG,"create_subscriber successfully");
    }
    

    /* Register type before creating topic */
    domain_list[idx].type_name = IPRC_COMM_LINKTypeSupport_get_type_name();
    retcode = IPRC_COMM_LINKTypeSupport_register_type(
        domain_list[idx].participant, domain_list[idx].type_name);
    if (retcode != DDS_RETCODE_OK) {
        sys_printlog(SYS_LOG_INFO, "register_type error %d", retcode);
        writer_shutdown(domain_list[idx].participant);
        return -1;
    }
    sys_printlog(SYS_LOG_DEBUG, "register_type successfully");

    /* To customize topic QoS, use 
    the configuration file USER_QOS_PROFILES.xml */
    topic_idx = is_domain_topic_created(idx, topic_name);

    if ( domain_list[idx].topic_list[topic_idx].topic != NULL )
    {
         topic_found = 1;
    }    
    
    if ( !topic_found)
    {
        if(myUrlProfile[0] != NULL && !(IsEmptyString(myUrlProfile[0]))) {
          domain_list[idx].topic_list[topic_idx].topic = DDS_DomainParticipant_create_topic_with_profile(
            domain_list[idx].participant, topic_name,
            domain_list[idx].type_name,libraryName,profileName, NULL /* listener */,
            DDS_STATUS_MASK_NONE);
        }else{
          domain_list[idx].topic_list[topic_idx].topic = DDS_DomainParticipant_create_topic(
            domain_list[idx].participant, topic_name,
            domain_list[idx].type_name, &DDS_TOPIC_QOS_DEFAULT, NULL /* listener */,
            DDS_STATUS_MASK_NONE);
        }
        if (domain_list[idx].topic_list[topic_idx].topic == NULL) {
            sys_printlog(SYS_LOG_INFO, "create_topic error");
            writer_shutdown(domain_list[idx].participant);
            return -1;
        }
    // set topic name 
    domain_list[idx].topic_list[topic_idx].topic_name = strdup(topic_name);
   sys_printlog(SYS_LOG_DEBUG, "create_topic successfully-%s",topic_name);
    }


    if(domain_list[idx].topic_list[topic_idx].writer == NULL & flag == LINK_WRITER )
    {
        /* To customize data writer QoS, use 
        the configuration file USER_QOS_PROFILES.xml */
        if(myUrlProfile[0] != NULL && !(IsEmptyString(myUrlProfile[0]))) {
          domain_list[idx].topic_list[topic_idx].writer = 
                                            DDS_Publisher_create_datawriter_with_profile(
	     domain_list[idx].publisher, domain_list[idx].topic_list[topic_idx].topic,
	     libraryName,profileName,NULL /* listener */, DDS_STATUS_MASK_NONE);
        }else{
          domain_list[idx].topic_list[topic_idx].writer = DDS_Publisher_create_datawriter(
	      domain_list[idx].publisher, domain_list[idx].topic_list[topic_idx].topic,
	     &DDS_DATAWRITER_QOS_DEFAULT, NULL /* listener */, DDS_STATUS_MASK_NONE);
        }
        if (domain_list[idx].topic_list[topic_idx].writer == NULL) {
	    sys_printlog(SYS_LOG_INFO, "create_datawriter error");
	    writer_shutdown(domain_list[idx].participant);
	    return -1;
        }

        domain_list[idx].topic_list[topic_idx].iprc_link_writer = 
                 IPRC_COMM_LINKDataWriter_narrow(domain_list[idx].topic_list[topic_idx].writer);
        if (domain_list[idx].topic_list[topic_idx].iprc_link_writer == NULL) {
	    sys_printlog(SYS_LOG_INFO, "iprc_link_writer narrow error");
	    writer_shutdown(domain_list[idx].participant);
	    return -1;
        }
   sys_printlog(SYS_LOG_DEBUG, "create_datawriter successfully(domain_idx=%d, topic_idx=%d)-%s",
                                               idx, topic_idx, domain_list[idx].topic_list[topic_idx].topic_name ); 
    }
    
    if(domain_list[idx].topic_list[topic_idx].reader == NULL & flag == LINK_READER )
    {
        reader_listener.on_requested_deadline_missed  = 
        IPRC_COMM_LINKListener_on_requested_deadline_missed;
        reader_listener.on_requested_incompatible_qos = 
        IPRC_COMM_LINKListener_on_requested_incompatible_qos;
        reader_listener.on_sample_rejected =
        IPRC_COMM_LINKListener_on_sample_rejected;
        reader_listener.on_liveliness_changed = 
        IPRC_COMM_LINKListener_on_liveliness_changed;
        reader_listener.on_sample_lost = 
        IPRC_COMM_LINKListener_on_sample_lost;
        reader_listener.on_subscription_matched = 
        IPRC_COMM_LINKListener_on_subscription_matched;
        reader_listener.on_data_available = func;
        //IPRC_COMM_LINKListener_on_data_available;

        if(myUrlProfile[0] != NULL && !(IsEmptyString(myUrlProfile[0]))) {
          domain_list[idx].topic_list[topic_idx].iprc_link_reader = DDS_Subscriber_create_datareader_with_profile(
            domain_list[idx].subscriber, DDS_Topic_as_topicdescription(domain_list[idx].topic_list[topic_idx].topic),
            libraryName,
            profileName, 
            &reader_listener, 
            DDS_STATUS_MASK_ALL);
        }else{
          domain_list[idx].topic_list[topic_idx].iprc_link_reader = DDS_Subscriber_create_datareader(
            domain_list[idx].subscriber, DDS_Topic_as_topicdescription(domain_list[idx].topic_list[topic_idx].topic),
            &DDS_DATAREADER_QOS_DEFAULT, 
            &reader_listener, 
            DDS_STATUS_MASK_ALL);
        }
        if (domain_list[idx].topic_list[topic_idx].iprc_link_reader == NULL) {
           sys_printlog(SYS_LOG_INFO, "create_datareader error");
           subscriber_shutdown(domain_list[idx].participant);
           return -1;
        }
   sys_printlog(SYS_LOG_DEBUG, "create_datareader successfully-%s",domain_list[idx].topic_list[topic_idx].topic_name); 
    }

   return idx;
}


int set_custom_QoS()
{

  int SetCustomQoS = 0;

   char* envVal =  getenv("TNG_USER");
   char file_name[256] = {0};
   char file_url[256] = "file://";
   if (envVal != NULL)
   {  
     sys_printlog(SYS_LOG_INFO, "set_custom_QoS():the env value is %s\n",envVal);
   }
   sprintf(file_name, "%s%s", envVal,"/config/USER_QOS_PROFILES.xml");
  
   strcat(file_url,file_name);
   myUrlProfile[0] = file_url;
   sys_printlog(SYS_LOG_INFO, "the myUrlProfile[0] is %s\n",myUrlProfile[0]);
  // If a user Qos profile URL was specified
  if(myUrlProfile[0] != NULL && !(IsEmptyString(myUrlProfile[0]))) {
    struct DDS_DomainParticipantFactoryQos factory_qos =
        DDS_DomainParticipantFactoryQos_INITIALIZER;
    DDS_ReturnCode_t retcode;
    /* There are several different approaches for loading QoS profiles from XML
     * files (see Configuring QoS with XML chapter in the RTI Connext Core
     * Libraries and Utilities User's Manual).
     *
     * One approach is adding XML documents to the DomainParticipantFactory using its
     * Profile QoSPolicy (DDS Extension). In this case, we add
     * USER_QOS_PROFILES.xml to the url_profile sequence, which stores
     * the URLs of all the XML documents with QoS policies that are loaded by
     * the DomainParticipantFactory aside from the ones that are automatically
     * loaded.
     * USER_QOS_PROFILES.xml defines a QoS profile named
     * transient_local_profile that configures reliable, transient local
     * DataWriters and DataReaders.
     */

    /* To load USER_QOS_PROFILES.xml, as explained above, we need to modify
     * the  DDSTheParticipantFactory Profile QoSPolicy */
    DDS_DomainParticipantFactory_get_qos(DDS_TheParticipantFactory, 
        &factory_qos);

    /* We are only going to add one XML file to the url_profile sequence, so our
     * NUM_PROFILE_FILES is 1 (defined at start) */
    DDS_StringSeq_from_array(&(factory_qos.profile.url_profile),
        (const char **) myUrlProfile, NUM_PROFILE_FILES);

    retcode = DDS_DomainParticipantFactory_set_qos(DDS_TheParticipantFactory,
        &factory_qos);
    if (retcode != DDS_RETCODE_OK) {
        sys_printlog(SYS_LOG_INFO, "set_qos error %d\n", retcode);
        //publisher_shutdown(participant, &factory_qos);
        return -1;
    }
    SetCustomQoS = 1;
  }
  return SetCustomQoS;
}
/* include if want to build test executable */
#ifdef BUILD_TEST
int main(int argc, char *argv[])
{
    int domainId     = DEFAULT_DOMAIN_ID;
    int sample_count = 100; /*if 0 then infinite loop */
    char *topic_name_r = "IPRC_CLSD_SUPR";
    char *topic_name_s = "IPRC_COMM_LINK";
    DDS_InstanceHandle_t instance_handle = DDS_HANDLE_NIL;
    struct DDS_Duration_t send_period = {4,0};
    int count = 0;  
    int domain_idx = 0, domain_idx2 = 0; 
    ReturnCode_t   retcode; 
    LinkData link_data;

    if (argc >= 2) {
        domainId = atoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    /*
    if ( domain_idx = iprclink_api_create_link(domainId, topic_name_r, IPRC_COMM_LINKListener_on_data_available,LINK_READER) < 0 )
    {
       sys_printlog(SYS_LOG_INFO, "iprclink_api_create_link %s ERROR", topic_name_r);
       return;          
    }
    */

    if ( domain_idx = iprclink_api_create_link(domainId, topic_name_s, IPRC_COMM_LINKListener_on_data_available, LINK_WRITER) < 0 )
    {
       sys_printlog(SYS_LOG_INFO, "iprclink_api_create_link %s ERROR", topic_name_s);
       return;          
    }
    /* create data instance by alocate memory */
    link_data.body = iprclink_writer_api_create_data();
    if (link_data.body == NULL) {
         sys_printlog(SYS_LOG_INFO, "IPRC_COMM_LINKTypeSupport_create_data error");
         writer_shutdown(domain_list[domain_idx].participant);
        return -1;
    }

    /* for debug */
    //print_type_code(instance, "Data");

    //data packets 
    memset(link_data.head.fromlink, '\0', sizeof(link_data.head.fromlink));
    memset(link_data.head.tolink, '\0', sizeof(link_data.head.tolink));
    //strcpy(dest, src);
    strcpy(link_data.head.fromlink,topic_name_r);
    strcpy(link_data.head.tolink,topic_name_s);
    link_data.body->Data = NULL;
    link_data.body->Name = NULL;   
 
    for (count=0; (sample_count == 0) || (count < sample_count); ++count) 
    {
       sys_printlog(SYS_LOG_DEBUG, "Writing to topic:'%s',domain:'%d', count %d", topic_name_s, domainId, count);

        link_data.body->Rec_num = count + 1;
       // retcode = IPRC_COMM_LINKDataWriter_write(
       //     iprc_data_writer, instance, &instance_handle);
        
        retcode = iprclink_api_send(&link_data);
        if (retcode != DDS_RETCODE_OK) {
            sys_printlog(SYS_LOG_INFO, "write error %d", retcode);
        }
       

        NDDS_Utility_sleep(&send_period);
    } 

    /* Delete data sample */
    //retcode = IPRC_COMM_LINKTypeSupport_delete_data_ex(instance, DDS_BOOLEAN_TRUE);
    retcode = iprclink_api_delete_data(link_data.body);
    if (retcode != DDS_RETCODE_OK) {
        sys_printlog(SYS_LOG_INFO, "IPRC_COMM_LINKTypeSupport_delete_data error %d", retcode);
    }

    /* Cleanup and delete delete all entities */         
    //return writer_shutdown(domain_list[domain_idx].participant);
}
#endif /* for TEST */
