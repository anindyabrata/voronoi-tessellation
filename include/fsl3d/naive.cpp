#ifndef FSL3D_NAIVE
#define FSL3D_NAIVE

#include <algorithm>
#include <CGAL/Vector_3.h>
#include "fsl3d/common.hpp"

namespace fsl3d{
	Voronoi generate_naive_voronoi(std::vector<vertex_type> site_list){
		Voronoi vor(site_list);
		std::vector<std::vector<std::vector<int>>> cells;
		std::vector<vertex_type> vertices;
		for(int mask = 8; mask--; ){
			//vertex_type vert = vor.bb_min;
			vertex_type vert(
					(mask & 1)? vor.bb_min.x(): vor.bb_max.x(),
					(mask & 2)? vor.bb_min.y(): vor.bb_max.y(),
					(mask & 4)? vor.bb_min.z(): vor.bb_max.z());
			//for(int i = 0; i < 3; ++i) if(mask & (1 << i)) vert[i] = vor.bb_max[i];
			vertices.push_back(vert);
		}
		// initialize each cell as faces of entire bounding box
		for(int site_i = 0; site_i <= site_list.size(); ++site_i){
			std::vector<std::vector<int>> cell;
			for(int fi = 0; fi < 6; ++fi){
				auto m = fi / 3? vor.bb_max: vor.bb_min;
				int i = fi % 3;
				std::vector<int> face;
				for(int j = 0; j < vertices.size(); ++j) if(vertices[j][i] == m[i]) face.push_back(j);
				for(int j = 1; j < face.size() - 1; ++j) for(int k = j; k < face.size(); ++k) {
					auto vl = vertices[face[j-1]], vr = vertices[face[k]];
					if(vl[0] == vr[0] || vl[1] == vr[1]){
						std::swap(face[j], face[k]);
						break;
					}
				}
				cell.push_back(face);
			}
			cells.push_back(cell);
		}
		// for each pair of sites 
		//		find separating plane
		//		reduce cells using plane
		for(int i = 0; i < site_list.size(); ++i) for(int j = 0; j < site_list.size(); ++j) if(i != j){
			std::vector<std::vector<int>> cell;
			auto old_cell = cells[i];
			std::vector<int> new_face;
			for(auto old_face: old_cell){
				std::vector<int> face;
				std::vector<bool> closer;
				for(auto verti: old_face) closer.push_back(CGAL::has_smaller_distance_to_point(vertices[verti], site_list[i], site_list[j]));
				for(int k = 0; k < face.size(); ++k){
					int kr = (k + 1) % face.size();
					if(closer[k] && closer[kr]){
						if(0 == face.size()) face.push_back(old_face[k]);
						face.push_back(old_face[kr]);
					}
					else if(closer[k] || closer[kr]){
						if(closer[k]) face.push_back(old_face[k]);
						// calculate vertex
						auto bplane = CGAL::bisector(site_list[i], site_list[j]);
						K::Line_3 line(vertices[old_face[k]], vertices[old_face[kr]]);
						auto calcres = CGAL::intersection(line, bplane);
						vertex_type calcvert = *boost::get<vertex_type>(&*calcres);
						// add calculated vertex to vertices
						vertices.push_back(calcvert);
						// add index of calculated vertex to face
						int ind = vertices.size() - 1;
						face.push_back(ind);
						// add index of calculated vertex to new_face
						new_face.push_back(ind);
						if(closer[kr]) face.push_back(old_face[kr]);
					}
				}
				if(face.size() > 1 && face.front() == face.back()) face.pop_back();
			}
			if(new_face.size() > 1){
				// sort newface
				K::Plane_3 pln(vertices[new_face[0]], vertices[new_face[1]], vertices[new_face[2]]);
				std::vector<K::Point_2> twod;
				for(int k = 0; k < new_face.size(); ++k) twod.push_back(pln.to_2d(vertices[new_face[k]]));
				K::FT mdx = twod[0].x(), mdy = twod[0].y();
				K::Point_2 md(twod[0]);
				for(int k = 1; k < new_face.size(); ++k) mdx += twod[k].x(), mdy += twod[k].y();
				mdx /= new_face.size(), mdy /= new_face.size();
				//for(int k = 0; k < new_face.size(); ++k) twod[k] -= md;
				std::vector<K::FT> slope;
				for(int k = 0; k < new_face.size(); ++k) slope.push_back((twod[k].y() - mdy) / (twod[k].x() - mdx));
				int sorder[new_face.size()];
				for(int k = 0; k < new_face.size(); ++k) sorder[k] = k;
				//struct{
					//bool operator()(int a, int b) const{ return slope[a] < slope[b]; }
				//} cmp;
				auto cmp = [slope](const int a, const int b) -> bool { return slope[a] < slope[b]; };
				std::sort(sorder, sorder + new_face.size(), cmp);
				std::vector<int> sorted_new_face;
				for(int k = 0; k < new_face.size(); ++k) sorted_new_face.push_back(new_face[sorder[k]]);
				cell.push_back(sorted_new_face);
			}
			cells[i] = cell;
		}
		// put results in vor
		vor.cell_vertices = vertices;
		for(int i = 0; i < site_list.size(); ++i){
			auto cell = cells[i];
			std::vector<int> cf;
			for(auto face: cell){
				cf.push_back(vor.faces.size());
				vor.faces.push_back(face);
			}
			vor.cell_faces.push_back(cf);
		}
		return vor;
	}
}

#endif
