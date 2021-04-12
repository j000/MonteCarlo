#include "generator.hxx"
#include <algorithm>
#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>

constexpr auto ITER{15u};

template <typename T, unsigned long N>
std::array<T, N>
mul(const std::array<std::array<T, N>, N>& A, const std::array<T, N>& B)
{
	std::array<T, N> out{};
	for (unsigned i = 0; i < N; i++)
		for (unsigned k = 0; k < N; k++)
			out[i] += A[k][i] * B[k];

	return out;
}

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

	// auto P = std::to_array({0.5, 0.2, 0.2, 0.05, 0.05});
	auto P = std::to_array({0.5, 0.2, 0.05, 0.05, 0.2});

	double sum = std::reduce(P.begin(), P.end());
	std::cout << "sum = " << sum << '\n';

	for (auto& p : P)
		p /= sum;

	std::cout << "target distribution:\n";
	for (const auto& elem : P)
		std::cout << std::setw(8) << elem << ' ';
	std::cout << '\n';

	std::array<std::array<double, P.size()>, P.size()> Pacc;
	for (auto i = 0u; i < P.size(); i++)
		for (auto j = 0u; j < P.size(); j++)
			Pacc[i][j] = std::min(1.0, P[j] / P[i]);

	std::cout << "Pacc values:\n";
	for (const auto& row : Pacc) {
		for (const auto& elem : row)
			std::cout << std::setw(8) << elem << ' ';
		std::cout << '\n';
	}

	std::array<std::array<double, P.size()>, P.size()> T;
	for (auto i = 0u; i < P.size(); i++)
		for (auto j = 0u; j < P.size(); j++)
			T[i][j] = (1. / P.size()) * Pacc[i][j];

	for (auto i = 0u; i < P.size(); i++)
		for (auto k = 0u; k < P.size(); k++)
			T[i][i] += (1. / P.size()) * (1. - Pacc[i][k]);

	std::cout << "T values:\n";
	for (const auto& row : T) {
		for (const auto& elem : row)
			std::cout << std::setw(8) << elem << ' ';
		std::cout << '\n';
	}

	std::array<double, P.size()> v{};

	v[0] = 0.5;
	v[1] = 0.5;

	for (auto iter = 1u; iter <= ITER; iter++) {
		std::cout << "iteration " << iter << '\n';


		for (const auto& elem : v)
			std::cout << std::setw(8) << elem << ' ';
		std::cout << '\n';

		auto u = mul(T, v);
		v = u;
	}

	std::cout << "target distribution:\n";
	for (const auto& elem : P)
		std::cout << std::setw(8) << elem << ' ';
	std::cout << '\n';
}
