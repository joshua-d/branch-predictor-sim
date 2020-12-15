# branch-predictor-sim
Simulators for different types of branch instruction outcome predictors in C++, a project for CS 320 Advanced Computer Architecture at Binghamton University.

<br>

### Predictors

- Always Taken Predictor
- Always Non-Taken Predictor
- Bimodal Predictor with a single bit of history
- Bimodal Predictor with 2-bit saturating counters
- GShare Predictor
- Tournament Predictor (selects between 2-bit Bimodal and GShare)
- 1-bit Bimodal Predictor with Branch Target Buffer

<br>

### Usage

`predictors <predictor> <input_trace_file> [bits]`

The program will output the number of correct predictions, the total number of predictions, and the prediction accuracy.

`predictor`:

- `always_taken`
- `always_non_taken`
- `bimodal_one_bit`
- `bimodal_saturating`
- `gshare`
- `tournament`
- `btb`

`bits` required for:

- `bimodal_one_bit` `(bits for table size)`
- `bimodal_saturating` `(bits for table size)`
- `gshare` `(bits of history)`

<br>

Fixed quantities (by project specifications):

- GShare Predictor table size 2048 bytes (11 bit)
- Tournament Predictor table size 2048 bytes, 11 bits of history for GShare
- BTB Predictor table size 512 bytes (9 bit)
