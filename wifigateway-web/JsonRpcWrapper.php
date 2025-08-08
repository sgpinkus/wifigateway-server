<?php

/**
 * Takes an arbitrary object and turns its return values into JSON-RPC.
 * The object should throw exceptions on errors.
 * @see http://json-rpc.org/wiki/specification.
 */
class JsonRpcWrapper
{
	private $obj = null;

	public function __construct($obj)
	{
		if(!is_object($obj))
		{
			throw new Exception("Require object. Given " . gettype($obj));
		}
		$this->obj = $obj;
	}

	public function call($body)
	{
		if(is_string($body))
		{
			$body = json_decode($body);
		}
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
		$body['method'] = (isset($body['method']) ? $body['method'] : "");
		$body['params'] = (isset($body['params']) ? $body['params'] : array());
		$body['id'] = (isset($body['id']) ? $body['id'] : null);

		if(is_callable(array($this->obj, $body['method'])))
		{
			try
			{
				$result = call_user_func_array(array($this->obj, $body['method']), $body['params']);
				return json_encode(array(
					'result' => $result,
					'error' => null,
					'id' => $body['id']
				));
			}
			catch(Exception $e)
			{
				return json_encode(array(
					'result' => null,
					'error' => array(
						'code' => $e->getCode(),
						'message' => $e->getMessage()
					),
					'id' => $body['id']
				));
			}
		}
		else
		{
			return json_encode(array(
				'result' => null,
				'error' => array(
					'code' => -(32601),
					'message' => "Method not found",
					'id' => $body['id']
				)
			));
		}
	}
}
