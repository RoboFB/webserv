'use strict';

function applyBackground() {
	const cookie = document.cookie.split(/;\s*/).find(value => value.startsWith('WebservDemoTheme='));
	const color = cookie?.slice('WebservDemoTheme='.length).match(/^(?:%23|#)([0-9a-f]{6})$/i)?.[1] ?? 'ffffff';
	const [red, green, blue] = [0, 2, 4].map(offset => {
		const channel = parseInt(color.slice(offset, offset + 2), 16) / 255;
		return channel <= 0.04045 ? channel / 12.92 : ((channel + 0.055) / 1.055) ** 2.4;
	});
	const luminance = 0.2126 * red + 0.7152 * green + 0.0722 * blue;
	document.documentElement.style.setProperty('--background', '#' + color);
	document.documentElement.style.setProperty('--page-text', luminance >= 0.179 ? '#000' : '#fff');
}

applyBackground();
window.addEventListener('pageshow', applyBackground);

const responseWindow = document.querySelector('#response-window');
const requestText = document.querySelector('#request-text');
const responseText = document.querySelector('#response-text');

const altSite = new URL('/', location.href);
altSite.port = '5002';
document.querySelector('#alt-site').href = altSite.href;

function previewBody(body) {
	const edgeSize = 2048;
	if (body.length <= 2 * edgeSize) return body;

	const start = body.slice(0, edgeSize).replace(/[\uD800-\uDBFF]$/, '');
	const end = body.slice(-edgeSize).replace(/^[\uDC00-\uDFFF]/, '');
	return start + '\n[…]\n' + end;
}

async function formatRequest(request) {
	const url = new URL(request.url);
	const headers = Array.from(request.headers, ([name, value]) => name + ': ' + value);
	return [request.method + ' ' + url.pathname + url.search, ...headers, '', previewBody(await request.text())].join('\n');
}

async function formatResponse(response) {
	const status = [response.status, response.statusText].filter(Boolean).join(' ');
	const headers = Array.from(response.headers, ([name, value]) => name + ': ' + value);
	const destination = response.redirected ? ['Final URL: ' + response.url] : [];
	let body;
	try {
		body = previewBody(await response.text());
	} catch (error) {
		body = 'Could not read the response body: ' + error.message;
	}
	return [status, ...destination, ...headers, '', body].join('\n');
}

async function runRequest(button, request, onResponse) {
	if (button.disabled) return;
	button.disabled = true;
	let requestDetails = '';
	let responseDetails;
	try {
		requestDetails = await formatRequest(request.clone());
		const response = await fetch(request);
		onResponse?.(response);
		responseDetails = await formatResponse(response);
	} catch (error) {
		responseDetails = 'No HTTP response available to the page.\n' + error.message;
	} finally {
		button.disabled = false;
	}
	requestText.textContent = requestDetails;
	responseText.textContent = responseDetails;
	if (!responseWindow.open) responseWindow.showModal();
	responseWindow.scrollTop = 0;
}

const uploadForm = document.querySelector('#upload');
const uploadPicker = document.querySelector('#file');
const uploadButton = uploadForm.querySelector('button[type="submit"]');
const uploadList = document.querySelector('#uploaded-files');
const uploadListStatus = document.querySelector('#upload-list-status');
const uploadTemplate = document.querySelector('#uploaded-file');
const uploads = new Map();

function uploadFile(button, file, name) {
	const data = new FormData();
	data.append('file', file, name);
	return runRequest(button, new Request('/upload', {
		method: 'POST',
		body: data,
	}), response => {
		if (response.ok) showUpload(name, file.type.startsWith('image/'), '?v=' + Date.now());
	});
}

function createUpload(name) {
	const figure = uploadTemplate.content.firstElementChild.cloneNode(true);
	const url = '/upload/' + encodeURIComponent(name);
	const replaceButton = figure.querySelector('[data-action="replace"]');
	const deleteButton = figure.querySelector('[data-action="delete"]');
	const picker = figure.querySelector('input[type="file"]');

	figure.querySelector('figcaption').textContent = name;
	figure.querySelector('a').href = url;
	replaceButton.addEventListener('click', () => picker.click());
	picker.addEventListener('change', async () => {
		const file = picker.files[0];
		if (!file) return;
		picker.value = '';
		deleteButton.disabled = true;
		try {
			await uploadFile(replaceButton, file, name);
		} finally {
			deleteButton.disabled = false;
		}
	});
	deleteButton.addEventListener('click', async () => {
		replaceButton.disabled = true;
		try {
			await runRequest(deleteButton, new Request(url, { method: 'DELETE' }), response => {
				if (!response.ok) return;
				figure.remove();
				uploads.delete(name);
			});
		} finally {
			replaceButton.disabled = false;
		}
	});

	uploadList.append(figure);
	uploads.set(name, figure);
	return figure;
}

function showUpload(name, isImage, version = '') {
	const figure = uploads.get(name) ?? createUpload(name);
	const image = figure.querySelector('img');
	image.hidden = !isImage;
	if (isImage) image.src = figure.querySelector('a').href + version;
	else image.removeAttribute('src');
}

async function loadUploads() {
	uploadButton.disabled = true;
	try {
		const response = await fetch('/upload/', { cache: 'no-store' });
		if (!response.ok) throw new Error('HTTP ' + response.status);

		const listing = new DOMParser().parseFromString(await response.text(), 'text/html');
		if (listing.querySelector('body > h1')?.textContent !== 'Index of /upload/') {
			throw new Error('Expected the Webserv directory listing.');
		}
		const names = [...listing.querySelectorAll('ul > li > a[href]')]
			.filter(link => /^(?:[a-z0-9_.~-]|%[a-f0-9]{2})+$/i.test(link.getAttribute('href')))
			.map(link => link.textContent);
		names.sort((left, right) => left.localeCompare(right));
		for (const name of names) showUpload(name, /\.(?:jpe?g|png|svg)$/i.test(name));
	} catch (error) {
		uploadListStatus.textContent = 'Could not load existing uploads. ' + error.message;
		uploadListStatus.hidden = false;
	} finally {
		uploadButton.disabled = false;
	}
}

uploadForm.addEventListener('submit', event => {
	event.preventDefault();
	const file = uploadPicker.files[0];
	if (file) uploadFile(uploadButton, file, file.name);
});
loadUploads();

const quickTests = [
	['#test-405', '/', { method: 'POST' }],
	['#test-501', '/', { method: 'RANDOM' }],
	['#test-413', '/limit-demo', {
		method: 'POST',
		body: new Uint8Array(1025),
	}],
];

for (const [selector, path, options] of quickTests) {
	const button = document.querySelector(selector);
	button.addEventListener('click', () => runRequest(button, new Request(path, options)));
}

const builder = document.querySelector('#request-builder');
const methodField = builder.elements.namedItem('method');
const targetField = builder.elements.namedItem('target');
const headerField = builder.elements.namedItem('headers');
const bodyField = builder.elements.namedItem('body');
const sendButton = builder.querySelector('button[type="submit"]');
const requestStatus = document.querySelector('#request-status');

function syncBody() {
	bodyField.disabled = /^(GET|HEAD)$/i.test(methodField.value.trim());
}

function buildRequest() {
	const path = targetField.value.trim();
	const url = new URL(path, location.origin);
	if (!path.startsWith('/') || path.startsWith('//') || url.origin !== location.origin || url.hash) {
		throw new Error('Enter a local path starting with /, with an optional query.');
	}

	const headers = new Headers();
	for (const line of headerField.value.split('\n')) {
		if (!line.trim()) continue;
		const colon = line.indexOf(':');
		if (colon < 1) throw new Error('Write each header as Name: value.');
		headers.append(line.slice(0, colon).trim(), line.slice(colon + 1).trim());
	}

	const request = new Request(url, {
		method: methodField.value.trim(),
		headers,
		body: bodyField.disabled || bodyField.value === '' ? undefined : bodyField.value,
		mode: 'same-origin',
	});
	for (const name of headers.keys()) {
		if (!request.headers.has(name)) throw new Error('The browser controls the ' + name + ' header.');
	}
	return request;
}

methodField.addEventListener('input', syncBody);
syncBody();

builder.addEventListener('submit', event => {
	event.preventDefault();
	requestStatus.textContent = '';
	try {
		syncBody();
		runRequest(sendButton, buildRequest());
	} catch (error) {
		requestStatus.textContent = error.message;
	}
});
