#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// 定义互斥锁和条件变量
pthread_mutex_t mutex;
pthread_cond_t odd_cond;
pthread_cond_t even_cond;

int number = 1;
int MAX_NUMBER = 100;

// 奇数线程函数
void* print_odd(void* arg) {
    while (number < MAX_NUMBER) {
        // 上锁
        pthread_mutex_lock(&mutex);
        
        // 当前是偶数，等待奇数线程打印
        if (number % 2 == 0) {
            ///////Begin///////////
            pthread_cond_wait(&odd_cond, &mutex);  

            ////////End//////////// 
        }
        
        // 打印奇数
        printf("%d\n", number++);
        
        // 发送信号给偶数线程
        pthread_cond_signal(&even_cond);
        
        // 解锁
        pthread_mutex_unlock(&mutex);
    }
    
    pthread_exit(NULL);
}

// 偶数线程函数
void* print_even(void* arg) {
    while (number < MAX_NUMBER) {
        // 上锁
        pthread_mutex_lock(&mutex);
        
        // 当前是奇数，等待偶数线程打印
        if (number % 2 == 1) {
            ///////Begin///////////
            pthread_cond_wait(&even_cond, &mutex);  

            ////////End//////////// 
        }
        
        // 打印偶数
        printf("%d\n", number++);
        
        // 发送信号给奇数线程
        pthread_cond_signal(&odd_cond);
        
        // 解锁
        pthread_mutex_unlock(&mutex);
    }
    
    pthread_exit(NULL);
}

int main() {
    // 初始化互斥锁和条件变量
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&odd_cond, NULL);
    pthread_cond_init(&even_cond, NULL);
    
    // 创建奇数线程和偶数线程
    pthread_t odd_thread, even_thread;
    pthread_create(&odd_thread, NULL, print_odd, NULL);
    pthread_create(&even_thread, NULL, print_even, NULL);
    
    // 等待线程执行结束
    pthread_join(odd_thread, NULL);
    pthread_join(even_thread, NULL);
    
    // 销毁互斥锁和条件变量
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&odd_cond);
    pthread_cond_destroy(&even_cond);
    
    return 0;
}
