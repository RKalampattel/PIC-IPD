//
//  stencilinfo.hpp
//  OP2A
//
//  Created by Kim M.K. on 08/09/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#ifndef stencilinfo_hpp
#define stencilinfo_hpp

#include <stdio.h>

template <class T, class T2>
class stencil_info {
    
    // Constructor and Destructor
public:
    stencil_info()
    : L(NULL), L2(NULL), R(NULL), R2(NULL), flag_L(), flag_L2(0), flag_R(0), flag_R2(0)
    {
        neighborList.reserve(26);
    }
    
    ~stencil_info()
    {
        L = NULL;
        L2 = NULL;
        R = NULL;
        R2 = NULL;
    };
    
public:
    T* L;
    T* R;
    T* L2;
    T* R2;
    
    unsigned int flag_L;
    unsigned int flag_L2;
    unsigned int flag_R;
    unsigned int flag_R2;
    
    std::vector<T2*> neighborList;
};


#endif /* stencilinfo_hpp */
