
// Project Libraries
#include "ece556.h"
//#include "gwlibc.h"



/* ****************************** Linked List (start)  ****************************** */
/* List Node  */
typedef struct __node_LL_t{
  void* data;
  struct __node_LL_t *next;
}node_LL_t;


/* LinkedList ADT */
typedef struct __LL_t{
  size_t numItems;
  node_LL_t* head;
  node_LL_t* last;		/* for optimizing frequent adding-to-end oprations */
}LL_t;

/* create a new liked list node */
node_LL_t* new_node_LL_t() {
  node_LL_t *node = (node_LL_t*)malloc(sizeof(*node));
  if(node==NULL) {
    perror("new_node_LL_t");
    exit(-1);
  }
  else {
    node->next=NULL;
    return node;
  }
}

LL_t* new_LL_t() {
  LL_t* list=(LL_t*)malloc(sizeof(*list));
  if(list==NULL) {
    perror("new_LL_t");
    exit(-1);
  }
  else {
    list->numItems=0;
    list->head=NULL;
    list->last=NULL;
  }
  return list;
}

/* add node to list, right after n=pos node. */
void LL_add_node(LL_t* list, size_t pos, node_LL_t* node){

  /* error check */
  if(pos > list->numItems) {
    perror("add_node_LL");
    exit(-1);
  }

  /* add in head of LL */
  if(pos==0){
    if(list->head != NULL) node->next= list->head->next; /* added 0427 */
    else node->next = NULL;	/* added 0427 */
    list->head = node;
  }
  /* add in middle or end of LL */
  else  {
    int		i;
    node_LL_t*	ip;
    for(ip=list->head,i=0;i<(int)pos-1;ip=ip->next,i++)
      ;
    /* now ip is at pos-th node */
    node->next = ip->next;
    ip->next   = node;
  }
  if(pos==list->numItems) list->last=node;
  list->numItems++;
  return;
}

/* return -1 if not found, or pos if found */
long  LL_remove_node(LL_t* list, node_LL_t* node) {
  /* error check */
  if(node == NULL || list == NULL){
    perror("LL_remove_node");
    exit(-1);
  }

  if(node == list->head) {
    list->head = node->next;
    if(list->numItems >0)    list->numItems--;
    if(list->numItems == 0) list->last = NULL; /* update ptr to last node  */
    return 1;
  }
  else {
    int i;
    node_LL_t* ip;

    for(i=0,ip=list->head;
	ip->next != node && ip->next != NULL;
	i++,ip = ip->next)
      ;

    if(list->numItems>0 && i < (int)list->numItems-2){
      /* found in middle*/
      ip->next = node->next;
      list->numItems--;
      return i;
    }
    else if(list->numItems > 0 && i == (int)list->numItems-2){
      /* found at the end, need to update last */
      list->last = ip;
      ip->next = node->next;
      list->numItems--;
      return i;
    }
    else{
      return -1;
    }
  }

}

/* ****************************** Linked List (end) ****************************** */



/* ******************************(min) Priority Queue (start) ****************************** */
#define MIN_HEAP_SIZE 11

/* (min) PQ */
typedef struct __node_PQ_t{
  void* data;
  void* key;
}node_PQ_t;

typedef struct __PQ_t{
  size_t numItems;
  size_t heapSize;
  node_PQ_t** heap;
  __compar_fn_t comp;
}PQ_t;

node_PQ_t* new_node_PQ_t() {
  node_PQ_t* node = (node_PQ_t*)malloc(sizeof(node_PQ_t));
  node->data = NULL;
  node->key = NULL;
  return node;
}



PQ_t* new_PQ_t(size_t initCapacity, __compar_fn_t comp) {
  //  int i=0;
  PQ_t* pq     = (PQ_t*)malloc(sizeof(PQ_t));
  pq->numItems = 0;

  size_t	newSize = initCapacity + 1 > MIN_HEAP_SIZE ? (initCapacity + 1):MIN_HEAP_SIZE;
  //  pq->heap = (node_PQ_t**)malloc(newSize*sizeof(node_PQ_t*));
  pq->heap = NULL;
  pq->heap = (node_PQ_t**)realloc(pq->heap, (newSize*sizeof(node_PQ_t*)));
  //  for(i=0; i<newSize; i++) 
  //    pq->heap[i] = NULL;
    pq->heap[0]		= NULL; /* 0 is never used */
  pq->heapSize		= newSize;

  pq->comp = comp;
  return pq;
}

void swap_PQ_node(PQ_t* pq, size_t n1, size_t n2){
  if((n1 > pq->numItems +1)	||
     (n2 > pq->numItems +1)	||
     (n1 == 0)			||
     (n2 == 0)) {
    perror("swap_PQ_node");
    exit(-1);
  }
  node_PQ_t*	temp;
  temp	       = pq->heap[n1];
  pq->heap[n1] = pq->heap[n2];
  pq->heap[n2] = temp;
  return;
}

void PQ_add_node(PQ_t* pq, node_PQ_t* node) {
  int numOld = pq->numItems;
  /* resize if too small */
  while(pq->numItems + 2 > pq->heapSize) {
    int newSize =  2*(numOld+2);
    pq->heapSize = newSize;
    pq->heap = (node_PQ_t**)realloc(pq->heap, newSize*sizeof(node_PQ_t*)); /* dubious */
  }

  pq->heap[numOld +1] =  node;

  if(numOld > 0) {
    size_t n1,n2;
    for(n1 = (numOld +1)/2, n2 = (numOld+1);
	n1>0 && n2>0 && (*pq->comp)(pq->heap[n1], pq->heap[n2]) > 0;
	n1 = n1/ 2, n2 = n2/2)
      swap_PQ_node(pq,n1,n2);
  }

  pq->numItems = ++numOld;
  return;
}

node_PQ_t* PQ_remove_node(PQ_t* pq) {

  if(pq->numItems == 0) 
    return NULL;

  node_PQ_t*	node = pq->heap[1];

  int	numOld = pq->numItems;

  if(numOld > 1) {
    pq->heap[1]	     = pq->heap[numOld];
    pq->heap[numOld] = NULL;

    int n1,		n2,	n3,		rflag,		lflag;
    n1	      = 1,	n2=1*2, n3=1*2+1,	lflag=0,	rflag=0;
    do {
      /* is lchild swappable? */
      if(n2 > numOld-1) 
	break;
      else 
	lflag = (*pq->comp)(pq->heap[n1],pq->heap[n2]) > 0 ? 1:0;

      /* is rchild swappable? */
      if(n3 > numOld-1)
	rflag = 0; 
      else 
	rflag = (*pq->comp)(pq->heap[n1],pq->heap[n3]) > 0 ? 1:0;

      /* choose 1 between lchild and rchild */
      if (lflag && rflag) {
	lflag = (*pq->comp)(pq->heap[n2],pq->heap[n3]) < 0 ? 1:0;
	rflag = lflag ? 0:1;
      }

      /* actual swapping */
      if(lflag && !rflag) {
	swap_PQ_node(pq,n1,	n2);
	n1 = n2;
	n2 = 2*n1;
	n3 = n2+1;
      }
      else if(!lflag && rflag){
	swap_PQ_node(pq,n1,	n3);
	n1 = n3;
	n2 = 2*n1;
	n3 = n2+1;
      }
      else if(!lflag && !rflag) {
	break;
      }
      else {
	perror("PQ_remove_node");
	exit(-1);
      }
    } while(lflag||rflag);
  }
  
  pq->numItems = --numOld;

  while(pq->heapSize/2 >pq->numItems+1 && pq->heapSize/2 > MIN_HEAP_SIZE ) {
    int newSize	 = pq->heapSize/2;
    pq->heapSize = newSize;
    pq->heap = (node_PQ_t**)realloc(pq->heap, newSize*sizeof(node_PQ_t*));
  }
    
  return node;
}

//helper func
int min(int n1, int n2) {
  return n1>n2?n2:n1;
}
void PQ_print(PQ_t* pq) {
  int height = 1 + (int)(log((double)pq->numItems)/log(2.0));
  //  int width = pow (2, height - 1);
  int num = pq->numItems;
  int i,j;
  int index;
  index = 0;
  for(i=0;i<height;i++) {
    printf("\n");
    for(j=0;j< min((int) pow(2,i) , num -( (int) pow(2,i) - 1)  );j++) {
      index ++;
      printf(" %d",*(int*)pq->heap[index]->data);
    }
  }
  printf("\n ");
  return;
}


/* sample comparator */
/* int compar_PQ(const void* a, const void* b) { */
/*   int aa=*(int*)((node_PQ_t*)a)->key; */
/*   int bb=*(int*)((node_PQ_t*)b)->key; */
/*   if(aa<bb) return -1; */
/*   else if(aa == bb) return 0; */
/*   else return 1; */
/* } */
/* ******************************(min) Priority Queue (end) ****************************** */


/* ******************************MergeSort (start) ****************************** */
/* helper func */
void swap(void **v, int i, int j) { 
  void *temp; 
  temp = v[i]; 
  v[i] = v[j]; 
  v[j] = temp; 
  return;
}

void merge(void** m, void** a, size_t na, void** b, size_t nb, __compar_fn_t comp) {
  //merge two portions into one sorted whole
  //the twist here is you have to keep an original ordered m sequence in order to fill the merged-sorted-list
  void** new_array=(void**)malloc((na+nb)*sizeof(void*));
  int ia,ib,im;
  ia=0,ib=0,im=0;

  while((ia<(int)na)&&(ib<(int)nb))
    if((*comp)(a[ia], b[ib]) < 0) 
      new_array[im++]=a[ia++];
    else
      new_array[im++]=b[ib++];

  if(ia==(int)na)
    while(ib<(int)nb)
      new_array[im++]=b[ib++];
  else if(ib==(int)nb)
    while(ia<(int)na)
      new_array[im++]=a[ia++];
  else {
    perror("merge");
    exit(-1);
  }

  int i;
  for(i=0;i<(int)(na+nb);i++)
    m[i]=new_array[i];
  free(new_array);
  return;
}

void msort(void **v, size_t nmemb, size_t size, __compar_fn_t comp) {
  //base case 1
  if(nmemb == 1) return;
  //base case 2
  if(nmemb == 2) {
    if((*comp)(v[0], v[1]) > 0)
      swap(v, 0, 1);
    return;
  }
  size_t middle=(nmemb)/2;
  //recursion 1, left half
  msort(v,middle,size,comp);
  //recursion 2, right half
  msort(v + middle, nmemb - middle,size,comp);
  //merge sorted two lists
  merge(v, v, middle, v+middle, nmemb - middle, comp);
  return;
}
/* ******************************MergeSort (end) ****************************** */



//#define INFINITY INT_MAX;
#define MAX_CHAR_PER_LINE	1000

typedef struct __token_t {
  char**		tokens;
  unsigned	count;
}token_t;

typedef struct {
  point *p1;
  point *p2;
  long unsigned dist;
} pair;

//MBB is nothing but a series of pins that forms a rectangle
//impl MBB as point**

typedef struct __MBB{
  point** pins;
  int numPins;
} MBB;

int coord_to_id(point p, routingInst* rst) {
  return (rst->gx * p.y + p.x);
}

/* in gwlibc.h */
/* int min(int n1, int n2) { */
/*   return n1>n2?n2:n1; */
/* } */

int max(int n1, int n2) {
  return n1>n2?n1:n2;
}

