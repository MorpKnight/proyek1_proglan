#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>
#include <time.h>

typedef struct DATA {
    char title[100], singer[100], link[200], genre[100];
    char genres[100][100];
    int year_release, duration, genre_count;
} SONG;

void splitGenreToGenres(SONG *data, int count); //gio
void printTitleSingeronly(SONG *data, int count); //gio
void searchSongbyGenre(SONG *data, SONG *search, int count, int *amount); //gio
void playSong(char *link); //kamal
void sortList(SONG *data, int count); //gio
void askToPlayByIndex(SONG *data, int count, int found);
void searchByYear(SONG *data, SONG *search, int count, int *amount); //kevin
void save(SONG *data, int count); //kamal
void searchSongBySinger(SONG *data, SONG *search, int count, int *amount); //kevin
void searchSongByTitle(SONG *data, SONG *search, int count, int *amount); //kevin
void delay(int seconds); 
void playList(SONG *data, int soungAmount); //kamal
void configureData(SONG *data, int position, int count); //kamal

void queue(SONG *data, int count);
void showQueue(SONG *data, int count);
void dequeue(SONG *data, int count);
void shuffle(SONG *data, int count);

int main(){
    SONG *data, *search;
    FILE *fp;
    char temp[1000];
    int count, n=0, i=0, j=0, mode=0, found=0, maxCount;

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
    maxCount = count;

    rewind(fp);
    data = (SONG*)calloc(count, sizeof(SONG));
    search = (SONG*)calloc(count, sizeof(SONG));
    fgets(temp, 1000, fp);

    n = 0;
    while(fgets(temp, 1000, fp) != NULL){
        sscanf(temp, "%[^,],%[^,],%d,%d,%[^,],%[^\n]", data[n].title, data[n].singer, &data[n].duration, &data[n].year_release, data[n].link, data[n].genre);
        n++;
    }

    fclose(fp);

    splitGenreToGenres(data, count);

    menu:
        system("cls");
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
        if(mode < 1 || mode > 9){
            printf("Masukkan mode yang benar: ");
        }
    } while(mode < 1 || mode > 9);

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

            askToPlayByIndex(data, count, count);
            goto menu;
            break;
        case 2:
            system("cls");
            searchSongbyGenre(data, search, count, &found);
            askToPlayByIndex(search, count, found);
            goto menu;
            break;
        case 3:
            system("cls");
            searchByYear(data, search, count, &found);
            askToPlayByIndex(search, count, found);
            goto menu;
            break;
        case 4:
            system("cls");
            searchSongBySinger(data, search, count, &found);
            askToPlayByIndex(search, count, found);
            goto menu;
            break;
        case 5:
            system("cls");
            searchSongByTitle(data, search, count, &found);
            askToPlayByIndex(search, count, found);
            goto menu;
            break;
        case 6:
            system("cls");
            printf("genres format: genre1,genre2...\n");
            do{
                count ++;
                if(count>maxCount){
                    count=maxCount;
                    data = realloc(data, count*sizeof(*data));
                }
                configureData(data, count-1, count);
                printf("Ketik 0 untuk berhenti, atau tekan sembarang untuk kembali ke menu: ");
                scanf(" %[^\n]", temp);
            }while(strcmp(temp,"0")!=0);
            goto menu;
            break;
        case 7:
            printf("Masukan angka diluar batas untuk kembali ke menu\n");
            while(1){
                system("cls");
                printTitleSingeronly(data, count);
                printf("Masukan nomor lagu yang ingin dihapus:");scanf("%d", &j);
                if(j<1 || j>count+1){
                    break;
                }
                count --;
                for(i=j-1 ; i<count ; i++){
                    data[i] = data [i+1];
                }
                save(data,count);
            }
            goto menu;
            break;
        case 8:
            system("cls");
            printTitleSingeronly(data, count);
            do{
                printf("Masukan angka diluar batas untuk kembali ke menu\n");
                printf("\nMasukan nomer lagu yang ingin diganti:"); scanf("%d", &i);
                if(i<1 || i>count+1){
                    break;
                }
                configureData(data, i-1, count);
                printf("Masukan angka diluar batas untuk kembali ke menu\n"); scanf("%d", &i);
            }while(i!=-1);
            goto menu;
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

void printTitleSingeronly(SONG *data, int count){
    int i;

    printf("Daftar lagu:\n");
    for(i = 0; i < count; i++){
        printf("%d. %s - %s\n", i+1, data[i].title, data[i].singer);
    }
}

