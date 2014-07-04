<?php
require_once("DbusIntrospectableObject.php");

#define("DBUS_NAME",  "com.epicmorsel.dbus");
#define("DBUS_INTERFACE", "com.epicmorsel.dbus.gateway");
#define("DBUS_OBJECT_PATH", "/com/epicmorsel/dbus/gateway");
#define("DBUS_NAME",  "org.PulseAudio1");
#define("DBUS_INTERFACE", "org.PulseAudio1.ServerLookup1");
#define("DBUS_OBJECT_PATH", "/org/pulsaudio/server_lookup1");
define("DBUS_NAME",  "com.epicmorsel.dbus");
define("DBUS_INTERFACE", "com.epicmorsel.dbus.gateway");
define("DBUS_OBJECT_PATH", "/com/epicmorsel/dbus/gateway");

try
{
  $dbus = new Dbus(Dbus::BUS_SYSTEM, true);
  $dbus_object = $dbus->createProxy(DBUS_NAME, DBUS_OBJECT_PATH, DBUS_INTERFACE);
  $dbus_i_obj = new DbusIntrospectableObject($dbus, DBUS_NAME, DBUS_OBJECT_PATH, DBUS_INTERFACE);
  $methods = $dbus_i_obj->_methods();
}
catch(Exception $e)
{
  print "ERROR: Could not init connection: '" . $e->getMessage() . "'\n";
  exit(1);
}

while(true)
{
  $method_names = array_keys($methods);
  $method_index = select($method_names, "Select the method you want to call");
  
  if(isset($method_index) && isset($methods[$method_names[$method_index]]))
  {
    $method_name = $method_names[$method_index];
    $method = $methods[$method_name];
    $args_scheme = $method->xpath(".//arg[@direction='in']");
    $args = read_args($args_scheme);
    $result = NULL;
    
    try 
    {
      $result = $dbus_object->__call($method_name, $args);
    }
    catch(Exception $e)
    {
      print "ERROR: " . $e->getMessage();
      continue;
    }
    
    print "Result:\n";
    // Result may be primitive, or structured type which is repd as DbusXXX object.
    if(is_object($result))
    {
      Reflection::export(new ReflectionClass($result));
      var_dump($result->getData());
    }
    else
    {
      var_dump($result);
    }
  }
  else
  {
    print "Try again\n";
  }
}


/**
 *
 */
function read_args($n)
{
  $args = array();
  if(sizeof($n))
  {
    $fmt_str = "";
    $fmt = "";
    foreach($n as $arg)
    {
      $fmt_str .= $arg['name'] . ":" . $arg['type'] . " ";
      $fmt .= "%" . $arg['type'] . " ";
    }
    $fmt_str = trim($fmt_str);
    $fmt = trim($fmt);
    $arg_fmt = str_repeat("%s ", sizeof($n));
    printf("Enter args (%s):\n", $fmt_str, $fmt);
    $args = fscanf(STDIN, $arg_fmt);
  }
  return $args;
}


/**
 * Select one or a number of options from a list specified as an array.
 * @param $options an array with a numbered list of options which will be printed and user select one.
 * @param $msg and optional msg to print before printing the list of options.
 * @param $loop if not empty(), on invalid seletion this is printed and the process loops.
 * @returns integer selection in range of options indexes, or null on no selection.
 */
function select(Array $options, $msg = "", $loop = false)
{
  $stdin = fopen("php://stdin", "r");
    
  print $msg.":\n";
  
  foreach($options as $i => $o)
  {
    print "$i) $o\n";
  }
  
  $response = trim(fread($stdin, 1024));
  
  while(((! ctype_digit($response)) || ($response > (sizeof($options) -1))) && $loop)
  {
    print $loop."\n";
    print $msg.":\n";
    foreach($options as $i => $o)
    {
      print "$i) $o\n";
    }
    $response = trim(fread($stdin, 1024));
  }
  
  fclose($stdin);
  
  if(ctype_digit($response))
  {
    return $response;
  }
  
  return null; 
}

