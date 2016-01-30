/* All code provided is as is 
 * and not completely tested
 *
 * Author: Aadil Rizvi
 * Date: 22/1/2016
 *
 * Website: arembedded.com
*/

#include <cam_inf.h>
#include <cam.h>

void cam_power_on(void) {
    int status;
    cam_mq_msg_t send_msg;

    send_msg.type = CAM_POWER_ON;

    status = mq_send(cam_mq, (const char*)&send_msg, sizeof(send_msg), 1);
    ASSERT(status != -1);
}

void cam_take_img(void) {
    int status;
    cam_mq_msg_t send_msg;

    send_msg.type = CAM_TAKE_IMG;

    status = mq_send(cam_mq, (const char*)&send_msg, sizeof(send_msg), 1);
    ASSERT(status != -1);
}

void cam_change_brt(unsigned int brightness) {
    int status;
    cam_mq_msg_t send_msg;

    send_msg.type = CAM_CHANGE_BRT;
    send_msg.data.cam_change_brt.brightness = brightness;

    status = mq_send(cam_mq, (const char*)&send_msg, sizeof(send_msg), 1);
    ASSERT(status != -1);
}




