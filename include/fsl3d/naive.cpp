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
			vertex_type vert(
					(mask & 1)? vor.bb_max: vor.bb_min,
					(mask & 2)? vor.bb_max: vor.bb_min,
					(mask & 4)? vor.bb_max: vor.bb_min);
			vertices.push_back(vert);
		}
		// initialize each cell as faces of entire bounding box
		for(int site_i = 0; site_i <= site_list.size(); ++site_i){
			std::vector<std::vector<int>> cell;
			cell.push_back({0,1,3,2}); // +z
			cell.push_back({4,5,7,6}); // -z
			cell.push_back({0,1,5,4}); // +y
			cell.push_back({2,3,7,6}); // -y
			cell.push_back({0,2,6,4}); // +x
			cell.push_back({1,3,7,5}); // -x
			cells.push_back(cell);
		}
		// for each pair of sites 
		//		find separating plane
		//		reduce cells using plane
		for(int i = 0; i < site_list.size(); ++i) for(int j = 0; j < site_list.size(); ++j) if(i != j){
			break;
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
				if(face.size() > 0) cell.push_back(face);
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
