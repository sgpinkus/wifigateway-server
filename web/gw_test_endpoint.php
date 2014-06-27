<?php

require_once( "gw.php" );

if( $argc < 2 )
{
  echo "Bad args\n";
  exit();
}
if( ! function_exists( "gw_".$argv[1] ) )
{
  echo "{$argv[1]} not a function\n";
  exit();
}

$ip =  ( empty($argv[2]) ? null : $argv[2] );
$set = ( empty($argv[3]) ? null : $argv[3] );
if( strpos( $argv[1], "set_" ) === 0  )
{
  call_user_func( "gw_".$argv[1], $ip, (int)$set );
}
else
{  
  call_user_func( "gw_".$argv[1], $ip );
}
?>
  
