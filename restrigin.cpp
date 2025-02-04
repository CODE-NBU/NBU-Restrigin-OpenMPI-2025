/*
https://en.wikipedia.org/wiki/Rastrigin_function
https://en.wikipedia.org/wiki/Random_search
https://en.wikipedia.org/wiki/Genetic_algorithm
*/

#include <mpi.h>
#include <unistd.h>

#include <cmath>
#include <vector>
#include <cstdlib>

using namespace std;

const int ROOT_NODE = 0;

const int DEFAULT_TAG = 1;

const int N = 5;

double f(const vector<double> &x) {
	const double A = 10.0;

	double result = A * x.size();

	for(double xi : x) {
		result += xi*xi - A*cos(2.0*M_PI*xi);
	}

	return result;
}

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	srand( time(NULL)^(rank<<1) );

	if(rank == ROOT_NODE) {
		for(int r=0; r<size; r++) {
			if(r == ROOT_NODE) {
				continue;
			}

			MPI_Send(&N, 1, MPI_INT, r, DEFAULT_TAG, MPI_COMM_WORLD);
		}

		double best, current;
		vector<double> global;
		for(int r=0; r<size; r++) {
			if(r == ROOT_NODE) {
				continue;
			}

			double *result = new double[ N ];
			MPI_Recv(result, N, MPI_DOUBLE, r, DEFAULT_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			vector<double> x;
			for(int i=0; i<N; i++) {
				x.push_back( result[i] );
			}
			delete []result;

			current = f(x);
			if(global.size() != N) {
				best = current;
				global = x;
			} else if(current < best) {
				best = current;
				global = x;
			}

			cout << r << "\t" << f(x) << endl;
		}
		cout << "Global: " << "\t" << best << endl;

		for(double xi : global) {
			cout << xi << " ";
		}
		cout << endl;
	} else {
		int n;
		MPI_Recv(&n, 1, MPI_INT, ROOT_NODE, DEFAULT_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		double best;
		vector<double> x, y;
		for(int g=0; g<1000; g++) {
			for(int i=0; i<n; i++) {
				double xi = 10.24 * (double)rand() / (double)RAND_MAX - 5.12;
				x.push_back( xi );
			}

			if(g == 0) {
				best = f( x );
				y = x;
				continue;
			}

			double current = f( x );
			if(current < best) {
				y = x;
				best = current;
			}
		}

		x = y;

		int i = 0;
		double *result = new double[ x.size() ];
		for(double xi : x) {
			result[i++] = xi;
		}

		MPI_Send(result, x.size(), MPI_DOUBLE, ROOT_NODE, DEFAULT_TAG, MPI_COMM_WORLD);

		delete []result;
	}

	MPI_Finalize();
}
