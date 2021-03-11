#include "generator.hxx"
#include <algorithm>
#include <cmath>
#include <gsl/gsl_randist.h>
#include <iomanip>
#include <iostream>
#include <sys/time.h>
#include <vector>

constexpr long unsigned N{80'000'000};
constexpr unsigned BINS{64};
constexpr double X_MAX = 10.0;

constexpr double PI{3.141592653589793238};

#pragma omp declare reduction(vec_unsigned_plus : std::vector<unsigned> \
		: std::transform(omp_out.begin(), omp_out.end(), omp_in.begin(), omp_out.begin(), std::plus<unsigned>())) \
		initializer(omp_priv = decltype(omp_orig)(omp_orig.size()))

int main(int, char**)
{
	static Generator generator;

	std::vector<unsigned> inverse(BINS, 0);
	std::vector<unsigned> inverse_accepted(BINS, 0);
	long unsigned accepted = 0;

#pragma omp parallel for schedule(static) reduction(vec_unsigned_plus:inverse) reduction(+:accepted)
	for (auto i = N; i > 0; i--) {
		auto x = generator();
		x = -std::log(1 - x) / X_MAX;
		if (x < 1) {
			inverse[std::floor(BINS * x)] += 1;

			auto acc = generator();
			if (acc < 4. / 3. / std::sqrt(PI) * std::exp(-x * x + x)) {
				inverse_accepted[std::floor(BINS * x)] += 1;
				accepted += 1;
			}
		}
	}

	auto points_per_slot = X_MAX * accepted / BINS;

	std::cout << std::scientific;
	std::cout << "$DATABLOCK << EOD" << std::endl;
	std::cout << "inverse,inverse\\_accepted" << std::endl;
	for (std::size_t i = 0, e = inverse.size(); i != e; ++i) {
		std::cout << inverse[i] / (X_MAX * N / BINS) << ","
				  << inverse_accepted[i] / points_per_slot << std::endl;
	}
	std::cout << "EOD" << std::endl;

	std::cout << R"GNUPLOT(
set yrange [0:]
set key bottom right
plot $DATABLOCK using 1, $DATABLOCK using 2 pt 0
)GNUPLOT";
}
