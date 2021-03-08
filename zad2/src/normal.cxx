#include "generator.hxx"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

constexpr auto N{16'000'000u};
constexpr auto BINS{64u};
constexpr auto SIGMA{0.5};
constexpr auto MU{0.5};

constexpr auto pi()
{
	return 3.141592653589793238;
	// return std::atan(1) * 4;
}

#pragma omp declare reduction(vec_unsigned_plus : std::vector<unsigned> \
		: std::transform(omp_out.begin(), omp_out.end(), omp_in.begin(), omp_out.begin(), std::plus<unsigned>())) \
		initializer(omp_priv = decltype(omp_orig)(omp_orig.size()))

auto normal_distribution(const double x)
{
	const auto inv_sqrt_2pi = 1. / std::sqrt(2 * pi());
	const auto a = (x - MU) / SIGMA;
	return inv_sqrt_2pi / SIGMA * std::exp(-0.5 * a * a);
}

int main(int, char**)
{
	static Generator generator;

	std::vector<unsigned> normal(BINS, 0);
	std::vector<unsigned> normal_accept(BINS, 0);
	long unsigned in_range = 0;
	long unsigned accepted = 0;

#pragma omp parallel for schedule(static) reduction(vec_unsigned_plus:normal,normal_accept) reduction(+:accepted,in_range)
	for (auto i = N; i > 0; --i) {
		{
			// Box-Muller transformation
			auto x1 = generator.get_pos();
			auto x2 = generator.get_pos();
			auto x
				= SIGMA * std::sqrt(-2 * std::log(x1)) * std::cos(2 * pi() * x2)
				+ MU;

			if (x >= 0 && x < 1) {
				++normal[std::floor(BINS * x)];
				++in_range;
			}
		}

		{
			// accept/reject method
			const auto x1 = generator();
			const auto x2 = generator();

			// const auto C{1. / std::sqrt(2 * pi()) / SIGMA};
			// if (C * x2 < normal_distribution(x1)) {
			// OPTIMIZE!
			const auto a = (x1 - MU) / SIGMA;
			if (x2 < std::exp(-0.5 * a * a)) {
				++normal_accept[std::floor(BINS * x1)];
				++accepted;
			}
		}
	}

	std::cout << std::scientific;
	std::cout << "axis,normal (" << in_range << "),normal\\\\_accepted ("
			  << accepted << "),analitical" << std::endl;

	for (std::size_t i = 0; i != BINS; ++i) {
		// 0-1
		std::cout << 1. * i / (BINS - 1.) << ",";
		// Box-Muller
		std::cout << 1. * normal[i] * BINS / in_range << ",";
		// Reject method
		std::cout << 1. * normal_accept[i] * BINS / accepted << ",";
		// Analytical - with n/in_range approximation
		std::cout << normal_distribution(1. * i / (BINS - 1.)) * N / in_range
				  << ",";
		std::cout << std::endl;
	}
}
