#ifndef GENERATOR_HXX
#define GENERATOR_HXX

#include <gsl/gsl_randist.h>

class Generator {
public:
	Generator();
	~Generator();

	double operator()();
	double get_pos();

private:
	static gsl_rng* m_generator;
#pragma omp threadprivate(m_generator)
};

#endif /* GENERATOR_HXX */
