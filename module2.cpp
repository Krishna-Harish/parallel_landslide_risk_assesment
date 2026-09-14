
//OPENMP PARALLEL VERSION

#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <omp.h>

using namespace std;

const double RAIN_MIN  = 0.0;
const double RAIN_MAX  = 500.0;

const double SLOPE_MIN = 0.0;
const double SLOPE_MAX = 90.0;

const double MOIST_MIN = 0.0;
const double MOIST_MAX = 100.0;

const double VEG_MIN   = 0.0;
const double VEG_MAX   = 1.0;


const double W_RAIN  = 0.4;
const double W_SLOPE = 0.3;
const double W_MOIST = 0.2;
const double W_VEG   = 0.1;

const double LOW_THRESHOLD = 0.33;
const double MOD_THRESHOLD = 0.66;


const int REPETITIONS = 10;
struct GridCell
{
    double rainfall;
    double slope;
    double moisture;
    double vegFactor;

    double riskScore;
    int riskCategory;       // 0 = LOW, 1 = MODERATE, 2 = HIGH
};

struct Summary
{
    long long lowCount = 0;
    long long modCount = 0;
    long long highCount = 0;

    double avgRisk = 0.0;
    double maxRisk = -1.0;

    int maxIndex = -1;
    int overallWarning = 0;
};

const char* categoryName(int category)
{
    if (category == 0)
        return "LOW";

    if (category == 1)
        return "MODERATE";

    return "HIGH";
}
void generateData(vector<GridCell>& data, int n)
{
    data.resize(n);

    mt19937 generator(42);

    uniform_real_distribution<double> rainfallDist(0.0, 500.0);
    uniform_real_distribution<double> slopeDist(0.0, 90.0);
    uniform_real_distribution<double> moistureDist(0.0, 100.0);
    uniform_real_distribution<double> vegetationDist(0.0, 1.0);

    for (int i = 0; i < n; i++)
    {
        data[i].rainfall  = rainfallDist(generator);
        data[i].slope     = slopeDist(generator);
        data[i].moisture  = moistureDist(generator);
        data[i].vegFactor = vegetationDist(generator);

        data[i].riskScore = 0.0;
        data[i].riskCategory = 0;
    }
}
void calculateRisk(GridCell& cell)
{
    double R = (cell.rainfall - RAIN_MIN) /
               (RAIN_MAX - RAIN_MIN);

    double S = (cell.slope - SLOPE_MIN) /
               (SLOPE_MAX - SLOPE_MIN);

    double M = (cell.moisture - MOIST_MIN) /
               (MOIST_MAX - MOIST_MIN);

    double V = (cell.vegFactor - VEG_MIN) /
               (VEG_MAX - VEG_MIN);

    cell.riskScore =
        W_RAIN  * R +
        W_SLOPE * S +
        W_MOIST * M +
        W_VEG   * V;

    if (cell.riskScore < LOW_THRESHOLD)
    {
        cell.riskCategory = 0;
    }
    else if (cell.riskScore < MOD_THRESHOLD)
    {
        cell.riskCategory = 1;
    }
    else
    {
        cell.riskCategory = 2;
    }
}

Summary processParallel(vector<GridCell>& data)
{
    Summary summary;

    long long lowCount = 0;
    long long modCount = 0;
    long long highCount = 0;

    double sum = 0.0;

    #pragma omp parallel
    {
        // Each thread has its own local maximum.

        double localMaxRisk = -1.0;
        int localMaxIndex = -1;

        #pragma omp for reduction(+:lowCount, modCount, highCount, sum)

        for (int i = 0;
             i < static_cast<int>(data.size());
             i++)
        {
            calculateRisk(data[i]);

            sum += data[i].riskScore;

            // Category counts

            if (data[i].riskCategory == 0)
            {
                lowCount++;
            }
            else if (data[i].riskCategory == 1)
            {
                modCount++;
            }
            else
            {
                highCount++;
            }

            // Local maximum for this thread

            if (data[i].riskScore > localMaxRisk)
            {
                localMaxRisk = data[i].riskScore;
                localMaxIndex = i;
            }
        }

        #pragma omp critical
        {
            if (localMaxRisk > summary.maxRisk)
            {
                summary.maxRisk = localMaxRisk;
                summary.maxIndex = localMaxIndex;
            }
        }
    }

    summary.lowCount = lowCount;
    summary.modCount = modCount;
    summary.highCount = highCount;

    summary.avgRisk = sum / data.size();

    if (summary.maxIndex >= 0)
    {
        summary.overallWarning =
            data[summary.maxIndex].riskCategory;
    }

    return summary;
}

