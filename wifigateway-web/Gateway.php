<?php
/**
 * Proxy API to gateway session backend. Hosts are identified by IP.
 * This is because this is what the web side has to identify the client.
 * The IP is mapped to a MAC address on the backend.
 * @todo this should not be encoding JSON.
 */
 
define("DBUS_NAME",  "com.epicmorsel.dbus");
define("DBUS_INTERFACE", "com.epicmorsel.dbus.gateway");
define("DBUS_OBJECT_PATH", "/com/epicmorsel/dbus/gateway");

class Gateway
{
  private $gw = null;
  private $bus = null;
  private static $stats = array(
    'IP',
    'MAC',
    'state',
    'bandwidth',
    'timeRemaining',
    'quotaRemaining',
    'activityTimeRemaining',
    'updateTimeRemaining',
    'pauseTimeRemaining',
    'endTimeRemaining',
    'sessionTime'
  );

  public function __construct()
  {
    $this->bus = new Dbus(Dbus::BUS_SYSTEM, true);
    $this->gw = $this->bus->createProxy(
      DBUS_NAME,        // connection name
      DBUS_OBJECT_PATH, // object
      DBUS_INTERFACE    // interface
   );
  }


  public function start_session($host_ip)
  {
    $return = array('status' => "ok");
    
    if($host_ip)
    {
      $ret = $this->gw->startSession($host_ip);
      if($ret != 0)
      {
        throw new GatewayException("could not create session with IP $host_ip", $ret);
      }
    }
    return $return;
  }


  public function pause_session($host_ip)
  {
    $return = array('status' => "ok");
    
    if($host_ip)
    {
      $ret = $this->gw->pauseSession($host_ip);
      if($ret != 0)
      {
        throw new GatewayException("could not pause session with IP $host_ip", $ret);
      }
    }
    return $return;
  }


  public function play_session($host_ip)
  {
    $return = array('status' => "ok");
    
    if($host_ip)
    {
      $ret = $this->gw->playSession($host_ip);
      if($ret != 0)
      {
        throw new GatewayException("could not play session with IP $host_ip", $ret);
      }
    }
    return $return;
  }


  public function end_session($host_ip)
  {
    $return = array('status' => "ok");
    $ret = $this->gw->endSession($host_ip);
    if($ret != 0)
    {
      throw new GatewayException("could not end session with IP $host_ip", $ret);
    }
    return $return;
  }


  /**
   * Update timeout. Not implemented.
   */
  public function update_session($host_ip)
  {
    $return = array('status' => "ok");
    $ret = $this->gw->updateSession($host_ip);
    if($ret != 0)
    {
      throw new GatewayException("could not update session with IP $host_ip", $ret);
    }
    return $return;
  }


  /**
   * For now no need to break this bundle up by index.
   * @todo There a way to pass a dictionary in DBus.
   */
  public function get_session_stats($host_ip)
  {
    $stats  = array();
    $raw_stats = $this->gw->getSession($host_ip)->getData();
    // Make into assoc.
    $stats = array_combine(self::$stats, $raw_stats);
    $return = array('status' => "ok", 'stats' => $stats);
    return $return;
  }


  public function get_ips()
  {
    $ips = $this->gw->getIPs()->getData();
    $return = array('status' => "ok", 'ips' => $ips);
    return $return;
  }


  public function set_bandwidth($ip, $bw)
  {
    $return = array('status' => "ok");

    $ret = $this->gw->setBandwidth($ip, $bw);
    if($ret != 0)
    {
      $return = array('status' => 'fail');
    }
    return $return;
  }


  public function set_quota($ip, $qa)
  {
    $return = array('status' => "ok");
    
    $ret = $this->gw->setQuota($ip, $qa);
    if($ret != 0)
    {
      $return = array('status' => 'fail');
    }
    return $return;
  }

  
  public function set_time($ip, $qa)
  {
    $return = array('status' => "ok");
    
    $ret = $this->gw->setQuota($ip, $qa);
    if($ret != 0)
    {
      $return = array('status' => 'fail');
    }
    return $return;
  }
}

class GatewayException extends Exception {}
