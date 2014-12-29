#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "util.h"
#include "trie.h"
#include "editor.h"

#define MAXWORD 100


struct maxstate
{
  Trie *tp;
  long max;
  unsigned char *maxs;
};

typedef struct maxstate MaxState;

/* 'update' allows us to use 'MaxState'
   as an accumulator, keeping track of the word
   with the maximum count as per the associated
   Trie.
*/
void update(MaxState *ms,unsigned char *t,size_t len)
{
  long val = lookup(ms->tp,t);
  if (val>ms->max){
    if (ms->maxs!=NULL)
      free(ms->maxs);
    ms->max = val;
    ms->maxs = nv_dup(t,len);
  }
}

/* To be passed as a callback to 'edits1'*/
void updater(void *p,unsigned char *s,size_t len){
  update(p,s,len);
}


/* Take a 'word' of length 'len' and return
   a corrected word. The return value might
   be 'word' itself or a newly allocated string.
   In the latter case the caller is responsible
   for freeing it.
*/
unsigned char *correct(Trie *tp,unsigned char *word,size_t len)
{
  MaxState ms;
  if (lookup(tp,word))
    return word;
  
  ms.tp = tp;
  ms.max = 0;
  ms.maxs = NULL;

  edits1(word,len,updater,&ms);
  if (ms.max)
    return ms.maxs;
  edits2(word,len,updater,&ms);
  if (ms.max)
    return ms.maxs;
  return word;
}

int main(int argc,char*argv[])
{
  if (argc!=2){
    fputs("Usage: norvig [training dataset]",stderr);
    return 2;
  }
  FILE *fp= fopen(argv[1],"r");
  if (fp==NULL){
    perror(argv[1]);
    return 1;
  }
  Trie *mod=train(fp);
  fclose(fp);
  
  unsigned char word[MAXWORD+1];
  while (!feof(stdin)){
    int c;
    int len=0;
    while ((c=getchar())!=EOF && c!='\n'){
      if (len==MAXWORD){
        word[len]='\0';
        fprintf(stderr,"Word too long: %s\n",word);
        exit(1);
      }
      word[len++] = tolower(c);
    }
    word[len]='\0';
    if (!len)
      continue;
    unsigned char *p = correct(mod,word,len);
    printf("%s, %s\n",word,p);
    if (p!=word)
      free(p);
  }
}
