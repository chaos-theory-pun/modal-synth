# Modal Synthesiser

This is a software synthesiser that uses modal synthesis, a
physical modelling technique using banks of resonators to filter an exciter signal, to create
sounds with a parametrically controlled frequency spectrum. This synthesiser is designed to
allow for the composition and performance of spectral music, and it produces sounds with timbres
similar to those of natural materials such as wood, metal, and glass.

The synthesiser is available as VST and AU plugins and as a standalone application. It is written in C++ using the JUCE audio framework. The repo contains the code for two plugins: `ModalSynth`, which exposes the full functionality of the synth, and `MiniModal`, which is a redesign of the interface to promote more 'expressive' playing. `MiniModal` is in a very early stage of development, so I am currently only providing builds of `ModalSynth`.

At the moment the best place for discussions etc. of this is the Discussions and Issues tabs of this repo.

## Repo Layout

The code in this repo is in the `include/` and `src/` directories, split into subfolders: `dsp` for synthesis and signal processing, `ui` for classes that integrate with JUCE's UI functionality, and `ModalSynth` and `MiniModal`, which have the code for the respective VST plugins.

The code is documented using Doxygen-style inline documentation, with higher-level docs in the `docs/` folder.  External libraries are mostly managed with Git submodules and are in the `libs/` folder. The Catch2 test runner is set up with tests in the `test/` folder, though at present only basic tests are present.

## Cloning

This repo uses Git submodules to manage dependencies, clone using `git clone --recurse-submodules` to initialise them.

## Building

This uses CMake to build, which can be run from the terminal or through an IDE's CMake integration (eg. CLion). 
There are several build options, which can be set by adding the flag `-D<option>` to the first `cmake` invocation.
- `MODAL_NUM_TYPE=<type>` to set the float type used for DSP calculations, defaults to `float`
- `MODAL_INSTALL_PLUGIN=<on|off>` to install the plugins to the default user plugin directories after every build, defaults to `off`
- `MODAL_DEBUG_UI=<on|off>` to build and enable a JUCE UI inspector, defaults to `off`
- `MODAL_BUILD_DOCS=<on|off>` to build docs using Doxygen, adds target ModalSynthDocs, defaults to `on` (will be skipped if Doxygen is not installed)
- `MODAL_BUILD_TESTS=<on|off>` to build tests using Catch2, adds target ModalSynthTests, defaults to `on`

To build `ModalSynth` using the CMake CLI:
```shell
$ cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
$ cmake --build build --target ModalSynthPlug_All -j8
```
This will put the built plugins in subdirectories of `<repo-path>/build/ModalSynthPlug_artefacts`.

## License

The code in this repository is released under the General Public License version 3 or later, see `LICENSE.md` for the full license text and the following paragraphs for caveats.

This program uses the following libraries:
- [JUCE](https://juce.com/), released under the [AGPLv3](https://github.com/juce-framework/JUCE/blob/master/LICENSE.md)
- [melatonin_inspector](https://github.com/sudara/melatonin_inspector/), released under the [MIT license](https://github.com/sudara/melatonin_inspector/blob/main/LICENSE)
- [randutils](https://gist.github.com/imneme/540829265469e673d045), released under the MIT license
- [doxygen-awesome-css](https://github.com/jothepro/doxygen-awesome-css), released under the MIT license
- [Catch2](https://github.com/catchorg/Catch2), released under the [Boost Software License](https://github.com/catchorg/Catch2/blob/v3.8.1/LICENSE.txt)

In addition, the modal resonator is derived from the implementation in [modal-waterbottles by Jatin Chowdhury et al](https://github.com/jatinchowdhury18/modal-waterbottles), which is released under the GPLv3.

Note that while the code in this repository is provided under the GPL it depends on JUCE, an AGPL-licensed work, and the "special requirements" of the AGPL apply to any combination of this work with JUCE, most pertinently audio plugins using the JUCE framework. See section 13 of the GPL and the AGPL licenses for full details.