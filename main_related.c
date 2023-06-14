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

        } else {
            printf("Error membaca input, silahkan masukkan ulang: ");
            clearerr(stdin);
        }
    }
}

int getNumberOfThreads() {
    int numberOfThreads = 0;
    #pragma omp parallel
    {
        #pragma omp single
        {
            numberOfThreads = omp_get_num_threads();
        }
    }

    return numberOfThreads;
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

    *head = merge(first, tail);
}

SONG *searchSong(playList *queue, SONG *head, SONG **jumpSpot, int numberOfThreads){
    char searchKeyword[100];
    SONG *current = NULL;
    SONG *searchList = NULL;
    SONG *searchCurrent = NULL;
    int i, index, totalSong;
    i = 0;
    totalSong = 0;

    do {
        printf("Search song: ");
        scanf("%[^\n]s", searchKeyword);
        if(strcmp(searchKeyword, "exit") == 0){
            return NULL;
        }
    } while(strlen(searchKeyword) < 3);

#pragma omp parallel private(i, current)
{
    #pragma omp for reduction(+:totalSong)
    for (i = 0; i < numberOfThreads; i++) {
        current = jumpSpot[i];
        
        while (current != jumpSpot[i+1]) {
            if (strstr(current->title, searchKeyword) != NULL ||
                strstr(current->singer, searchKeyword) != NULL ||
                strstr(current->genre, searchKeyword) != NULL) {
                #pragma omp critical
                {
                    if (searchList == NULL) {
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
                    totalSong++;
                }
            }
            current = current->next;
        }
    }
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

    askToPlay(queue, current);

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
    } else {
        return;
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

void removeSong(SONG *head, SONG **jumpSpot){
    SONG *searchCurrent = head;
    SONG *searchResult = NULL;
    char title[100], printFile;
    int totalSong,i;
    int numberOfThreads = getNumberOfThreads();

    totalSong = 0;

    system("cls");
    printf("Menu Hapus Lagu:\n");
    printf("Masukkan judul lagu: ");
    scanf(" %[^\n]", title);

    #pragma omp parallel private(i, searchCurrent)
    {
        #pragma omp for reduction(+:totalSong)
        for (i = 0; i < numberOfThreads; i++) {
            searchCurrent = jumpSpot[i];
            
            while (searchCurrent != jumpSpot[i+1]) {
                if (strstr(searchCurrent->title, title) != NULL ||
                    strstr(searchCurrent->singer, title) != NULL ||
                    strstr(searchCurrent->genre, title) != NULL) {
                    #pragma omp critical
                    {
                        if (searchResult == NULL) {
                            searchResult = (SONG*) malloc(sizeof(SONG));
                            strcpy(searchResult->title, searchCurrent->title);
                            strcpy(searchResult->singer, searchCurrent->singer);
                            strcpy(searchResult->link, searchCurrent->link);
                            strcpy(searchResult->genre, searchCurrent->genre);
                            searchResult->year_release = searchCurrent->year_release;
                            searchResult->duration = searchCurrent->duration;
                            searchResult->genre_count = searchCurrent->genre_count;
                            searchResult->next = NULL;
                            searchCurrent = searchResult;
                        } else {
                            searchCurrent->next = (SONG*) malloc(sizeof(SONG));
                            strcpy(searchCurrent->next->title, searchCurrent->title);
                            strcpy(searchCurrent->next->singer, searchCurrent->singer);
                            strcpy(searchCurrent->next->link, searchCurrent->link);
                            strcpy(searchCurrent->next->genre, searchCurrent->genre);
                            searchCurrent->next->year_release = searchCurrent->year_release;
                            searchCurrent->next->duration = searchCurrent->duration;
                            searchCurrent->next->genre_count = searchCurrent->genre_count;
                            searchCurrent->next->next = NULL;
                            searchCurrent = searchCurrent->next;
                        }
                        totalSong++;
                    }
                }
                searchCurrent = searchCurrent->next;
            }
        }
    }
    
    if(searchCurrent == NULL && totalSong == 0){
        printf("Lagu tidak ditemukan!\n");
        printf("Press enter to continue...");
        getchar();
        getchar();
        system("cls");
        return;
    }


    if(totalSong > 1){
        int i = 1;
        SONG *searchCurrent = searchResult;
        printf("Lagu yang ditemukan:\n");
        while(searchCurrent != NULL){
            printf("%d. %s\n", i, searchCurrent->title);
            searchCurrent = searchCurrent->next;
            i++;
        }

        int songNumber;
        do {
            printf("Pilih lagu yang ingin dihapus: ");
            songNumber = getValidInteger();
        } while(songNumber < 1 || songNumber > totalSong);

        searchCurrent = searchResult;
        for(int i = 1; i < songNumber; i++){
            searchCurrent = searchCurrent->next;
        }

        searchCurrent = head;
        while(searchCurrent->next != NULL){
            if(strcmp(searchCurrent->next->title, searchCurrent->title) == 0){
                SONG *temp = searchCurrent->next;
                searchCurrent->next = searchCurrent->next->next;
                free(temp);
                break;
            }
            searchCurrent = searchCurrent->next;
        }
    } else {
        searchCurrent = head;
        while(searchCurrent->next != NULL){
            if(strcmp(searchCurrent->next->title, searchResult->title) == 0){
                SONG *temp = searchCurrent->next;
                searchCurrent->next = searchCurrent->next->next;
                free(temp);
                break;
            }
            searchCurrent = searchCurrent->next;
        }
    }
}

void modifySong(SONG *head, SONG **jumpSpot){
    int workMode;

    do {
        system("cls");
        printf("Menu Tambah/Hapus Lagu:\n");
        printf("1. Tambah lagu\n");
        printf("2. Hapus lagu\n");
        printf("3. Kembali ke menu utama\n");

        do {
            printf("Pilih menu: ");
            workMode = getValidInteger();

            switch(workMode){
                case 1:
                    break;
                case 2:
                    removeSong(head, jumpSpot);
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

void freeQueue(playList *queue){
    while(queue->front != NULL){
        dequeue(queue);
    }
}

void shuffleQueue(playList *queue){
    int size = 0;
    SONG *current = queue->front;

    while(current != NULL){
        size++;
        current = current->next;
    }

    SONG *temp = (SONG*) malloc(sizeof(SONG) * size);
    if(temp == NULL){
        printf("Memory allocation failed!\n");
        exit(1);
    }

    current = queue->front;
    for(int i = 0; i < size; i++){
        temp[i] = *current;
        current = current->next;
    }

    srand(time(NULL));
    for(int i = 0; i < size; i++){
        int random = rand() % (i + 1);
        SONG tempSong = temp[i];
        temp[i] = temp[random];
        temp[random] = tempSong;
    }

    current = queue->front;
    for(int i = 0; i < size; i++){
        *current = temp[i];
        current = current->next;
    }

    free(temp);
}

void queueMenu(playList *queue){
    int option, queueCallback;
    SONG *temp = queue->front;
    printf("Menu Queue:\n");
    queueCallback = printQueue(queue);

    if(queueCallback == 0){
        printf("Tidak dapat meneruskan ke menu selanjutnya karena queue kosong!\n");
        printf("Press enter to continue...");
        getchar();
        getchar();
        system("cls");
        return;
    }

    do {
        printf("\nApa yang ingin dilakukan?\n");
        printf("1. Play\n");
        printf("2. Next\n");
        printf("3. Prev\n");
        printf("4. Shuffle\n");
        printf("5. Clear\n");
        printf("6. Print queue\n");
        printf("7. Kembali ke menu utama\n");
        printf("Pilih menu: ");
        option = getValidInteger();

        switch(option){
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
                printf("Invalid input, harap ulang!\n");
                break;
        }
    } while(option != 7);
}


void jumpSpotMaker(int numberOfThreads, SONG **jumpSpot, int totalSong, SONG *head){
    int i, j = 0, nextSpot = totalSong / numberOfThreads;
    SONG *current = head;
    jumpSpot[0] = head;

    for(i = 1; i < totalSong; i++) {
        if(i == nextSpot){
            nextSpot += totalSong / numberOfThreads;
            j++;
            jumpSpot[j] = current;
        }
        current = current->next;
    }
}

int main(){
    int totalSong, workMode, numberOfThreads = getNumberOfThreads();
    SONG *head, *searchList, **jumpSpot = (SONG**)malloc(sizeof(SONG*)*numberOfThreads);
    playList *queue = (playList*) malloc(sizeof(playList));
    
    queue->front = NULL;
    queue->rear = NULL; 
    head = readSong(&totalSong);
    splitGenre(head, totalSong);
    jumpSpotMaker(numberOfThreads,jumpSpot, totalSong,head);
    searchList = NULL;

    do {
        printf("Selamat datang di Music Manager:\n");
        printf("1. Tampilkan semua lagu\n");
        printf("2. Cari lagu\n");
        printf("3. Tambah/Hapus lagu\n");
        printf("4. Lihat playlist/queue\n");
        printf("5. Keluar\n");

        printf("Pilih menu: ");
        workMode = getValidInteger();

        switch(workMode) {
            case 1:
                showSong(head);
                break;
            case 2:
                system("cls");
                searchList = searchSong(queue, head, jumpSpot, numberOfThreads);
                break;
            case 3:
                modifySong(head, jumpSpot);
                break;
            case 4:
                system("cls");
                queueMenu(queue);
                break;
            case 5:
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