
/* iprclink_api_test.c
*/

#include <stdio.h>
#include <stdlib.h>
//#include "ndds/ndds_c.h"
//#include "IPRC_COMM_LINK.h"
//#include "IPRC_COMM_LINKSupport.h"
#include "iprclink__interface.h"

void Listener_on_data_available(void *listener_data, DDS_DataReader* reader)
{

}

int main(int argc, char *argv[])
{
    int domainId     = DEFAULT_DOMAIN_ID;
    int sample_count = 50; /*if 0 then infinite loop */
    char *topic_name_r = "IPRC_SUB_LINK";
    char *topic_name_s = "IPRC_PUB_LINK";
    DDS_InstanceHandle_t instance_handle = DDS_HANDLE_NIL;
    struct DDS_Duration_t send_period = {4,0};
    int count = 0;  
    int domain_idx = 0, domain_idx2 = 0; 
    ReturnCode_t   retcode; 
    LinkData *link_data;
    char buff[100];

    if (argc >= 2) {
        domainId = atoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    if ( domain_idx = iprclink_api_create_publisher(domainId, topic_name_s, NULL) < 0 )
    {
       fprintf(stderr, "iprclink_api_create_link %s ERROR\n", topic_name_s);
       return;          
    }
   
    iprclink_api_set_my_domain(domainId);
    iprclink_api_set_my_topic(topic_name_s);

    /* create data/instance by alocate memory */
    
    link_data = iprclink_writer_api_create_data();
    if (link_data == NULL) {
         fprintf(stderr, "IPRC_COMM_LINKTypeSupport_create_data error\n");
         writer_shutdown(domain_list[domain_idx].participant);
        return -1;
    }
  

    //data packets 
    memset(link_data->h_srcTopic, '\0', sizeof(link_data->h_srcTopic));
    memset(link_data->h_destTopic, '\0', sizeof(link_data->h_destTopic));
    //strcpy(dest, src);
    strcpy(link_data->h_srcTopic,topic_name_r);
    strcpy(link_data->h_destTopic,topic_name_s);
    memset(link_data->Data, '\0', sizeof(link_data->Data));
    link_data->h_Type = LINK_MSG_K_DATA;   
 
 
    for (count=0; (sample_count == 0) || (count < sample_count); ++count) 
    {
        printf("Writing to topic:'%s',domain:'%d', count %d\n", topic_name_s, domainId, count);

        sprintf(buff, "This is record %d", count + 1);
        link_data->h_Domain = iprclink_api_get_my_domain();
        link_data->h_RefNumber = count + 1;
        link_data->h_Time_Date = timeutils_get_universal_time();
        strncpy(link_data->Data, buff, strlen(buff));
        link_data->h_DataLen = strlen(buff);
        link_data->h_Type = LINK_MSG_K_DATA;
        
        retcode = iprclink_api_send(link_data);
        if (retcode != DDS_RETCODE_OK) {
            fprintf(stderr, "write error %d\n", retcode);
        }
       

        NDDS_Utility_sleep(&send_period);
    } 
    

    /* Delete data sample */
    memset(link_data->Data, '\0', sizeof(link_data->Data));
    link_data = NULL;
   /*
    retcode = iprclink_api_delete_data(link_data.MSG);
    if (retcode != DDS_RETCODE_OK) {
        fprintf(stderr, "IPRC_COMM_LINKTypeSupport_delete_data error %d\n", retcode);
    }
    */
    /* Cleanup and delete delete all entities */         
    //return writer_shutdown(domain_list[domain_idx].participant);

}
//#endif /* for TEST */
