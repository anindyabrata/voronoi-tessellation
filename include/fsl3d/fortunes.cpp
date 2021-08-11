#ifndef FSL3D_FORTUNES
#define FSL3D_FORTUNES

namespace fsl3d{
	class Event_type{
	public:
		Event_type(const vertex_type &v){
		}
		bool is_site_event;
		friend bool operator<(const Event_type& l, const Event_type& r){
			return l.y < r.y;
		}
	};

	Voronoi generate_fortunes_voronoi(std::vector<vertex_type> site_list){
		// populate queue with site events
		std::priority_queue<Event_type> q;
		for(auto st: site_list) q.push(st);
		// loop
		while(!q.empty()){
		}
		//		if site event: insert into beachline
		//			Find all edges it may make with other sites and where those edges start
		//			Find all edges that it may squeeze out (not own edge)
		//		if intersection event: solidify, remove closed cell's site from beachline
	}
}

#endif