int is_point_equal(point p1, point p2) {

  if(p1.x==p2.x&&p1.y==p2.y)
    return 1;
  else 
    return 0;
}


//def: mergesort starts
int comparator_pair_dist(const void *p1, const void *p2){
  long unsigned aa,bb;

  aa=((pair*)p1)->dist;
  bb=((pair*)p2)->dist;
  if (aa<bb) return -1;
  else if(aa==bb) return 0;
  else return 1;
}

//
int comparator_point_x(const void *p1, const void *p2){
  int aa,bb;
  aa=((point*)p1)->x;
  bb=((point*)p2)->x;
  if (aa<bb) return -1;
  else if(aa==bb) return 0;
  else return 1;
}


int comparator_point_y(const void *p1,const void *p2){
  int aa,bb;
  aa=((point*)p1)->y;
  bb=((point*)p2)->y;
  if (aa<bb) return -1;
  else if(aa==bb) return 0;
  else return 1;
}



long unsigned getManhattanDist(point* p1, point* p2) {
  long unsigned dist=0;
  if(p1->x > p2->x)
    dist+=(long unsigned)(p1->x - p2->x);
  else 
    dist+=(long unsigned)(p2->x - p1->x);

  if(p1->y > p2->y) 
    dist+=(long unsigned)(p1->y - p2->y);
  else 
    dist+=(long unsigned)(p2->y - p1->y);
  return dist;
}
// helper func
size_t pairUpWithNext15PointsY(point** S, int numS, int i, pair** pairs,  int k) {
  if((numS<1)||(i>numS)||(i<0)) {perror("getDistToNext15PointsY"); exit(-1);}
  point** is;
  pair** ip;
  int j;
  //numPairs-k is used to check remaining space available in numPairs
  for(j=0,is=S+i+1,ip=pairs + k;(j<numS - i-1)&&(j<16 -1);j++,is++,ip++) {
    (*ip)->p1=S[i];
    (*ip)->p2=*is;
    (*ip)->dist=getManhattanDist((*ip)->p1, (*ip)->p2);
  }

  if(j==numS - i -1) return (size_t) (numS-i -1);
  else return (size_t)(16-1);
}
// helper func
void Slow_Closest_Pair(point **P,int numP, pair* min_pr ) {
  if(numP>3||numP<2) {
    perror("Slow_Closest_Pair");
    exit(-1);
  }

  if(numP==2) {
    min_pr->p1=P[0];
    min_pr->p2=P[1];
    min_pr->dist=getManhattanDist(min_pr->p1, min_pr->p2);
    return;
  }
  else if(numP==3) {
    long unsigned dist_1_2 = getManhattanDist(P[0],P[1]);
    long unsigned dist_2_3 = getManhattanDist(P[1],P[2]);
    long unsigned dist_1_3 = getManhattanDist(P[0],P[2]);


    if(dist_1_2 < dist_2_3 ) {
      if(dist_1_3 < dist_1_2) {
	min_pr->p1=P[0];  
	min_pr->p2=P[2];
	min_pr->dist=dist_1_3;
      }
      else {
	min_pr->p1=P[0];  
	min_pr->p2=P[1];
	min_pr->dist=dist_1_2;
      }
    }
    else {
      if(dist_1_3 < dist_2_3) {
	min_pr->p1=P[0];  
	min_pr->p2=P[2];
	min_pr->dist=dist_1_3;
      }
      else {
	min_pr->p1=P[1];  
	min_pr->p2=P[2];
	min_pr->dist=dist_2_3;
      }
    }
    return;
  }
}
// helper func
void Closest_Pair_Rec(point **Px, point ** Py, size_t numP, pair* min_pr) {
  if (numP<=3) {
    Slow_Closest_Pair(Px,numP, min_pr);
    return;
  }

  size_t middle=numP/2;
  point** Qx=(point**)malloc(middle*sizeof(point*));
  point** Rx=(point**)malloc((numP-middle)*sizeof(point*));
  point** Qy=(point**)malloc(middle*sizeof(point*));
  point** Ry=(point**)malloc((numP-middle)*sizeof(point*));

  int i;
  for(i=0;i<(int)middle;i++)
    Qy[i]=Qx[i]=Px[i];
  //  msort((void**)Qy,0,middle-1,comparator_point_y);
  msort((void**)Qy,middle,sizeof(*Qy),comparator_point_y);

  for(i=(int)middle;i<(int)numP;i++)
   Ry[i-middle]= Rx[i-middle]=Px[i];
  //  msort((void**)Ry,0, numP-middle-1,comparator_point_y);
  msort((void**)Ry,numP-middle,sizeof(*Ry),comparator_point_y);

  pair *min_pr_q = (pair*)malloc(sizeof(pair));

  min_pr_q->dist=ULONG_MAX;
  Closest_Pair_Rec(Qx, Qy, middle, min_pr_q);


  pair *min_pr_r = (pair*)malloc(sizeof(pair));

  min_pr_r->dist=ULONG_MAX;
  Closest_Pair_Rec(Rx, Ry, numP-middle, min_pr_r);

  long unsigned delta=min_pr_q->dist <= min_pr_r->dist?min_pr_q->dist : min_pr_r->dist;

  int x_star=Qx[middle-1]->x;

  point** S=(point**)malloc(sizeof(point*));
  int numS;
  for(i=0,numS=0;i<(int)numP;i++) {
    if((long unsigned)abs(Px[i]->x-x_star)<=delta) {
      S[numS]=Px[i];
      numS++;
      S=(point**)realloc(S,(numS+1)*sizeof(point*));
    }
  }

  if(numS==0||numS==1) {
    min_pr->p1=min_pr_q-> dist < min_pr_r->dist? min_pr_q->p1:min_pr_r->p1;
    min_pr->p2=min_pr_q-> dist < min_pr_r->dist? min_pr_q->p2:min_pr_r->p2;
    min_pr->dist=min_pr_q-> dist < min_pr_r->dist? min_pr_q->dist:min_pr_r->dist;
    return;
  }


  msort((void**)S,numS, sizeof(*S),comparator_point_y);

  point** is;
  pair** pairs_S=(pair**)malloc(15*numS*sizeof(pair*));

  for(i=0;i<15*numS;i++)
    pairs_S[i]=(pair*)malloc(sizeof(pair)); //scattered spaces

  size_t n=0;
  size_t count=0;
  for(i=0,is=S;i<numS;i++,is++) {
    n= pairUpWithNext15PointsY( S,  numS,  i, pairs_S, count);
    count+=n;
  }

  msort((void**)pairs_S,count,sizeof(*pairs_S),comparator_pair_dist);
  pair* min_pr_s=pairs_S[0]; //extract min


  if (min_pr_s->dist < delta) {
    min_pr->p1=min_pr_s->p1;
    min_pr->p2=min_pr_s->p2;
    min_pr->dist=min_pr_s->dist;
  }
  else if(min_pr_q < min_pr_r) {
    min_pr->p1=min_pr_q->p1;
    min_pr->p2=min_pr_q->p2;
    min_pr->dist=min_pr_q->dist;
  }
  else {
    min_pr->p1=min_pr_r->p1;
    min_pr->p2=min_pr_r->p2;
    min_pr->dist=min_pr_r->dist;
  }

  //release mem, in reverse order of allocation  
  for(i=0;i<15*numS;i++)
    free(pairs_S[i]);
  free(pairs_S);
  free(S);
  free(min_pr_r);
  free(min_pr_q);
  free(Ry);
  free(Qy);
  free(Rx);
  free(Qx);
  // TODO: why no return here?
  return;
}
// closest_pair main
void Closest_Pair(point **P,int numP, pair* min_pr) {

  point **ppx=(point**)malloc(numP*sizeof(point*));
  point **ppy=(point**)malloc(numP*sizeof(point*));

  int i;
  for(i=0;i<numP;i++) {
    ppx[i]=ppy[i]=P[i];
  }

  //  msort((void**)ppx,0,numP-1,comparator_point_x);
  msort((void**)ppx,numP,sizeof(*ppx),comparator_point_x);
  //  msort((void**)ppy,0,numP-1,comparator_point_y);
  msort((void**)ppy,numP,sizeof(*ppy),comparator_point_y);

  Closest_Pair_Rec(ppx, ppy,  numP,  min_pr);

  free(ppx);
  free(ppy);
  return;
}


// helper func
token_t *tokenize(const char* input) {
  char* str = strdup(input);
  int count = 0;
  int capacity = 10;
  char** result = (char**)malloc(capacity*sizeof(*result));

  char* tok = strtok(str, " \t");

  while (1) {
    if (count >= capacity)
      result = (char**)realloc(result, (capacity *= 2)*sizeof(*result));

    result[count++] = tok ? strdup(tok) : tok;

    if (!tok) break;

    tok = strtok(NULL, " \t");
  }

  free(str);

  token_t *toks = (token_t*)malloc(sizeof(token_t));

  toks->tokens = result;
  toks->count = count - 1;//the last increment of count is trash
  return toks;
}


// Tested OK
int getEdgeId(point *p1, point *p2, routingInst* rst) {
  int gx = rst->gx;
  int gy = rst->gy;


  int edgeId = 0;

  if(p1->x==p2->x&&abs(p1->y-p2->y)==1) {
    //    an vertical edge;
    point *pt_high=(p1->y>=p2->y)?p1:p2;
    point *pt_low=(p1->y>=p2->y)?p2:p1;

    //    int ix=pt_low->x;
    int iy=pt_low->y;
    while(iy<pt_high->y)
      {
	edgeId=((gx-1)*gy-1)+gx*iy+(pt_low->x+1);
	iy++;
      }
    return edgeId;
  }
  else if(p1->y==p2->y&&abs(p1->x-p2->x)==1) {
    //    an vertical edge;
    point *pt_right=(p1->x>=p2->x)?p1:p2;
    point *pt_left=(p1->x>=p2->x)?p2:p1;

    int ix=pt_left->x,iy=pt_left->y;
    while(ix<pt_right->x)
      {
	edgeId=ix+iy*(gx-1);
	ix++;
      }
    return edgeId;
  }
  else return -1;
}

//Tested OK
pair* getEdgePoints(int edgeId, routingInst* rst) {

  if(edgeId < 0) {
    perror("getEdgePoints");
    exit(-1);
  }

  int gx = rst->gx;
  int gy = rst->gy;

  int boundary = (gx - 1) * gy;

  pair* pr = (pair*) malloc(sizeof(*pr));
  pr->p1 = (point*) malloc(sizeof(point));
  pr->p2 = (point*) malloc(sizeof(point));

  // horizontal edge
  if(edgeId < boundary) {
    pr->p1->y = pr->p2->y = edgeId / (gx - 1);
    pr->p1->x = edgeId % (gx -1);
    pr->p2->x = edgeId % (gx -1) +1;
  }
  // vertical edge
  else {
    pr->p1->x = pr->p2->x = (edgeId - boundary) % gx;
    pr->p1->y = (edgeId - boundary) / gx;
    pr->p2->y = (edgeId - boundary) / gx +1;
  }

  return pr;
}



