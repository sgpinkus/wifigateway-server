<?php
date_default_timezone_set(@date_default_timezone_get());
#@touch(dirname(__FILE__)."/log/error.log"));
#@touch(dirname(__FILE__)."/log/debug.log"));
ini_set("error_reporting", E_ALL);
ini_set("display_errors", 1);
ini_set("log_errors", 1);
ini_set("error_log", dirname(__FILE__)."/log/error.log");


function debug_log($str) 
{
	if(is_writable(dirname(__FILE__)."/log/debug.log"))
	{
		file_put_contents(dirname(__FILE__)."/log/debug.log", var_dump_s($str), FILE_APPEND);
	}	
}

function var_dump_s($var)
{
  ob_start();
  var_dump($var);
  $c = ob_get_contents();
  ob_end_clean();
  return $c;
}
?>
