#include<iostream>
#include<pthread.h>
using namespace std;

int *array; int *resArray;
int n;
int count=0;
const int sortThreadNum=2;

void sort(int low, int high){
	for (int i=low;i<high;i++){
		int tmp=i;
		for(int j=i+1;j<high+1;j++){
			if (array[j]<array[tmp]){
				tmp = j;
			}
		}
		int tmpVal=array[tmp];
		array[tmp]=array[i];
		array[i]=tmpVal;
	}
}

void *merge(void *arg){
	int index1=0,index2=n/2;
	for(int i=0;i<n;i++){
		if(index1<n/2 && index2<n){
			if(array[index1]<=array[index2]){resArray[i]=array[index1++];}
			else{resArray[i]=array[index2++];}
		}
		else{
			if(index1<n/2){resArray[i]=array[index1++];}
			if(index2<n){resArray[i]=array[index2++];}
		}
	}
}

void *sortAlgorithm(void *arg){
	int curThread=count++;
	int low=curThread*(n/2);
	int high=(curThread+1)*(n/2)-1;
	sort(low,high);
}

int main() {
	cout<<"Please Input the number of array you want to sort: "; cin>>n;
	cout<<"Please Input the array: "; 

	array=new int[n];
	resArray=new int[n];

	for(int i=0;i<n;i++){
		cin>>array[i];
	}

	pthread_t sortThread[sortThreadNum];
	for(int i=0;i<sortThreadNum;i++){
		pthread_create(&sortThread[i],NULL,sortAlgorithm,(void*)NULL);
	}

	for(int i=0;i<sortThreadNum;i++){
		pthread_join(sortThread[i],NULL);
	}

	cout<<"Raw Array: ";
	for(int i=0;i<n;i++){
		cout<<array[i]<<' ';
	}


	pthread_t mergeThread;
	pthread_create(&mergeThread,NULL,merge,(void*)NULL);
	pthread_join(mergeThread,NULL);

	cout<<"\nAfter sorting: ";
	for(int i=0;i<n;i++){
		cout<<resArray[i]<<' ';
	}
	cout<<"\n";
	
	return 0;
	}