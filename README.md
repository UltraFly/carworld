# CarWorld

CarWorld is a small open-source 3D driving simulator and vehicle-mechanics
demonstration. It was originally developed as a student project and later used
to experiment with rendering, input, networking, and vehicle simulation.

![CarWorld driving simulator](docs/cwscreen3.gif)

## Project status

CarWorld is a historical project that is being restored and modernized. The
original SourceForge repository, website, release history, and available
packages have been preserved on GitHub.

The project now uses CMake and a vcpkg dependency manifest. A 64-bit Windows
Debug build has been verified with the MSVC 19.51 compiler supplied by Visual
Studio 2026.

## Features

- Classical-mechanics vehicle simulation using metric units
- Configurable mass, inertia, suspension, damping, torque, and friction
- OpenGL rendering with textured models and projected shadows
- SDL-based windowing, input, image loading, joystick, and force-feedback support
- Interactive in-game console
- Experimental networked client/server mode using SDL3_net
- Linux and Windows project files

## Requirements

The source currently references:

- A C++ compiler
- SDL3
- SDL3_image
- SDL3_net
- OpenGL and GLU

The dependencies are acquired automatically through the repository's
[`vcpkg.json`](vcpkg.json) manifest. OpenGL and GLU are provided by the target
platform and located by CMake.

## Building

### Windows with Visual Studio 2026

Install Visual Studio 2026 Community with the **Desktop development with C++**
workload. Ensure its CMake tools, Windows SDK, and Git components are selected.

Install vcpkg in a stable location and bootstrap it:

```powershell
git clone https://github.com/microsoft/vcpkg.git C:\Development\vcpkg
C:\Development\vcpkg\bootstrap-vcpkg.bat
$env:VCPKG_ROOT = "C:\Development\vcpkg"
```

To make `VCPKG_ROOT` available in future terminals, add it as a Windows user
environment variable. Do not commit a machine-specific vcpkg path to the
repository.

Open the repository folder in Visual Studio. Visual Studio will detect
[`CMakePresets.json`](CMakePresets.json); select **Windows x64 — Visual Studio
2026**, then choose either the Debug or Release build preset. During the first
configuration, vcpkg downloads and builds SDL3, SDL3_image, SDL3_net, and their
transitive dependencies.

The equivalent command-line workflow from a Visual Studio developer PowerShell
is:

```powershell
cmake --preset windows-vs2026
cmake --build --preset windows-debug
```

The build copies the `data/` directory next to `carworld.exe`, allowing the
program to find its runtime assets when launched from the build output.

The Windows Debug configuration has been compiled successfully with Visual
Studio 2026. Runtime behavior still needs broader testing; the original
packages remain available from
[GitHub Releases](https://github.com/UltraFly/carworld/releases).

## Running

CarWorld expects to be started from the root of its distribution so it can find
the files under `data/`.

Historical controls:

| Control | Action |
| --- | --- |
| Arrow keys | Control the vehicle |
| Space | Handbrake |
| Tab | Open the in-game console |
| F2 | Change camera |
| F3 | Reset the vehicle |
| F4 | Use the physical joystick or steering wheel |
| F5 | Use keyboard controls |

The application uses `carworld.cfg` as its historical configuration filename.
When a physical input device is selected, CarWorld enables its continuous
constant-force effect when SDL reports compatible haptic support. Detection or
runtime errors are written to the application log.

## Networking

Network play uses SDL3_net datagram sockets and is enabled by default. It can
be omitted by configuring CMake with
`-DCARWORLD_ENABLE_NETWORKING=OFF`.

Start a server on the default UDP port (12000) with:

```text
carworld -server
```

An alternate port can be supplied after `-server`. A running client can connect
from the in-game console:

```text
join <server-name> [port]
```

Networking retains the historical native-structure protocol, so peers should
use the same CarWorld build and CPU architecture.

## Documentation

- [Project website](https://ultrafly.github.io/carworld/)
- [Historical changelog](CHANGELOG.md)
- [Releases](https://github.com/UltraFly/carworld/releases)
- [Issue tracker](https://github.com/UltraFly/carworld/issues)

The original SourceForge project remains available as a read-only historical
archive.

## Contributing

Bug reports, build fixes, portability work, documentation improvements, and
tested platform instructions are welcome. See [CONTRIBUTING.md](CONTRIBUTING.md)
before submitting a change.

## License

CarWorld is licensed under the GNU General Public License version 3. See
[LICENSE](LICENSE).
