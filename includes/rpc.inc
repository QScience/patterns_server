<?php
/**
 function _patterns_server_pattern_category_page($category_name) {
 drupal_add_css(drupal_get_path('module', 'patterns_server') . '/patterns_server.css');
 $result = patterns_server_get_category_patterns_from_db($category_name);
 $page = theme('patterns_server_one_pattern_info',array(
 'patterns' => $result,
 ));

 */

/**
 * Implements hook_xmlrpc().
 *
 * Provides Drupal with an array to map XML-RPC callbacks to existing functions.
 * These functions may be defined in other modules. The example implementation
 * defines specific functions for the example services.
 *
 * Note: Drupal's built-in XML-RPC server already includes several methods by
 * default:
 *
 * Service dicovery methods:
 * - system.listMethods: return a list of the methods the server has, by name.
 * - system.methodSignature: return a description of the argument format a
 * - system.methodHelp: returns a text description of a particular method.
 *   particular method expects.
 *
 * Other:
 * - system.multicall: perform several method calls in a single xmlrpc request.
 * - system.getCapabilities: determine if a given capability is supported.
 *
 * The methods defined by hook_xmlrpc() will be added to those provided by
 * default by Drupal's XML-RPC server.
 *
 * @see hook_xmlrpc()
 */
function patterns_server_xmlrpc() {
  $methods[] = array(
      // First argument is the method name.
      'patterns_server.get_patterns_list',
      // Callback to execute when this method is requested.
      '_patterns_server_get_patterns_list',
      // An array defines the types of output and input values for this method.
      array(
          // The first value is the return type, an array in this case.
          'struct',
          // First operand is an integer (max number of patterns)
          'int',
      ),
      // Include a little description that is shown when XML-RPC server is
      // requested for the implemented methods list.
      // Method description.
      t('Returns the list of patterns available in this server. The argument is the max number of patterns to receive.'),
  );
  return $methods;
}

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
function _patterns_server_get_patterns_list($limit) {
  //return xmlrpc_error(10002, t('Result is below the lower limit defined by the server (@min).', array('@min' => $min)));

  //@TO-DO: Perform the real query
  $patterns_list = array('uuid'=> '12346', 'desc' => 'blah blah blah', 'url' => '/patatin/patatan');
  return $patterns_list;
}
