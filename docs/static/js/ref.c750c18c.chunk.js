(()=>{var t={815:t=>{var e=(()=>{var t="undefined"!==typeof document&&document.currentScript?document.currentScript.src:void 0;return function(){let e=arguments.length>0&&void 0!==arguments[0]?arguments[0]:{};var r,n,a=e;a.ready=new Promise(((t,e)=>{r=t,n=e})),a.print=t=>{"Ready"==t&&i(),postMessage({text:t})},a.printErr=t=>postMessage({err:t});let i,o=!1;const s=new Promise((t=>i=t));a.queue=function(){var t,e=[];return{get:async function(){return e.length>0?e.shift():await new Promise((function(e){return t=e}))},put:function(r){t?(t(r),t=null):e.push(r)},clear:()=>e.length=0}}(),addEventListener("message",(async t=>{if(t.data){o?await s:o=!0;for(const e of t.data)a.queue.put(e)}}));var u,c=Object.assign({},a),l="./this.program",f=(t,e)=>{throw e},d=!1,m=!0,h="";(d||m)&&(m?h=self.location.href:"undefined"!=typeof document&&document.currentScript&&(h=document.currentScript.src),t&&(h=t),h=h.startsWith("blob:")?"":h.substr(0,h.replace(/[?#].*/,"").lastIndexOf("/")+1),t=>{var e=new XMLHttpRequest;return e.open("GET",t,!1),e.send(null),e.responseText},m&&(u=t=>{var e=new XMLHttpRequest;return e.open("GET",t,!1),e.responseType="arraybuffer",e.send(null),new Uint8Array(e.response)}));var p,y,g=a.print||console.log.bind(console),v=a.printErr||console.error.bind(console);Object.assign(a,c),c=null,a.arguments&&a.arguments,a.thisProgram&&(l=a.thisProgram),a.quit&&(f=a.quit),a.wasmBinary&&(p=a.wasmBinary);var w,_,b,S,D,A,R,k,M=!1;function E(){var t=y.buffer;a.HEAP8=_=new Int8Array(t),a.HEAP16=S=new Int16Array(t),a.HEAPU8=b=new Uint8Array(t),a.HEAPU16=new Uint16Array(t),a.HEAP32=D=new Int32Array(t),a.HEAPU32=A=new Uint32Array(t),a.HEAPF32=R=new Float32Array(t),a.HEAPF64=k=new Float64Array(t)}var F=[],x=[],I=[],P=[];var H=0,C=null,O=null;function T(t){var e;null===(e=a.onAbort)||void 0===e||e.call(a,t),v(t="Aborted("+t+")"),M=!0,w=1,t+=". Build with -sASSERTIONS for more info.";var r=new WebAssembly.RuntimeError(t);throw n(r),r}var W,Y,j=t=>t.startsWith("data:application/octet-stream;base64,");function U(t){if(t==W&&p)return new Uint8Array(p);if(u)return u(t);throw"both async and sync fetching of the wasm failed"}function N(t,e,r){return function(t){return p||!d&&!m||"function"!=typeof fetch?Promise.resolve().then((()=>U(t))):fetch(t,{credentials:"same-origin"}).then((e=>{if(!e.ok)throw"failed to load wasm binary file at '".concat(t,"'");return e.arrayBuffer()})).catch((()=>U(t)))}(t).then((t=>WebAssembly.instantiate(t,e))).then(r,(t=>{v("failed to asynchronously prepare wasm: ".concat(t)),T(t)}))}function B(t){this.name="ExitStatus",this.message="Program terminated with exit(".concat(t,")"),this.status=t}j(W="ref.wasm")||(Y=W,W=a.locateFile?a.locateFile(Y,h):h+Y);var L=t=>{for(;t.length>0;)t.shift()(a)},V=a.noExitRuntime||!0;function q(t,e){let r=arguments.length>2&&void 0!==arguments[2]?arguments[2]:"i8";switch(r.endsWith("*")&&(r="*"),r){case"i1":case"i8":_[t]=e;break;case"i16":S[t>>1]=e;break;case"i32":D[t>>2]=e;break;case"i64":T("to do setValue(i64) use WASM_BIGINT");case"float":R[t>>2]=e;break;case"double":k[t>>3]=e;break;case"*":A[t>>2]=e;break;default:T("invalid type for setValue: ".concat(r))}}class z{constructor(t){this.excPtr=t,this.ptr=t-24}set_type(t){A[this.ptr+4>>2]=t}get_type(){return A[this.ptr+4>>2]}set_destructor(t){A[this.ptr+8>>2]=t}get_destructor(){return A[this.ptr+8>>2]}set_caught(t){t=t?1:0,_[this.ptr+12]=t}get_caught(){return 0!=_[this.ptr+12]}set_rethrown(t){t=t?1:0,_[this.ptr+13]=t}get_rethrown(){return 0!=_[this.ptr+13]}init(t,e){this.set_adjusted_ptr(0),this.set_type(t),this.set_destructor(e)}set_adjusted_ptr(t){A[this.ptr+16>>2]=t}get_adjusted_ptr(){return A[this.ptr+16>>2]}get_exception_ptr(){if(vt(this.get_type()))return A[this.excPtr>>2];var t=this.get_adjusted_ptr();return 0!==t?t:this.excPtr}}var G=t=>{var e=(t-y.buffer.byteLength+65535)/65536;try{return y.grow(e),E(),1}catch(r){}},X={},J=()=>{if(!J.strings){var t={USER:"web_user",LOGNAME:"web_user",PATH:"/",PWD:"/",HOME:"/home/web_user",LANG:("object"==typeof navigator&&navigator.languages&&navigator.languages[0]||"C").replace("-","_")+".UTF-8",_:l||"./this.program"};for(var e in X)void 0===X[e]?delete t[e]:t[e]=X[e];var r=[];for(var e in t)r.push("".concat(e,"=").concat(t[e]));J.strings=r}return J.strings},Z=()=>V||!1,$=(t,e)=>{w=t,(t=>{var e;w=t,Z()||(null===(e=a.onExit)||void 0===e||e.call(a,t),M=!0),f(t,new B(t))})(t)},K=$;var Q=[null,[],[]],tt="undefined"!=typeof TextDecoder?new TextDecoder("utf8"):void 0,et=(t,e,r)=>{for(var n=e+r,a=e;t[a]&&!(a>=n);)++a;if(a-e>16&&t.buffer&&tt)return tt.decode(t.subarray(e,a));for(var i="";e<a;){var o=t[e++];if(128&o){var s=63&t[e++];if(192!=(224&o)){var u=63&t[e++];if((o=224==(240&o)?(15&o)<<12|s<<6|u:(7&o)<<18|s<<12|u<<6|63&t[e++])<65536)i+=String.fromCharCode(o);else{var c=o-65536;i+=String.fromCharCode(55296|c>>10,56320|1023&c)}}else i+=String.fromCharCode((31&o)<<6|s)}else i+=String.fromCharCode(o)}return i},rt=(t,e)=>{var r=Q[t];0===e||10===e?((1===t?g:v)(et(r,0)),r.length=0):r.push(e)},nt=(t,e)=>t?et(b,t,e):"",at=t=>t%4===0&&(t%100!==0||t%400===0),it=[31,29,31,30,31,30,31,31,30,31,30,31],ot=[31,28,31,30,31,30,31,31,30,31,30,31];var st,ut=(t,e,r,n)=>{var a=A[n+40>>2],i={tm_sec:D[n>>2],tm_min:D[n+4>>2],tm_hour:D[n+8>>2],tm_mday:D[n+12>>2],tm_mon:D[n+16>>2],tm_year:D[n+20>>2],tm_wday:D[n+24>>2],tm_yday:D[n+28>>2],tm_isdst:D[n+32>>2],tm_gmtoff:D[n+36>>2],tm_zone:a?nt(a):""},o=nt(r),s={"%c":"%a %b %d %H:%M:%S %Y","%D":"%m/%d/%y","%F":"%Y-%m-%d","%h":"%b","%r":"%I:%M:%S %p","%R":"%H:%M","%T":"%H:%M:%S","%x":"%m/%d/%y","%X":"%H:%M:%S","%Ec":"%c","%EC":"%C","%Ex":"%m/%d/%y","%EX":"%H:%M:%S","%Ey":"%y","%EY":"%Y","%Od":"%d","%Oe":"%e","%OH":"%H","%OI":"%I","%Om":"%m","%OM":"%M","%OS":"%S","%Ou":"%u","%OU":"%U","%OV":"%V","%Ow":"%w","%OW":"%W","%Oy":"%y"};for(var u in s)o=o.replace(new RegExp(u,"g"),s[u]);var c=["Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"],l=["January","February","March","April","May","June","July","August","September","October","November","December"];function f(t,e,r){for(var n="number"==typeof t?t.toString():t||"";n.length<e;)n=r[0]+n;return n}function d(t,e){return f(t,e,"0")}function m(t,e){function r(t){return t<0?-1:t>0?1:0}var n;return 0===(n=r(t.getFullYear()-e.getFullYear()))&&0===(n=r(t.getMonth()-e.getMonth()))&&(n=r(t.getDate()-e.getDate())),n}function h(t){switch(t.getDay()){case 0:return new Date(t.getFullYear()-1,11,29);case 1:return t;case 2:return new Date(t.getFullYear(),0,3);case 3:return new Date(t.getFullYear(),0,2);case 4:return new Date(t.getFullYear(),0,1);case 5:return new Date(t.getFullYear()-1,11,31);case 6:return new Date(t.getFullYear()-1,11,30)}}function p(t){var e=((t,e)=>{for(var r=new Date(t.getTime());e>0;){var n=at(r.getFullYear()),a=r.getMonth(),i=(n?it:ot)[a];if(!(e>i-r.getDate()))return r.setDate(r.getDate()+e),r;e-=i-r.getDate()+1,r.setDate(1),a<11?r.setMonth(a+1):(r.setMonth(0),r.setFullYear(r.getFullYear()+1))}return r})(new Date(t.tm_year+1900,0,1),t.tm_yday),r=new Date(e.getFullYear(),0,4),n=new Date(e.getFullYear()+1,0,4),a=h(r),i=h(n);return m(a,e)<=0?m(i,e)<=0?e.getFullYear()+1:e.getFullYear():e.getFullYear()-1}var y={"%a":t=>c[t.tm_wday].substring(0,3),"%A":t=>c[t.tm_wday],"%b":t=>l[t.tm_mon].substring(0,3),"%B":t=>l[t.tm_mon],"%C":t=>d((t.tm_year+1900)/100|0,2),"%d":t=>d(t.tm_mday,2),"%e":t=>f(t.tm_mday,2," "),"%g":t=>p(t).toString().substring(2),"%G":p,"%H":t=>d(t.tm_hour,2),"%I":t=>{var e=t.tm_hour;return 0==e?e=12:e>12&&(e-=12),d(e,2)},"%j":t=>d(t.tm_mday+((t,e)=>{for(var r=0,n=0;n<=e;r+=t[n++]);return r})(at(t.tm_year+1900)?it:ot,t.tm_mon-1),3),"%m":t=>d(t.tm_mon+1,2),"%M":t=>d(t.tm_min,2),"%n":()=>"\n","%p":t=>t.tm_hour>=0&&t.tm_hour<12?"AM":"PM","%S":t=>d(t.tm_sec,2),"%t":()=>"\t","%u":t=>t.tm_wday||7,"%U":t=>{var e=t.tm_yday+7-t.tm_wday;return d(Math.floor(e/7),2)},"%V":t=>{var e=Math.floor((t.tm_yday+7-(t.tm_wday+6)%7)/7);if((t.tm_wday+371-t.tm_yday-2)%7<=2&&e++,e){if(53==e){var r=(t.tm_wday+371-t.tm_yday)%7;4==r||3==r&&at(t.tm_year)||(e=1)}}else{e=52;var n=(t.tm_wday+7-t.tm_yday-1)%7;(4==n||5==n&&at(t.tm_year%400-1))&&e++}return d(e,2)},"%w":t=>t.tm_wday,"%W":t=>{var e=t.tm_yday+7-(t.tm_wday+6)%7;return d(Math.floor(e/7),2)},"%y":t=>(t.tm_year+1900).toString().substring(2),"%Y":t=>t.tm_year+1900,"%z":t=>{var e=t.tm_gmtoff,r=e>=0;return e=(e=Math.abs(e)/60)/60*100+e%60,(r?"+":"-")+String("0000"+e).slice(-4)},"%Z":t=>t.tm_zone,"%%":()=>"%"};for(var u in o=o.replace(/%%/g,"\0\0"),y)o.includes(u)&&(o=o.replace(new RegExp(u,"g"),y[u](i)));var g,v,w=function(t,e,r){var n=r>0?r:(t=>{for(var e=0,r=0;r<t.length;++r){var n=t.charCodeAt(r);n<=127?e++:n<=2047?e+=2:n>=55296&&n<=57343?(e+=4,++r):e+=3}return e})(t)+1,a=new Array(n),i=((t,e,r,n)=>{if(!(n>0))return 0;for(var a=r,i=r+n-1,o=0;o<t.length;++o){var s=t.charCodeAt(o);if(s>=55296&&s<=57343&&(s=65536+((1023&s)<<10)|1023&t.charCodeAt(++o)),s<=127){if(r>=i)break;e[r++]=s}else if(s<=2047){if(r+1>=i)break;e[r++]=192|s>>6,e[r++]=128|63&s}else if(s<=65535){if(r+2>=i)break;e[r++]=224|s>>12,e[r++]=128|s>>6&63,e[r++]=128|63&s}else{if(r+3>=i)break;e[r++]=240|s>>18,e[r++]=128|s>>12&63,e[r++]=128|s>>6&63,e[r++]=128|63&s}}return e[r]=0,r-a})(t,a,0,a.length);return e&&(a.length=i),a}(o=o.replace(/\0\0/g,"%"),!1);return w.length>e?0:(g=w,v=t,_.set(g,v),w.length-1)},ct=t=>{if(t instanceof B||"unwind"==t)return w;f(1,t)},lt=t=>{try{return t()}catch(e){T(e)}},ft=t=>{if(!M)try{t(),(()=>{if(!Z())try{K(w)}catch(t){ct(t)}})()}catch(e){ct(e)}},dt={instrumentWasmImports(t){var e=/^(emscripten_utils_get_double_impl|invoke_.*|__asyncjs__.*)$/;for(let[r,n]of Object.entries(t)){n.sig;if("function"==typeof n){n.isAsync||e.test(r)}}},instrumentWasmExports(t){var e={};for(let[r,n]of Object.entries(t))e[r]="function"==typeof n?function(){dt.exportCallStack.push(r);try{return n(...arguments)}finally{if(!M){dt.exportCallStack.pop();dt.maybeStopUnwind()}}}:n;return e},State:{Normal:0,Unwinding:1,Rewinding:2,Disabled:3},state:0,StackSize:4096,currData:null,handleSleepReturnValue:0,exportCallStack:[],callStackNameToId:{},callStackIdToName:{},callStackId:0,asyncPromiseHandlers:null,sleepCallbacks:[],getCallStackId(t){var e=dt.callStackNameToId[t];return void 0===e&&(e=dt.callStackId++,dt.callStackNameToId[t]=e,dt.callStackIdToName[e]=t),e},maybeStopUnwind(){dt.currData&&dt.state===dt.State.Unwinding&&0===dt.exportCallStack.length&&(dt.state=dt.State.Normal,lt(_t),"undefined"!=typeof Fibers&&Fibers.trampoline())},whenDone:()=>new Promise(((t,e)=>{dt.asyncPromiseHandlers={resolve:t,reject:e}})),allocateData(){var t=gt(12+dt.StackSize);return dt.setDataHeader(t,t+12,dt.StackSize),dt.setDataRewindFunc(t),t},setDataHeader(t,e,r){A[t>>2]=e,A[t+4>>2]=e+r},setDataRewindFunc(t){var e=dt.exportCallStack[0],r=dt.getCallStackId(e);D[t+8>>2]=r},getDataRewindFunc(t){var e=D[t+8>>2],r=dt.callStackIdToName[e];return ht[r]},doRewind:t=>dt.getDataRewindFunc(t)(),handleSleep(t){if(!M){if(dt.state===dt.State.Normal){var e=!1,r=!1;t((function(){let t=arguments.length>0&&void 0!==arguments[0]?arguments[0]:0;if(!M&&(dt.handleSleepReturnValue=t,e=!0,r)){dt.state=dt.State.Rewinding,lt((()=>bt(dt.currData))),"undefined"!=typeof Browser&&Browser.mainLoop.func&&Browser.mainLoop.resume();var n,a=!1;try{n=dt.doRewind(dt.currData)}catch(v){n=v,a=!0}var i=!1;if(!dt.currData){var o=dt.asyncPromiseHandlers;o&&(dt.asyncPromiseHandlers=null,(a?o.reject:o.resolve)(n),i=!0)}if(a&&!i)throw n}})),r=!0,e||(dt.state=dt.State.Unwinding,dt.currData=dt.allocateData(),"undefined"!=typeof Browser&&Browser.mainLoop.func&&Browser.mainLoop.pause(),lt((()=>wt(dt.currData))))}else dt.state===dt.State.Rewinding?(dt.state=dt.State.Normal,lt(St),pt(dt.currData),dt.currData=null,dt.sleepCallbacks.forEach(ft)):T("invalid state: ".concat(dt.state));return dt.handleSleepReturnValue}},handleAsync:t=>dt.handleSleep((e=>{t().then(e)}))},mt={a:(t,e,r)=>{throw new z(t).init(e,r),t},b:()=>{T("")},g:(t,e,r)=>b.copyWithin(t,e,e+r),f:t=>{var e=b.length,r=2147483648;if((t>>>=0)>r)return!1;for(var n,a,i=1;i<=4;i*=2){var o=e*(1+.2/i);o=Math.min(o,t+100663296);var s=Math.min(r,(n=Math.max(t,o))+((a=65536)-n%a)%a);if(G(s))return!0}return!1},i:function(){a.queue.clear()},h:function(){return dt.handleAsync((async()=>(t=>{const e=gt(8);return q(e,t,"double"),e})(await a.queue.get())))},l:(t,e)=>{var r=0;return J().forEach(((n,a)=>{var i=e+r;A[t+4*a>>2]=i,((t,e)=>{for(var r=0;r<t.length;++r)_[e++]=t.charCodeAt(r);_[e]=0})(n,i),r+=n.length+1})),0},m:(t,e)=>{var r=J();A[t>>2]=r.length;var n=0;return r.forEach((t=>n+=t.length+1)),A[e>>2]=n,0},n:K,d:t=>52,e:(t,e,r,n)=>52,j:function(t,e,r,n,a){return r,e,70},c:(t,e,r,n)=>{for(var a=0,i=0;i<r;i++){var o=A[e>>2],s=A[e+4>>2];e+=8;for(var u=0;u<s;u++)rt(t,b[o+u]);a+=s}return A[n>>2]=a,0},k:(t,e,r,n,a)=>ut(t,e,r,n)},ht=function(){var t,e,r,i,o={a:mt};function s(t,e){var r;return ht=t.exports,ht=dt.instrumentWasmExports(ht),y=ht.o,E(),r=ht.p,x.unshift(r),function(t){var e;if(H--,null===(e=a.monitorRunDependencies)||void 0===e||e.call(a,H),0==H&&(null!==C&&(clearInterval(C),C=null),O)){var r=O;O=null,r()}}(),ht}if(function(t){var e;H++,null===(e=a.monitorRunDependencies)||void 0===e||e.call(a,H)}(),a.instantiateWasm)try{return a.instantiateWasm(o,s)}catch(u){v("Module.instantiateWasm callback failed with error: ".concat(u)),n(u)}return(t=p,e=W,r=o,i=function(t){s(t.instance)},t||"function"!=typeof WebAssembly.instantiateStreaming||j(e)||"function"!=typeof fetch?N(e,r,i):fetch(e,{credentials:"same-origin"}).then((t=>WebAssembly.instantiateStreaming(t,r).then(i,(function(t){return v("wasm streaming compile failed: ".concat(t)),v("falling back to ArrayBuffer instantiation"),N(e,r,i)}))))).catch(n),{}}(),pt=t=>(pt=ht.r)(t),yt=a._main=(t,e)=>(yt=a._main=ht.s)(t,e),gt=t=>(gt=ht.t)(t),vt=t=>(vt=ht.u)(t),wt=t=>(wt=ht.v)(t),_t=()=>(_t=ht.w)(),bt=t=>(bt=ht.x)(t),St=()=>(St=ht.y)();a.___start_em_js=27716,a.___stop_em_js=27974;function Dt(){function t(){st||(st=!0,a.calledRun=!0,M||(!0,L(x),L(I),r(a),a.onRuntimeInitialized&&a.onRuntimeInitialized(),At&&function(){var t=yt;try{var e=t(0,0);return $(e,!0),e}catch(r){return ct(r)}}(),function(){if(a.postRun)for("function"==typeof a.postRun&&(a.postRun=[a.postRun]);a.postRun.length;)t=a.postRun.shift(),P.unshift(t);var t;L(P)}()))}H>0||(!function(){if(a.preRun)for("function"==typeof a.preRun&&(a.preRun=[a.preRun]);a.preRun.length;)t=a.preRun.shift(),F.unshift(t);var t;L(F)}(),H>0||(a.setStatus?(a.setStatus("Running..."),setTimeout((function(){setTimeout((function(){a.setStatus("")}),1),t()}),1)):t()))}if(a.setValue=q,O=function t(){st||Dt(),st||(O=t)},a.preInit)for("function"==typeof a.preInit&&(a.preInit=[a.preInit]);a.preInit.length>0;)a.preInit.pop()();var At=!0;return a.noInitialRun&&(At=!1),Dt(),e.ready}})();t.exports=e}},e={};function r(n){var a=e[n];if(void 0!==a)return a.exports;var i=e[n]={exports:{}};return t[n](i,i.exports,r),i.exports}r.n=t=>{var e=t&&t.__esModule?()=>t.default:()=>t;return r.d(e,{a:e}),e},r.d=(t,e)=>{for(var n in e)r.o(e,n)&&!r.o(t,n)&&Object.defineProperty(t,n,{enumerable:!0,get:e[n]})},r.o=(t,e)=>Object.prototype.hasOwnProperty.call(t,e),r.p="/reference-finder/",(()=>{"use strict";var t=r(815),e=r.n(t);const n=r.p+"static/bin/ref.405206f9.wasm";e()({locateFile:t=>t.endsWith(".wasm")?n:t})})()})();