#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <ranges>
#include <string>
#include <vector>
#include <unordered_map>

struct Game07
{
	enum class Hand
	{
		None,
		HighCard,
		OnePair,
		TwoPair,
		ThreeOfAKind,
		FullHouse,
		FourOfAKind,
		FiveOfAKind
	};

	std::string cardsAsString;
	uint32_t bid = 0;

	std::vector<int> cards;
	std::unordered_map<int, int> cardCounts;
	Hand hand = Hand::None;

	explicit Game07(const std::string& _cards, uint32_t _bid)
		: cardsAsString(_cards)
		, bid(_bid)
	{
	}

	void reset()
	{
		*this = Game07(cardsAsString, bid);
	}

	void evaluatePart1()
	{
		reset();
		convertStringToCardsWithValues("0123456789TJQKA");
		countCards();
		evaluateHand();
	}

	void evaluatePart2()
	{
		reset();
		convertStringToCardsWithValues("J123456789T_QKA");
		countCards();
		evaluateHand();

		for (int i = 0, jokerCount = cardCounts[0]; i < jokerCount; ++i)
			bumpHandValue();
	}

	void convertStringToCardsWithValues(const std::string& ordering)
	{
		for (const char c : cardsAsString)
			cards.push_back((int) ordering.find(c));

		assert(std::find(begin(cards), end(cards), -1) == end(cards));
	}

	void countCards()
	{
		for (int card : cards)
			cardCounts[card]++;
	}

	void evaluateHand()
	{
		const int highestCardCount = std::accumulate(
			begin(cardCounts),
			end(cardCounts),
			0,
			[] (int count, auto iter) { return std::max(count, iter.first != 0 ? iter.second : 0); }
		);

		const int pairCount = std::accumulate(
			begin(cardCounts),
			end(cardCounts),
			0,
			[] (int sum, auto iter) { return sum + (iter.first != 0 && iter.second == 2); }
		);

		switch (highestCardCount)
		{
			case 5: hand = Hand::FiveOfAKind; break;
			case 4: hand = Hand::FourOfAKind; break;
			case 3: hand = (pairCount == 1) ? Hand::FullHouse : Hand::ThreeOfAKind; break;
			case 2: hand = (pairCount == 1) ? Hand::OnePair : Hand::TwoPair; break;
			case 1: hand = Hand::HighCard; break;
			case 0: hand = Hand::None; break;
			default: assert(false); break;
		}
	}

	void bumpHandValue()
	{
		switch (hand)
		{
			case Hand::None: hand = Hand::HighCard; break;
			case Hand::HighCard: hand = Hand::OnePair; break;
			case Hand::OnePair: hand = Hand::ThreeOfAKind; break;
			case Hand::TwoPair: hand = Hand::FullHouse; break;
			case Hand::ThreeOfAKind: hand = Hand::FourOfAKind; break;
			case Hand::FullHouse: assert(false); break;
			case Hand::FourOfAKind: hand = Hand::FiveOfAKind; break;
			case Hand::FiveOfAKind: assert(false); break;
		}
	}

	bool operator<(const Game07& rhs) const
	{
		if (hand != rhs.hand)
			return hand < rhs.hand;
		else
			return cards < rhs.cards;
	}

	static const char* toString(Hand hand)
	{
		switch (hand)
		{
			case Hand::None: return "None";
			case Hand::HighCard: return "HighCard";
			case Hand::OnePair: return "OnePair";
			case Hand::TwoPair: return "TwoPair";
			case Hand::ThreeOfAKind: return "ThreeOfAKind";
			case Hand::FullHouse: return "FullHouse";
			case Hand::FourOfAKind: return "FourOfAKind";
			case Hand::FiveOfAKind: return "FiveOfAKind";
		}

		assert(false);

		return "<Unknown>";
	}

	void printHand() const
	{
		std::cout << cardsAsString << ": " << toString(hand) << std::endl;
	}
};

static auto loadData(const char* filename)
{
	std::fstream s{ filename, s.in };
	assert(s.is_open());

	std::vector<Game07> games;

	while (s.peek() != EOF)
	{
		std::string line;
		std::getline(s, line);
		assert(line.size() > 0);

		std::string cards;
		int bid = -1;

		for (const auto word : std::views::split(line, ' '))
		{
			const std::string token(&*word.begin(), std::ranges::distance(word));

			if (cards.size() == 0)
				cards = token;
			else
				bid = std::stoi(token);
		}

		games.emplace_back(cards, bid);
	}

	return games;
}

static uint64_t calculateWinnings(const auto& data)
{
	uint64_t money = 0;

	for (size_t i = 0; i < data.size(); ++i)
		money += (i + 1) * data[i].bid;

	return money;
}

static uint64_t partOne(auto& games)
{
	for (auto& game : games)
		game.evaluatePart1();

	std::sort(begin(games), end(games));

	std::cout << "------ Part One ------" << std::endl;
	for (auto& game : games)
		game.printHand();

	return calculateWinnings(games);
}

static uint64_t partTwo(auto& games)
{
	for (auto& game : games)
		game.evaluatePart2();

	std::sort(begin(games), end(games));

	std::cout << "------ Part Two ------" << std::endl;
	for (auto& game : games)
		game.printHand();

	return calculateWinnings(games);
}

static std::pair<uint64_t, uint64_t> process(const char* filename)
{
	auto games = loadData(filename);
	return std::make_pair(partOne(games), partTwo(games));
}

static void processPrintAndAssert(const char* filename, std::optional<std::pair<uint64_t, uint64_t>> expected = {})
{
	const auto result = process(filename);
	std::cout << "Part 1: " << result.first << " Part 2: " << result.second << std::endl;

	assert(!expected || expected.value().first == 0 || result.first == expected.value().first);
	assert(!expected || expected.value().second == 0 || result.second == expected.value().second);
}

void day07()
{
	processPrintAndAssert("../data/07/test.txt", std::make_pair(6440ull, 5905ull));
	processPrintAndAssert("../data/07/real.txt", std::make_pair(250951660ull, 251481660ull));
}
