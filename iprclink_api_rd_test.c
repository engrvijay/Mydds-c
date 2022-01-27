
/* iprclink_api_test.c
*/

#include <stdio.h>
#include <stdlib.h>
//#include "ndds/ndds_c.h"
//#include "IPRC_COMM_LINK.h"
//#include "IPRC_COMM_LINKSupport.h"
#include "iprclink__interface.h"

/* include if want to build test executable */

void Listener_on_data_available(void *listener_data, DDS_DataReader* reader)
{
    LINKDataReader *data_reader = NULL;
    struct IPRC_COMM_LINKSeq data_seq = DDS_SEQUENCE_INITIALIZER;
    struct DDS_SampleInfoSeq info_seq = DDS_SEQUENCE_INITIALIZER;
    DDS_ReturnCode_t retcode;
    int i, domain_idx, reader_idx;
    char *topic_name = "IPRC_SUB_LINK";


   printf("%s-Received data\n", __func__);
   if ( domain_idx = is_domain_participant_created(DEFAULT_DOMAIN_ID) < 0 )
   {
       fprintf(stderr,"%s-no initialization fornd for domain:%d\n",__func__, DEFAULT_DOMAIN_ID);
       return ;
   }

   if ( reader_idx = is_domain_topic_created(domain_idx,topic_name) < 0 )
   {
       fprintf(stderr,"%s-no data_writer found for link:%s\n",__func__, topic_name);
       return ;
   }

   fprintf(stderr, "%s-domain_idx:%d,writer_idx:%d for topic:%s\n",__func__,domain_idx,reader_idx,topic_name);
   data_reader = domain_list[domain_idx].topic_list[reader_idx].iprc_link_reader;
   if( data_reader == NULL)
   {
      fprintf(stderr,"%s-no data_reader failed for link:%s\n",__func__, topic_name);
      return ;
   }


    retcode = IPRC_COMM_LINKDataReader_take(
        data_reader,
        &data_seq, &info_seq, DDS_LENGTH_UNLIMITED,
        DDS_ANY_SAMPLE_STATE, DDS_ANY_VIEW_STATE, DDS_ANY_INSTANCE_STATE);
    if (retcode == DDS_RETCODE_NO_DATA) {
        return;
    } else if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "take error %d\n", retcode);
        return;
    }

    for (i = 0; i < IPRC_COMM_LINKSeq_get_length(&data_seq); ++i) {
        if (DDS_SampleInfoSeq_get_reference(&info_seq, i)->valid_data) {
           printf("Received data\n");
           IPRC_COMM_LINK   *rcvd = IPRC_COMM_LINKSeq_get_reference(&data_seq, i);
           printf ("Time  : %ld\n",rcvd->h_Time_Date);
           printf ("RefNum: %d\n",rcvd->h_RefNumber);
           printf ("Type  : %d\n",rcvd->h_Type);
           printf ("Data  : %s\n",rcvd->Data);
           printf ("DataL : %ld\n",rcvd->h_DataLen);

        }
    }

    retcode = IPRC_COMM_LINKDataReader_return_loan(
        data_reader,
        &data_seq, &info_seq);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "Reader_return_loan error %d\n", retcode);
    }
}



int main(int argc, char *argv[])
{
    int domainId     = DEFAULT_DOMAIN_ID;
    int sample_count = 100; /*if 0 then infinite loop */
    char *topic_name_r = "IPRC_SUB_SUPR";
    char *topic_name_s = "IPRC_PUB_LINK";
    DDS_InstanceHandle_t instance_handle = DDS_HANDLE_NIL;
    struct DDS_Duration_t send_period = {4,0};
    int count = 0;  
    int domain_idx = 0, domain_idx2 = 0; 
    ReturnCode_t   retcode; 
    LinkData *link_data;
    int x = sizeof(link_data->Data);

    if (argc >= 2) {
        sample_count = atoi(argv[1]);
    }

     
    //if ( domain_idx = iprclink_api_create_link(domainId, topic_name_s, Listener_on_data_available, LINK_READER) < 0 )
    if ( domain_idx = iprclink_api_create_subscriber(domainId, topic_name_s, Listener_on_data_available) < 0 )
    {
       fprintf(stderr, "iprclink_api_create_link %s ERROR\n", topic_name_s);
       return;          
    }
    
   /*
    if ( domain_idx = iprclink_api_create_link(domainId, topic_name_s,Listener_on_data_available, LINK_WRITER) < 0 )
    {
       fprintf(stderr, "iprclink_api_create_link %s ERROR\n", topic_name_s);
       return;          
    }
    */

    /* create data instance by alocate memory */
    link_data = iprclink_writer_api_create_data();
    if (link_data == NULL) {
         fprintf(stderr, "IPRC_COMM_LINKTypeSupport_create_data error\n");
         writer_shutdown(domain_list[domain_idx].participant);
        return -1;
    }

    /* for debug */
    //print_type_code(instance, "Data");

    //data packets 
    memset(link_data->h_srcTopic, '\0', sizeof(link_data->h_srcTopic));
    memset(link_data->h_destTopic, '\0', sizeof(link_data->h_destTopic));
    //strcpy(dest, src);
    strcpy(link_data->h_srcTopic,topic_name_r);
    strcpy(link_data->h_destTopic,topic_name_s);
    memset(link_data->Data, '\0', sizeof(link_data->Data));
    link_data->h_Type = LINK_MSG_K_UNKNOWN;   
 
 
    for (count=0; (sample_count == 0) || (count < sample_count); ++count) 
    {
        printf("Reading from topic:'%s',domain:'%d', count %d\n", topic_name_s, domainId, count);

        link_data->h_RefNumber = count + 1;
       // retcode = IPRC_COMM_LINKDataWriter_write(
       //     iprc_data_writer, instance, &instance_handle);
        
        /*
        retcode = iprclink_api_send(&link_data);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "write error %d\n", retcode);
        }
        */    
        NDDS_Utility_sleep(&send_period);
    } 
    
    /* Delete data sample */
    //retcode = IPRC_COMM_LINKTypeSupport_delete_data_ex(instance, DDS_BOOLEAN_TRUE);
    retcode = iprclink_api_delete_data(link_data);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "IPRC_COMM_LINKTypeSupport_delete_data error %d\n", retcode);
    }

    /* Cleanup and delete delete all entities */         
    //return writer_shutdown(domain_list[domain_idx].participant);

}
