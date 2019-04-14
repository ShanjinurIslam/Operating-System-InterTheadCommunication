#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include<queue>
#include<semaphore.h>
#include <iostream>
#include<string.h>

using namespace std ;

struct cake{
    string s ;
};

sem_t e;
sem_t f;
sem_t w1;
sem_t w2;

queue<struct cake> q1;
queue<struct cake> q2;
queue<struct cake> q3;
pthread_mutex_t lock;
pthread_mutex_t lockw1;
pthread_mutex_t lockw2;

void *chefXY(void * arg){
    while(true){
        sem_wait(&e);
        int choice = *(int *)(arg) ;
        if(choice==1){
            pthread_mutex_lock(&lock);
            struct cake c ;
            c.s = "Vanilla" ;		
            q1.push(c);
            cout<<"Chef X : Pushed 1 Vanilla Cake to Queue 1"<<endl ;
            sem_post(&f);
            pthread_mutex_unlock(&lock);   
        }
        else{
            pthread_mutex_lock(&lock);
            struct cake c ;
            c.s = "Chocolate" ;		
            q1.push(c);
            cout<<"Chef Y : Pushed 1 Chocolate Cake to Queue 1"<<endl ;
            sem_post(&f);
            pthread_mutex_unlock(&lock);   
        }
        sleep(1) ;
    }
}

void *waiter12(void * arg){
    while(true){
        int choice = *(int *)(arg) ;
        if(choice==1){
            sem_wait(&w1);
            pthread_mutex_lock(&lockw1);
            struct cake c ;
            c = q2.front() ;
            q2.pop() ;
            cout<<"Waiter 1 : Extracted 1 "<<c.s<<" Cake from Queue 2"<<endl ;
            pthread_mutex_unlock(&lockw1);   
        }
        else{
            sem_wait(&w2);
            pthread_mutex_lock(&lockw2);
            struct cake c ;
            c = q3.front() ;
            q3.pop() ;
            cout<<"Waiter 2 : Extracted 1 "<<c.s<<" Cake from Queue 2"<<endl ;
            pthread_mutex_unlock(&lockw2);   
        }
        sleep(1) ;
    }
}

void *chefZWork(void *arg){
    while(true){
        sem_wait(&f);
        pthread_mutex_lock(&lock);
        struct cake details = q1.front() ;
        q1.pop() ;
        pthread_mutex_unlock(&lock);  
        cout<<"Chef Z : Extracted 1 "<<details.s<<" Cake from Queue 1"<<endl ;
        if(details.s=="Vanilla"){
            sem_post(&w1) ;
            pthread_mutex_lock(&lockw1);
            q2.push(details) ;
            cout<<"Chef Z : Pused 1 "<<details.s<<" Cake to Queue 2"<<endl ;
            pthread_mutex_unlock(&lockw1);
        }
        else{
            sem_post(&w2) ;
            pthread_mutex_lock(&lockw2);
            q3.push(details) ;
            cout<<"Chef Z : Pused 1 "<<details.s<<" Cake to Queue 3"<<endl ;
            pthread_mutex_unlock(&lockw2);
        }
        sem_post(&e);
        sleep(1) ;
    }
}

void init_semaphore()
{ 
	sem_init(&e,0,5);
	sem_init(&f,0,0);
    sem_init(&w1,0,0);
    sem_init(&w2,0,0);
    pthread_mutex_init(&lock,0);
    pthread_mutex_init(&lockw1,0);
    pthread_mutex_init(&lockw2,0);
}

int main(){
    int x = 1 ;
    int y = 2 ;
    int z = 1 ;
    init_semaphore() ;
    pthread_t chefX,chefY,chefZ,waiter1,waiter2 ;
    pthread_create(&chefZ,NULL,chefZWork,&z) ;
    pthread_create(&chefX,NULL,chefXY,&x) ;
    pthread_create(&chefY,NULL,chefXY,&y) ;
    pthread_create(&waiter1,NULL,waiter12,&x) ;
    pthread_create(&waiter1,NULL,waiter12,&y) ;
    pthread_join(chefX,NULL) ;
    pthread_join(chefY,NULL) ;
    pthread_join(chefZ,NULL) ;
    pthread_join(waiter1,NULL) ;
    pthread_join(waiter2,NULL) ;
    return 0 ;
}
