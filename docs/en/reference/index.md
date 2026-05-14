# Reference

This page is not a flat directory of every document. It is an **intent-based routing point**. Once you have decided to keep using FastQTools, the next step is usually not more narrative reading. It is finding the right deep manual as quickly as possible.

## If you need to...

### 1. Run a command right away

Read these first:

- [`Getting Started`](../guide/getting-started) — use the smallest path to install, build, and complete the first run;
- [`CLI Reference`](../guide/cli-reference) — check all commands, options, and examples;
- [`Configuration`](../guide/configuration) — when you need environment variables, config-file behavior, or default-value details.

**This is a fit if:** you are new to the project, or you simply want to confirm that the current task can be solved through the CLI.

### 2. Put commands into production or automation

Read these first:

- [`CLI Reference`](../guide/cli-reference)
- [`Deployment`](../guide/deployment)
- [`Workflows`](../workflows/)
- [`Performance`](../performance/) (if throughput and resource usage are adoption gates)

**This is a fit if:** you already understand what the commands do and are now planning containerization, CI integration, batch execution, or environment consistency.

### 3. Embed FastQTools into a C++ application

Read these first:

- [`API Overview`](../api/overview) — first understand the public entry points and module relationships;
- [`IO Module`](../api/io) — understand record views, batch handling, and the I/O surface;
- [`Processing Module`](../api/processing) — understand predicates, mutators, and the pipeline;
- [`Statistics Module`](../api/statistics) / [`Core Module`](../api/core) — continue based on actual integration needs.

**This is a fit if:** shell invocation is no longer enough and you need to reuse internal capability more directly.

### 4. Understand internals and maintenance constraints

Read these first:

- [`Architecture`](../architecture/)
- [`Developer Architecture`](../dev/architecture)
- [`Core Design`](../dev/design)
- [`Build Guide`](../dev/build)
- [`Testing`](../dev/testing)

**This is a fit if:** you want to submit patches, investigate problems, verify design boundaries, or maintain the project over time.

### 5. Review release state, change history, and collaboration context

Read these first:

- [`Changelog (bilingual; English-first)`](../release-notes/changelog)
- [`Contributing`](../contributing)
- [`Resources`](../resources/)
- [`Archive Overview`](../archive/)

**This is a fit if:** you care about version history, collaboration entry points, or historical migration context.

## Quick routing by role

| Your role | Recommended first stop | Second stop |
| --- | --- | --- |
| New user / evaluator | [`Workflows`](../workflows/) | [`Getting Started`](../guide/getting-started) |
| Bioinformatics analyst or platform engineer | [`CLI Reference`](../guide/cli-reference) | [`Deployment`](../guide/deployment) |
| C++ integrator | [`API Overview`](../api/overview) | [`Architecture`](../architecture/) |
| Contributor / maintainer | [`Developer Guide`](../dev/) | [`Contributing`](../contributing) |

## When you should leave the reference layer and return to the narrative layer

If, while reading implementation details, you suddenly realize you are no longer sure:

- why a module exists;
- what the performance numbers really mean;
- what the project’s real positioning and boundaries are;

then you should temporarily leave the concrete manuals and go back to:

- [`Why FastQTools`](../why-fastqtools/)
- [`Architecture`](../architecture/)
- [`Performance`](../performance/)

Reference docs answer “how do I do it?” Narrative docs answer “why is it done this way?” Reading both layers together is the fastest path.
