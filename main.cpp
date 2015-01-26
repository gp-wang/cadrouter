// ECE556 - Copyright 2014 University of Wisconsin-Madison.  All Rights Reserved.

#include "ece556.h"

int main(int argc, char **argv)
{

  if(!(argc ==5)){
    printf("Usage : ./ROUTE.exe <input_benchmark_name> <output_file_name> -d=<0 or 1>, -n=<0 or 1> \n");
    return 1;
  }

  int net_d = 0;
  int net_o = 0;

  //  unsigned long start_time = time(0);
  //  time_t* start = (time_t*) malloc(sizeof(*start));
  //  time(start);
  time_t start = time(0);
  //  printf("\n Start time is: %ld", start);


  //  time_t end_time_limit = start + 5;

  int status;
  char *inputFileName = argv[3];
  char *outputFileName = argv[4];
  char* d = argv[1];
  char* n = argv[2];


  if(!(d[3]=='0' || d[3] == '1') && !(n[3] =='0' || n[3] == '1') ){
    printf("Usage : ./ROUTE.exe -d=<0 or 1>, -n=<0 or 1> <input_benchmark_name> <output_file_name> \n");
    return 1;
  }

  net_d = atoi(&d[3]);
  net_o = atoi(&n[3]);
  time_t end_time_limit = 0;	// init
  if(net_d==1)
    //    end_time_limit = start + 200;
    end_time_limit = start + 300;
  else 
    end_time_limit = start + 900;

  /// create a new routing instance
  //  routingInst *rst = new routingInst;
  routingInst *rst = (routingInst*) malloc(sizeof(*rst));
	
  /// read benchmark
  status = readBenchmark(inputFileName, rst);
  if(status==0){
    printf("ERROR: reading input file \n");
    return 1;
  }
	
  /// run actual routing
  status = solveRouting(rst, end_time_limit, net_d, net_o);
  if(status==0){
    printf("ERROR: running routing \n");
    release(rst);
    return 1;
  }
	
  /// write the result
  status = writeOutput(outputFileName, rst);
  if(status==0){
    printf("ERROR: writing the result \n");
    release(rst);
    return 1;
  }

  release(rst);

  printf("\nDONE!\n");	
  //  unsigned long end_time = time(0);
  time_t end = time(0);
  //  printf("\n End time is: %ld", end);

  double time_spent = difftime(end, start);
  time_spent /= 60.0;

  //  double total_time = (end_time - start_time) / 60.0;
  printf("\n Total time is: %.3lf (min)\n", time_spent);

  return 0;
}


// int main(int argc, char **argv)
// {

//  	if(argc!=3){
//  		printf("Usage : ./ROUTE.exe <input_benchmark_name> <output_file_name> \n");
//  		return 1;
//  	}

//  	int status;
// 	char *inputFileName = argv[1];
//  	char *outputFileName = argv[2];

//  	/// create a new routing instance
//  	routingInst *rst = new routingInst;
	
//  	/// read benchmark
//  	status = readBenchmark(inputFileName, rst);
//  	if(status==0){
//  		printf("ERROR: reading input file \n");
//  		return 1;
//  	}
	
//  	/// run actual routing
//  	status = solveRouting(rst);
//  	if(status==0){
//  		printf("ERROR: running routing \n");
//  		release(rst);
//  		return 1;
//  	}
	
//  	/// write the result
//  	status = writeOutput(outputFileName, rst);
//  	if(status==0){
//  		printf("ERROR: writing the result \n");
//  		release(rst);
//  		return 1;
//  	}

//  	release(rst);
//  	printf("\nDONE!\n");	
//  	return 0;
// }
