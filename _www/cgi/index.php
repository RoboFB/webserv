<?php
require __DIR__ . '/demo_session.php';

header('Content-Type: text/html; charset=utf-8');
header('Cache-Control: no-store');

$isPost = ($_SERVER['REQUEST_METHOD'] ?? 'GET') === 'POST';
$action = $isPost ? ($_POST['action'] ?? 'message') : '';
$color = demo_color();
$status = '';
$messages = [];
$savedMessage = false;

if ($action === 'clear') {
	if (isset($_COOKIE[DEMO_SESSION_COOKIE])) {
		open_demo_session();
		$_SESSION = [];
		session_destroy();
	}
	expire_demo_cookie(DEMO_SESSION_COOKIE, DEMO_SESSION_COOKIE_OPTIONS);
	expire_demo_cookie(DEMO_COLOR_COOKIE, DEMO_COLOR_COOKIE_OPTIONS);
	$color = '#ffffff';
	$status = 'Cleared the saved color and recent messages.';
} else {
	open_demo_session();
	$messages = $_SESSION['messages'] ?? [];
	$message = $_POST['message'] ?? null;
	$savedMessage = $action === 'message' && is_string($message) && trim($message) !== '';
	if ($savedMessage) {
		array_unshift($messages, $message);
		$messages = array_slice($messages, 0, 5);
		$_SESSION['messages'] = $messages;
	}
	session_write_close();

	$legacyColor = $_COOKIE[LEGACY_DEMO_COLOR_COOKIE] ?? null;
	$saveColor = !valid_demo_color($_COOKIE[DEMO_COLOR_COOKIE] ?? null)
		&& valid_demo_color($legacyColor);
	if ($action === 'save') {
		$chosen = $_POST['color'] ?? null;
		if (valid_demo_color($chosen)) {
			$color = $chosen;
			$saveColor = true;
			$status = 'Background saved for seven days.';
		} else {
			$status = 'Choose a valid background color.';
		}
	}
	if ($saveColor) save_demo_color($color);
}
if (isset($_COOKIE[LEGACY_DEMO_COLOR_COOKIE])) {
	expire_demo_cookie(LEGACY_DEMO_COLOR_COOKIE, DEMO_SESSION_COOKIE_OPTIONS);
}

$latest = $messages[0] ?? '';
$outgoingCookies = array_filter(headers_list(), static function ($header) {
	return stripos($header, 'Set-Cookie:') === 0;
});
?>
<!DOCTYPE html>
<html lang="en" style="<?= demo_html(demo_page_style($color)) ?>">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>The wonderful world of CGI</title>
	<link rel="stylesheet" href="/assets/reset.css">
	<link rel="stylesheet" href="/assets/main.css">
</head>
<body class="cgi-page">
	<header>
		<p class="site-brand">HELLO WEBSERV?!</p>
		<nav aria-label="Demo mode">
			<ul>
				<li><a href="/">static</a></li>
				<li><a href="/cgi/" aria-current="page">cgi</a></li>
			</ul>
		</nav>
	</header>
	<main>
		<h1>CGI request viewer</h1>
		<section aria-labelledby="post-heading">
			<h2 id="post-heading">Post a message</h2>
			<form action="/cgi/" method="post">
				<label for="cgi-message">Message</label>
				<div class="control-row">
					<input id="cgi-message" name="message" required>
					<button type="submit">Send</button>
				</div>
			</form>
		</section>
		<?php if ($latest !== ''): ?>
			<section aria-labelledby="message-heading">
				<h2 id="message-heading"><?= $savedMessage ? 'Received message' : 'Last saved message' ?></h2>
				<p class="cgi-message"><?= demo_html($latest) ?></p>
				<?php if (count($messages) > 1): ?>
					<details class="message-history">
						<summary>Earlier messages</summary>
						<ol>
							<?php foreach (array_slice($messages, 1) as $previous): ?>
								<li><?= demo_html($previous) ?></li>
							<?php endforeach; ?>
						</ol>
					</details>
				<?php endif; ?>
			</section>
		<?php endif; ?>
		<section id="cookies" aria-labelledby="cookies-heading">
			<h2 id="cookies-heading">Cookies and sessions</h2>
			<p>The background lasts seven days. PHP keeps the last five messages in this session.</p>
			<div>
				<label for="demo-color">Background color</label>
				<div class="control-row">
					<form class="control-row" action="/cgi/#cookies" method="post">
						<input type="hidden" name="action" value="save">
						<input type="color" id="demo-color" name="color" value="<?= demo_html($color) ?>">
						<button type="submit">Save color</button>
					</form>
					<form action="/cgi/#cookies" method="post">
						<input type="hidden" name="action" value="clear">
						<button type="submit">Clear both</button>
					</form>
				</div>
			</div>
			<?php if ($status !== ''): ?>
				<p role="status"><?= demo_html($status) ?></p>
			<?php endif; ?>
			<h3>Response cookie headers</h3>
			<pre><?= $outgoingCookies === [] ? '(No Set-Cookie headers)' : demo_html(implode("\n", $outgoingCookies)) ?></pre>
		</section>
		<section aria-labelledby="request-heading">
			<h2 id="request-heading">HTTP request</h2>
			<?php
				$target = $_SERVER['SCRIPT_NAME'] ?? '/';
				if (($_SERVER['QUERY_STRING'] ?? '') !== '') {
					$target .= '?' . $_SERVER['QUERY_STRING'];
				}

				$request = ($_SERVER['REQUEST_METHOD'] ?? 'GET') . ' ' . $target;
				foreach (getallheaders() as $name => $value) {
					$request .= "\n$name: $value";
				}

				$body = file_get_contents('php://input', false, null, 0, 1201);
				if ($body !== false && $body !== '') {
					$request .= "\n\n" . $body;
				}

				$reqPreview = substr($request, 0, 1200);
				if (strlen($request) > 1200) {
					$reqPreview .= "\n…";
				}
			?>
			<pre><?= demo_html($reqPreview) ?></pre>
		</section>
	</main>
</body>
</html>
