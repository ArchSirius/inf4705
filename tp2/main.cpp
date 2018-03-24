#include <algorithm>
#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include "block.hpp"

const unsigned int TABOU_COOLDOWN_MIN_DURATION = 7;
const unsigned int TABOU_COOLDOWN_MAX_DURATION = 10;
const unsigned int TABOU_MAXIMUM_ITERATIONS_BELOW_THRESHOLD = 100;
const double VORACE_DETERMINISTIC_PROBABILITY = 0.95;
const double VORACE_RANDOM_MAX_ATTEMPTS = 10;

auto tower = std::vector<const Block*>();
std::map<const Block*, unsigned int> tabouList;

decltype(std::declval<Block>().getHeight())
getTowerHeight(decltype(tower)& tower)
{
	// Taken from http://en.cppreference.com/w/cpp/algorithm/accumulate
	return std::accumulate(tower.begin(), tower.end(), 0,
		[](const decltype(std::declval<Block>().getHeight()) a, const Block* b) {
			return a + b->getHeight();
		});
}

void
makeTabou(const Block* block)
{
	const auto duration = (std::rand() %
		(TABOU_COOLDOWN_MAX_DURATION - TABOU_COOLDOWN_MIN_DURATION + 1))
		+ TABOU_COOLDOWN_MIN_DURATION;
	tabouList[block] = duration;
}

bool
isTabou(const Block* block)
{
	const auto it = tabouList.find(block);
	return it != tabouList.end() && it->second > 0;
}

void
decrementAllBlocksInTabouList()
{
	auto it = tabouList.begin();
	for (auto it = tabouList.begin(); it != tabouList.end(); ++it) {
		if (--it->second <= 0) {
			tabouList.erase(it);
		}
	}
}

bool
isBlockValid(const Block* block)
{
	return std::find(tower.begin(), tower.end(), block) != tower.end() ||
           !isTabou(block);
}

unsigned int
highestIndexInTowerSupporting(const Block* block)
{
	auto it = tower.rbegin();
	const auto bestBaseBlock = find_if(it, tower.rend(),
		[block](const Block* b) {
			return block->isStackableOn(*b);
		});

	return tower.rend() - bestBaseBlock;
}

void
vorace(const std::vector<std::unique_ptr<const Block>>& blocks)
{
	// Taken from http://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);
	std::srand(time(NULL));

	// Copy blocks to allow use of erase()
	std::vector<const Block*> varBlocks;
	for (auto it = blocks.begin(); it < blocks.end(); ++it) {
		varBlocks.push_back(it->get());
	}

	while(!varBlocks.empty()) {
		if (dis(gen) < VORACE_DETERMINISTIC_PROBABILITY) {
			auto it = varBlocks.begin();
			for (; it < varBlocks.end(); ++it) {
				if (tower.empty() || (*it)->isStackableOn(*tower.back())) {
					tower.push_back(*it);
					break;
				}
			}
			varBlocks.erase(varBlocks.begin(), it);
		} else {
			auto attempts = 0;
			while (attempts++ < VORACE_RANDOM_MAX_ATTEMPTS) {
				const auto randIndex = std::rand() % varBlocks.size();
				if (tower.empty() || varBlocks.at(randIndex)->isStackableOn(*tower.back())) {
					tower.push_back(varBlocks.at(randIndex));
					varBlocks.erase(varBlocks.begin() + randIndex);
					break;
				}
			}
		}
	}
}

void
progdyn(const std::vector<std::unique_ptr<const Block>>& blocks)
{
	// Assuming blocks are sorted by surface area in decreasing order
	// Omitted here because it should not affect the algorithm's performance (chrono)

	// Allocate results vector
	std::vector<decltype(std::declval<Block>().getHeight())> height(blocks.size());

	// Iterate on all possible towertops
	for (auto j = 0; j < height.size(); ++j) {
		const auto currentBlock = blocks.at(j).get();
		const auto currentBlockHeight = currentBlock->getHeight();
		auto newHeight = currentBlockHeight;

		// Iterate on all previous towertops
		for (auto i = 0; i < j; ++i) {
			const auto previousHeight = height.at(i);
			const auto top = blocks.at(i).get();
			const auto candidateNewHeight = previousHeight + currentBlockHeight;
			if (currentBlock->isStackableOn(*top) && candidateNewHeight > newHeight) {
				newHeight = candidateNewHeight;
			}
		}

		// Save result
		height.at(j) = newHeight;
	}

	// Determine best result
	auto currentHeight = std::max_element(height.rbegin(), height.rend());
	auto remainingHeight = *currentHeight;
	auto currentBlock = blocks.at(height.rend() - currentHeight - 1).get();

	// Find composing blocks
	tower.push_back(currentBlock);
	remainingHeight -= currentBlock->getHeight();
	while (currentHeight < height.rend() && remainingHeight > 0) {
		currentHeight = std::find_if(currentHeight, height.rend(),
			[remainingHeight](const decltype(*currentHeight) h) {
				return h == remainingHeight;
			});
		if (currentHeight != height.rend()) {
			currentBlock = blocks.at(height.rend() - currentHeight - 1).get();
			tower.push_back(currentBlock);
			remainingHeight -= currentBlock->getHeight();
		}
	}

	// Reverse order
	std::reverse(tower.begin(), tower.end());
}