// Tested OK
int readBenchmark(const char *fileName, routingInst *rst) {

  FILE *fp = fopen(fileName, "r");
  if (fp == NULL) {
    printf("\nOpen file %s failed!", fileName);
    return 0;
  }


  char *s = (char *)malloc(sizeof(char)*MAX_CHAR_PER_LINE);

  if (s == NULL) {
    printf("\nMalloc line buffer failed!");
    return 0;
  }


  int netindex = 0;
  int pinindex = 0;

  int blockadenum = 0;
  int blockadeindex = 0;

  while (!feof(fp)) {
    fgets(s, MAX_CHAR_PER_LINE, fp);

    //trim trailing \n of s
    if (s[strlen(s) - 1] == '\n') s[strlen(s) - 1] = '\0';

    //tokenize s
    char* s_dup = strdup(s);	/* watch for this */
    token_t *toks = tokenize(s_dup);
    //     token_t *toks = tokenize(strdup(s));
    //    token_t *toks = tokenize(s);
    free(s_dup);		/* watch */


    //readin grid dimensions
    if (strcmp(*toks->tokens, "grid") == 0) {
      char** it;
      it = toks->tokens;

      ++it;
      rst->gx = atoi(*it);
      free(*it);

      ++it;
      rst->gy = atoi(*it);
      free(*it);

      //begin building edges
      rst->numEdges = rst->gy*(rst->gx - 1) + rst->gx*(rst->gy - 1);
      rst->edgeCaps = (int*)malloc(rst->numEdges*sizeof(int)); //please FREE this in Release(); -GW
      //      rst->edgeCaps = new int[rst->numEdges];
      if (rst->edgeCaps == NULL) {
	printf("\nMalloc edgeCaps buffer failed!");
	return 0;
      }
      int cnt = 0;

      rst->edgeUtils = (int*)malloc(rst->numEdges*sizeof(int));//please FREE this in Release(); -GW
      //      rst->edgeUtils = new int[rst->numEdges];
      if (rst->edgeUtils == NULL) {
	printf("\nMalloc edgeUtils buffer failed!");
	return 0;
      }

      //initialize edge's cap and utils
      for (cnt = 0; cnt < rst->numEdges; cnt++) {
	rst->edgeCaps[cnt] = rst->cap;
	rst->edgeUtils[cnt] = 0;
      }

    }
    //readin capacity
    else if (strcmp(*toks->tokens, "capacity") == 0) {
      char** it;
      it = toks->tokens;

      ++it;
      rst->cap = atoi(*it);
      free(*it);

      int i;
      //initialize the array of edge caps and edge utils 
      //(TBD: edgeUtils are how many edges used out of available caps)
      for (i = 0; i < rst->numEdges; ++i) {
	rst->edgeCaps[i] = rst->cap;
	rst->edgeUtils[i] = 0;
      }
    }
    //reading num net
    else if (strcmp(*toks->tokens, "num") == 0) {
      char** it;
      it = toks->tokens;

      ++it;
      if (strcmp(*toks->tokens, "net")) {
	free(*it);

	++it;
	rst->numNets = atoi(*it);
	free(*it);

	rst->nets = (net*)malloc(rst->numNets*sizeof(net));
	if (rst->nets == NULL) {
	  printf("\nMalloc nets buffer failed!");
	  return 0;
	}
      }
    }
    //readin a net
    else if (strncmp(*toks->tokens, "n", 1) == 0) {
      char** it;
      it = toks->tokens;

      //*it points to "n...", *it +1 points to "..."(digits only)
      (&(rst->nets[netindex]))->id = atoi(*it + 1);

      ++it;
      (&(rst->nets[netindex]))->numPins = atoi(*it);
      (&(rst->nets[netindex]))->pins = (point*)malloc((&(rst->nets[netindex]))->numPins*sizeof(point));
      if ((&(rst->nets[netindex]))->pins == NULL) {
	printf("\nMalloc pin buffer failed!");
	return 0;
      }

      free(*it);

      netindex++;// increment netindex because one net header is readin
      pinindex = 0;// zero pinindex used by last net

    }
    //readin pins of a net
    else if (pinindex < ((&(rst->nets[netindex - 1]))->numPins)) { //netindex-1 is because the prev increment 
      char** it;
      it = toks->tokens;

      (&((&(rst->nets[netindex - 1]))->pins[pinindex]))->x = atoi(*it); //note that netindex-1

      ++it;
      (&((&(rst->nets[netindex - 1]))->pins[pinindex]))->y = atoi(*it); //note that netindex-1
      free(*it);

      pinindex++; //increment pinindex and go to next pin

    }
    //readin blockade count
    else if (toks->count == 1 && (blockadenum = atoi(*toks->tokens)) > 0) {//**********/03/16/2014
      // do nothing
    }
    //readin blockade edges
    else if (blockadeindex < blockadenum) {//blockadge section

      char** it;
      it = toks->tokens;

      //define p1
      point *blkd_p1 = (point*)malloc(sizeof(point));
      blkd_p1->x = atoi(*it);

      ++it;
      blkd_p1->y = atoi(*it);
      free(*it); //TBC: only free it from 2nd onwards, (1st is freed as "token" at last.) -GW

      //define p2
      ++it;
      point* blkd_p2 = (point*)malloc(sizeof(point));
      blkd_p2->x = atoi(*it);
      free(*it);

      ++it;
      blkd_p2->y = atoi(*it);
      free(*it);

      //define capacity
      ++it;
      int blkd_capacity;
      blkd_capacity = atoi(*it);
      free(*it);

      //update affected edge's capacity
      int blkd_edgeId;

      blkd_edgeId = getEdgeId(blkd_p1, blkd_p2,rst);
      free(blkd_p1);
      free(blkd_p2);
      if (blkd_edgeId < 0) {
	printf("\n Failed to get edge id of an blockade!");
	return 0;
      }
      rst->edgeCaps[blkd_edgeId] = blkd_capacity;

      blockadeindex++;
    }
    free(toks->tokens);		/* watch this */
    free(toks);
  }

  free(s);
  fclose(fp);

  return 1;
}

// Tested OK
//given a pair* pr defining a MBB, and a point* pt to consider.(pt is outside of the MBB)
//return the distance between the MBB and the pt.
//record the resulted closest pair's other point into point* other
//treat point as primitive type, allow duplicated points with same coordinate
int distPtMBB(pair* pr, point* pt, point* other) {

  //bottom left conner point
  point* pt_BL=(point*) malloc(sizeof(point));
  pt_BL->x=min(pr->p1->x, pr->p2->x);
  pt_BL->y=min(pr->p1->y, pr->p2->y);

  //bottom right conner point
  point* pt_BR=(point*) malloc(sizeof(point));
  pt_BR->x = max(pr->p1->x, pr->p2->x);
  pt_BR->y=min(pr->p1->y, pr->p2->y);

  //top left conner point
  point* pt_TL=(point*) malloc(sizeof(point));
  pt_TL->x=min(pr->p1->x, pr->p2->x);
  pt_TL->y=max(pr->p1->y, pr->p2->y);


  //top right conner point
  point* pt_TR=(point*) malloc(sizeof(point));
  pt_TR->x=max(pr->p1->x, pr->p2->x);
  pt_TR->y=max(pr->p1->y, pr->p2->y);


  //TODO: also return the point found on MBB, may need to make a new struct containing both the dist and the returned point
  int dist=0;
  //convention of internal, (...< pt->x <=]
  //9 scenarios in total
  //1: pt is to the bottom left corner of MBB
  if(pt->x <=pt_BL->x && pt->y<=pt_BL->y ){
    dist= (pt_BL->x - pt->x) + (pt_BL->y - pt->y);
    *other=*pt_BL; //direct assignment by value, other will persists after exiting current function
  }

  //2: pt is below MBB in y , but is in middle of MBB in x
  else if( (pt_BL->x < pt->x && pt->x <= pt_BR->x) && pt->y<=pt_BL->y ) {
    dist= (pt_BL->y - pt->y);
    other->x=pt->x;
    other->y=pt_BL->y;
  }

  //3: pt is below MBB in y , but is to the right of MBB
  else if(pt_BR->x < pt->x && pt->y<=pt_BL->y ) {
    dist= ( pt->x - pt_BL->x) + (pt_BL->y - pt->y);
    *other=*pt_BR;
  }

  //4: pt is to the left of MBB in x, but is in the middle of MBB in y
  else if(pt->x <=pt_BL->x && (pt_BL->y <pt->y && pt->y <= pt_TL->y) ) {
    dist= (pt_BL->x - pt->x);
    other->x=pt_BL->x;
    other->y=pt->y;
  }


  //5: pt is in the middle of MBB in x, but is in the middle of MBB in y
  else if((pt_BL->x < pt->x && pt->x <= pt_BR->x)&&(pt_BL->y < pt->y && pt->y <=pt_TL->y)) {
    //optimize below into if statements if have time
    dist=min( min(pt_BR->x - pt->x, pt->x - pt_BL->x ),min(pt_TL->y - pt->y, pt->y - pt_BL->y));

    if((pt->x- pt_BL->x)<=(pt_BR->x-pt->x)) {
      other->x=pt_BL->x; // in case of tie, keep left
      if((pt->y-pt_BL->y)<=(pt_TL->y - pt->y)) { 
	other->y=pt_BL->y;//in case of tie, keep bottom-side
      }
      else {
	other->y=pt_TL->y;
      }
    }
    else {
      other->x=pt_BR->x; 
      if((pt->y-pt_BL->y)<=(pt_TL->y - pt->y)) { 
	other->y=pt_BL->y;//in case of tie, keep bottom-side
      }
      else {
	other->y=pt_TL->y;
      }
    }


  }

  //6: pt isto the right of MBB in x, but is in the middle of MBB in y
  else if(pt_BR->x < pt->x &&(pt_BL->y < pt->y && pt->y <=pt_TL->y)) {
    dist= pt->x - pt_BR->x;
    other->x=pt_BR->x;
    other->y=pt->y;
  }

  //7: pt is to the left of MBB in x, but is to the top of MBB in y
  else if(pt->x <=pt_BL->x && pt_TL->y < pt->y) {
    dist= (pt_BL->x - pt->x) + (pt->y - pt_TL->y);
    *other=*pt_TL;
  }

  //8: pt is in the middle of MBB in x, but is to the top o fMBB in y
  else if((pt_BL->x < pt->x && pt->x <= pt_BR->x) && pt_TL->y < pt->y) {
    dist=pt->y - pt_TL->y;
    other->x=pt->x;
    other->y=pt_TL->y;
  }

  //9: pt is to the right of MBB in x, but is to the top o fMBB in y
  else if(pt_BR->x < pt->x && pt_TL->y < pt->y) {
    dist= (pt->x - pt_BR->x) +  (pt->y - pt_TL->y);
    *other=*pt_TR;
  }

  //error if reached here
  else {
    perror("error: in distPtMBB, invalid relative position");
    exit(-1);
  }


    free(pt_TR);
    free(pt_TL);
    free(pt_BR);
    free(pt_BL);
    return dist;
}


