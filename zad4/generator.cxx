#include "generator.hxx"
#include <iostream>
#include <sys/time.h>

#ifdef _OPENMP
#include <omp.h>
#else
#define omp_get_thread_num() 0
#endif

gsl_rng* Generator::m_generator{};

Generator::Generator(unsigned long seed)
{
#pragma omp parallel
	{
		// https://www.gnu.org/software/gsl/doc/html/rng.html?highlight=ranlux#performance
		m_generator = gsl_rng_alloc(gsl_rng_ranlxs2); // slowest
		// generator = gsl_rng_alloc(gsl_rng_taus2); // fastest
		// Seed generation based on time
        if (seed == 0) {
            struct timeval tv;
            gettimeofday(&tv, 0);
            seed = tv.tv_sec + tv.tv_usec + omp_get_thread_num();
        }
		gsl_rng_set(m_generator, seed);
	}
}

Generator::~Generator()
{
#pragma omp parallel
	gsl_rng_free(m_generator);
}

double Generator::operator()()
{
	return gsl_rng_uniform(m_generator);
}

double Generator::get_pos()
{
	return gsl_rng_uniform_pos(m_generator);
}

unsigned long Generator::get_int(unsigned long n)
{
    return gsl_rng_uniform_int(m_generator, n);
}
