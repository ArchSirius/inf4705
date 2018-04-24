#include <algorithm>
#include <list>
#include <memory>
#include <sstream>
#include "block.hpp"

#pragma once

typedef std::shared_ptr<const Block> BlockPtr;
typedef std::list<BlockPtr> BlockHolder;

class Tower : private BlockHolder
{
public:
	Tower(const unsigned int heightLimit);
	Tower(const Tower& tower);
	~Tower() = default;
	decltype(std::declval<BlockHolder>().size()) size() const noexcept;
	decltype(std::declval<BlockHolder>().begin()) begin() noexcept;
	decltype(std::declval<BlockHolder>().end()) end() noexcept;
	decltype(std::declval<BlockHolder>().cbegin()) cbegin() const noexcept;
	decltype(std::declval<BlockHolder>().cend()) cend() const noexcept;
	BlockHolder::const_iterator insert(BlockPtr& block);
	BlockPtr remove(BlockHolder::const_iterator position);
	decltype(std::declval<Block>().getHeight()) getCurrentHeight() const noexcept;
	unsigned int getHeightLimit() const noexcept;
	bool isFit(const Block* block) const;
	bool isHeightFit(const Block* block) const;
	friend std::ostream& operator<<(std::ostream& os, const Tower& t);

private:
	BlockHolder::const_iterator insert(BlockHolder::const_iterator position, BlockPtr& block);
	decltype(std::declval<Block>().getHeight()) currentHeight = 0;
	unsigned int heightLimit;
};

Tower::Tower(const unsigned int heightLimit)
	: BlockHolder(),
	  heightLimit(heightLimit)
{}

Tower::Tower(const Tower& tower)
	: BlockHolder(),
	  heightLimit(heightLimit)
{
	for (auto block = tower.cbegin(); block != tower.cend(); ++block) {
		emplace_back(std::make_shared<const Block>(**block));
		currentHeight += (*block)->getHeight();
	}
}

decltype(std::declval<BlockHolder>().size())
Tower::size() const noexcept
{
	return BlockHolder::size();
}

decltype(std::declval<BlockHolder>().begin())
Tower::begin() noexcept
{
	return BlockHolder::begin();
}

decltype(std::declval<BlockHolder>().end())
Tower::end() noexcept
{
	return BlockHolder::end();
}

decltype(std::declval<BlockHolder>().cbegin())
Tower::cbegin() const noexcept
{
	return BlockHolder::cbegin();
}

decltype(std::declval<BlockHolder>().cend())
Tower::cend() const noexcept
{
	return BlockHolder::cend();
}

BlockHolder::const_iterator
Tower::insert(BlockPtr& block)
{
	if (!isHeightFit(block.get())) {
		return cend();
	}
	if (empty() || (*cbegin())->isStackableOn(block.get())) {
		return insert(cbegin(), block);
	}
	for (auto it = cbegin(); it != std::prev(cend()); ++it) {
		if (block->isStackableOn(it->get()) && (*std::next(it))->isStackableOn(block.get())) {
			return insert(std::next(it), block);
		}
	}
	if (block->isStackableOn(std::prev(cend())->get())) {
		return insert(cend(), block);
	}
	return cend();
}

BlockPtr
Tower::remove(BlockHolder::const_iterator position)
{
	auto block = *position;
	erase(position);
	currentHeight -= block->getHeight();
	return block;
}

decltype(std::declval<Block>().getHeight())
Tower::getCurrentHeight() const noexcept
{
	return currentHeight;
}

unsigned int
Tower::getHeightLimit() const noexcept
{
	return heightLimit;
}

bool
Tower::isFit(const Block* block) const
{
	if (!isHeightFit(block)) {
		return false;
	}
	if (empty() || (*cbegin())->isStackableOn(block)) {
		return true;
	}
	for (auto it = cbegin(); it != std::prev(cend()); ++it) {
		if (block->isStackableOn(it->get()) && (*std::next(it))->isStackableOn(block)) {
			return true;
		}
	}
	if (block->isStackableOn(std::prev(cend())->get())) {
		return true;
	}
	return false;
}

bool
Tower::isHeightFit(const Block* block) const
{
	return block->getHeight() + currentHeight <= heightLimit;
}

std::ostream&
operator<<(std::ostream& os, const Tower& t)
{
	os << std::to_string(t.size());
	for (auto it = t.cbegin(); it != t.cend(); ++it) {
		os << std::endl << **it;
	}
	return os;
}

BlockHolder::const_iterator
Tower::insert(BlockHolder::const_iterator position, BlockPtr& block)
{
	currentHeight += block->getHeight();
	return BlockHolder::insert(position, std::move(block));
}
