//
//  grid_def.h
//  OP2A
//
//  Created by Kim M.K. on 16/01/2017.
//  Copyright © 2017 Kim M.K. All rights reserved.
//

#ifndef grid_def_h
#define grid_def_h



// 1. Grid Setting (CAN BE ADJUSTIBLE)
#define GRID_AXISMEMTRIC_Y      1.0e-5
#define GRID_MAX_REFINEMENT_LVL 5
#define GRID_MAX_NUM_PATH       1000
#define GRID_MAX_NUM_LINES      2000
#define GRID_MAX_NUM_CPU        2000
#define GRID_GHOST_RESERVE_FOR_MPI GRID_MAX_NUM_CPU
#define MIN_PARTICLE_PER_CELL 20



// 2. VTK Format Identifier (DO NO CHANGE)
#define	LINE	 3
#define	TRI3	 5
#define	QUAD4	 9
#define	TETRA4   10
#define	HEXA8	 12
#define PRISM6	 13
#define PYRAMID5 14


// 3. BC FLAG (DO NO CHANGE)
#define BC_INTERIOR 	0
#define BC_WALL			10
#define BC_INLET		20
#define BC_OUTLET		30
#define BC_FREESTREAM	40
#define BC_SYMMETRY	    50
#define BC_AXIS			60
#define BC_ANODE		70
#define BC_CATHODE		80
#define BC_DIELECTRIC	90
#define BC_MAX_DETAIL_SUBCOND 10


// 4. GEOMETRY/CONNECTIVITY NAME (DO NO CHANGE)
#define GRID_NOR	0
#define GRID_TAN1	1
#define GRID_TAN2	2
#define GRID_X 0
#define GRID_Y 1
#define GRID_Z 2

#define BC_WEST    0
#define BC_SOUTH   1
#define BC_EAST    2
#define BC_NORTH   3
#define BC_TOP     4
#define BC_BOTTOM  5


// 5. FLAG ETC (DO NO CHANGE)
#define PARTICLE_OLD 0
#define PARTICLE_NEW 1

#define INDEX_ADD_CPU    10
#define INDEX_ADD_GHOST  100










#endif /* grid_def_h */
