# Contributing to FastQTools

Thank you for considering contributing to FastQTools! To keep things simple (KISS) and efficient, please follow the guidelines below.

## Spec-Driven Development (SDD)

This project follows **Spec-Driven Development** paradigm. All code implementation must be based on specifications in the `/specs` directory as the Single Source of Truth.

### Before Writing Code

1. **Review relevant specs**: Check `/specs` directory for product requirements, RFCs, and API specifications
2. **Spec-first**: If your contribution changes behavior or interfaces, update or create specs first
3. **Get alignment**: Ensure spec changes are reviewed before implementation

### Spec Categories

- `/specs/product/`: Product requirements and feature definitions
- `/specs/rfc/`: Technical design documents (numbered, e.g., `0001-core-architecture.md`)
- `/specs/api/`: API interface specifications
- `/specs/db/`: Data models and configuration schemas
- `/specs/testing/`: Testing strategies and conventions

## Getting Started

- Use recent CMake (>= 3.28) and a C++23 compiler (GCC 11+ / Clang 12+).
- Preferred build flow uses Conan toolchains.
- Run a full build and tests locally before opening a PR.
- **Full spec documentation**: See [specs/README.md](specs/README.md)

## Development Workflow

1. **Check specs**: Review `/specs` for relevant requirements before starting
2. Fork the repo and create a feature branch.
3. Keep PRs small and focused. One change per PR.
4. **Update specs first**: If changing behavior, update `/specs` documents
5. Ensure formatting/lint pass:
   - `./scripts/core/lint format`
   - `./scripts/core/lint check`
6. Run tests: `./scripts/core/test` (or `ctest`).
7. Add/update documentation when behavior changes.

## Commit Messages

- Use conventional commits where possible, e.g.:
  - `feat: ...`, `fix: ...`, `chore: ...`, `docs: ...`, `test: ...`.
  - For spec changes: `docs(specs): ...`
- Keep subject concise; add details in body if necessary.

## Code Style

- Follow `.clang-format` and `.clang-tidy`.
- Prefer modern C++ (RAII, smart pointers, const-correctness).
- Keep public headers minimal and stable.
- **100% spec compliance**: Code must implement behavior defined in `/specs`.

## Tests

- Add unit tests for new features and bug fixes.
- Keep tests fast and deterministic.
- **Cover spec acceptance criteria**: Ensure tests validate all acceptance criteria in relevant specs.

## Security

- See `SECURITY.md` for reporting vulnerabilities.

## License

- By contributing, you agree that your contributions will be licensed under the project license (MIT unless otherwise specified).
