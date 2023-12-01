import * as fs from "fs";

class DigitString {
    text: string;
    digit: number;

    constructor(text: string, digit: number) {
        this.text = text;
        this.digit = digit;
    }
};

class DigitPosition {
    digit: number;
    position: number;

    constructor(digit: number, position: number) {
        this.digit = digit;
        this.position = position;
    }
};

const digitStrings: Array<DigitString> = [
	new DigitString("0", 0),
	new DigitString("1", 1),
	new DigitString("2", 2),
	new DigitString("3", 3),
	new DigitString("4", 4),
	new DigitString("5", 5),
	new DigitString("6", 6),
	new DigitString("7", 7),
	new DigitString("8", 8),
	new DigitString("9", 9),
	new DigitString("zero",   0),
	new DigitString("one",    1),
	new DigitString("two",    2),
	new DigitString("three",  3),
	new DigitString("four",   4),
	new DigitString("five",   5),
	new DigitString("six",    6),
	new DigitString("seven",  7),
	new DigitString("eight",  8),
	new DigitString("nine",   9),
];

function process(filename: string, considerText: boolean): number {
    const data: string = fs.readFileSync(filename, "utf-8");
    const lines: string[] = data.split('\n');

	let sum: number = 0;

    lines.forEach((line: string) => {
		let digitsAtPositions: DigitPosition[] = [];

        digitStrings.forEach((toFind: DigitString) => {
			if (!considerText && toFind.text.length != 1)
				return;

			const firstPos: number = line.indexOf(toFind.text);
			if (firstPos == -1)
                return;

            digitsAtPositions.push(new DigitPosition(toFind.digit, firstPos));

			const lastPos: number = line.lastIndexOf(toFind.text);
			if (firstPos != lastPos)
                digitsAtPositions.push(new DigitPosition(toFind.digit, lastPos));
        });

        digitsAtPositions.sort((lhs: DigitPosition, rhs: DigitPosition): number => {
			return lhs.position - rhs.position;
        });

		const first: number = digitsAtPositions.at(0).digit;
        const last : number = digitsAtPositions.at(digitsAtPositions.length - 1).digit;

		const num: number = (first * 10) + last;
		sum += num;
    });

	return sum;
}

export function day01() {
    console.log(process("../data/01/test.txt", false));
    console.log(process("../data/01/real.txt", false));
    console.log(process("../data/01/test2.txt", true));
    console.log(process("../data/01/real.txt", true));
}
