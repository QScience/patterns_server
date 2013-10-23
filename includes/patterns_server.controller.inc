<?php
/**
 * @file
 */
module_load_include('inc', 'patterns_server', 'includes/history');

/**
 * customise entity controller class.
 */
class PatternsServerEntityController extends EntityAPIController {

  /**
   * Implements EntityAPIControllerInterface.
   *
   * @param $transaction
   *   Optionally a DatabaseTransaction object to use. Allows overrides to pass
   *   in their transaction object.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {

    $entity->created = REQUEST_TIME;

    $a = parent::save($entity, $transaction);

    $pid = $entity->pid;
    $child_uuuid = $entity->uuuid;
    $pattern = $entity->pattern;
    
    if (isset($pattern['info']['parent']) && count($pattern['info']['parent']) > 0) {
      $p_parents = $pattern['info']['parent'];
      foreach ($p_parents as $p_parent) {
        $uuuid = $p_parent['uuuid'];
        $parent_core = array(
          'action' => $p_parent['action'],
          'date' => $p_parent['date'],
          //the next five are needed to be validated.
          'host' => $p_parent['host'],
          'd2did' => $p_parent['d2d_id'],
          'title' => $p_parent['title'],
          'category' => $p_parent['category'],
          'author' => $p_parent['author'],
        );
        //if the parent is in the db. actually it will need a validation.
        //TODO validate the $p_parent and the fields in db. 
        //if they are not same, there should be an error. means somebody change the parents section 
        //in pattern file manually.
        //but for now, I just assume they are same.

        $exist = patterns_server_check_whether_uuuid_exist($uuuid);
        if ($exist != FALSE) {
          if ($exist->host != $parent_core['host'] ||
            $exist->d2did !=  $parent_core['d2did'] ||
            $exist->title !=  $parent_core['title'] ||
            $exist->category != $parent_core['category'] ||
            $exist->author != $parent_core['author'] ) {
              drupal_set_message(t('The parent section of this pattern has been damaged. So the parent section is ignored'), 'error');
              return $a;
            }
          else {
            $parent_core['user'] = $exist->uploader;
            $parent_core['pid'] = $exist->pid;
          }
        }
        else {
          $parent_core['user'] = -1;
          $parent_core['pid'] = -1;
        }

        $child_core = array();
        $child_core['host'] = $entity->host;
        $child_core['d2did'] = $entity->d2did;
        $child_core['title'] = $entity->title;
        $child_core['category'] = $entity->category;
        $child_core['author'] = $entity->author;
        $child_core['pid'] = $entity->pid;
        $child_core['user'] = $entity->uploader;

        patterns_server_insert_pair($uuuid, $child_uuuid, $parent_core);
        patterns_server_update_index($uuuid, $child_uuuid, $p_parent['action'], 1, $parent_core, $child_core);
        patterns_server_update_index($child_uuuid, $uuuid, $p_parent['action'], 0, $parent_core, $child_core);
      }
    }

    return $a;
  }

   /**
   * Implements EntityAPIControllerInterface.
   */
  public function view($entities, $view_mode = 'full', $langcode = NULL, $page = NULL) {
    // For Field API and entity_prepare_view, the entities have to be keyed by
    // (numeric) id.
    $entities = entity_key_array_by_property($entities, $this->idKey);
    if (!empty($this->entityInfo['fieldable'])) {
      field_attach_prepare_view($this->entityType, $entities, $view_mode);
    }
    entity_prepare_view($this->entityType, $entities);
    $langcode = isset($langcode) ? $langcode : $GLOBALS['language_content']->language;

    $view = array();
    foreach ($entities as $entity) {
      $build = entity_build_content($this->entityType, $entity, $view_mode, $langcode);


      foreach ($build as $key => $value) {
        if (isset($value['#field_type']) and $value['#field_type'] == 'taxonomy_term_reference') {
          if (isset($value['#items'])) {
            foreach ($value['#items'] as $_key => $_value) {
              $tid = $_value['tid'];
              $tags_name = $value['#field_name'];
              $build[$key][$_key]['#href'] = "pattern/tags/$tags_name/$tid";
            }
          }
        }
      }

      $build += array(
        // If the entity type provides an implementation, use this instead the
        // generic one.
        // @see template_preprocess_entity()
        '#theme' => 'entity',
        '#entity_type' => $this->entityType,
        '#entity' => $entity,
        '#view_mode' => $view_mode,
        '#language' => $langcode,
        '#page' => $page,
      );
      // Allow modules to modify the structured entity.
      drupal_alter(array($this->entityType . '_view', 'entity_view'), $build, $this->entityType);
      $key = isset($entity->{$this->idKey}) ? $entity->{$this->idKey} : NULL;
      $view[$this->entityType][$key] = $build;
    }
    return $view;
  }


  /**
   * Implements EntityAPIControllerInterface.
   *
   * @param $transaction
   *   Optionally a DatabaseTransaction object to use. Allows overrides to pass
   *   in their transaction object.
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    $entities = $ids ? $this->load($ids) : FALSE;
    if (!$entities) {
      // Do nothing, in case invalid or no ids have been passed.
      return;
    }
    // This transaction causes troubles on MySQL, see
    // http://drupal.org/node/1007830. So we deactivate this by default until
    // is shipped in a point release.
    // $transaction = isset($transaction) ? $transaction : db_transaction();

    try {
      $ids = array_keys($entities);

      db_delete($this->entityInfo['base table'])
        ->condition($this->idKey, $ids, 'IN')
        ->execute();

      if (isset($this->revisionTable)) {
        db_delete($this->revisionTable)
          ->condition($this->idKey, $ids, 'IN')
          ->execute();
      }
      // Reset the cache as soon as the changes have been applied.
      $this->resetCache($ids);

      foreach ($entities as $id => $entity) {
        $this->invoke('delete', $entity);

        //add by rui.
        patterns_server_delete_pairs($entity->uuuid);
        patterns_server_delete_index($entity->uuuid);

      }
      // Ignore slave server temporarily.
      db_ignore_slave();
    }
    catch (Exception $e) {
      if (isset($transaction)) {
        $transaction->rollback();
      }
      watchdog_exception($this->entityType, $e);
      throw $e;
    }
  }

}
