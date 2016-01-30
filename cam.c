/* All code provided is as is 
 * and not completely tested
 *
 * Author: Aadil Rizvi
 * Date: 22/1/2016
 *
 * Website: arembedded.com
*/

#include <cam.h>

struct mq_attr cam_mq_attr;
mqd_t cam_mq;

/* CAM state */
static cam_state_t cam_state;

unsigned char time_diff(struct timeval *curr_time, struct timeval *timeout_ts) {
    ASSERT(curr_time != NULL);
    ASSERT(timeout_ts != NULL);

    if (curr_time->tv_sec > timeout_ts->tv_sec)
        return 1;

    if ((curr_time->tv_sec == timeout_ts->tv_sec) && (curr_time->tv_usec >= timeout_ts->tv_usec))
        return 1;

    return 0; 
}

void cam_set_state(cam_state_id_t new_cam_state, unsigned int timeout_us) {
    struct timeval timestamp;

    ASSERT(new_cam_state <= CAM_STATE_CHANGING_BRIGHTNESS);

    // Get current timestamp
    gettimeofday(&timestamp, NULL);

    // Add the whole seconds from input timeout specified
    // to current time whole seconds to check for expiration later
    timestamp.tv_sec += timeout_us / 1000000;

    // Check for usec rollover to whole second when adding 
    // input argument timeout us amount to current time
    if (timestamp.tv_usec + (timeout_us % 1000000) >= 1000000) {
       timestamp.tv_sec += 1;
       timestamp.tv_usec = ((timeout_us % 1000000) + timestamp.tv_usec) % 1000000;
    }
    else {
       timestamp.tv_usec = (timeout_us % 1000000) + timestamp.tv_usec;
    }

    if (timeout_us > 0) {
        cam_state.timeout_ts = timestamp;
    }
    else {
        cam_state.timeout_ts.tv_sec = 0;
        cam_state.timeout_ts.tv_usec = 0;
    }

    gettimeofday(&timestamp, NULL);
    printf("%ld.%06ld: CAM changing from state %d to %d\n",\
                                            timestamp.tv_sec,
                                            timestamp.tv_usec,
                                            cam_state.state_id,
                                            new_cam_state);
    cam_state.state_id = new_cam_state;  
}

void cam_init(void) {

    cam_mq_attr.mq_maxmsg = 10;
    cam_mq_attr.mq_msgsize = sizeof(cam_mq_msg_t);

    cam_mq = mq_open(CAM_MQ_NAME, \
                    O_CREAT | O_RDWR | O_NONBLOCK, \
                    0666, \
                    &cam_mq_attr);

    ASSERT(cam_mq != -1);

    // Hard code initial CAM state
    cam_state.state_id = CAM_STATE_POWERED_OFF;
}

void cam_main(void) {
    unsigned int exec_period_usecs;
    int status;
    cam_mq_msg_t recv_msg;
    struct timeval curr_time;
    unsigned char timeout_status;

    exec_period_usecs = 1000; /*in micro-seconds*/

    printf("CAM thread started. Execution period = %d uSecs\n",\
                                           exec_period_usecs);

    while(1) {
        gettimeofday(&curr_time, NULL);

        // Check for timeout in cam state
        // If timed out then change state to POWERED_ON
        // (i.e. CAM is done taking an image or changing brightness)

        timeout_status = time_diff(&curr_time, &cam_state.timeout_ts);
        if (cam_state.timeout_ts.tv_sec != 0 && (timeout_status == 1)) {
            printf("%ld.%06ld: Timeout Occured. ", curr_time.tv_sec, curr_time.tv_usec);
            switch (cam_state.state_id) {
                case CAM_STATE_TAKING_IMG:
                    printf("Done Taking an IMG\n");
                    break;
                case CAM_STATE_CHANGING_BRIGHTNESS:
                    printf("Done Changing Brightness\n");
                    break;
                default:
                    printf("CAM timeout occured in bad state %d\n", cam_state.state_id);
                    ASSERT(0);
                    break;
            }
            cam_set_state(CAM_STATE_POWERED_ON, 0);
        }

        status = mq_receive(cam_mq, (char*)&recv_msg, \
                            sizeof(recv_msg), NULL);

        if (status == sizeof(recv_msg)) {

            switch(recv_msg.type) {
                case CAM_POWER_ON:
                    cam_power_on_msg_handler();
                    break;         
                case CAM_TAKE_IMG:
                    cam_take_img_msg_handler();
                    break;         
                case CAM_CHANGE_BRT:
                    cam_change_brt_msg_handler(recv_msg.data.cam_change_brt);
                    break;         
                default:
                    printf("CAM Received invalid msg.\n");
                    ASSERT(0);
                    break;         
            }
        }
 
        usleep(exec_period_usecs);
    }
}

void cam_power_on_msg_handler(void) {
    struct timeval ts;

    gettimeofday(&ts, NULL);

    // Check current state
    // Must be POWERED_OFF.
    if (cam_state.state_id != CAM_STATE_POWERED_OFF) {
        printf("%ld.%06ld: Received CAM_POWER_ON command in bad state %d\n",
                                                              ts.tv_sec,
                                                              ts.tv_usec,
                                                              cam_state.state_id);
        return;
    } 

    printf("%ld.%06ld: Turning CAM POWER ON\n", ts.tv_sec, ts.tv_usec);

    // Set CAM state as POWERED_ON forever
    cam_set_state(CAM_STATE_POWERED_ON, 0);
}

void cam_take_img_msg_handler(void) {
    struct timeval ts;

    gettimeofday(&ts, NULL);

    // Check current state
    // Must be POWERED_ON.
    if (cam_state.state_id != CAM_STATE_POWERED_ON) {
        printf("%ld.%06ld: Received CAM_TAKE_IMG command in bad state %d\n",
                                                       ts.tv_sec,
                                                      ts.tv_usec,
                                              cam_state.state_id);
        return;
    } 

    printf("%ld.%06ld: CAM TAKING IMAGE\n", ts.tv_sec, ts.tv_usec);

    // Set CAM state as TAKING_IMG for 1 sec.
    cam_set_state(CAM_STATE_TAKING_IMG, 1000000);
}

void cam_change_brt_msg_handler(cam_change_brt_data_t data) {
    struct timeval ts;

    gettimeofday(&ts, NULL);

    // Check current state
    // Must be POWERED_ON.
    if (cam_state.state_id != CAM_STATE_POWERED_ON) {
        printf("%ld.%06ld: Received CAM_CHANGE_BRIGHTNESS command in bad state %d\n",
                                                                ts.tv_sec,
                                                               ts.tv_usec,
                                                       cam_state.state_id);
        return;
    } 

    printf("%ld.%06ld: CAM CHANGING BRIGHTNESS to %d\n",
                                              ts.tv_sec,
                                             ts.tv_usec,
                                        data.brightness);

    // Set CAM state as CHANGING BRIGHTNESS for 2 secs.
    cam_set_state(CAM_STATE_CHANGING_BRIGHTNESS, 2000000);
}
