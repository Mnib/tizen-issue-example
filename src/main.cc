#include "scene.hh"
#include <cassert>
#include <emscripten/html5.h>
#include <emscripten/html5_webgl.h>

EM_BOOL one_iter(double, void *) {
    return EM_TRUE;
}

extern "C" {
int run() {
    EmscriptenWebGLContextAttributes attributes;
    emscripten_webgl_init_context_attributes(&attributes);
    attributes.alpha = false;
    attributes.antialias = true;
    auto handle = emscripten_webgl_create_context("#canvas", &attributes);
    assert(emscripten_webgl_make_context_current(handle) == EMSCRIPTEN_RESULT_SUCCESS);

    // Setup scene
    SetupScene();

    // Render
    RenderScene();
    emscripten_request_animation_frame(one_iter, 0);

    Cleanup();

    return 0;
}
}
