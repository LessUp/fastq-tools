# Testing

Testing in FastQTools is not a cleanup step. It is part of the maintenance boundary. A change is only truly reviewable when the relevant behavior is expressed at the appropriate verification level.

## Layered verification model

| Layer | Focus | Typical location |
| --- | --- | --- |
| Unit tests | Small logic, boundary cases, and error paths | `tests/unit/` |
| Integration tests | Module collaboration, configuration, and I/O combinations | `tests/integration/` |
| End-to-end tests | CLI behavior, help output, and real workflows | `tests/e2e/` |
| Documentation tests | Site IA, page skeletons, and content contracts | `docs/tests/` |

## Maintainer rhythm

1. Write a failing test first for the new behavior or fix.
2. Make only the smallest implementation change needed to turn it green.
3. Run the smallest relevant test set for the change.
4. Then return to higher-level regression checks such as `./scripts/core/test` or the docs build.

## Common commands

```bash
./scripts/core/test
./scripts/core/test --unit
./scripts/core/test --integration
./scripts/core/test --e2e
```

If you change the docs site, run the relevant Node tests and docs build from `docs/`. If you change C++ code, pair the test run with formatting, clang-tidy, and the relevant CTest or GTest targets.

## A simple standard for enough testing

- Does the test precisely express the behavior this change really promises?
- When it fails, does it clearly say why?
- When it passes, does it prove the target behavior rather than merely touch the code?

Tests are not there to create more files. They are there to let maintainers change the same area again with more confidence later.
