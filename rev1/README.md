# cadrouter  
cad router: Class project for ece556 at University of Wisconsin–Madison: create an automatic router for global net routing  

Implementation details:  
	- maze router with A-star heuristic and global routing framework  
	- Implemented an router in C language, including  
	- implementation of ADT (Priority Queue, Linked List),   
	- implementation of an closest_pair algorithm with O(nlogn) complexity.   
	- L-shape route, Z-shape route, RRR, maze-routing with A* heuristic

Instruction:  
	- Download all files into local folder  
	- Building using "./make"  
	- Start routing using : ./ROUTE.exe -d=<0 or 1>, -n=<0 or 1> <input_benchmark_name> <output_file_name>  
		- arguments:  
			if d=0, use L-shaped route  
			if d=1, use Z-shaped route  
			if n=0, no rip-up and re-route  
			if n=1, rip-up and reroute  
		- e.g.: ./ROUTE.exe -d=1 -n=1 ./adaptec1.gr output1  
	- Evaluate the completed routing using 556_eval.exe,  
		Usage: ./556_eval.exe <input_benchmark_name> <output_file_name> <eval_mode>  
		eval_mode is 0 or 1: 0 only checks connectivity, 1 checks routing capacity overflow  
		e.g.: ./556_eval.exe ./adaptec1.gr output1 0



