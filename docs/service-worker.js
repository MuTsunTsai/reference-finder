if(!self.define){let e,r={};const n=(n,i)=>(n=new URL(n+".js",i).href,r[n]||new Promise((r=>{if("document"in self){const e=document.createElement("script");e.src=n,e.onload=r,document.head.appendChild(e)}else e=n,importScripts(n),r()})).then((()=>{let e=r[n];if(!e)throw new Error(`Module ${n} didn’t register its module`);return e})));self.define=(i,s)=>{const f=e||("document"in self?document.currentScript.src:"")||location.href;if(r[f])return;let c={};const t=e=>n(e,f),l={module:{uri:f},exports:c,require:t};r[f]=Promise.all(i.map((e=>l[e]||t(e)))).then((e=>(s(...e),c)))}}define(["./workbox-cbbbbfe9"],(function(e){"use strict";self.skipWaiting(),e.clientsClaim(),e.precacheAndRoute([{url:"/reference-finder/index.html",revision:"2524ccfd437e15c1c9f3fc07aafc9244"},{url:"/reference-finder/static/css/index.fc316719.css",revision:null},{url:"/reference-finder/static/font/fa-solid-900.2584ff1a.woff2",revision:null},{url:"/reference-finder/static/font/fa-solid-900.6ca5a22c.ttf",revision:null},{url:"/reference-finder/static/font/twemoji.0418fc94.woff2",revision:null},{url:"/reference-finder/static/js/async/ref.187728d7.js",revision:null},{url:"/reference-finder/static/js/index.ada6a06e.js",revision:null},{url:"/reference-finder/static/js/locale.04c91d64.js",revision:null},{url:"/reference-finder/static/js/rabbit-ear.16b592a7.js",revision:null},{url:"/reference-finder/static/js/react.c87cef15.js",revision:null},{url:"/reference-finder/static/js/vendor.9b65bd23.js",revision:null},{url:"/reference-finder/static/wasm/fe23f114cba2f9d6.module.wasm",revision:null}],{})}));
