### Copyright 2024 ~ Toma-Ioan Dumitrescu

### Regex generator

### Description:

The project takes as input a language Accept which consists in a finite set of words, and
also a language Fail. The program will return a reasonable regex that will accept the
words from Accept, and will dismiss the words from Fail.

### Algorithm:

Because of the constraints, there is no utility in using Kleene star or + symbol (more time
for checking if the regex is correct, especially for *, +, because of having the variable
of the number of repetitions of that letter that needs to be find).

The solution idea consists in generating a range of letters for each letter in Accept,
and a range for each letter in Fail. Now, evidently, the resulted regex will be the range
for letters found in Accept at every letter position. This works only when there exists
an "Accept range" from position p (let it I), and an "Fail range" from position p (let it J),
so that (I, J) are disjoint sets.

So, if (I, J) disjoint found for the same position, then the regex will be .....I.......,
where I is in the p position. In more complex cases, I will be a union of intervals. Since
no intervals like [a-z] are allowed, an interval will be considered as (letter1 | letter2 | etc).

However, a counterexample is:

Accept: ab, bc, cd, de, ea
Fail: ba, cb, dc, ed, ae

From this point, intuitively, my algorithm searches for (I, J) with minimum common elements,
and from there take a minimum neighborhood of positions (minimum epsilon for the ball) so that
the program returns correct results, and, instead of range output for those positions, chunks
from Accept words separated by | will be displayed.
So, in this case, the solution will be:
(ab | bc | cd | de | ea)

This approach is helpful in many situations in which the words are very big (and not so problematic
in terms of time exection for short input), only small chunks | separated being displayed, since
the card(I intersected J) = minimum, the rest being only points.

For every approach, there will be worst cases when all words will be put in the OR chain, because
of no Accept pattern or no Fail pattern. The purpose of my algorithm is to minimize the number
of worst cases that need a lot of computation and memory.

Let N be the number of words, and L the length of a word, then:

Space complexity: O(L)
Time complexity: O(N * (L ^ 2))

### Implementation:

main(): read and write in files

calculateMinPos(---): finds the position (minPos) in 0...(lenStr - 1) to minimize the number of
common letters on that column (minPos) of the input. Because the algorithm of minimum will lead
minPos to be probably at edges (thus not able to expand both ways, and lowering the probability
to get smaller regex), I added the AI function that guesses based on the length of the string
where the minPos should be found when minCommon is achieved by more positions (AI should also
modify the formula based on training sets, but it's not the case for this program).

getRegex(---): if minCommon is 0, then the sequence S has been found (OR-chaining all letters
at minPos from every Accept-word). If minCommon is not equal with 0, then the euristic part
of the algorithm is triggered. Starting from minPos, 1 step at the time increase the length
of the minPos neighborhood (by decreasing start, or by increasing end variables). Now, the
"words" taken in consideration will be the chunks from [start, end] of each line of the
input file. The chunks from the Accept (which may be added to the regex by OR-chaining) will
be added in a vector to be tested to not have an equal word in Fail, thus the Fail chunks
will be added in a hashmap<string, bool> for efficient retrieve and store. After the disjoint
sets test, the regex can be created, because we found the sequence S of OR-chained chunks present
in Accept, and not present in Fail. From [0, start - 1] we can have any character, so put '.' in
the regex. For [start, end] we put in the regex the elements from the accChunks vector OR-separated,
but no duplicates are allowed (for this I reused the failChunks hashmap to register the element visit).
Then fill with '.' to complete lenStr (this can also be done with .* to have shorter regexes).

### Optimization:

This euristic can be further optimized, by calculating for each i = 0, lenStr - 1 the number of
common letters, and choose the starting position minPos at the mid of the interval with minimum
density of common values (a way to calculate the density could be summing the values of number common
values from start to end). So, when calculating the probability of finding a smaller regex starting
with the neighborhood of minPos, we get a more accurate result with densities on arrays of minCommon
than a single minCommon. For example:

Suppose the cardinals of Ik intersected with Jk = Ck; k = 1, n; n > 50. We need to find a
sequence S = (s1 | s2 | ... | sp) so that S is in Accept, and S is not in Fail.

(C1 C2 ... Cn)

Then if C1 = 1, C2 = 100; C7 = 3, C8 = 3, C9 = 2, C10 = 3, C11 = 2, C12 = 3, my algorithm will
start with C1 instead of C9, even though it's more probable that the sequence S will be found
starting from C9 because of its neighborhood values.
C1 + C2 = 101
C7 + C8 + ... + C12 = 16
