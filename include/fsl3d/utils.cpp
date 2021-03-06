#ifndef FSL3D_UTILS
#define FSL3D_UTILS

#include <cstdlib>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <CGAL/Vector_3.h>
#include <CGAL/squared_distance_3.h>
#include "fsl3d/common.hpp"
#include "fsl3d/voronoi.hpp"

namespace fsl3d::utils{

	// Exit the program and display error message s
	void exit_with_message(const char *s){
		std::cerr << "Aborting" << std::endl;
		std::cerr << s << std::endl;
		std::exit(EXIT_FAILURE);
	}

	// Check if two vertices are too close to each other
	bool p_equal(const vertex_type &a, const vertex_type &b){
		return CGAL::squared_distance(a, b) < EPS * EPS;
	}

	// Read site list from standard input
	// Validate input (number of vertices and duplicates)
	std::vector<vertex_type> read_site_list(){

		// Read from standard input
		std::istream_iterator<vertex_type> ibegin(std::cin);
		std::istream_iterator<vertex_type> iend;
		std::vector<vertex_type> ret(ibegin, iend);

		// Validate input
		if(ret.size() < 2){
			exit_with_message("At least 2 sites required as input");
		}
		for(int i = 0; i < ret.size(); ++i){
			for(int j = i + 1; j < ret.size(); ++j){
				if(p_equal(ret[i], ret[j]))
					exit_with_message("Duplicate sites  exist or sites not far enough from each other");
			}
		}
		return ret;
	}

	// Write description of voronoi diagram to output
	void write_voronoi(Voronoi &vor){
		auto sites = vor.get_site_vertices();
		std::cout << sites.size() << std::endl;							// S: number of sites
		for(auto vert: sites) std::cout << vert << std::endl;			// site vertices
		auto cell_verts = vor.get_cell_vertices();
		std::cout << cell_verts.size() << std::endl;					// V: number of cell vertices
		for(auto vert: cell_verts) std::cout << vert << std::endl;		// cell vertices
		for(int si = 0; si < sites.size(); ++si){
			auto faces = vor.get_cell_faces(si);
			std::cout << faces.size() << std::endl;						// F: number of faces in cell
			for(auto face: faces){
				std::cout << face.size();								// VF: number of vertices in face
				for(auto index: face){
					index = index < 8? index: index - sites.size();
					std::cout << " " << index;							// index to vertex
				}
				std::cout << std::endl;
			}
		}
	}

	// Check if two voronoi diagrams generated from the same site list
	// are equal
	// Precondition: Must be generated from the same sites in same order 
	bool are_voronois_equal(Voronoi &a, Voronoi &b){
		if(a.site_count != b.site_count) return false;
		auto as = a.get_site_vertices(), bs = b.get_site_vertices();
		for(int si = 0; si < a.site_count; ++si){
			if(!p_equal(as[si], bs[si])) return false;
			auto ac = a.cells[si], bc = b.cells[si];
			std::sort(ac.begin(), ac.end());
			std::sort(bc.begin(), bc.end());
			if(ac.size() != bc.size()) return false;
			for(int i = 0; i < ac.size(); ++i)
				if((ac[i] >= 0 || bc[i] >= 0) && ac[i] != bc[i])
					return false;
		}
		return true;
	}
}


#endif
