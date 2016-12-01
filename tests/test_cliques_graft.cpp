#include <iostream>
#include <fstream>
#include <array>
#include <unistd.h>
#include <cstring>
#include "Escape/GraphIO.h"
// #include "Escape/EdgeHash.h"
#include "Escape/Digraph.h"
#include "Escape/CliquesGraft.h"
#include "Escape/nCr.h"

using namespace Escape;

void usage() {
    printf("\ntest_cliques \n\t-i input_file_1[,input_file_2,...] \n\t-m min_clique_size \n\t-M max_clique_size \n\t-r runs \n\t-n num_samples\n");
}

int main(int argc, char *argv[])
{
    if(argc < 11) {
        usage();
        exit(1);
    }

    std::vector<std::string> graphs; // input files
	srand (time(NULL));
    int min_clique_size = 0, max_clique_size = 0, runs = 0, i = 0;
    float start_threshold = 0.0, end_threshold = 0.0, threshold_interval = 0.0;
	int num_samples=0, full=0;

    char c;
    printf("*****************************\n");
    while ((c = getopt(argc, argv, "i:m:M:r:p:n:")) != -1) {
        switch (c) {
            case 'i': {
                // multiple input files
                char *token = std::strtok(optarg, ",");
                printf("Input Files: ");
                while (token != NULL) {
                    graphs.push_back(std::string(token));
                    printf("%s,",token);
                    token = std::strtok(NULL, ",");
                }
                printf("\n");
            }
            break;

            case 'm':
                min_clique_size = atoi(optarg);
                cout << "Min Clique Size: " << min_clique_size << endl;
            break;

            case 'M':
                max_clique_size = atoi(optarg);
                cout << "Max Clique Size: " << max_clique_size << endl;
            break;

            case 'r':
                runs = atoi(optarg);
                cout << "Runs: " << runs << endl;
            break;

            case 'n':
                num_samples = atoi(optarg);
                cout << "Num samples: " << num_samples << endl;
            break;

        }
    }
    printf("*****************************\n\n");

    
    populate_nCr();

    for (std::vector<std::string>::iterator it=graphs.begin(); it!=graphs.end(); ++it)
    {
        std::string fname = "../graphs/" + *it;
        std::cout << fname << std::endl;

        Graph g;
        printf("Loading graph\n");
        if (loadGraph(fname.c_str(), g, 1, IOFormat::escape))
            exit(1);
        cout << "vertices: " << g.nVertices << " edges = " << g.nEdges << std::endl;

        printf("Converting to CSR\n");
        CGraph cg = makeCSR(g);



        printf("Creating DAG\n");
        CDAG dag = degreeOrdered(&cg);

        cout << "vertices: " << g.nVertices << " edges = " << g.nEdges << std::endl;
 
    
        string f = *it;
        char *token = std::strtok(strdup(f.c_str()), ".");
        string gname(token);
        cout << "GNAME = " << gname << endl;
	
        for(i=min_clique_size; i<=max_clique_size; i++)
        {
			string outfile(gname + "_" + to_string(num_samples) + "_" + to_string(i) + "_" + to_string(runs) + "_graft");
			std::string ofname = "../results/cliques/" + outfile;
    		std::cout << ofname << std::endl;
    		std::ofstream of;
    		of.open(ofname);
    		if (!of.is_open())
    		{
        		std::cout << "Could not open output file." << std::endl;
        		exit(1);
  			}
    		printf("Output File: %s\n", ofname.c_str());
			of << "csize,estimate,time" << endl;
       	
			for (int run=0; run < runs; run++)
			{	
            	cliques_edge_sampling_2(g, cg, dag, of, gname, "", i, num_samples);
            	of << std::endl;
			}
			cout << "About to close output file." << endl;
			of.close();
        }
        delGraph(g);
        delCGraph(cg);
        delCDAG(dag);
    }
   


}
