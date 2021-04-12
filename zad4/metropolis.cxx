#include "generator.hxx"
#include <algorithm>
#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>

constexpr auto ITER{10'000'000u};
constexpr auto O_RESET{ITER / 100};

int main(int argc, char** argv)
{
	std::ios::sync_with_stdio(false);

	unsigned long seed = 0;
	if (argc > 1) {
		std::istringstream ss(argv[1]);
		if (!(ss >> seed)) {
			std::cerr << "Invalid number: " << argv[1] << '\n';
		} else if (!ss.eof()) {
			std::cerr << "Trailing characters after number: " << argv[1]
					  << '\n';
		}
	}
	Generator generator{seed};

	auto P = std::to_array({0.5, 0.2, 0.2, 0.05, 0.05});

	double sum = std::reduce(P.begin(), P.end());
	std::cout << "sum = " << sum << '\n';

	for (auto& p : P)
		p /= sum;

	std::cout << "target distribution:\n";
	for (auto i = 0u; i < P.size(); i++)
		std::cout << "P[" << i << "] = " << P[i] << '\n';

	std::array<std::array<double, P.size()>, P.size()> Pacc;
	for (auto i = 0u; i < P.size(); i++)
		for (auto j = 0u; j < P.size(); j++)
			Pacc[i][j] = std::min(1.0, P[j] / P[i]);

	std::cout << "Pacc values:\n";
	for (const auto& row : Pacc) {
		for (const auto& elem : row)
			std::cout << std::setw(8) << elem;
		std::cout << '\n';
	}

	auto state{generator.get_int(P.size())};
	std::cout << "initial state: " << state << '\n';

	unsigned acc{0};
	double o{0};
	double oe{0};

	for (auto iter = 1u; iter <= ITER; iter++) {
		const int state_p = generator.get_int(P.size());
		if (generator() < Pacc[state][state_p]) {
			acc++;
			state = state_p;
		}
		o += 1. * state;
		oe += 1. * state * state;

		if (iter % O_RESET == 0) {
			std::cout << "acceptance = " << (acc * 1. / iter) << '\n';
			std::cout << "observable exp = " << o * 1. / O_RESET << " "
					  << std::sqrt(oe * O_RESET - o * o) / O_RESET
					/ std::sqrt(O_RESET)
					  << '\n';

			o = 0;
			oe = 0;
		}
	}
}
