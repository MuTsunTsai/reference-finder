Module.print = text => {
	if(text == "Ready") readyResolve();
	postMessage({ text });
}
Module.printErr = err => postMessage({ err });

let init = false;
let readyResolve;
const ready = new Promise(resolve => readyResolve = resolve);

function createQueue() {
	var list = [];
	var pending;
	return {
		get: async function() {
			if(list.length > 0) {
				return list.shift();
			}
			return await new Promise(function(resolve) {
				return pending = resolve;
			});
		},
		put: function(data) {
			if(pending) {
				pending(data);
				pending = null;
			} else {
				list.push(data);
			}
		},
		clear: () => list.length = 0,
	};
}

Module.queue = createQueue();

addEventListener('message', async e => {
	if(!e.data) return;
	if(init) await ready;
	else init = true;
	for(const item of e.data) Module.queue.put(item);
});