# cadrouter  
cad router: Class project for ece556 at University of Wisconsin–Madison: create an automatic router for global net routing  
rev2 notes: migrate from C to C++ with STL  

Implementation details:  
	- maze router with A-star heuristic and global routing framework  
	- Implemented an router in C++ language, including Z-shape route, RRR, maze-routing with A* heuristic  
	- Optimized the runtime of Z-shape routing algorithm. Eliminated the need for L-shape algorithm.  
	- made RRR as default option with little overhead compared to non RRR.  

Instruction:
	- Download all files into local folder  
	- Building using "./make"  
	- Start routing using : ./ROUTE.exe <input_benchmark_name> <output_file_name>  
		e.g.: ./ROUTE.exe -d=1 -n=1 ./adaptec1.gr output1  
	- Evaluate the completed routing using 556_eval.exe,  
		Usage: ./556_eval.exe <input_benchmark_name> <output_file_name> <eval_mode>  
		eval_mode is 0 or 1: 0 only checks connectivity, 1 checks routing capacity overflow  
		e.g.: ./556_eval.exe ./adaptec1.gr output1 0  