LL_t* pin_order(net* net1, int net_d) {

  int i;
  //int j;
  if(net1 == NULL) {
    perror("pin_order");
    exit(-1);
  }

  int num = net1->numPins;
  point* pins = net1->pins;

  point** set=(point**)malloc(num*sizeof(*set));

  node_LL_t** nodes=(node_LL_t**)malloc(num*sizeof(*nodes));
  LL_t* list=new_LL_t();

  for(i=0;i<num;i++) {
    set[i]=&pins[i];
    nodes[i]=(node_LL_t*)malloc(sizeof(*nodes[i]));
    nodes[i]->data=(void*)set[i];
    nodes[i]->next=NULL;
    LL_add_node(list,i,nodes[i]);
  }
  if(num<3 || !net_d) 		/* impl net_d */
    return list;

  pair pr;
  Closest_Pair(set,num,&pr);
  free(set);

  int  ns = 0;			// num items in sorted list
  LL_t* lsort=new_LL_t();
  // add 1st Two nodes to lsort

  node_LL_t** newnodes=(node_LL_t**)malloc(num*sizeof(*newnodes));
  node_LL_t* in;
  int numnew = 0;
  for(in=list->head; in != NULL; in = in->next) {
    if((point*)in->data == pr.p1 || (point*) in->data == pr.p2 ) {
      newnodes[numnew]=(node_LL_t*)malloc(sizeof(*newnodes[numnew]));
      *newnodes[numnew] = *in;
      //      if(lsort->last != NULL) lsort->last->next = newnodes[numnew];
      LL_add_node(lsort,ns++,newnodes[numnew++]);
      LL_remove_node(list,in);
    }
  }




  int nnew = 0; 		// keep track or new points created. (on MBB)
  while(list->numItems > 0){

    node_LL_t* jn = NULL;
    node_LL_t* kn = NULL;
    point* ip1 = (point*) malloc(sizeof(*ip1));
    // loop on points on MBB
    size_t dist=ULONG_MAX;
    size_t temp=ULONG_MAX;
    for(in=list->head;in!=NULL;in=in->next){
      temp = distPtMBB(&pr,(point*)in->data,ip1); // ip1 is the point on MBB
      if(temp<dist) {
	dist=temp;
	jn=in;			// jn is always from existing nodes in list
      }
    }
    // result in ip1, dist

    if(is_point_equal(*ip1,*pr.p1)) {
      newnodes[numnew]=(node_LL_t*)malloc(sizeof(*newnodes[numnew]));
      newnodes[numnew]->data = jn->data;
      LL_add_node(lsort,ns++,newnodes[numnew++]);
      // dont add node for ip1 because ip1 is already in lsort (the pair defining the MBB)
      pr.p2 = (point*)jn->data; // p1 no change
      //      LL_remove_node(list,kn);
      LL_remove_node(list,jn);
    }
    else if ( is_point_equal(*ip1,*pr.p2)) {
      newnodes[numnew]=(node_LL_t*)malloc(sizeof(*newnodes[numnew]));
      newnodes[numnew]->data = jn->data;
      LL_add_node(lsort,ns++,newnodes[numnew++]);
      // dont add node for ip1 because ip1 is already in lsort (the pair defining the MBB)
      pr.p1=(point*)jn->data;	// p2 no change
      //      LL_remove_node(list,kn);
      LL_remove_node(list,jn);
    }
    else {
      // ip1 is not a point in pr
      // ip1 might be a point in old list
      // not sure wh. ip1 can be in new lsort. if yes, duplicates point exist in lsort, cycle is resulted, may be ok to deal with this later.
      nnew++;
      //      kn = (node_LL_t*)realloc(kn,((nnew+1)*sizeof(*kn)));
      kn = (node_LL_t*)realloc(kn,((nnew)*sizeof(*kn)));
      kn[nnew-1].data = (void*) ip1;
      LL_add_node(lsort,ns++,&kn[nnew-1]);	// kn contains the point from MBB,	which coult be a point from current pair defining the MBB
      newnodes[numnew]=(node_LL_t*)malloc(sizeof(*newnodes[numnew]));
      newnodes[numnew]->data = jn->data;
      LL_add_node(lsort,ns++,newnodes[numnew++]);

      pr.p1=(point*)kn[nnew-1].data;
      pr.p2=(point*)jn->data;

      LL_remove_node(list,&kn[nnew-1]);	// kn may not in list, but ok
      LL_remove_node(list,jn);
    }

  }


  for(i=0;i<num;i++)
    free(nodes[i]);
  free(nodes);
  free(list);
  return lsort;
}

/* actually estimate Utils, not ofl, =>better result */
int estimate_ofl(point* start, point* end, routingInst* rst) {
  int	i;
  if(start == NULL || end == NULL || rst == NULL) {
    perror("estimate_ofl");
    exit(-1);
  }

  point*	ip = start;
  point*	jp = end;

  //  segment*	seg = (segment*)malloc(sizeof(*seg));
  //  seg->p1	    = *start;
  //  seg->p2	    = *end;
  //  int gx	    = rst->gx;
  int total_ofl = 0;

  int	num;
  // if in to jn is flat
  if(ip->y	  == jp->y) {
    num		  = max(ip->x, jp->x) - min(ip->x, jp->x);
    //seg->numEdges = num;
    //    seg->edges	  = (int*) malloc(num*sizeof(int));

    for(i					     = 0 ; i<num; i++) {
      point	p1,					p2;
      p1.x	       = (min(ip->x, jp->x) + i),	p1.y = ip->y;
      p2.x	       = (p1.x + 1),			p2.y = p1.y;
      int	edgeId = getEdgeId(&p1, &p2, rst); 
      //      seg->edges[i]    = edgeId;
      //      total_ofl += max(0, rst->edgeUtils[edgeId] -rst->edgeCaps[edgeId]) ;
      total_ofl +=  rst->edgeUtils[edgeId] ;
    }
  }
  else if(start->x == end->x) {
    num		  = max(ip->y, jp->y) - min(ip->y, jp->y);
    //    seg->numEdges = num;
    //    seg->edges	  = (int*) malloc(num*sizeof(int));

    for(i	       = 0 ; i<num; i++) {
      point	p1,					p2;
      p1.y	       = (min(ip->y, jp->y) + i),	p1.x = ip->x;
      p2.y	       = (p1.y + 1),			p2.x = p1.x;
      int	edgeId = getEdgeId(&p1, &p2,		rst); 
      //      seg->edges[i]    = edgeId;
      //      total_ofl += max(0, rst->edgeUtils[edgeId] -rst->edgeCaps[edgeId]) ;
      total_ofl +=  rst->edgeUtils[edgeId] ;
    }

  }
  else {
    perror("Error: not a flat route.");
    exit(-1);
  }

  return total_ofl;
}


// use two flat_route to create a L shape route
segment* flat_route(point* start, point* end, routingInst* rst) {
  int	i;
  if(start == NULL || end == NULL || rst == NULL) {
    perror("flat_route");
    exit(-1);
  }

  point*	ip = start;
  point*	jp = end;

  segment*	seg = (segment*)malloc(sizeof(*seg));
  seg->p1	    = *start;
  seg->p2	    = *end;
  //  int gx	    = rst->gx;

  int	num;
  // if in to jn is flat
  if(ip->y	  == jp->y) {
    num		  = max(ip->x, jp->x) - min(ip->x, jp->x);
    seg->numEdges = num;
    seg->edges	  = (int*) malloc(num*sizeof(int));

    for(i					     = 0 ; i<num; i++) {
      point	p1,					p2;
      p1.x	       = (min(ip->x, jp->x) + i),	p1.y = ip->y;
      p2.x	       = (p1.x + 1),			p2.y = p1.y;
      int	edgeId = getEdgeId(&p1, &p2, rst); 
      seg->edges[i]    = edgeId;
      rst->edgeUtils[edgeId] ++;
    }
  }
  else if(start->x == end->x) {
    num		  = max(ip->y, jp->y) - min(ip->y, jp->y);
    seg->numEdges = num;
    seg->edges	  = (int*) malloc(num*sizeof(int));

    for(i	       = 0 ; i<num; i++) {
      point	p1,					p2;
      p1.y	       = (min(ip->y, jp->y) + i),	p1.x = ip->x;
      p2.y	       = (p1.y + 1),			p2.x = p1.x;
      int	edgeId = getEdgeId(&p1, &p2,		rst); 
      seg->edges[i]    = edgeId;
      rst->edgeUtils[edgeId] ++;
    }

  }
  else {
    perror("Error: not a flat route.");
    exit(-1);
  }

  return seg;
}

/* imbeded in initial_routing_z */
/* segment* z_route(point* start, point* end, routingInst* rst) { */
/*   int	i; */
/*   if(start == NULL || end == NULL || rst == NULL) { */
/*     perror("z_route"); */
/*     exit(-1); */
/*   } */

/*   point*	ip = start; */
/*   point*	jp = end; */

/*   segment*	seg = (segment*)malloc(sizeof(*seg)); */
/*   seg->p1	    = *start; */
/*   seg->p2	    = *end; */
/*   int gx	    = rst->gx; */

/*   int num; */
/*   int xmin = min(start->x, end->x); */
/*   int xmax = max(start->x, end->x); */
/*   int ymin = min(start->y, end->y); */
/*   int ymax = max(start->y, end->y); */
/*   /\* stopped here *\/ */


/*   // if in to jn is flat */
/*   if(ip->y	  == jp->y) { */
/*     num		  = max(ip->x, jp->x) - min(ip->x, jp->x); */
/*     seg->numEdges = num; */
/*     seg->edges	  = (int*) malloc(num*sizeof(int)); */

/*     for(i					     = 0 ; i<num; i++) { */
/*       point	p1,					p2; */
/*       p1.x	       = (min(ip->x, jp->x) + i),	p1.y = ip->y; */
/*       p2.x	       = (p1.x + 1),			p2.y = p1.y; */
/*       int	edgeId = getEdgeId(&p1, &p2, rst); */
/*       seg->edges[i]    = edgeId; */
/*       rst->edgeUtils[edgeId] ++; */
/*     } */
/*   } */
/*   else if(start->x == end->x) { */
/*     num		  = max(ip->y, jp->y) - min(ip->y, jp->y); */
/*     seg->numEdges = num; */
/*     seg->edges	  = (int*) malloc(num*sizeof(int)); */

/*     for(i	       = 0 ; i<num; i++) { */
/*       point	p1,					p2; */
/*       p1.y	       = (min(ip->y, jp->y) + i),	p1.x = ip->x; */
/*       p2.y	       = (p1.y + 1),			p2.x = p1.x; */
/*       int	edgeId = getEdgeId(&p1, &p2,		rst); */
/*       seg->edges[i]    = edgeId; */
/*       rst->edgeUtils[edgeId] ++; */
/*     } */

/*   } */
/*   else { */
/*     perror("Error: not a flat route."); */
/*     exit(-1); */
/*   } */

/*   return seg; */
/* } */


int is_seg_equal(segment* seg1, point p1, point p2, routingInst* rst) {
  int result = 0;
  if( is_point_equal(seg1->p1, p1) && is_point_equal(seg1->p2, p2)) result = 1;
  else if( is_point_equal(seg1->p1 ,p2) && is_point_equal(seg1->p2, p1)) result = 1;
  else result = 0;
  return result;
}

int is_seg_contained(point p1, point p2, net* net1, routingInst* rst) {
 int result = 0;
 route* r = &net1->nroute;
 int num = r->numSegs;
 int i;
 for(i=0; i< num; i++) {
   if(is_seg_equal(&r->segments[i], p1, p2, rst)) {
     result = 1;
     break;
   }
 }

 return result;
}

int is_point_contained(point pt, net* net1, routingInst* rst) {
  int result = 0;
  int num = net1->numPins;
  int i;
  for(i=0; i<num; i++) {
    if(is_point_equal(pt, net1->pins[i])) {
      result =1;
      break;
    }
  }

  return result;
}

