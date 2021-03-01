#include <array>
#include <cmath>
#include <gsl/gsl_randist.h>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sys/time.h>

constexpr int N{1 << 12};
constexpr int DIMS{2};

constexpr double pi()
{
	return std::atan(1) * 4;
}

int main(int, char**)
{
	auto generator = gsl_rng_alloc(gsl_rng_ranlux);

	{ // Seed generation based on time
		struct timeval tv;
		gettimeofday(&tv, 0);
		unsigned long seed = tv.tv_sec + tv.tv_usec;
		gsl_rng_set(generator, seed);
	}

	std::array<double, N> sum{};

	for (auto& i : sum) {
		auto result{1.};
		for (int j = 0; j < DIMS; ++j) {
			auto x = pi() * gsl_rng_uniform(generator);
			result *= std::sin(x);
		}
		i = result;
	}

	double average{std::accumulate(sum.begin(), sum.end(), 0.) / N};

	auto calc = [average](double result, double x) {
		return std::move(result) + std::pow(x - average, 2.);
	};

	double sigma_y{std::sqrt(
		std::accumulate(sum.begin(), sum.end(), 0., calc) / (N - 1.))};

	double error{sigma_y * pi() / std::sqrt(N)};

	std::cout << std::scientific;
	std::cout << std::setw(20) << "result - expected = " << std::setw(13)
			  << average * std::pow(pi(), DIMS) - std::pow(2., DIMS)
			  << std::endl;
	std::cout << std::setw(20) << "error = " << std::setw(13) << error
			  << std::endl;

	gsl_rng_free(generator);
}
