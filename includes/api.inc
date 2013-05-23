<?php // 
// The following code for the server is optional if the callbacks already exist.
// A server may implement methods associated to callbacks like node_load(),
// variable_get() or any other existing function (php functions as well).
//
// If the callbacks associated to the methods don't exist they must be
// created. This implementation requires two specific callbacks:
// - _xmlrpc_example_server_add()
// - _xmlrpc_example_server_subtract()
//
//
/**
 * This is the callback for the xmlrpc_example.add method.
 *
 * Sum the two arguments and return value or an error if the result is out of
 * the configured limits.
 *
 * @param $num1
 *   The first number to be summed.
 * @param $num2
 *   The second number to be summed.
 *
 * @return
 *   The sum of the arguments, or error if it is not in server defined bounds.
 *
 * @see xmlrpc_error()
 */
/*function _patterns_server_get_patterns_list($limit) {
  //return xmlrpc_error(10002, t('Result is below the lower limit defined by the server (@min).', array('@min' => $min)));

  //@TO-DO: Perform the real query
  $patterns_list = array('uuid'=> '12346', 'desc' => 'blah blah blah', 'url' => '/patatin/patatan');
  return $patterns_list;
}*/
