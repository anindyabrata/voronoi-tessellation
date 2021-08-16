#include <cassert>
#include "ogview/voronoi_viewable.hpp"

namespace ogview{
	float sqr(float f){
		return f * f;
	}
	float sq_dist(std::vector<float> vverts, int i, int j){
		i *= 3;
		j *= 3;
		float ret = 0;
		for(int k = 3; k--; ++i, ++j) ret += sqr(vverts[i] - vverts[j]);
		return ret;
	}
	VoronoiViewable::VoronoiViewable(){
	}
	VoronoiViewable::VoronoiViewable(fsl3d::Voronoi &vor){
		// Save float copy of voronoi obj, not the actual obj
		site_count = vor.site_count;

		// Save vertices
		auto exact_verts = vor.get_vertices();
		for(int i = 0; i < exact_verts.size(); ++i) for(int j = 0; j < 3; ++j) vverts.push_back((float)exact_verts[i][j]);
		for(int i = 0; i < vverts.size(); ++i) vverts[i] = ((vverts[i] - vor.bb_min) / (vor.bb_max - vor.bb_min)) * (2 * boundary) - boundary;

		// Save faces
		vfaces = vor.get_unique_faces();

		// Save face indices
		vfinds = vor.get_unique_face_indices();

		// Find y at which a cell is complete
		// Used in only showing completed cells
		for(int i = 0; i < site_count; ++i){
			float sqd = 0, f;
			for(auto fid: vfinds[i]) for(auto vi: vfaces[fid]){
				float nsqd = sq_dist(vverts, 8 + i, vi);
				if(nsqd > sqd) sqd = nsqd;
			}
			f = vverts[1 + 3 *(8 + i)] + sqrt(sqd);
			finy.push_back(f);
		}

		// Precompute y values for beachline
		using K = CGAL::Cartesian<float>;
		float b = boundary;
		for(int ps = 1; ps < PSTEPS; ++ps) {
			float y = (2 * ps / (float)PSTEPS) * 2 * b - b;
			for(int xs = 0; xs <= GSTEPS; ++xs) for(int zs = 0; zs <= GSTEPS; ++zs){
				float x = (xs / (float)GSTEPS) * 2 * b - b;
				float z = (zs / (float)GSTEPS) * 2 * b - b;
				float ry = -b;
				K::Point_3 tp(x, y, z);
				K::Line_3 ln(tp, K::Point_3(x, y - 1, z));
				for(int site = 0; site < site_count; ++site){
					int si = 8 + 3 * site;
					if(vverts[1 + si] >= y) continue;
					K::Point_3 sp(vverts[si], vverts[1 + si], vverts[2 + si]);
					auto bplane = CGAL::bisector(tp, sp);
					auto ires = CGAL::intersection(ln, bplane);
					K::Point_3 ip = *boost::get<K::Point_3>(&*ires);
					if(ip.y() > ry) ry = ip.y();
				}
				cbeach[ps][xs][zs] = ry;
			}
		}

		setFullProgress();
	}
	void VoronoiViewable::setFullProgress(){
		setProgress(PSTEPS);
	}
	void VoronoiViewable::setNoProgress(){
		setProgress(0);
	}
	void VoronoiViewable::increment(){
		setProgress((1 + progress) % (1 + PSTEPS));
	}
	void VoronoiViewable::setProgress(size_t prog){
		if(prog == progress) return;
		progress = prog;

		float b = boundary;
		float y = (2 * prog / (float)PSTEPS) * 2 * b - b;

		// gen verts
		rverts = vverts;

		// gen sites
		rsinds.clear();
		for(int i = 0; i < site_count; ++i) rsinds.push_back((8 + i));

		// gen sweep
		if(prog > 0 && prog < PSTEPS){
			int si = rverts.size() / 3;
			for(float v: {-b, y, -b, -b, y, b, b, y, b, b, y, -b}) rverts.push_back(v);
			std::vector<unsigned int> rsv;
			for(int i = 0; i < 4; ++i) rsv.push_back(si + i);
			rsweep.clear(); rsweep.push_back(rsv);
		}
		else rsweep.clear();

		// gen cells
		if(PSTEPS <= prog) rcells = vfaces;
		else if(0 < prog){
			rcells.clear();
			std::set<int> fids;
			for(int i = 0; i < site_count; ++i)
				if(finy[i] <= y) for(auto fid: vfinds[i]) fids.insert(fid);
			for(auto fid: fids) rcells.push_back(vfaces[fid]);
		}
		else rcells.clear();

		// gen beach
		if(0 < prog && PSTEPS > prog){
			int ivi = rverts.size() / 3;
			for(int xs = 0; xs <= GSTEPS; ++xs) for(int zs = 0; zs <= GSTEPS; ++zs){
				float x = (xs / (float)GSTEPS) * 2 * b - b;
				float z = (zs / (float)GSTEPS) * 2 * b - b;
				for(float v: {x, cbeach[prog][xs][zs], z}) rverts.push_back(v);
			}
			rbeach.clear();
			for(int xs = 0; xs < GSTEPS; ++xs) for(int zs = 0; zs < GSTEPS; ++zs){
				unsigned int r1 = ivi + xs * (1 + GSTEPS) + zs;
				unsigned int r2 = r1 + 1 + GSTEPS;
				rbeach.push_back({r1, r2, 1 + r1});
				rbeach.push_back({r1, r2, 1 + r2});
			}
		}
		else rbeach.clear();
	}
	const std::vector<float>& VoronoiViewable::getVertices(){
		return rverts;
	}
	const std::vector<unsigned int>& VoronoiViewable::getSiteIndices(){
		return rsinds;
	}
	const std::vector<std::vector<unsigned int>>& VoronoiViewable::getCells(){
		return rcells;
	}
	const std::vector<std::vector<unsigned int>>& VoronoiViewable::getSweepline(){
		return rsweep;
	}
	const std::vector<std::vector<unsigned int>>& VoronoiViewable::getBeachline(){
		return rbeach;
	}
}
