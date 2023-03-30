#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

typedef struct DATA {
    char title[100], singer[100], link[200], genre[100];
    char genres[100][100];
    int year_release;
} SONG;

void splitGenreToGenres(SONG *data, int count){
    int i, j;
    for(i = 0; i < count; i++){
        char *token = strtok(data[i].genre, ",");
        int j = 0;
        while(token != NULL){
            strcpy(data[i].genres[j], token);
            token = strtok(NULL, ",");
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

void printTitleSingeronly(SONG *data, int count){
    int i;

    printf("Daftar lagu:\n");
    for(i = 0; i < count; i++){
        printf("%d. %s - %s\n", i+1, data[i].title, data[i].singer);
    }
}

void searchSongbyGenre(SONG *data, SONG *search, int count){
    char genre[100];
    int i, j, found, index;

    printf("Masukkan genre: ");
    scanf("%s", genre);

    // tolower genre
    for(i = 0; i < strlen(genre); i++){
        genre[i] = tolower(genre[i]);
    }

    found = 0;
    index = 0;
    for(i = 0; i < count; i++){
        j = 0;
        while(strcmp(data[i].genres[j], "") != 0){
            // tolower genres
            for(int k = 0; k < strlen(data[i].genres[j]); k++){
                data[i].genres[j][k] = tolower(data[i].genres[j][k]);
            }
            if(strcmp(data[i].genres[j], genre) == 0){
                printf("%d. %s - %s\n", index+1, data[i].title, data[i].singer);
                index++;
                found = 1;

                // copy data to search
                strcpy(search[index-1].title, data[i].title);
                strcpy(search[index-1].singer, data[i].singer);
                strcpy(search[index-1].link, data[i].link);

                break;
            }
            j++;
        }
    }

    if(found == 0){
        printf("Tidak ada lagu dengan genre %s\n", genre);
    }
}

void playSong(char *link){
    char command[256];
    snprintf(command, sizeof(command), "start %s", link);
    system(command);
}

void sortList(SONG *data, int count){
    int i, j;
    SONG temp;
    for(i = 0; i < count; i++){
        for(j = i+1; j < count; j++){
            if(strcmp(data[i].title, data[j].title) > 0){
                temp = data[i];
                data[i] = data[j];
                data[j] = temp;
            }
        }
    }
}

void askToPlayByIndex(SONG *data, int count) {
    int index;
    do {
        printf("Masukkan nomor lagu (0 untuk kembali ke menu utama): ");
        scanf("%d", &index);

        if (index == 0) {
            printf("Kembali ke menu utama...\n");
            break;
        } else if (index > 0 && index <= count) {
            printf("Apakah anda ingin memutar lagu %s (y/n): ", data[index - 1].title);
            char play;
            do {
                scanf(" %c", &play);
                if (play != 'y' && play != 'n') {
                    printf("Masukkan y atau n: ");
                }
            } while (play != 'y' && play != 'n');

            if (play == 'y') {
                playSong(data[index - 1].link);
            }
            printf("\n");
            printf("Selamat mendengarkan!\n");
        } else {
            printf("Tidak ada lagu dengan nomor %d\n", index);
        }
    } while (1);
}

void searchByYear(SONG *data, SONG *search, int count) {
    int year, i, index, found;

    do {
        printf("Masukkan tahun: ");
        scanf("%d", &year);
        if (year < 0) {
            printf("Tahun tidak boleh negatif\n");
        }
    } while (year < 0);

    found = 0;
    index = 0;
    do {
        for (i = 0; i < count; i++) {
            if (data[i].year_release == year) {
                printf("%d. %s - %s\n", index + 1, data[i].title, data[i].singer);
                index++;
                found = 1;

                // copy data to search
                strcpy(search[index - 1].title, data[i].title);
                strcpy(search[index - 1].singer, data[i].singer);
                strcpy(search[index - 1].link, data[i].link);
            }
        }

        if (found == 0) {
            printf("Tidak ada lagu yang dirilis pada tahun %d\n", year);
            do {
                printf("Masukkan tahun: ");
                scanf("%d", &year);
                if (year < 0) {
                    printf("Tahun tidak boleh negatif\n");
                }
            } while (year < 0);
        }
    } while (found == 0);
}

void searchSongBySinger(SONG *data, SONG *search, int count) {
    char singer[100];
    int i, j, found, index;

    printf("Masukkan nama penyanyi: ");
    scanf("%s", singer);

    // tolower singer
    for (i = 0; i < strlen(singer); i++) {
        singer[i] = tolower(singer[i]);
    }

    // find possible singer name if given name is not complete
    for (i = 0; i < count; i++) {
        // tolower singer
        for (j = 0; j < strlen(data[i].singer); j++) {
            data[i].singer[j] = tolower(data[i].singer[j]);
        }

        if (strstr(data[i].singer, singer) != NULL) {
            // copy singer name to singer
            strcpy(singer, data[i].singer);
        }
    }

    found = 0;
    index = 0;
    for (i = 0; i < count; i++) {
        // tolower singer
        for (j = 0; j < strlen(data[i].singer); j++) {
            data[i].singer[j] = tolower(data[i].singer[j]);
        }

        if (strcmp(data[i].singer, singer) == 0) {
            printf("%d. %s - %s\n", index + 1, data[i].title, data[i].singer);
            index++;
            found = 1;

            // copy data to search
            strcpy(search[index - 1].title, data[i].title);
            strcpy(search[index - 1].singer, data[i].singer);
            strcpy(search[index - 1].link, data[i].link);
        }
    }

    if (found == 0) {
        printf("Tidak ada lagu dengan penyanyi %s\n", singer);
    }
}


int main(){
    SONG *data, *search;
    FILE *fp;
    char temp[1000];
    int count, n, i, j, mode;

    fp = fopen("song.txt", "r");
    if(fp == NULL){
        printf("File tidak ditemukan");
        return 0;
    }
    
    count = 0;
    fgets(temp, 1000, fp);
    while(fgets(temp, 1000, fp) != NULL){
        count++;
    }

    rewind(fp);
    data = (SONG*)calloc(count, sizeof(SONG));
    search = (SONG*)calloc(count, sizeof(SONG));
    fgets(temp, 1000, fp);

    n = 0;
    while(fgets(temp, 1000, fp) != NULL){
        // sscanf with this format SONG_NAME,SONG_SINGER,SONG_YEAR_RELEASE,SONG_LINK,SONG_GENRES
        sscanf(temp, "%[^,],%[^,],%d,%[^,],%[^\n]", data[n].title, data[n].singer, &data[n].year_release, data[n].link, data[n].genre);
        n++;
    }

    fclose(fp);

    // split genre to genres that separated with ","
    splitGenreToGenres(data, count);

    menu:
        printf("Selamat datang di Music Player!\n");
        printf("1. Tampilkan semua lagu\n");
        printf("2. Tampilkan lagu berdasarkan genre\n");
        printf("3. Tampilkan lagu berdasarkan tahun rilis\n");
        printf("4. Tampilkan lagu berdasarkan penyanyi\n");
        printf("5. Tampilkan lagu berdasarkan judul\n");
        printf("6. Tambah lagu\n");
        printf("7. Hapus lagu\n");
        printf("8. Modifikasi lagu\n");
        printf("9. Keluar program\n");
        printf("Masukkan mode: ");

    do {
        scanf("%d", &mode);
        if(mode < 1 || mode > 6){
            printf("Masukkan mode yang benar: ");
        }
    } while(mode < 1 || mode > 6);

    switch(mode){
        case 1:
            system("cls");
            printTitleSingeronly(data, count);
            
            printf("Apakah list ingin di sort berdasarkan abjad (y/n): ");
            char sort;
            do {
                scanf(" %c", &sort);
                if(sort != 'y' && sort != 'n'){
                    printf("Masukkan y atau n: ");
                }
            } while(sort != 'y' && sort != 'n');

            if(sort == 'y'){
                sortList(data, count);
                printTitleSingeronly(data, count);
            }

            askToPlayByIndex(data, count);
            goto menu;
            break;
        case 2:
            system("cls");
            searchSongbyGenre(data, search, count);
            askToPlayByIndex(search, count);
            break;
        case 3:
            system("cls");
            searchByYear(data, search, count);
            askToPlayByIndex(search, count);
            break;
        case 4:
            system("cls");
            searchSongBySinger(data, search, count);
            askToPlayByIndex(search, count);
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
        case 9:
            printf("\nTerima kasih telah menggunakan Music Player!\n");
            return 0;
        default:
            printf("Masukkan mode yang benar\n");
            return 0;
    }

    return 0;
}