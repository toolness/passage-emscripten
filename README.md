This is an attempt to port Jason Rohrer's [Passage][] (2007) to the Web via
[Emscripten][].

**WARNING:** This port features functional video and input, but does not
currently feature any audio. As such, it's not recommended that you use
this to play the game for the first time!

## Quick Start

First, you will need to [download and install Emscripten][download]. You can
verify everything is working by running `emcc`.

Then do:

```
cd Passage_v4_UnixSource
./runToBuild
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

## License

The original Passage has been placed in the public domain.

This port is in the public domain as well.

[Passage]: http://hcsoftware.sourceforge.net/passage/
[Emscripten]: http://emscripten.org/
[Emterpreter]: https://github.com/kripken/emscripten/wiki/Emterpreter
[download]: http://kripken.github.io/emscripten-site/docs/getting_started/downloads.html
[Docker]: http://docker.com/
[Docker Compose]: https://docs.docker.com/compose/
[history]: https://github.com/toolness/passage-emscripten/commits/master/Passage_v4_UnixSource/gamma256