void runDemo()
{
    const int n = 5;
    const int threads = 4;

    omp_set_num_threads(threads);

    vector<GridCell> data;

    generateData(data, n);

    double start = omp_get_wtime();

    Summary summary = processParallel(data);

    double end = omp_get_wtime();

    cout << "\n";
    cout << "========================================\n";
    cout << "OPENMP DEMONSTRATION\n";
    cout << "Threads Used: " << threads << "\n";
    cout << "========================================\n";

    cout << left
         << setw(10) << "Cell"
         << setw(12) << "Rainfall"
         << setw(10) << "Slope"
         << setw(12) << "Moisture"
         << setw(10) << "Veg"
         << setw(12) << "Risk"
         << setw(12) << "Category"
         << "\n";

    cout << string(78, '-') << "\n";

    for (size_t i = 0; i < data.size(); i++)
    {
        cout << left
             << setw(10) << i
             << setw(12) << fixed << setprecision(2)
             << data[i].rainfall
             << setw(10) << data[i].slope
             << setw(12) << data[i].moisture
             << setw(10) << data[i].vegFactor
             << setw(12) << data[i].riskScore
             << setw(12)
             << categoryName(data[i].riskCategory)
             << "\n";
    }

    cout << "\n";

    cout << fixed << setprecision(4);

    cout << "Low Risk         : " << summary.lowCount << "\n";
    cout << "Moderate Risk    : " << summary.modCount << "\n";
    cout << "High Risk        : " << summary.highCount << "\n";
    cout << "Average Risk     : " << summary.avgRisk << "\n";
    cout << "Maximum Risk     : " << summary.maxRisk << "\n";
    cout << "Highest Risk Cell: " << summary.maxIndex << "\n";
    cout << "Overall Warning  : "
         << categoryName(summary.overallWarning) << "\n";

    cout << "Execution Time   : "
         << (end - start) * 1000.0
         << " ms\n";

    cout << "========================================\n";
}
void runPerformanceTest(int n,
                        const vector<int>& threadCounts)
{
    cout << "\n";
    cout << "========================================\n";
    cout << "Grid Cells: " << n << "\n";
    cout << "========================================\n";

    for (int threads : threadCounts)
    {
        omp_set_num_threads(threads);

        vector<GridCell> data;

        // Generate once, outside timing.

        generateData(data, n);

        double totalTime = 0.0;

        Summary finalSummary;

        for (int run = 0; run < REPETITIONS; run++)
        {
            double start = omp_get_wtime();

            finalSummary = processParallel(data);

            double end = omp_get_wtime();

            totalTime +=
                (end - start) * 1000.0;
        }

        double averageTime =
            totalTime / REPETITIONS;

        cout << fixed << setprecision(4);

        cout << "Threads = " << setw(2)
             << threads
             << " | Avg Time = "
             << setw(10)
             << averageTime
             << " ms"
             << " | Low = "
             << finalSummary.lowCount
             << " | Moderate = "
             << finalSummary.modCount
             << " | High = "
             << finalSummary.highCount
             << " | AvgRisk = "
             << finalSummary.avgRisk
             << " | MaxRisk = "
             << finalSummary.maxRisk
             << " | MaxCell = "
             << finalSummary.maxIndex
             << " | Warning = "
             << categoryName(
                    finalSummary.overallWarning)
             << "\n";
    }
}
int main()
{
    cout << "=====================================================\n";
    cout << " Parallel Landslide Risk Assessment\n";
    cout << " OPENMP PARALLEL VERSION\n";
    cout << "=====================================================\n";

    // Small demonstration

    runDemo();

    // Thread counts

    vector<int> threadCounts = {2, 4, 8};

    // Input sizes

    vector<int> inputSizes =
    {
        1000,
        10000,
        100000,
        1000000
    };

    // Performance experiments

    for (int n : inputSizes)
    {
        runPerformanceTest(n, threadCounts);
    }

    cout << "\n";
   

    cout << "Each performance case uses "
         << REPETITIONS
         << " repetitions.\n";

    cout << "\nSpeedup = Serial Time / Parallel Time\n";
    cout << "Efficiency = Speedup / Number of Threads\n";

   

    return 0;
}