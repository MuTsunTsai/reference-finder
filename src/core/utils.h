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
#if CALCINPUT
EM_JS(const char *, emscripten_utils_getline_impl, (), {
	const toHeap = (js_str) =>
	{
		const num_bytes = lengthBytesUTF8(js_str) + 1;
		const ptr = _malloc(num_bytes);
		stringToUTF8(js_str, ptr, num_bytes);
		return ptr;
	};

	return Asyncify.handleAsync(async() => {
		const command = await Module["queue"].get();
		return toHeap(command);
	});
});
#else
EM_JS(const double *, emscripten_utils_get_double_impl, (), {
	const toHeap = (value) =>
	{
		const ptr = _malloc(8);
		setValue(ptr, value, "double");
		return ptr;
	};

	return Asyncify.handleAsync(async() => {
		const command = await Module["queue"].get();
		return toHeap(command);
	});
});
#endif

EM_JS(void, emscripten_utils_clear_impl, (), {
	Module["queue"].clear();
});
// clang-format on

void emscripten_utils_clear() {
	emscripten_utils_clear_impl();
}

#if CALCINPUT
void emscripten_utils_getline(std::string &result) {
	const char *ptr = emscripten_utils_getline_impl();
	result = ptr;
	std::free((void *)ptr);
}
#else
void emscripten_utils_get_double(double &n) {
	const double *ptr = emscripten_utils_get_double_impl();
	n = *ptr;
	std::free((void *)ptr);
}
#endif
