<?php
/**
 * POST web API to very basic gateway backend.
 * Just adds and removes sessions.
 */
$FIFO_FILENAME = "/tmp/gw_fifo";
$action = ( empty( $_POST['action'] ) ? null : $_POST['action'] );

switch( $action )
{
  case "start_session" :
  {
    gw_start_session();
    break;
  }
  case "end_session" :
  {
    gw_end_session();
    break;
  }
  case "update_session" :
  {
    gw_update_session();
    break;
  }
}

function gw_start_session()
{
  $host_ip = $_SERVER['REMOTE_ADDR'];
  if( $host_ip )
  {
    file_put_contents( $FIFO_FILENAME, "add_host $host_ip" );
  }
  header( "Location: started.html" );
}

function gw_end_session()
{
  $host_ip = $_SERVER['REMOTE_ADDR'];
  if( $host_ip )
  {
    file_put_contents( $FIFO_FILENAME, "remove_host $host_ip" );
  }
  header( "Location: ended.html" );
}

/** Update timeout. Timeout not actually implemented. */
function gw_update_session()
{
  $host_ip = $_SERVER['REMOTE_ADDR'];
  if( $host_ip )
  {
    file_put_contents( $FIFO_FILENAME, "add_host $host_ip" );
  }
}
