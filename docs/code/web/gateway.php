<?php

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
  case "get_quota" :
  {
    gw_get_quota();
    break;
  }
  case "get_bw" :
  {
    gw_get_bw();
    break;
  }
  default :
  {
    gw_none();
  }
}


function gw_start_session()
{
  $host_ip = $_SERVER['REMOTE_ADDR'];
  if( $host_ip )
  {
    file_put_contents( $FIFO_FILENAME, "add_host $host_ip" );
  }
}


function gw_end_session()
{
  $host_ip = $_SERVER['REMOTE_ADDR'];
  if( $host_ip )
  {
    file_put_contents( $FIFO_FILENAME, "remove_host $host_ip" );
  }
}


/**
 * Update timeout. Not implemented.
 */
function gw_update_session()
{
  $host_ip = $_SERVER['REMOTE_ADDR'];
  if( $host_ip )
  {
    file_put_contents( $FIFO_FILENAME, "add_host $host_ip" );
  }
}


/**
 * Not implemented.
 */
function gw_get_quota()
{}


/**
 * Not implemented.
 */
function gw_get_bw()
{}
