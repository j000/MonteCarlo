#include "generator.hxx"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

constexpr long unsigned N{80'000'000};
constexpr unsigned BINS{1000};

#pragma omp declare reduction(vec_unsigned_plus : std::vector<unsigned> \
		: std::transform(omp_out.begin(), omp_out.end(), omp_in.begin(), omp_out.begin(), std::plus<unsigned>())) \
		initializer(omp_priv = decltype(omp_orig)(omp_orig.size()))

int main(int, char**)
{
	static Generator generator;

	std::vector<unsigned> hist(BINS, 0);

#pragma omp parallel for schedule(static) reduction(vec_unsigned_plus : hist)
	for (auto i = N; i > 0; i--) {
		auto x = generator();
		hist[std::floor(x * BINS)] += 1;
	}

	double max = *std::max_element(hist.begin(), hist.end());

	std::cout << std::scientific;
	std::cout << "$DATABLOCK << EOD\n";
	std::cout << "random generator\n";
	for (std::size_t i = 0, e = hist.size(); i != e; ++i) {
		std::cout << hist[i] / max << '\n';
	}
	std::cout << "EOD\n";

	std::cout << R"GNUPLOT(
set yrange [0:]
set key bottom right
plot $DATABLOCK using 1
)GNUPLOT";
}
