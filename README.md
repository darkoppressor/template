# template

[![Build Status](https://wells-family.xyz/jenkins/buildStatus/icon?job=template)](https://wells-family.xyz/jenkins/job/template/)

Template project for use with the Cheese Engine
https://github.com/darkoppressor/cheese-engine

This template is a complete project for use with the Cheese Engine. Before being used, it should be properly initialized
using the rename-template tool.

## Updating the version

    ~/build-server/cheese-engine/tools/version <PROJECT-DIRECTORY> <VERSION-COMPONENT> [STATUS]
    ~/build-server/cheese-engine/tools/build-date <PROJECT-DIRECTORY>

In version.cpp:

        Update the version series in Engine_Version::populate_version_series to include the new version

When an updated version has been pushed to main, tag the commit locally and remotely:

    git tag <VERSION>
    git push origin tag <VERSION>

## License

This project's source code is licensed under the MIT License. See [LICENSE.md](docs/LICENSE.md) for the full license
text.

Please note that the file docs/contentLicense.md is included for convenience when starting a new project, and does not
pertain to this template project itself.
