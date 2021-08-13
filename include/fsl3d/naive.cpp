#ifndef FSL3D_NAIVE
#define FSL3D_NAIVE

#include <algorithm>
#include "fsl3d/common.hpp"

namespace fsl3d{
	Voronoi generate_naive_voronoi(std::vector<vertex_type> site_list){
		Voronoi vor(site_list);

		// Initialize each cell as faces of entire bounding box
		for(int site_i = 0; site_i < site_list.size(); ++site_i){
			std::vector<int> cell;
			vor.faces[vor.to_fid(-1, site_i)] = {0,2,6,4}; // +x
			cell.push_back(-1);
			vor.faces[vor.to_fid(-2, site_i)] = {1,3,7,5}; // -x
			cell.push_back(-2);
			vor.faces[vor.to_fid(-3, site_i)] = {0,1,5,4}; // +y
			cell.push_back(-3);
			vor.faces[vor.to_fid(-4, site_i)] = {2,3,7,6}; // -y
			cell.push_back(-4);
			vor.faces[vor.to_fid(-5, site_i)] = {0,1,3,2}; // +z
			cell.push_back(-5);
			vor.faces[vor.to_fid(-6, site_i)] = {4,5,7,6}; // -z
			cell.push_back(-6);
			vor.cells.push_back(cell);
		}

		// for each pair of sites 
		//		find separating plane
		//		reduce cells using plane
		for(int i = 0; i < site_list.size(); ++i) for(int j = 0; j < site_list.size(); ++j) if(i != j){
			std::vector<int> cell;
			auto old_cell = vor.cells[i];
			std::vector<int> new_face;
			for(auto old_face_vert: old_cell){
				auto fid = vor.to_fid(i, old_face_vert);
				auto old_face = vor.faces[fid];
				std::vector<int> face;
				std::vector<bool> closer;
				for(auto verti: old_face) closer.push_back(CGAL::has_smaller_distance_to_point(vor.vertices[verti], site_list[i], site_list[j]));
				for(int k = 0; k < old_face.size(); ++k){
					int kr = (k + 1) % old_face.size();
					if(closer[k] && closer[kr]){
						if(0 == face.size()) face.push_back(old_face[k]);
						face.push_back(old_face[kr]);
					}
					else if(closer[k] || closer[kr]){
						if(closer[k]) face.push_back(old_face[k]);

						// Calculate vertex
						auto bplane = CGAL::bisector(site_list[i], site_list[j]);
						K::Line_3 line(vor.vertices[old_face[k]], vor.vertices[old_face[kr]]);
						auto calcres = CGAL::intersection(line, bplane);
						vertex_type calcvert = *boost::get<vertex_type>(&*calcres);

						// Add calculated vertex to vertices
						vor.vertices.push_back(calcvert);

						// Add index of calculated vertex to face
						int ind = vor.vertices.size() - 1;
						face.push_back(ind);

						// Add index of calculated vertex to new_face
						new_face.push_back(ind);
						if(closer[kr]) face.push_back(old_face[kr]);
					}
				}
				// Remove duplicate vertices
				if(face.size() > 1 && face.front() == face.back()) face.pop_back();

				// If face not completely outside cell, include in cell
				if(face.size() > 0){
					vor.faces[fid] = face;
					cell.push_back(old_face_vert);
				}
			}
			// If current bisector plane contributed, add it to the cell
			if(new_face.size() > 1){
				auto nfid = vor.to_fid(i,j);
				vor.faces[nfid] = new_face;
				vor.sort_face(nfid);
				cell.push_back(j);
			}
			vor.cells[i] = cell;
		}
		return vor;
	}
}

#endif
