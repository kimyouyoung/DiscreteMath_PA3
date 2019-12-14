#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>
#include "../include/libstemmer.h"

#define k 0

FILE *model;
int count_n = 0;
int count_nn = 0;

void read_csv(FILE *f, GHashTable *counter, int nega, GHashTable *stop);
void stopword(FILE *s, GHashTable *stop);
void print_counter (gpointer key, gpointer value, gpointer userdata);

int main () {   

    FILE *s = fopen("../data/stopwords", "r");
    if(s==NULL){
        printf("s is wrong\n");
        return 1;
    }
    GHashTable * stop = g_hash_table_new(g_str_hash, g_str_equal);
    stopword(s, stop);
    fclose(s);


    FILE *f = fopen("../data/train.negative.csv", "r");
    if(f==NULL){
        printf("f is wrong\n");
        return 1;
    }

    GHashTable * counter = g_hash_table_new(g_str_hash, g_str_equal);
    read_csv(f, counter, 0, stop);
    fclose(f);

    FILE *fp = fopen("../data/train.non-negative.csv", "r");
    if(fp==NULL){
        printf("fp is wrong\n");
        return 1;
    }

    read_csv(fp, counter, 1, stop);
    fclose(fp);

    model = fopen("../data/model.csv", "w");
    if(model==NULL){
        printf("m is wrong\n");
        return 1;
    }
    g_hash_table_foreach(counter, print_counter, 0x0); 
    fclose(model);
    
    return 0;
}

void stopword(FILE *s, GHashTable *stop){
    char word[32];
    int *a;

    while(!feof(s)){
        fscanf(s, "%s", word);
        a = malloc(sizeof(int));
        *a = 1;
        g_hash_table_insert(stop, strdup(word), a);
    }
}


void read_csv(FILE *f, GHashTable *counter, int nega, GHashTable *stop){
    char * line = 0x0 ;
    size_t r ; 
    size_t n = 0 ;
    struct sb_stemmer *stemmer = sb_stemmer_new("english", 0x0);

    int ex = 0;
    int ex_1=0;
    int ex_2=0;
    while (getline(&line, &n, f) >= 0) {
        char * t ;
        char * _line = line ;
        int ch;
        if(nega)
            count_n++;
        else  
            count_nn++;

        for (t = strtok(line, " \n\t") ; t != 0x0 ; t = strtok(0x0, " \n\t")) {
            int temp = 0, index = 0;
            while(t[index] != '\0'){
                ch = t[index];
                if(isalpha(ch)){
                    if(isupper(ch))
                        t[index] = t[index] + 32;
                    t[temp++] = t[index];
                }
                index++;
            }
            t[temp] = '\0';

            const char *stem = sb_stemmer_stem(stemmer, t, strlen(t));
            
            int * a;
            a = g_hash_table_lookup(stop, stem);
            if(strlen(stem) > 0 && a == NULL){
                int * d ;         
                d = g_hash_table_lookup(counter, stem);
                if (d == NULL) {
                    d = malloc(sizeof(int));
                    d[nega] = 1;
                    if(nega==0) d[1]=0;
                    else d[0]=0;
                    g_hash_table_insert(counter, strdup(stem), d) ;
                }
                else
                    d[nega]++;
            }
        }
        free(_line) ;
        line = 0x0 ;
   }
    sb_stemmer_delete(stemmer);
}

void print_counter (gpointer key, gpointer value, gpointer userdata){
    char * t = key ;
    int * d = value ;
    if(d[0]==0){
        if(d[1]>2)
            fprintf(model, "%s %0.20lf %0.20lf\n", t, (double)(50+d[0])/(2*50 + count_n), (double)(50+d[1])/(2*50 + count_nn));
    
    }else
        if(d[0]>2)
            fprintf(model, "%s %0.20lf %0.20lf\n", t, (double)(50+d[0])/(2*50 + count_n), (double)(50+d[1])/(2*50 + count_nn));

}
