<?php
/**
 * JSON API to gateway session backend. Most calls are parametized by IP,
 * so they can be identified on client side if needed.
 */
 
define( "DBUS_NAME",  "com.epicmorsel.dbus" );
define( "DBUS_INTERFACE", "com.epicmorsel.dbus.gateway" );
define( "DBUS_OBJECT_PATH", "/com/epicmorsel/dbus/gateway" );
$gw = null;
$bus = null;


function init()
{
  global $bus;
  global $gw;
  
  if( ! $bus )
  {
    // Init dbus service endpoint.
    try
    {
      $bus = new Dbus( Dbus::BUS_SYSTEM, true );
      $gw = $bus->createProxy(
          DBUS_NAME,        // connection name
          DBUS_OBJECT_PATH, // object
          DBUS_INTERFACE    // interface
      );
    }
    catch( Exception $e )
    {
      echo json_encode( array( "status" => "fail", "message" => "a server error occured", 'ip' => $host_ip ) );
      exit();
    }
  }
}


function gw_start_session($host_ip)
{
  global $gw;
  init($host_ip);
  $return = array( 'status' => "ok", 'ip' => $host_ip );
  
  if( $host_ip )
  {
    $ret = $gw->newSession( $host_ip );
    if( $ret != 0 )
    {
      $return = array( 'status' => 'fail', 'msg' => "could not create session with IP $host_ip", 'ip' => $host_ip );
    }
  }
  echo json_encode( $return );
}


function gw_pause_session($host_ip)
{
  global $gw;
  init($host_ip);
  $return = array( 'status' => "ok", 'ip' => $host_ip );
  
  if( $host_ip )
  {
    $ret = $gw->pauseSession( $host_ip );
    if( $ret != 0 )
    {
      $return = array( 'status' => 'fail', 'msg' => "could not pause session with IP $host_ip", 'ip' => $host_ip );
    }
  }
  echo json_encode( $return );
}


function gw_play_session($host_ip)
{
  global $gw;
  init($host_ip);
  $return = array( 'status' => "ok", 'ip' => $host_ip );
  
  if( $host_ip )
  {
    $ret = $gw->playSession( $host_ip );
    if( $ret != 0 )
    {
      $return = array( 'status' => 'fail', 'msg' => "could not play session with IP $host_ip", 'ip' => $host_ip );
    }
  }
  echo json_encode( $return );
}


function gw_end_session($host_ip)
{
  global $gw;
  init($host_ip);
  $return = array( 'status' => "ok", 'ip' => $host_ip );
  $ret = $gw->endSession( $host_ip );
  if( $ret != 0 )
  {
    $return = array( 'status' => 'fail', 'msg' => "could not end session with IP $host_ip", 'ip' => $host_ip );
  }
  echo json_encode( $return );
}


/**
 * Update timeout. Not implemented.
 */
function gw_update_session($host_ip)
{
  global $gw;
  init($host_ip);
  $return = array( 'status' => "ok", 'ip' => $host_ip );
  $ret = $gw->updateSession( $host_ip );
  if( $ret != 0 )
  {
    $return = array( 'status' => 'fail', 'msg' => "could not update session with IP $host_ip", 'ip' => $host_ip );
  }
  echo json_encode( $return );
}


/**
 * For now no need to break this bundle up.
 * Argh this struct def has to be maintained:
 * @todo use a map.
 */
function gw_get_session_stats($host_ip)
{
  global $gw;
  init($host_ip);
  
  $stats  = array();
  $raw_stats = $gw->getStats( $host_ip )->getData();
  // Make into assoc. 
  $stats['IP'] = $raw_stats[0];
  $stats['MAC'] = $raw_stats[1];
  $stats['state'] = $raw_stats[2];
  $stats['time'] = $raw_stats[3];
  $stats['quota'] = $raw_stats[4];
  $stats['bandwidth'] = $raw_stats[5];
  $stats['timeRemaining'] = $raw_stats[6];
  $stats['quotaRemaining'] = $raw_stats[7];
  $stats['activityTimeRemaining'] = $raw_stats[8];
  $stats['updateTimeRemaining'] = $raw_stats[9];
  $stats['pauseTimeRemaining'] = $raw_stats[10];
  $stats['endTimeRemaining'] = $raw_stats[11];
  $stats['sessionTime'] = $raw_stats[12];
  $return = array( 'status' => "ok", 'stats' => $stats, 'ip' => $host_ip );
  echo json_encode( $return );
}


function gw_get_ips()
{
  global $gw;
  init(null);
  $ips = $gw->getIPs()->getData();
  $return = array( 'status' => "ok", 'ips' => $ips );
  echo json_encode( $return );
}


function gw_set_bandwidth( $ip, $bw )
{
  global $gw;
  init($host_ip);
  $return = array( 'status' => "ok", 'ip' => $host_ip );

  $ret = $gw->setBandwidth( $ip, $bw );
  if( $ret != 0 )
  {
    $return = array( 'status' => 'fail', 'ip' => $host_ip );
  }
  echo json_encode( $return );
}


function gw_set_quota( $ip, $qa )
{
  global $gw;
  init($host_ip);
  $return = array( 'status' => "ok", 'ip' => $host_ip );
  
  $ret = $gw->setQuota( $ip, $qa );
  if( $ret != 0 )
  {
    $return = array( 'status' => 'fail', 'ip' => $host_ip );
  }
  echo json_encode( $return );
}
