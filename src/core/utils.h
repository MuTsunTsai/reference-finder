#include <emscripten.h>
#include <string>

// For silencing VS Code errors
#ifdef __INTELLISENSE__
#pragma diag_suppress 67
#pragma diag_suppress 79
#pragma diag_suppress 90
#pragma diag_suppress 130
#endif

// clang-format off
EM_JS(const long *, emscripten_utils_get_now_impl, (), {
	const ptr = _malloc(4);
	setValue(ptr, performance.now(), "i32");
	return ptr;
});

EM_JS(const double *, emscripten_utils_get_double_impl, (), {
	return Asyncify.handleAsync(async() => {
		const command = await Module["queue"].get();
		const ptr = _malloc(8);
		setValue(ptr, command, "double");
		return ptr;
	});
});

EM_JS(const bool *, emscripten_utils_check_cancel_impl, (), {
	return Asyncify.handleAsync(async() => {
		const command = await Module.checkCancel();
		const ptr = _malloc(1);
		setValue(ptr, command, "i8");
		return ptr;
	});
});

EM_JS(void, emscripten_utils_clear_impl, (), {
	Module["queue"].clear();
});
// clang-format on

void emscripten_utils_clear() {
	emscripten_utils_clear_impl();
}

long emscripten_utils_get_now() {
	const long *ptr = emscripten_utils_get_now_impl();
	long n = *ptr;
	std::free((void *)ptr);
	return n;
}

double emscripten_utils_get_double() {
	const double *ptr = emscripten_utils_get_double_impl();
	double n = *ptr;
	std::free((void *)ptr);
	return n;
}

bool emscripten_utils_check_cancel() {
	const bool *ptr = emscripten_utils_check_cancel_impl();
	bool n = *ptr;
	std::free((void *)ptr);
	return n;
}
