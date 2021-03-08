#include <algorithm>
#include <cmath>
#include <gsl/gsl_randist.h>
#include <iomanip>
#include <iostream>
#include <sys/time.h>
#include <vector>
#ifdef _OPENMP
#include <omp.h>
#else
#define omp_get_thread_num() 0
#endif

constexpr long unsigned N{80'000'000};
constexpr unsigned BINS{64};
constexpr double X_MAX = 10.0;

constexpr double pi()
{
	return std::atan(1) * 4;
}

#pragma omp declare reduction(vec_unsigned_plus : std::vector<unsigned> \
		: std::transform(omp_out.begin(), omp_out.end(), omp_in.begin(), omp_out.begin(), std::plus<unsigned>())) \
		initializer(omp_priv = decltype(omp_orig)(omp_orig.size()))

int main(int, char**)
{
	static gsl_rng* generator;
#pragma omp threadprivate(generator)

#pragma omp parallel
	{
		// https://www.gnu.org/software/gsl/doc/html/rng.html?highlight=ranlux#performance
		generator = gsl_rng_alloc(gsl_rng_ranlxs2); // slowest
		// generator = gsl_rng_alloc(gsl_rng_taus2); // fastest
		// Seed generation based on time
		struct timeval tv;
		gettimeofday(&tv, 0);
		unsigned long seed = tv.tv_sec + tv.tv_usec + omp_get_thread_num();
		gsl_rng_set(generator, seed);
	}

	std::vector<unsigned> inverse(BINS, 0);
	std::vector<unsigned> inverse_accepted(BINS, 0);
	long unsigned accepted = 0;

#pragma omp parallel for schedule(static) reduction(vec_unsigned_plus:inverse) reduction(+:accepted)
	for (auto i = N; i > 0; i--) {
		auto x = gsl_rng_uniform(generator);
		x = -std::log(1 - x) / X_MAX;
		if (x < 1) {
			inverse[std::floor(BINS * x)] += 1;

			auto acc = gsl_rng_uniform(generator);
			if (acc < 4. / 3. / std::sqrt(pi()) * std::exp(-x * x + x)) {
				inverse_accepted[std::floor(BINS * x)] += 1;
				accepted += 1;
			}
		}
	}

	auto points_per_slot = X_MAX * accepted / BINS;

	std::cout << std::scientific;
	std::cout << "inverse,inverse\\\\_accepted" << std::endl;
	for (std::size_t i = 0, e = inverse.size(); i != e; ++i) {
		std::cout << inverse[i] / (X_MAX * N / BINS) << ","
				  << inverse_accepted[i] / points_per_slot << std::endl;
	}

#pragma omp parallel
	gsl_rng_free(generator);
}