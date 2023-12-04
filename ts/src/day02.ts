import * as fs from "fs";
import { assert } from "./assert"

class Set {
	red: number;
	green: number;
	blue: number;

    constructor(r: number, g: number, b: number) {
        this.red = r;
        this.green = g;
        this.blue = b;
    }

    isPossible(maxSet: Set): boolean {
		return this.red <= maxSet.red
			&& this.green <= maxSet.green
			&& this.blue <= maxSet.blue;
	}

	getPower(): number {
        return this.red * this.green * this.blue;
    }
};

class Game {
	id: number = 0;
	sets: Set[] = [];

	getMinPossibleSet(): Set {
        let set: Set = new Set(0, 0, 0);

        this.sets.forEach((s:Set) => {
            set.red = Math.max(set.red, s.red);
            set.green = Math.max(set.green, s.green);
            set.blue = Math.max(set.blue, s.blue);
        });

        return set;
	}
};

function parseGame(gameString: string): Game {
	let game = new Game();
    assert(game.id === 0);
	game.sets.push(new Set(0, 0, 0));

	let currentNum: number = 0;

	gameString.split(' ').forEach((token: string) => {
		let set = game.sets.at(game.sets.length - 1);

		if (token == "Game")
			return;
		else if (game.id === 0)
			game.id = parseInt(token);
		else if ('0' <= token[0] && token[0] <= '9')
			currentNum = parseInt(token);
		else if (token.includes("red"))
			set.red = currentNum;
		else if (token.includes("green"))
			set.green = currentNum;
		else if (token.includes("blue"))
			set.blue = currentNum;
		else
			assert(false);

		if (token[token.length - 1] == ';')
            game.sets.push(new Set(0, 0, 0));
	});

	return game;
}

function loadGames(filename: string): Array<Game>
{
	let games: Game[] = [];

    const data: string = fs.readFileSync(filename, "utf-8");
    const lines: string[] = data.split('\n');

    lines.forEach((line: string) => {
        let game: Game = parseGame(line);
        games.push(game);
        assert(game.id == games.length, "id: " + game.id + ", length: " + games.length);
    });

	return games;
}

function partOne(games: Game[]): number
{
    const maxSet: Set = new Set(12, 13, 14);

    return games.reduce<number>((sum: number, game: Game) => {
        return game.sets.every((set: Set) => { return set.isPossible(maxSet)})
            ? sum + game.id
            : sum;
    }, 0);
}

function partTwo(games: Game[]): number
{
    return games.reduce<number>((sum: number, game: Game) => {
		return sum + game.getMinPossibleSet().getPower();
	}, 0);
}

function process(filename: string) {
	const games: Array<Game> = loadGames(filename);
	return { first: partOne(games), second: partTwo(games) };
}

function processPrintAndAssert(filename: string, expected = { first: 0, second: 0 })
{
	const result = process(filename);
    console.log("Part 1: " + result.first + " Part 2: " + result.second);
	assert(!expected || expected.first === 0 || result.first === expected.first);
	assert(!expected || expected.second === 0 || result.second === expected.second);
}

export function day02() {
	processPrintAndAssert("../data/02/test.txt", { first: 8, second: 2286 });
	processPrintAndAssert("../data/02/real.txt", { first: 2447, second: 56322 });
}
