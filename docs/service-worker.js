if(!self.define){let e,r={};const n=(n,i)=>(n=new URL(n+".js",i).href,r[n]||new Promise((r=>{if("document"in self){const e=document.createElement("script");e.src=n,e.onload=r,document.head.appendChild(e)}else e=n,importScripts(n),r()})).then((()=>{let e=r[n];if(!e)throw new Error(`Module ${n} didn’t register its module`);return e})));self.define=(i,s)=>{const f=e||("document"in self?document.currentScript.src:"")||location.href;if(r[f])return;let t={};const c=e=>n(e,f),l={module:{uri:f},exports:t,require:c};r[f]=Promise.all(i.map((e=>l[e]||c(e)))).then((e=>(s(...e),t)))}}define(["./workbox-099bf95e"],(function(e){"use strict";self.skipWaiting(),e.clientsClaim(),e.precacheAndRoute([{url:"/reference-finder/index.html",revision:"8356ff603d091b8b6f4a8538190f4500"},{url:"/reference-finder/static/css/index.049450d6.css",revision:null},{url:"/reference-finder/static/font/fa-solid-900.157cb9d4.woff2",revision:null},{url:"/reference-finder/static/font/fa-solid-900.dd77acae.ttf",revision:null},{url:"/reference-finder/static/font/twemoji.249d008a.woff2",revision:null},{url:"/reference-finder/static/js/async/466.fdb26167.js",revision:null},{url:"/reference-finder/static/js/async/842.9e5cf135.js",revision:null},{url:"/reference-finder/static/js/async/847.1537b6af.js",revision:null},{url:"/reference-finder/static/js/async/910.923443d0.js",revision:null},{url:"/reference-finder/static/js/async/996.78df147c.js",revision:null},{url:"/reference-finder/static/js/async/ref.5b48792c.js",revision:null},{url:"/reference-finder/static/js/index.92e3d270.js",revision:null},{url:"/reference-finder/static/js/rabbit-ear.126278b0.js",revision:null},{url:"/reference-finder/static/js/react.da7820e5.js",revision:null},{url:"/reference-finder/static/js/vendor.f271b781.js",revision:null},{url:"/reference-finder/static/wasm/fccac13ec235488c.module.wasm",revision:null}],{})}));
