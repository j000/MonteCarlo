#ifndef GENERATOR_HXX
#define GENERATOR_HXX

#include <gsl/gsl_randist.h>

class Generator {
public:
	Generator(unsigned long seed = 0);
	~Generator();

	double operator()();
	double get_pos();
    unsigned long get_int(unsigned long max);

private:
	static gsl_rng* m_generator;
#pragma omp threadprivate(m_generator)
};

#endif /* GENERATOR_HXX */
