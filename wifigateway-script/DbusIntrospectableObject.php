<?php

/**
 * PHP-DBus doesnt expose Introspectable I.f. for you - or publish them for you either. Ideally it would implement PHP Reflector.
 * This class provides a simple wrapper on DBus introspection I.f. to native PHP, where an object is introspectable.
 * Note introspectable and in fact all freedesktop.org interfaces are optional and may not be implemented!
 */
class DbusIntrospectableObject
{
  private $connection_name;
  private $object_name;
  private $interface_name;
  private $proxy;
  private $intro_proxy;
  private $xml;
  private $cache = array();
  
  
  /**
   * Init. Unlike DbusObject fails if there is no actual dbus object present.
   */
  public function __construct(Dbus $dbus, $connection_name, $object_name, $interface_name)
  {
    $this->connection_name = $connection_name;
    $this->object_name =  $object_name;
    $this->interface_name = $interface_name;
    $this->proxy = $dbus->createProxy($connection_name, $object_name, $interface_name);
    $this->intro_proxy = $dbus->createProxy($connection_name, $object_name, "org.freedesktop.DBus.Introspectable");
    $this->xml = static::DbusGetSimpleXMLIntrospection($dbus, $connection_name, $object_name, $interface_name);
  }
  
  
  public function _methods()
  {
    if(!isset($this->cache['methods']))
    {
      $this->cache['methods'] = array();
      $sxml = $this->xml->node->xpath("/node/interface[@name='" . $this->interface_name . "']/method");
      foreach($sxml as $k => $ent)
      {
        $this->cache['methods'][$ent['name'].""] = $ent;
      }
    }
    return $this->cache['methods'];
  }
  
  
  public function _signals()
  {
    if(!isset($this->cache['signals']))
    {
      $this->cache['signals'] = array();
      $sxml = $this->cache['signals'] = $this->xml->node->xpath("/node/interface[@name='" . $this->interface_name . "']/signal");
      foreach($sxml as $k => $ent)
      {
        $this->cache['signals'][$ent['name'].""] = $ent;
      }
    }
    return $this->cache['signals'];
  }
  
  
  public function _methodNames()
  {
    if(!isset($this->cache['methodNames']))
    {
      $sxml = $this->xml->node->xpath("/node/interface[@name='" . $this->interface_name . "']/method/@name");
      $this->cache['methodNames'] = array_map(function($n){ return $n['name'] . ""; }, $sxml);
    }
    return $this->cache['methodNames'];
  }
  
  
  public function _signalNames()
  {
    if(!isset($this->cache['signalNames']))
    {
      $sxml = $this->xml->node->xpath("/node/interface[@name='" . $this->interface_name . "']/signal/@name");
      $this->cache['signalNames'] = array_map(function($n){ return $n['name'] . ""; }, $sxml);
    }
    return $this->cache['signalNames'];
  }
  
  
  public function _asXML()
  {
    return $this->xml->asXML();
  }


  public static function DbusGetSimpleXMLIntrospection(Dbus $dbus, $connection_name, $object_name, $interface_name)
  {
    $intro_proxy = $dbus->createProxy($connection_name, $object_name, "org.freedesktop.DBus.Introspectable");
    $xml_str = NULL;
    try
    {
      $xml_str = $intro_proxy->Introspect();
    }
    catch(Exception $e)
    {
      throw new DBusIntrospectionException("Object does not support introspection.");
    }
    $xml = new SimpleXMLElement($xml_str);
    return $xml;
  }
}


class DBusIntrospectionException extends Exception
{}

