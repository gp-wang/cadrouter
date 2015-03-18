// ECE556 - Copyright 2014 University of Wisconsin-Madison.  All Rights Reserved.


#ifndef ECE556_H
#define ECE556_H

// ECE556 - Copyright 2014 University of Wisconsin-Madison.  All Rights Reserved.
// C Libraries
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <assert.h>
#include <time.h>


//#include "gwlibc.h"

 /**
  * A structure to represent a 2D Point. 
  */
 typedef struct
 {
   int x ; /* x coordinate ( >=0 in the routing grid)*/
   int y ; /* y coordinate ( >=0 in the routing grid)*/

 } point ;


  /**
  * A structure to represent a segment
  */
 typedef struct
 {
   point p1 ; 	/* start point of a segment */
   point p2 ; 	/* end point of a segment */
   
   int numEdges ; 	/* number of edges in the segment*/
   int *edges ;  	/* array of edges representing the segment*/
   
 } segment ;
 
 
  /**
  * A structure to represent a route
  */
  typedef struct
  {
    int numSegs ;  	/* number of segments in a route*/
    segment *segments ;  /* an array of segments (note, a segment may be flat, L-shaped or any other shape, based on your preference */

  } route ;
 
 
  /**
  * A structure to represent nets
  */
  typedef struct
  {

   int id ; 		/* ID of the net */
   int numPins ; 		/* number of pins (or terminals) of the net */
   point *pins ; 		/* array of pins (or terminals) of the net. */
    route nroute ;		/* stored route for the net. */
    //    LL_t* pinList;		/* gw: hold ordered list of pins from net decomposition */
    int weight;			/* gw: for net ordering */
    point* ordered_pins;	/* gw: for storing ordred pins for maze routing */
    int num_ordered_pins;	/* gw: same as above */
  } net ;
  
  /**
  * A structure to represent the routing instance
  */
  typedef struct
  {
   int gx ;		/* x dimension of the global routing grid */
   int gy ;		/* y dimension of the global routing grid */
   
   int cap ;
   
   int numNets ;	/* number of nets */
   net *nets ;		/* array of nets */
   
   int numEdges ; 	/* number of edges of the grid */
   int *edgeCaps; 	/* array of the actual edge capacities after considering for blockages */
   int *edgeUtils;	/* array of edge utilizations */  
   
  } routingInst ;

/* gw: self implemented ADT */
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
node_LL_t* new_node_LL_t() ;

LL_t* new_LL_t() ;

/* add node to list, right after n=pos node. */
void LL_add_node(LL_t* list, size_t pos, node_LL_t* node);

/* return -1 if not found, or pos if found */
long  LL_remove_node(LL_t* list, node_LL_t* node) ;

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

node_PQ_t* new_node_PQ_t();

PQ_t* new_PQ_t(size_t initCapacity, __compar_fn_t comp);

void swap_PQ_node(PQ_t* pq, size_t n1, size_t n2);

void PQ_add_node(PQ_t* pq, node_PQ_t* node) ;

node_PQ_t* PQ_remove_node(PQ_t* pq) ;

//helper func
int min(int n1, int n2) ;
void PQ_print(PQ_t* pq) ;

/* gw: end of self implemented ADT. */

/* int readBenchmark(const char *fileName, routingInst *rst)
   Read in the benchmark file and initialize the routing instance.
   This function needs to populate all fields of the routingInst structure.
   input1: fileName: Name of the benchmark input file
   input2: pointer to the routing instance
   output: 1 if successful
*/
int readBenchmark(const char *fileName, routingInst *rst);

  
/* int solveRouting(routingInst *rst)
   This function creates a routing solution
   input: pointer to the routing instance
   output: 1 if successful, 0 otherwise (e.g. the data structures are not populated) 
*/
int solveRouting(routingInst *rst, time_t end_time_limit, int net_d, int net_o);
  
/* int writeOutput(const char *outRouteFile, routingInst *rst)
   Write the routing solution obtained from solveRouting(). 
   Refer to the project link for the required output format.

   Finally, make sure your generated output file passes the evaluation script to make sure
   it is in the correct format and the nets have been correctly routed. The script also reports
   the total wirelength and overflow of your routing solution.

   input1: name of the output file
   input2: pointer to the routing instance
   output: 1 if successful, 0 otherwise 
  */
  int writeOutput(const char *outRouteFile, routingInst *rst);
  
  /* int release(routingInst *rst)
     Release the memory for all the allocated data structures. 
     Failure to release may cause memory problems after multiple runs of your program. 
     Need to recursively delete all memory allocations from bottom to top 
     (starting from segments then routes then individual fields within a net struct, 
     then nets, then the fields in a routing instance, and finally the routing instance)

     output: 1 if successful, 0 otherwise 
  */
 int release(routingInst *rst);


#endif // ECE556_H

