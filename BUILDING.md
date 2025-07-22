`Visual Studio 2022` is required to build. You must have the desktop C++ workload,
the `v143` toolset and `ATL` for the `v143` toolset installed.

3rd party dependencies are installed via [vcpkg](https://github.com/microsoft/vcpkg).

To setup from a `Powershell` prompt:
```
git clone --recurse https://github.com/marc2k3/foo_spider_monkey_panel
git clone https://github.com/microsoft/vcpkg
cd vcpkg
./bootstrap-vcpkg
./vcpkg integrate install
```

Now browse inside the `foo_spider_monkey_panel` folder and open `src/foo_spider_monkey_panel.sln` in
`Visual Studio 2022` and it should build.

When building succeeds, check the `component` folder as the compiled `dll` is copied there.
