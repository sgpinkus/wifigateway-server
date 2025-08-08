<?php
require_once 'Gateway.php';
require_once 'JsonRpcWrapper.php';

if($argc < 2)
{
  echo "Bad args\n";
  exit();
}
$proxy = new JsonRpcWrapper(new Gateway());
array_shift($argv);
$method = array_shift($argv);
var_dump($proxy->call(array('method' => $method, 'params' => $argv)));
?>
  