void searchSongbyGenre(SONG *data, SONG *search, int count, int *amount){
    char genre[100];
    int i, j, found, index;

    printf("Masukkan genre: ");
    scanf("%s", genre);

    for(i = 0; i < strlen(genre); i++){
        genre[i] = tolower(genre[i]);
    }

    found = 0;
    index = 0;
    for(i = 0; i < count; i++){
        j = 0;
        while(strcmp(data[i].genres[j], "") != 0){
            for(int k = 0; k < strlen(data[i].genres[j]); k++){
                data[i].genres[j][k] = tolower(data[i].genres[j][k]);
            }
            if(strcmp(data[i].genres[j], genre) == 0){
                printf("%d. %s - %s\n", index+1, data[i].title, data[i].singer);
                index++;
                found = 1;

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

    *amount = index+1;
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

void askToPlayByIndex(SONG *data, int count, int found) {
    int index;
    do {
        printf("Kembali ke menu utama (-1)\nMemutar semua lagu(0)\nNomor lagu(1-%d)\nNomor pilihan:",found);
        scanf("%d", &index);

        if (index == -1) {
            printf("Kembali ke menu utama...\n");
            break;
        } 
        else if(index==0){
            playList(data, found);
        }
        else if (index > 0 && index <= count) {
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

void searchByYear(SONG *data, SONG *search, int count, int *amount) {
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

    *amount=index+1;
}

void searchSongByTitle(SONG *data, SONG *search, int count, int *amount) {
    char title[100];
    int i, j, found, index;

    printf("Masukkan judul: ");
    scanf("%[^\n]s", title);

    for (i = 0; i < strlen(title); i++) {
        title[i] = tolower(title[i]);
    }

    for (i = 0; i < count; i++) {
        for (j = 0; j < strlen(data[i].title); j++) {
            data[i].title[j] = tolower(data[i].title[j]);
        }

        if (strstr(data[i].title, title) != NULL) {
            strcpy(title, data[i].title);
            break;
        }
    }

    found = 0;
    index = 0;
    for (i = 0; i < count; i++) {
        if (strstr(data[i].title, title) != NULL) {
            printf("%d. %s - %s\n", index + 1, data[i].title, data[i].singer);
            index++;
            found = 1;

            strcpy(search[index - 1].title, data[i].title);
            strcpy(search[index - 1].singer, data[i].singer);
            strcpy(search[index - 1].link, data[i].link);
        }
    }

    if (found == 0) {
        printf("Tidak ada lagu dengan judul %s\n", title);
    }

    *amount = index + 1;
}

void searchSongBySinger(SONG *data, SONG *search, int count, int *amount) {
    char singer[100];
    int i, j, found, index;

    printf("Masukkan nama penyanyi: ");
    scanf("%s", singer);

    for (i = 0; i < strlen(singer); i++) {
        singer[i] = tolower(singer[i]);
    }

    for (i = 0; i < count; i++) {
        for (j = 0; j < strlen(data[i].singer); j++) {
            data[i].singer[j] = tolower(data[i].singer[j]);
        }

        if (strstr(data[i].singer, singer) != NULL) {
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

            strcpy(search[index - 1].title, data[i].title);
            strcpy(search[index - 1].singer, data[i].singer);
            strcpy(search[index - 1].link, data[i].link);
        }
    }

    if (found == 0) {
        printf("Tidak ada lagu dengan penyanyi %s\n", singer);
    }

    *amount = index+1;
}

void save(SONG *data, int count){
    int i,j,min;
    SONG temp;

    FILE *fp;
    fp = fopen("song.txt", "w");
    if(fp == NULL){
        printf("File tidak ditemukan");
    }

    fprintf(fp, "SONG_NAME,SONG_SINGER,DURATION,SONG_YEAR_RELEASE,SONG_LINK,SONG_GENRES\n");
    for(i = 0; i < count; i++){
        fprintf(fp, "%s,%s,%d,%d,%s ", data[i].title, data[i].singer, data[i].duration, data[i].year_release, data[i].link);
        for(int j = 0; j < data[i].genre_count; j++){
            fprintf(fp, ",%s", data[i].genres[j]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}
 
void delay(int seconds) {
    clock_t start_time = clock();
    while (clock() < start_time + (seconds*1000));
}

void playList(SONG *data, int songAmount){
    int i;
    clock_t start_time, time_played;
    printf("\n\nPlaying the list\nHere are the commands to use:\n1.Stop\n2.Pause\n3.Play\n4.Skip\nOnly type the number for the commands\n");

    for(i=0 ; i<songAmount ; i++){
        printf("Playing %s, by %s\n", data[i].title, data[i].singer);
        start_time = clock();
        playSong(data[i].link);
    
        while(1){
            time_played = (clock() - start_time) / CLOCKS_PER_SEC;
            if(time_played >= data[i].duration){
                printf("Finished playing %s\n", data[i].title);
                break;
            }
            delay(1000);
        }
    }
}

void configureData(SONG *data, int position, int count){
    int i,j;
    printf("\nJudul   : "); scanf(" %[^\n]", data[position].title);
    printf("Penyanyi: "); scanf(" %[^\n]", data[position].singer);
    printf("Menit   : "); scanf("%d",&i);
    printf("Detik   : "); scanf("%d",&j);
    data[count-1].duration= (i*60)+j;
    printf("Tahun   : "); scanf("%d", &data[position].year_release);
    printf("Link    : "); scanf(" %[^\n]", data[position].link);
    printf("Genres  : "); scanf(" %[^\n]", data[position].genre);
    splitGenreToGenres(data, count);
    save(data,count);
}

void queue(SONG *data, int count){
    int i;
    printf("\n\nPlaying the list\nHere are the commands to use:\n1.Stop\n2.Pause\n3.Play\n4.Skip\nOnly type the number for the commands\n");
    for(i=0 ; i<count ; i++){
        printf("Playing %s, by %s\n", data[i].title, data[i].singer);
        playSong(data[i].link);
    }
}

void showQueue(SONG *data, int count){
    int i;
    for(i=0 ; i<count ; i++){
        printf("%d. %s, by %s\n", i+1, data[i].title, data[i].singer);
    }
}

void dequeue(SONG *data, int count){
    int i;
    for(i=0 ; i<count-1 ; i++){
        data[i] = data[i+1];
    }
}

void shuffle(SONG *data, int count){
    int i, j, k;
    SONG temp;
    srand(time(NULL));
    for(i=0 ; i<count ; i++){
        j = rand() % count;
        k = rand() % count;
        temp = data[j];
        data[j] = data[k];
        data[k] = temp;
    }
}