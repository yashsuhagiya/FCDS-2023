#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <utility>
#include <functional>
#include <future>
#include <stdexcept>
#include "ThreadPool.h" // assuming the ThreadPool code is in a separate header file

int levenshtein(const std::string &s1, const std::string &s2)
{
    const int len1 = s1.size();
    const int len2 = s2.size();

    // Swap strings to ensure s1 is shorter or equal to s2
    if (len1 > len2)
        return levenshtein(s2, s1);

    std::vector<int> previous(len1 + 1); // Previous row
    std::vector<int> current(len1 + 1);  // Current row

    // Initialize the first row
    for (int i = 0; i <= len1; ++i)
        previous[i] = i;

    // Compute the distance for each character in s2
    for (int j = 1; j <= len2; ++j)
    {
        current[0] = j; // Initialize the first column

        for (int i = 1; i <= len1; ++i)
        {
            int insertion = current[i - 1] + 1;
            int deletion = previous[i] + 1;
            int substitution = previous[i - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1);
            current[i] = std::min({insertion, deletion, substitution});
        }

        // Swap previous and current rows
        std::swap(previous, current);
    }

    return previous[len1];
}

int levenshtein_rec(const std::string &s1, const std::string &s2)
{
    const int len1 = s1.size();
    const int len2 = s2.size();

    if (len1 == 0)
    {
        return len2;
    }

    if (len2 == 0)
    {
        return len1;
    }

    // compute the Levenshtein distance recursively
    int cost = s1[len1 - 1] == s2[len2 - 1] ? 0 : 1;
    int dist1 = levenshtein(s1.substr(0, len1 - 1), s2);
    int dist2 = levenshtein(s1, s2.substr(0, len2 - 1));
    int dist3 = levenshtein(s1.substr(0, len1 - 1), s2.substr(0, len2 - 1)) + cost;

    return std::min({dist1 + 1, dist2 + 1, dist3});
}

int parallel_levenshtein(const std::string &s1, const std::string &s2, ThreadPool &pool)
{
    const int len1 = s1.size();
    const int len2 = s2.size();
    std::vector<std::future<int>> futures(len1 + 1);

    // enqueue each row of the dp table as a task to be executed by a worker thread
    for (int i = 0; i <= len1; ++i)
    {
        futures[i] = pool.enqueue(levenshtein_rec, s1.substr(0, i), s2);
    }

    // wait for all tasks to complete and return the last element of the dp table
    int result = futures[len1].get();
    for (int i = 0; i < len1; ++i)
    {
        futures[i].wait();
    }
    return result;
}

int main()
{
    std::string s;
    std::string t;
    std::getline(std::cin, s);
    std::getline(std::cin, t);

    const int len1 = s.size();
    const int len2 = t.size();

    // use a single thread for small input sizes
    if (len1 <= 10 && len2 <= 10)
    {
        return levenshtein(s, t);
    }

    // create a thread pool with available cpus worker threads
    ThreadPool pool(std::thread::hardware_concurrency());

    // compute the Levenshtein distance in parallel
    int distance = parallel_levenshtein(s, t, pool);

    std::cout << distance << std::endl;

    return 0;
}
