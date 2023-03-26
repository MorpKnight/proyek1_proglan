#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>

int main(){
    int mode, algoType;

    do {
        printf("Pilih mode:\n1. Sorting\n2. Searching\nPilih: ");
        scanf("%d", &mode);
        switch(mode){
            case 1:
                printf("Pilih algoritma:\n1. Bubble Sort\n2. Selection Sort\n3. Insertion Sort\n4. Merge Sort\n5. Quick Sort\nPilih: ");
                scanf("%d", &algoType);
                switch(algoType){
                    case 1:
                        break;
                    case 2:
                        break;
                    case 3:
                        break;
                    case 4:
                        break;
                    case 5:
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

    return 0;
}