#ifndef FSL3D_FORTUNES
#define FSL3D_FORTUNES

#include <cmath>
#include <algorithm>
#include <queue>
#include "fsl3d/common.hpp"

namespace fsl3d{
	class Event_type{
	public:
		// Site event constructor
		Event_type(int index, vertex_type vert){
			ind = index;
			v = vert;
			y = vert.y();
			is_site_event = true;
		}
		// Intersection event constructor
		Event_type(int index, int afid, vertex_type vert, vertex_type sitev){
			ind = index;
			fid = afid;
			v = vert;
			sv = sitev;
			is_site_event = false;
			// Compute y when event should be evaluated using sphere-line intersection
			// SK::Sphere_3 sphere(vert, CGAL::squared_distance(vert, sitev));
			// vertex_type yvert(vert.x(), vert.y() - 1, vert.z());
			// SK::Line_3 line(vert, yvert);
			// auto intersection = CGAL::intersection(sphere, line);
			// for(vertex_type tvert: intersection) if(tvert.y() > vert.y()) y = tvert.y();
			y = vert.y() + sqrt((long double)CGAL::squared_distance(vert, sitev));
		}
		int ind, fid;
		vertex_type v, sv;
		scalar_type y;
		bool is_site_event;
		friend bool operator<(const Event_type& l, const Event_type& r){
			if(l.y != r.y) return l.y < r.y;
			if(l.is_site_event != r.is_site_event) return l.is_site_event;
			return l.v < r.v;
		}
	};

	Voronoi generate_fortunes_voronoi(std::vector<vertex_type> site_list){
		Voronoi vor(site_list);
		
		// Initialize empty beachline
		std::set<int> beachline; // Sites whose cell is still not complete (not squeezed)
		std::map<int, std::vector<unsigned int>> pending_faces; // Faces that are not confirmed; Does not contain impossible faces
		std::map<int, int> solid_vert_count; // Number of confirmed vertices in each face

		// Initialize empty cells
		for(int i = 0; i < site_list.size(); ++i) vor.cells.emplace_back();

		// Populate queue with site events
		std::priority_queue<Event_type> q;
		for(int i = 0; i < site_list.size(); ++i) q.emplace(i, site_list[i]);

		// Get first site event from q and associate with boundary faces
		for(int isite = 0; isite < site_list.size(); ++isite){
			int ifid = vor.to_fid(-1, isite);
			vor.faces[ifid] = {0,2,6,4}; // +x
			pending_faces[isite].push_back(-1);
			ifid = vor.to_fid(-2, isite);
			vor.faces[ifid] = {1,3,7,5}; // -x
			pending_faces[isite].push_back(-2);
			ifid = vor.to_fid(-3, isite);
			vor.faces[ifid] = {0,1,5,4}; // +y
			pending_faces[isite].push_back(-3);
			ifid = vor.to_fid(-4, isite);
			vor.faces[ifid] = {2,3,7,6}; // -y
			pending_faces[isite].push_back(-4);
			ifid = vor.to_fid(-5, isite);
			vor.faces[ifid] = {0,1,3,2}; // +z
			pending_faces[isite].push_back(-5);
			ifid = vor.to_fid(-6, isite);
			vor.faces[ifid] = {4,5,7,6}; // -z
			pending_faces[isite].push_back(-6);
		}


		while(!q.empty()){
			auto ev = q.top(); q.pop();
			if(ev.is_site_event){
				auto site = ev.ind;
				// Bisect initial faces (bounding box)
				for(auto beach_site: beachline){
					std::vector<unsigned int> nface;
					auto &pasites = pending_faces[site];
					std::vector<unsigned int> npasites;
					for(auto pasite: pasites){
						int fid = vor.to_fid(pasite, site);
						std::vector<unsigned int> face;
						auto intersect = vor.bisect_face(fid, beach_site, site, face);
						vor.faces[fid] = face;
						if(face.size()) npasites.push_back(pasite);
						for(auto vi: intersect) nface.push_back(vi);
					}
					if(nface.size()){
						int bfid = vor.to_fid(site, beach_site);
						vor.faces[bfid] = nface;
						vor.sort_face(bfid);
						npasites.push_back(beach_site);
					}
					pending_faces[site] = npasites;
				}
				// Bisect beachline faces by site
				for(auto beach_site: beachline){
					auto &pasites = pending_faces[beach_site];
					std::vector<unsigned int> npasites;
					for(auto pasite: pasites) if(pasite < beach_site){
						int fid = vor.to_fid(pasite, beach_site);
						std::vector<unsigned int> face;
						auto intersect = vor.bisect_face(fid, site, beach_site, face);
						vor.faces[fid] = face;
						if(face.size()) npasites.push_back(pasite);
						else if(pasite >= 0){ // Since we're only checking faces once, empty faces need to be removed from pasite as well
							auto it = std::find(pending_faces[pasite].begin(), pending_faces[pasite].end(), beach_site);
							if(it != pending_faces[pasite].end()) pending_faces[pasite].erase(it);
						}
						// Create intersection events for intersecting points
						for(auto vi: intersect) q.emplace(vi, fid, vor.vertices[vi], site_list[beach_site]);
					}
					pending_faces[beach_site] = npasites;
				}
				// Create intersection events for site adjacent faces
				for(auto pasite: pending_faces[site]){
					int fid = vor.to_fid(pasite, site);
					for(auto vi: vor.faces[fid]) q.emplace(vi, fid, vor.vertices[vi], site_list[site]);
				}
				beachline.insert(site);
			}
			else{
				// If vertex valid, add to vert_count of face
				int vi = ev.ind, fid = ev.fid;
				auto &vf = vor.faces[fid];
				if(std::find(vf.begin(), vf.end(), vi) != vf.end()) ++solid_vert_count[fid];
				// If vertex closes face, close face
				if(solid_vert_count[fid] >= vor.faces[fid].size()){
					// Check if count >= face.size() and check if all vertices closer to site than current y to confirm
					bool closer = true;
					for(auto vi: vor.faces[fid]){
						auto vert = vor.vertices[vi];
						vertex_type yvert(vert.x(), ev.y, vert.z());
						if(CGAL::has_smaller_distance_to_point(vert, yvert, ev.sv)){
							closer = false;
							break;
						}
					}
					if(closer){ // Close face
						int sid1, sid2;
						vor.from_fid(fid, sid1, sid2);
						if(sid1 >= 0){
							auto &pfs = pending_faces[sid1];
							auto it = std::find(pfs.begin(), pfs.end(), sid2);
							if(it != pfs.end()){ // Add face to cell, no longer pending
								pending_faces[sid1].erase(it);
								vor.cells[sid1].push_back(sid2);
								// If closing face closes cell, remove site from beachline
								if(!pending_faces[sid1].size()) beachline.erase(sid1);
							}
						}
						if(sid2 >= 0){
							auto &pfs = pending_faces[sid2];
							auto it = std::find(pfs.begin(), pfs.end(), sid1);
							if(it != pfs.end()){ // Add face to cell, no longer pending
								pending_faces[sid2].erase(it);
								vor.cells[sid2].push_back(sid1);
								// If closing face closes cell, remove site from beachline
								if(!pending_faces[sid2].size()) beachline.erase(sid2);
							}
						}
					}
				}
			}
		}
		return vor;
	}
}

#endif
