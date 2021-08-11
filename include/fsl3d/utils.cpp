#ifndef FSL3D_UTILS
#define FSL3D_UTILS

#include <cstdlib>
#include <iostream>
#include <iterator>
#include <CGAL/Vector_3.h>
#include <CGAL/squared_distance_3.h>
#include "fsl3d/common.hpp"
#include "fsl3d/voronoi.hpp"

namespace fsl3d::utils{
	void exit_with_message(const char *s){
		std::cerr << "Aborting" << std::endl;
		std::cerr << s << std::endl;
		std::exit(EXIT_FAILURE);
	}
	bool p_equal(const vertex_type &a, const vertex_type &b){
		// CGAL::Vector_3<K> v(a,b);
		// return v.squared_length() < EPS * EPS;
		return CGAL::squared_distance(a, b) < EPS * EPS;
	}
	std::vector<vertex_type> read_site_list(){
		std::istream_iterator<vertex_type> ibegin(std::cin);
		std::istream_iterator<vertex_type> iend;
		std::vector<vertex_type> ret(ibegin, iend);
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
	void write_voronoi(const Voronoi &vor){
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
				for(auto index: face) std::cout << " " << face[index];	// index to vertex
				std::cout << std::endl;
			}
		}
	}
}


#endif
