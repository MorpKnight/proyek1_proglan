#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>

#define DELAY 10 // ganti delay sesuka hati

void delay(int miliSeconds){
    clock_t start_time = clock();
    while (clock() < start_time + miliSeconds);
}

void graphSort(int arr[], int len){
    int i,j;
    system("cls");
    for(i=0 ; i<len ; i++){
        printf("%3d|", arr[i]);
        for(j=0 ; j<arr[i] ; j++){
            printf("#");
        }
        printf("\n");
    }
    delay(DELAY);
}

void randomList(int *list, int n){
    srand(time(NULL));
    for (int i = 0; i < n; i++){
        list[i] = rand() % 100;
    }
}

void randomNatural(int arr[], int len) {
    int i,j,temp;

    for (i = 0; i < len; i++) {
        arr[i] = i + 1;
    }

    for (i = 0; i < len; i++) {
        j = rand() % len; 
        temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

void BubbleSort(int *list, int n){
    int temp;
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n - i - 1; j++){
            if (list[j] > list[j + 1]){
                temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
                graphSort(list, n);
            }
        }
    }
}

void SelectionSort(int *list, int n){
    int temp, min;
    for (int i = 0; i < n; i++){
        min = i;
        for (int j = i + 1; j < n; j++){
            if (list[j] < list[min]){
                min = j;
            }
        }
        temp = list[i];
        list[i] = list[min];
        list[min] = temp;

        graphSort(list, n);
    }
}

void InsertionSort(int *list, int n){
    int temp, j;
    for (int i = 1; i < n; i++){
        temp = list[i];
        j = i - 1;
        while (j >= 0 && list[j] > temp){
            list[j + 1] = list[j];
            j--;
        }
        list[j + 1] = temp;
        graphSort(list,n);
    }
}

void Merge(int *list, int l, int m, int r){
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    int L[n1], R[n2];

    for (i = 0; i < n1; i++){
        L[i] = list[l + i];
    }
    for (j = 0; j < n2; j++){
        R[j] = list[m + 1 + j];
    }

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2){
        if (L[i] <= R[j]){
            list[k] = L[i];
            i++;
        }
        else {
            list[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1){
        list[k] = L[i];
        i++;
        k++;
    }

    while (j < n2){
        list[k] = R[j];
        j++;
        k++;
    }
}

void MergeSort(int *list, int l, int r){
    if (l < r){
        int m = l + (r - l) / 2;
        MergeSort(list, l, m);
        MergeSort(list, m + 1, r);
        Merge(list, l, m, r);
    }
}

void QuickSort(int *list, int l, int r){
    int i = l;
    int j = r;
    int temp;
    int pivot = list[(l + r) / 2];

    while (i <= j){
        while (list[i] < pivot){
            i++;
        }
        while (list[j] > pivot){
            j--;
        }
        if (i <= j){
            temp = list[i];
            list[i] = list[j];
            list[j] = temp;
            i++;
            j--;
        }
    }

    if (l < j){
        QuickSort(list, l, j);
    }
    if (i < r){
        QuickSort(list, i, r);
    }
}

int main(){
    int mode, algoType, n, modeRandom;

    // generate random list of numbers
    printf("Masukkan jumlah data: ");
    scanf("%d", &n);
    int list[n];

    do{
        printf("\nMode random number:\n1. Full random\n2. Urutan random bilangan natural\n\nMode terpilih:");
        scanf("%d", &modeRandom);
        switch(modeRandom){
            case 1:
                randomList(list, n);
                break;
            case 2:
                randomNatural(list, n);
                break;
            default:
                system("cls");
                printf("Pilihan tidak valid!\n");
                break;
        }
    }while(modeRandom != 1 && modeRandom != 2);

    for (int i = 0; i < n; i++){
        printf("%d ", list[i]);
    }
    printf("\n");

    do {
        printf("Pilih mode:\n1. Sorting\n2. Searching\nPilih: ");
        scanf("%d", &mode);
        switch(mode){
            case 1:
                printf("Pilih algoritma:\n1. Bubble Sort\n2. Selection Sort\n3. Insertion Sort\n4. Merge Sort\n5. Quick Sort\nPilih: ");
                scanf("%d", &algoType);
                switch(algoType){
                    case 1:
                        BubbleSort(list, n);
                        break;
                    case 2:
                        SelectionSort(list, n);
                        break;
                    case 3:
                        InsertionSort(list, n);
                        break;
                    case 4:
                        MergeSort(list, 0, n - 1);
                        break;
                    case 5:
                        QuickSort(list, 0, n - 1);
                        break;
                    default:
                        system("cls");
                        printf("Pilihan tidak valid!\n");
                        break;
                }
                break;
            case 2:
                printf("Pilih algoritma:\n1. Linear Search\n2. Binary Search\nPilih: ");
                scanf("%d", &algoType);
                switch(algoType){
                    case 1:
                        break;
                    case 2:
                        break;
                    default:
                        system("cls");
                        printf("Pilihan tidak valid!\n");
                        break;
                }
                break;
            default:
                system("cls");
                printf("Pilihan tidak valid!\n");
                break;
        }
    } while (mode != 1 && mode != 2);

    for(int i = 0; i < n; i++){
        printf("%d ", list[i]);
    }

    return 0;
}