#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct DATA {
    char title[100], singer[100], link[200], genre[100];
    char genres[100][100];
    int year_release, duration, genre_count;
} SONG;

void splitGenreToGenres(SONG *data, int count){
    int i, j;
    for(i = 0; i < count; i++){
        char *token = strtok(data[i].genre, ",");
        int j = 0;
        data[i].genre_count=0;
        while(token != NULL){
            strcpy(data[i].genres[j], token);
            token = strtok(NULL, ",");
            data[i].genre_count++;
            j++;
        }
    }
}

void testPrint(SONG *data, int count){
    int i, j;
    for(i = 0; i < count; i++){
        printf("%s - %s (%d)\n", data[i].title, data[i].singer, data[i].year_release);
        printf("Genre: ");
        j = 0;
        while(strcmp(data[i].genres[j], "") != 0){
            printf("%s", data[i].genres[j]);
            if(strcmp(data[i].genres[j+1], "") != 0){
                printf(", ");
            }
            j++;
        }
        printf("\n");
        printf("Link: %s\n", data[i].link);
        printf("\n");
    }
}

void merge(SONG *arr, int left, int mid, int right) {
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    SONG L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    i = 0;
    j = 0;
    k = left;
    while (i < n1 && j < n2) {
        if (strcmp(L[i].title, R[j].title) <= 0) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(SONG *arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

void sortList(SONG *data, int count) {
    mergeSort(data, 0, count - 1);
}

int main(){
    SONG *data, *search;
    FILE *fp;
    char temp[1000];
    int totalSong, i, j, workMode;

    fp = fopen("song.txt", "r");

    if(fp == NULL){
        printf("File not found!\n");
        exit(1);
    }

    totalSong = 0;

    fgets(temp, 1000, fp);
    while(fgets(temp, 1000, fp) != NULL){
        totalSong++;
    }

    data = (SONG*) calloc(totalSong, sizeof(SONG));
    search = (SONG*) calloc(totalSong, sizeof(SONG));

    i = 0;
    rewind(fp);

    fgets(temp, 1000, fp);
    while(fgets(temp, 1000, fp) != NULL){
        sscanf(temp, "%[^,],%[^,],%d,%d,%[^,],%[^\n]", data[i].title, data[i].singer, &data[i].duration, &data[i].year_release, data[i].link, data[i].genre);
        i++;
    }
    fclose(fp);

    splitGenreToGenres(data, totalSong);

    do {
        printf("Selamat datang di Music Manager\n");
        printf("================================\n\n\n");
        printf("Berikut merupakan menu dari program ini:\n");

        printf("1. Tampilkan semua lagu\n");
        printf("2. Cari lagi berdasarkan keyword\n");
        printf("3. Keluar dari program\n\n");

        do {
            printf("Masukkan pilihan anda: ");
            scanf("%d", &workMode);
            switch(workMode){
                case 1:
                    system("cls");
                    testPrint(data, totalSong);

                    printf("Apakah mau di sort? (y/n): ");
                    char sort;
                    scanf(" %c", &sort);
                    if(tolower(sort) == 'y'){
                        system("cls");
                        sortList(data, totalSong);
                        testPrint(data, totalSong);
                    }
                    printf("Tekan enter untuk kembali ke menu utama...");
                    getchar();
                    getchar();
                    system("cls");
                    break;
                case 2:
                    break;
                case 3:
                    system("cls");
                    printf("Terima kasih telah menggunakan program ini!\n");
                    break;
                default:
                    system("cls");
                    printf("Pilihan tidak tersedia!\n");
                    break;
            }
        } while(workMode != 1 && workMode != 2 && workMode != 3);
    } while(workMode != 3);

    free(data);
    free(search);
    return 0;
}