# Contributing to CarWorld

CarWorld is being restored from a historical codebase. Contributions that make
the project easier to build, run, understand, and maintain are welcome.

## Before starting

- Search the [issue tracker](https://github.com/UltraFly/carworld/issues) for
  related work.
- Open an issue before making a large architectural or format change.
- Keep historical behavior intact unless the change deliberately replaces it.

## Useful contributions

- Reproducible Linux and Windows build instructions
- Build-system modernization
- SDL2, SDL2_image, and SDL2_net compatibility fixes
- Compiler-warning and portability fixes
- Runtime bug reports with reproduction steps
- Documentation and accessibility improvements
- Tests for vehicle mechanics and file parsing

## Submitting changes

1. Fork the repository and create a focused branch.
2. Keep each commit limited to one logical change.
3. Explain how the change was tested.
4. Update documentation when behavior or dependencies change.
5. Open a pull request and link any relevant issue.

Until automated checks are available, include the compiler, operating system,
dependency versions, commands, and results used to test a change.

## Reporting bugs

Include:

- Operating system and version
- Compiler or downloaded release used
- Graphics hardware and driver when relevant
- Exact steps to reproduce
- Expected and actual behavior
- Console output or crash details

## License

By contributing, you agree that your contribution will be licensed under the
GNU General Public License version 3, as described in [LICENSE](LICENSE).
