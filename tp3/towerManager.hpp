#include <algorithm>
#include <iostream>
#include <random>
#include <vector>
#include "tower.hpp"

#pragma once

class TowerManager
{
public:
	TowerManager(const unsigned int towerHeightLimit, const bool printUpdates);
	void insertBlock(BlockPtr& block);
	void moveTowers();
	size_t nbTowers() const;
	void print() const;
	std::vector<BlockPtr> releaseHorizontal();
	std::vector<BlockPtr> releaseVertical();
	std::vector<BlockPtr> releaseBySize(const bool smallest);
	void reinsert(std::vector<BlockPtr>& blocks);
	void restack();
	bool checkPostRestack(const std::vector<Tower>& previousSolution) const;

private:
	bool moveTower(const unsigned int pos);
	unsigned int towerHeightLimit;
	bool printUpdates;
	std::vector<Tower> towers;
	std::mt19937 gen;
	std::uniform_real_distribution<> dis;
	const double DETERMINISTIC_PROBABILITY = 0.6;
	const unsigned int MAX_ITERATIONS_WITHOUT_IMPROVEMENT = 100;
};

TowerManager::TowerManager(const unsigned int towerHeightLimit, const bool printUpdates)
	: towerHeightLimit(towerHeightLimit),
	  printUpdates(printUpdates)
{
	// Taken from http://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
	std::random_device rd;
	gen = std::mt19937(rd());
	dis = std::uniform_real_distribution<>(0.0, 1.0);
	std::srand(time(NULL));
}

void
TowerManager::insertBlock(BlockPtr& block)
{
	for (auto it = towers.begin(); it != towers.end(); ++it) {
		if (it->insert(block) != it->cend()) {
			return;
		}
	}
	towers.emplace_back(towerHeightLimit);
	towers.back().insert(block);
}

void
TowerManager::moveTowers()
{
	auto hasMoved = false;
	auto nbTowers = towers.size();
	do {
		hasMoved = false;
		for (auto i = 0; i < towers.size(); ++i) {
			hasMoved |= moveTower(i);
		}
		towers.erase(std::remove_if(towers.begin(), towers.end(), [](const Tower& tower) {
			return tower.size() == 0;
		}), towers.end());
		if (printUpdates && towers.size() < nbTowers) {
			print();
		}
	} while (hasMoved);
}

bool
TowerManager::moveTower(const unsigned int pos)
{
	auto hasMoved = false;
	auto sourceTower = towers.begin() + pos;
	for (auto block = sourceTower->begin(); block != sourceTower->end();) {
		auto next = std::next(block);
		if (dis(gen) <= DETERMINISTIC_PROBABILITY) {
			for (auto targetTower = towers.begin() + pos + 1; targetTower != towers.end(); ++targetTower) {
				if (targetTower->isFit(block->get())) {
					auto movedBlock = sourceTower->remove(block);
					targetTower->insert(movedBlock);
					hasMoved = true;
					break;
				}
			}
		} else {
			const auto randIndex = std::rand() % (towers.size() - pos) + pos;
			const auto targetTower = towers.begin() + randIndex;
			if (targetTower->isFit(block->get())) {
				auto movedBlock = sourceTower->remove(block);
				targetTower->insert(movedBlock);
				hasMoved = true;
			}
		}
		block = next;
	}
	return hasMoved;
}

size_t
TowerManager::nbTowers() const
{
	return towers.size();
}

void
TowerManager::print() const
{
	for (auto it = towers.cbegin(); it != towers.cend(); ++it) {
		std::cout << *it << std::endl;
	}
	std::cout << "fin" << std::endl;
}

std::vector<BlockPtr>
TowerManager::releaseHorizontal()
{
	std::vector<BlockPtr> blocks;
	while (towers.begin() != towers.end()) {
		for (auto tower = towers.begin(); tower != towers.end(); ++tower) {
			if (tower->begin() != tower->end()) {
				blocks.push_back(tower->remove(tower->begin()));
			}
		}
		towers.erase(std::remove_if(towers.begin(), towers.end(), [](const Tower& tower) {
			return tower.size() == 0;
		}), towers.end());
	}
	towers.clear();
	return blocks;
}

std::vector<BlockPtr>
TowerManager::releaseVertical()
{
	std::vector<BlockPtr> blocks;
	for (auto tower = towers.rbegin(); tower != towers.rend(); ++tower) {
		while (tower->begin() != tower->end()) {
			blocks.push_back(tower->remove(tower->begin()));
		}
	}
	towers.clear();
	return blocks;
}

std::vector<BlockPtr>
TowerManager::releaseBySize(const bool smallest)
{
	std::vector<BlockPtr> blocks;
	std::sort(towers.begin(), towers.end(), [smallest](const Tower& a, const Tower& b) {
		if (smallest) {
			return a.size() > b.size();
		}
		return a.size() < b.size();
	});
	return releaseHorizontal();
}

void
TowerManager::reinsert(std::vector<BlockPtr>& blocks)
{
	for (auto it = blocks.begin(); it != blocks.end(); ++it) {
		insertBlock(*it);
	}
	moveTowers();
}

void
TowerManager::restack()
{
	auto previousSolution = towers;
	auto nbIterationsWithoutImprovement = 0;
	while (nbIterationsWithoutImprovement++ < MAX_ITERATIONS_WITHOUT_IMPROVEMENT) {
		auto blocks = releaseVertical();
		reinsert(blocks);
		if (checkPostRestack(previousSolution)) {
			nbIterationsWithoutImprovement = 0;
			previousSolution = towers;
		}

		blocks = releaseHorizontal();
		reinsert(blocks);
		if (checkPostRestack(previousSolution)) {
			nbIterationsWithoutImprovement = 0;
			previousSolution = towers;
		}

		blocks = releaseBySize(true);
		reinsert(blocks);
		if (checkPostRestack(previousSolution)) {
			nbIterationsWithoutImprovement = 0;
			previousSolution = towers;
		}
	}
	towers = previousSolution;
}

bool
TowerManager::checkPostRestack(const std::vector<Tower>& previousSolution) const
{
	if (towers.size() < previousSolution.size()) {
		if (printUpdates) {
			print();
		}
		return true;
	}
	return false;
}
