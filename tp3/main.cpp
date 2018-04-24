#include <fstream>
#include <memory>
#include <vector>
#include <random>
#include "block.hpp"
#include "tower.hpp"
#include "towerManager.hpp"

void
run(const unsigned int maximumHeight, std::vector<BlockPtr>& blocks, const bool printUpdates)
{
	// Taken from http://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
	std::random_device rd;
	std::mt19937 gen = std::mt19937(rd());
	std::uniform_real_distribution<> dis = std::uniform_real_distribution<>(0.0, 1.0);
	std::srand(time(NULL));
	const double DETERMINISTIC_PROBABILITY = 0.95;

	std::sort(blocks.begin(), blocks.end(),
		[](const BlockPtr& a, const BlockPtr& b) {
			return a->surfaceArea() > b->surfaceArea();
		}
	);

	TowerManager manager(maximumHeight, printUpdates);
	while(blocks.size() > 0) {
		if (dis(gen) <= DETERMINISTIC_PROBABILITY) {
			manager.insertBlock(blocks.front());
			blocks.erase(blocks.begin());
		} else {
			const auto randIndex = std::rand() % blocks.size();
			manager.insertBlock(blocks.at(randIndex));
			blocks.erase(blocks.begin() + randIndex);
		}
	}
	
	manager.moveTowers();
	manager.restack();

	manager.print();
}

int
main(const int argc, const char *argv[])
{
	auto filePath = "";
	auto printUpdates = false;

	// Read program arguments
	for (int i=1; i<argc; ++i) {
		const std::string arg(argv[i]);
		if (arg == "-e") {
			filePath = argv[++i];
		} else if (arg == "-p") {
			printUpdates = true;
		}
	}

	unsigned int maximumHeight;
	std::vector<BlockPtr> blocks;
	{
		std::fstream ex_file(filePath);

		// Read number of blocks and tower height limit
		unsigned int nbBocks;
		ex_file >> nbBocks >> maximumHeight;

		// Read blocks into vector
		unsigned int height, width, depth;
		while (ex_file >> height >> width >> depth) {
			blocks.emplace_back(std::make_shared<const Block>(height, width, depth));
		}
	}

	run(maximumHeight, blocks, printUpdates);
}
