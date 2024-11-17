//This program brute-forces a given password hash by trying all possible
//passwords of a given length.
//
//Usage:
//crack <threads> <keysize> <target>
//
//Where <threads> is the number of threads to use, <keysize> is the maximum
//password length to search, and <target> is the target password hash.
//
//For example:
//
//./crack 1 5 na3C5487Wz4zw
//
//Should return the password 'apple'

#include <crypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct CrackThreader {
    int id;
    int size;
    char* target;
    char startLetter;
    char endLetter;
};

void *check(void *arg) {
  struct CrackThreader *data = (struct CrackThreader*)arg;
  int currentCheck[data->size+1];
  char currentCheckStr[256];
  struct crypt_data cryptData;

  for(int runSize = 1; runSize <= data->size; runSize++){
    for (int i = 0; i < runSize; i++) {
      if (i == 0) {
        currentCheck[i] = data->startLetter - 'a';
      } 
      else {
        currentCheck[i] = 0;
      } 
    }
    while (1) {
      currentCheckStr[0] = '\0';

      for (int i = 0; i < runSize; i++) {
        char temp[2]; 
        temp[0] = 'a' + currentCheck[i]; 
        temp[1] = '\0'; 
        strcat(currentCheckStr, temp);
      }
      
      //printf("'%s'", currentCheckStr);
      //printf("\n");

      char salt[3];
      salt[0] = data->target[0];
      salt[1] = data->target[1];
      salt[2] = '\0';

      char* result;
      result = crypt_r(currentCheckStr, salt, &cryptData);
      if(strcmp(result, data->target) == 0){
        printf("Match Found: %s\n", currentCheckStr);
        exit(1);
      }

      int counter = 1;
      for (int i = runSize - 1; i >= 0; i--) {
        currentCheck[i] += counter;
        counter = currentCheck[i] / 26;
        currentCheck[i] %= 26;
      }
      int timeToEnd = 0;
      if (currentCheck[0] == data->endLetter - 'a' + 1){
        //printf("Passed end letter, broke\n");
        timeToEnd = 1;
      }
      if (counter == 1){
        //printf("Counter Reached 1, broke");
        timeToEnd = 1;
      }
      if(timeToEnd == 1){
        if(runSize == data->size){
          return NULL;
        }
        else{
          break;
        }
      }
    }
  }
  //printf("No match found\n");
  return NULL;
}

int main( int argc, char* argv[] ){
  if(argc != 4){
    perror("Usage: crack <threads> <keysize> <target>");
    exit(EXIT_FAILURE);
  }
  if(atoi(argv[2]) > 8){
    perror("Maximum allowable keysize is 8");
    exit(EXIT_FAILURE);
  }

  int numThreads = atoi(argv[1]);
  int keySize = atoi(argv[2]);
  char* target = argv[3];

  pthread_t threads[numThreads];
  struct CrackThreader threadArgs[numThreads];

  int lettersPerThread = 26 / numThreads;
  int remainderCounter = 26 % numThreads;

  char startLetter = 'a';
  char endLetter = startLetter + lettersPerThread - 1;
  if(remainderCounter > 0){
    endLetter++;
    remainderCounter--;
  }
  for(int i = 0; i < numThreads; i++) {
    threadArgs[i].id = i;
    threadArgs[i].size = keySize;
    threadArgs[i].target = target;
    threadArgs[i].startLetter = startLetter;
    threadArgs[i].endLetter = endLetter;

    //printf("creating thread with startLetter %i and endLetter %i\n", startLetter - 'a' + 1 ,endLetter - 'a' + 1);

    pthread_create(&threads[i], NULL, check, (void*)&threadArgs[i]);

    startLetter = endLetter + 1;
    endLetter += lettersPerThread;
    if(remainderCounter > 0){
      endLetter += 1;
      remainderCounter--;
    }
  }

  for (int i = 0; i < numThreads; i++) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}
