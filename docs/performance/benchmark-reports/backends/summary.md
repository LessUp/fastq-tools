# Execution backend comparison

| Benchmark | Samples | p50 (ms) | p95 (ms) | Median MiB/s | Peak RSS (MiB) |
|---|---:|---:|---:|---:|---:|
| BM_Backend_OneTbbCpu/100000/2/real_time | 7 | 29.76 | 40.30 | 1012.41 | 54.7 |
| BM_Backend_OneTbbCpu/100000/4/real_time | 7 | 21.68 | 30.48 | 1389.51 | 53.2 |
| BM_Backend_OneTbbCpu/100000/8/real_time | 7 | 25.39 | 25.89 | 1186.69 | 76.3 |
| BM_Backend_OneTbbReadWrite/100000/2/real_time | 7 | 71.59 | 138.14 | 420.78 | 60.7 |
| BM_Backend_OneTbbReadWrite/100000/4/real_time | 7 | 57.43 | 147.71 | 524.51 | 76.7 |
| BM_Backend_OneTbbReadWrite/100000/8/real_time | 7 | 76.59 | 163.39 | 393.34 | 106.7 |
| BM_Backend_SequentialCpu/100000/1/real_time | 7 | 55.94 | 65.97 | 538.50 | 34.7 |
| BM_Backend_SequentialReadWrite/100000/1/real_time | 7 | 140.05 | 153.66 | 215.10 | 34.6 |
| BM_Backend_TaskflowCpu/100000/2/real_time | 7 | 36.08 | 43.02 | 835.00 | 44.6 |
| BM_Backend_TaskflowCpu/100000/4/real_time | 7 | 22.01 | 24.04 | 1368.42 | 53.9 |
| BM_Backend_TaskflowCpu/100000/8/real_time | 7 | 23.77 | 25.69 | 1267.43 | 63.0 |
| BM_Backend_TaskflowReadWrite/100000/2/real_time | 7 | 78.18 | 139.21 | 385.32 | 49.6 |
| BM_Backend_TaskflowReadWrite/100000/4/real_time | 7 | 38.43 | 53.58 | 783.84 | 69.0 |
| BM_Backend_TaskflowReadWrite/100000/8/real_time | 7 | 78.58 | 173.09 | 383.36 | 104.0 |