void
tabou(const std::vector<std::unique_ptr<const Block>>& blocks)
{
	// Start with a possible solution
	vorace(blocks);

	auto iterationsWithoutImprovement = 0;
	decltype(std::declval<Block>().getHeight()) bestSolution = 0;

	while (iterationsWithoutImprovement < TABOU_MAXIMUM_ITERATIONS_BELOW_THRESHOLD) {
		auto it = std::find_if(blocks.begin(), blocks.end(),
			[](const std::unique_ptr<const Block>& b) {
				return isBlockValid(b.get());
			});
		auto bestBlockForIteration = it->get();
		decltype(std::declval<Block>().getHeight()) bestSolutionForIteration = 0;
		std::vector<const Block*> removedBlocksForIteration;

		// Itate over valid blocks
		while (it != blocks.end()) {

			// Insert at valid position
			auto tempTower = decltype(tower)(tower);
			const auto index = highestIndexInTowerSupporting(it->get());
			const auto insertedBlock = tempTower.insert(tempTower.begin() + index, it->get());

			// Find first block that can fit on the new one
			const auto firstStackableBlock = find_if(insertedBlock + 1, tempTower.end(),
				[insertedBlock](const Block* b) {
					return b->isStackableOn(**insertedBlock);
				});

			std::vector<const Block*> removedBlocks = {insertedBlock + 1, firstStackableBlock};
			tempTower.erase(insertedBlock + 1, firstStackableBlock);
			const auto solution = getTowerHeight(tempTower);

			if (solution >= bestSolutionForIteration) {
				bestSolutionForIteration = solution;
				bestBlockForIteration = *insertedBlock;
				removedBlocksForIteration.assign(removedBlocks.begin(), removedBlocks.end());
			}

			it = std::find_if(++it, blocks.end(),
				[](const std::unique_ptr<const Block>& b) {
					return isBlockValid(b.get());
				});
		}

		if (bestSolutionForIteration > bestSolution) {
			iterationsWithoutImprovement = 0;
			bestSolution = bestSolutionForIteration;

			// Add block to tabou list
			makeTabou(bestBlockForIteration);
			for (const auto block : removedBlocksForIteration) {
				makeTabou(block);
			}

			const auto index = highestIndexInTowerSupporting(bestBlockForIteration);
			const auto insertedBlock = tower.insert(tower.begin() + index, bestBlockForIteration);

			// Find first block that can fit on the new one
			const auto firstStackableBlock = find_if(insertedBlock + 1, tower.end(),
				[insertedBlock](const Block* b) {
					return b->isStackableOn(**insertedBlock);
				});
			tower.erase(insertedBlock + 1, firstStackableBlock);

		} else {
			++iterationsWithoutImprovement;
		}

		// Decrement tabou
		decrementAllBlocksInTabouList();
	}
}

void
run(const std::function<void(std::vector<std::unique_ptr<const Block>>&)>& algo,
    std::vector<std::unique_ptr<const Block>>& blocks,
    const bool print_res,
    const bool print_time,
    const bool print_height)
{
	const auto start = std::chrono::steady_clock::now();
	algo(blocks);
	const auto end = std::chrono::steady_clock::now();

	if (print_time) {
		std::chrono::duration<double> s = end-start;
		std::cout << std::fixed << s.count() << std::endl;
	}

	if (print_res) {
		for (const auto block : tower) {
			std::cout << *block << std::endl;
		}
	}

	if (print_height) {
		std::cout << getTowerHeight(tower) << std::endl;
	}
}

int
main(const int argc, const char *argv[])
{
	struct {
		std::string algo;
		std::string file_path;
		bool print_res{false};
		bool print_time{false};
		bool print_height{false};
	} prog_args;

	// Read program arguments
	for (int i=1; i<argc; ++i) {
		const std::string arg(argv[i]);
		if (arg == "-a") {
			prog_args.algo = argv[i + 1];
			++i;
		} else if (arg == "-e") {
			prog_args.file_path = argv[i + 1];
			++i;
		} else if (arg == "-p") {
			prog_args.print_res = true;
		} else if (arg == "-t") {
			prog_args.print_time = true;
		} else if (arg == "--height") {
			prog_args.print_height = true;
		}
	}

	// Read blocks into vector
	std::vector<std::unique_ptr<const Block>> blocks;
	{
		std::fstream ex_file(prog_args.file_path);

		unsigned int height, width, depth;
		while (ex_file >> height >> width >> depth) {
			blocks.emplace_back(std::make_unique<const Block>(height, width, depth));
		}
	}
	// Sort by decreasing surface area
	std::sort(blocks.begin(), blocks.end(),
		[](const std::unique_ptr<const Block>& a, const std::unique_ptr<const Block>& b) {
			return a->surfaceArea() > b->surfaceArea();
		});

	// Apply correct algorithm
	if (prog_args.algo == "vorace") {
		run(vorace, blocks, prog_args.print_res, prog_args.print_time, prog_args.print_height);
	}
	else if (prog_args.algo == "progdyn") {
		run(progdyn, blocks, prog_args.print_res, prog_args.print_time, prog_args.print_height);
	}
	else if (prog_args.algo == "tabou") {
		run(tabou, blocks, prog_args.print_res, prog_args.print_time, prog_args.print_height);
	}
}
