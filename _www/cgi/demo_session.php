<?php
const DEMO_SESSION_COOKIE = 'WebservDemoSession';
const DEMO_COLOR_COOKIE = 'WebservDemoTheme';
const LEGACY_DEMO_COLOR_COOKIE = 'WebservDemoColor';
const DEMO_SESSION_COOKIE_OPTIONS = [
	'path' => '/cgi/',
	'httponly' => true,
	'samesite' => 'Lax',
];
const DEMO_COLOR_COOKIE_OPTIONS = ['path' => '/', 'samesite' => 'Lax'];

function open_demo_session(): void
{
	session_name(DEMO_SESSION_COOKIE);
	session_set_cookie_params(['lifetime' => 0] + DEMO_SESSION_COOKIE_OPTIONS);
	if (!session_start([
		'use_strict_mode' => true,
		'use_only_cookies' => true,
		'cache_limiter' => '',
	])) {
		http_response_code(500);
		exit('Could not open the demo session.');
	}
}

function valid_demo_color($value): bool
{
	return is_string($value) && preg_match('/^#[0-9a-f]{6}$/iD', $value) === 1;
}

function demo_color(): string
{
	foreach ([DEMO_COLOR_COOKIE, LEGACY_DEMO_COLOR_COOKIE] as $name) {
		$value = $_COOKIE[$name] ?? null;
		if (valid_demo_color($value)) return $value;
	}
	return '#ffffff';
}

function save_demo_color(string $color): void
{
	setcookie(DEMO_COLOR_COOKIE, $color, ['expires' => time() + 7 * 86400] + DEMO_COLOR_COOKIE_OPTIONS);
}

function expire_demo_cookie(string $name, array $options): void
{
	setcookie($name, '', ['expires' => time() - 3600] + $options);
}

function demo_page_style(string $color): string
{
	$linear = [];
	foreach ([1, 3, 5] as $offset) {
		$channel = hexdec(substr($color, $offset, 2)) / 255;
		$linear[] = $channel <= 0.04045 ? $channel / 12.92 : (($channel + 0.055) / 1.055) ** 2.4;
	}
	$luminance = 0.2126 * $linear[0] + 0.7152 * $linear[1] + 0.0722 * $linear[2];
	$text = $luminance >= 0.179 ? '#000000' : '#ffffff';

	return '--background: ' . $color . '; --page-text: ' . $text;
}

function demo_html(string $text): string
{
	return htmlspecialchars($text, ENT_QUOTES | ENT_SUBSTITUTE, 'UTF-8');
}