/* z route version */
int initial_routing_z(routingInst* rst, int net_d) {
  int i,j;
  if(rst == NULL) {
    perror("solveRouting");
    exit(-1);
  }

  int nnet = rst->numNets;
  for(i=0; i<nnet; i++ ) {
    net*	n      = &rst->nets[i];

    LL_t*	lorder = pin_order(n, net_d);


    route* r = &n->nroute;
    int nseg = lorder->numItems;

    r->segments = NULL;
    int cseg = 0;

    int ipin = 0;
    n->ordered_pins = (point*) malloc(lorder->numItems * sizeof(point));

    /* pre-process lorder to remove non-pin points */
    //    int pos_lorder = 0;
    node_LL_t* nd;
    for(nd = lorder->head; nd != NULL; nd = nd->next) {
      point pt = *(point*)nd->data;
      if(!is_point_contained(pt,n,rst)) {
	LL_remove_node(lorder, nd);
      }
      //pos_lorder++;
    }


    //    node_LL_t* nd;
    for(nd = lorder->head; nd->next != NULL; nd = nd->next) {

      point* p1 = (point*)nd->data;
      point* p2 = (point*)nd->next->data;

      n->ordered_pins[ipin++]=*p1; //update the orderd pins into nets
      n->ordered_pins[ipin]=*p2;   /* if changed to ipin++ here then fail */
      n->num_ordered_pins = ipin+1;

      if (p1->x == p2->x || p1->y == p2->y) {
	segment* s;
	if(!is_seg_contained(*p1,*p2,n, rst)) {
	  s = flat_route(p1,p2,rst);
	  cseg++;
	  r->segments =  (segment*)realloc(r->segments, cseg * sizeof(segment));
	  r->segments[cseg -1 ] = *s;
	  /* remove ofl */
	  r->numSegs = nseg = cseg;
	  free(s);
	}

	//	cseg += 1;
	//	r->segments =  (segment*)realloc(r->segments, cseg * sizeof(segment));
	//	r->segments[cseg -1 ] = *s;


      }
      else {
	point start = *p1;
	point end = *p2;

	//	int num;
	int xmin = min(p1->x, p2->x);
	int xmax = max(p1->x, p2->x);
	int ymin = min(p1->y, p2->y);
	int ymax = max(p1->y, p2->y);

	/* case 1 */
	/* point* mid_1 = (point*) malloc(sizeof(*mid_1)); */
	/* mid_1->x = xmin , mid_1->y = ymax; */
	/* int total_1 = INT_MAX; */
	/* int cost_1_seg1 = 0, cost_1_seg2 = 0; */
	/* cost_1_seg1 = estimate_ofl(p1, mid_1, rst); */
	/* cost_1_seg2 = estimate_ofl(mid_1, p2, rst); */

	/* total_1 = cost_1_seg1 + cost_1_seg2; */
	/* free(mid_1); */

	/* case 2 */
	/* point* mid_2 = (point*) malloc(sizeof(*mid_2)); */
	/* mid_2->x = xmax , mid_2->y = ymin; */
	/* int total_2 = INT_MAX; */
	/* int cost_2_seg1 = 0, cost_2_seg2 = 0; */
	/* cost_2_seg1 = estimate_ofl(p1, mid_2, rst); */
	/* cost_2_seg2 = estimate_ofl(mid_2, p2, rst); */

	/* total_2 = cost_2_seg1 + cost_2_seg2; */
	/* free(mid_2); */


	/* case 3 */
	/* Z-route horizontally */
	point pt_3_left = start.x < end.x ? start : end;
	point pt_3_right = is_point_equal(pt_3_left, start) ? end : start;
	int total_3 = INT_MAX, pos_3 = ymin;

	point* mid_3_left = (point*) malloc(sizeof(*mid_3_left));
	point* mid_3_right = (point*) malloc(sizeof(*mid_3_right));

	for(j = 0; j<ymax - ymin + 1; j++) {

	  mid_3_left->x = xmin , mid_3_left->y = ymin + j; /* correct */
	  mid_3_right->x = xmax , mid_3_right->y = ymin + j; /* correct */

	  int cost_3_seg1 = 0, cost_3_seg2 = 0, cost_3_seg3 = 0;
	  cost_3_seg1 = estimate_ofl(&pt_3_left, mid_3_left, rst);
	  cost_3_seg2 = estimate_ofl(mid_3_left, mid_3_right, rst);
	  cost_3_seg3 = estimate_ofl(mid_3_right, &pt_3_right, rst);
	  int temp = cost_3_seg1 + cost_3_seg2 + cost_3_seg3;
	  if(temp <= total_3) {
	    total_3 = temp;
	    pos_3 = ymin + j;
	  }

	}

	free(mid_3_left);
	free(mid_3_right);

	/* now we have total_3 and pos 3 */

	/* case 4 */
	/* Z-route vertically */
	point pt_4_bottom = start.y < end.y ? start : end;
	point pt_4_top = is_point_equal(pt_4_bottom, start) ? end : start;
	int total_4 = INT_MAX, pos_4 = xmin;
	
	point* mid_4_bottom = (point*) malloc(sizeof(*mid_4_bottom));
	point* mid_4_top = (point*) malloc(sizeof(*mid_4_top));

	for(j = 0; j<xmax - xmin + 1; j++) {

	  mid_4_bottom->y = ymin , mid_4_bottom->x = xmin + j; /* correct */
	  mid_4_top->y = ymax , mid_4_top->x = xmin + j; /* correct */

	  int cost_4_seg1 = 0, cost_4_seg2 = 0, cost_4_seg3 = 0;
	  cost_4_seg1 = estimate_ofl(&pt_4_bottom, mid_4_bottom, rst);
	  cost_4_seg2 = estimate_ofl(mid_4_bottom, mid_4_top, rst);
	  cost_4_seg3 = estimate_ofl(mid_4_top, &pt_4_top, rst);
	  int temp = cost_4_seg1 + cost_4_seg2 + cost_4_seg3;
	  if(temp <= total_4) {
	    total_4 = temp;
	    pos_4 = xmin + j;
	  }

	}
	free(mid_4_bottom);
	free(mid_4_top);
	/* total_4 and pos_4 is ready */

	/* actual routing */
	if(total_3 < total_4) {
	  /* two seg */
	  if(pos_3 == ymin || pos_3 == ymax) {
	    point* mid = (point*) malloc(sizeof(*mid));
	    mid->x = p1->x , mid->y = p2->y;
	    segment* s1, *s2;

	    if(!is_seg_contained(*p1,*mid,n, rst)) {
	      s1 = flat_route(p1, mid, rst);
	      cseg++;
	      r->segments =  (segment*)realloc(r->segments, cseg * sizeof(segment));
	      r->segments[cseg -1 ] = *s1;
	      r->numSegs = nseg = cseg;
	      free(s1);
	    }


	    if(!is_seg_contained(*mid,*p2,n, rst)) {
	      s2 = flat_route(mid, p2, rst);
	      cseg++;
	      r->segments =  (segment*)realloc(r->segments, cseg * sizeof(segment));
	      r->segments[cseg -1 ] = *s2;
	      r->numSegs = nseg = cseg;
	      free(s2);
	    }

	    //	    cseg += 2;
	    //	    r->segments =  (segment*)realloc(r->segments, cseg * sizeof(segment));
	    //	    r->segments[cseg -2 ] = *s1;
	    //	    r->segments[cseg -1] = *s2;

	    free(mid);


	  }
	  /* three segs */
	  else {
	    point pt_left = start.x < end.x ? start : end;
	    point pt_right = is_point_equal(pt_left, start) ? end : start;
	    point* mid_left = (point*) malloc(sizeof(*mid_left));
	    point* mid_right = (point*) malloc(sizeof(*mid_right));

	    mid_left->x = pt_left.x , mid_left->y = pos_3;
	    mid_right->x = pt_right.x , mid_right->y = pos_3;

	    segment* s1, *s2, *s3;

	    if(!is_seg_contained(pt_left,*mid_left,n, rst)) {
	      s1 = flat_route(&pt_left, mid_left, rst);
	      cseg++;
	      r->segments =  (segment*)realloc(r->segments, cseg * sizeof(segment));
	      r->segments[cseg -1 ] = *s1;
	      r->numSegs = nseg = cseg;
	      free(s1);
	    }


	    if(!is_seg_contained(*mid_left, *mid_right,n, rst)) {
	      s2 = flat_route(mid_left, mid_right, rst);
	      cseg++;
	      r->segments =  (segment*)realloc(r->segments, cseg * sizeof(segment));
	      r->segments[cseg -1 ] = *s2;
	      r->numSegs = nseg = cseg;
	      free(s2);
	    }


	    if(!is_seg_contained(*mid_right,pt_right,n, rst)) {
	      s3 = flat_route(mid_right, &pt_right, rst);
	      cseg++;
	      r->segments =  (segment*)realloc(r->segments, cseg * sizeof(segment));
	      r->segments[cseg -1 ] = *s3;
	      r->numSegs = nseg = cseg;
	      free(s3);
	    }

	    //	    cseg += 3;
	    //	    r->segments =  (segment*)realloc(r->segments, cseg * sizeof(segment));
	    //	    r->segments[cseg -3 ] = *s1;
	    //	    r->segments[cseg -2] = *s2;
	    //	    r->segments[cseg -1] = *s3;

	    free(mid_left);
	    free(mid_right);



	  } /* inner if-else */
	}   /* outer if-else */
	else {
	  /* two segs */
	  if(pos_4 == xmin || pos_4 == xmax) {
	    point* mid = (point*) malloc(sizeof(*mid));
	    mid->x = p1->x , mid->y = p2->y;
	    segment* s1, *s2;


	    if(!is_seg_contained(*p1,*mid,n, rst)) {
	      s1 = flat_route(p1, mid, rst);
	      cseg++;
	      r->segments =  (segment*)realloc(r->segments, cseg * sizeof(segment));
	      r->segments[cseg -1 ] = *s1;
	      r->numSegs = nseg = cseg;
	      free(s1);
	    }


	    if(!is_seg_contained(*mid,*p2,n, rst)) {
	      s2 = flat_route(mid, p2, rst);
	      cseg++;
	      r->segments =  (segment*)realloc(r->segments, cseg * sizeof(segment));
	      r->segments[cseg -1 ] = *s2;
	      r->numSegs = nseg = cseg;
	      free(s2);
	    }

	    //	    cseg += 2;
	    //	    r->segments =  (segment*)realloc(r->segments, cseg * sizeof(segment));
	    //	    r->segments[cseg -2 ] = *s1;
	    //	    r->segments[cseg -1] = *s2;

	    free(mid);


	  }
	  /* three segs */
	  else {
	    point pt_bottom = start.y < end.y ? start : end;
	    point pt_top = is_point_equal(pt_bottom, start) ? end : start;
	    point* mid_bottom = (point*) malloc(sizeof(*mid_bottom));
	    point* mid_top = (point*) malloc(sizeof(*mid_top));

	    mid_bottom->y = pt_bottom.y , mid_bottom->x = pos_4;
	    mid_top->y = pt_top.y , mid_top->x = pos_4;

	    segment* s1, *s2, *s3;

	    if(!is_seg_contained(pt_bottom, *mid_bottom,n, rst)) {
	      s1 = flat_route(&pt_bottom, mid_bottom, rst);
	      cseg++;
	      r->segments =  (segment*)realloc(r->segments, cseg * sizeof(segment));
	      r->segments[cseg -1 ] = *s1;
	      r->numSegs = nseg = cseg;
	      free(s1);
	    }


	    if(!is_seg_contained(*mid_bottom, *mid_top,n, rst)) {
	      s2 = flat_route(mid_bottom, mid_top, rst);
	      cseg++;
	      r->segments =  (segment*)realloc(r->segments, cseg * sizeof(segment));
	      r->segments[cseg -1 ] = *s2;
	      r->numSegs = nseg = cseg;
	      free(s2);
	    }


	    if(!is_seg_contained(*mid_top,pt_top,n, rst)) {
	      s3 = flat_route(mid_top, &pt_top, rst);
	      cseg++;
	      r->segments =  (segment*)realloc(r->segments, cseg * sizeof(segment));
	      r->segments[cseg -1 ] = *s3;
	      r->numSegs = nseg = cseg;
	      free(s3);
	    }

	    //	    cseg += 3;
	    //	    r->segments =  (segment*)realloc(r->segments, cseg * sizeof(segment));
	    //	    r->segments[cseg -3 ] = *s1;
	    //	    r->segments[cseg -2] = *s2;
	    //	    r->segments[cseg -1] = *s3;

	    free(mid_bottom);
	    free(mid_top);
	  } /* inner */
	}   /* outer */

      }

      //     free((point*)nd->data);	/* free point in lsort */
    }

    /* release lsort */
    //    node_LL_t * temp;
    /* for(nd = lorder->head; nd != NULL; ) { */
    /*   temp = nd->next; */
    /*   free(nd); */
    /*   nd = temp; */
    /* } */
    free(lorder);



    //       free((point*)nd->data); 	/* free the last point in lsort */

    // finalize size
  }
  return 1;
}

