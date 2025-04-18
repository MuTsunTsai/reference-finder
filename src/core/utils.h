
#pragma once

#include <emscripten.h>

// For silencing VS Code errors
#ifdef __INTELLISENSE__
#pragma diag_suppress 67
#pragma diag_suppress 79
#pragma diag_suppress 90
#pragma diag_suppress 130
#endif

// clang-format off
EM_ASYNC_JS(const bool, emscripten_utils_mount_fs, (), {
	try {
		FS.mkdir("/data");
		FS.mount(IDBFS, {}, "/data");
		const err = await new Promise(resolve => FS.syncfs(true, resolve));
		return !err;
	} catch(e) {
		return false;
	}
});

EM_ASYNC_JS(void, emscripten_utils_sync_fs, (), {
	await new Promise(resolve => FS.syncfs(false, resolve));
});

EM_ASYNC_JS(const double, emscripten_utils_get_double, (), {
	return await Module.get();
});

EM_ASYNC_JS(const bool, emscripten_utils_check_cancel, (), {
	return await Module.checkCancel();
});

EM_JS(void, emscripten_utils_clear, (), {
	Module.clear();
});
// clang-format on
