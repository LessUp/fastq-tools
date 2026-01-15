# Fuzzing Corpus

This directory contains seed files for fuzzing the FASTQ parser.

## Seed Files

| File | Description |
|------|-------------|
| `seed_valid_single.fq` | Single valid FASTQ record |
| `seed_valid_multi.fq` | Multiple valid FASTQ records |
| `seed_with_n.fq` | Record with ambiguous N bases |
| `seed_low_quality.fq` | Record with low quality scores |

## Running Fuzzers

```bash
# Build with fuzzing enabled
cmake -DENABLE_FUZZING=ON -DCMAKE_CXX_COMPILER=clang++ -B build-fuzz
cmake --build build-fuzz

# Run parser fuzzer
./build-fuzz/fuzzers/fastq_parser_fuzzer tools/fuzz/corpus/ -max_len=4096

# Run record fuzzer
./build-fuzz/fuzzers/fastq_record_fuzzer tools/fuzz/corpus/ -max_len=1024
```

## Adding New Seeds

Add new seed files that represent:
- Edge cases in FASTQ format
- Different quality encodings
- Various header formats
- Unusual but valid sequences

## Crash Artifacts

When a fuzzer finds a crash, it saves the input to a file like `crash-<hash>`.
These files should be:
1. Analyzed to understand the bug
2. Used to create regression tests
3. Kept as seeds after the bug is fixed
