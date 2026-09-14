Parallel Landslide Risk Assessment and Early Warning System using OpenMP

Overview

This project implements a landslide risk assessment system using C++ and OpenMP.

The system evaluates the landslide risk of individual grid cells using environmental parameters such as:

- Rainfall
- Slope
- Soil Moisture
- Vegetation Factor

The project contains two implementations:

1. Serial Implementation – processes all grid cells sequentially.
2. OpenMP Parallel Implementation – distributes grid-cell processing across multiple threads.

The performance of both implementations is evaluated for different input sizes.



Problem Description

A geographical region is represented as a collection of independent grid cells.

Each grid cell contains environmental parameters that are used to calculate its landslide risk.

For this implementation, synthetic data is generated for the grid cells so that different input sizes can be tested under controlled conditions.

The input sizes used are:

- 1,000 grid cells
- 10,000 grid cells
- 100,000 grid cells
- 1,000,000 grid cells


Risk Assessment Model

The environmental parameters are normalized to the range `[0,1]`.

The risk score is calculated using the following weighted formula:


Risk = 0.4R + 0.3S + 0.2M + 0.1V

Where:

R = Normalized Rainfall
S = Normalized Slope
M = Normalized Soil Moisture
V = Vegetation Factor
Risk Classification
Risk Score	Category
Risk < 0.33	LOW
0.33 ≤ Risk < 0.66	MODERATE
Risk ≥ 0.66	HIGH

The system also calculates:

Number of LOW-risk cells
Number of MODERATE-risk cells
Number of HIGH-risk cells
Average risk
Maximum risk
Grid cell with maximum risk
Overall warning level

Module 1 – Serial Implementation

The serial implementation processes each grid cell sequentially.

For every cell, the program:

Normalizes the environmental parameters.
Calculates the risk score.
Classifies the risk.
Updates the risk-category counts.
Calculates the total risk.
Tracks the maximum-risk cell.
Serial Complexity
Time Complexity  : O(N)
Space Complexity : O(N)

where N is the number of grid cells.

Module 2 – OpenMP Parallel Implementation

The OpenMP implementation parallelizes the grid-cell processing loop.

Since each grid cell can be processed independently, the workload can be divided among multiple threads.

The implementation uses:

#pragma omp parallel

to create the parallel region,

#pragma omp for

to distribute iterations among threads, and

reduction

to safely combine:

LOW count
MODERATE count
HIGH count
Total risk

A critical section is used to safely combine the local maximum-risk results from different threads.

Parallel Complexity

For P threads, the ideal computational complexity is approximately:

O(N/P)

The actual execution time also depends on thread overhead, synchronization, memory access, and hardware limitations.

Correctness Verification

The serial and OpenMP implementations use the same fixed random seed and data-generation method.

Therefore, both implementations process identical input data.

The results were compared for all tested input sizes.

Grid Cells	Serial Low	OpenMP Low	Serial Moderate	OpenMP Moderate	Serial High	OpenMP High
1,000	146	146	692	692	162	162
10,000	1,515	1,515	6,806	6,806	1,679	1,679
100,000	15,200	15,200	68,165	68,165	16,635	16,635
1,000,000	151,982	151,982	680,555	680,555	167,463	167,463

The results match for all tested input sizes, confirming the correctness of the parallel implementation.

Performance Results
Serial Execution Time
Grid Cells	Average Time (ms)
1,000	0.0000
10,000	0.5572
100,000	1.5891
1,000,000	19.8619
OpenMP Execution Time
Grid Cells	2 Threads	4 Threads	8 Threads
1,000	0.0000 ms	0.0000 ms	0.0000 ms
10,000	0.0000 ms	0.0000 ms	0.1000 ms
100,000	0.4000 ms	1.0000 ms	0.4000 ms
1,000,000	9.9000 ms	10.5000 ms	6.6000 ms
Speedup

Speedup is calculated as:

Speedup = Serial Execution Time / Parallel Execution Time

For 1,000,000 grid cells:

Threads	Serial Time (ms)	OpenMP Time (ms)	Speedup
2	19.8619	9.9000	2.01×
4	19.8619	10.5000	1.89×
8	19.8619	6.6000	3.01×

The best measured performance was obtained using 8 threads, with an execution time of 6.6000 ms, compared with 19.8619 ms for the serial implementation.

This gives an approximate speedup of:

3.01×

Technologies Used:
C++
OpenMP
GCC / MinGW
Git
GitHub
How to Compile and Run
Serial Version

Compile:

g++ module1.cpp -o module1

Run:

module1
OpenMP Version

Compile:

g++ -fopenmp module2.cpp -o module2

Run:

module2
OpenMP Concepts Used

This project demonstrates the following OpenMP concepts:

Parallel regions
Parallel loops
Thread-level parallelism
Reduction operations
Critical sections
Shared and private variables
Race-condition handling
Performance measurement
Limitations
The environmental data used in the project is synthetic.
The risk calculation formula is a simplified model for academic demonstration.
Real-world landslide prediction requires real geographical, geological, meteorological, and historical data.
Execution time can vary depending on processor, operating system, compiler, and system load.
Future Enhancements

Possible future improvements include:

Integration of real geographical data.
Integration of real rainfall and soil-moisture datasets.
Addition of elevation and terrain information.
Use of more advanced risk prediction models.
Map-based visualization of risk levels.
Real-time environmental data processing.
Distributed processing for larger datasets.