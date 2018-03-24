#include <sstream>

class Block
{
public:
	Block(const unsigned int height,
		  const unsigned int width,
		  const unsigned int depth);
	Block(const Block& block);

	unsigned int getHeight() const;
	unsigned int getWidth() const;
	unsigned int getDepth() const;
	bool isStackableOn(const Block &block) const;
	float surfaceArea() const;

	bool operator==(const Block &r) const;
	friend std::ostream& operator<<(std::ostream& os, const Block& b);

private:
	unsigned int height;
	unsigned int width;
	unsigned int depth;
};

Block::Block(const unsigned int height,
			 const unsigned int width,
			 const unsigned int depth)
	: height(height),
	  width(width),
	  depth(depth)
{}

Block::Block(const Block& block)
	: height(block.height),
	  width(block.width),
	  depth(block.depth)
{}

unsigned int
Block::getHeight() const
{
	return height;
}

unsigned int
Block::getWidth() const
{
	return width;
}

unsigned int
Block::getDepth() const
{
	return depth;
}

bool
Block::isStackableOn(const Block &block) const
{
	return width < block.width
	    && depth < block.depth;
}

float
Block::surfaceArea() const
{
	return 1.0f * width * depth;
}

bool
Block::operator==(const Block &r) const
{
	return height == r.getHeight()
	    && width == r.getWidth()
	    && depth == r.getDepth();
}

std::ostream&
operator<<(std::ostream& os, const Block& b)
{
	os << std::to_string(b.height) << " " << std::to_string(b.width) << " " << std::to_string(b.depth);
	return os;
}
