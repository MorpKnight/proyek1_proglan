#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <omp.h>

typedef struct DATA {
    char title[100], singer[100], link[200], genre[100];
    char genres[100][100];
    int year_release, duration, genre_count;
    struct DATA *next;
} SONG;

typedef struct queueNode {
    SONG *front, *rear;
} playList;

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

    while (fgets(temp, sizeof(temp), fp) != NULL) {
        (*totalSong)++;
    }

    rewind(fp);

    fgets(temp, sizeof(temp), fp);  // Skip the first line

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


void askToPlay(SONG *current){
    char command[256], answer;

    do {
        printf("Do you want to play %s - %s? (y/n) ", current->title, current->singer);
        scanf(" %c", &answer);
    } while(answer != 'y' && answer != 'n');

    if(answer == 'y'){
        snprintf(command, sizeof(command), "start %s", current->link);
        system(command);
    }
}

void printSong(SONG *head){
    SONG *current = head;
    int i = 0;
    while(current != NULL){
        printf("%d. %s - %s\n", i+1, current->title, current->singer);
        current = current->next;
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

    mergeSort(&first);
    mergeSort(&tail);

    *head = merge(first, tail);
}

SONG *searchSong(SONG *head){
    char searchKeyword[100];
    SONG *current = head;
    SONG *searchList = NULL;
    SONG *searchCurrent = NULL;
    int i, index;
    i = 0;

    do {
        printf("Search song: ");
        scanf("%s", searchKeyword);
        if(strcmp(searchKeyword, "exit") == 0){
            return NULL;
        }
    } while(strlen(searchKeyword) < 3);

    // search based on title, store in searchList, and check if alreadt exist
    while(current != NULL){
        if(strstr(current->title, searchKeyword) != NULL){
            if(searchList == NULL){
                searchList = (SONG*) malloc(sizeof(SONG));
                strcpy(searchList->title, current->title);
                strcpy(searchList->singer, current->singer);
                strcpy(searchList->link, current->link);
                strcpy(searchList->genre, current->genre);
                searchList->year_release = current->year_release;
                searchList->duration = current->duration;
                searchList->genre_count = current->genre_count;
                searchList->next = NULL;
                searchCurrent = searchList;
            } else {
                searchCurrent->next = (SONG*) malloc(sizeof(SONG));
                strcpy(searchCurrent->next->title, current->title);
                strcpy(searchCurrent->next->singer, current->singer);
                strcpy(searchCurrent->next->link, current->link);
                strcpy(searchCurrent->next->genre, current->genre);
                searchCurrent->next->year_release = current->year_release;
                searchCurrent->next->duration = current->duration;
                searchCurrent->next->genre_count = current->genre_count;
                searchCurrent->next->next = NULL;
                searchCurrent = searchCurrent->next;
            }
        }
        current = current->next;
    }

    // search based on singer, store in searchList, and check if already exist
    current = head;
    while(current != NULL){
        if(strstr(current->singer, searchKeyword) != NULL){
            if(searchList == NULL){
                searchList = (SONG*) malloc(sizeof(SONG));
                strcpy(searchList->title, current->title);
                strcpy(searchList->singer, current->singer);
                strcpy(searchList->link, current->link);
                strcpy(searchList->genre, current->genre);
                searchList->year_release = current->year_release;
                searchList->duration = current->duration;
                searchList->genre_count = current->genre_count;
                searchList->next = NULL;
                searchCurrent = searchList;
            } else {
                searchCurrent->next = (SONG*) malloc(sizeof(SONG));
                strcpy(searchCurrent->next->title, current->title);
                strcpy(searchCurrent->next->singer, current->singer);
                strcpy(searchCurrent->next->link, current->link);
                strcpy(searchCurrent->next->genre, current->genre);
                searchCurrent->next->year_release = current->year_release;
                searchCurrent->next->duration = current->duration;
                searchCurrent->next->genre_count = current->genre_count;
                searchCurrent->next->next = NULL;
                searchCurrent = searchCurrent->next;
            }
        }
        current = current->next;
    }

    // search based on genre, store in searchList, and check if already exist
    current = head;
    while(current != NULL){
        if(strstr(current->genre, searchKeyword) != NULL){
            if(searchList == NULL){
                searchList = (SONG*) malloc(sizeof(SONG));
                strcpy(searchList->title, current->title);
                strcpy(searchList->singer, current->singer);
                strcpy(searchList->link, current->link);
                strcpy(searchList->genre, current->genre);
                searchList->year_release = current->year_release;
                searchList->duration = current->duration;
                searchList->genre_count = current->genre_count;
                searchList->next = NULL;
                searchCurrent = searchList;
            } else {
                searchCurrent->next = (SONG*) malloc(sizeof(SONG));
                strcpy(searchCurrent->next->title, current->title);
                strcpy(searchCurrent->next->singer, current->singer);
                strcpy(searchCurrent->next->link, current->link);
                strcpy(searchCurrent->next->genre, current->genre);
                searchCurrent->next->year_release = current->year_release;
                searchCurrent->next->duration = current->duration;
                searchCurrent->next->genre_count = current->genre_count;
                searchCurrent->next->next = NULL;
                searchCurrent = searchCurrent->next;
            }
        }
        current = current->next;
    }

    // print searchList
    if(searchList == NULL){
        printf("Song not found!\n");
    } else {
        printf("Search result:\n");
        printSong(searchList);
    }

    do {
        printf("Lagu mana yang dimaksud? (masukkan nomor): ");
        scanf("%d", &index);
    } while(index < 1 || index > i);

    current = searchList;

    for(i = 1; i < index; i++){
        current = current->next;
    }
    askToPlay(current);

    return current;
}

void printToFile(SONG *head){
    FILE *fp = fopen("song.txt", "w");
    SONG *current = head;

    fprintf(fp, "SONG_NAME,SONG_SINGER,DURATION,SONG_YEAR_RELEASE,SONG_LINK,SONG_GENRES\n");
    while(current != NULL){
        fprintf(fp, "%s,%s,%d,%d,%s,%s\n", current->title, current->singer, current->duration, current->year_release, current->link, current->genre);
        current = current->next;
    }

    fclose(fp);
}

void showSong(SONG *head){
    char sort;
    
    system("cls");
    printSong(head);
    printf("\n");

    do {
        printf("Apakah ingin di sort? (y/n): ");
        scanf(" %c", &sort);
    } while(sort != 'y' && sort != 'n');

    if(sort == 'y'){
        system("cls");
        mergeSort(&head);
        printSong(head);
        printf("\n");
    }

    do {
        printf("Apakah ingin di print ke file? (y/n): ");
        scanf(" %c", &sort);
    } while(sort != 'y' && sort != 'n');

    if(sort == 'y'){
        printToFile(head);
        printf("Berhasil di print ke file\n");
    }

    printf("Press enter to continue...");
    getchar();
    getchar();
    system("cls");
}

void addSong(SONG *head){
    SONG *temp = (SONG*) malloc(sizeof(SONG));
    SONG *current = head;
    char printFile;

    system("cls");
    printf("Menu Tambah Lagu:\n");
    printf("Masukkan judul lagu: ");
    scanf(" %[^\n]", temp->title);
    printf("Masukkan nama penyanyi: ");
    scanf(" %[^\n]", temp->singer);
    printf("Masukkan durasi lagu (dalam detik): ");
    scanf("%d", &temp->duration);
    printf("Masukkan tahun rilis lagu: ");
    scanf("%d", &temp->year_release);
    printf("Masukkan link lagu: ");
    scanf(" %[^\n]", temp->link);
    printf("Masukkan genre lagu: ");
    scanf(" %[^\n]", temp->genre);

    while(current->next != NULL){
        current = current->next;
    }
    current->next = temp;
    temp->next = NULL;

    printf("Lagu berhasil ditambahkan!\n");

    do {
        printf("Apakah ingin di print ke file? (y/n): ");
        scanf(" %c", &printFile);
    } while(printFile != 'y' && printFile != 'n');

    if(printFile == 'y'){
        printToFile(head);
        printf("Berhasil di print ke file\n");
    }

    printf("Press enter to continue...");
    getchar();
    getchar();
    system("cls");
}

void modifySong(SONG *head){
    int workMode;

    do {
        system("cls");
        printf("Menu Tambah/Hapus Lagu:\n");
        printf("1. Tambah lagu\n");
        printf("2. Hapus lagu\n");
        printf("3. Kembali ke menu utama\n");

        do {
            printf("Pilih menu: ");
            scanf("%d", &workMode);

            switch(workMode){
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                    printf("Kembali ke menu utama...\n");
                    break;
                default:
                    printf("Invalid input, harap ulang!\n");
                    break;
            }
        } while(workMode != 1 && workMode != 2 && workMode != 3);
    } while(workMode != 3);
}

int main(){
    SONG *head, *searchList;
    int totalSong, workMode;

    head = readSong(&totalSong);
    splitGenre(head, totalSong);
    searchList = NULL;

    do {
        printf("Selamat datang di Music Manager:\n");
        printf("1. Tampilkan semua lagu\n");
        printf("2. Cari lagu\n");
        printf("3. Tambah/Hapus lagu\n");
        printf("4. Keluar\n");

        do {
            printf("Pilih menu: ");
            scanf("%d", &workMode);

            switch(workMode) {
                case 1:
                    showSong(head);
                    break;
                case 2:
                    searchList = searchSong(head);
                    break;
                case 3:
                    modifySong(head);
                    break;
                case 4:
                    system("cls");
                    printf("Terima kasih telah menggunakan Music Manager\n");
                    break;
                default:
                    printf("Input tidak valid, harap ulangi!\n");
                    break;
            }
         } while(workMode != 4);
    } while(workMode != 4);

    return 0;
}