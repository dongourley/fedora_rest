<?php
// $Id$

/**
 * @file
 *
 * Administration page callbacks for the fedora_rest module
 */

define('FEDORA_REST_DEFAULT_COLLECTION', 'drupal:ImportCollection');
define('FEDORA_REST_DEFAULT_MODEL', 'drupal:UnspecifiedModel');
define('FEDORA_REST_DEFAULT_PREDICATE', 'rel:hasPart');
define('FEDORA_REST_DEMO_COLLECTION', 'info:fedora/demo:SmileyStuff');
define('FEDORA_REST_DEMO_PREDICATE', 'info:fedora/fedora-system:def/relations-external#isMemberOf');


/**
 * Form builder. Configure fedora_rest module.
 *
 * @ingroup forms
 * @see system_settings_form().
 */
function fedora_rest_admin_settings() {

    // create arrays for selecting options
    $users = array ();
    $result = db_query("SELECT name FROM {users} WHERE status>0 ORDER BY name");
    while ($u = db_fetch_object($result)) {
        $users[$u->name] = $u->name;
    }
    $vocabs = array ();
    if (module_exists('taxonomy')) {
        foreach(taxonomy_get_vocabularies( ) as $v) {
            $vocabs[$v->name] = $v->name;
        }
    }

    // IMPORT OPTIONS: for importing batches of digital objects
    $form['fedora_rest_import'] = array(
        '#type' => 'fieldset',
        '#title' => t('Import options'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#weight' => -10,
        '#description' => t('Import settings from the last import are saved. You can also set import options here without importing to use them with the CLI import program.'),
    );
    $form['fedora_rest_import']['fedora_rest_import_flag'] = array(
        '#type' => 'checkbox',
        '#title' => t('Harvest metadata from Fedora Repository'),
        '#default_value' => FALSE,
        '#description' => t('Select this box to synchronize with your repository. Make sure you have correctly set all the parameters before clicking the "Save Configuration" button to start the import. NB: The import may take 15-30 seconds per 100 objects imported; for large collections you may want to use the CLI import program.'),
        '#weight' => -10,
    );

    // COLLECTION PARAMETERS: apply to a batch and stored in collection record
    $form['fedora_rest_import']['fedora_rest_batch'] = array(
        '#type' => 'fieldset',
        '#title' => t('Collection settings'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#description' => t('These parameters describe how a batch of digital objects is imported into Drupal.'),
        '#weight' => -5,
    );
    $form['fedora_rest_import']['fedora_rest_batch']['fedora_rest_collection_arg'] = array(
        '#type' => 'textfield',
        '#title' => t('Collection URI'),
        '#required' => TRUE,
        '#default_value' => variable_get( 'fedora_rest_collection_arg',
                                            FEDORA_REST_DEFAULT_COLLECTION ),
        '#maxlength' => 255,
        '#description' => t('Enter a Uniform Resource Id for the set of objects being imported; this can be the PID of the collection in the repository or an identifier that you assign for the part of a collection that is imported with these collection settings.'),
        '#weight' => -10,
    );
    $form['fedora_rest_import']['fedora_rest_batch']['fedora_rest_server_arg'] = array(
        '#type' => 'textfield',
        '#title' => t('Repository URL'),
        '#required' => TRUE,
        '#default_value' => variable_get('fedora_rest_server_arg',
                                        'http://localhost:8080/fedora'),
        '#maxlength' => 255,
        '#description' => t('Enter the URL of the Fedora repository REST API.'),
        '#weight' => -9,
    );
    $form['fedora_rest_import']['fedora_rest_batch']['fedora_rest_version_arg'] = array(
        '#type' => 'select',
        '#options' => drupal_map_assoc(array('3.0','3.1','3.2','3.3','3.4')),
        '#multiple' => FALSE,
        '#title' => t('REST API version'),
        '#default_value' => variable_get('fedora_rest_version_arg', '3.3'),
        '#description' => t('Select the Fedora repository version; this handles differences in the REST API that were introduced with version 3.2'),
        '#weight' => -8,
    );
    $form['fedora_rest_import']['fedora_rest_batch']['fedora_rest_predicate_arg'] = array(
        '#type' => 'textfield',
        '#title' => t('SPO query predicate'),
        '#required' => TRUE,
        '#default_value' => variable_get('fedora_rest_predicate_arg',
                                        FEDORA_REST_DEMO_PREDICATE),
        '#maxlength' => 255,
        '#description' => t('Enter the URI of the Predicate used in the Resource Index to relate digital objects you want to import to the Object that you enter in the next field; these fields will be used to create an SPO query of the form "* &lt;Predicate&gt; &lt;Object&gt;" to select digital objects.'),
        '#weight' => -7,
    );
    $form['fedora_rest_import']['fedora_rest_batch']['fedora_rest_object_arg'] = array(
        '#type' => 'textfield',
        '#title' => t('SPO query object'),
        '#required' => TRUE,
        '#default_value' => variable_get('fedora_rest_object_arg',
                                        FEDORA_REST_DEMO_COLLECTION),
        '#maxlength' => 255,
        '#description' => t('Enter the URI of, or String representing, the Object used in the Resource Index that digital objects you want to import are related to by the Predicate that you enter in the previous field; these fields will be used to create an SPO query of the form "* &lt;Predicate&gt; &lt;Object&gt;" to select digital objects.'),
        '#weight' => -6,
    );
    $form['fedora_rest_import']['fedora_rest_batch']['fedora_rest_nt_arg'] = array(
        '#type' => 'select',
        '#title' => t('Drupal node type'),
        '#options' => node_get_types('names'),
        '#multiple' => FALSE,
        '#default_value' => variable_get('fedora_rest_nt_arg', 'fedora_object'),
        '#description' => t('Enter the Drupal node type that imported objects will be assigned to.'),
        '#weight' => -5,
    );
    $form['fedora_rest_import']['fedora_rest_batch']['fedora_rest_owner_arg'] = array(
        '#type' => 'select',
        '#title' => t('Drupal node owner'),
        '#options' => $users,
        '#default_value' => variable_get('fedora_rest_owner_arg', 'admin'),
        '#description' => t('Select the user to mark as the author for each NEW node that is created (does not affect previously created nodes).'),
        '#weight' => -4,
    );
    if (module_exists('taxonomy') and ! empty($vocabs)) {
        $form['fedora_rest_import']['fedora_rest_batch']['fedora_rest_clear_vocab'] = array(
            '#type' => 'select',
            '#title' => t('Delete vocabulary terms'),
            '#options' => $vocabs,
            '#multiple' => TRUE,
            //'#size' => 10,
            '#default_value' => array(),
            '#description' => t('Select vocabularies to clear before importing. WARNING: Only clear vocabularies that will be rebuilt by the import!'),
            '#weight' => 0,
        );
    }
    $form['fedora_rest_import']['fedora_rest_batch']['fedora_rest_delete_arg'] = array(
        '#type' => 'checkbox',
        '#title' => t('Delete nodes before importing'),
        '#default_value' => FALSE,
        '#description' => t("Select this to delete all nodes that are in this set (ie, have been assigned the same Collection URI) before importing. Don't do this if you have anything that won't be regenerated by the import, like files or CCK fields, attached to the existing nodes."),
        '#weight' => 10,
    );

    // NODE PARAMETERS: apply to a Drupal node, used on node edit/create forms
    //      (fedora_rest_node_fields() is defined in fedora_rest.module)
    $form['fedora_rest_import']['fedora_rest_node'] = fedora_rest_node_fields($vocabs);

    // DEVELOPMENT OPTIONS: for creating custom modules
    $form['fedora_rest_devel'] = array(
        '#type' => 'fieldset',
        '#title' => t('Development options'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#description' => t('These options are useful when you are developing custom modules based on the Fedora REST API.'),
        '#weight' => 0,
    );
    $form['fedora_rest_devel']['fedora_rest_limit_arg'] = array(
        '#type' => 'select',
        '#title' => t('Import limit'),
        '#options' => drupal_map_assoc(array('no limit',1,2,4,8,16,32,64,128)),
        '#multiple' => FALSE,
        '#required' => TRUE,
        '#default_value' => 'no limit',
        '#description' => t('Use this to limit the number of objects being imported in order to test the import process.'),
        '#weight' => 0,
    );
    if (module_exists('taxonomy')) {
        $form['fedora_rest_devel']['fedora_rest_dict_flag'] = array(
            '#type' => 'checkbox',
            '#title' => t('Generate the vocabulary dictionaries.'),
            '#default_value' => TRUE,
            '#description' => t('Enable this option to generate the vocabulary dictionary that maps names to vids for each vocabulary.'),
            '#weight' => 1,
        );
    }
    $form['fedora_rest_devel']['fedora_rest_trace_flag'] = array(
        '#type' => 'checkbox',
        '#title' => 'Turn on verbose tracing for debugging.',
        '#default_value' => variable_get('fedora_rest_trace_flag', FALSE),
        '#description' => t('This option enables the display of trace messages for each time a fedora_rest function is called (and for your module if you instrument it like fedora_rest.module).'),
        '#weight' => 2,
    );

    $form['fedora_rest_import']['filter'] = filter_form( variable_get('fedora_rest_format', 0));

    $form['#submit'][] = 'fedora_rest_admin_settings_submit';

    return system_settings_form($form);
}


/**
 * Form handler.
 *
 */
function fedora_rest_admin_settings_submit($form, &$form_state) {

    /**
     * Note: form defined with fieldsets but $form['#tree'] was not set to TRUE
     * so the values here are flattened, not hierarchical under fieldsets
     * see Pro Drupal Development pg 234-235
     */

    /**
     * content settings
     */
    $dictflag = $form_state['values']['fedora_rest_dict_flag'];
    if ($dictflag) {
        $vdictionary = array();
        $fliptionary = array();
        foreach(taxonomy_get_vocabularies( ) as $vocab) {
            $vdictionary[$vocab->vid]=$vocab->name;
            $fliptionary[$vocab->name]=$vocab->vid;
        };
        variable_set('fedora_rest_vdictionary', $vdictionary);
        variable_set('fedora_rest_fliptionary', $fliptionary);
        drupal_set_message(t("Vocabulary dictionaries are in 'fedora_rest_vdictionary' (vid=>name) and 'fedora_rest_fliptionary' (name=>vid)"));
    }
    $vocabs = $form_state['values']['fedora_rest_clear_vocab'];
    if (is_array($vocabs)) {
        foreach ($vocabs as $key => $vname) {
            $vd = variable_get('fedora_rest_fliptionary', array());
            $vid= $vd[$vname];
            fedora_rest_delete_terms($vid);
            drupal_set_message(t("All terms in vocabulary $vname ($vid) have been deleted."));
        }
    }

    $format = $form_state['values']['format'];
    variable_set('fedora_rest_format', $format);

    /**
     * import settings
     */
    $importflag = $form_state['values']['fedora_rest_import_flag'];
    if ($importflag) {
        // these options are accessed by hooks in fedora_rest.module
        variable_set('fedora_rest_xmlds_arg',
                     $form_state['values']['fedora_rest_xmlds_arg']);
        variable_set('fedora_rest_depth',
                     $form_state['values']['fedora_rest_depth']);
        variable_set('fedora_rest_tag_parents_flag',
                     $form_state['values']['fedora_rest_tag_parents_flag']);
        variable_set('fedora_rest_has_part_arg',
                     $form_state['values']['fedora_rest_has_part_arg']);
        if (module_exists('taxonomy')) {
            foreach(taxonomy_get_vocabularies( ) as $v) {
                $varg = 'fedora_rest_' . str_replace(' ','_',$v->name) . '_arg';
                if (isset($form_state['values'][$varg])) {
                    variable_set($varg, $form_state['values'][$varg]);
                }
            }
        }

        // these options are passed to the import_items method
        $collection = $form_state['values']['fedora_rest_collection_arg'];
        $server = $form_state['values']['fedora_rest_server_arg'];
        $version = $form_state['values']['fedora_rest_version_arg'];
        $predicate = $form_state['values']['fedora_rest_predicate_arg'];
        $object = $form_state['values']['fedora_rest_object_arg'];
        $ntype = $form_state['values']['fedora_rest_nt_arg'];
        $nuser = $form_state['values']['fedora_rest_owner_arg'];
        $urow = db_fetch_object(db_query("SELECT uid FROM {users} WHERE name='%s'",check_plain($nuser)));
        if (is_object($urow)) {
            $nuid = $urow->uid;
        } else {
            drupal_set_message(t("WARNING: user '$nuser' not found; nodes will be owned by 'admin'."), 'warning');
            $nuid = 1;
        }
        $delete = $form_state['values']['fedora_rest_delete_arg'];
        $limit = $form_state['values']['fedora_rest_limit_arg'];
        if ($limit == 'no limit') $limit = 0;

        $rtn = fedora_rest_import_items($server,$collection,$predicate,$object,
                                $ntype,$nuid,$delete,$limit,$format,$version);
        if ($rtn === FALSE) {
            drupal_set_message(t('Import aborted!'), 'error');
        }
        // import_items collected messages, now have to re-set them for display
        fedora_rest_reset_messages( );
    }
}


function fedora_rest_import_items(  $server, $collection, $spoPredicate,
                                    $spoObject, $nodeType, $nodeUID=1,
                                    $delete=FALSE, $resLimit=0, $format=0,
                                    $version='3.3')
{
    global $user;
    global $fedora_rest_import_messages;

    fedora_rest_trace("fedora_rest_import_items($server,$collection,$spoPredicate,$spoObject,$nodeType,$nodeUID,$delete,$resLimit,$format,$version)");

    $fedora_rest_import_messages = array();
    try {
        $client = new FedoraClient( $server, NULL, NULL, $version );
    } catch (Exception $e) {
        $exception = '<pre>'.print_r($e, true).'</pre>';
        drupal_set_message( t("Exception Object:: $exception\n"), 'error');
        fedora_rest_save_messages();
        return FALSE;
    }

    /***********************************************************************
     * Get or Create collection record in Drupal database
     */
    $coll = db_fetch_object( db_query("SELECT * FROM {fedora_rest_collection} WHERE uri='%s' AND server='%s' AND version='%s'", check_plain($collection),check_plain($server),check_plain($version)) );
    if (is_object($coll)) {
        $collectionID = $coll->id;
    } else {
        db_query("INSERT INTO {fedora_rest_collection} (uri,server,version) VALUES ('%s','%s','%s')", check_plain($collection), check_plain($server), check_plain($version));
        $collectionID = db_last_insert_id('fedora_rest_collection', 'id');
    }


    /***********************************************************************
     * Clear existing nodes if requested, otherwise node ids will be preserved
     */
    if ($delete) {
        if  ($user->uid == 1) {
            $count = fedora_rest_delete_nodes($collectionID);
            drupal_set_message(t("!cnt drupal nodes from collection $collectionID deleted prior to import", array('!cnt' => $count)),'status');
        } else {
            drupal_set_message(t("WARNING: you must run import as admin user to delete nodes, delete option ignored."),'warning');
        }
    }


    /***********************************************************************
     * find objects with the specified relationship in the resource index
     */
    $spoQuery = '* <' . $spoPredicate . '> <' . $spoObject . '>';
    fedora_rest_trace("SPO Query: $spoQuery (view source to see tags)");

    try {
        $results = $client->findTriples( $spoQuery, $resLimit );
    } catch (Exception $e) {
        $exception = '<pre>'.print_r($e, true).'</pre>';
        drupal_set_message( "Exception Object:: $exception\n", 'error');
        fedora_rest_save_messages();
        return FALSE;
    }

    if (strpos($spoPredicate, 'hasModel') === FALSE) {
        $contentModel = FEDORA_REST_DEFAULT_MODEL;
    } else {
        $contentModel = $spoObject;
    }

    /***********************************************************************
     * create a drupal node for each fedora object
     */
    $count = 0;
    foreach($results as $pidURI => $rdf) {
        $path = split('/', $pidURI);
        $pid = array_pop($path);

        $q = "pid=$pid";
        $request = array( title => 'true', query => $q );
        try{
            $response = $client->findObjects( $request );
        } catch (Exception $e) {
            $exception = '<pre>'.print_r($e, true).'</pre>';
            drupal_set_message( "Exception Object:: $exception\n", 'error');
            fedora_rest_save_messages();
            return FALSE;
        }
        $rcount = count($response);
        if ($rcount != 1) {
            drupal_set_message(t("Search for objects with !q returned !r!\n",
                                 array('!q'=>$q, '!r'=>$rcount)),
                               'error');
            fedora_rest_save_messages();
            return FALSE;
        }

        if ($fo = $response[$pid]) {
            $fo->model = $contentModel;
            $fo->type  = $nodeType;
            $fo->persistent_id = $pid;
            $fo->collection_id = $collectionID;
            $fo->uid = $nodeUID;
            $fo->fmt = $format;
            $status_msg = "importing Fedora object $pid => ";
            $nid = fedora_rest_make_node($fo);
            $status_msg.= "Drupal node $nid";
            drupal_set_message($status_msg, 'status');
            $count++;
            fedora_rest_save_messages();
        } else {
            drupal_set_message(t("Unexpected response from repository for pid=$pid\n",array(!pid=>$pid)),'error');
            fedora_rest_save_messages();
            return FALSE;
        }
    }

    $recount = fedora_rest_reorder_nodes($collectionID);

    drupal_set_message( t("!c drupal nodes imported", array(!c=>$count)),
                        'status' );
    fedora_rest_save_messages();
    return $count;
}


/**
 * Create a Drupal node for the Fedora object
 */
function fedora_rest_make_node($fedoraObject) {
    global $user;

    $node = new stdClass();
    // make sure fedoraObject SXE elements are cast to their string values
    $node->type = (string)$fedoraObject->type;
    $node->title = (string)$fedoraObject->title;
    $node->body = '';
    $node->import_model = (string)$fedoraObject->model;
    $node->persistent_id = (string)$fedoraObject->persistent_id;
    $node->collection_id = (string)$fedoraObject->collection_id;

    $node->created = time();
    $node->changed = $node->created;

    if ($fedoraObject->fmt) {
        // apply a default Input Format
        $node->format = $fedoraObject->fmt;
    }

    // use defaults configured in Drupal for this content type, rather than
    // these hard-coded values
    //$node->status = 0;
    //$node->promote = 0;
    //$node->sticky = 0;

    // set import options for hooks in fedora_rest.module
    $node->fedora_rest_xmlds_arg = variable_get('fedora_rest_xmlds_arg','');
    $node->fedora_rest_depth = variable_get('fedora_rest_depth',0);
    $node->fedora_rest_tag_parents_flag = variable_get('fedora_rest_tag_parents_flag',FALSE);
    $node->fedora_rest_has_part_arg = variable_get('fedora_rest_has_part_arg','rel:hasPart');
    if (module_exists('taxonomy')) {
        foreach(taxonomy_get_vocabularies( ) as $v) {
            $varg = 'fedora_rest_' . str_replace(' ','_',$v->name) . '_arg';
            if (isset($form_state['values'][$varg])) {
                $node->{$varg} = variable_get($varg, '');
            }
        }
    }

    // check whether node exists, if so keep same nid but replace object record
    $fo = db_fetch_object(db_query("SELECT * FROM {fedora_rest_node} WHERE persistent_id='%s'", check_plain($node->persistent_id)));
    if (is_object($fo)) {
        $node->nid = $fo->nid;
        // need the vid also to make sure node_revisions is updated
        $no = db_fetch_object(db_query("SELECT * FROM {node} WHERE nid=%d", $fo->nid));
        $node->vid = $no->vid;
    } else {
        // set ownership for new node
        $node->uid = $fedoraObject->uid;
    }

    node_save($node);

    return $node->nid;
}

/**
 * Delete all existing Drupal nodes that were created for this collection
 */
function fedora_rest_delete_nodes($collectionID) {

    // delete any nodes from this collection
    $count = 0;
    $result = db_query( "SELECT nid FROM {fedora_rest_node} WHERE collection_id = %d", $collectionID );

    while ($row = db_fetch_object($result)) {
        node_delete($row->nid);
        $count++;
    }

    return $count;
}

/**
 * Modify the create date for each node so taxonomy_selet_nodes will
 * get them in ascending title order when "order by node.created DESC"
 */
function fedora_rest_reorder_nodes($collectionID) {

    // get a base timestamp
    $createtime = time();
    // select nodes from this collection
    $count = 0;
    $result = db_query( "SELECT n.nid FROM {node} n INNER JOIN {fedora_rest_node} frn ON n.nid=frn.nid WHERE collection_id = %d ORDER BY n.title DESC", $collectionID );

    while ($row = db_fetch_object($result)) {
        db_query("UPDATE {node} SET created=%d, changed=%d WHERE nid = %d",
                    $createtime, $createtime, $row->nid);
        $createtime++;
        $count++;
    }

    return $count;
}

/**
 *
 *
 */
function fedora_rest_save_messages( ) {
    global $fedora_rest_import_messages;

    $mess = drupal_get_messages( );
    if (isset($mess['error'])) {
        foreach ($mess['error'] as $message) {
            $fedora_rest_import_messages[] = "ERROR: $message\n";
        }
    }
    if (isset($mess['warning'])) {
        foreach ($mess['warning'] as $message) {
            $fedora_rest_import_messages[] = "WARNING: $message\n";
        }
    }
    if (isset($mess['status'])) {
        foreach ($mess['status'] as $message) {
            $fedora_rest_import_messages[] = "$message\n";
        }
    }
    if (isset($mess['debug'])) {
        foreach ($mess['debug'] as $message) {
            $fedora_rest_import_messages[] = "$message\n";
        }
    }
}

/**
 *
 *
 */
function fedora_rest_reset_messages( ) {
    global $fedora_rest_import_messages;

    foreach ($fedora_rest_import_messages as $message) {
        if (0 === strpos($message, 'ERROR: ')) {
            drupal_set_message(substr($message,7), 'error');
        } elseif (0 === strpos($message, 'WARNING: ')) {
            drupal_set_message(substr($message,9), 'warning');
        } else {
            drupal_set_message($message, 'status');
        }
    }
}

