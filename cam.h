/* All code provided is as is 
 * and not completely tested
 *
 * Author: Aadil Rizvi
 * Date: 22/1/2016
 *
 * Website: arembedded.com
*/

#ifndef CAM_H
#define CAM_H

#include <pthread.h>
#include <util/util.h>
#include <mqueue.h>
#include <fcntl.h>
#include <cam_inf.h>

#define CAM_MQ_NAME "/cam_mq"

extern struct mq_attr cam_mq_attr;
extern mqd_t cam_mq;

typedef enum cam_state_id_s {
    CAM_STATE_POWERED_OFF=0,
    CAM_STATE_POWERED_ON,
    CAM_STATE_TAKING_IMG,
    CAM_STATE_CHANGING_BRIGHTNESS
} cam_state_id_t;

typedef struct cam_state_s {
    cam_state_id_t state_id;
    struct timeval timeout_ts;
} cam_state_t;

void cam_main(void);
void cam_power_on_msg_handler(void);
void cam_take_img_msg_handler(void);
void cam_change_brt_msg_handler(cam_change_brt_data_t data);

#endif
