#include "fsl3d/utils.cpp"
#include "fsl3d/naive.cpp"
#include "fsl3d/fortunes.cpp"

int main(int argc, char** argv)
{
	auto site_list = fsl3d::utils::read_site_list();
	auto naive = fsl3d::generate_naive_voronoi(site_list);
	auto fortunes = fsl3d::generate_fortunes_voronoi(site_list);
	auto equal = fsl3d::utils::are_voronois_equal(naive, fortunes);
	std::cout << (equal? "Test passed": "Test Failed") << std::endl;
	return 0;
}
