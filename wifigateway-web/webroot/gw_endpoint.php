<?php
/**
 * End point for web calls.
 */
ini_set('include_path', dirname(dirname(__FILE__)) . "/:" . ini_get('include_path'));
require_once "gw_logging.php";
require_once "Gateway.php";
require_once "JsonRpcWrapper.php";

$gw = new JsonRpcWrapper(new Gateway());
$body = @file_get_contents('php://input'); //http_get_request_body();
$body = $_POST;
debug_log($body);

if(!is_array($body))
{
	return json_encode(array(
		'result' => null,
		'error' => array(
			'code' => -(32600),
			'message' => "Invalid Request"
		)
	));
}

// Hmm I need to unpack it anyway to do ~~sec check. ip param should always be a remote addr here.
$body['params']['ip'] = (empty($body['params']['ip']) ? $_SERVER['REMOTE_ADDR'] : $body['params']['ip']);

if($body['params']['ip'] != $_SERVER['REMOTE_ADDR'])
{
	return json_encode(array(
		'result' => null,
		'error' => array(
			'code' => -(32602),
			'message' => "Invalid params"
		)
	));	
}

$r = $gw->call($body);
debug_log($r);
echo $r;
?>