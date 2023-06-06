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

SONG *getSong(SONG *head, int *totalSong){
    SONG *new = NULL;
    SONG *current = NULL;
    char temp[1000];
    FILE *fp = fopen("song.txt", "r");
    int i, j;

    if(fp == NULL){
        printf("File not found!\n");
        exit(1);
    }

    *totalSong = 0;
    fgets(temp, 1000, fp);
    while(fgets(temp, 1000, fp) != NULL){
        (*totalSong)++;
    }

    i = 0;
    rewind(fp);

    fgets(temp, 1000, fp);
    while(fgets(temp, 1000, fp) != NULL){
        new = (SONG*) malloc(sizeof(SONG));
        sscanf(temp, "%[^,],%[^,],%d,%d,%[^,],%[^\n]", new->title, new->singer, &new->duration, &new->year_release, new->link, new->genre);
        new->genre_count = 0;
        new->next = NULL;

        if(head == NULL){
            head = new;
            current = new;
        } else {
            current->next = new;
            current = new;
        }
        i++;
    }

    fclose(fp);
    return head;
}

void printSong(SONG *head){
    SONG *current = head;
    while(current != NULL){
        printf("%s\n", current->title);
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
    int i = 0;

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

    return searchList;
}

int main(){
    SONG *head = NULL;
    int totalSong;

    head = getSong(head, &totalSong);
    splitGenre(head, totalSong);
    printSong(head);

    mergeSort(&head);
    printf("\n\n");
    printSong(head);

    // search song
    SONG *searchList = searchSong(head);
    if(searchList != NULL){
        printSong(searchList);
    }

    return 0;
}