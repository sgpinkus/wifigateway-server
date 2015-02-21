#!/usr/bin/php
<?php
/**
 * Create a proxy to a dbus object and calls methods on them interactively.
 * No support for signals.
 * ./shell.php org.freedesktop.ModemManager org.freedesktop.ModemManager /org/freedesktop/ModemManager
 * ./shell.php com.epicmorsel.dbus com.epicmorsel.dbus.gateway /com/epicmorsel/dbus/gateway
 * ./shell.php org.PulseAudio1 org.PulseAudio1.ServerLookup1 /org/pulsaudio/server_lookup1
 * @todo nested argument support.
 */ 
 
require_once("DbusIntrospectableObject.php");

if($argc != 5 || !in_array($argv[1], array("system", "session"))) {
  print usage();
  exit(1);
}

function usage() {
  global $argv;
  return "Usage: {$argv[0]} (system|session) <busname> <interface> <object>\n";
}
$DbusAddress = $argv[1];
$DbusName = $argv[2];
$DbusInterface = $argv[3];
$DbusObjectPath = $argv[4];

try
{
  $dbus = ($DbusAddress == "system") ? new Dbus(Dbus::BUS_SYSTEM, true) : new Dbus(Dbus::BUS_SYSTEM, true);
  $dbus_object = $dbus->createProxy($DbusName, $DbusObjectPath, $DbusInterface);
  $dbus_i_obj = new DbusIntrospectableObject($dbus, $DbusName, $DbusObjectPath, $DbusInterface);
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
      //Reflection::export(new ReflectionClass($result));
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
 * Read dbus type. Broken for non primitives - and signed types.
 * @param $n array of dbus type specifers.
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
      $fmt .= "%" . dbus_to_printf($arg['type']."") . " ";
    }
    $fmt_str = trim($fmt_str);
    $fmt = trim($fmt);
    printf("Enter args (%s) (%s):\n", $fmt_str, $fmt);
    $args = fscanf(STDIN, $fmt);
  }
  return $args;
}


/**
 * Clearly inaccurate.
 */
function dbus_to_printf($s)
{
  $map = array(
    "y" => "hhu",
    "n" => "hi",
    "q" => "hu",
    "i" => "i",
    "u" => "u",
    "x" => "lli",
    "t" => "llu",
    "d" => "g",
    "s" => "s",
    "o" => "s",
    "b" => "s",
    "a" => "s",
    "g" => "s"
  );
  return empty($map[$s]) ? "s" : $map[$s];
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

