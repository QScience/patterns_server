<?php
/**
 * @file
 */

/**
 * Implements entity class.
 */
class Patterns_Server_Entity extends Entity {

  /**
   * Override this in order to implement a custom default URI and specify
   * 'entity_class_uri' as 'uri callback' hook_entity_info().
   */
  protected function defaultUri() {
    return array('path' => 'pattern/' . $this->identifier());
  }


}
