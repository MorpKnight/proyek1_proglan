#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <omp.h>

typedef struct DATA {
    char title[100], singer[100], link[200], genre[100];
    char genres[100][100];
    int year_release, duration, genre_count;
} SONG;

void splitGenreToGenres(SONG *data, int count);
void testPrint(SONG *data, int count);
void merge(SONG *arr, int left, int mid, int right);
void mergeSort(SONG *arr, int left, int right);
void sortList(SONG *data, int count);
void searchRelated(SONG *data, SONG *search, int count);
void playSong(char *link);
void askToPlay(SONG *data, int count);
void menu1(SONG *data, int count);
void menu3();

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
        printf("3. Tambah/Hapus lagu\n");
        printf("4. Keluar dari program\n\n");

        do {
            printf("Masukkan pilihan anda: ");
            scanf("%d", &workMode);
            switch(workMode){
                case 1:
                    menu1(data, totalSong);
                    break;
                case 2:
                    system("cls");
                    searchRelated(data, search, totalSong);
                    printf("Tekan enter untuk kembali ke menu utama...");
                    getchar();
                    getchar();
                    break;
                case 3:
                    menu3();
                    break;
                case 4:
                    system("cls");
                    printf("Terima kasih telah menggunakan program ini!\n");
                    break;
                default:
                    system("cls");
                    printf("Pilihan tidak tersedia!\n");
                    break;
            }
        } while(workMode < 1 || workMode > 4);
    } while(workMode != 4);

    free(data);
    free(search);
    return 0;
}

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
        printf("%d. %s - %s (%d)\n", i+1, data[i].title, data[i].singer, data[i].year_release);
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

void searchRelated(SONG *data, SONG *search, int count) {
    char keyword[100];
    int i, j, k, isAlreadyInList;
    k = 0;
    do {
        printf("Keyword: ");
        scanf(" %[^\n]", keyword);
    } while(strcmpi(keyword, "") == 0);

    // search for keyword in title
    #pragma omp parallel for private(i, j) shared(data, search)
    for(i = 0; i < count; i++) {
        if(strstr(data[i].title, keyword) != NULL) {
            int isAlreadyInList = 0;
            for(j = 0; j < count; j++) {
                if(strcmpi(data[i].title, search[j].title) == 0) {
                    isAlreadyInList = 1;
                    break;
                }
            }
            if(isAlreadyInList == 0) {
                #pragma omp critical
                {
                    search[k] = data[i];
                    k++;
                }
            }
        }
    }

    // search for keyword in singer
    #pragma omp parallel for private(i, j) shared(data, search)
    for(i = 0; i < count; i++) {
        if(strstr(data[i].singer, keyword) != NULL) {
            int isAlreadyInList = 0;
            for(j = 0; j < count; j++) {
                if(strcmpi(data[i].title, search[j].title) == 0) {
                    isAlreadyInList = 1;
                    break;
                }
            }
            if(isAlreadyInList == 0) {
                #pragma omp critical
                {
                    search[k] = data[i];
                    k++;
                }
            }
        }
    }

    // search for keyword in genre
    #pragma omp parallel for private(i, j) shared(data, search)
    for(i = 0; i < count; i++) {
        for(j = 0; j < data[i].genre_count; j++) {
            if(strstr(data[i].genres[j], keyword) != NULL) {
                int isAlreadyInList = 0;
                for(j = 0; j < count; j++) {
                    if(strcmpi(data[i].title, search[j].title) == 0) {
                        isAlreadyInList = 1;
                        break;
                    }
                }
                if(isAlreadyInList == 0) {
                    #pragma omp critical
                    {
                        search[k] = data[i];
                        k++;
                    }
                }
            }
        }
    }

    testPrint(search, k);
}

void playSong(char *link){
    char command[100];
    strcpy(command, "start ");
    strcat(command, link);
    system(command);
}

void askToPlay(SONG *data, int count){
    int index;
    char play;

    do {
        printf("Apakah ingin memutar lagu? (y/n) ");
        scanf(" %c", &play);
    } while(play != 'y' && play != 'n');

    if(play == 'y'){
        do {
            printf("Index: ");
            scanf("%d", &index);
        } while(index <= 0 || index > count);
        playSong(data[index+1].link);
        printf("Memutar %s - %s (%d)\n", data[index].title, data[index].singer, data[index].year_release);
    }
}

void menu1(SONG *data, int totalSong){
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
    askToPlay(data, totalSong);
    printf("Tekan enter untuk kembali ke menu utama...");
    getchar();
    getchar();
    system("cls");
}

void menu3(){
    int workMode2;
    do {
        system("cls");
        printf("Menu tambah/hapus lagu\n");
        printf("1. Tambah lagu\n");
        printf("2. Hapus lagu\n");
        printf("3. Kembali ke menu utama\n");
        printf("Masukkan pilihan anda: ");
        scanf("%d", &workMode2);
        switch(workMode2){
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            default:
                system("cls");
                printf("Pilihan tidak tersedia!\n");
                break;
        }
    } while(workMode2 < 1 || workMode2 > 3);
}