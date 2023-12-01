import * as fs from "fs";

export function day01() {
    const filename = "../data/01/test.txt";
    const data = fs.readFileSync(filename, "utf-8");
    
    console.log(data);
}
