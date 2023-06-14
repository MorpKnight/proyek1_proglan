#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <omp.h>
#include <time.h>

typedef struct DATA {
    char title[100], singer[100], link[200], genre[100];
    char genres[100][100];
    int year_release, duration, genre_count;
    struct DATA *next;
} SONG;

typedef struct queueNode {
    SONG *front, *rear;
} playList;

int getValidInteger(){
    int input;
    char buff[256], c;

    while (1) {
        if (fgets(buff, sizeof(buff), stdin) != NULL) {
            if (sscanf(buff, " %d", &input) == 1) {
                return input;
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

void splitGenre(SONG *head, int totalSong){
    SONG *current = head;
    int i, j;

    while(current != NULL){
        char *token = strtok(current->genre, ",");
        int j = 0;
        current->genre_count=0;
        while(token != NULL){
            strcpy(current->genres[j], token);
            token = strtok(NULL, ",");
            current->genre_count++;
            j++;
        }
        current = current->next;
    }
}

void enqueue(playList *queue, SONG *head){
    SONG *current = head;
    SONG *temp = (SONG*) malloc(sizeof(SONG));

    temp = current;
    temp->next = NULL;

    if(queue->front == NULL && queue->rear == NULL){
        queue->front = temp;
        queue->rear = temp;
    } else {
        queue->rear->next = temp;
        queue->rear = temp;
    }
}

void dequeue(playList *queue){
    SONG *temp = queue->front;

    if(queue->front == NULL){
        printf("Queue is empty!\n");
    } else if(queue->front == queue->rear){
        queue->front = NULL;
        queue->rear = NULL;
        free(temp);
    } else {
        queue->front = queue->front->next;
        free(temp);
    }
}

int printQueue(playList *queue){
    SONG *current = queue->front;
    int i = 1;

    if(queue->front == NULL){
        printf("Queue is empty!\n");
        return 0;
    } else {
        while(current != NULL){
            printf("%d. %s\n", i, current->title);
            current = current->next;
            i++;
        }
    }

    return 1;
}

SONG* readSong(int* totalSong) {
    SONG* head = NULL;
    SONG* current = NULL;
    FILE* fp = fopen("song.txt", "r");
    char temp[1000];

    if (fp == NULL) {
        printf("File not found!\n");
        return NULL;
    }

    *totalSong = 0;
    fgets(temp, sizeof(temp), fp);
    while (fgets(temp, sizeof(temp), fp) != NULL) {
        (*totalSong)++;
    }

    rewind(fp);

    fgets(temp, sizeof(temp), fp);

    SONG* songs = (SONG*)malloc(sizeof(SONG) * (*totalSong));
    if (songs == NULL) {
        printf("Memory allocation failed!\n");
        fclose(fp);
        return NULL;
    }

    int i = 0;

    while (fgets(temp, sizeof(temp), fp) != NULL) {
        SONG* new = &songs[i];  // Get a pointer to the current song

        sscanf(temp, "%[^,],%[^,],%d,%d,%[^,],%[^\n]",
               new->title, new->singer, &new->duration,
               &new->year_release, new->link, new->genre);

        new->next = NULL;

        if (head == NULL) {
            head = new;
            current = head;
        } else {
            current->next = new;
            current = current->next;
        }

        i++;
    }

    fclose(fp);
    return head;
}

void playSong(SONG *current){
    char command[256];

    printf("Now playing: %s\n", current->title);
    snprintf(command, sizeof(command), "start %s", current->link);
    system(command);
    printf("Press enter to stop playing\n");
    getchar();
    getchar();
}


void askToPlay(playList *queue, SONG *current){
    char command[256];
    int answer;

    do {
        printf("Opsi:\n");
        printf("1. Mainkan lagu sekarang\n");
        printf("2. Tambahkan ke playlist\n");
        printf("3. Kembali ke menu\n");
        printf("Masukkan pilihan: ");
        scanf("%d", &answer);
    } while(answer < 1 || answer > 3);

    switch(answer){
        case 1:
            playSong(current);
            break;
        case 2:
            enqueue(queue, current);
            printf("\nLagu berhasil ditambahkan ke playlist\n");
            break;
        case 3:
            system("cls");
            return;
            break;
        default:
            printf("Masukkan pilihan yang benar!\n");
            break;
    }

    printf("\nTekan enter untuk kembali");
    getchar();
    getchar();
    system("cls");
}

void printSong(SONG *head){
    SONG *current = head;
    int i = 0;
    while(current != NULL){
        printf("%d. %s - %s\n", i+1, current->title, current->singer);
        current = current->next;
        i++;
    }
}

SONG *merge(SONG *first, SONG *tail){
    if(first == NULL){
        return tail;
    }

    if(tail == NULL){
        return first;
    }

    SONG *result = NULL;

    if(strcmp(first->title, tail->title) <= 0){
        result = first;
        result->next = merge(first->next, tail);
    } else {
        result = tail;
        result->next = merge(first, tail->next);
    }

    return result;
}

void split(SONG *head, SONG **first, SONG **tail){
    SONG *fast = head;
    SONG *slow = head;

    while(fast->next != NULL && fast->next->next != NULL){
        fast = fast->next->next;
        slow = slow->next;
    }

    *first = head;
    *tail = slow->next;
    slow->next = NULL;
}

void mergeSort(SONG **head){
    SONG *first = NULL;
    SONG *tail = NULL;

    if((*head == NULL) || ((*head)->next == NULL)){
        return;
    }

    split(*head, &first, &tail);

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            mergeSort(&first);
        }

        #pragma omp section
        {
            mergeSort(&tail);
        }
    }

    // search for keyword in genre
    for(i = 0; i < count; i++){
        for(j = 0; j < data[i].genre_count; j++){
            if(strstr(data[i].genres[j], keyword) != NULL){
                int isAlreadyInList = 0;
                for(j = 0; j < count; j++){
                    if(strcmp(data[i].title, search[j].title) == 0){
                        isAlreadyInList = 1;
                        break;
                    }
                }
                if(isAlreadyInList == 0){
                    search[k] = data[i];
                    k++;
                }
            }
        }
        current = current->next;
    }

    // print searchList
    if(searchList == NULL){
        printf("Song not found!\n\n");
        return NULL;
    } else {
        printf("\nSearch result:\n");
        printSong(searchList);
    }

    if(totalSong == 1){
        printf("Lagu yang dimaksud adalah: %s - %s\n", searchList->title, searchList->singer);
        askToPlay(queue, searchList);
        return searchList;
    } else {
        printf("0. Kembali ke menu\n");
        printf("Lagu mana yang dimaksud? (masukkan nomor): ");
        scanf("%d", &index);
    }

    if(index == 0){
        system("cls");
        return NULL;
    }

    current = searchList;

    for(i = 1; i < index; i++){
        current = current->next;
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
        } while(index < 0 || index >= count);

        index--;
        playSong(data[index].link);
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

    printf("Press enter to continue...");
    getchar();
    getchar();
    system("cls");
}

void menu3(){
    int workMode2;
    do {
        system("cls");
        printf("Menu Tambah/Hapus Lagu:\n");
        printf("1. Tambah lagu\n");
        printf("2. Hapus lagu\n");
        printf("3. Kembali ke menu utama\n");
        printf("Masukkan pilihan anda: ");
        scanf("%d", &workMode2);
        switch(workMode2){
            case 1:
                playSong(temp);
                dequeue(queue);
                break;
            case 2:
                temp = temp->next;
                playSong(temp);
                dequeue(queue);
                break;
            case 3:
                temp = queue->rear;
                playSong(temp);
                dequeue(queue);
                break;
            case 4:
                shuffleQueue(queue);
                printf("Queue berhasil di shuffle!\n\n");
                break;
            case 5:
                freeQueue(queue);
                printf("Queue berhasil di clear!\n\n");
                break;
            case 6:
                printf("Queue:\n");
                printQueue(queue);
                break;
            case 7:
                printf("Kembali ke menu utama...\n\n");
                break;
            default:
                system("cls");
                printf("Terima kasih telah menggunakan Music Manager\n");
                break;
            default:
                printf("Input tidak valid, harap ulangi!\n");
                break;
        }
    } while(workMode != 5);

    return 0;
}