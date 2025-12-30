#!/bin/bash
set -e

# Compile benchmark tool
echo "Building FastQTools..."
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# Create dummy input data if not exists (needs some size to be useful)
TEST_FILE="test_input.fq"
if [ ! -f "$TEST_FILE" ]; then
    echo "Generating test data..."
    # Generate ~50MB of FASTQ-like data
    for i in {1..200000}; do
        echo "@READ_$i" >> $TEST_FILE
        echo "AGCTAGCTAGCTAGCTAGCTAGCTAGCTAGCTAGCTAGCTAGCTAGCTAGCTAGCTAGCTAGCT" >> $TEST_FILE
        echo "+" >> $TEST_FILE
        echo "IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII" >> $TEST_FILE
    done
fi
COMPRESSED_INPUT="test_input.fq.gz"
if [ ! -f "$COMPRESSED_INPUT" ]; then
    gzip -c $TEST_FILE > $COMPRESSED_INPUT
fi

echo "Running IO Benchmark..."

# Test Decompression (Reader) speed - currently zlib/gzip
echo "--- Reader Benchmark (Zlib) ---"
time ./build/FastQTools stat $COMPRESSED_INPUT > /dev/null

# Test Compression (Writer) speed - should be libdeflate
echo "--- Writer Benchmark (Libdeflate) ---"
# Use Filter command to read and then write back
time ./build/FastQTools filter --output test_output.fq.gz --min-len 0 $COMPRESSED_INPUT

echo "Cleaning up..."
rm -f test_output.fq.gz
