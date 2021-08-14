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
				std::vector<int> face;
				auto intersect = vor.bisect_face(fid, i, j, face);
				for(auto vi: intersect) new_face.push_back(vi);

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
