#include "lite_engine.h"
#include "lite_engine_gl.h"

int main() {
	//lite_engine_use_render_api(LITE_ENGINE_RENDERER_GL);
	lite_engine_gl_set_prefer_window_size_x(1600);
	lite_engine_gl_set_prefer_window_size_y(900);
	lite_engine_start();

	while (lite_engine_is_running()) {
		lite_engine_update();
		//lite_engine_stop();
	}

	return 0;
}
