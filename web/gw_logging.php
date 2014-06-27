<?php
date_default_timezone_set(@date_default_timezone_get());
ini_set( "error_reporting", E_ALL );
ini_set( "display_errors", 1 );
//ini_set( "error_append_string" , "<br />" );
ini_set( "log_errors", 1 );
ini_set( "error_log", dirname( __FILE__ )."/error.log" );

function debug_log( $str ) 
{
  file_put_contents( dirname( __FILE__ )."/debug.log", $str, FILE_APPEND );
}

function var_dump_s( $var )
{
  ob_start();
  var_dump( $var );
  $c = ob_get_contents();
  ob_end_clean();
  return $c;
}
?>
