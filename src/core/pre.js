Module.print = text => {
	if(text == "Ready") readyResolve();
	postMessage({ text });
}
Module.printErr = err => postMessage({ err });

let initialized = false;
let cancelResolve = () => { };
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
Module.checkCancel = () => new Promise(function(resolve) {
	cancelResolve = resolve;

	// if any message comes in when the tread is blocked,
	// it will be executed before setTimeout
	setTimeout(() => resolve(false), 0);
});

addEventListener('message', async e => {
	if(!e.data) return;
	if(initialized) await ready;
	else initialized = true;
	if(e.data == "cancel") {
		cancelResolve(true);
	} else {
		for(const item of e.data) Module.queue.put(item);
	}
});
