/**
 * If session exist start is successful.
 */
function start_session(ip_in)
{
  
  console.log( "start_session " + typeof(ip_in) );
  var settings = {};
  ip_in = ( ip_in ? ip_in : 0 );
  settings.type = "POST";
  settings.dataType = "json";
  settings.url = "gw_endpoint.php";
  settings.data = { action:"start_session", ip:ip_in };
  settings.error = function()
  {
    jQuery(document).trigger("start_session_cb",{status:"fail",msg:"comms err"});
    jQuery(document).trigger("comms_err");
  }
  settings.success = function(result)
  {
    jQuery(document).trigger("start_session_cb",result);
  }
  jQuery.ajax( settings );
}

function update_session(ip_in)
{
  console.log( "update_session " + ip_in );
  var settings = {};
  ip_in = ( ip_in ? ip_in : 0 );
  settings.type = "POST";
  settings.dataType = "json";
  settings.url = "gw_endpoint.php";
  settings.data = { action:"update_session", ip:ip_in };
  settings.error = function()
  {
    jQuery(document).trigger("update_session_cb",{status:"fail",msg:"comms err"});
    jQuery(document).trigger("comms_err");
  }
  settings.success = function(result)
  {
    jQuery(document).trigger("update_session_cb",result);
  }
  jQuery.ajax( settings );
}

function pause_session(ip_in)
{
  console.log( "pause_session " + ip_in );
  console.log( arguments );
  var settings = {};
  ip_in = ( ip_in ? ip_in : 0 );
  settings.type = "POST";
  settings.dataType = "json";
  settings.url = "gw_endpoint.php";
  settings.data = { action:"pause_session", ip:ip_in };
  settings.error = function()
  {
    jQuery(document).trigger("pause_session_cb",{status:"fail",msg:"comms err"});
    jQuery(document).trigger("comms_err");
  }
  settings.success = function(result)
  {
    jQuery(document).trigger("pause_session_cb",result);
  }
  jQuery.ajax( settings );
}

function play_session(ip_in)
{
  console.log( "play_session", ip_in );
  var settings = {};
  ip_in = ( ip_in ? ip_in : 0 );
  settings.type = "POST";
  settings.dataType = "json";
  settings.url = "gw_endpoint.php";
  settings.data = { action:"play_session", ip:ip_in };
  settings.error = function()
  {
    jQuery(document).trigger("play_session_cb",{status:"fail",msg:"comms err"});
    jQuery(document).trigger("comms_err");
  }
  settings.success = function(result)
  {
    jQuery(document).trigger("play_session_cb",result);
  }
  jQuery.ajax( settings );
}

function end_session(ip_in)
{
  console.log( "end_session " + ip_in );
  var settings = {};
  ip_in = ( ip_in ? ip_in : 0 );
  settings.type = "POST";
  settings.dataType = "json";
  settings.url = "gw_endpoint.php";
  settings.data = { action:"end_session", ip:ip_in };
  settings.error = function()
  {
    jQuery(document).trigger("end_session_cb",{status:"fail",msg:"comms err"});
    jQuery(document).trigger("comms_err");
  }
  settings.success = function(result)
  {
    jQuery(document).trigger("end_session_cb",result);
  }
  jQuery.ajax( settings );
}   

function stats(ip_in)
{
  console.log( "stats " + ip_in );
  var settings = {};
  ip_in = ( ip_in ? ip_in : 0 );
  settings.type = "POST";
  settings.dataType = "json";
  settings.url = "gw_endpoint.php";
  settings.data = { action:"get_session_stats", ip:ip_in};
  settings.error = function()
  {
    jQuery(document).trigger("stats_cb",{status:"fail",msg:"comms err"});
    jQuery(document).trigger("comms_err");
  }
  settings.success = function(result)
  {
    jQuery(document).trigger("stats_cb",result);
  }
  jQuery.ajax( settings );
}

function ips()
{
  console.log( "ips " );
  var settings = {};
  settings.type = "POST";
  settings.dataType = "json";
  settings.url = "gw_endpoint.php";
  settings.data = { action:"get_ips"};
  settings.error = function()
  {
    jQuery(document).trigger("ips_cb",{status:"fail",msg:"comms err"});
    jQuery(document).trigger("comms_err");
  }
  settings.success = function(result)
  {
    jQuery(document).trigger("ips_cb",result);
  }
  jQuery.ajax( settings );
}

function state_code_to_string(code)
{
  var str = "unknown";
  switch( code )
  {
    case 0 :
    {
      str = "uninitialized";
      break;
    }
    case 1 :
    {
      str = "started";
      break;
    }
    case 2 :
    {
      str = "paused";
      break;
    }
    case 3 :
    {
      str = "ended";
      break;
    }
  }
  return  str;
}
