#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "../include/libstemmer.h"

/* Sorry, this values are hard coded for cpu efficiency.
 * With other train datas, the defined values should be different.
 * this values express the size of the train cases for each neagative and non-negative cases.
 */
#define NEGATIVE 9078
#define NON_NEGATIVE 5565
#define K 0


FILE * model;
GHashTable * stop; //stopwords

void print_csv (gpointer key, gpointer value, gpointer userdata) 
{
	char * t = key ;
	int * d = value ;
    fprintf(model,"%s %0.80lf %0.80lf\n", t, (double)d[0]/(2*K + NEGATIVE), (double)d[1]/(2*K + NON_NEGATIVE));
	// printf("%s %lf %lf\n", t, (double)d[0]/Negative, (double)d[1]/nonNegative) ;
}

/* look-up stop dictionary for unnecessary words*/
int check(const char *s){
    //hashtags
    int * d ;			
    d = g_hash_table_lookup(stop, s);
    if(d==NULL) return 1;
    return 0;
}

/* initialize stopword dictionary */
void SWDictionary(GHashTable * dic){
    FILE *fp = fopen("../data/stopwords","r");
    char buf[64] = "";
    while(!feof(fp)){
        fscanf(fp,"%s",buf);
        if(buf[0]=='#') continue;
        int *d = malloc(sizeof(short));
        *d = 1;
        g_hash_table_insert(dic,strdup(buf),d);
    }
    fclose(fp);
}

/* Reads the data file and trains the model.
 * trains sets can be distinguished by the 'index' at the last paramenter
 * @param FILE *f: give data file opened by fopen with reading mode.
 * @param GHashTable *counter: give the hash table which will be trained.
 * @param int index: put 0 for negative, 1 for non-negative data-set
 */
void read(FILE *f, GHashTable * counter, int index){
    char * line = 0x0 ;
	size_t n = 0;
    struct sb_stemmer * stemmer = sb_stemmer_new("english", 0x0) ;

    while (getline(&line, &n, f) >= 0) {
		char * t ;
		for (t = strtok(line, " \n\t"); t != 0x0 ; t = strtok(0x0, " \n\t")) { 
            int src = 0, dest = 0;
            //lowercase && remove special chars
            while (t[src] != '\0') {
                if (isalpha(t[src])) {
                    t[dest] = tolower(t[src]);
                    ++dest;
                }
                ++src;
            }
            t[dest] = '\0';
            const char *s = sb_stemmer_stem(stemmer, t, strlen(t));

            //add to hash when it is necessary
            if(strlen(s)>0 && check(s)){ //conditions: length>0 && stopwords
                int * d ;			
                d = g_hash_table_lookup(counter, s) ;
                if (d == NULL) {
                    /* When smoothing, this is where you should MANIPULATE */
                    d = malloc(sizeof(int)*2) ;
                    if(index){ 
                        d[0] = K + 0;
                        d[1] = K + 1;
                    }
                    else{
                        d[0] = K + 1;
                        d[1] = K + 0;
                    }
                    g_hash_table_insert(counter, strdup(s), d);
                }
                else {
                    d[index]++;
                }
            }
		}
		line = 0x0;
	}
	sb_stemmer_delete(stemmer) ;
}

int main () 
{
	printf("START");
    /* Hash Initialization */
	GHashTable * counter = g_hash_table_new(g_str_hash, g_str_equal) ;
    stop = g_hash_table_new(g_str_hash, g_str_equal);
    SWDictionary(stop); //dictionary for stopwords initialized.
	
    //negative cases
    FILE * f = fopen("../data/train.negative.csv", "r") ;
	read(f,counter,0);
	fclose(f) ;

    //non-negative cases
    f = fopen("../data/train.non-negative.csv", "r");
    read(f,counter,1);
    fclose(f);

    //print out to model.csv
    model = fopen("./model.csv","w"); //model is a global variable, because it is also used at 'print_csv' function
	g_hash_table_foreach(counter, print_csv, 0x0) ;
    fclose(model);

    return 0;
}
