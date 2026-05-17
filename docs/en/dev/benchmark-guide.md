# Benchmark Guide

Benchmark documentation should not be a pile of extra numbers. Its job is to help maintainers explain where numbers came from, what boundaries apply, and whether the results are meaningfully comparable. Only then should performance conclusions enter the whitepaper, PR descriptions, or release notes.

## Answer three questions first

1. **What is being compared?** Different commits, different parameters, or different tools?
2. **What is the workload?** Are input size, read length, compression state, and thread count aligned?
3. **Who needs the conclusion?** Maintainers tuning the internals, or readers deciding whether the product positioning is credible?

## Use the repository workflow

Prefer the benchmark workflow and directories already used by the project instead of assembling ad hoc commands. Performance results only become comparable when the inputs, environment, and recording method stay consistent.

## How to read results

- **Results layer**: direct values such as throughput, latency, and memory.
- **Method layer**: thread count, input distribution, machine topology, and build configuration.
- **Specification layer**: whether the wording still matches the project’s public benchmark narrative.

If all you can say is “it got faster,” the benchmark material is not ready to be propagated yet.

## Common maintainer mistakes

- Comparing conclusions from different workloads in the same table.
- Treating one accidental peak as the long-term capability.
- Omitting I/O, compression ratio, or thread settings so nobody else can reproduce the result.
- Showing raw numbers without risk boundaries or non-applicable cases.

## When docs need updates too

Once benchmark conclusions appear in the homepage, whitepaper, release notes, or core PR narrative, they are no longer only internal material. At that point, check whether [`Performance`](../performance/), related changelog items, and the research appendix need to stay aligned.
