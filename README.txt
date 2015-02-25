This program illustrates the Shannon-Fano coding named after Claude Shannon and Robert Fano.
The technique constructs a low redundancy prefix code based on a set of symbols and their probabilities.

Shannon Fano Algorithm:

1. Create a list of symbols with their probabilities and their code (initialized to '')
2. Sort by probability from highest to lowest
3. Splitt the list into two so that the sum of probabilities of each partial list is as close to the other as possible
4. append a '0' to the codes of all symbols in the first list and a '0' to all codes in the other
5. Recursivly apply steps 3 and 4 to both lists (as long as each holds more than one symbol)
