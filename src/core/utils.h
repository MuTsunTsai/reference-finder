
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
// IDBFS uses the mount point as the name of the IndexedDB database. IndexedDB is shared by
// all apps on mutsuntsai.github.io, so the mount point must be specific to this app.
// It must match IDB_NAME in app/bridge.ts, and ReferenceFinder::db_path must be under it.
EM_ASYNC_JS(const bool, emscripten_utils_mount_fs, (), {
	const MOUNT = "/reference-finder";

	// Moves the database from the old mount point "/data", so that it doesn't need to be rebuilt.
	// IDBFS stores each file in the object store "FILE_DATA", keyed by its absolute path.
	// TODO: Migration code; can be removed after 2027-10.
	async function migrate() {
		const OLD = "/data", STORE = "FILE_DATA", VERSION = 21; // VERSION is IDBFS.DB_VERSION
		const request = (req) => new Promise((resolve, reject) => {
			req.onsuccess = () => resolve(req.result);
			req.onerror = () => reject(req.error);
		});
		if(indexedDB.databases && !(await indexedDB.databases()).some(db => db.name == OLD)) return;
		const oldDB = await request(indexedDB.open(OLD));
		try {
			if(!oldDB.objectStoreNames.contains(STORE)) return;
			const oldStore = oldDB.transaction(STORE).objectStore(STORE);
			const [keys, values] = await Promise.all([request(oldStore.getAllKeys()), request(oldStore.getAll())]);
			const openNew = indexedDB.open(MOUNT, VERSION);
			openNew.onupgradeneeded = () => {
				// Same schema as IDBFS creates
				const store = openNew.result.createObjectStore(STORE);
				store.createIndex("timestamp", "timestamp", { unique: false });
			};
			const newDB = await request(openNew);
			try {
				const tx = newDB.transaction(STORE, "readwrite");
				const newStore = tx.objectStore(STORE);
				if(await request(newStore.count()) == 0) {
					keys.forEach((key, i) => newStore.put(values[i], MOUNT + key.slice(OLD.length)));
				}
				await new Promise((resolve, reject) => {
					tx.oncomplete = resolve;
					tx.onerror = () => reject(tx.error);
				});
			} finally {
				newDB.close();
			}
		} finally {
			oldDB.close();
		}
		await request(indexedDB.deleteDatabase(OLD));
	}

	try {
		try {
			await migrate();
		} catch(e) {
			// The old database is just a cache; if migration fails, it will simply be rebuilt
			console.warn("IDBFS migration failed", e);
		}
		FS.mkdir(MOUNT);
		FS.mount(IDBFS, {}, MOUNT);
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