/* flat_route version */
int initial_routing_l(routingInst* rst, int net_d) {
  int i;
  if(rst == NULL) {
    perror("solveRouting");
    exit(-1);
  }

  int nnet = rst->numNets;
  for(i=0; i<nnet; i++ ) {
    net*	n      = &rst->nets[i];
    LL_t*	lorder = pin_order(n, net_d);

    route* r = &n->nroute;
    int nseg = lorder->numItems;

    r->segments = NULL;
    int cseg = 0;

    int ipin = 0;
    n->ordered_pins = (point*) malloc(lorder->numItems * sizeof(point));

    node_LL_t* nd;
    for(nd = lorder->head; nd->next != NULL; nd = nd->next) {

      point* p1 = (point*)nd->data;
      point* p2 = (point*)nd->next->data;

      n->ordered_pins[ipin++]=*p1; //update the orderd pins into nets
      n->ordered_pins[ipin]=*p2;   /* if changed to ipin++ here then fail */
      n->num_ordered_pins = ipin+1;



      if (p1->x == p2->x || p1->y == p2->y) {
	segment* s;
	s = flat_route(p1,p2,rst);
	cseg += 1;
	r->segments =  (segment*)realloc(r->segments, cseg * sizeof(segment));
	r->segments[cseg -1 ] = *s;

	free(s);
      }
      else {
	point* mid = (point*) malloc(sizeof(*mid));
	mid->x = p1->x , mid->y = p2->y;
	segment* s1, *s2;
	s1 = flat_route(p1, mid, rst);
	s2 = flat_route(mid, p2, rst);
	cseg += 2;
	r->segments =  (segment*)realloc(r->segments, cseg * sizeof(segment));
	r->segments[cseg -2 ] = *s1;
	r->segments[cseg -1] = *s2;

	free(mid);
	free(s1);
	free(s2);
      }

      //     free((point*)nd->data);	/* free point in lsort */
    }

    /* release lsort */
    //    node_LL_t * temp;
    /* for(nd = lorder->head; nd != NULL; ) { */
    /*   temp = nd->next; */
    /*   free(nd); */
    /*   nd = temp; */
    /* } */
    free(lorder);
    r->numSegs = nseg = cseg;


    //       free((point*)nd->data); 	/* free the last point in lsort */

    // finalize size
  }
  return 1;
}


int get_id_of_point (point pt, routingInst* rst) {
  int x = pt.x;
  int y = pt.y;
  return x % rst->gx + y * rst->gx;
}

point get_point_of_id (int id, routingInst* rst) {
  point pt;
  pt.x = id % rst->gx;
  pt.y = id / rst->gy;
  return pt;
}

LL_t* getNeighbors(point *center, int xmin, int xmax, int ymin, int ymax, routingInst* rst) {

  if(center->x <0 || center->x>rst->gx || center->y<0 || center->y>rst->gy|| rst == NULL) {
    perror("getNeighbors");
    exit(-1);
  }

  LL_t* neighbors = new_LL_t();

  int gx = rst->gx, gy = rst->gy;
  int x = center->x, y = center->y;

  int count = 0;

  if ( y + 1 <= min((gy - 1),ymax) ) {
    point* up = (point*) malloc (sizeof(point));
    up->x = x, up->y = y + 1;
    node_LL_t* node_up = new_node_LL_t();
    node_up->data = (void*)up;
    LL_add_node(neighbors, count++, node_up);
  }

  if ( x - 1 >= max(0, xmin) ) {
    point* left = (point*) malloc (sizeof(point));
    left->x = x - 1, left->y = y;
    node_LL_t* node_left = new_node_LL_t();
    node_left->data = (void*)left;
    LL_add_node(neighbors, count++, node_left);
  }

  if ( y - 1 >= max(0, ymin) ) {
    point* down = (point*) malloc (sizeof(point));
    down->x = x, down->y = y - 1;
    node_LL_t* node_down = new_node_LL_t();
    node_down->data = (void*)down;
    LL_add_node(neighbors, count++, node_down);
  }

  if ( x + 1 <= min((gx -1), xmax)) {
    point* right = (point*) malloc (sizeof(point));
    right->x = x + 1, right->y = y;
    node_LL_t* node_right = new_node_LL_t();
    node_right->data = (void*)right;
    LL_add_node(neighbors, count++, node_right);
  }

  return neighbors;
}


/* LL_t* getNeighbors(point *center, routingInst* rst) { */

/*   if(center->x <0 || center->x>rst->gx || center->y<0 || center->y>rst->gy|| rst == NULL) { */
/*     perror("getNeighbors"); */
/*     exit(-1); */
/*   } */

/*   LL_t* neighbors = new_LL_t(); */

/*   int gx = rst->gx, gy = rst->gy; */
/*   int x = center->x, y = center->y; */

/*   int count = 0; */

/*   if ( y + 1 <= gy - 1 ) { */
/*     point* up = (point*) malloc (sizeof(point)); */
/*     up->x = x, up->y = y + 1; */
/*     node_LL_t* node_up = new_node_LL_t(); */
/*     node_up->data = (void*)up; */
/*     LL_add_node(neighbors, count++, node_up); */
/*   } */

/*   if ( x - 1 >= 0 ) { */
/*     point* left = (point*) malloc (sizeof(point)); */
/*     left->x = x - 1, left->y = y; */
/*     node_LL_t* node_left = new_node_LL_t(); */
/*     node_left->data = (void*)left; */
/*     LL_add_node(neighbors, count++, node_left); */
/*   } */

/*   if ( y - 1 >= 0 ) { */
/*     point* down = (point*) malloc (sizeof(point)); */
/*     down->x = x, down->y = y - 1; */
/*     node_LL_t* node_down = new_node_LL_t(); */
/*     node_down->data = (void*)down; */
/*     LL_add_node(neighbors, count++, node_down); */
/*   } */

/*   if ( x + 1 <= gx -1 ) { */
/*     point* right = (point*) malloc (sizeof(point)); */
/*     right->x = x + 1, right->y = y; */
/*     node_LL_t* node_right = new_node_LL_t(); */
/*     node_right->data = (void*)right; */
/*     LL_add_node(neighbors, count++, node_right); */
/*   } */

/*   return neighbors; */
/* } */


/* int getNeighbors(point center, point** ptr_neighbors, routingInst* rst) { */

/*   point* neighbors=NULL; */

/*   if(center.x <0 || center.x>rst->gx || center.y<0 || center.y>rst->gy|| rst == NULL) { */
/*     perror("getNeighbors"); */
/*     exit(-1); */
/*   } */

/*   /\* if edge cap && not boundary *\/ */
/*   /\* then neighbor exists *\/ */
/*   int gx = rst->gx, gy = rst->gy; */
/*   int x = center.x, y = center.y; */

/*   int count = 0; */
/*   neighbors = NULL; */
/*   if ( y + 1 <= gy ) { */
/*     point up; */
/*     up.x = x, up.y = y + 1; */
/*     int edgeId =  getEdgeId( &center, &up, rst); */
/*     //    int ofl = rst->edgeUtils[edgeId] - rst->edgeCaps[edgeId]; */
/*     //    if(ofl < 0) { */
/*       neighbors = (point*) realloc(neighbors, (++count)*sizeof(*neighbors)); */
/*       neighbors[count-1] = up; */

/*       //    } */
/*   } */

/*   if ( x - 1 >= 0 ) { */
/*     point left; */
/*     left.x = x - 1, left.y = y; */
/*     int edgeId =  getEdgeId( &center, &left, rst); */
/*     //    int ofl = rst->edgeUtils[edgeId] - rst->edgeCaps[edgeId]; */
/*     //    if(ofl < 0) { */
/*       neighbors = (point*) realloc(neighbors, (++count)*sizeof(*neighbors)); */
/*       neighbors[count-1] = left; */
/*       //    } */
/*   } */

/*   if ( y - 1 >= 0 ) { */
/*     point down; */
/*     down.x = x, down.y = y - 1; */
/*     int edgeId =  getEdgeId( &center, &down, rst); */
/*     //    int ofl = rst->edgeUtils[edgeId] - rst->edgeCaps[edgeId]; */
/*     //    if(ofl < 0) { */
/*       neighbors = (point*) realloc(neighbors, (++count)*sizeof(*neighbors)); */
/*       neighbors[count-1] = down; */
/*       //    } */
/*   } */

/*   if ( x + 1 <= gx ) { */
/*     point right; */
/*     right.x = x + 1, right.y = y; */
/*     int edgeId =  getEdgeId( &center, &right, rst); */
/*     //    int ofl = rst->edgeUtils[edgeId] - rst->edgeCaps[edgeId]; */
/*     //    if(ofl < 0) { */
/*       neighbors = (point*) realloc(neighbors, (++count)*sizeof(*neighbors)); */
/*       neighbors[count-1] = right; */
/*       //    } */
/*   } */
/*   *ptr_neighbors = neighbors; */

/*   return count; */
/* } */

typedef struct __node_Fringe_t {
  //  point this;
  int this_id;
  point this_point;
  int cost;
  int group;
  int lazy_counter; 		/* for implementing lazy removal scheme */
  struct __node_Fringe_t* parent;
} node_Fringe_t;


/* min PQ */
int compar_node_Fringe_t(const void* a, const void* b) {
  int aa=*(int*)((node_PQ_t*)a)->key;
  int bb=*(int*)((node_PQ_t*)b)->key;
  if(aa<bb) return -1;
  else if(aa == bb) return 0;
  else return 1;
}

