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

EM_JS(void, emscripten_utils_clear_impl, (), {
	Module["queue"].clear();
});
// clang-format on

void emscripten_utils_clear() {
	emscripten_utils_clear_impl();
}

void emscripten_utils_get_double(double &n) {
	const double *ptr = emscripten_utils_get_double_impl();
	n = *ptr;
	std::free((void *)ptr);
}
