# Getting Started

This quick start aims at one goal: the first successful run. It does not try to cover every feature. After completing this page, you should be able to build FastQTools locally, inspect the CLI help, and understand the basic rhythm of `stat` and `filter`.

## Success criteria

When you finish this page, you should have:

1. prepared dependencies from the repository root;
2. built the executable;
3. successfully displayed the CLI help;
4. understood where to go next for commands, configuration, and deployment details.

## Environment setup

Run these commands from the repository root:

```bash
./scripts/core/install-deps
./scripts/core/build --dev
```

If you only need a runtime-oriented validation path, you can use the default Release build instead:

```bash
./scripts/core/build
```

The project uses `scripts/core/*` as the unified entry points so you do not need to hand-maintain Conan, CMake directory, or preset details.

## First confirmation that the CLI works

After the build completes, first confirm the binary exists and then open the help:

```bash
./build/clang-debug/FastQTools --help
```

If you just ran the default Release build, use:

```bash
./build/clang-release/FastQTools --help
```

You should see the global logging options and the two core subcommands, `stat` and `filter`. At that point you have confirmed that the tool starts and the command entry points exist.

## Meet the first workflow pair

### `stat`: build evidence first

`stat` turns a FASTQ input into read-length, base-composition, and quality-distribution evidence. It fits before filtering, tuning, or report writing.

```bash
./build/clang-debug/FastQTools stat -i reads.fastq.gz -o stats.txt
```

### `filter`: then apply cleanup

`filter` applies quality thresholds, length thresholds, trimming, and preprocessing policy to the input data.

```bash
./build/clang-debug/FastQTools filter -i reads.fastq.gz -o filtered.fastq.gz     --min-quality 20     --min-length 50     --trim-quality 20     --trim-mode both
```

The goal is not to memorize every flag now. The goal is to learn the normal usage rhythm: inspect first, then decide on filtering and deployment strategy.

## Where to go next

- Want the complete command and option list? Continue to [`CLI Reference`](./cli-reference)
- Want to understand configuration precedence and recommended patterns? Continue to [`Configuration`](./configuration)
- Want to place the tool into containers, CI, or batch environments? Continue to [`Deployment`](./deployment)
- Want to study internal design? Move to the [`Developer Guide`](../dev/)