segment* a_star(point s, point t, int* nsegs, int frame_margin_width, routingInst* rst) {
  int i;
  if(rst == NULL) {
    perror("a_star");
    exit(-1);
  }

  /* framing */
  int xmin = max(min(s.x, t.x) - frame_margin_width, 0);
  int xmax = min(max(s.x, t.x) + frame_margin_width, (rst->gx - 1));
  int ymin = max(min(s.y, t.y) - frame_margin_width, 0);
  int ymax = min(max(s.y, t.y) + frame_margin_width, (rst->gy - 1));
  //  int NUM_POINT = rst->gx * rst->gy;
  int NUM_POINT = (xmax - xmin + 1) * (ymax - ymin + 1);
  point ll_corner;
  ll_corner.x = xmin, ll_corner.y = ymin;
  int ll_corner_id = get_id_of_point(ll_corner, rst);
  int width = xmax - xmin +1;

  node_Fringe_t* nodes = (node_Fringe_t*) malloc(NUM_POINT*sizeof(node_Fringe_t));
  for(i=0; i<NUM_POINT; i++) {
    /* init to group 1 */

    int pointId = i % (xmax - xmin + 1) + (i / (xmax - xmin + 1)) * rst->gx + ll_corner_id; /* global id */
    nodes[i].this_id = pointId;	/* global id */
    nodes[i].this_point = get_point_of_id(pointId,rst); /* global point */
    nodes[i].cost = INT_MAX;	/* cost from this node to s */
    nodes[i].group = 1; 	/* use this field to track and test group of node */
    nodes[i].lazy_counter = 0;
    nodes[i].parent = NULL;
  }

  /* init for starting node s */
  int s_id_global = get_id_of_point(s,rst);

  int s_id_local = ((s_id_global - ll_corner_id) % rst->gx) % width + ((s_id_global - ll_corner_id) / rst->gx) * width;
  //  node_Fringe_t* s_node = &nodes[s_id];
  node_Fringe_t* s_node = &nodes[s_id_local];
  s_node->this_point = get_point_of_id(s_node->this_id,rst);
  s_node->group = 3;
  /* A-Star alg */
  s_node->cost = getManhattanDist(&s, &t);

  /* denote t_node */
  int t_id_global = get_id_of_point(t,rst);
  int t_id_local = ((t_id_global - ll_corner_id) % rst->gx) % width + ((t_id_global - ll_corner_id) / rst->gx) * width;
  //  node_Fringe_t* t_node = &nodes[t_id];
  node_Fringe_t* t_node = &nodes[t_id_local];

  node_Fringe_t* curr_node = s_node;

  /* group 2, impl as PQ */
  /* non-indexable */
   PQ_t* group_2 = new_PQ_t(0,compar_node_Fringe_t);

  /* group 3 is implemented using the digit in node_Fringe_t */

  while (curr_node != t_node) {
    /* get neighbors */
    point curr_point = curr_node->this_point;
    LL_t* neighbors = getNeighbors(&curr_point, xmin, xmax, ymin, ymax, rst);

    node_LL_t* nd;
    for(nd = neighbors->head; nd != NULL; nd = nd->next) {

      int node_id_global = get_id_of_point(*(point*)nd->data,rst);
      int node_id_local = ((node_id_global - ll_corner_id) % rst->gx) % width + ((node_id_global - ll_corner_id) / rst->gx) * width;
      //      node_Fringe_t* this_node = &nodes[node_id];
      node_Fringe_t* this_node = &nodes[node_id_local];
      free((point*)nd->data);

      //      int pointId = this_node->this;
      point this_point = this_node->this_point;
      if (this_node->group == 3) continue;

      /* Dijkstra Alg */
      //      int w_curr_to_this = 1;

      /* A-star */
      int w_curr_to_this = 1 + getManhattanDist(&this_point,&t);
      int edgeId = getEdgeId(&curr_point, &this_point, rst);
      int ofl = rst->edgeUtils[edgeId] - rst->edgeCaps[edgeId];
      if(ofl > 0) w_curr_to_this += ofl; /* ofl adds cost */

      int trial_cost = curr_node->cost + w_curr_to_this ; 

      /* if this_node is in group1 */
      if (this_node->group == 1 ) {
	/* update fringe node info */
	this_node->group = 2;
	this_node->cost = trial_cost;
	this_node->parent = curr_node;

	/* wrap fringe node to PQ_node */
	node_PQ_t* this_node_PQ = new_node_PQ_t();
	this_node_PQ->data = (void*) this_node;
	this_node_PQ->key = (void*) &this_node->cost;
	PQ_add_node(group_2, this_node_PQ);
      }
      /* if this_node is in group 2 && smaller cost */
      /* ??? if an cost in group 2 is changed then how to re-order it after the value is changed? */
      /* solution: use lazy scheme, allowing for duplicates to exist in the heap */
      else if (trial_cost < this_node->cost) {
	this_node->cost = trial_cost;
	this_node->parent = curr_node;

	/* wrap fringe node to PQ_node */
	node_PQ_t* this_node_PQ = new_node_PQ_t();

	this_node_PQ->data = (void*) this_node;
	this_node_PQ->key = (void*) &this_node->cost;
	PQ_add_node(group_2, this_node_PQ);
      }

    }

    /* ensure only pick-up a non-duplicated net */
    do {
      node_PQ_t* curr_node_PQ = NULL;
      curr_node_PQ = PQ_remove_node(group_2);
      if (curr_node_PQ == NULL) return NULL; //failed to find new maze route, due to lack of reachable neighbors.
      curr_node = (node_Fringe_t*) curr_node_PQ->data;
      free(curr_node_PQ);
    } 
    while(curr_node->lazy_counter > 0);
    curr_node->lazy_counter = 1; /* the successfully dequed node is marked as visited */
    curr_node->group = 3;

    /* release the list */
    node_LL_t* temp;
    for(nd = neighbors->head; nd != NULL;) {
      temp = nd->next;
      free(nd);
      nd = temp;
    }
    free(neighbors);
  }


  /* back-trace */
  int pos = 0;
  LL_t* path = new_LL_t();
  while(curr_node != s_node) {
    /* wrap into LL node */
    node_LL_t* curr_node_LL = new_node_LL_t();
    curr_node_LL->data = (void*) curr_node;
    LL_add_node(path,pos++ ,curr_node_LL);

    curr_node = curr_node->parent;
  }
  /* add s_node */
  node_LL_t* curr_node_LL = new_node_LL_t();
  curr_node_LL->data = (void*) curr_node;
  LL_add_node(path,pos++ ,curr_node_LL);


  /* construct new segments */
  segment* segs = NULL;
  int cseg = 0;

  /* TODO: optimize below code */
  node_LL_t* nd;
  for(nd = path->head; nd->next != NULL; nd = nd->next) {

    node_Fringe_t* ndf1 = (node_Fringe_t*)nd->data;
    node_Fringe_t* ndf2 = (node_Fringe_t*)nd->next->data;

    point p1 = ndf1->this_point;
    point p2 = ndf2->this_point;

    if (p1.x == p2.x || p1.y == p2.y) {
      segment* s;
      s = flat_route(&p1,&p2,rst);
      cseg += 1;
      segs =  (segment*)realloc(segs, cseg * sizeof(segment));
      segs[cseg -1 ] = *s;
      free(s);
    }
    else {
      point mid;
      mid.x = p1.x , mid.y = p2.y;
      segment* s1, *s2;
      s1 = flat_route(&p1, &mid, rst);
      s2 = flat_route(&mid, &p2, rst);
      cseg += 2;
      segs =  (segment*)realloc(segs, cseg * sizeof(segment));
      segs[cseg -2 ] = *s1;
      segs[cseg -1] = *s2;
      free(s1);
      free(s2);
    }

    //    free((node_PQ_t*)nd->data);
  }
  //    r->numSegs = cseg;
  *nsegs = cseg;

  /* free path */
  node_LL_t *nd_ll, *temp;
  for(nd_ll = path->head; nd_ll != NULL; ) {
    temp = nd_ll->next;
    free(nd_ll);
    nd_ll = temp;
  }
  free(path);

  /* free PQ */
  node_PQ_t* nd_pq;
  /* maybe more efficient */
  for(i=0; i<(int)group_2->numItems; i++) {
    nd_pq = group_2->heap[1 + i];
    free(nd_pq);
  }
  free(group_2->heap);
  free(group_2);


  free(nodes);
  return segs;
}

/* max PQ */
int compar_net(const void* a, const void* b) {
  int aa=*(int*)((node_PQ_t*)a)->key;
  int bb=*(int*)((node_PQ_t*)b)->key;
  if(aa<bb) return 1;
  else if(aa == bb) return 0;
  else return -1;
}



int rip_up(routingInst* rst, time_t end_time_limit) {
  int i,j,l;
  int gx = rst->gx, gy = rst->gy;
  int frame_marge_width = 10;

  int nnet = rst->numNets;

  /* edge */
  int EDGE_NUM = (gx - 1)* gy + gx * (gy-1);
  int edge_ofl[EDGE_NUM];	/* ofl */
  int edge_hist[EDGE_NUM];	/* history */
  int edge_weight[EDGE_NUM];

  /* PQ for net ordering */
  PQ_t* pq = new_PQ_t((size_t) nnet, compar_net);
  node_PQ_t* nodes = (node_PQ_t*) calloc(nnet, sizeof(*nodes)); /* holding actual data info */


  /* rip-up loop */
  //  int nround = 1;
  int k = 0;
  int terminate_flag = 0;
  //  unsigned long secs = 900;	/* set timer */
  //  unsigned long retTime = time(0) + secs;     
  while (!terminate_flag && time(0) < end_time_limit) {

    /* if initial solution, construct initial PQ */
    if (k == 0) {

      /* init edge_hist */
      for(i=0; i< EDGE_NUM; i++) {
	edge_hist[i] = 0;
	edge_ofl[i] = 0;
      }
      /* udpate initial edge weight */
      /* calc ofl for iter. k-1 */
      for(i=0; i< EDGE_NUM; i++) {
	if(edge_ofl[i] > 0) 
	  edge_hist[i] ++;
	edge_weight[i] = edge_hist[i] * edge_ofl[i];
	edge_ofl[i] = max (rst->edgeUtils[i] - rst->edgeCaps[i], 0);
      }


      /* calculate initial net ordering */
      for(i=0; i< nnet; i++) {

	net* n = &rst->nets[i];
	route* r = &rst->nets[i].nroute;
	int nseg = n->nroute.numSegs;
	n->weight = 0;
	for(j=0; j< nseg; j++) {
	  segment* s = &r->segments[j];
	  int nedge = s->numEdges;
	  for(l=0; l<nedge; l++) {
	    int edgeId = s->edges[l];
	    n->weight += edge_weight[edgeId];
	  } /* done for 1 seg */
	}	/* done for all segs of one net */
	nodes[i].data = (void*) n;	 /* pointer to net */
	
	nodes[i].key = (void*) &n->weight; /* pointer to int */

	PQ_add_node(pq, &nodes[i]); /* now pq contains all nodes */
      }
    }
    /* use pq */
    else {
      /* pick the net with max weight*/
      node_PQ_t* nd = PQ_remove_node(pq);
      net* npick = (net*)nd->data;
      route* r = &npick->nroute;

      /* eleminate the current routing */
      int nsegs = r->numSegs;
      for(i=0; i<nsegs; i++) {
	segment* seg = &r->segments[i];
	int nedges = seg->numEdges;
	for(j=0; j<nedges; j++) {
	  int edgeId = seg->edges[j];
	  rst->edgeUtils[edgeId] --;
	}
	free(seg->edges);
      }
      free(r->segments);

      /* construct new segments */
      segment* snew = NULL;
      int* ptr_nsegs_astar = (int*) malloc(sizeof(int));
      int snew_total = 0;
      for(i=0; i<npick->num_ordered_pins-1; i++) {
	point p1 = npick->ordered_pins[i];
	point p2 = npick->ordered_pins[i+1];
	if(is_point_equal(p1,p2)) continue;
	*ptr_nsegs_astar = 0;

	segment* astar_segs = a_star(p1,p2, ptr_nsegs_astar, frame_marge_width,rst);

	if(astar_segs != NULL) {
	  int num_astar = *ptr_nsegs_astar;
	  snew_total += num_astar;
	  snew = (segment*) realloc(snew, snew_total * sizeof(*snew));
	  for(j= snew_total - num_astar; j< snew_total; j++)
	    snew[j] = astar_segs[j - (snew_total - num_astar)];
	  free(astar_segs);
	}
	else {
	  perror("rip_up");
	  exit(-1);
	} 
      }	  /* end for */
      r->segments = snew;
      r->numSegs = snew_total;
      free(ptr_nsegs_astar);

      /* update edge weight, ovl, hist */
      for(i=0; i< EDGE_NUM; i++) {
	if(edge_ofl[i] > 0) edge_hist[i] ++;
	edge_weight[i] = edge_hist[i] * edge_ofl[i];
	edge_ofl[i] = max (rst->edgeUtils[i] - rst->edgeCaps[i], 0);
      }
    }
    k++;
    //    if(k>nround * rst->numNets) terminate_flag = 1; /* use rounds of nets as termination flag */
    /* termination condition */

    if(pq->numItems < 1) {
      //      terminate_flag = 1; 
      k = 0;
    }
  }			
  /* end while loop */

  free(nodes);
  return 1;
}


