//
//  cal_area.cpp
//  OP2A
//
//  Created by Kim M.K. on 06/09/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//
#include <limits>
#include "cal_area.hpp"

#include "../COMM/error_codes.hpp"
#include "../COMM/error_check.hpp"


double area_line(matrix& n1, matrix& n2)
{
    // Check type of matrix
    if (matrix_type(n1) == 1 && matrix_type(n2) == 1)
    {
        matrix n12;
        n12 = n2 - n1;
    
        double res = norm(n12, 2);
        return (res);
    }
    else
    {
        Common::OP2A_ERROR(FromHere(), 1, 1, __FUNCTION__);
    }
    
    return (-1.0);
}


double area_triangle(matrix& n1, matrix& n2, matrix& n3)
{
    if (matrix_type(n1) == 1 && matrix_type(n2) == 1 && matrix_type(n3) == 1)
    {
        double n12 = area_line(n1, n2);
        double n23 = area_line(n2, n3);
        double n31 = area_line(n3, n1);
    
        double area, s;
    
        s = 0.5 * (n12 + n23 + n31);
        area	= s * (s-n12) * (s-n23) * (s-n31);
    
        if (area < 0.0)
        {
            Common::OP2A_ERROR2(FromHere(), 3, 20, __FUNCTION__, "area", area);
            
        }
    
        return (sqrt(area));
    }
    else
    {
        Common::OP2A_ERROR(FromHere(), 1, 1, __FUNCTION__);
    }
    
    return (-1.0);
}


double area_quadrilateral(matrix& n1, matrix& n2, matrix& n3, matrix& n4)
{
    double s1, s2;
    
    s1 = area_triangle(n1, n2, n3);
    s2 = area_triangle(n2, n3, n4);
    
    return (s1 + s2);
}



double area_tetrahedron(matrix& n1, matrix& n2, matrix& n3, matrix& n4)
{
    if (matrix_type(n1) == 1 &&
        matrix_type(n2) == 1 &&
        matrix_type(n3) == 1 &&
        matrix_type(n4) == 1)
    {
        double vol;
    
        matrix n41 = n1 - n4;
        matrix n42 = n2 - n4;
        matrix n43 = n3 - n4;
    
        matrix temp = cross(n42, n43);
    
        vol  = dot(n41, temp);
        vol	= fabs(vol) / 6.0;
        
        if (vol < 0.0)
            Common::OP2A_ERROR2(FromHere(), 3, 20, __FUNCTION__, "vol", vol);
        
        if (vol != vol)
            Common::OP2A_ERROR2(FromHere(), 3, 21, __FUNCTION__, "vol", vol);
        
        if (vol == std::numeric_limits<double>::infinity())
            Common::OP2A_ERROR2(FromHere(), 3, 22, __FUNCTION__, "vol", vol);
        
        return (vol);
    }
    else
    {
        Common::OP2A_ERROR(FromHere(), 1, 1, __FUNCTION__);
    }
    
    return (-1.0);
}


double area_pyramid(matrix& n1, matrix& n2,  matrix& n3, matrix& n4,  matrix& n5)
{
    int i;
    double volume;
    
    matrix n6 (3,1);
    
    for (i = 0; i <= 2; i++) n6(i) = 0.25 * (n1(i) + n2(i) + n3(i) + n4(i));
    
    volume = area_tetrahedron(n1, n2, n6, n5);
    volume += area_tetrahedron(n2, n3, n6, n5);
    volume += area_tetrahedron(n3, n4, n6, n5);
    volume += area_tetrahedron(n4, n1, n6, n5);
    
    return (volume);
}


double area_prism(matrix& n1, matrix& n2, matrix& n3, matrix& n4, matrix& n5, matrix& n6)
{
    int i;
    matrix n7(3, 1);
    
    double volume;
    
    for (i = 0; i <= 2; i++) n7(i) = (n1(i) + n2(i) + n3(i) + n4(i) + n5(i) + n6(i)) / 6.0;
    
    volume = area_pyramid(n1, n4, n5, n2, n7);
    volume += area_pyramid(n2, n3, n6, n3, n7);
    volume += area_pyramid(n1, n3, n6, n4, n7);
    volume += area_tetrahedron(n1, n2, n3, n7);
    volume += area_tetrahedron(n4, n6, n5, n7);
    
    return (volume);
}


double area_hexahedron(matrix& n1, matrix& n2, matrix& n3, matrix& n4, matrix& n5, matrix& n6, matrix& n7, matrix& n8)
{
    int i;
    matrix n9(3, 1);
    double volume;
    
    for (i = 0; i <= 2; i++)	n9(i) = 0.125 * (n1(i) + n2(i) + n3(i) + n4(i) + n5(i) + n6(i) + n7(i) + n8(i));
    
    volume = area_pyramid(n1, n2, n3, n4, n9);
    volume += area_pyramid(n5, n8, n7, n6, n9);
    volume += area_pyramid(n1, n5, n6, n2, n9);
    volume += area_pyramid(n3, n7, n8, n4, n9);
    volume += area_pyramid(n2, n6, n7, n3, n9);
    volume += area_pyramid(n1, n4, n8, n5, n9);
    
    return (volume);
}




double calculateArea(std::vector<matrix>& n, int dim, int flag)
{
    double res = -1.0;
    switch (flag)
    {
        case 2:
            res = area_line(n[0], n[1]);
            break;
            
        case 3:
            res = area_triangle(n[0], n[1], n[2]);
            break;
            
        case 4:
            res = area_quadrilateral(n[0], n[1], n[2], n[3]);
            break;
            
        case 5:
            res = area_pyramid(n[0], n[1], n[2], n[3], n[4]);
            break;
            
        case 6:
            res = area_prism(n[0], n[1], n[2], n[3], n[4], n[5]);
            break;
            
        case 7:
            res = area_tetrahedron(n[0], n[1], n[2], n[3]);
            break;
            
        case 8:
            res = area_hexahedron(n[0], n[1], n[2], n[3], n[4], n[5], n[6], n[7]);
            break;
    }
    
    return (res);
}
