#include <array>
#include <cmath>
#include <gsl/gsl_randist.h>
#include <iostream>
#include <numeric>
#include <sys/time.h>

constexpr int N{4096};

constexpr double pi()
{
	return std::atan(1) * 4;
}

double f(const double x)
{
	return std::sin(x);
}

int main(int, char**)
{
	auto generator = gsl_rng_alloc(gsl_rng_ranlux);
	struct timeval tv; // Seed generation based on time
	gettimeofday(&tv, 0);
	unsigned long seed = tv.tv_sec + tv.tv_usec;
	gsl_rng_set(generator, seed); // seed
	std::array<double, N> sum{};

	for (auto& i : sum) {
		double x{pi() * gsl_rng_uniform(generator)};
		i = f(x);
	}

	double average{std::accumulate(sum.begin(), sum.end(), 0.) / N};

	auto calc = [average](double result, double x) {
		return std::move(result) + std::pow(x - average, 2.);
	};

	double sigma_y{std::sqrt(
		std::accumulate(sum.begin(), sum.end(), 0., calc) / (N - 1.))};

	double error{sigma_y * pi() / std::sqrt(N)};

	std::cout << "result: " << average * pi() << std::endl;
	std::cout << "error: " << error << std::endl;

	gsl_rng_free(generator);
}
