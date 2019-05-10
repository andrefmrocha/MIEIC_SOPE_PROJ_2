#include "producer.h"
extern sem_t empty, full;

void produce_data(tlv_request_t *request) {
  sem_wait(&empty);
  push_data(request);
  sem_post(&full);
}

int initialize_shutdown(tlv_request_t * request){
  if(request->value.header.account_id == ADMIN_ACCOUNT_ID){
    int active_threads = stop_sync();
    tlv_reply_t reply;
    reply.type = OP_SHUTDOWN;
    reply.value.header.account_id = ADMIN_ACCOUNT_ID;
    reply.value.header.ret_code = RC_OK;
    reply.value.shutdown.active_offices = active_threads;
    answer_user(request->value.header.pid, &reply);
    return 0;
  }else{
    tlv_reply_t reply;
    reply.type = OP_SHUTDOWN;
    reply.value.header.account_id = ADMIN_ACCOUNT_ID;
    reply.value.header.ret_code = RC_OP_NALLOW;
    answer_user(request->value.header.pid, &reply);
    return -1;
  }
  

}