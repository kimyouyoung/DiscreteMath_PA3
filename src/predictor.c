#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>
#include <math.h>
#include "../include/libstemmer.h"

#define theshold 0.7

void stopword(GHashTable *stop){

    FILE *s = fopen("../data/stopwords", "r");
    
    char word[32];
    int *a;

    while(!feof(s)){
        fscanf(s, "%s", word);
        a = malloc(sizeof(int));
        *a = 1;
        g_hash_table_insert(stop, strdup(word), a);
    }
    free(a);
    fclose(s);
}

void read_model(GHashTable *count){
    FILE *f = fopen("../data/model.csv", "r");
    char a[64];
    while(fscanf(f,"%s",a)!=EOF){
        double *d = malloc(sizeof(double));
        fscanf(f,"%lf",&d[0]);
        fscanf(f,"%lf",&d[1]);
        g_hash_table_insert(count, strdup(a), d);
    }
    fclose(f);
}

void test_data(FILE *f, GHashTable *stop, GHashTable *count){
    
    char * line = 0x0 ;
    size_t r ; 
    size_t n = 0 ;
    struct sb_stemmer *stemmer = sb_stemmer_new("english", 0x0);
    
    while (getline(&line, &n, f) >= 0) {
        char * t ;
        char * _line = line ;
        int ch;
        double sum_n = 0, sum_nn = 0, pro = 0;
        
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
            
            int *b = g_hash_table_lookup(stop, stem);
            if(strlen(stem) > 0 && b == NULL){
                double *d = malloc(sizeof(double));        
                d = g_hash_table_lookup(count, stem);
                if(d != NULL){
                    sum_n += log2(d[0]);
                    sum_nn += log2(d[1]);
                }
            }
        }
        printf("\n");
        sum_n = pow(2, sum_n);
        sum_nn = pow(2, sum_nn);
        pro = sum_n / (sum_n+sum_nn);
        printf("%lf\t", pro);
        if(pro > theshold)
            printf("NEGATIVE");
        else
            printf("NON-NEGATIVE");

        free(_line) ;
        line = 0x0 ;
   }
    sb_stemmer_delete(stemmer);
    printf("\n");
}

int main () 
{
    struct sb_stemmer * stemmer ;
   
    stemmer = sb_stemmer_new("english", 0x0) ;

    GHashTable * stop = g_hash_table_new(g_str_hash, g_str_equal);
    stopword(stop);

    GHashTable * count = g_hash_table_new(g_str_hash, g_str_equal);
    read_model(count);

    int choice=0;
    printf("1. Test Data or 2. Standard Input >> ");
    scanf("%d", &choice);
   
    if(choice == 1){
        FILE *n = fopen("../data/test.negative.csv", "r");
        test_data(n, stop, count);
        fclose(n);
        FILE *nn = fopen("../data/test.non-negative.csv", "r");
        test_data(nn, stop, count);
        fclose(nn);
        exit(0);
    }
    getchar();
    while(1){
        char result[50][50]= {};
        int ch=0;
        int i=0;
        int m=0;
        char buf[300];
        double sum_n = 0, sum_nn = 0, pro = 0;
        
        fgets(buf, 300, stdin);
        if(strcmp(buf, "exit\n")==0) break;
        char *ptr = strtok(buf, " ");
        while(ptr!=NULL){
            int temp = 0, index = 0;
            while(ptr[index] != '\0'){
                ch = ptr[index];
                if(isalpha(ch)){
                    if(isupper(ch))
                        ptr[index] = ptr[index] + 32;
                    ptr[temp++] = ptr[index];
                }
                index++;
            }
            ptr[temp] = '\0';

            strcpy(result[i], ptr);
            ptr = strtok(NULL, " ");
            i++;
        }

        int j=0;
        do {
            const char * s ;
            s = sb_stemmer_stem(stemmer, result[j], strlen(result[j]));
            int *a = g_hash_table_lookup(stop, s);
            if(strlen(s) > 0 && a == NULL){
                strcpy(result[m++], s);
                double *d = malloc(sizeof(double));        
                d = g_hash_table_lookup(count, s);
                if(d != NULL){
                    sum_n += log2(d[0]);
                    sum_nn += log2(d[1]);
                }
            }
            j++;
        } while (j < i);

        for(int k=0;k<m;k++)
            printf("%s " ,result[k]);

        printf("\n");
        sum_n = pow(2, sum_n);
        sum_nn = pow(2, sum_nn);
        pro = sum_n / (sum_n+sum_nn);
        printf("%lf\t", pro);
        if(pro > theshold)
            printf("NEGATIVE");
        else   
            printf("NON-NEGATIVE");

        printf("\n");
    }
    sb_stemmer_delete(stemmer) ;
    
    return 0;
}
