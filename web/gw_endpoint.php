<?php
/**
 * End point for web calls.
 * Even though the class returns JSON reponses, kept sep for testing.
 * @todo auth for ip parametized.
 */
require_once( "gw_logging.php" );
require_once( "gw.php" );
$host_ip = null;
$action = null;
 
//debug_log( "B ".$_POST['ip']." ".var_dump_s( $_POST['ip'] ) );
 
// Should always be a remote addr even if dont use.
$host_ip = ( empty( $_POST['ip'] ) ? null : $_POST['ip'] );
$host_ip = ( empty( $host_ip ) ? $_SERVER['REMOTE_ADDR'] : $host_ip );

//debug_log( "host_ip ".$host_ip."\n\n" );

if( empty( $host_ip ) )
{
  echo json_encode( array( "status" => "fail", "message" => "no IP address provided!" ) );
  exit();
}
$action = ( empty( $_POST['action'] ) ? "x" : $_POST['action'] );


switch( $action )
{
  case "start_session" :
  {
    gw_start_session($host_ip);
    break;
  }
  case "pause_session" :
  {
    gw_pause_session($host_ip);
    break;
  }
  case "play_session" :
  {
    gw_play_session($host_ip);
    break;
  }
  case "end_session" :
  {
    gw_end_session($host_ip);
    break;
  }
  case "update_session" :
  {
    gw_update_session($host_ip);
    break;
  }
  case "get_session_stats" :
  {
    gw_get_session_stats($host_ip);
    break;
  }
  case "get_ips" :
  {
    gw_get_ips(null);
    break;
  }
  default :
  {
    echo json_encode( array( "status" => "fail", "message" => "unknown command" ) );
  }
  exit();
}
?>
