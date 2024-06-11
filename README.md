# Tizen WebGL issue

## How to install emscripten

```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

## How to compile project

```bash
make
```

## How to run project

Spin up a web server (ex: serve) with CORS enabled.
