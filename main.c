#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void searchSongbyGenre(SONG *data, int count){
    char genre[100];
    int i, j, found;

    printf("Masukkan genre: ");
    scanf("%s", genre);

    found = 0;
    for(i = 0; i < count; i++){
        j = 0;
        while(strcmp(data[i].genres[j], "") != 0){
            if(strcmp(data[i].genres[j], genre) == 0){
                printf("%d. %s - %s\n", i+1, data[i].title, data[i].singer);
                found = 1;
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

void askToPlaySong(SONG *data, int count){
    int i, j, found;

    printf("Masukkan judul lagu: ");
    char title[100];
    scanf("%s", title);

    found = 0;
    for(i = 0; i < count; i++){
        if(strcmp(data[i].title, title) == 0){
            printf("Apakah anda ingin memutar lagu %s (y/n): ", data[i].title);
            char play;
            do {
                scanf(" %c", &play);
                if(play != 'y' && play != 'n'){
                    printf("Masukkan y atau n: ");
                }
            } while(play != 'y' && play != 'n');

            if(play == 'y'){
                playSong(data[i].link);
            }
            found = 1;
            break;
        }
    }

    if(found == 0){
        printf("Tidak ada lagu dengan judul %s\n", title);
    }
}

int main(){
    SONG *data;
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
        printf("6. Keluar program\n");
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

            askToPlaySong(data, count);
            break;
        case 2:
            system("cls");
            searchSongbyGenre(data, count);
            goto menu;
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            printf("\nTerima kasih telah menggunakan Music Player!\n");
            return 0;
        default:
            printf("Masukkan mode yang benar\n");
            return 0;
    }

    return 0;
}