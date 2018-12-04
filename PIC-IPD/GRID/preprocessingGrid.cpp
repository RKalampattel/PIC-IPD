//
//  preprocessingGrid.cpp
//  OP2A
//
//  Created by Kim M.K. on 09/11/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <string>
#include "grid.hpp"
#include "../COMM/version.hpp"

int preprocessingGrid(int argc, char **argv)
{
    Common::version ver(1, 0, 2016, 11, 10, "Grid Converter");
    ver.info();
    
    if (std::string(argv[1]) == "-h")
    {
        std::cout << "         OP2A (R) Grid Converter HELP" << std::endl;
        std::cout << "         ============================" << std::endl;
        std::cout << std::endl;
        std::cout << "OP2A Grid Converter converts different format of grid files to a grid file which is readable in OP2A." << std::endl;
        std::cout << std::endl;
        std::cout << "usage: grid_converter.op2a grid_filename [grid file format option] outputfile name" << std::endl;
        
        std::cout << std::endl;
        std::cout << "         Grid File Format Option List" << std::endl;
        std::cout << "         ----------------------------" << std::endl;
        std::cout << "-su2         SU2" << std::endl;
        std::cout << "-flu         FLUENT" << std::endl << std::endl;
    }
    else
    {
        std::string infile(argv[1]);
        std::string filetype;
        std::string outfile;
        int typeflag;
        if (std::string(argv[2]) == "-su2")
        {
            filetype = "SU2";
            typeflag = 0;
        }
        else if (std::string(argv[2]) == "-flu")
        {
            filetype = "Fluent";
            typeflag = 1;
        }
        else
        {
            filetype = "NOT SUPPORTED TYPE";
            typeflag = -1;
        }
        
        
        std::cout <<"converting grid file" << std::endl;
        std::cout <<"--------------------" << std::endl;
        
        std::cout <<"           [File Name]: " << infile << std::endl;
        std::cout <<"           [File Type]: " << filetype << std::endl;
        
        if (filetype != "NOT SUPPORTED TYPE")
        {
            if (argc == 3)
            {
                outfile = infile;
                if (typeflag == 0)      precessingGridSU2(infile);
                else if (typeflag == 1) precessingGridFLUENT(infile);
            }
            else
            {
                outfile = argv[3];
                if (typeflag == 0)      precessingGridSU2(infile, outfile);
                else if (typeflag == 1) precessingGridFLUENT(infile, outfile);
            }
            std::cout <<"           [  Output ]: " << outfile << ".op2"<< std::endl;
            std::cout << "Done" << std::endl;
        }
        
    }
    
    std::cout << argv[2];

	return 0;
}

