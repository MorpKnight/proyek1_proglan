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

void play(char *link){
    char command[256];
    snprintf(command, sizeof(command), "start %s", link);
    system(command);
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

    printf("Selamat datang di Music Player!\n");
    printf("1. Tampilkan semua lagu\n");
    printf("2. Tampilkan lagu berdasarkan genre\n");
    printf("3. Tampilkan lagu berdasarkan tahun rilis\n");
    printf("4. Tampilkan lagu berdasarkan penyanyi\n");
    printf("5. Tampilkan lagu berdasarkan judul\n");
    printf("6. Keluar program\n");
    printf("Masukkan mode: ");

    return 0;
}