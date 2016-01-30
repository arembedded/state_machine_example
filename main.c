/* All code provided is as is 
 * and not completely tested
 *
 * Author: Aadil Rizvi
 * Date: 22/1/2016
 *
 * Website: arembedded.com
*/

#include <thread1.h>
#include <cam.h>

pthread_t thread1;
pthread_t cam;

void sig_handler(int signum) {
    if (signum != SIGINT) {
        printf("Received invalid signum = %d in sig_handler()\n", signum);
        ASSERT(signum == SIGINT);
    }

    printf("Received SIGINT. Exiting Application\n");

    pthread_cancel(thread1);
    pthread_cancel(cam);

    mq_close(cam_mq);
    mq_unlink(CAM_MQ_NAME);

    exit(0);
}

int main(void) {
    pthread_attr_t attr;
    int status; 

    signal(SIGINT, sig_handler);

    thread1_init();
    cam_init();

    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 1024*1024);
    
    printf("Creating thread1\n");
    status = pthread_create(&thread1, &attr, (void*)&thread1_main, NULL);
    if (status != 0) {
        printf("Failed to create thread1 with status = %d\n", status);
        ASSERT(status == 0);
    }    

    printf("Creating CAM thread\n");
    status = pthread_create(&cam, &attr, (void*)&cam_main, NULL);
    if (status != 0) {
        printf("Failed to create CAM thread with status = %d\n", status);
        ASSERT(status == 0);
    }

    pthread_join(thread1, NULL);
    pthread_join(cam, NULL);

    return 0;
}