int rip_up_no_order(routingInst* rst, time_t end_time_limit) {
  int i,j;//,l;
  //  int gx = rst->gx, gy = rst->gy;
  int frame_marge_width = 10;

  int nnet = rst->numNets;

  /* edge */
  //  int EDGE_NUM = (gx - 1)* gy + gx * (gy-1);
  //  int edge_ofl[EDGE_NUM];	/* ofl */
  //  int edge_hist[EDGE_NUM];	/* history */
  //  int edge_weight[EDGE_NUM];

  /* PQ for net ordering */
  //  PQ_t* pq = new_PQ_t((size_t) nnet, compar_net);
  node_PQ_t* nodes = (node_PQ_t*) calloc(nnet, sizeof(*nodes)); /* holding actual data info */
  int zero_weight = 0;

  /* rip-up loop */
  //  int nround = 1;
  int k = 0;
  int terminate_flag = 0;
  //  unsigned long secs = 900;	/* set timer */
  //  unsigned long retTime = time(0) + secs;     
  while (!terminate_flag && time(0) < end_time_limit) {

    /* if initial solution, construct initial PQ */
    if (k == 0) {

      /* init edge_hist */
      //      for(i=0; i< EDGE_NUM; i++) {
	//	edge_hist[i] = 0;
      //	edge_ofl[i] = 0;
	//      }
      /* udpate initial edge weight */
      /* calc ofl for iter. k-1 */
	//      for(i=0; i< EDGE_NUM; i++) {
	//	if(edge_ofl[i] > 0) 
	//edge_hist[i] ++;
	//	edge_weight[i] = edge_hist[i] * edge_ofl[i];
	//	edge_ofl[i] = max (rst->edgeUtils[i] - rst->edgeCaps[i], 0);
	//      }


      /* calculate initial net ordering */
      for(i=0; i< nnet; i++) {

	net* n = &rst->nets[i];
	//route* r = &rst->nets[i].nroute;
	//int nseg = n->nroute.numSegs;
	n->weight = 0;
	//	for(j=0; j< nseg; j++) {
	  //	  segment* s = &r->segments[j];
	  //	  int nedge = s->numEdges;
	  //	  for(l=0; l<nedge; l++) {
	    //	    int edgeId = s->edges[l];
	    //  n->weight += edge_weight[edgeId];
	  //	  } /* done for 1 seg */
	//	}	/* done for all segs of one net */
	nodes[i].data = (void*) n;	 /* pointer to net */
	
	//	nodes[i].key = (void*) &n->weight; /* pointer to int */
	nodes[i].key = (void*) &zero_weight; /* pointer to int */

	//	PQ_add_node(pq, &nodes[i]); /* now pq contains all nodes */
      }
    }
    /* use pq */
    else {
      /* pick the net with max weight*/
      //      node_PQ_t* nd = PQ_remove_node(pq);
      for(i=0; i<nnet; i++) {
	node_PQ_t* nd = &nodes[i];
	net* npick = (net*)nd->data;
	route* r = &npick->nroute;

	/* eleminate the current routing */
	int nsegs = r->numSegs;
	for(i=0; i<nsegs; i++) {
	  segment* seg = &r->segments[i];
	  int nedges = seg->numEdges;
	  for(j=0; j<nedges; j++) {
	    int edgeId = seg->edges[j];
	    rst->edgeUtils[edgeId] --;
	  }
	  free(seg->edges);
	}
	free(r->segments);

	/* construct new segments */
	segment* snew = NULL;
	int* ptr_nsegs_astar = (int*) malloc(sizeof(int));
	int snew_total = 0;
	for(i=0; i<npick->num_ordered_pins-1; i++) {
	  point p1 = npick->ordered_pins[i];
	  point p2 = npick->ordered_pins[i+1];
	  if(is_point_equal(p1,p2)) continue;
	  *ptr_nsegs_astar = 0;

	  segment* astar_segs = a_star(p1,p2, ptr_nsegs_astar, frame_marge_width,rst);

	  if(astar_segs != NULL) {
	    int num_astar = *ptr_nsegs_astar;
	    snew_total += num_astar;
	    snew = (segment*) realloc(snew, snew_total * sizeof(*snew));
	    for(j= snew_total - num_astar; j< snew_total; j++)
	      snew[j] = astar_segs[j - (snew_total - num_astar)];
	    free(astar_segs);
	  }
	  else {
	    perror("rip_up_no_order");
	    exit(-1);
	  } 
	}	  /* end for */
	r->segments = snew;
	r->numSegs = snew_total;
	free(ptr_nsegs_astar);

	/* update edge weight, ovl, hist */
	//	for(i=0; i< EDGE_NUM; i++) {
	  //	  if(edge_ofl[i] > 0) edge_hist[i] ++;
	  //	  edge_weight[i] = edge_hist[i] * edge_ofl[i];
	  //edge_ofl[i] = max (rst->edgeUtils[i] - rst->edgeCaps[i], 0);
	//	}
      }
    }
    k++;
    //    if(k>nround * rst->numNets) terminate_flag = 1; /* use rounds of nets as termination flag */
    /* termination condition */

    //    if(pq->numItems < 1) {
    if(i==nnet - 1) {
      //      terminate_flag = 1; 
      k = 0;
    }
  }			
  /* end while loop */

  free(nodes);
  return 1;
}


int solveRouting(routingInst* rst, time_t end_time_limit, int net_d, int net_o) {

  //  initial_routing_l(rst);
  initial_routing_z(rst, net_d);


  if(net_o)
    rip_up(rst, end_time_limit);
  else if(net_d)
    rip_up_no_order(rst, end_time_limit);
  // special case: -d=0 -n=0, no rip_up at all
  else
    ;
  return 1;
}



int writeOutput(const char* outRouteFile, routingInst* rst) {
  // read input file
  FILE* fp;
  fp = fopen(outRouteFile, "w");
  if (fp == NULL) {
    fclose(fp);
    return 0;
  }

  int i = 0;
  int j = 0;
  for (i = 0; i < rst->numNets; i++) {
    fprintf(fp, "n%d\n", rst->nets[i].id);
    for (j = 0; j < rst->nets[i].nroute.numSegs; j++) {
      segment seg = rst->nets[i].nroute.segments[j];
      fprintf(fp, "(%d,%d)-", seg.p1.x, seg.p1.y);
      fprintf(fp, "(%d,%d)\n", seg.p2.x, seg.p2.y);
    }
    fprintf(fp, "!\n");
  }

  // file is closed
  fclose(fp);
  return 1;
}

int release(routingInst *rst) {
  int i = 0;
  int j = 0;
  for (i = 0; i < rst->numNets; i++) {
    for (j = 0; j < rst->nets[i].nroute.numSegs; j++) {
      segment seg = rst->nets[i].nroute.segments[j];
      free(seg.edges);//free edges

    }

    if (rst->nets[i].nroute.numSegs > 0) 
      free(rst->nets[i].nroute.segments);
    // free(rst->net[i]->nroute);	// free routes
    rst->nets[i].pins->x = 0;
    rst->nets[i].pins->y = 0;
    free(rst->nets[i].pins);		// free pin;
    free(rst->nets[i].ordered_pins); //free ordered pins
  }

  free(rst->nets);//free nets;
  free(rst->edgeCaps);//free filed in routing instance;
  free(rst->edgeUtils);
  free(rst);//free routing isntance;

  return 1;
}


// int main(int argc, char **argv)
// {

//   if(!(argc ==5)){
//     printf("Usage : ./ROUTE.exe <input_benchmark_name> <output_file_name> -d=<0 or 1>, -n=<0 or 1> \n");
//     return 1;
//   }

//   int net_d = 0;
//   int net_o = 0;

//   //  unsigned long start_time = time(0);
//   //  time_t* start = (time_t*) malloc(sizeof(*start));
//   //  time(start);
//   time_t start = time(0);
//   //  printf("\n Start time is: %ld", start);

//   time_t end_time_limit = start + 180;
//   //  time_t end_time_limit = start + 5;

//   int status;
//   char *inputFileName = argv[1];
//   char *outputFileName = argv[2];
//   char* d = argv[3];
//   char* n = argv[4];


//   if(!(d[3]=='0' || d[3] == '1') && !(n[3] =='0' || n[3] == '1') ){
//     printf("Usage : ./ROUTE.exe <input_benchmark_name> <output_file_name> -d=<0 or 1>, -n=<0 or 1> \n");
//     return 1;
//   }

//   net_d = atoi(&d[3]);
//   net_o = atoi(&n[3]);


//   /// create a new routing instance
//   //  routingInst *rst = new routingInst;
//   routingInst *rst = (routingInst*) malloc(sizeof(*rst));
	
//   /// read benchmark
//   status = readBenchmark(inputFileName, rst);
//   if(status==0){
//     printf("ERROR: reading input file \n");
//     return 1;
//   }
	
//   /// run actual routing
//   status = solveRouting(rst, end_time_limit, net_d, net_o);
//   if(status==0){
//     printf("ERROR: running routing \n");
//     release(rst);
//     return 1;
//   }
	
//   /// write the result
//   status = writeOutput(outputFileName, rst);
//   if(status==0){
//     printf("ERROR: writing the result \n");
//     release(rst);
//     return 1;
//   }

//   release(rst);

//   printf("\nDONE!\n");	
//   //  unsigned long end_time = time(0);
//   time_t end = time(0);
//   //  printf("\n End time is: %ld", end);

//   double time_spent = difftime(end, start);
//   time_spent /= 60.0;

//   //  double total_time = (end_time - start_time) / 60.0;
//   printf("\n Total time is: %.3lf (min)\n", time_spent);

//   return 0;
// }
