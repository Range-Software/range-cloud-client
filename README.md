# Range Software - Cloud Client
Range Cloud client application

## Prepare build environment
Initialize all submodules
```
git submodule init && git submodule update --remote
```
Following command will attempt to download and install all required packages, therefore it must be executed under priviledged (root) user
```
sudo ./src/range-build-tools/prereqs.sh
```
## Build
```
cmake -S src -B build-Release && \
cmake --build build-Release --parallel
```
## Create an installation packages and installers
```
cmake --build build-Release --target package
```
## Installation using generated installers
### Windows

### MacOS
#### Drag & Drop
```
open './build-Release/range-cloud-client-0.0.1-Darwin.dmg'
```
#### GUI Installer
```
./build-Release/range-cloud-client-0.0.1-Darwin.run
```
#### Self extracting Tar GZip
```
./build-Release/range-cloud-client-0.0.1-Darwin.sh
```
### Linux
#### GUI Installer
```
./build-Release/range-cloud-client-0.0.1-Linux.run
```
#### Self extracting Tar GZip
```
./build-Release/range-cloud-client-0.0.1-Linux.sh
```
#### Debian DEB package
```
sudo apt install ./build-Release/range-cloud-client-0.0.1-Linux.deb
```
#### RedHat RPM package
```
sudo dnf install ./build-Release/range-cloud-client-0.0.1-Linux.rpm
```

## Download
To download already built binaries please visit http://range-software.com

## Powered by

* Qt - https://www.qt.io/
