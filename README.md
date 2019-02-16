This is an attempt to port Jason Rohrer's [Passage][] (2007) to the Web via
[Emscripten][].

## Very Quick Start

If you just want to play the game, you can either visit [passage.toolness.org][] or
open `dist/index.html` in your browser.

## Quick Start

First, you will need to [download and install Emscripten][download]. You can
verify everything is working by running `emcc`.

Then do:

```
cd Passage_v4_UnixSource
./runToBuild
```

To build with optimizations, run:

```
EMCC_FLAGS='-O2' ./runToBuild
```

### Quick Start (Docker)

If you don't want to deal with installing the Emscripten toolchain on your
system, you can install it via [Docker][] and [Docker Compose][], though
the container takes a very long time to build. Once Docker and Docker Compose
are installed, run:

```
docker-compose build
docker-compose run em bash
```

At this point you will be in a bash shell in a container that has the
Emscripten toolchain pre-installed. You'll be in the `/usr/src/passage`
directory, which maps directly to the `Passage_v4_UnixSource` of your
repository. So to build everything, you can just run `./runToBuild`.

## Notes

* [Emterpreter][] (specifically Emterpreter-Async) is used to manually
  control the call stack so the original code can work without too much
  refactoring.

* See the [history for `Passage_v4_UnixSource/gamma256`][history] for
  details on what was changed to support Emscripten; I tried to make
  sure that the commits are fairly clean and easy to understand.

* Running `emcc -v` on my development environment yielded:

  ```
  emcc (Emscripten gcc/clang-like replacement + linker emulating GNU ld) 1.35.0
  clang version 3.7.0 (https://github.com/kripken/emscripten-fastcomp-clang/ dbe68fecd03d6f646bd075963c3cc0e7130e5767) (https://github.com/kripken/emscripten-fastcomp/ 4e83be90903250ec5142edc57971ed4c633c5e25)
  Target: x86_64-unknown-linux-gnu
  Thread model: posix
  Found candidate GCC installation: /usr/lib/gcc/x86_64-linux-gnu/4.8
  Found candidate GCC installation: /usr/lib/gcc/x86_64-linux-gnu/4.8.4
  Found candidate GCC installation: /usr/lib/gcc/x86_64-linux-gnu/4.9
  Found candidate GCC installation: /usr/lib/gcc/x86_64-linux-gnu/4.9.2
  Selected GCC installation: /usr/lib/gcc/x86_64-linux-gnu/4.9
  Candidate multilib: .;@m64
  Selected multilib: .;@m64
  ```

  It is entirely possible that different versions may cause compilation
  to fail.

## License

The original Passage has been placed in the public domain.

This port is in the public domain as well.

[Passage]: http://hcsoftware.sourceforge.net/passage/
[Emscripten]: http://emscripten.org/
[passage.toolness.org]: http://passage.toolness.org
[Emterpreter]: https://github.com/kripken/emscripten/wiki/Emterpreter
[download]: http://kripken.github.io/emscripten-site/docs/getting_started/downloads.html
[Docker]: http://docker.com/
[Docker Compose]: https://docs.docker.com/compose/
[history]: https://github.com/toolness/passage-emscripten/commits/master/Passage_v4_UnixSource/gamma256
