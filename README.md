<h1 align="center">OpenGeode<sup><i>by Geode-solutions</i></sup></h1>
<h3 align="center">Open source framework for representing and manipulating geometric models</h3>

<p align="center">
  <img src="https://github.com/Geode-solutions/OpenGeode/workflows/CI/badge.svg" alt="Build Status">
  <img src="https://github.com/Geode-solutions/OpenGeode/workflows/CD/badge.svg" alt="Deploy Status">
  <img src="https://codecov.io/gh/Geode-solutions/OpenGeode/branch/master/graph/badge.svg" alt="Coverage Status">
  <img src="https://img.shields.io/github/release/Geode-solutions/OpenGeode.svg" alt="Version">
  <img src="https://img.shields.io/pypi/v/opengeode-core" alt="PyPI" >
</p>

<p align="center">
  <img src="https://img.shields.io/static/v1?label=Windows&logo=windows&logoColor=white&message=support&color=success" alt="Windows support">
  <img src="https://img.shields.io/static/v1?label=Ubuntu&logo=Ubuntu&logoColor=white&message=support&color=success" alt="Ubuntu support">
  <img src="https://img.shields.io/static/v1?label=Red%20Hat&logo=Red-Hat&logoColor=white&message=support&color=success" alt="Red Hat support">
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-blue.svg" alt="Language">
  <img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="License">
  <img src="https://img.shields.io/badge/%20%20%F0%9F%93%A6%F0%9F%9A%80-semantic--release-e10079.svg" alt="Semantic-release">
  <a href="https://geode-solutions.com/#slack">
    <img src="https://opengeode-slack-invite.herokuapp.com/badge.svg" alt="Slack invite">
  </a>
  <a href="https://doi.org/10.5281/zenodo.3610370">
    <img src="https://zenodo.org/badge/DOI/10.5281/zenodo.3610370.svg" alt="DOI">
  </a>
</p>

---

## Introduction

OpenGeode is an open source framework for representing and manipulating geometric models, from simple meshes to complex boundary representations. The framework is easy to use thanks to its readable API and its Python bindings for quick prototyping. It is the industrialization of a 5-year research prototype platform [RINGMesh](http://ringmesh.org).

## Ecosystem

| Project                    | Version                             | Description                                                    |
| -------------------------- | ----------------------------------- | -------------------------------------------------------------- |
| [OpenGeode-IO]             | ![OpenGeode-IO-version]             | Implementation of input and output formats                     |
| [OpenGeode-Geosciences]    | ![OpenGeode-Geosciences-version]    | Custom objects and algorithms tailored for Geosciences         |
| [OpenGeode-GeosciencesIO]  | ![OpenGeode-GeosciencesIO-version]  | Implementation of input and output formats for Geosciences     |
| [OpenGeode-ModuleTemplate] | ![OpenGeode-ModuleTemplate-version] | Template for creating your own OpenGeode modules               |
| [OpenGeode-Inspector]      | ![OpenGeode-Inspector-version]      | Inspecting meshes and models                                   |
| [Geode]                    | ![Geode-version]                    | Open-source software for visualization, modeling and much more |

[OpenGeode-IO]: https://github.com/Geode-solutions/OpenGeode-IO
[OpenGeode-IO-version]: https://img.shields.io/github/release/Geode-solutions/OpenGeode-IO.svg
[OpenGeode-Geosciences]: https://github.com/Geode-solutions/OpenGeode-Geosciences
[OpenGeode-Geosciences-version]: https://img.shields.io/github/release/Geode-solutions/OpenGeode-Geosciences.svg
[OpenGeode-GeosciencesIO]: https://github.com/Geode-solutions/OpenGeode-GeosciencesIO
[OpenGeode-GeosciencesIO-version]: https://img.shields.io/github/release/Geode-solutions/OpenGeode-GeosciencesIO.svg
[OpenGeode-Inspector]: https://github.com/Geode-solutions/OpenGeode-Inspector
[OpenGeode-Inspector-version]: https://img.shields.io/github/release/Geode-solutions/OpenGeode-Inspector.svg
[OpenGeode-ModuleTemplate]: https://github.com/Geode-solutions/OpenGeode-ModuleTemplate
[OpenGeode-ModuleTemplate-version]: https://img.shields.io/github/release/Geode-solutions/OpenGeode-ModuleTemplate.svg
[Geode]: https://github.com/Geode-solutions/GeodePackage
[Geode-version]: https://img.shields.io/github/release/Geode-solutions/GeodePackage.svg

## Documentation

Code and API are fully documented inside the code itself.
To check out our live documentation, visit [docs.geode-solutions.com](https://docs.geode-solutions.com).

Installing OpenGeode is done:

- either, by compiling the C++ source. Check [this documentation page](https://docs.geode-solutions.com/guides/compile-opengeode.html) for more details.
- or, by using pip command `pip install OpenGeode-core` and add `import opengeode` in your Python script. Check [this documentation page](https://docs.geode-solutions.com/guides/use-opengeode-binding.html) for more details.

## Questions

For questions and support please use the official [slack](https://geode-solutions.com/#slack). The issue list of this repo is exclusively for bug reports and feature requests.

## Changelog

Detailed changes for each release are documented in the [release notes](https://github.com/Geode-solutions/OpenGeode/releases).

## License

[MIT](https://opensource.org/licenses/MIT)

Copyright (c) 2019 - 2024, Geode-solutions
