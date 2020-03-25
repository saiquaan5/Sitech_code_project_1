#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>

#define NUMTHREADS          2

using namespace std;

void *thread1(void *threadid)
{
    while(1)
    {
        cout << "hello from thread 1" <<endl;   
    }
    sleep(3);
}

void *thread2(void *threadid)
{
    while(1)
    {
        cout << "hello from thread 2" <<endl;   
    }
    sleep(4);
}

int main()
{
    pthread_t threads[NUMTHREADS];
    int rc, i = 1;
    cout << "Create thread " << i <<endl;
    if(rc = pthread_create(&threads[i],NULL,thread1,NULL))
    {
        cout << "Unable to create thread " << rc <<endl;
        return 0;
    }
    i++;
    if(rc = pthread_create(&threads[i],NULL,thread2,NULL))
    {
        cout << "Unable to create thread " << rc <<endl;
        return 0;
    }
    while(1)
    {
        printf("main hello \n");
        sleep(2);
    }
    pthread_exit(NULL);
    return 0;
}

