# Avendish data / message object template

A canonical template for making **data / message** objects with
[Avendish](https://github.com/celtera/avendish): a single C++ object that processes
strings (and other non-audio data) and is compiled, without rewrite, to the host formats
that have a real object model.

It is the message-processing counterpart of the
[audio](https://github.com/celtera/avendish-audio-processor-template),
[video](https://github.com/celtera/avendish-video-processor-template) and
[geometry](https://github.com/celtera/avendish-geometry-processor-template) templates.

## What gets built

The `avnd_addon_object(... CATEGORY object)` call in `CMakeLists.txt` instantiates the
object back-ends — deliberately **not** the audio plug-in formats (VST3 / CLAP / …).
The same `CMakeLists.txt` also builds as an ossia/score add-on (ossia back-end only) when
dropped into score, because it resolves Avendish via `find_package(Avendish)`:

| Back-end | Object kind | SDK required |
|---|---|---|
| **Max/MSP** | a Max external | Max SDK |
| **Pure Data** | a Pd external | PureData headers (`m_pd.h`) |
| **TouchDesigner** | a CHOP (message) | TouchDesigner Custom Operator SDK |
| **Godot** | a `GDExtension` node | none — `godot-cpp` is fetched automatically |
| **Python** | an importable extension module | pybind11 + Python dev headers |
| **ossia score** | a process | libossia |

Back-ends whose SDK is not provided are silently skipped.

## The object

`src/Model.hpp` implements `MyDataProcessor`: it takes a string input, applies a
case/reverse `Operation`, and emits the transformed string on its output. It also exposes
a `set_prefix` message (a `halp_mem_fun`) that prepends a prefix to every output —
showing string ports (`halp::val_port<…, std::string>`), an enum control
(`halp::enum_t`), input-driven recompute (`update()`), and message handlers.

Start from there: change the ports/messages and rewrite `operator()`. See
[`examples/Complete/CompleteMessageExample.hpp`](https://github.com/celtera/avendish/blob/main/examples/Complete/CompleteMessageExample.hpp)
in Avendish for the full set of string / message / callback features.

## Building

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

With no extra options this builds the **Godot** back-end (it fetches `godot-cpp`
automatically). Pass the relevant SDK paths to enable the others — see the
[Github actions workflow](.github/workflows/build_cmake.yml), which builds the project on
clean VMs via the shared [`ossia/actions/avendish-template`](https://github.com/ossia/actions)
action:

- **Max/MSP**: `-DAVND_MAXSDK_PATH=path/to/max-sdk-base`
- **Pure Data**: make `m_pd.h` findable, e.g. `-DCMAKE_INCLUDE_PATH=path/to/pure-data/src`
- **TouchDesigner**: `-DTOUCHDESIGNER_SDK_PATH=path/to/CustomOperatorSamples`
- **Python**: `-Dpybind11_DIR="$(python -m pybind11 --cmakedir)"`
- **ossia score**: install [libossia](https://github.com/ossia/libossia)
