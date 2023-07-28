# Navi

Navi is a lisp-inspired programming language designed for being fast to code in whilst also being readable. 

## Documentation

Information on the built-in functions and standard library can be found on the [wiki](https://github.com/ImNotJahan/Navi/wiki).

## Installing

On windows all you need to do is download the latest navi-windows release and unzip it. Running navi.exe will start the REPL with library.navi automatically loaded. For info on the command-line options, see the [wiki](https://github.com/ImNotJahan/Navi/wiki/Command%E2%80%90line-options).

## Examples

Here's a code sample of what a simple number guessing game would look like in Navi:

```common-lisp
(load "navi/random.navi" "random_range")
(load "navi/strings.navi" "ctoi") ~ ctoi -> char to int

(set (guessing_game)
  (set correct_answer to (random_range 0 9))
  (sayline "Guess a number 0 through 9")

  (if (= (ctoi (head (listen))) correct_answer) then
    (sayline "You won!") otherwise
    (do
      (say "Wrong answer! The right number was ")
      (sayline correct_answer))))
	  
(guessing_game)
```

More examples can be found under [navi/examples](https://github.com/ImNotJahan/Navi/tree/master/navi/examples)
