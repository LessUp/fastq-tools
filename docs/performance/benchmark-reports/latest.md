# FastQTools Performance Report

**Generated:** 2026-01-12T08:23:37+00:00
**Git Commit:** unknown (unknown)

## System Information

| Property | Value |
|----------|-------|
| CPU | unknown |
| Cores | 16 |
| Memory | Unknown |
| OS | release |
| Compiler | unknown |

## Benchmark Results

### FILTER Benchmarks

| Benchmark | Time (ms) | Throughput | Iterations |
|-----------|-----------|------------|------------|
| BM_Filter_NoFilter/10000 | 8.69 | 1.52 M reads/s | 116 |
| BM_Filter_NoFilter/50000 | 42.33 | 1.39 M reads/s | 23 |
| BM_Filter_NoFilter/100000 | 100.99 | 1.05 M reads/s | 9 |
| BM_Filter_MinLength/10000 | 11.68 | 1.07 M reads/s | 62 |
| BM_Filter_MinLength/50000 | 53.79 | 1.09 M reads/s | 28 |
| BM_Filter_MinLength/100000 | 81.11 | 1.42 M reads/s | 10 |
| BM_Filter_MinQuality/10000 | 19.12 | 712.56 K reads/s | 57 |
| BM_Filter_MinQuality/50000 | 59.01 | 988.82 K reads/s | 13 |
| BM_Filter_MinQuality/100000 | 107.61 | 1.02 M reads/s | 10 |
| BM_Filter_MaxNRatio/10000 | 6.55 | 1.72 M reads/s | 115 |
| BM_Filter_MaxNRatio/50000 | 10.28 | 5.05 M reads/s | 52 |
| BM_Filter_MaxNRatio/100000 | 35.13 | 2.93 M reads/s | 17 |
| BM_Filter_Combined/10000 | 5.60 | 2.03 M reads/s | 134 |
| BM_Filter_Combined/50000 | 31.65 | 1.62 M reads/s | 23 |
| BM_Filter_Combined/100000 | 60.53 | 1.67 M reads/s | 20 |

### IO Benchmarks

| Benchmark | Time (ms) | Throughput | Iterations |
|-----------|-----------|------------|------------|
| BM_FastQReader_Small | 1.30 | 2462.88 MB/s | 436 |
| BM_FastQReader_Medium | 18.79 | 1696.50 MB/s | 67 |
| BM_FastQWriter_Small | 7.77 | 1.39 M reads/s | 101 |
| BM_FastQWriter_Medium | 57.04 | 1.76 M reads/s | 12 |
| BM_FastQReader_Parameterized/10000 | 1.00 | 3178.14 MB/s | 894 |
| BM_FastQReader_Parameterized/50000 | 8.68 | 1835.35 MB/s | 97 |
| BM_FastQReader_Parameterized/100000 | 17.30 | 1865.18 MB/s | 46 |
| BM_FastQWriter_Parameterized/10000 | 7.86 | 1.38 M reads/s | 96 |
| BM_FastQWriter_Parameterized/50000 | 31.50 | 1.62 M reads/s | 21 |
| BM_FastQWriter_Parameterized/100000 | 63.46 | 1.60 M reads/s | 11 |

### STAT Benchmarks

| Benchmark | Time (ms) | Throughput | Iterations |
|-----------|-----------|------------|------------|
| BM_Stat_Basic/10000 | 11.51 | 273.07 MB/s | 64 |
| BM_Stat_Basic/50000 | 51.45 | 306.05 MB/s | 13 |
| BM_Stat_Basic/100000 | 106.89 | 294.41 MB/s | 7 |
| BM_Stat_BaseComposition/10000 | 9.96 | 315.53 MB/s | 68 |
| BM_Stat_BaseComposition/50000 | 50.37 | 311.75 MB/s | 14 |
| BM_Stat_BaseComposition/100000 | 99.90 | 314.34 MB/s | 7 |
| BM_Stat_QualityDistribution/10000 | 1.28 | 2448.47 MB/s | 517 |
| BM_Stat_QualityDistribution/50000 | 8.45 | 1856.92 MB/s | 85 |
| BM_Stat_QualityDistribution/100000 | 16.23 | 1934.09 MB/s | 34 |
| BM_Stat_LengthDistribution/10000 | 0.55 | 5666.79 MB/s | 1032 |
| BM_Stat_LengthDistribution/50000 | 4.72 | 3322.86 MB/s | 158 |
| BM_Stat_LengthDistribution/100000 | 9.60 | 3309.03 MB/s | 81 |
| BM_Stat_Full/10000 | 10.32 | 306.48 MB/s | 69 |
| BM_Stat_Full/50000 | 59.70 | 265.08 MB/s | 13 |
| BM_Stat_Full/100000 | 104.61 | 301.91 MB/s | 6 |
