#include <vector>
#include <fstream>
#include <functional>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <iterator>

using Int = long long;
using Algo = const std::function<void(std::vector<Int>&)>&;

void stdsort(std::vector<Int>& numbers) {
    std::sort(numbers.begin(), numbers.end());
}

int cmpfunc (const void * a, const void * b) {
    // cmpfunc doit renvoyer un int. On ne peut pas renvoyer directement
    // res puisqu'il pourrait y avoir un overflow. Nous devons faire
    // un traitement de plus qui ralentira le tri.
    auto res = *(Int*)a - *(Int*)b;
    if (res < 0) return -1;
    else if (res > 0) return 1;
    else return 0;
}

void c_qsort(std::vector<Int>& numbers) {
    qsort(&numbers[0], numbers.size(), sizeof(Int), cmpfunc);
}

/*
 * Source : http://rosettacode.org/wiki/Sorting_algorithms/Insertion_sort#C.2B.2B
 */
template <typename RandomAccessIterator, typename Predicate>
void insertion_sort(RandomAccessIterator begin, RandomAccessIterator end,
                    Predicate p) {
  for (auto i = begin; i != end; ++i) {
    std::rotate(std::upper_bound(begin, i, *i, p), i, i + 1);
  }
}

template <typename RandomAccessIterator>
void insertion_sort(RandomAccessIterator begin, RandomAccessIterator end) {
  insertion_sort(
      begin, end,
      std::less<
          typename std::iterator_traits<RandomAccessIterator>::value_type>());
}

void insertionSort(std::vector<Int>& numbers) {
    insertion_sort(std::begin(numbers), std::end(numbers));
}

/*
 * Source : http://rosettacode.org/wiki/Sorting_algorithms/Merge_sort#C.2B.2B
 */
template<typename RandomAccessIterator, typename Order>
 void mergesort(RandomAccessIterator first, RandomAccessIterator last, Order order)
{
  if (last - first > 1)
  {
    RandomAccessIterator middle = first + (last - first) / 2;
    mergesort(first, middle, order);
    mergesort(middle, last, order);
    std::inplace_merge(first, middle, last, order);
  }
}
 
template<typename RandomAccessIterator>
 void mergesort(RandomAccessIterator first, RandomAccessIterator last)
{
  mergesort(first, last, std::less<typename std::iterator_traits<RandomAccessIterator>::value_type>());
}

void mergeSort(std::vector<Int>& numbers) {
    mergesort(std::begin(numbers), std::end(numbers));
}

void mergeSeuilSort(std::vector<Int>& numbers) {
    if (numbers.size() < 1250) {
        insertion_sort(std::begin(numbers), std::end(numbers));
    }
    else {
        mergesort(std::begin(numbers), std::end(numbers));
    }
}

void run(Algo algo, std::vector<Int>& numbers, bool print_res, bool print_time) {
    using namespace std::chrono;
    auto start = steady_clock::now();
    algo(numbers);
    auto end = steady_clock::now();

    if (print_time) {
        duration<double> s = end-start;
        std::cout << std::fixed << s.count() << std::endl;
    }

    if (print_res) {
        std::cout << std::fixed;
        for (auto n : numbers)
            std::cout << n << std::endl;
    }
}

int main(int argc, char *argv[]) {
    struct {
        std::string algo;
        std::string file_path;
        bool print_res{false};
        bool print_time{false};
    } prog_args;

    // Read program arguments
    for (int i=1; i<argc; i++) {
        std::string arg(argv[i]);
        if (arg == "-a") {
            prog_args.algo = argv[i+1]; i++;
        } else if (arg == "-e") {
            prog_args.file_path = argv[i+1]; i++;
        } else if (arg == "-p") {
            prog_args.print_res = true;
        } else if (arg == "-t") {
            prog_args.print_time = true;
        }
    }

    // Read numbers into vector
    std::vector<Int> numbers;
    {
        std::fstream ex_file(prog_args.file_path);
        Int n;
        while (ex_file >> n)
            numbers.push_back(n);
    }

    // Apply correct algorithm
    if (prog_args.algo == "stdsort")
        run(stdsort, numbers, prog_args.print_res, prog_args.print_time);
    else if(prog_args.algo == "qsort")
        run(c_qsort, numbers, prog_args.print_res, prog_args.print_time);
    else if(prog_args.algo == "insertion")
        run(insertionSort, numbers, prog_args.print_res, prog_args.print_time);
    else if(prog_args.algo == "merge")
        run(mergeSort, numbers, prog_args.print_res, prog_args.print_time);
    else if(prog_args.algo == "mergeSeuil")
        run(mergeSeuilSort, numbers, prog_args.print_res, prog_args.print_time);
}
