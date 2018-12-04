//
//  cal_area.hpp
//  OP2A
//
//  Created by Kim M.K. on 06/09/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#ifndef cal_area_hpp
#define cal_area_hpp

#include <stdio.h>
#include <vector>
#include "matrix.hpp"





double area_line(matrix& n1, matrix& n2);
double area_triangle(matrix& n1, matrix& n2, matrix& n3);
double area_quadrilateral(matrix& n1, matrix& n2, matrix& n3, matrix& n4);
double area_tetrahedron(matrix& n1, matrix& n2, matrix& n3, matrix& n4);
double area_pyramid(matrix& n1, matrix& n2,  matrix& n3, matrix& n4,  matrix& n5);
double area_prism(matrix& n1, matrix& n2, matrix& n3, matrix& n4, matrix& n5, matrix& n6);
double area_hexahedron(matrix& n1, matrix& n2, matrix& n3, matrix& n4, matrix& n5, matrix& n6, matrix& n7, matrix& n8);


double calculateArea(std::vector<matrix>& n, int dim, int flag);

#endif /* cal_area_hpp */
