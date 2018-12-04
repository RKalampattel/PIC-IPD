//
//  speciesDatabase.cpp
//  OP2A
//
//  Created by Kim M.K. on 24/11/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#include "speciesDatabase.hpp"


speciesDataBase::speciesDataBase()
{
    
}


speciesDataBase::~speciesDataBase()
{
    
}


void speciesDataBase::read(const std::string& filename)
{
    // Open File to read
    std::ifstream	prob_file;
    prob_file.open(filename.c_str());
    if (!prob_file.is_open())
    {
        Common::ExceptionError(FromHere(), "Cannot find Species Data file. Please check your file!", Common::ErrorCodes::FileSystem());
    }
    
    
    // Read DATA
    std::string	line;
    std::string tempText;
    int read_mode;
    
    
    while (! prob_file.eof())
    {
        getline(prob_file, line);
        Common::read_data_from_string::remove_comments(line, "%");
        
        
        // READ Species DATA
        if (line == DATA_SPECIES_START)
        {
            species speciesTemp;
            std::vector<std::string> lineTemp;
            int num;
            
            while (line != DATA_SPECIES_END)
            {
                getline(prob_file, line);
                Common::read_data_from_string::remove_comments(line, "%");
                
                read_mode = -1;
                if (line.compare(0, 5, "NAME:") == 0)         read_mode = 0;
                if (line.compare(0, 6, "BASIC:") == 0)        read_mode = 1;
                if (line.compare(0, 6, "NONEQ:") == 0)        read_mode = 2;
                if (line.compare(0, 6, "ELECT:") == 0)        read_mode = 3;
                if (line.compare(0, 5, "K_EV:") == 0)         read_mode = 4;
                if (line.compare(0, 5, "LERC:") == 0)         read_mode = 5;
                if (line.compare(0, 9, "BLOTTNER:") == 0)     read_mode = 6;
                if (line.compare(0, 11, "SUTHERLAND:") == 0)  read_mode = 7;
                if (line.compare(0, 9, "LJ_POTEN:") == 0)     read_mode = 8;

  

    
                
                switch (read_mode)
                {
                    case 0:
                        speciesTemp.name = Common::read_data_from_string::read_string(line, "NAME:");
                        break;
                        
                    case 1:
                        line.erase(0, 6);
                        Common::read_data_from_string::remove_space_front(line);
                        speciesTemp.basic.read(line);
                        break;
                        
                    case 2:
                        line.erase(0, 6);
                        Common::read_data_from_string::remove_space_front(line);
                        speciesTemp.noneq.read(line);
                        break;
                        
                    case 3:
                        num = Common::read_data_from_string::read_numeric<int>(line, "ELECT:");
                        lineTemp.resize(num);
                        
                        speciesTemp.noneq.ele.lvl = num;
                        for (int i = 0; i < num; i++)
                        {
                            getline(prob_file, lineTemp[i]);
                            Common::read_data_from_string::remove_comments(lineTemp[i], "%");
                        }
                        speciesTemp.noneq.read(lineTemp, 1);
                        break;
                        
                    case 4:
                        num = Common::read_data_from_string::read_numeric<int>(line, "K_EV:");
                        speciesTemp.noneq.k_ev.lvl = num;
                        
                        if (num > 0)
                        {
                            lineTemp.resize(num);
                        
                            for (int i = 0; i < num; i++)
                            {
                                getline(prob_file, lineTemp[i]);
                                Common::read_data_from_string::remove_comments(lineTemp[i], "%");
                            }
                            speciesTemp.noneq.read(lineTemp, 2);
                        }
                        break;
                        
                    case 5:
                        num = Common::read_data_from_string::read_numeric<int>(line, "LERC:");
                        speciesTemp.basic.lerc.lvl = num;
                        
                        if (num > 0)
                        {
                            lineTemp.resize(num);
                            
                            for (int i = 0; i < num; i++)
                            {
                                getline(prob_file, lineTemp[i]);
                                Common::read_data_from_string::remove_comments(lineTemp[i], "%");
                            }
                            speciesTemp.basic.lerc.read(lineTemp);
                        }
                        break;
                        
                    case 6:
                        line.erase(0, 9);
                        Common::read_data_from_string::remove_space_front(line);
                        speciesTemp.trans.read(line, 0);
                        break;
                        
                    case 7:
                        line.erase(0, 11);
                        Common::read_data_from_string::remove_space_front(line);
                        speciesTemp.trans.read(line, 1);
                        break;
                        
                    case 8:
                        line.erase(0, 9);
                        Common::read_data_from_string::remove_space_front(line);
                        speciesTemp.trans.read(line, 2);
                        break;
                        
                }
            }
            
            if (datamap.exists(speciesTemp.name) != true)
            {
                int n = data.size();
                data.push_back(speciesTemp);
                datamap.insert(speciesTemp.name, n);
            }
        }
    }

    prob_file.close();
}



species speciesDataBase::find(std::string name)
{
    int n = datamap.find(name);
    return (data[n]);
}







