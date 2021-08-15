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
		// save in 2D grid

		setFullProgress();
	}
	void VoronoiViewable::setFullProgress(){
		setProgress(PSTEPS);
	}
	void VoronoiViewable::setNoProgress(){
		setProgress(0);
	}
	void VoronoiViewable::increment(){
		setProgress((1 + progress) % (2 * PSTEPS));
	}
	void VoronoiViewable::setProgress(size_t prog){
		if(prog == progress) return;
		progress = prog;

		float b = boundary;
		float y = (prog / (float)PSTEPS) * 2 * b - b;

		// gen verts
		rverts = vverts;

		// gen sites
		rsinds.clear();
		for(int i = 0; i < site_count; ++i) rsinds.push_back((8 + i));

		// gen sweep
		if(prog > 0 && prog < PSTEPS){
			int si = vverts.size() / 3;
			for(float v: {-b, y, -b, -b, y, b, b, y, b, b, y, -b}) rverts.push_back(v);
			std::vector<unsigned int> rsv;
			for(int i = 0; i < 4; ++i) rsv.push_back(si + i);
			rsweep.clear(); rsweep.push_back(rsv);
		}

		// gen cells
		if(PSTEPS <= prog) rcells = vfaces;
		else if(0 < prog){
			rcells.clear();
			std::set<int> fids;
			for(int i = 0; i < site_count; ++i)
				if(finy[i] <= y) for(auto fid: vfinds[i]) fids.insert(fid);
			for(auto fid: fids) rcells.push_back(vfaces[fid]);
		}

		// gen beach
		if(0 < prog && PSTEPS > prog){
			// For every x,z pair, get highest y value bisector for cur_y and every site with y less than cur_y
		}
	}
	void VoronoiViewable::setBoundary(float bound){
		boundary = bound;
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
