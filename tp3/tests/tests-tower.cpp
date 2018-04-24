#include <memory>
#include <sstream>
#include "catch.hpp"
#include "../tower.hpp"

TEST_CASE( "Tower::size()" )
{
	Tower tower(100);
	REQUIRE( tower.size() == 0 );

	auto block = std::make_shared<const Block>(1, 2, 3);
	tower.insert(block);
	REQUIRE( tower.size() == 1 );

	tower.remove(tower.cbegin());
	REQUIRE( tower.size() == 0 );
}

TEST_CASE( "Tower::begin()" )
{
	Tower tower(100);
	auto b1 = std::make_shared<const Block>(1, 2, 3);
	tower.insert(b1);
	const Block b2(1, 2, 3);
	REQUIRE( **tower.begin() == b2 );
}

TEST_CASE( "Tower::end()" )
{
	Tower tower(100);
	auto b1 = std::make_shared<const Block>(1, 2, 3);
	tower.insert(b1);
	const Block b2(1, 2, 3);
	REQUIRE( **std::prev(tower.end()) == b2 );
}

TEST_CASE( "Tower::insert(BlockPtr&)" )
{
	Tower tower(10);
	const Block b1(1, 1, 1);
	const Block b2(2, 2, 2);
	const Block b3(3, 3, 3);
	const Block b4(4, 4, 4);
	auto bp1 = std::make_shared<const Block>(b1);
	auto bp2 = std::make_shared<const Block>(b2);
	auto bp3 = std::make_shared<const Block>(b3);
	auto bp4 = std::make_shared<const Block>(b4);

	SECTION ( "Insert valid blocks" ) {
		const auto i2 = tower.insert(bp2);
		REQUIRE( **i2 == b2 );
		REQUIRE( tower.cbegin() == i2 );
		REQUIRE( std::prev(tower.cend()) == i2 );

		const auto i4 = tower.insert(bp4);
		REQUIRE( **i4 == b4 );
		REQUIRE( tower.cbegin() == i4 );
		REQUIRE( std::next(i4) == i2 );
		REQUIRE( std::prev(tower.cend()) == i2 );

		const auto i3 = tower.insert(bp3);
		REQUIRE( **i3 == b3 );
		REQUIRE( tower.cbegin() == i4 );
		REQUIRE( std::next(i4) == i3 );
		REQUIRE( std::next(i3) == i2 );
		REQUIRE( std::prev(tower.cend()) == i2 );

		const auto i1 = tower.insert(bp1);
		REQUIRE( **i1 == b1 );
		REQUIRE( tower.cbegin() == i4 );
		REQUIRE( std::next(i4) == i3 );
		REQUIRE( std::next(i3) == i2 );
		REQUIRE( std::next(i2) == i1 );
		REQUIRE( std::prev(tower.cend()) == i1 );

		SECTION( "Ownership is transferred" ) {
			REQUIRE( bp1.get() == nullptr );
		}
	}

	SECTION( "Insert invalid blocks" ) {
		const Block b5(55, 5, 5);
		const Block b6(4, 4, 4);
		const Block b7(5, 4, 5);
		const Block b8(1, 2, 3);
		auto bp5 = std::make_shared<const Block>(b5);
		auto bp6 = std::make_shared<const Block>(b6);
		auto bp7 = std::make_shared<const Block>(b7);
		auto bp8 = std::make_shared<const Block>(b8);

		REQUIRE( tower.insert(bp5) == tower.cend() );

		tower.insert(bp4);
		REQUIRE( tower.insert(bp6) == tower.cend() );

		tower.insert(bp2);
		REQUIRE( tower.insert(bp7) == tower.cend() );
		REQUIRE( tower.insert(bp8) == tower.cend() );

		SECTION( "Ownership is not transferred" ) {
			REQUIRE( *bp5.get() == b5 );
		}
	}
}

TEST_CASE( "Tower::remove(BlockHolder::const_iterator)" )
{
	Tower tower(10);
	const Block b1(1, 1, 1);
	const Block b2(2, 2, 2);
	const Block b3(3, 3, 3);
	const Block b4(4, 4, 4);
	auto bp1 = std::make_shared<const Block>(b1);
	auto bp2 = std::make_shared<const Block>(b2);
	auto bp3 = std::make_shared<const Block>(b3);
	auto bp4 = std::make_shared<const Block>(b4);
	auto i1 = tower.insert(bp1);
	auto i2 = tower.insert(bp2);
	auto i3 = tower.insert(bp3);
	auto i4 = tower.insert(bp4);

	REQUIRE( tower.cbegin() == i4 );
	auto r4 = tower.remove(i4);
	REQUIRE( *r4 == b4 );
	REQUIRE( tower.cbegin() == i3 );

	REQUIRE( std::next(i3) == i2 );
	REQUIRE( std::prev(i1) == i2 );
	auto r2 = tower.remove(i2);
	REQUIRE( *r2 == b2 );
	REQUIRE( std::next(i3) == i1 );
	REQUIRE( std::prev(i1) == i3 );

	REQUIRE( std::prev(tower.cend()) == i1 );
	auto r1 = tower.remove(i1);
	REQUIRE( *r1 == b1 );
	REQUIRE( std::prev(tower.cend()) == i3 );
}

TEST_CASE( "Tower::getCurrentHeight()" )
{
	Tower tower(100);
	REQUIRE( tower.getCurrentHeight() == 0 );

	auto block = std::make_shared<const Block>(1, 2, 3);
	tower.insert(block);
	REQUIRE( tower.getCurrentHeight() == 1 );

	tower.remove(tower.cbegin());
	REQUIRE( tower.getCurrentHeight() == 0 );
}

TEST_CASE( "Tower::getHeightLimit()" )
{
	Tower tower(100);
	REQUIRE( tower.getHeightLimit() == 100 );
}

TEST_CASE( "Tower::isHeightFit(const Block*)" )
{
	Tower tower(10);
	REQUIRE( tower.getCurrentHeight() == 0 );
	auto block = std::make_shared<const Block>(5, 2, 3);
	tower.insert(block);
	REQUIRE( tower.getCurrentHeight() == 5 );

	SECTION( "Should fit when under limit" ) {
		Block block(4, 2, 3);
		REQUIRE( tower.isHeightFit(&block) );
	}

	SECTION( "Should fit when at limit" ) {
		Block block(5, 2, 3);
		REQUIRE( tower.isHeightFit(&block) );
	}

	SECTION( "Should not fit when over limit" ) {
		Block block(6, 2, 3);
		REQUIRE( !tower.isHeightFit(&block) );
	}
}

TEST_CASE( "operator<<(std::ostream&, const Tower&)" )
{
	Tower tower(100);
	auto block = std::make_shared<const Block>(10, 20, 30);
	tower.insert(block);
	std::stringstream ss;
	ss << tower;
	REQUIRE( ss.str().compare("1\n10 20 30") == 0 );
}
