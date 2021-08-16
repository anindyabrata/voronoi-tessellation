#ifndef FSL3D_COMMON
#define FSL3D_COMMON

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

// Common types and values used by other files
namespace fsl3d{
	typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
	typedef K::FT scalar_type;
	typedef K::Point_3 vertex_type;
	const double EPS = 1e-8;
}

#endif
