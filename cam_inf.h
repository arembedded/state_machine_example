/* All code provided is as is
 * and not completely tested
 *
 * Author: Aadil Rizvi
 * Date: 22/1/2016
 *
 * Website: arembedded.com
*/         

#ifndef CAM_INF_H
#define CAM_INF_H

#include <pthread.h>
#include <util/util.h>
#include <mqueue.h>
#include <fcntl.h>

typedef enum cam_msg_id_s {
    CAM_POWER_ON = 0,
    CAM_TAKE_IMG,
    CAM_CHANGE_BRT
} cam_msg_id_t;

typedef union cam_change_brt_data_s {
    unsigned int brightness;
} cam_change_brt_data_t;

typedef union cam_data_s {
    cam_change_brt_data_t cam_change_brt;
} cam_data_t;

typedef struct my_mq_msg_s {
    cam_msg_id_t type;
    cam_data_t data;
} cam_mq_msg_t;

void cam_power_on(void);
void cam_take_img(void);
void cam_change_brt(unsigned int val);

#endif
