/* All code provided is as is 
 * and not completely tested
 *
 * Author: Aadil Rizvi
 * Date: 22/1/2016
 *
 * Website: arembedded.com
*/

#include <thread1.h>
#include <cam_inf.h>

void thread1_init(void) {

    //Nothing right now    

}

void thread1_main(void) {
    unsigned int exec_period_usecs;
    int cnt=0;

    exec_period_usecs = 300000; /*in micro-seconds*/

    printf("Thread 1 started. Execution period = %d uSecs\n",\
                                           exec_period_usecs);
    while(1) {
        switch (cnt) {

            case 0:
                cam_change_brt(200);
                break;
            case 1:
                cam_power_on();
                break;
            case 2:
                cam_take_img();
                break;
            case 3:
                cam_take_img();
                break;
            case 4:
                cam_take_img();
                break;
            case 5:
                cam_change_brt(100);
                break;
            case 6:
                break;
            default:
                printf("Invalid counter value in sending thread\n");
                break;
        }

        cnt += 1;

        if (cnt > 6) {
           cnt = 0;
        }
        usleep(exec_period_usecs);
    }
}

