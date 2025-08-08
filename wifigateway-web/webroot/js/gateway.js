/**
 * If session exist start is successful.
 */

function gw_call(method, params)
{
  console.log("call::" + method, params);
  
  if(!method)
  {
    throw Error("At least give me a method");
  }
  if(typeof(params) == "undefined")
  {
    params = {};
  }
  var callback = method + "_cb";
  var ajax_call = {
    type: "POST",
    dataType: "json",
    url:"gw_endpoint.php",
    data: {method:method, params:params}
  }

  ajax_call.error = function(jqXHR, textStatus, errorThrown)
  {
    jQuery(document).trigger(callback, {result:null, error:{message:textStatus, code:-32769}});
    jQuery(document).trigger("call_comm_error", {result:null, error:{message:textStatus, code:-32769}});
  }
  ajax_call.success = function(result)
  {
    jQuery(document).trigger(callback, result);
  }
  jQuery.ajax(ajax_call);
}


function state_code_to_string(code)
{
  ret = "unknown";
  var map = {
    0: "uninitialized",
    1: "started",
    2: "paused",
    3: "exhausted",
    4: "ended"
  }
  if(typeof map[code] != "undefined")
  {
    ret = map[code];
  }
  return ret;
}
